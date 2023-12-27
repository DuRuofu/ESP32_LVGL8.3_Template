#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_demos.h"

//用定时器提供lvgl时钟
static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(10);
}


void app_main(void)
{
    //printf("Hello world!\n");
    //LVGL初始化
    lv_init();
    lv_port_disp_init();

    /*  esp_register_freertos_tick_hook(lv_tick_task);
    Create and start a periodic timer interrupt to call lv_tick_inc 
    */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 10 * 1000));

    //启动demo
    lv_demo_benchmark();
    while (1)
    {
        lv_task_handler();
        //lv_tick_inc(10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
}

