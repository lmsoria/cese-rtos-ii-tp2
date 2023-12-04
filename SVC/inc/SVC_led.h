#pragma once

#include <stdbool.h>

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

/// @brief Initialize the LED Active Object. By default This function will assign `task_led()` to the task field
/// @param ao Active Object to initialize
/// @param ao_task_name Name for the task
bool svc_led_initialize();
