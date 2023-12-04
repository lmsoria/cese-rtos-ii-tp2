// ------ inclusions ---------------------------------------------------
#include <stdio.h>

#include "app_resources.h"

#include "HAL_button.h"

#include "SVC_button.h"
#include "SVC_led.h"

/// | Private typedef -----------------------------------------------------------

/// @brief Debouncer FSM States
typedef enum
{
    DEBOUNCER_STATE_WAIT_PRESS = 0,    ///< Key released, waiting to be pressed
    DEBOUNCER_STATE_DEBOUNCE_ACTIVE,   ///< The key has been pressed and we need to filter the debouncing
    DEBOUNCER_STATE_WAIT_RELEASE,      ///< Key pressed, waiting to be released
    DEBOUNCER_STATE_DEBOUNCE_INACTIVE, ///< The key has been released, so we need to filter the debouncing
} DebouncerState;

/// @brief Struct that applies an debouncing mechanism to an specific button.
typedef struct
{
    BoardButtons button;     ///< Button instance associated to the Debouncer
    DebouncerState state;    ///< Current debouncer state
    uint32_t timer_debounce; ///< Counter used for measuring debouncing transients
    uint32_t timer_up;       ///< Counter used for measuring pressed time.
} Debouncer;

/// | Private define ------------------------------------------------------------

#define DEBOUNCE_PERIOD_MS 40
#define EVENT_SHORT_THRESHOLD_MIN_MS 100
#define EVENT_LONG_THRESHOLD_MIN_MS 2000
#define EVENT_BLOCKED_THRESHOLD_MIN_MS 8000

/// | Private macro -------------------------------------------------------------

/// | Private variables ---------------------------------------------------------
extern LEDActiveObject ao_led;

/// | Private function prototypes -----------------------------------------------

/// @brief Process the "button pressed" action, which happens whenever the Debouncer is at DEBOUNCER_STATE_WAIT_RELEASE state.
///        Use this function to propagate events to other actors.
/// @param current_event current ButtonEvent. The function will modify its content (iif it's different from the previous one).
/// @param timer_up time that the button has been pressed, in ms
static void process_button_pressed_state(ButtonEvent* const current_event, const uint32_t timer_up);

/// @brief Process the "button released" action, which happens whenever the Debouncer is at DEBOUNCER_STATE_DEBOUNCE_INACTIVE state.
///        Use this function to propagate events to other actors.
/// @param current_event current ButtonEvent. The function won't modify its content.
static void process_button_released_state(ButtonEvent* const current_event);

/// | Private functions ---------------------------------------------------------
void task_button(void* parameters)
{
    Debouncer debouncer =
    {
        .button = USER_BUTTON,
        .state = DEBOUNCER_STATE_WAIT_PRESS,
        .timer_debounce = 0,
        .timer_up = 0,
    };

    ButtonEvent current_event = EVENT_INITIAL;

    printf("[%s] Task Created\n", pcTaskGetName(NULL));

    // Basic flow:
    // 1. Read button
    // 2. Debounce its state by using a Debouncer FSM
    // 3. If button is pressed, call process_button_pressed_state()
    // 4. If button released, call process_button_released_state()
    while (1) {
        switch (debouncer.state) {
        case DEBOUNCER_STATE_WAIT_PRESS:
            if (button_read(debouncer.button) == BUTTON_PRESSED) {
                debouncer.timer_debounce = pdMS_TO_TICKS(DEBOUNCE_PERIOD_MS);
                debouncer.state = DEBOUNCER_STATE_DEBOUNCE_ACTIVE;
            }
            break;
        case DEBOUNCER_STATE_DEBOUNCE_ACTIVE:
            if (button_read(debouncer.button) == BUTTON_RELEASED) {
                debouncer.state = DEBOUNCER_STATE_WAIT_PRESS;
            } else if (debouncer.timer_debounce) {
                debouncer.timer_debounce--;
            } else {
                debouncer.timer_up = 0;
                debouncer.state = DEBOUNCER_STATE_WAIT_RELEASE;
            }
            break;
        case DEBOUNCER_STATE_WAIT_RELEASE:
            if (button_read(debouncer.button) == BUTTON_RELEASED) {
                debouncer.timer_debounce = pdMS_TO_TICKS(DEBOUNCE_PERIOD_MS);
                debouncer.state = DEBOUNCER_STATE_DEBOUNCE_INACTIVE;
            }
            break;
        case DEBOUNCER_STATE_DEBOUNCE_INACTIVE:
            if (button_read(debouncer.button) == BUTTON_PRESSED) {
                debouncer.state = DEBOUNCER_STATE_WAIT_RELEASE;
            } else if (debouncer.timer_debounce) {
                debouncer.timer_debounce--;
            } else {
                debouncer.state = DEBOUNCER_STATE_WAIT_PRESS;
                process_button_released_state(&current_event);
            }
            break;
        default:
            debouncer.state = DEBOUNCER_STATE_WAIT_PRESS;
            break;
        }

        if (debouncer.state == DEBOUNCER_STATE_WAIT_RELEASE) {
            process_button_pressed_state(&current_event, ++debouncer.timer_up);
        } else {
            debouncer.timer_up = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static void process_button_pressed_state(ButtonEvent* const current_event, const uint32_t timer_up)
{
    ButtonEvent new_event;

    const char* BUTTON_TASK_NAME = pcTaskGetName(NULL);

    if (timer_up >= EVENT_SHORT_THRESHOLD_MIN_MS && timer_up < EVENT_LONG_THRESHOLD_MIN_MS) {
        new_event = EVENT_SHORT;
    } else if (timer_up >= EVENT_LONG_THRESHOLD_MIN_MS && timer_up < EVENT_BLOCKED_THRESHOLD_MIN_MS) {
        new_event = EVENT_LONG;
    } else if (timer_up >= EVENT_BLOCKED_THRESHOLD_MIN_MS) {
        new_event = EVENT_BLOCKED;
    } else {
        new_event = EVENT_INITIAL;
    }

    // Since this function is being called periodically, we need to keep track of the new event and only send events
    // when there is a difference with the previous one.
    if (new_event != *current_event) {
        *current_event = new_event;
        LEDEvent* event_to_be_sent = NULL;

        switch (*current_event) {
        case EVENT_SHORT:
            printf("[%s] Detected SHORT press\n", BUTTON_TASK_NAME);
            event_to_be_sent = pvPortMalloc(sizeof(LEDEvent));
            configASSERT(event_to_be_sent);
            event_to_be_sent->type = LED_EVENT_TOGGLE;
            event_to_be_sent->led = LED_GREEN;
            led_ao_send_event(&ao_led, event_to_be_sent);
            break;

        case EVENT_LONG:
            printf("[%s] Detected LONG press\n", BUTTON_TASK_NAME);
            event_to_be_sent = pvPortMalloc(sizeof(LEDEvent));
            configASSERT(event_to_be_sent);
            event_to_be_sent->type = LED_EVENT_TOGGLE;
            event_to_be_sent->led = LED_RED;
            led_ao_send_event(&ao_led, event_to_be_sent);
            break;

        case EVENT_BLOCKED:
            printf("[%s] Detected BLOCKED press\n", BUTTON_TASK_NAME);
            event_to_be_sent = pvPortMalloc(sizeof(LEDEvent));
            configASSERT(event_to_be_sent);
            event_to_be_sent->type = LED_EVENT_ON;
            event_to_be_sent->led = LED_RED;
            led_ao_send_event(&ao_led, event_to_be_sent);

            event_to_be_sent = pvPortMalloc(sizeof(LEDEvent));
            configASSERT(event_to_be_sent);
            event_to_be_sent->type = LED_EVENT_ON;
            event_to_be_sent->led = LED_GREEN;
            led_ao_send_event(&ao_led, event_to_be_sent);
            break;

        default:
            break;
        }
    }
}

static void process_button_released_state(ButtonEvent* const current_event)
{
	LEDEvent* event_to_be_sent = NULL;
    printf("[%s] Button Released\n", pcTaskGetName(NULL));

    switch (*current_event) {
    case EVENT_SHORT:
        break;

    case EVENT_LONG:
        break;

    case EVENT_BLOCKED:
        // As per design, only turn off the LEDs when the current state is BLOCKED
    	event_to_be_sent = pvPortMalloc(sizeof(LEDEvent));
    	configASSERT(event_to_be_sent);
        event_to_be_sent->type = LED_EVENT_OFF;
        event_to_be_sent->led = LED_RED;
        led_ao_send_event(&ao_led, event_to_be_sent);

    	event_to_be_sent = pvPortMalloc(sizeof(LEDEvent));
    	configASSERT(event_to_be_sent);
        event_to_be_sent->type = LED_EVENT_OFF;
        event_to_be_sent->led = LED_GREEN;
        led_ao_send_event(&ao_led, event_to_be_sent);
        break;

    default:
        break;
    }
}
