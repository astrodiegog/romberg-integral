SHELL = /usr/bin/env bash

#-- Set default include makefile
TYPE    ?= default
HOST 	?= dag

#-- Set the stuffix 
SUFFIX ?= .$(TYPE).$(HOST)

#-- Load any flags from TYPE & HOST
include builds/make.type.$(TYPE)
include builds/make.host.$(HOST)

#-- Directories to: find source files, build objects, and place binary
SRC_DIRS := src
BUILD_DIR := build
BIN_DIR := bin

#-- Place all c files into a var
CFILES := $(foreach DIR,$(SRC_DIRS),$(wildcard $(DIR)/*.c))

#-- Prepend BUILD_DIR and appends .o to all source C files
CLEAN_OBJS   := $(CFILES:%=$(BUILD_DIR)/%.o)

#-- In the case any CFILES changes
OBJS   := $(CFILES:%=$(BUILD_DIR)/%.o)

#-- Set default compiler and flags
CC               ?= CC
CPPFLAGS_OPTIMIZE ?= -g -Ofast -std=gnu17
CPPFLAGS_DEBUG    ?= -g -O0 -std=gnu17

BUILD             ?= OPTIMIZE

#-- Grab the appropriate CPPFLAGS
CPPFLAGS          += $(CPPFLAGS_$(BUILD))

#-- Add flags and libraries as needed
CPPFLAGS += $(DFLAGS) -Isrc

#-- Define the executable
EXEC := bin/romberg$(SUFFIX)

#-- Label flags
MACRO_FLAGS := -DMACRO_FLAGS='"$(DFLAGS)"'
DFLAGS      += $(MACRO_FLAGS)

#-- Define the link editor & flags
LD := $(CC)
LDFLAGS := $(CPPFLAGS)

#-- Execute the target executable once prereq-build is set
$(EXEC): prereq-build $(OBJS)
	echo "We are linking tings!"
	echo $(LDFLAGS)
	mkdir -p bin/ && $(LD) $(LDFLAGS) $(OBJS) -o $(EXEC) $(LIBS)

#-- Build step for the C source code
$(BUILD_DIR)/%.c.o: %.c
	echo "We are building object files!"
	mkdir -p $(dir $@)
	# echo $(CC)
	$(CC) $(CPPFLAGS) -c $< -o $@

foo += more

.PHONY: clean

all: 
	echo $(foo)
	echo $(DFLAGS)
	echo $(CFILES)
	echo $(OBJS)
	echo $(OBJS1)


prereq-build:
	echo "We are in prereq-build!"
	builds/prereq.sh build $(HOST)


clean:
	rm -f $(CLEAN_OBJS)
	rm -r $(BUILD_DIR)
