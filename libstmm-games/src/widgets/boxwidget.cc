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
 * File:   boxwidget.cc
 */

#include "widgets/boxwidget.h"

#include <iostream>
#include <string>

namespace stmg
{

void BoxWidget::dump(int32_t
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
	auto sIndent = std::string(nIndentSpaces, ' ');
	if (bHeader) {
		std::cout << sIndent << "BoxWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	ContainerWidget::dump(nIndentSpaces + 2, false);
	for (int32_t nIdx = 0; nIdx < g_nTotLayoutConfigs; ++nIdx) {
		std::cout << sIndent << "  " << "m_aVertical[" << nIdx << "]:       " << m_oData.m_aVertical[nIdx] << '\n';
		std::cout << sIndent << "  " << "m_aInvertChildren[" << nIdx << "]: " << m_oData.m_aInvertChildren[nIdx] << '\n';
	}
	#endif //NDEBUG
}

} // namespace stmg
