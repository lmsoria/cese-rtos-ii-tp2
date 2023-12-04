#pragma once

/// | Includes ------------------------------------------------------------------

#include <FreeRTOS.h>
#include <task.h>

#include "SVC_led.h"

/// | Exported types ------------------------------------------------------------

/// | Exported data -------------------------------------------------------------

extern TaskHandle_t button_task_handle;
extern LEDActiveObject ao_led;

/// | Exported constants --------------------------------------------------------
/// | Exported macro ------------------------------------------------------------
/// | Exported functions --------------------------------------------------------
