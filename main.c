/*
 * $ Copyright YEAR Cypress Semiconductor $
 */

/** @file
*
* File Name:   main.c
*
* Description: This is the source code for the Bluetooth LE Beacon application
*              for ModusToolbox.
*
* Related Document: See README.md
*
*/
#include "cybt_platform_trace.h"
#include "wiced_bt_trace.h"
#include "cybsp.h"
#include "cybt_platform_config.h"
#include "beacon.h"
#ifdef ENABLE_BT_SPY_LOG
#include "cybt_debug_uart.h"
#else
#include "cy_retarget_io.h"
#endif

#define CYBSP_CONTROLLER_FEATURE_BAUD_RATE 115200
#define CYBSP_BAUD_RATE_FW_DOWNLOAD 115200

extern void vTaskStartScheduler( void );

const cybt_platform_config_t bt_platform_cfg_settings =
{
    .hci_config =
    {
        .hci_transport = CYBT_HCI_IPC,
    },

    .controller_config =
    {
        .bt_power_pin      = NC,
#if defined(CY_CFG_PWR_SYS_IDLE_MODE) && \
                   ((CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_SLEEP) || \
                   (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP))
        .sleep_mode = { .sleep_mode_enabled = 1 },
#else
        .sleep_mode = { .sleep_mode_enabled = 0 },
#endif
    },

    .task_mem_pool_size    = 2048
};

int main()
{
    cy_rslt_t result;

    /* Initialize the board support package */
    result = cybsp_init() ;

    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

#ifdef ENABLE_BT_SPY_LOG
    {
        cybt_debug_uart_config_t config = {
            .uart_tx_pin = CYBSP_DEBUG_UART_TX,
            .uart_rx_pin = CYBSP_DEBUG_UART_RX,
            .uart_cts_pin = CYBSP_DEBUG_UART_CTS,
            .uart_rts_pin = CYBSP_DEBUG_UART_RTS,
            .baud_rate = DEBUG_UART_BAUDRATE,
            .flow_control = TRUE};
        cybt_debug_uart_init(&config, NULL);
    }
#else
    {
        /* Initialize retarget-io to use the debug UART port */
        cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    }
#endif //ENABLE_BT_SPY_LOG

    WICED_BT_TRACE("main start!\n");
    cybt_platform_set_trace_level(CYBT_TRACE_ID_STACK, CYBT_TRACE_ID_MAX);
    cybt_platform_config_init(&bt_platform_cfg_settings);
    /* Initialize GPIO for button interrupt*/
    if(CY_RSLT_SUCCESS != cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF))
    {
        WICED_BT_TRACE("Button GPIO init failed! \n");
        CY_ASSERT(0);
    }

    /* Configure GPIO interrupt. */
    //cyhal_gpio_register_callback(CYBSP_USER_BTN, button_interrupt_handler, NULL);
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL, CYHAL_ISR_PRIORITY_DEFAULT, true);

    application_start();
    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    /* Should never get here */
    CY_ASSERT(0);
}

/* [] END OF FILE */
