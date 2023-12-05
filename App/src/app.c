#include <stdbool.h>
#include <stdio.h>

#include "event.h"

#include "app.h"
#include "SVC_led.h"
#include "SVC_button.h"

#include "memory_pool.h"

/// | Private typedef -----------------------------------------------------------
/// | Private define ------------------------------------------------------------
#define MEMORY_POOL_NBLOCKS 16
#define MEMORY_POOL_BLOCK_SIZE sizeof(Event)
/// | Private macro -------------------------------------------------------------
/// | Private function prototypes -----------------------------------------------
/// | Private variables ---------------------------------------------------------
static memory_pool_t memory_pool;
static uint8_t memory_pool_area[MEMORY_POOL_SIZE(MEMORY_POOL_NBLOCKS, MEMORY_POOL_BLOCK_SIZE)];
/// | Exported variables --------------------------------------------------------
memory_pool_t* const MEMPOOL = &memory_pool;

/// | Private functions ---------------------------------------------------------

void app_init()
{
    printf("Main application starts here\n");

    memory_pool_init(MEMPOOL, memory_pool_area, MEMORY_POOL_NBLOCKS, MEMORY_POOL_BLOCK_SIZE);

    // Initialize led service
    if(!svc_led_initialize()) {
    	printf("Error: Couldn't initialize led service\n");
    	while(1);
    }

    // Initialize button service
    if(!svc_button_initialize()) {
    	printf("Error: Couldn't initialize button service\n");
    	while(1);
    }

}
