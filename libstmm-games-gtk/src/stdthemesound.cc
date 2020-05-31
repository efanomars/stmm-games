/*
 * File:   stdthemesound.cc
 *
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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

#include "stdthemesound.h"

#include "stdtheme.h"
#include "stdthemecontext.h"

#include <stmm-games/util/basictypes.h>

#include <stmm-input-au/playbackcapability.h>

#include <utility>
#include <cassert>
#include <algorithm>
#include <memory>

#include <stdint.h>


namespace stmg
{

StdThemeSound::StdThemeSound(StdTheme* p1Owner, StdThemeContext* p0StdThemeCtx
							, const std::vector<shared_ptr<stmi::PlaybackCapability>>& aPlaybacks
							, std::vector< int32_t >&& aSoundIds, bool bRelative) noexcept
: m_p1Owner(p1Owner)
, m_aPlaybacks(aPlaybacks)
, m_aSoundIds(std::move(aSoundIds))
, m_bRelative(bRelative)
{
	assert(p1Owner != nullptr);
	assert(p0StdThemeCtx != nullptr);
	m_fScaleX = p0StdThemeCtx->m_fSoundScaleX;
	m_fScaleY = p0StdThemeCtx->m_fSoundScaleY;
	m_fScaleZ = p0StdThemeCtx->m_fSoundScaleZ;

	calcActive();
}

void StdThemeSound::reInit(StdTheme* p1Owner, StdThemeContext* p0StdThemeCtx
							, const std::vector<shared_ptr<stmi::PlaybackCapability>>& aPlaybacks
							, std::vector< int32_t >&& aSoundIds, bool bRelative) noexcept
{
	assert(p1Owner != nullptr);
	assert(p0StdThemeCtx != nullptr);
	m_p1Owner = p1Owner;
	m_fScaleX = p0StdThemeCtx->m_fSoundScaleX;
	m_fScaleY = p0StdThemeCtx->m_fSoundScaleY;
	m_fScaleZ = p0StdThemeCtx->m_fSoundScaleZ;
	m_aPlaybacks = aPlaybacks;
	m_aSoundIds = std::move(aSoundIds);
	m_bRelative = bRelative;
	calcActive();
}
void StdThemeSound::calcActive() noexcept
{
	assert(! m_aPlaybacks.empty());
	assert(m_aPlaybacks.size() == m_aSoundIds.size());
	m_nActiveSounds = std::count_if(m_aPlaybacks.begin(), m_aPlaybacks.end(), [](const shared_ptr<stmi::PlaybackCapability>& refCapa)
	{
		return (refCapa.get() != nullptr);
	});
}
bool StdThemeSound::isFinished() noexcept
{
	return (m_nActiveSounds <= 0);
}
void StdThemeSound::pause() noexcept
{
	const int32_t nTotCapas = static_cast<int32_t>(m_aPlaybacks.size());
	for (int32_t nIdx = 0; nIdx < nTotCapas; ++nIdx) {
		auto& refCapa = m_aPlaybacks[nIdx];
		if (refCapa) {
			refCapa->pauseSound(m_aSoundIds[nIdx]);
		}
	}
}
void StdThemeSound::resume() noexcept
{
	const int32_t nTotCapas = static_cast<int32_t>(m_aPlaybacks.size());
	for (int32_t nIdx = 0; nIdx < nTotCapas; ++nIdx) {
		auto& refCapa = m_aPlaybacks[nIdx];
		if (refCapa) {
			refCapa->resumeSound(m_aSoundIds[nIdx]);
		}
	}
}
void StdThemeSound::setPos(FPoint oXY, double fZPosition) noexcept
{
	const int32_t nTotCapas = static_cast<int32_t>(m_aPlaybacks.size());
	for (int32_t nIdx = 0; nIdx < nTotCapas; ++nIdx) {
		auto& refCapa = m_aPlaybacks[nIdx];
		if (refCapa) {
			refCapa->setSoundPos(m_aSoundIds[nIdx], m_bRelative, oXY.m_fX, oXY.m_fY, fZPosition);
		}
	}
}
void StdThemeSound::setVolume(double fVolume01) noexcept
{
	const int32_t nTotCapas = static_cast<int32_t>(m_aPlaybacks.size());
	for (int32_t nIdx = 0; nIdx < nTotCapas; ++nIdx) {
		auto& refCapa = m_aPlaybacks[nIdx];
		if (refCapa) {
			refCapa->setSoundVol(m_aSoundIds[nIdx], fVolume01);
		}
	}
}
void StdThemeSound::stop() noexcept
{
	const int32_t nTotCapas = static_cast<int32_t>(m_aPlaybacks.size());
	for (int32_t nIdx = 0; nIdx < nTotCapas; ++nIdx) {
		auto& refCapa = m_aPlaybacks[nIdx];
		if (refCapa) {
			refCapa->stopSound(m_aSoundIds[nIdx]);
		}
	}
}
bool StdThemeSound::removeCapability(int32_t nCapabilityId) noexcept
{
	m_p1Owner->removeCapability(nCapabilityId); // remove cached file ids
	const int32_t nTotCapas = static_cast<int32_t>(m_aPlaybacks.size());
	for (int32_t nIdx = 0; nIdx < nTotCapas; ++nIdx) {
		auto& refCapa = m_aPlaybacks[nIdx];
		if (refCapa && (refCapa->getId() == nCapabilityId)) {
			refCapa.reset();
			--m_nActiveSounds;
		}
	}
	return (m_nActiveSounds <= 0);
}
bool StdThemeSound::finishedSound(int32_t nCapabilityId, int32_t nSoundId) noexcept
{
	const int32_t nTotCapas = static_cast<int32_t>(m_aPlaybacks.size());
	for (int32_t nIdx = 0; nIdx < nTotCapas; ++nIdx) {
		auto& refCapa = m_aPlaybacks[nIdx];
		if (refCapa && (refCapa->getId() == nCapabilityId) && (m_aSoundIds[nIdx] == nSoundId)) {
			refCapa.reset();
			--m_nActiveSounds;
		}
	}
	return (m_nActiveSounds <= 0);
}

} // namespace stmg
