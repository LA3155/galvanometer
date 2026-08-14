CDC / USBD 原理与当前工程说明
==============================

1. 当前 USB CDC 做了什么
-----------------------
本工程使用 GD32F30x 官方 USBD Device 栈实现 CDC ACM 虚拟串口。

电脑端看到的是一个 COM 口，例如 COM3；但它本质不是传统 UART。
它是 USB Device 枚举成功后，由 CDC ACM 类描述符告诉 Windows：
“我是一个类似串口的 USB 设备”。

数据路径：

PC 串口助手
  -> Windows USB CDC 驱动
  -> USB D+/D- 差分线
  -> GD32 USB Device 外设
  -> USBD_LP_CAN0_RX0_IRQHandler
  -> usbd_isr()
  -> usbd_enum.c / usbd_lld_int.c / usbd_transc.c
  -> cdc_acm_core.c
  -> cdc.c 封装函数
  -> cli_poll() 命令解析

所以 main.c 不需要直接处理 USB 包、端点、描述符、枚举事务。
这些由官方 USBD 库和 cdc_acm_core.c 解决。


2. 为什么 D+ 拉高后 PC 能识别
----------------------------
USB Full Speed 设备通过 D+ 上拉电阻告诉主机：
“这里接入了一个 Full Speed USB Device”。

对于 Full Speed：
- D+ 上拉表示 Full Speed 设备
- D- 上拉表示 Low Speed 设备

主机检测到 D+ 被拉高后，会开始 USB 复位和枚举流程：
1. 主机检测插入
2. 主机复位总线
3. 主机读取设备描述符
4. 主机读取配置描述符
5. 主机根据 CDC 描述符加载虚拟串口驱动
6. Windows 出现 COM 口

本板硬件原先没有 D+ 1.5k 上拉，所以 PC 不知道有 Full Speed 设备接入。
现在临时加了 D+ 到 3.3V 的上拉后，PC 能进入枚举流程。

注意：标准值通常是 1.5k 到 3.3V；当前 1k 可以短时间验证，但后续改板建议换标准 1.5k。


3. 为什么 MCU 能直接给 PC 发数据
-------------------------------
MCU 不是“直接把字符串打到 USB 线上”。
实际过程是：

cdc_printf()
  -> 格式化字符串
  -> cdc_write()
  -> 把数据放进 CDC 类缓冲区
  -> cdc_acm_data_send()
  -> usbd_ep_send()
  -> USB Device 端点发送
  -> PC USB 驱动接收
  -> 串口助手显示文本

中间的 USB 协议层由这些文件处理：
- usbd_core.c：USB Device 核心初始化和端点发送/接收入口
- usbd_enum.c：处理枚举、标准请求、描述符请求
- usbd_transc.c：控制传输处理
- usbd_lld_core.c：底层端点、地址、缓冲区操作
- usbd_lld_int.c：USB 中断分发
- cdc_acm_core.c：CDC ACM 类协议、端点初始化、收发缓冲区管理

你自己主要使用 cdc.c 暴露出来的简单接口即可。


4. cdc_read() 的作用
-------------------
典型形式：

uint32_t cdc_read(uint8_t *buf, uint32_t max_len)
{
    usb_cdc_handler *cdc;
    uint32_t len;

    if ((NULL == buf) || (0U == max_len)) {
        return 0U;
    }

    if (USBD_CONFIGURED != usbd_cdc.cur_status) {
        return 0U;
    }

    cdc = (usb_cdc_handler *)usbd_cdc.class_data[CDC_COM_INTERFACE];
    if (NULL == cdc) {
        return 0U;
    }

    if (0U == cdc->packet_receive) {
        return 0U;
    }

    len = cdc->receive_length;
    if (len > max_len) {
        len = max_len;
    }

    memcpy(buf, cdc->data, len);

    cdc_acm_data_receive(&usbd_cdc);

    return len;
}

逐行理解：

- buf：调用者提供的接收缓冲区，例如 cli_poll() 里的 rx。
- max_len：buf 最大能装多少字节，防止拷贝越界。
- 检查 buf 和 max_len：避免空指针和无效长度。
- 检查 usbd_cdc.cur_status：只有 USB 已配置完成后才读数据。
- class_data：USBD 给 CDC 类保存私有数据的位置。
- packet_receive：由 USB OUT 中断置位，表示 PC 发来了一包数据。
- receive_length：本次收到的数据长度。
- memcpy(buf, cdc->data, len)：真正把 PC 发来的数据复制到调用者的 rx 数组。
- cdc_acm_data_receive()：重新打开 OUT 端点，准备接收下一包。
- return len：告诉上层这次读到了多少字节。

cli_poll() 中：

uint8_t rx[64];
uint32_t len = cdc_read(rx, sizeof(rx));

rx 是局部数组，本身一开始没有内容。
调用 cdc_read(rx, sizeof(rx)) 时，rx 会退化成指针，传给 cdc_read 的 buf。
cdc_read 内部执行 memcpy(buf, cdc->data, len) 后，rx 里面才有 PC 发来的内容。

也就是说：
rx 的数据不是 cli_poll 自己生成的，而是 cdc_read 从 CDC 内部缓冲区复制进去的。


5. cli_poll() 的作用
-------------------
cli_poll() 不负责底层 USB。
它只做三件事：
1. 调用 cdc_read() 看看 PC 有没有发数据。
2. 把收到的字节拼成一行字符串。
3. 遇到 \r 或 \n 后调用 cli_handle_command() 处理命令。

典型命令：
- help
- status
- range 10m
- range 1r
- range 10r
- zero


6. 后续建议的接口分层
--------------------
cdc.c / cdc.h：
- cdc_init()
- cdc_is_configured()
- cdc_read()
- cdc_write()
- cdc_printf()

cli.c / cli.h：
- cli_poll()
- cli_handle_command()
- cli_print_help()
- cli_print_status()

业务模块：
- measurement_get_latest()
- range_get_current()
- logger_start()
- logger_stop()
- logger_export_csv()

这样 USB、命令解析、功耗业务不会互相缠在一起。
