
#### 说明：

1. 本文档由杜若甫撰写，由杜若甫负责解释及执行。
2. 硬件平台：ESP32-WROOM-32   1.3寸tft显示屏（240x240）（st7735驱动芯片）

#### 修订历史：

|  文档名称  |  版本  |  作者  |      时间      |   备注   |
| :----: | :----: | :------------: | :------: |
| esp-idf移植lvgl | DuRuofu | 2023-04-28 | 首次建立 |

## esp-idf移植lvgl

### 1.准备工作
准备空白工程（确认可以顺利编译）和对应的lvgl代码及对应驱动库
![](attachments/Pasted%20image%2020230428185535.png)
对应地址：
https://github.com/lvgl/lvgl
https://github.com/lvgl/lvgl_esp32_drivers
![](attachments/Pasted%20image%2020230428185658.png)
### 2.文件移植
将要移植的代码移动至工程组件目录，并修改名称，调整部分代码
![](attachments/Pasted%20image%2020230428185957.png)
lvgl目录下如图所示：
![](attachments/Pasted%20image%2020230428190127.png)

进行编译：报如下错误
![](attachments/Pasted%20image%2020230428191954.png)
#### 错误1：
```
../components/lvgl_esp32_drivers/lvgl_helpers.h:57:25: error: 'LV_HOR_RES_MAX' undeclared (first use in this function); did you mean 'LV_HOR_RES'?

```
解决方法：

在lvgl_esp32_drivers文件夹中的lvg_helpers.h中定义一个宏#define LV_HOR_RES_MAX 320，顺便加上另外一个宏定义
![](attachments/Pasted%20image%2020230428192242.png)
#### 错误2：
```
../components/lvgl_esp32_drivers/lvgl_helpers.c:157:28: error: 'SPI_HOST_MAX' undeclared (first use in this function); did you mean 'GPIO_PORT_MAX'?

```
在lvgl_esp32_drivers文件夹中的lvg_helpers.h中定义一个宏`#define SPI_HOST_MAX 3`
![](attachments/Pasted%20image%2020230428192527.png)

-   `SPI_HOST_MAX` 宏定义用于设置支持的SPI主机数量。在LVGL的某些驱动程序中，需要指定使用哪个SPI主机，通过该宏可以限制可用的SPI主机数量，保证程序的正确性。

-   `LV_HOR_RES_MAX` 和 `LV_VER_RES_MAX` 宏定义分别用于设置屏幕的最大水平和垂直分辨率。在使用LVGL库时，需要根据实际使用的设备设置这两个宏定义，以确保界面的正确显示。


修改后编译不报错即可：
![](attachments/Pasted%20image%2020230428193006.png)

### 3.驱动配置


然后将lvgl/examples下的porting提升为组件（将这个文件夹作为新的组件）
![](attachments/Pasted%20image%2020230428200026.png)
修改名称为lvgl_porting,并配置cmakelist文件
![](attachments/Pasted%20image%2020230428200207.png)
这里面的文件分别是显示，文件系统，以及输入部分，
![](attachments/Pasted%20image%2020230428200349.png)
修改名称如下：（以显示为例：）

![](attachments/Pasted%20image%2020230428200522.png)


cmakelist文件如下：
```
idf_component_register(SRCS "lv_port_disp.c"
                       INCLUDE_DIRS "."
                       REQUIRES lvgl lvgl_esp32_drivers
                       )
```


同时修改代码里的宏定义启用文件：（0改为1，.c  .h  都改）
![](attachments/Pasted%20image%2020230428200611.png)
记得头文件也要修改，（刚才修改了文件名）
![](attachments/Pasted%20image%2020230428200807.png)

#### 设置屏幕大小：
![](attachments/Pasted%20image%2020230428202708.png)
#### 设置屏幕缓存模式：
（这里注释其他两种，使用双缓存）
![](attachments/Pasted%20image%2020230428202759.png)
设置为模式2：
![](attachments/Pasted%20image%2020230428202839.png)
#### 设置刷屏方式：
![](attachments/Pasted%20image%2020230428202925.png)
函数来自与于lvgl_esp32_drivers组件disp_driver.h文件
```
/* Initialize display */

void *disp_driver_init(void);

  

/* Display flush callback */

void disp_driver_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_map);

  

/* Display rounder callback, used with monochrome dispays */

void disp_driver_rounder(lv_disp_drv_t * disp_drv, lv_area_t * area);

  

/* Display set_px callback, used with monochrome dispays */

void disp_driver_set_px(lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,

    lv_color_t color, lv_opa_t opa);
```
#### 设置初始化入口函数
disp_driver_init();同样来自于lvgl_esp32_drivers组件disp_driver.h文件
![](attachments/Pasted%20image%2020230428203237.png)


### 4. 在设置中配置硬件
打开设置
![](attachments/Pasted%20image%2020230428204418.png)
进行硬件配置：
![](attachments/Pasted%20image%2020230428204615.png)
#### 配置控制芯片：
![](attachments/Pasted%20image%2020230428205031.png)
#### 配置速度：
![](attachments/Pasted%20image%2020230428205101.png)
#### 配置引脚
![](attachments/Pasted%20image%2020230428205116.png)
![](attachments/Pasted%20image%2020230428205539.png)

#### 启用lvgl的demo
![](attachments/Pasted%20image%2020230428210352.png)
#### 启用字体
![](attachments/Pasted%20image%2020230428210310.png)
#### 写测试程序
``` c
/* LVGL Example project
 *
 * Basic project to test LVGL on ESP32 based projects.
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
 
/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#include "demos/lv_demos.h"
#else
#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#endif
 
 
#include "lvgl_helpers.h"
 
/*********************
 *      DEFINES
 *********************/
#define TAG "demo"
#define LV_TICK_PERIOD_MS 1
 
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
static void create_demo_application(void);
 
/**********************
 *   APPLICATION MAIN
 **********************/
void app_main()
{
 
    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */
    xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 2, NULL, 0, NULL, 1);
}
 
/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;
 
static void guiTask(void *pvParameter)
{
 
    (void)pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();
 
    lv_init();
 
    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();
 
    lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);
 
    /* Use double buffered when not working with monochrome displays */
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2 != NULL);
#else
    static lv_color_t *buf2 = NULL;
#endif
 
    static lv_disp_draw_buf_t disp_buf;
 
    uint32_t size_in_px = DISP_BUF_SIZE;
 
#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820 || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306
    size_in_px *= 8;
#endif
 
    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px);
 
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;
 
    /* When using a monochrome display we need to register the callbacks:
     * - rounder_cb
     * - set_px_cb */
#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif
 
    disp_drv.draw_buf = &disp_buf;
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    lv_disp_drv_register(&disp_drv);
 
    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif
 
    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui",
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));
 
    /* Create the demo application */
    create_demo_application();
 
    while (1)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));
 
        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }
 
    /* A task should NEVER return */
    free(buf1);
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    free(buf2);
#endif
    vTaskDelete(NULL);
}
 
static void create_demo_application(void)
{
    lv_demo_music();
}
 
static void lv_tick_task(void *arg)
{
    (void)arg;
 
    lv_tick_inc(LV_TICK_PERIOD_MS);
}
```


## 参考
https://blog.csdn.net/XiaoXiaoPengBo/article/details/124137559