#include <stdio.h>

#include "app.h"
#include "app_resources.h"

#include "SVC_led.h"
#include "SVC_button.h"

/// | Private typedef -----------------------------------------------------------
/// | Private define ------------------------------------------------------------
/// | Private macro -------------------------------------------------------------
/// | Private function prototypes -----------------------------------------------
/// | Private variables ---------------------------------------------------------
/// | Exported variables --------------------------------------------------------
TaskHandle_t button_task_handle;
LEDActiveObject ao_led;

/// | Private functions ---------------------------------------------------------

void app_init()
{
    BaseType_t ret;

    printf("Main application starts here\n");

    // Initialize LED Active Object
    led_initialize_ao(&ao_led, "ao_led");

    // Create button task
    ret = xTaskCreate(
            task_button,
            "Task Button",
            (2 * configMINIMAL_STACK_SIZE),
            NULL,
            (tskIDLE_PRIORITY + 1UL),
            &button_task_handle);
    configASSERT(ret == pdPASS);
}
