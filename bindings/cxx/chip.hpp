/*
 * C++ bindings for libgpiod.
 *
 * Copyright (C) 2017 Bartosz Golaszewski <bartekgola@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 */

#ifndef __GPIOD_CXX_CHIP_HPP__
#define __GPIOD_CXX_CHIP_HPP__

#include "gpiod-internal.hpp"

#include <string>

namespace gpiod {

class chip
{
public:

	chip(void);

	chip(const std::string& device);

	chip(unsigned int number);

	~chip(void) throw();

	void open(const std::string& device);

	void open(unsigned int number);

private:

	::gpiod_chip *_m_chip;
};

} /* namespace gpiod */

#endif /* __GPIOD_CXX_CHIP_HPP__ */
