#include <assert.h>
#include <stdio.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

#include "HAL_uart.h"

/// | Private typedef -----------------------------------------------------------

/// @brief Platform-dependant struct that wraps the vendor HAL for the UART.
/// It also contains callbacks for notifying that a transaction has finished.
typedef struct
{
    UART_HandleTypeDef huart;         ///< STM32 UART instance.
    uart_callback_t tx_done_callback; ///< Callback triggered every time a UART transmission ends.
    uart_callback_t rx_done_callback; ///< Callback triggered every time a UART reception ends. The reception buffer must be read here.
} UARTInstance_port;

/// | Private define ------------------------------------------------------------

#define INITIALIZATION_BUFFER_SIZE 256 // Size of the buffer used for transmitting the UART initialization message

/// | Private macro -------------------------------------------------------------

/// | Private variables ---------------------------------------------------------

/// @brief This array will map the UART instances of the API to the actual UART instances that the microcontroller has.
/// By default the callbacks are NULL, and should be initialized during `uart_init()`.
static UARTInstance_port UART_INSTANCES[UART_INSTANCE_TOTAL] =
{
    [UART_INSTANCE_1] =
    {
        .huart =
        {
            .Instance = USART3,
        },
        .tx_done_callback = NULL,
        .rx_done_callback = NULL,
    },
};

/// | Private function prototypes -----------------------------------------------

/// @brief Returns a printable string of the UART instance in use. Used for debugging purposes.
/// @param handler UART instance
/// @return
static const char* uart_instance_name(UART_HandleTypeDef* handler);

/// @brief Returns a printable string of the data bits used by the UART instance. Used for debugging purposes.
/// @param handler
/// @return
static const char* uart_data_bits_string(UART_HandleTypeDef* handler);

/// @brief Returns a printable string of the stop bits used by the UART instance. Used for debugging purposes.
/// @param handler UART instance
/// @return
static const char* uart_stop_bits_string(UART_HandleTypeDef* handler);

/// @brief Returns a printable string of the parity used by the UART instance. Used for debugging purposes.
/// @param handler UART instance
/// @return
static const char* uart_parity_string(UART_HandleTypeDef* handler);

/// @brief Helper function for parsing UARTDataBits values to platform-dependant data bits.
///        This function should return a value suitable for the used microcontroller (ie for STM32 it's an uint32_t).
/// @param data_bits This function will assert if an invalid value is passed.
/// @return Parsed data bits.
static uint32_t parse_data_bits(const UARTDataBits data_bits);

/// @brief Helper function for parsing UARTStopBits values to platform-dependant stop bits.
///        This function should return a value suitable for the used microcontroller (ie for STM32 it's an uint32_t).
/// @param stop_bits This function will assert if an invalid value is passed.
/// @return Parsed stop bits.
static uint32_t parse_stop_bits(const UARTStopBits stop_bits);

/// @brief Helper function for parsing UARTStopBits values to platform-dependant parity.
///        This function should return a value suitable for the used microcontroller (ie for STM32 it's an uint32_t).
/// @param parity This function will assert if an invalid value is passed.
/// @return Parsed parity.
static uint32_t parse_parity(const UARTParity parity);

/// | Private functions ---------------------------------------------------------

static const char* uart_instance_name(UART_HandleTypeDef* handler)
{
    if(handler->Instance == USART1) { return "USART1"; }
    else if(handler->Instance == USART2) { return "USART2"; }
    else if(handler->Instance == USART3) { return "USART3"; }
    else if(handler->Instance == UART4) { return "UART4"; }
    else if(handler->Instance == UART5) { return "UART5"; }
    else { return "Unknown"; }
}

static const char* uart_data_bits_string(UART_HandleTypeDef* handler)
{
    if(handler->Init.WordLength == UART_WORDLENGTH_8B) { return "8 bits"; }
    else if(handler->Init.WordLength == UART_WORDLENGTH_9B) { return "9 bits"; }
    else { return "Unknown"; }
}

static const char* uart_stop_bits_string(UART_HandleTypeDef* handler)
{
    if(handler->Init.StopBits == UART_STOPBITS_1) { return "1 bit"; }
    else if(handler->Init.StopBits == UART_STOPBITS_2) { return "2 bits"; }
    else { return "Unknown"; }
}

static const char* uart_parity_string(UART_HandleTypeDef* handler)
{
    if(handler->Init.Parity == UART_PARITY_NONE) { return "None"; }
    else if(handler->Init.Parity == UART_PARITY_EVEN) { return "Even"; }
    else if(handler->Init.Parity == UART_PARITY_ODD) { return "Odd"; }
    else { return "Unknown"; }
}

static uint32_t parse_data_bits(const UARTDataBits data_bits)
{
    switch(data_bits)
    {
    case DATA_BITS_8:
        return UART_WORDLENGTH_8B;
    case DATA_BITS_9:
        return UART_WORDLENGTH_9B;
    default:
        assert(false && "Invalid Data Bits");
    }

    // Should never reach here.
    return 0;
}

static uint32_t parse_stop_bits(const UARTStopBits stop_bits)
{
    switch(stop_bits)
    {
    case STOP_BITS_1:
        return UART_STOPBITS_1;
    case STOP_BITS_2:
        return UART_STOPBITS_2;
    default:
        assert(false && "Invalid Stop Bits");
    }

    // Should never reach here.
    return 0;
}

static uint32_t parse_parity(const UARTParity parity)
{
    switch(parity)
    {
    case PARITY_NONE:
        return UART_PARITY_NONE;
    case PARITY_EVEN:
        return UART_PARITY_EVEN;
    case PARITY_ODD:
        return UART_PARITY_ODD;
    default:
        assert(false && "Invalid Parity");
    }

    // Should never reach here.
    return 0;
}

bool uart_init(UARTConfig* config)
{
    uint8_t buffer[INITIALIZATION_BUFFER_SIZE];

    bool ret = false;
    const UARTInstance INSTANCE = config->instance;

    UART_INSTANCES[INSTANCE].huart.Init.BaudRate = config->baudrate;
    UART_INSTANCES[INSTANCE].huart.Init.WordLength = parse_data_bits(config->data_bits);
    UART_INSTANCES[INSTANCE].huart.Init.StopBits = parse_stop_bits(config->stop_bits);
    UART_INSTANCES[INSTANCE].huart.Init.Parity = parse_parity(config->parity);
    UART_INSTANCES[INSTANCE].huart.Init.Mode = UART_MODE_TX_RX;
    UART_INSTANCES[INSTANCE].huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UART_INSTANCES[INSTANCE].huart.Init.OverSampling = UART_OVERSAMPLING_16;
    UART_INSTANCES[INSTANCE].tx_done_callback = config->tx_done_callback;
    UART_INSTANCES[INSTANCE].rx_done_callback = config->rx_done_callback;

    ret = (HAL_UART_Init(&UART_INSTANCES[INSTANCE].huart) == HAL_OK);

    // Only print the message if the initialization was OK
    if(ret) {
        snprintf((char*)buffer, INITIALIZATION_BUFFER_SIZE,
                "--------------------\r\n"
                "%s initialized!\r\n"
                "Parameters:\r\n"
                "\t> Baudrate: %ld\r\n"
                "\t> Data bits: %s\r\n"
                "\t> Stop bits: %s\r\n"
                "\t> Parity: %s\r\n"
                "--------------------\r\n",
                uart_instance_name(&UART_INSTANCES[INSTANCE].huart),
                UART_INSTANCES[INSTANCE].huart.Init.BaudRate,
                uart_data_bits_string(&UART_INSTANCES[INSTANCE].huart),
                uart_stop_bits_string(&UART_INSTANCES[INSTANCE].huart),
                uart_parity_string(&UART_INSTANCES[INSTANCE].huart));

        printf("%s\n", buffer);
    }

    return ret;
}

void uart_send(UARTInstance instance, uint8_t* p_data, size_t size)
{
    HAL_UART_Transmit_IT(&UART_INSTANCES[instance].huart, p_data, size);
}

void uart_receive(UARTInstance instance, uint8_t* p_data, size_t size)
{
    HAL_UART_Receive_IT(&UART_INSTANCES[instance].huart, p_data, size);
}

void uart_irq_handler(UARTInstance instance)
{
    HAL_UART_IRQHandler(&UART_INSTANCES[instance].huart);
}

/// @brief Platform override for the original weak functions. It is fired every time a transmission has finished.
/// @param huart
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    for(size_t i = 0; i < UART_INSTANCE_TOTAL; i++) {
        if(huart == &UART_INSTANCES[i].huart && UART_INSTANCES[i].tx_done_callback) {
            UART_INSTANCES[i].tx_done_callback((UARTInstance)i);
            break;
        }
    }
}

/// @brief Platform override for the original weak functions. It is fired every time a reception has finished.
/// @param huart 
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    for(size_t i = 0; i < UART_INSTANCE_TOTAL; i++) {
        if(huart == &UART_INSTANCES[i].huart && UART_INSTANCES[i].rx_done_callback) {
            UART_INSTANCES[i].rx_done_callback((UARTInstance)i);
            break;
        }
    }
}
