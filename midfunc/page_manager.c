#include "page_manager.h"

pagestack_t pagestack;

void page_load(page_t *new_page);
void page_back(void);

void page_manage_init(page_t *homepage)
{
    pagestack.top = 0;
    page_load(homepage);
}

void page_load(page_t *new_page)
{
    if(new_page == NULL || pagestack.top >= MAX_DEPTH) return;
    pagestack.pages[pagestack.top++] = new_page;
    if(new_page->init)
    {
        new_page->init();
    }
}

void page_back(void)
{
    pagestack.pages[--pagestack.top]->deinit();
}

static page_t *page_get_current(pagestack_t *stack)
{
    if(stack->top == 0)
    {
        return NULL;
    }
    return stack->pages[stack->top -1];
}

page_t *page_get_nowpage(void)
{
    return page_get_current(&pagestack);
}