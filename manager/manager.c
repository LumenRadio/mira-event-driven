/*----------------------------------------------------------------------------
Copyright (c) 2022 LumenRadio AB
This code is the property of Lumenradio AB and may not be redistributed in any
form without prior written permission from LumenRadio AB.
----------------------------------------------------------------------------*/
#include "manager.h"

#include <mira.h>
#include <stdio.h>

#include "application_config.h"
#include "event_dispatch.h"

/* Comment this out to turn off debug prints. */
// #define DEBUG_PRINT
#if defined(DEBUG_PRINT)
#define PRINT(...) printf(__VA_ARGS__)
#else
#define PRINT(...)
#endif

// ******************************************************************************
// Module globals
// ******************************************************************************
process_event_t manager_event;

// ******************************************************************************
// Module macros
// ******************************************************************************
#define DEFAULT_NUMBER_OF_SAMPLES   (5)

// ******************************************************************************
// Module variables
// ******************************************************************************
static uint32_t manager_default_measurement_period_s = 1;

// ******************************************************************************
// Function prototypes
// ******************************************************************************
PROCESS(manager_proc, "Main process for manager module");

// ******************************************************************************
// Function definitions
// ******************************************************************************
void manager_init(
    manager_config_t *config)
{
    manager_event = process_alloc_event();

    if (config != NULL) {
        process_start(&manager_proc, &config->measurement_period_s);
    } else {
        process_start(&manager_proc, &manager_default_measurement_period_s);
    }
}

void manager_stop(
    void)
{
    process_exit(&manager_proc);
}

void manager_result_display(
    uint32_t value)
{
    printf("%s: measurement value: %lu\n", __func__, value);
}

void manager_progress_display(
    void)
{
    printf(".");
    fflush(stdout);
}

// ******************************************************************************
// Internal functions
// ******************************************************************************
PROCESS_THREAD(manager_proc, ev, data)
{
    static struct etimer timer;

    PROCESS_BEGIN();

    uint32_t measurement_period_s = *(uint32_t *) data;
    etimer_set(&timer, CLOCK_SECOND * measurement_period_s);

    PRINT("%s: started, period %lu s\n", __func__, measurement_period_s);

    while (1) {
        /* Event data must be static, because accessed asynchronously. */
        static manager_event_data_t manager_event_data = {
            .type = MANAGER_EVENT_MEASUREMENT_REQUEST,
            .number_of_samples = APPLICATION_N_SAMPLES_PER_MEASUREMENT,
            .sample_period_ms = APPLICATION_SAMPLE_PERIOD_MS
        };

        event_dispatch_post(manager_event, &manager_event_data);

        PROCESS_YIELD_UNTIL(etimer_expired(&timer));
        etimer_reset(&timer);
    }
    PROCESS_END();
}
