/*
 * C++ bindings for libgpiod.
 *
 * Copyright (C) 2017 Bartosz Golaszewski <bartekgola@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#ifndef __GPIOD_CXX_LINE_HPP__
#define __GPIOD_CXX_LINE_HPP__

#include "gpiod-internal.hpp"
#include "event.hpp"

#include <string>

namespace gpiod {

class chip;

class line
{
public:

	line(void);

	~line(void) throw();

	unsigned int offset(void);

	const char * name(void);

	const char * consumer(void);

	int direction(void);

	int active_state(void);

	bool used_by_kernel(void);

	bool open_drain(void);

	bool open_source(void);

	void update(void);

	bool needs_update(void);

	void request(const std::string& consumer,
		     int direction, int active_state, int flags);

	int value(void);

	void value(int val);

	void request_event(const std::string& consumer, int event_type,
			   bool active_low, int flags, int default_val);

	bool event_wait(unsigned int sec, unsigned int nsec);

	void event_read(event& ev);

	int get_fd(void);

	void release(void);

	bool reserved(void);

	bool monitored(void);

	bool free(void);

	gpiod::chip * chip(void);

	enum {
		DIRECTION_AS_IS,
		DIRECTION_INPUT,
		DIRECTION_OUTPUT,
	};

	enum {
		ACTIVE_STATE_HIGH,
		ACTIVE_STATE_LOW,
	};

	enum {
		OPEN_DRAIN = GPIOD_BIT(0),
		OPEN_SOURCE = GPIOD_BIT(1),
	};

	enum {
		EVENT_RISING_EDGE,
		EVENT_FALLING_EDGE,
		EVENT_BOTH_EDGES,
	};

	static void event_read_fd(line& line, event& ev);

private:

	::gpiod_line *_m_line;
	gpiod::chip *_m_chip;
};

} /* namespace gpiod */

#endif /* __GPIOD_CXX_LINE_HPP__ */
