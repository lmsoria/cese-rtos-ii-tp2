#pragma once

#include <stdint.h>

/// @brief 
typedef void (*dispatch_event_handler_t)(uint32_t, void*);

/// @brief Common structure that represents the data passed to a task when an event occurs 
typedef struct
{
	uint32_t id;            ///< The ID of the event, defined by the services.
	void* opt_data_address; ///< Optional data defined by the event.
} Event;
