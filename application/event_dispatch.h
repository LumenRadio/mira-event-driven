/*----------------------------------------------------------------------------
Copyright (c) 2022 LumenRadio AB
This code is the property of Lumenradio AB and may not be redistributed in any
form without prior written permission from LumenRadio AB.
----------------------------------------------------------------------------*/

#ifndef EVENT_DISPATCH_H
#define EVENT_DISPATCH_H

/**
 * In order to allow good modularity and event-drivenness, modules should only
 * publish events that describe what they control. Events should preferably
 * describe something that happened, not something that a module requires. This
 * ensures minimal binding between modules.
 *
 * The event_dispatch_proc process describes what must happen on any posted
 * event. This process (see the C file) is what binds modules together into a
 * product application.
 *
 * The event_dispatch_proc process must interpret the data pointer in accordance
 * with the module posting the event.
 *
 * Example:
 * module.h:
 * ```
 * #include "mira.h"
 * extern process_event_t my_event;
 *
 * typedef enum {
 *     NONE,
 *     EVENT_TYPE_1,
 *     EVENT_TYPE_2,
 * } my_event_type_t;
 *
 * typedef struct {
 *     my_event_type_t type;
 *     union {
 *         uint32_t data_for_event_type_1;
 *         uint16_t data_for_event_type_2;
 *     };
 * } my_event_data_t;
 * ```
 * module.c:
 * ```
 * #include "module.h"
 * process_event_t my_event;
 * void module_init(
 *     void)
 * {
 *     my_event = process_alloc_event():
 * }
 *
 * static my_event_data_t my_event_data;
 *
 * void module_function(
 *     void)
 * {
 *     my_event_data = (my_event_data_t) {
 *         .type = EVENT_TYPE_2,
 *         .data_for_event_type_2 = 42,
 *     };
 *     event_dispatch_post(
 *         my_event,
 *         &my_event_data
 *     );
 * }
 * ```
 * In `event_dispatch_proc`:
 * ```
 * if (ev == my_event) {
 *     my_event_data_t *my_event_data = (my_event_data_t *) data;
 *     if (my_event_data->type == EVENT_TYPE_2) {
 *         printf(
 *             "Received my_event type 2, payload: %d\n",
 *             my_event_data->data_for_event_type_2
 *         );
 *         // Take other relevant action
 *     }
 * }
 * ```
 */

#include <mira.h>

/**
 * Start receiving events, and reacting.
 */
void event_dispatch_init(
    void);

/**
 * Post an event to the dispatcher. The event dispatcher can make this reaction
 * synchronous (to make it happen right away). That decision is not for the
 * publisher of the event to make, but for the event dispatcher.
 *
 * @param event  The event. The caller module must allocate the event with
 * `process_alloc_event()` (probably at init), and make it available in its
 * header file with `extern`. If a module needs to post multiple types of
 * events, it may instead own a single event, and give more information in the
 * data.
 * @param data  Attached to the event. If a module needs to post multiple types
 * of events, data may contain a field that describes which sub-event actually
 * occurred. If there is no relevant data to attach, use NULL.
 * Note that the execution of the event dispatch process occurs later, you must
 * make sure that the memory area that `data` points to stays allocated.
 * @retval 0:  Success.
 * @retval < 0: the event could not be posted.
 */
int event_dispatch_post(
    process_event_t event,
    void *data);

#endif
