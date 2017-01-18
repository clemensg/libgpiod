#
# Copyright (C) 2017 Bartosz Golaszewski <bartekgola@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#

AM_CFLAGS = -I$(top_srcdir)/include/ -include $(top_srcdir)/config.h
AM_CFLAGS += -Wall -Wextra -g -D_GNU_SOURCE -L$(top_srcdir)/src/lib
AM_LDFLAGS = -lgpiod
DEPENDENCIES = libgpiod.la

bin_PROGRAMS = gpioctld

gpioctld_SOURCES = gpioctld.c
