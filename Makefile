EMACS_ROOT ?= ../..
EMACS ?= emacs
JULIA_DIR ?= /usr/local

CC      = gcc
LD      = gcc
JULIA_CFLAGS = $(shell $(JULIA_DIR)/bin/julia $(JULIA_DIR)/share/julia/julia-config.jl --cflags)
CPPFLAGS = -I$(EMACS_ROOT)/src
CFLAGS = -std=gnu99 -ggdb3 -Wall -fPIC $(CPPFLAGS) $(JULIA_CFLAGS)
JULIA_LDFLAGS = $(shell $(JULIA_DIR)/bin/julia $(JULIA_DIR)/share/julia/julia-config.jl --ldflags)
JULIA_LDLIBS = $(shell $(JULIA_DIR)/bin/julia $(JULIA_DIR)/share/julia/julia-config.jl --ldlibs)
LDFLAGS = $(JULIA_LDFLAGS)

.PHONY : test

all: julia-core.so

julia-core.so: julia-core.o
	$(LD) -shared $(LDFLAGS) -o $@ $^ -ljulia

julia-core.o: julia-core.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f julia-core.so julia-core.o

test:
	$(EMACS) -Q -batch -L . $(LOADPATH) \
		-l test/test.el \
		-f ert-run-tests-batch-and-exit
