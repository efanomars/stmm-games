/*
 * File:   volatilewidget.cc
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

#include "widgets/volatilewidget.h"

#include "gameproxy.h"
#include "variable.h"

#include <cassert>
#include <iostream>
#include <string>

namespace stmg
{

const Variable& VolatileWidget::variable() const noexcept
{
	return *m_p0Variable;
}
void VolatileWidget::reInitCommon() noexcept
{
	assert(m_oData.m_nVarId >= 0);
	if (m_oData.m_eVarOwnerType != OwnerType::GAME) {
		assert(getTeam() >= 0);
		if (m_oData.m_eVarOwnerType == OwnerType::PLAYER) {
			assert(getMate() >= 0);
		}
	}
	assert(! m_oData.m_aValueImgs.empty());
	#ifndef NDEBUG
	for (const ValueImg& oValueImg : m_oData.m_aValueImgs) {
		assert(oValueImg.m_oValues.size() > 0);
	}
	#endif //NDEBUG
	assert(getChildren().size() == 1);

	m_nOlderCachedValue = 1;
}
void VolatileWidget::onAddedToGame() noexcept
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
	m_p0Variable = &(game().variable(m_oData.m_nVarId, nLevel, nLevelTeam, nMate));
}
int32_t VolatileWidget::calcImgIdxFromValue(int32_t nValue) const noexcept
{
	const std::vector<VolatileWidget::ValueImg>& aValueImgs = m_oData.m_aValueImgs;
	int32_t nImgIdx = 0;
	for (const auto& oValueImg : aValueImgs) {
		const int32_t nIdx = oValueImg.m_oValues.getIndexOfValue(nValue);
		if (nIdx >= 0) {
			return nImgIdx; //--------------------------------------------------
		}
		++nImgIdx;
	}
	return -1;
}
int32_t VolatileWidget::calcImgIdFromIdx(int32_t nImgIdx) const noexcept
{
	if (nImgIdx < 0) {
		return m_oData.m_nDefaultImgId;
	}
	const std::vector<VolatileWidget::ValueImg>& aValueImgs = m_oData.m_aValueImgs;
	return aValueImgs[nImgIdx].m_nImgId;
}

bool VolatileWidget::isChanged(int32_t& nNewCachedValue) const noexcept
{
	const auto& oGame = game();
	const int32_t nCurTick = oGame.gameElapsed() - (oGame.isInGameTick() ? 0 : 1);
	{
	CachedValue& oNewCached = m_aCachedValues[1 - m_nOlderCachedValue];
	if (nCurTick == oNewCached.m_nGameTick) {
		nNewCachedValue = 1 - m_nOlderCachedValue;
		const CachedValue& oOldCached = m_aCachedValues[m_nOlderCachedValue];
		if (oOldCached.m_nGameTick < 0) {
			return true; //-----------------------------------------------------
		}
		return (oNewCached.m_nImgId != oOldCached.m_nImgId); //-----------------
	}
	}
	nNewCachedValue = m_nOlderCachedValue;
	m_nOlderCachedValue = 1 - m_nOlderCachedValue;
	CachedValue& oNewCached = m_aCachedValues[1 - m_nOlderCachedValue];
	oNewCached.m_nGameTick = nCurTick;
	oNewCached.m_nValue = variable().get();
	const CachedValue& oOldCached = m_aCachedValues[m_nOlderCachedValue];
	if (oOldCached.m_nGameTick < 0) {
		oNewCached.m_nIdx = calcImgIdxFromValue(oNewCached.m_nValue);
		oNewCached.m_nImgId = calcImgIdFromIdx(oNewCached.m_nIdx);
		return true; //---------------------------------------------------------
	}
	if (oNewCached.m_nValue == oOldCached.m_nValue) {
		oNewCached.m_nIdx = oOldCached.m_nIdx;
		oNewCached.m_nImgId = oOldCached.m_nImgId;
		return false; //--------------------------------------------------------
	}
	oNewCached.m_nIdx = calcImgIdxFromValue(oNewCached.m_nValue);
	if (oNewCached.m_nIdx == oOldCached.m_nIdx) {
		oNewCached.m_nImgId = oOldCached.m_nImgId;
		return false; //--------------------------------------------------------
	}
	oNewCached.m_nImgId = calcImgIdFromIdx(oNewCached.m_nIdx);
	if (oNewCached.m_nImgId == oOldCached.m_nImgId) {
		return false; //--------------------------------------------------------
	}
	return true;
}
bool VolatileWidget::isChanged() const noexcept
{
	int32_t nNewCachedValue;
	return isChanged(nNewCachedValue);
}
int32_t VolatileWidget::getCurrentImgId() const noexcept
{
	int32_t nNewCachedValue;
	isChanged(nNewCachedValue);
	const CachedValue& oNewCached = m_aCachedValues[nNewCachedValue];
	return oNewCached.m_nImgId;
}

void VolatileWidget::dump(int32_t
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
		std::cout << sIndent << "VolatileWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	ContainerWidget::dump(nIndentSpaces + 2, false);
	std::cout << sIndent << "  " << "m_nVarId:     " << m_oData.m_nVarId << '\n';
	#endif //NDEBUG
}

} // namespace stmg
