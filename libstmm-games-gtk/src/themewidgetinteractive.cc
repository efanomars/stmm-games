/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   themewidgetinteractive.cc
 */

#include "themewidgetinteractive.h"

#include <cassert>
#include <iostream>
#include <string>

namespace stmg { class ThemeWidget; }

namespace stmg
{

ThemeWidgetInteractive::ThemeWidgetInteractive(ThemeWidget* p0TW, bool bInteractive) noexcept
: m_p0TW(p0TW)
, m_bInteractive(bInteractive)
{
	assert(p0TW != nullptr);
}

void ThemeWidgetInteractive::reInit(ThemeWidget* p0TW, bool bInteractive) noexcept
{
	assert(p0TW != nullptr);
	m_p0TW = p0TW;
	m_bInteractive = bInteractive;
}

#ifndef NDEBUG
void ThemeWidgetInteractive::dump(int32_t nIndentSpaces) const noexcept
{
	//ThemeWidget::dump(nIndentSpaces);
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent << "m_bInteractive: " << (m_bInteractive ? "T" : "F") << '\n';
}
#endif //NDEBUG

} // namespace stmg
