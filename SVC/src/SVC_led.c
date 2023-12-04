// ------ inclusions ---------------------------------------------------
#include <stdio.h>

#include "memory_pool.h"

#include "HAL_led.h"
#include "SVC_led.h"

/// | Private typedef -----------------------------------------------------------
/// | Private define ------------------------------------------------------------

#define LED_TASK_STACK_SIZE 200

/// | Private macro -------------------------------------------------------------
/// | Private variables ---------------------------------------------------------

/// Structure that will hold the TCB of the task being created.
static StaticTask_t led_task_buffer;

/// Buffer that the task being created will use as its stack.
static StackType_t led_task_stack[LED_TASK_STACK_SIZE];

/// Structure that will hold the LED AO's queue structure.
static StaticQueue_t led_queue_structure;

/// Storage Area for LED AO's queue
static uint8_t led_queue_storage[AO_STATIC_QUEUE_STORAGE_AREA];

extern memory_pool_t* const MEMPOOL;

/// | Private function prototypes -----------------------------------------------

/// @brief Process events received on the AO queue
/// @param event
static void svc_led_dispatch_event(Event* event);

/// | Private functions ---------------------------------------------------------

bool svc_led_initialize(ActiveObject* const ao, const char* ao_task_name)
{
	ao->task_control_block = led_task_buffer;
	ao->task_stack = led_task_stack;
	ao->task_stack_size = LED_TASK_STACK_SIZE;
	ao->queue_structure = &led_queue_structure;
	ao->queue_storage_area = led_queue_storage;
	return ao_initialize(ao, ao_task_name, svc_led_dispatch_event);
}

static void svc_led_dispatch_event(Event* event)
{
    printf("[%s] Event Received: ", pcTaskGetName(NULL));
    const ApplicationLEDs LED = (ApplicationLEDs)(event->opt_data_address);

    switch ((LEDEventType)(event->id)) {
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

    memory_pool_block_put(MEMPOOL, (void*)event);
}
