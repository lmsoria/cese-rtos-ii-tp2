#include <stdbool.h>
#include <stdio.h>

#include "ao.h"
#include "app.h"
#include "SVC_led.h"
#include "SVC_button.h"

/// | Private typedef -----------------------------------------------------------
/// | Private define ------------------------------------------------------------
/// | Private macro -------------------------------------------------------------
/// | Private function prototypes -----------------------------------------------
/// | Private variables ---------------------------------------------------------
/// | Exported variables --------------------------------------------------------
ActiveObject ao_led;

/// | Private functions ---------------------------------------------------------

void app_init()
{
    printf("Main application starts here\n");

    // Initialize LED Active Object
    if(!svc_led_initialize(&ao_led, "led")) {
    	printf("Error: Couldn't initialize led AO\n");
    	configASSERT(false);
    }

    // Initialize button service
    if(!svc_button_initialize()) {
    	printf("Error: Couldn't initialize button service\n");
    	configASSERT(false);
    }

}
