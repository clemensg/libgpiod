/*
 * C++ bindings for libgpiod.
 *
 * Copyright (C) 2017 Bartosz Golaszewski <bartekgola@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include "line.hpp"

#include <stdexcept>

namespace gpiod {

line::line(void) : _m_line(0), _m_chip(0)
{

}

line::~line(void) throw()
{

}

unsigned int line::offset(void)
{
	return gpiod_line_offset(this->_m_line);
}

const char * line::name(void)
{
	return gpiod_line_name(this->_m_line);
}

const char * line::consumer(void)
{
	return gpiod_line_consumer(this->_m_line);
}

int line::direction(void)
{
	int direction;

	direction = gpiod_line_direction(this->_m_line);

	switch (direction) {
	case GPIOD_DIRECTION_INPUT:
		return line::DIRECTION_INPUT;
	case GPIOD_DIRECTION_OUTPUT:
		return line::DIRECTION_OUTPUT;
	default:
		throw std::logic_error("invalid direction");
	}
}

int line::active_state(void)
{
	int active;

	active = gpiod_line_active_state(this->_m_line);

	switch (active) {
	case GPIOD_ACTIVE_STATE_HIGH:
		return line::ACTIVE_STATE_HIGH;
	case GPIOD_ACTIVE_STATE_LOW:
		return line::ACTIVE_STATE_LOW;
	default:
		throw std::logic_error("invalid active state");
	}
}

bool line::used_by_kernel(void)
{
	return gpiod_line_is_used_by_kernel(this->_m_line);
}

bool line::open_drain(void)
{
	return gpiod_line_is_open_drain(this->_m_line);
}

bool line::open_source(void)
{
	return gpiod_line_is_open_source(this->_m_line);
}

void line::update(void)
{
	int status;

	status = gpiod_line_update(this->_m_line);
	if (status < 0)
		throw gpiod::error(gpiod_errno());
}

bool line::needs_update(void)
{
	return gpiod_line_needs_update(this->_m_line);
}

void line::request(const std::string& consumer,
		   int direction, bool active_low, int flags, int default_val)
{
	::gpiod_line_request_config config;
	int status;

	config.consumer = consumer.c_str();

	switch (direction) {
	case DIRECTION_INPUT:
		config.direction = GPIOD_DIRECTION_INPUT;
		break;
	case DIRECTION_OUTPUT:
		config.direction = GPIOD_DIRECTION_OUTPUT;
		break;
	case DIRECTION_AS_IS:
		config.direction = GPIOD_DIRECTION_AS_IS;
		break;
	default:
		throw std::logic_error("invalid direction");
	}

	if (active_low)
		config.active_state = GPIOD_ACTIVE_STATE_LOW;
	else
		config.active_state = GPIOD_ACTIVE_STATE_HIGH;

	if (flags & OPEN_DRAIN)
		config.flags = GPIOD_REQUEST_OPEN_DRAIN;
	if (flags & OPEN_SOURCE)
		config.flags = GPIOD_REQUEST_OPEN_SOURCE;

	status = gpiod_line_request(this->_m_line, &config, default_val);
	if (status < 0)
		throw gpiod::error(gpiod_errno());
}

int line::value(void)
{
	int val;

	val = gpiod_line_get_value(this->_m_line);
	if (val < 0)
		throw gpiod::error(gpiod_errno());

	return val;
}

void line::value(int val)
{
	int status;

	status = gpiod_line_set_value(this->_m_line, val);
	if (status < 0)
		throw gpiod::error(gpiod_errno());
}

void line::request_event(const std::string& consumer,
		   int event_type, int active_state, int flags)
{

}

bool line::event_wait(unsigned int sec, unsigned int nsec)
{

}

void line::event_read(event& ev)
{

}

int line::get_fd(void)
{

}

void line::release(void)
{

}

bool line::reserved(void)
{

}

bool line::monitored(void)
{

}

bool line::free(void)
{

}

gpiod::chip * line::chip(void)
{

}

} /* namespace gpiod */
