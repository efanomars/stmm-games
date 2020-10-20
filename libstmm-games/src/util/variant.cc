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
 * File:   variant.cc
 */

#include "util/variant.h"

#include <iostream>
//#include <cassert>
#include <string>

namespace stmg
{

#ifndef NDEBUG
void Variant::dump(int32_t nIndentSpaces) const noexcept
{
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent;
	std::cout << "Variant{";
	if (m_eType == Variant::TYPE_NULL) {
		std::cout << "NULL";
	} else if (m_eType == Variant::TYPE_BOOL) {
		std::cout << ((m_nValue != 0) ? "True" : "False");
	} else if (m_eType == Variant::TYPE_INT) {
		std::cout << m_nValue;
	} else {
		std::cout << "!ERROR!";
	}
	std::cout << "}" << '\n';
}
#endif //NDEBUG

} // namespace stmg
