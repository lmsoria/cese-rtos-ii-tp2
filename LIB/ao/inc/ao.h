#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "event.h"

#define AO_QUEUE_LENGHT 16
#define AO_STATIC_QUEUE_STORAGE_AREA AO_QUEUE_LENGHT * sizeof(Event)

/// @brief Active Object struct, which consists of an event queue and a task that will process them.
typedef struct
{

	// Queue section
	QueueHandle_t queue_handle;
	StaticQueue_t* queue_structure;             ///< Event Queue.
	uint8_t* queue_storage_area;                ///< Event Queue memory area. Must be provided by the service.
	// Task section
    TaskHandle_t task_handle;                   ///< Handle of the queue reception task (QRT)
    StaticTask_t task_control_block;            ///< QRT control block. Must be provided by the service.
    StackType_t* task_stack;                    ///< QRT stack. Must be provided by the service.
    uint32_t task_stack_size;                   ///< QRT stack size. Must be provided by the service.
    // Dispatch section
    dispatch_event_handler_t dispatch_function; ///< Callback that will be triggered on each event reception.
} ActiveObject;

/// @brief Initialize an Active Object. This function will start the queue processing thread.
/// @param ao Active Object to be initialized.
/// @param task_name Name of the running threads.
/// @param handler callback function used for processing incoming events.
/// @return true if the Active Object was initialized successfully, false otherwise
bool ao_initialize(ActiveObject* const ao, const char* task_name, dispatch_event_handler_t handler);


/// @brief Post an Event to the Active Object's queue.
/// @param ao Receiver of the event.
/// @param event Event to be sent.
/// @return true if the event was pushed into the queue, false otherwise.
bool ao_send_event(ActiveObject* const ao, Event* const event);
