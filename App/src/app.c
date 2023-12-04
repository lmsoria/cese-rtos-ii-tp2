#include <stdbool.h>
#include <stdio.h>

#include "app.h"
#include "app_resources.h"

#include "SVC_led.h"
#include "SVC_button.h"

/// | Private typedef -----------------------------------------------------------
/// | Private define ------------------------------------------------------------
/// | Private macro -------------------------------------------------------------
/// | Private function prototypes -----------------------------------------------
/// | Private variables ---------------------------------------------------------
/// | Exported variables --------------------------------------------------------
LEDActiveObject ao_led;

/// | Private functions ---------------------------------------------------------

void app_init()
{
    printf("Main application starts here\n");

    // Initialize LED Active Object
    led_initialize_ao(&ao_led, "ao_led");

    // Initialize button service
    if(!svc_button_initialize()) {
    	printf("Error: Couldn't initialize button service\n");
    	configASSERT(false);
    }

}
