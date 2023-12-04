#include <stdbool.h>
#include <stdio.h>

#include "ao.h"
#include "app.h"
#include "SVC_led.h"
#include "SVC_button.h"

#include "memory_pool.h"

/// | Private typedef -----------------------------------------------------------
/// | Private define ------------------------------------------------------------
#define MEMORY_POOL_NBLOCKS             (10)
#define MEMORY_POOL_BLOCK_SIZE          (sizeof(Event))
/// | Private macro -------------------------------------------------------------
/// | Private function prototypes -----------------------------------------------
/// | Private variables ---------------------------------------------------------
static memory_pool_t memory_pool_;
static uint8_t memory_pool_memory_[MEMORY_POOL_SIZE(MEMORY_POOL_NBLOCKS, MEMORY_POOL_BLOCK_SIZE)];
/// | Exported variables --------------------------------------------------------
ActiveObject ao_led;
memory_pool_t* const MEMPOOL = &memory_pool_;

/// | Private functions ---------------------------------------------------------

void app_init()
{
    printf("Main application starts here\n");

    memory_pool_init(MEMPOOL, memory_pool_memory_, MEMORY_POOL_NBLOCKS, MEMORY_POOL_BLOCK_SIZE);

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
