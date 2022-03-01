# Mira Event Driven


This is an example implementation of an event-driven architecture. Its goal is to support the implementation of modules with little to no interconnection, thus allowing for reusability, testability, and readability.

## The application

This application implements three modules:

- Manager starts measurements
- Sampler takes measurement samples
- Aggregator collects data from Sampler and condition it, before reporting to the Manager

Important point: the modules themselves do not know about one another. They do not include header files of other modules.

Another module, `event_dispatch` is what binds the modules into an application. Another application (whether it be another product, or a unit test, or a simulation) may implement another way to connect the modules, with another `event_dispatch` implementation.