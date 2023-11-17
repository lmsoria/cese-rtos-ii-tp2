// ------ inclusions ---------------------------------------------------
#include <stdio.h>

#include "app_resources.h"

#include "HAL_led.h"
#include "SVC_led.h"

/// | Private typedef -----------------------------------------------------------
/// | Private define ------------------------------------------------------------

#define LED_AO_QUEUE_LENGTH 16

/// | Private macro -------------------------------------------------------------
/// | Private variables ---------------------------------------------------------
/// | Private function prototypes -----------------------------------------------

/// @brief Process events received on the AO queue
/// @param event
static void execute_event(const LEDEvent event);

/// @brief LED Active object task.
/// @param parameters should be a reference to the AO.
static void ao_led_task(void* parameters);

/// | Private functions ---------------------------------------------------------
static void ao_led_task(void* parameters)
{
    LEDActiveObject* const AO = (LEDActiveObject*) (parameters);

    LEDEvent event;
    printf("[%s] Task Created\n", pcTaskGetName(NULL));

    while (1) {
        if (xQueueReceive(AO->queue, &event, portMAX_DELAY) == pdPASS) {
            execute_event(event);
        }
    }
}

void led_initialize_ao(LEDActiveObject* ao, const char* ao_task_name)
{
    BaseType_t ret;

    ret = xTaskCreate(
            ao_led_task,
            ao_task_name,
            (2 * configMINIMAL_STACK_SIZE),
            (void*) ao,
            (tskIDLE_PRIORITY + 1UL),
            ao->task);
    configASSERT(ret == pdPASS);

    ao->queue = xQueueCreate(LED_AO_QUEUE_LENGTH, sizeof(LEDEvent));
    configASSERT(ao->queue);
}

static void execute_event(const LEDEvent event)
{
    printf("[%s] Event Received: ", pcTaskGetName(NULL));
    const BoardLEDs LED = event.led;

    switch (event.type) {
    case LED_EVENT_ON:
        printf("LED_EVENT_ON\n");
        led_set(LED);
        break;
    case LED_EVENT_OFF:
        printf("LED_EVENT_OFF\n");
        led_clear(LED);
        break;
    case LED_EVENT_TOGGLE:
        printf("LED_EVENT_TOGGLE\n");
        led_toggle(LED);
        break;
    default:
        configASSERT(pdFAIL && "Invalid LED event")
        ;
        break;
    }
}

void led_ao_send_event(LEDActiveObject* ao, LEDEvent* const event)
{
    if (xQueueSend(ao->queue, (void*)(event), portMAX_DELAY) != pdPASS) {
        printf("Error sending LED event\n");
    }
}
