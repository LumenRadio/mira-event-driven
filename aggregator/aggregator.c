/*----------------------------------------------------------------------------
Copyright (c) 2022 LumenRadio AB
This code is the property of Lumenradio AB and may not be redistributed in any
form without prior written permission from LumenRadio AB.
----------------------------------------------------------------------------*/

#include "aggregator.h"

#include <mira.h>

#include "event_dispatch.h"

// ******************************************************************************
// Module globals
// ******************************************************************************
process_event_t aggregator_event;

// ******************************************************************************
// Module macros
// ******************************************************************************
#define AGGREGATOR_MAX_N_SAMPLES (128)

// ******************************************************************************
// Module variables
// ******************************************************************************
static aggregator_measurement_config_t measurement_config;
static uint16_t current_sample_index;
static uint32_t samples[AGGREGATOR_MAX_N_SAMPLES];

// ******************************************************************************
// Function prototypes
// ******************************************************************************
PROCESS(aggregator_proc, "Main process for aggregator module");
PROCESS(aggregator_measurement_proc, "Measurement process for aggregator module");
uint32_t measurement_finalize(void);

// ******************************************************************************
// Function definitions
// ******************************************************************************
void aggregator_init(void)
{
    aggregator_event = process_alloc_event();

    measurement_config.n_samples = 0;
    current_sample_index = 0;

    process_start(&aggregator_proc, NULL);
}

void aggregator_stop(void)
{
    process_exit(&aggregator_proc);
    process_exit(&aggregator_measurement_proc);
}

int aggregator_measurement_start(const aggregator_measurement_config_t* config)
{
    if (config->n_samples > AGGREGATOR_MAX_N_SAMPLES) {
        return -1;
    }

    if (process_is_running(&aggregator_measurement_proc)) {
        process_exit(&aggregator_measurement_proc);
    }

    measurement_config = *config;
    current_sample_index = 0;

    process_start(&aggregator_measurement_proc, NULL);

    return 0;
}

void aggregator_measurement_add(uint32_t sample)
{
    samples[current_sample_index++] = sample;

    if (current_sample_index >= measurement_config.n_samples) {
        /* Static because posting an event that may be asynchronous. */
        static aggregator_event_data_t result_event_data;

        result_event_data.type = AGGREGATOR_EVENT_MEASUREMENT_DONE;
        result_event_data.measurement_value = measurement_finalize();

        event_dispatch_post(aggregator_event, &result_event_data);
    }
}

// ******************************************************************************
// Internal functions
// ******************************************************************************
PROCESS_THREAD(aggregator_proc, ev, data)
{
    PROCESS_BEGIN();
    PROCESS_END();
}

PROCESS_THREAD(aggregator_measurement_proc, ev, data)
{
    static struct etimer timer;

    PROCESS_BEGIN();

    etimer_set(&timer, CLOCK_SECOND * measurement_config.sample_period_ms / 1000);

    while (current_sample_index < measurement_config.n_samples) {
        /* Event data must be static, because accessed asynchronously. */
        static aggregator_event_data_t aggregator_event_data = {
            .type = AGGREGATOR_EVENT_SAMPLE_REQUEST,
        };
        event_dispatch_post(aggregator_event, &aggregator_event_data);

        PROCESS_YIELD_UNTIL(etimer_expired(&timer));
        etimer_reset(&timer);
    }

    PROCESS_END();
}

uint32_t measurement_finalize(void)
{
    uint64_t sum = 0;
    for (int i = 0; i < measurement_config.n_samples; i++) {
        sum += samples[i];
    }
    return sum / measurement_config.n_samples;
}
