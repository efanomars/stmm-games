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
 * File:   relsizedgamewidget.cc
 */

#include "relsizedgamewidget.h"

#include <iostream>
#include <string>

namespace stmg
{


void RelSizedGameWidget::dump(int32_t
#ifndef NDEBUG
nIndentSpaces
#endif //NDEBUG
, bool /*bHeader*/) const noexcept
{
	#ifndef NDEBUG
	GameWidget::dump(nIndentSpaces, false);
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent << "m_fReferenceWFactor: " << m_oData.m_fReferenceWFactor << '\n';
	std::cout << sIndent << "m_fReferenceHFactor: " << m_oData.m_fReferenceHFactor << '\n';
	std::cout << sIndent << "m_fWHRatio:          " << m_oData.m_fWHRatio << '\n';
	#endif //NDEBUG
}

} // namespace stmg
