#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "./driver/gpio.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_demos.h"


static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        const char * txt = lv_btnmatrix_get_btn_text(obj, id);
 
        LV_LOG_USER("%s was pressed\n", txt);
    }
}
 
 
 
static const char * btnm_map[] = {"1", "2", "3", "4", "5", "\n",
                                  "6", "7", "8", "9", "0", "\n",
                                  "Action1", "Action2", ""
                                 };


//用定时器提供lvgl时钟
static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(10);
}


void app_main(void)
{





    //printf("Hello world!\n");
    lv_init();              //LVGL初始化
    lv_port_disp_init();    //显示器初始化

    //初始化按键(io4 and io19)
    gpio_config_t io_conf =
    {
        .intr_type      = GPIO_INTR_DISABLE,
        .mode           = GPIO_MODE_INPUT,
        .pin_bit_mask   = GPIO_SEL_4,
        .pull_down_en   = GPIO_PULLDOWN_DISABLE,
        .pull_up_en     = GPIO_PULLUP_ENABLE
    };
    gpio_config(&io_conf);
    io_conf.pin_bit_mask = GPIO_SEL_19;
    gpio_config(&io_conf);
    lv_port_indev_init();   //输入设备初始化
    lv_group_t *group = lv_group_create();
    lv_indev_set_group(indev_keypad, group);
 

    //心跳时钟
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
    //lv_demo_benchmark();
     lv_obj_t * btnm1 = lv_btnmatrix_create(lv_scr_act());
    lv_btnmatrix_set_map(btnm1, btnm_map);
    lv_btnmatrix_set_btn_width(btnm1, 10, 2);        /*Make "Action1" twice as wide as "Action2"*/
    lv_btnmatrix_set_btn_ctrl(btnm1, 10, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_btn_ctrl(btnm1, 11, LV_BTNMATRIX_CTRL_CHECKED);
    lv_obj_align(btnm1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btnm1, event_handler, LV_EVENT_ALL, NULL);

    lv_group_add_obj(group ,btnm1);

    while (1)
    {   
        lv_task_handler();
        //lv_tick_inc(10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
}

