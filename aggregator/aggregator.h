/*----------------------------------------------------------------------------
Copyright (c) 2022 LumenRadio AB
This code is the property of Lumenradio AB and may not be redistributed in any
form without prior written permission from LumenRadio AB.
----------------------------------------------------------------------------*/

#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include <mira.h>
#include <stdint.h>

/* Infrastructure for event-driven framework */
typedef enum {
    AGGREGATOR_EVENT_SAMPLE_REQUEST,
    AGGREGATOR_EVENT_MEASUREMENT_DONE,
    AGGREGATOR_EVENT_ERROR
} aggregator_event_type_t;

typedef struct {
    aggregator_event_type_t type;
    union {
        /* AGGREGATOR_EVENT_MEASUREMENT_DONE */
        uint32_t measurement_value;

        /* AGGREGATOR_EVENT_ERROR */
        uint32_t error_type
    };
} aggregator_event_data_t;

extern process_event_t aggregator_event;

/* Module API */

void aggregator_init(
    void);

typedef struct {
    uint32_t n_samples;
    uint32_t sample_period_ms;
} aggregator_measurement_config_t;

/**
 * Return -1 in case of error, 0 otherwise.
 */
int aggregator_measurement_start(
    const aggregator_measurement_config_t *measurement_config);

void aggregator_reset(
    void);

void aggregator_stop(
    void);

void aggregator_measurement_add(
    uint32_t sample);

#endif
