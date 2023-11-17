#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "HAL_led.h"

/// @brief Type of events handled by the LED AO
typedef enum
{
    LED_EVENT_ON,     ///< Turn on a LED
    LED_EVENT_OFF,    ///< Turn off a LED
    LED_EVENT_TOGGLE, ///< Toggle a LED
} LEDEventType;

/// @brief Wrapper for LEDs between Application <-> HAL
typedef enum
{
    LED_GREEN = LED1, ///< Green LED
    LED_RED = LED3,   ///< Red LED
} ApplicationLEDs;

/// @brief Struct that determines a LED Event itself. It consists of two thigs:
typedef struct
{
    ApplicationLEDs led; ///< Which LED we want to handle
    LEDEventType type;   ///< What action do we need to perform on the LED
} LEDEvent;

/// @brief LED Active Object. It basically consists of an event queue and a Task that process the queue.
typedef struct
{
    QueueHandle_t queue;
    TaskHandle_t* task;
} LEDActiveObject;

/// @brief Initialize the LED Active Object. By default This function will assign `task_led()` to the task field
/// @param ao Active Object to initialize
/// @param ao_task_name Name for the task
void led_initialize_ao(LEDActiveObject* ao, const char* ao_task_name);

/// @brief Post an Event to the LED Active Object queue.
/// @param ao Receiver of the event
/// @param event event to be sent
void led_ao_send_event(LEDActiveObject* ao, LEDEvent* const event);