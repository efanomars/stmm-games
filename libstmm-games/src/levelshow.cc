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
 * File:   levelshow.cc
 */

#include "levelshow.h"
#include "level.h"

#include "gameproxy.h"

#include <cassert>
//#include <iostream>


namespace stmg
{


FPoint LevelShow::getPos(int32_t nViewTick, int32_t nTotViewTicks) const noexcept
{
	assert(!m_p0Level->game().isInGameTick());
	if (m_p0Positioner == nullptr) {
		return m_oXY; //--------------------------------------------------------
	}
	const int32_t nGameElapsed = m_p0Level->game().gameElapsed() - 1;
	if ((m_nGameTickLastChanged == nGameElapsed)
			&& (m_nViewTickLastChanged == nViewTick)) {
		return m_oXY; //--------------------------------------------------------
	}
	FPoint oXY = m_p0Positioner->getPos(nViewTick, nTotViewTicks);
	if (!(oXY == m_oXY)) {
		LevelShow* p0This = const_cast<LevelShow*>(this);
		p0This->m_nGameTickLastChanged = nGameElapsed;
		p0This->m_nViewTickLastChanged = nViewTick;
		p0This->m_oXY = oXY;
	}
	return m_oXY;
}
FPoint LevelShow::getPos() const noexcept
{
	return m_oXY;
}
void LevelShow::setPos(const FPoint& oNewPos) noexcept
{
	double fX = oNewPos.m_fX;
	double fY = oNewPos.m_fY;
	if (fX > m_fShowMaxX) {
		fX = m_fShowMaxX;
	}
	if (fX < 0.0) {
		fX = 0.0;
	}
	if (fY > m_fShowMaxY) {
		fY = m_fShowMaxY;
	}
	if (fY < 0.0) {
		fY = 0.0;
	}
	FPoint oXY{fX, fY};
	if (m_p0Level == nullptr) {
		m_oXY = oXY;
		return; //--------------------------------------------------------------
	}
	assert(m_p0Level->game().isInGameTick());
	if (!(oXY == m_oXY)) {
		// Make sure it is seen as changed in the next view tick
		const int32_t nGameElapsed = m_p0Level->game().gameElapsed() - 1;
		m_nGameTickLastChanged = nGameElapsed;
		m_nViewTickLastChanged = -1;
		m_oXY = oXY;
	}
}
	//bool LevelShow::isChanged(int32_t nViewTick, int32_t nTotViewTicks) const
	//{
	//	assert(!m_p0Level->game().isInGameTick());
	//	const int32_t nGameElapsed = m_p0Level->game().gameElapsed() - 1;
	//	// check whether there is (still) a positioner
	//	if ((m_p0Positioner == nullptr) || m_refPositioner.expired()) {
	//		if (m_nGameTickLastChanged < nGameElapsed) {
	//			return false; //----------------------------------------------------
	//		}
	//		if ((m_nViewTickLastChanged == -1) && (nViewTick == 0)) {
	//			return true; //-----------------------------------------------------
	//		}
	//		return false; //--------------------------------------------------------
	//	}
	//	if ((m_nGameTickLastChanged == nGameElapsed) && (m_nViewTickLastChanged == nViewTick)) {
	//		return true; //---------------------------------------------------------
	//	}
	//	FPoint oXY = m_p0Positioner->getPos(nViewTick, nTotViewTicks);
	//	if (oXY == m_oXY) {
	//		return false; //--------------------------------------------------------
	//	}
	//	LevelShow* p0This = const_cast<LevelShow*>(this);
	//	p0This->m_oXY = oXY;
	//	p0This->m_nGameTickLastChanged = nGameElapsed;
	//	p0This->m_nViewTickLastChanged = nViewTick;
	//	return true;
	//}
bool LevelShow::isBoardPosVisible(double fX, double fY) const noexcept
{
	assert(m_p0Level != nullptr);
	assert(m_p0Level->game().isInGameTick());
	double fBaseX = m_oXY.m_fX;
	double fBaseY = m_oXY.m_fY;
	if (m_bIsSubshow) {
		const FPoint& oShowXY = m_p0Level->showGet().getPos();
		fBaseX += oShowXY.m_fX;
		fBaseY += oShowXY.m_fY;
	}
	if ((fX < fBaseX) || (fY < fBaseY)) {
		return false; //--------------------------------------------------------
	}
	if ((fX >= fBaseX + m_nShowW) || (fY >= fBaseY + m_nShowH)) {
		return false; //--------------------------------------------------------
	}
	return true;
}
FPoint LevelShow::getBoardPos(double fX, double fY) const noexcept
{
	assert(m_p0Level != nullptr);
	assert(m_p0Level->game().isInGameTick());
	double fBaseX = m_oXY.m_fX;
	double fBaseY = m_oXY.m_fY;
	if (m_bIsSubshow) {
		const FPoint& oShowXY = m_p0Level->showGet().getPos();
		fBaseX += oShowXY.m_fX;
		fBaseY += oShowXY.m_fY;
	}
	return FPoint{fX + fBaseX, fY + fBaseY};
}
void LevelShow::setPositioner(Positioner* p0Positioner) noexcept
{
	assert(p0Positioner != nullptr);
	m_p0Positioner = p0Positioner;
}
void LevelShow::resetPositioner() noexcept
{
	m_p0Positioner = nullptr;
}

} // namespace stmg
