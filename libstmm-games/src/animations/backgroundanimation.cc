/*
 * File:   backgroundanimation.cc
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

#include "animations/backgroundanimation.h"

#include "level.h"
#include "gameproxy.h"
#include "named.h"
#include "util/direction.h"
#include "util/namedindex.h"

//#include <iostream>
#include <cassert>

namespace stmg
{

BackgroundAnimation::BackgroundAnimation(const Init& oInit) noexcept
: LevelAnimation(oInit)
, m_nImgId(-1)
, m_nPosLastChangeTick(-1)
{
}
void BackgroundAnimation::reInit(const Init& oInit) noexcept
{
	LevelAnimation::reInit(oInit);
	m_nImgId = -1;
	m_oImgRelPos = FPoint{};
	m_oPosDelta = FPoint{};
	m_nPosLastChangeTick = -1;
}
void BackgroundAnimation::setImage(int32_t nImgId, const FSize& oNewSize, const FPoint& oImgRelPos) noexcept
{
	if (isActive()) {
		assert((nImgId == -1) || level().getNamed().images().isIndex(nImgId));
		assert(level().game().isInGameTick());
	}
	m_nImgId = nImgId;
	m_oImgRelPos = oImgRelPos;
	m_oImgSize = oNewSize;
}
void BackgroundAnimation::moveImageTo(const FPoint& oNewRelPos) noexcept
{
	if (!isActive()) {
		m_oPosDelta = FPoint{};
		m_nPosLastChangeTick = -1;
	} else {
		assert(level().game().isInGameTick());
		const int32_t nGameTick = level().game().gameElapsed();
		if (nGameTick != m_nPosLastChangeTick) {
			// We are in a new game tick, reset the game tick delta
			m_oPosDelta = FPoint{};
			m_nPosLastChangeTick = nGameTick;
		}
		m_oPosDelta.m_fX += oNewRelPos.m_fX - m_oImgRelPos.m_fX;
		m_oPosDelta.m_fY += oNewRelPos.m_fY - m_oImgRelPos.m_fY;
	}
	m_oImgRelPos = oNewRelPos;
}
FPoint BackgroundAnimation::getImageRelPos(int32_t nViewTick, int32_t nTotViewTicks) const noexcept
{
	assert((nViewTick >= 0) && (nViewTick < nTotViewTicks));
	assert(!level().game().isInGameTick());
	// final position
	FPoint oPos = m_oImgRelPos; //LevelAnimation::getPos();
	const int32_t nGameTick = level().game().gameElapsed() - 1;
	if (nGameTick != m_nPosLastChangeTick) {
		// stationary
		return oPos; //---------------------------------------------------------
	}
	// calc intermediate position
	// subtract the change added in the game tick and
	// add the nViewTick fraction
	oPos.m_fX += - m_oPosDelta.m_fX + 1.0 * nViewTick * m_oPosDelta.m_fX / nTotViewTicks;
	oPos.m_fY += - m_oPosDelta.m_fY + 1.0 * nViewTick * m_oPosDelta.m_fY / nTotViewTicks;
	return oPos; //-------------------------------------------------------------
}
void BackgroundAnimation::boardAfterScroll(Direction::VALUE eDir) noexcept
{
//std::cout << "BackgroundAnimation::boardPostScroll eDir=" << static_cast<int32_t>(eDir) << '\n';
	LevelAnimation::onScrolled(eDir);
	const int32_t nDx = Direction::deltaX(eDir);
	const int32_t nDy = Direction::deltaY(eDir);
	m_oImgRelPos.m_fX += nDx;
	m_oImgRelPos.m_fY += nDy;
}

} // namespace stmg
