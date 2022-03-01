/*----------------------------------------------------------------------------
Copyright (c) 2022 LumenRadio AB
This code is the property of Lumenradio AB and may not be redistributed in any
form without prior written permission from LumenRadio AB.
----------------------------------------------------------------------------*/

/**
 * This file connects modules, by defining reactions to posted events.  Modules
 * post events describing what happened, by calling `event_dispatch_post()`. The
 * internal process makes sure that the appropriate actions happen accordingly.
 */

#include <mira.h>
#include <stdbool.h>
#include <stdio.h>

#include "event_dispatch.h"

#include "application_config.h"
#include "aggregator.h"
#include "manager.h"
#include "sampler.h"

// ******************************************************************************
// Module macros
// ******************************************************************************
/* Comment this out to turn off debug prints. */
// #define DEBUG_PRINT
#if defined(DEBUG_PRINT)
#define PRINT(...) printf(__VA_ARGS__)
#else
#define PRINT(...)
#endif

// ******************************************************************************
// Function prototypes
// ******************************************************************************
PROCESS(event_dispatch_proc,
    "React on events from modules, with actions in other modules");

/**
 * Return true for events that require taking action directly.
 */
static bool event_requires_synch_action(
    process_event_t event);

static void on_manager_event(
    void *data);
static void on_aggregator_event(
    void *data);
static void on_sampler_event(
    void *data);

// ******************************************************************************
// Function definitions
// ******************************************************************************
void event_dispatch_init(
    void)
{
    process_start(&event_dispatch_proc, NULL);
}

int event_dispatch_post(
    process_event_t event,
    process_data_t data)
{
    if (event_requires_synch_action(event)) {
        PRINT("%s: synch event dispatch: %d\n", __func__, event);
        process_post_synch(&event_dispatch_proc, event, data);
        return 0;
    }

    if (process_post(&event_dispatch_proc, event, data) != PROCESS_ERR_OK) {
        return -1;
    }
    PRINT("%s: event dispatch: %d\n", __func__, event);
    return 0;
}

// ******************************************************************************
// Internal functions
// ******************************************************************************
PROCESS_THREAD(event_dispatch_proc, ev, data)
{
    PROCESS_BEGIN();
    PROCESS_PAUSE();

    PRINT("%s: started\n", __func__);
    while (1) {
        PROCESS_YIELD();

        if (ev == manager_event) {
            on_manager_event(data);
        } else if (ev == aggregator_event) {
            on_aggregator_event(data);
        } else if (ev == sampler_event) {
            on_sampler_event(data);
        }
    }
    PROCESS_END();
}

static bool event_requires_synch_action(
    process_event_t event)
{
    /* List all events that require synchronous reaction. */
    return
        event == sampler_event;
}

static void on_manager_event(
    void *data)
{
    manager_event_data_t event_data = *(manager_event_data_t *) data;
    PRINT("%s: %d\n", __func__, event_data.type);

    switch (event_data.type) {
    case MANAGER_EVENT_MEASUREMENT_REQUEST:
        aggregator_measurement_start(&(aggregator_measurement_config_t) {
            .n_samples = event_data.number_of_samples,
            .sample_period_ms = event_data.sample_period_ms
        });
        break;

    default:
        PRINT("ERROR: %s: undefined event type (%d)\n", __func__, event_data.type);
    }
}

static void on_aggregator_event(
    void *data)
{
    aggregator_event_data_t event_data = *(aggregator_event_data_t *) data;
    PRINT("%s: %d\n", __func__, event_data.type);

    switch (event_data.type) {
    case AGGREGATOR_EVENT_SAMPLE_REQUEST:
        sampler_measurement_start();
        break;

    case AGGREGATOR_EVENT_MEASUREMENT_DONE:
        manager_result_display(event_data.measurement_value);
        break;

    case AGGREGATOR_EVENT_ERROR:
        manager_stop();
        sampler_stop();
        aggregator_stop();
        break;

    default:
        PRINT("ERROR: %s: undefined event type (%d)\n", __func__, event_data.type);
    }
}

static void on_sampler_event(
    void *data)
{
    sampler_event_data_t event_data = *(sampler_event_data_t *) data;
    PRINT("%s: %d\n", __func__, event_data.type);

    switch (event_data.type) {
    case SAMPLER_EVENT_DONE:
        aggregator_measurement_add(event_data.sample_value);
        manager_progress_display();
        break;

    case SAMPLER_EVENT_ERROR:
        manager_stop();
        sampler_stop();
        aggregator_stop();

        PRINT("Stop because of error!\n");
        break;

    default:
        PRINT("ERROR: %s: undefined event type (%d)\n", __func__, event_data.type);
    }
}
