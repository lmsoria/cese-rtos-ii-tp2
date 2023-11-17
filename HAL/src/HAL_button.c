#include <assert.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#include "HAL_button.h"

/// @brief Platform-dependant struct that wraps the vendor HAL for GPIO management (with interest on inputs).
typedef struct
{
    GPIO_TypeDef* port; ///< STM32 GPIO Port
    uint16_t pin;       ///< STM32 GPIO Pin
    uint8_t pullup;     ///< Whether the input has any pull-up/down resistor.
} ButtonStruct;

static const ButtonStruct AVAILABLE_BUTTONS[BUTTONS_TOTAL] =
{
    [USER_BUTTON] = {GPIOC, GPIO_PIN_13, GPIO_NOPULL},
};

ButtonStatus button_read(const BoardButtons button)
{
    assert(button < BUTTONS_TOTAL);
    const GPIO_PinState BUTTON_STATE = HAL_GPIO_ReadPin(AVAILABLE_BUTTONS[button].port, AVAILABLE_BUTTONS[button].pin);

    // If we have a pull-up resistor, when the button is pressed the ReadPin function should return a low state.
    // If we have a pull-down resistor (and no pull?), the opposite happens (button pressed -> GPIO_PIN_SET read)
    GPIO_PinState condition_check = (AVAILABLE_BUTTONS[button].pullup == GPIO_PULLUP) ? GPIO_PIN_RESET : GPIO_PIN_SET;

    return (BUTTON_STATE == condition_check) ? BUTTON_PRESSED : BUTTON_RELEASED;
}

ButtonStatus button_debounce(ButtonStatus button_raw_read)
{
	static uint16_t state = 0; // Current debounce status.
	state = (state << 1) | 0xe000 | ( button_raw_read == BUTTON_PRESSED ? 0 : 1);
	return (state == 0xf000 ? BUTTON_PRESSED : BUTTON_RELEASED);
}
