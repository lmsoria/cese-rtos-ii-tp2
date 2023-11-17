#pragma once

/// @brief Events to be detected by the button task
typedef enum
{
    EVENT_INITIAL, ///< Initial state.
    EVENT_SHORT, ///< Detected when the button is being pressed in the range [EVENT_SHORT_THRESHOLD_MIN_MS, EVENT_LONG_THRESHOLD_MIN_MS)
    EVENT_LONG, ///< Detected when the button is being pressed in the range [EVENT_LONG_THRESHOLD_MIN_MS, EVENT_BLOCKED_THRESHOLD_MIN_MS)
    EVENT_BLOCKED ///< Detected when the button is being pressed in the range >= EVENT_BLOCKED_THRESHOLD_MIN_MS
} ButtonEvent;

void task_button(void* parameters);
