#include <assert.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#include "HAL_led.h"

/// @brief Polarity of the LED.
typedef enum
{
	ACTIVE_LOW,  ///< A logical 0 will turn on the LED
	ACTIVE_HIGH, ///< A logical 1 will turn on the LED
} Polarity;

/// @brief Platform-dependant struct that wraps the vendor HAL for GPIO management (with interest on outputs).
typedef struct
{
    GPIO_TypeDef* port; ///< STM32 GPIO Port
    uint16_t pin;       ///< STM32 GPIO Pin
    Polarity polarity;  ///< Polarity of the output
} LEDStruct;

static const LEDStruct AVAILABLE_LEDS[LEDS_TOTAL] =
{
    [LED1] = {GPIOB, GPIO_PIN_0, ACTIVE_HIGH},
    [LED2] = {GPIOB, GPIO_PIN_7, ACTIVE_HIGH},
    [LED3] = {GPIOB, GPIO_PIN_14, ACTIVE_HIGH},
};

void led_toggle(const BoardLEDs led)
{
    assert(led < LEDS_TOTAL);
    HAL_GPIO_TogglePin(AVAILABLE_LEDS[led].port, AVAILABLE_LEDS[led].pin);
}

void led_write(const BoardLEDs led, const LEDStatus status)
{
    assert(led < LEDS_TOTAL);
    assert(status == LED_ON || status == LED_OFF);

    GPIO_PinState state = AVAILABLE_LEDS[led].polarity == ACTIVE_HIGH ? GPIO_PIN_RESET : GPIO_PIN_SET;
    switch(status)
    {
    case LED_ON:
    	state = AVAILABLE_LEDS[led].polarity == ACTIVE_HIGH ? GPIO_PIN_SET : GPIO_PIN_RESET;
    	break;
    case LED_OFF:
    	state = AVAILABLE_LEDS[led].polarity == ACTIVE_HIGH ? GPIO_PIN_RESET : GPIO_PIN_SET;
    	break;
    }

    HAL_GPIO_WritePin(AVAILABLE_LEDS[led].port, AVAILABLE_LEDS[led].pin, state);
}

void led_set(const BoardLEDs led) { led_write(led, LED_ON); }

void led_clear(const BoardLEDs led) { led_write(led, LED_OFF); }
