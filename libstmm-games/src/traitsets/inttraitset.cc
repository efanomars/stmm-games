/*
 * File:   inttraitset.cc
 *
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
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

#include "traitsets/inttraitset.h"

#include <iostream>
#include <string>

namespace stmg
{

void IntTraitSet::dump(int32_t
#ifndef NDEBUG
nIndentSpaces
#endif //NDEBUG
, bool
#ifndef NDEBUG
bHeader
#endif //NDEBUG
) const noexcept
{
	#ifndef NDEBUG
	if (bHeader) {
		auto sIndent = std::string(nIndentSpaces, ' ');
		std::cout << sIndent << "IntTraitSet" << '\n';
	}
	m_oIntSet.dump(nIndentSpaces + 2, false);
	#endif //NDEBUG
}

} // namespace stmg
