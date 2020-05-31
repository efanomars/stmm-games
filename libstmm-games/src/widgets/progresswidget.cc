/*
 * File:   progresswidget.cc
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

#include "widgets/progresswidget.h"

#include "variable.h"

#include <iostream>
#include <string>

namespace stmg
{

bool ProgressWidget::isChanged() const noexcept
{
	const Variable& oVar = variable();
	return oVar.isChanged();
}

void ProgressWidget::dump(int32_t
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
		std::cout << sIndent << "ProgressWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	RelSizedGameWidget::dump(nIndentSpaces + 2, false);
	std::cout << sIndent << "  " << "m_nMinValue:       " << m_oData.m_nMinValue << '\n';
	std::cout << sIndent << "  " << "m_nMaxValue:       " << m_oData.m_nMaxValue << '\n';
	std::cout << sIndent << "  " << "m_nThresholdValue: " << m_oData.m_nThresholdValue << '\n';
	std::cout << sIndent << "  " << "m_bDangerBelow:    " << m_oData.m_bDangerBelow << '\n';
	std::cout << sIndent << "  " << "m_nVariableId:     " << m_oData.m_nVariableId << '\n';
	#endif //NDEBUG
}

} // namespace stmg
