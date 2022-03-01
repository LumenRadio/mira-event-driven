PROJECT_NAME = mira_event_driven

TARGET ?= nrf52840ble-os
LIBDIR ?= $(CURDIR)/../../..

PROJ_DIR := $(shell git rev-parse --show-toplevel)

APPLICATION_DIR := $(PROJ_DIR)/application
AGGREGATOR_DIR := $(PROJ_DIR)/aggregator
MANAGER_DIR := $(PROJ_DIR)/manager
SAMPLER_DIR := $(PROJ_DIR)/sampler

CFLAGS += \
	-I $(APPLICATION_DIR) \
	-I $(AGGREGATOR_DIR) \
	-I $(MANAGER_DIR) \
	-I $(SAMPLER_DIR)

CFLAGS += -std=c99 -Wall -Werror

SOURCE_FILES = \
	$(APPLICATION_DIR)/main.c \
	$(APPLICATION_DIR)/event_dispatch.c \
	$(AGGREGATOR_DIR)/aggregator.c \
	$(MANAGER_DIR)/manager.c \
	$(SAMPLER_DIR)/sampler.c

include $(LIBDIR)/Makefile.include
