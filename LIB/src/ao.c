#include <stdio.h>

#include "ao.h"

#define AO_QUEUE_LENGTH 16
#define AO_TASK_STACK_SIZE (2 * configMINIMAL_STACK_SIZE)
#define AO_TASK_PRIORIY (tskIDLE_PRIORITY + 1UL)

static void ao_task(void* parameters)
{
	ActiveObject* const AO = (ActiveObject*)(parameters);

    Event event;
    printf("[%s] Task Running\n", pcTaskGetName(NULL));

    while (1) {
        if (xQueueReceive(AO->queue, &event, portMAX_DELAY) == pdPASS) {
            if(AO->dispatch_function) {
            	AO->dispatch_function(event.id, event.opt_data_address);
            }
        }
    }
}


bool ao_initialize(ActiveObject* const ao, const char* task_name, dispatch_event_handler_t handler)
{
    BaseType_t ret;

    if(ao == NULL) {
    	printf("[AO] Received NULL AO\n");
    	return false;
    }

    if(handler == NULL) {
    	printf("[AO] Received NULL handler\n");
    	return false;
    }

    ao->dispatch_function = handler;

    ret = xTaskCreate(
    		ao_task,
			task_name,
			AO_TASK_STACK_SIZE,
            (void*) ao,
			AO_TASK_PRIORIY,
            ao->task);

    if(ret != pdPASS) {
    	printf("[AO] Error creating task \"%s\"\n", task_name);
    	return false;
    }

    ao->queue = xQueueCreate(AO_QUEUE_LENGTH, sizeof(Event));
    if(ao->queue == NULL) {
    	printf("[AO] Error creating queue for task \"%s\"\n", task_name);
    	vTaskDelete(*ao->task);
    	return false;
    }

    printf("[AO] Active Object \"%s\" created!\n", task_name);
    return true;
}

bool ao_send_event(ActiveObject* ao, Event* const event)
{
    if (xQueueSend(ao->queue, (void*)(event), portMAX_DELAY) != pdPASS) {
        printf("Error sending event to \"%s\" queue\n", pcTaskGetName(*ao->task));
        return false;
    }

    return true;
}
