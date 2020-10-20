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
 * File:   backgroundwidget.cc
 */

#include "widgets/backgroundwidget.h"

#include <iostream>
#include <string>


namespace stmg
{

void BackgroundWidget::dump(int32_t
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
		std::cout << sIndent << "BackgroundWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	ContainerWidget::dump(nIndentSpaces + 2, false);
	std::cout << sIndent << "  " << "m_nImgId:     " << m_oData.m_nImgId << '\n';
	#endif //NDEBUG
}

} // namespace stmg
