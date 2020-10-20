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
 * File:   levelanimation.cc
 */

#include "levelanimation.h"
#include "level.h"

#include "gameproxy.h"

#include <cassert>

namespace stmg
{

int32_t LevelAnimation::s_nId = 0;

const double LevelAnimation::s_fDurationUndefined = -1.0;
const double LevelAnimation::s_fDurationInfinity = 10000000000.0;

LevelAnimation::LevelAnimation(const Init& oInit) noexcept
: m_nId(++s_nId)
, m_nAnimationIdx(oInit.m_nAnimationNamedIdx)
, m_fDuration(oInit.m_fDuration)
, m_oPos(oInit.m_oPos)
, m_nZ(oInit.m_nZ)
, m_oSize(oInit.m_oSize)
, m_fStartedMillisec(-1.0)
, m_p0Level(nullptr)
, m_eRefSys(REFSYS_INVALID)
, m_p0Game(nullptr)
, m_nScrolledUnique(0)
{
	assert((m_fDuration >= 0.0) || (m_fDuration == s_fDurationUndefined));
	assert(m_nAnimationIdx >= -1);
}
void LevelAnimation::reInit(const Init& oInit) noexcept
{
	assert((m_fDuration >= 0.0) || (m_fDuration == s_fDurationUndefined));
	assert(m_nAnimationIdx >= -1);
	m_nId = ++s_nId;
	m_nAnimationIdx = oInit.m_nAnimationNamedIdx;
	m_fDuration = oInit.m_fDuration;
	m_oPos = oInit.m_oPos;
	m_nZ = oInit.m_nZ;
	m_oSize = oInit.m_oSize;
	m_fStartedMillisec = -1.0;
	m_p0Level = nullptr;
	m_p0Game = nullptr;
	m_eRefSys = REFSYS_INVALID;
	m_nScrolledUnique = 0;
}

double LevelAnimation::getElapsed(int32_t nViewTick, int32_t nTotViewTicks) const noexcept
{
	assert(isActive());
	assert((nViewTick >= 0) && (nViewTick < nTotViewTicks));
	assert(!m_p0Game->isInGameTick());
	const double fGameInterval = m_p0Game->gameInterval();
	// During a view tick, when this function is called, the game tick already has increased,
	// same for the elapsed game time in milliseconds. That's why the game interval is subtracted here.
	const double fGameMillisec = m_p0Game->gameElapsedMillisec() - fGameInterval;
	const double fElapsedMillisec = fGameMillisec + (0.0 + nViewTick) * fGameInterval / static_cast<double>(nTotViewTicks) - m_fStartedMillisec;
	return fElapsedMillisec;
}
bool LevelAnimation::isStarted(int32_t nViewTick, int32_t nTotViewTicks) const noexcept
{
	const double fElapsed = getElapsed(nViewTick, nTotViewTicks);
//std::cout << "----------- fElapsed = " << fElapsed << "  m_fDuration=" << m_fDuration << '\n';
	return (fElapsed >= 0.0);
}
bool LevelAnimation::isDone(int32_t nViewTick, int32_t nTotViewTicks) const noexcept
{
	if (m_fDuration < 0.0) {
		assert(isActive());
		assert((nViewTick >= 0) && (nViewTick < nTotViewTicks));
		assert(!m_p0Game->isInGameTick());
		return false; //--------------------------------------------------------
	}
	const double fElapsed = getElapsed(nViewTick, nTotViewTicks);
//std::cout << "----------- fElapsed = " << fElapsed << "  m_fDuration=" << m_fDuration << '\n';
	return (fElapsed >= m_fDuration);
}
void LevelAnimation::onScrolled(Direction::VALUE /*eDir*/) noexcept
{
}

double LevelAnimation::gameInterval() const noexcept
{
	return m_p0Game->gameInterval();
}
void LevelAnimation::setLevel(Level* p0Level) noexcept
{
	m_p0Level = p0Level;
	if (m_p0Level != nullptr) {
		m_p0Game = &(m_p0Level->game());
	} else {
		m_p0Game = nullptr;
	}
}

} // namespace stmg
