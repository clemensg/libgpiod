#
# Copyright (C) 2017 Bartosz Golaszewski <bartekgola@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#

lib_LTLIBRARIES = libgpiodcxx.la
libgpiodcxx_la_SOURCES = chip.cpp line.cpp event.cpp error.cpp
libgpiodcxx_la_CPPFLAGS = -Wall -Wextra -g -D_GNU_SOURCE
libgpiodcxx_la_CPPFLAGS += -fvisibility=hidden -I$(top_srcdir)/include/
libgpiodcxx_la_CPPFLAGS += -include $(top_srcdir)/config.h
libgpiodcxx_la_LDFLAGS = -version-number $(subst .,:,$(PACKAGE_VERSION))
libgpiodcxx_la_LDFLAGS += -lgpiod -L$(top_srcdir)/src/lib
DEPENDENCIES = libgpiod.la