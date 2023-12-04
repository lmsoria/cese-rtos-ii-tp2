#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "event.h"

/// @brief Active Object struct, which consists of an event queue and a task that will process them.
typedef struct
{
    QueueHandle_t queue;                        ///< Event Queue.
    TaskHandle_t* task;                         ///< Handle of the queue reception task
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
