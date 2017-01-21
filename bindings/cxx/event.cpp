/*
 * C++ bindings for libgpiod.
 *
 * Copyright (C) 2017 Bartosz Golaszewski <bartekgola@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include "event.hpp"

namespace gpiod {

event::event(void) throw()
{
	this->_m_event.ts.tv_sec = 0;
	this->_m_event.ts.tv_nsec = 0;
	this->_m_event.event_type = 0;
}

event::event(unsigned int sec, unsigned int nsec, int event_type) throw()
{
	this->_m_event.ts.tv_sec = sec;
	this->_m_event.ts.tv_nsec = nsec;
	this->_m_event.event_type = event_type;
}

event::event(const event& other) throw()
{
	this->_m_event.ts.tv_sec = other._m_event.ts.tv_sec;
	this->_m_event.ts.tv_nsec = other._m_event.ts.tv_nsec;
	this->_m_event.event_type = other._m_event.event_type;
}

unsigned int event::sec(void) throw()
{
	return this->_m_event.ts.tv_sec;
}

void event::set_sec(unsigned int sec) throw()
{
	this->_m_event.ts.tv_sec = sec;
}

unsigned int event::nsec(void) throw()
{
	return this->_m_event.ts.tv_nsec;
}

void event::set_nsec(unsigned int nsec) throw()
{
	this->_m_event.ts.tv_nsec = nsec;
}

int event::type(void) throw()
{
	return this->_m_event.event_type;
}

void event::set_type(int type) throw()
{
	this->_m_event.event_type = type;
}


} /* namespace gpiod */
