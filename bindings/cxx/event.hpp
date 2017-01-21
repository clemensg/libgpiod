/*
 * C++ bindings for libgpiod.
 *
 * Copyright (C) 2017 Bartosz Golaszewski <bartekgola@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#ifndef __GPIOD_CXX_EVENT_HPP__
#define __GPIOD_CXX_EVENT_HPP__

#include "gpiod-internal.hpp"

namespace gpiod {

class event
{
public:

	event(void) throw();

	event(unsigned int sec, unsigned int nsec, int event_type) throw();

	event(const event& other) throw();

	unsigned int sec(void) throw();

	void set_sec(unsigned int sec) throw();

	unsigned int nsec(void) throw();

	void set_nsec(unsigned int nsec) throw();

	int type(void) throw();

	void set_type(int type) throw();

	enum {
		EVENT_RISING_EDGE,
		EVENT_FALLING_EDGE,
	};

private:

	::gpiod_line_event _m_event;
};

} /* namespace gpiod */

#endif /* __GPIOD_CXX_EVENT_HPP__ */
