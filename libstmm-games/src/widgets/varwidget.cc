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
 * File:   varwidget.cc
 */

#include "widgets/varwidget.h"

#include "gameproxy.h"
#include "variable.h"

#include <cassert>
#include <iostream>

namespace stmg
{

const Variable& VarWidget::variable() const noexcept
{
	int32_t nLevel = -1;
	int32_t nLevelTeam = -1;
	int32_t nMate = -1;
	if (m_oData.m_eVarOwnerType != OwnerType::GAME) {
		const int32_t nTeam = getTeam();
		const bool bATIOL = game().isAllTeamsInOneLevel();
		nLevel = (bATIOL ? 0 : nTeam);
		nLevelTeam = (bATIOL ? nTeam : 0);
		if (m_oData.m_eVarOwnerType == OwnerType::PLAYER) {
			nMate = getMate();
		}
	}
	return game().variable(m_oData.m_nVarId, nLevel, nLevelTeam, nMate);
}

bool VarWidget::isChanged() const noexcept
{
	//TODO the address is stable throughout a game
	//     no need to recalc each game tick
	//     implement recalc in virtual void onAddedToGame() noexcept
	const Variable& oVar = variable();
	return oVar.isChanged();
}

void VarWidget::checkParams() noexcept
{
	assert(m_oData.m_nVarId >= 0);
	if (m_oData.m_eVarOwnerType != OwnerType::GAME) {
		assert(getTeam() >= 0);
		if (m_oData.m_eVarOwnerType == OwnerType::PLAYER) {
			assert(getMate() >= 0);
		}
	}
	assert((m_oData.m_nTitleFactorPerc >= 0) && (m_oData.m_nTitleFactorPerc <= 100));
	assert(m_oData.m_nVarDigits > 0);
}

void VarWidget::dump(int32_t
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
		std::cout << sIndent << "VarWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	RelSizedGameWidget::dump(nIndentSpaces + 2, false);
	std::cout << sIndent << "  " << "m_sTitle:            " << m_oData.m_sTitle << '\n';
	std::cout << sIndent << "  " << "m_nTitleFactorPerc:  " << m_oData.m_nTitleFactorPerc << '\n';
	std::cout << sIndent << "  " << "m_bTitleBesideValue: " << m_oData.m_bTitleBesideValue << '\n';
	std::cout << sIndent << "  " << "m_nVarId:            " << m_oData.m_nVarId << '\n';
	std::cout << sIndent << "  " << "m_nVarDigits:        " << m_oData.m_nVarDigits << '\n';
	#endif //NDEBUG
}

} // namespace stmg
