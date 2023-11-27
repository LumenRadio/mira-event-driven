/*----------------------------------------------------------------------------
Copyright (c) 2022 LumenRadio AB
This code is the property of Lumenradio AB and may not be redistributed in any
form without prior written permission from LumenRadio AB.
----------------------------------------------------------------------------*/

#include "sampler.h"

#include <mira.h>
#include <stdint.h>

#include "event_dispatch.h"

// ******************************************************************************
// Module variables
// ******************************************************************************
static const uint32_t sample_delay_ms = 50;

// ******************************************************************************
// Module variables
// ******************************************************************************
process_event_t sampler_event;

// ******************************************************************************
// Function prototypes
// ******************************************************************************
PROCESS(sampler_measurement_proc, "Sampler measurement process");

// ******************************************************************************
// Function definitions
// ******************************************************************************
void sampler_init(void)
{
    sampler_event = process_alloc_event();
}

void sampler_stop(void)
{
    process_exit(&sampler_measurement_proc);
}

void sampler_measurement_start(void)
{
    if (process_is_running(&sampler_measurement_proc)) {
        return;
    }
    process_start(&sampler_measurement_proc, NULL);
}

// ******************************************************************************
// Internal functions
// ******************************************************************************
PROCESS_THREAD(sampler_measurement_proc, ev, data)
{
    static struct etimer timer;
    PROCESS_BEGIN();

    /* Fake the amount of time for measuring a sample */
    etimer_set(&timer, CLOCK_SECOND * sample_delay_ms / 1000);
    PROCESS_YIELD_UNTIL(etimer_expired(&timer));

    /* Fake a sample value */
    uint32_t sample = 995 + mira_random_generate() % 10;

    /* Static because the event may be asynchronous */
    static sampler_event_data_t event_data;
    event_data = (sampler_event_data_t){ .type = SAMPLER_EVENT_DONE, .sample_value = sample };

    event_dispatch_post(sampler_event, &event_data);

    PROCESS_END();
}
