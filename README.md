# Mira Event Driven


This is an example implementation of an event-driven architecture. Its goal is to support the implementation of modules with little to no interconnection, thus allowing for reusability, testability, and readability.

## The application

This application implements three modules:

- Manager starts measurements, and display them.
- Sampler generates measurement samples
- Aggregator collects data from Sampler and condition it, before reporting to the Manager

Important point: the modules themselves do not know about one another. They do not include header files of other modules.

Another module, `event_dispatch` binds the modules into an application.

Other applications (whether it be another product, or a unit test, or a simulation) may implement other connections between the modules, with another `event_dispatch` implementation.

## Reading order

### Sampler

The simplest module to begin reading is `sampler`. Observe how it:
- owns its event
- defines a type for the data to join to the event
- has only functions for API
- only includes `event_dispatch.h`, no other modules
- publishes an event, with joined data, when done

### Manager

The module `manager` is of similar complexity, but also takes a config for its init function. This module handles printing messages to the UART output.

Notice the use of anonymous `union` and `struct` in the event data type.

### Aggregator

The module `aggregator` publishes two types of event: measurement request, and measurement done. It does not know who consumes these events, as it shouldn't.

Notice how the event data uses anonymous `union` to save memory.

### Event dispatch

See `event_dispatch_proc` and its sub-functions to see the connections between event and action. The sub-functions interpret the data according to the type of event, then call to the relevant functions.

Note how the `sampler` event `SAMPLER_EVENT_DONE` connects to two actions:
- add measurement to the aggregator
- display progress via the manager