/*
 * C++ bindings for libgpiod.
 *
 * Copyright (C) 2017 Bartosz Golaszewski <bartekgola@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#include "error.hpp"

namespace gpiod {

error::error(int errnum)
	: std::runtime_error(gpiod_strerror(errnum)),
	  _m_errnum(errnum)
{

}

int error::errnum(void) const throw()
{
	return this->_m_errnum;
}

} /* namespace gpiod */
