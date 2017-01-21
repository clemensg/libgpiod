/*
 * C++ bindings for libgpiod.
 *
 * Copyright (C) 2017 Bartosz Golaszewski <bartekgola@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include "chip.hpp"

namespace gpiod {

chip::chip(void) : _m_chip(0)
{

}

chip::~chip(void) throw()
{
	if (this->_m_chip)
		::gpiod_chip_close(this->_m_chip);
}

} /* namespace gpiod */
