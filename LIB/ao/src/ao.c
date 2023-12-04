#include <stdio.h>

#include "ao.h"

#define AO_TASK_PRIORIY (tskIDLE_PRIORITY + 1UL)

static void ao_task(void* parameters)
{
	ActiveObject* const AO = (ActiveObject*)(parameters);

    Event event;
    printf("[%s] Task Running\n", pcTaskGetName(NULL));

    while (1) {
        if (xQueueReceive(AO->queue_handle, &event, portMAX_DELAY) == pdPASS) {
            if(AO->dispatch_function) {
            	AO->dispatch_function(&event);
            }
        }
    }
}


bool ao_initialize(ActiveObject* const ao, const char* task_name, dispatch_event_handler_t handler)
{
    if(ao == NULL) {
    	printf("[AO] Received NULL AO\n");
    	return false;
    }

    if(handler == NULL) {
    	printf("[AO] Received NULL handler\n");
    	return false;
    }

    ao->dispatch_function = handler;

    ao->task_handle = xTaskCreateStatic(
                          ao_task,
                          task_name,
                          ao->task_stack_size,
                          (void*) ao,
                          AO_TASK_PRIORIY,
                          ao->task_stack,
                          &ao->task_control_block);

    if(ao->task_handle == NULL) {
    	printf("[AO] Error creating task \"%s\"\n", task_name);
    	return false;
    }

    ao->queue_handle = xQueueCreateStatic(
    		AO_QUEUE_LENGHT,
			sizeof(Event),
			ao->queue_storage_area,
			ao->queue_structure);

    if(ao->queue_handle == NULL) {
    	printf("[AO] Error creating queue for task \"%s\"\n", task_name);
    	return false;
    }

    printf("[AO] Active Object \"%s\" created!\n", task_name);
    return true;
}

bool ao_send_event(ActiveObject* ao, Event* const event)
{
    if (xQueueSend(ao->queue_handle, (void*)(event), portMAX_DELAY) != pdPASS) {
        printf("Error sending event to \"%s\" queue\n", pcTaskGetName(ao->task_handle));
        return false;
    }

    return true;
}
