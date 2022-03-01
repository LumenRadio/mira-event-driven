/*----------------------------------------------------------------------------
Copyright (c) 2022 LumenRadio AB
This code is the property of Lumenradio AB and may not be redistributed in any
form without prior written permission from LumenRadio AB.
----------------------------------------------------------------------------*/

#ifndef SAMPLER_H
#define SAMPLER_H

#include <mira.h>
#include <stdint.h>

extern process_event_t sampler_event;

typedef enum {
    SAMPLER_EVENT_DONE,
    SAMPLER_EVENT_ERROR,
} sampler_event_type_t;

typedef struct {
    sampler_event_type_t type;
    union {
        uint32_t sample_value;
    };
} sampler_event_data_t;

void sampler_init(
    void);

void sampler_stop(
    void);

void sampler_measurement_start(
    void);

#endif
