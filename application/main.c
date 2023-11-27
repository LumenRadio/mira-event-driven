/*----------------------------------------------------------------------------
Copyright (c) 2022 LumenRadio AB
This code is the property of Lumenradio AB and may not be redistributed in any
form without prior written permission from LumenRadio AB.
----------------------------------------------------------------------------*/

#include <mira.h>
#include <stdio.h>

#include "aggregator.h"
#include "application_config.h"
#include "event_dispatch.h"
#include "manager.h"
#include "sampler.h"

// ******************************************************************************
// Module constants
// ******************************************************************************

// ******************************************************************************
// Module variables
// ******************************************************************************

MIRA_IODEFS(MIRA_IODEF_NONE,    /* fd 0: stdin */
            MIRA_IODEF_UART(0), /* fd 1: stdout */
            MIRA_IODEF_NONE     /* fd 2: stderr */
                                /* More file descriptors can be added, for use with dprintf(); */
);

// ******************************************************************************
// Function prototypes
// ******************************************************************************

// ******************************************************************************
// Function definitions
// ******************************************************************************
void mira_setup(void)
{
    mira_status_t uart_ret;
    mira_uart_config_t uart_config = { .baudrate = 115200,
                                       .tx_pin = MIRA_GPIO_PIN(0, 6),
                                       .rx_pin = MIRA_GPIO_PIN(0, 8) };

    uart_ret = mira_uart_init(0, &uart_config);
    if (uart_ret != MIRA_SUCCESS) {
        /* Nowhere to send an error message */
    }

    printf("Starting Node (mira-event-driven)\n");

    event_dispatch_init();

    manager_init(&(manager_config_t){ .measurement_period_s = APPLICATION_MEASUREMENT_PERIOD_S });

    aggregator_init();
    sampler_init();
}

// ******************************************************************************
// Internal functions
// ******************************************************************************
