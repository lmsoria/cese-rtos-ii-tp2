#pragma once

/// @brief Possible Button status
typedef enum
{
    BUTTON_RELEASED = 0,
    BUTTON_PRESSED = 1,
} ButtonStatus;

/// @brief Enum that keeps track of the available LEDs
typedef enum
{
    USER_BUTTON = 0, ///< User Button (the blue one)
    BUTTONS_TOTAL,   ///< Total amount of buttons. Keep this value always at the bottom!
} BoardButtons;

/// @brief Read the status of the specified button
/// @param button Must be one of the defined in BoardButtons
/// @return BUTTON_PRESSED if the button is pressed, and BUTTON_RELEASED otherwise.
ButtonStatus button_read(const BoardButtons button);

ButtonStatus button_debounce(ButtonStatus button_raw_read);
