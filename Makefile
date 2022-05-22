# Copyright 2015 Samsung Austin Semiconductor, LLC.
#           2020 Zach Carmichael

# Description: Makefile for building a cbp 2016 submission.
#
# Note - g++ and boost libraries needed to be installed (run/see scripts/setup_cbp16.sh
#   for installation)
#  installed it in /usr/include/boost
#  lib at /usr/lib/x86_64-linux-gnu/
#  locate boost | grep lib
SHELL       := /bin/bash
# Include path to Boost area on local machine
#worked with both CENTOS, Ubuntu and Arch: export BOOST    := /usr/
BOOST       := /CBP-16-Simulation/cbp16sim/boost_1_67_0
ifndef BOOST
$(error "You must define BOOST")
endif

PYTHON      := $(shell bash get_python_basename.sh)
CXX         := g++

SRCDIR_PY   := src/simpython
SRCDIR_LG   := src/simnlog
SRCDIR_PS   := src/parser
COMMONDIR   := src/common
OBJDIR      := obj
OBJDIR_PY   := obj/simpython
OBJDIR_LG   := obj/simnlog
OBJDIR_PS   := obj/parser

SRC_PY      := $(wildcard $(SRCDIR_PY)/*.cc)
SRC_LG      := $(wildcard $(SRCDIR_LG)/*.cc)
SRC_PS      := $(wildcard $(SRCDIR_PS)/*.cc)
OBJ_PY      := $(SRC_PY:$(SRCDIR_PY)/%.cc=$(OBJDIR_PY)/%.o)
OBJ_LG      := $(SRC_LG:$(SRCDIR_LG)/%.cc=$(OBJDIR_LG)/%.o)
OBJ_PS      := $(SRC_PS:$(SRCDIR_PS)/%.cc=$(OBJDIR_PS)/%.o)
OBJ         := $(OBJ_PY) $(OBJ_LG) $(OBJ_PS)

LDLIBS      += -lboost_iostreams
LDFLAGS_LG  += -L$(BOOST)/lib -Wl,-rpath $(BOOST)/lib
LDFLAGS_PS  += -L$(BOOST)/lib -Wl,-rpath $(BOOST)/lib
LDFLAGS_PY  := $(LDFLAGS_LG) -l$(PYTHON)

CPPFLAGS    := -O3 -Wall -std=c++11 -Wextra -Winline -Winit-self -Wno-sequence-point \
               -Wno-unused-function -Wno-inline -fPIC -W -Wcast-qual -Wpointer-arith -Woverloaded-virtual \
               -I$(COMMONDIR) -I/usr/include -I/user/include/boost/ -I/usr/include/boost/iostreams/ \
               -I/usr/include/boost/iostreams/device/
CPPFLAGS_PY := $(CPPFLAGS) -I/usr/include/$(PYTHON)/
CPPFLAGS_LG := $(CPPFLAGS) -I$(SRCDIR_LG)
CPPFLAGS_PS := $(CPPFLAGS) -I$(SRCDIR_PS)

PROGRAMS    := simpython simnlog

.PHONY: all clean

all: $(PROGRAMS)

simpython: $(OBJ_PY)
	$(CXX) $(LDFLAGS_PY) $^ $(LDLIBS) -o $@

simnlog: $(OBJ_LG)
	$(CXX) $(LDFLAGS_LG) $^ $(LDLIBS) -o $@

parser: $(OBJ_PS)
	$(CXX) $(LDFLAGS_PS) $^ $(LDLIBS) -o $@

$(OBJDIR_PY)/%.o: $(SRCDIR_PY)/%.cc | $(OBJDIR_PY)
	$(CXX) $(CPPFLAGS_PY) -c $< -o $@

$(OBJDIR_LG)/%.o: $(SRCDIR_LG)/%.cc | $(OBJDIR_LG)
	$(CXX) $(CPPFLAGS_LG) -c $< -o $@

$(OBJDIR_PS)/%.o: $(SRCDIR_PS)/%.cc | $(OBJDIR_PS)
	$(CXX) $(CPPFLAGS_PS) -c $< -o $@

$(OBJDIR_PY): $(OBJDIR)
	mkdir $@

$(OBJDIR_LG): $(OBJDIR)
	mkdir $@

$(OBJDIR_PS): $(OBJDIR)
	mkdir $@

$(OBJDIR):
	mkdir $@

dbg: clean
	$(MAKE) DBG_BUILD=1 all

clean:
	$(RM) $(OBJ)
