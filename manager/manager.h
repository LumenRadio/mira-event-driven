/*----------------------------------------------------------------------------
Copyright (c) 2022 LumenRadio AB
This code is the property of Lumenradio AB and may not be redistributed in any
form without prior written permission from LumenRadio AB.
----------------------------------------------------------------------------*/

#ifndef MANAGER_H
#define MANAGER_H

#include <mira.h>

/* Event framework */
typedef enum
{
    MANAGER_EVENT_MEASUREMENT_REQUEST,
} manager_event_type_t;

typedef struct
{
    manager_event_type_t type;
    union
    {
        struct
        {
            uint32_t number_of_samples;
            uint32_t sample_period_ms;
        };
    };
} manager_event_data_t;

extern process_event_t manager_event;

/* Module API */

typedef struct
{
    uint32_t measurement_period_s;
} manager_config_t;

void manager_init(manager_config_t* config);

void manager_stop(void);

void manager_result_display(uint32_t value);

void manager_progress_display(void);

#endif
