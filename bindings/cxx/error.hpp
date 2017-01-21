/*
 * C++ bindings for libgpiod.
 *
 * Copyright (C) 2017 Bartosz Golaszewski <bartekgola@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#ifndef __GPIOD_CXX_ERROR_HPP__
#define __GPIOD_CXX_ERROR_HPP__

#include "gpiod-internal.hpp"

#include <stdexcept>

namespace gpiod {

class error : public std::runtime_error
{
public:

	error(int errnum);

	int errnum(void) const throw();

private:

	int _m_errnum;
};

} /* namespace gpiod */

#endif /* __GPIOD_CXX_ERROR_HPP__ */
