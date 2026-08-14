# Galvanometer GD32F303 board test

First bring-up firmware for the GD32F303CGT6 power analyzer board.

Build:

```powershell
cmake --preset gcc-debug
cmake --build --preset gcc-debug
```

Output files are generated in `build/gcc-debug`:

- `galvanometer.elf`
- `galvanometer.hex`
- `galvanometer.bin`

Initial test behavior:

- LCD fills white normally.
- KEY1/KEY2/KEY3/KEY4 change LCD color to red/green/blue/yellow.
- `GD32_DUT_EN` toggles every 500 ms, so `DUT_SRC` should toggle when H4 selects the onboard LDO.
- SPI Flash JEDEC ID is stored in `g_flash_id` for debugger inspection.
- I2C scan masks are stored in `g_i2c_low_mask` and `g_i2c_high_mask`.
