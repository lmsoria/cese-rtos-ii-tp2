// ------ inclusions ---------------------------------------------------
#include <stdio.h>

#include "app_resources.h"

#include "HAL_led.h"
#include "SVC_led.h"

/// | Private typedef -----------------------------------------------------------
/// | Private define ------------------------------------------------------------
/// | Private macro -------------------------------------------------------------
/// | Private variables ---------------------------------------------------------
/// | Private function prototypes -----------------------------------------------

/// @brief Process events received on the AO queue
/// @param event
static void svc_led_dispatch_event(uint32_t event_type, void* target_led);

/// | Private functions ---------------------------------------------------------

bool svc_led_initialize(ActiveObject* const ao, const char* ao_task_name)
{
	return ao_initialize(ao, ao_task_name, svc_led_dispatch_event);
}

static void svc_led_dispatch_event(uint32_t event_type, void* target_led)
{
    printf("[%s] Event Received: ", pcTaskGetName(NULL));
    const ApplicationLEDs LED = (ApplicationLEDs)(target_led);

    switch ((LEDEventType)(event_type)) {
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
        configASSERT(pdFAIL && "Invalid LED event");
        break;
    }
}
