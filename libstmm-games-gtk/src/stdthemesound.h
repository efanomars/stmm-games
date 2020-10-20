/*
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
/*
 * File:   stdthemesound.h
 */

#ifndef STMG_STD_THEME_SOUND_H
#define STMG_STD_THEME_SOUND_H

#include "themesound.h"

#include <memory>
#include <vector>

#include <stdint.h>

namespace stmg { struct FPoint; }
namespace stmg { class StdTheme; }
namespace stmg { class StdThemeContext; }

namespace stmi { class PlaybackCapability; }

namespace stmg
{

using std::shared_ptr;

class StdThemeSound  : public ThemeSound
{
public:
	StdThemeSound(StdTheme *m_p1Owner, StdThemeContext* p0StdThemeCtx
				, const std::vector<shared_ptr<stmi::PlaybackCapability>>& aPlaybacks
				, std::vector< int32_t >&& aSoundIds, bool bRelative) noexcept;

	void reInit(StdTheme *m_p1Owner, StdThemeContext* p0StdThemeCtx
				, const std::vector<shared_ptr<stmi::PlaybackCapability>>& aPlaybacks
				, std::vector< int32_t >&& aSoundIds, bool bRelative) noexcept;

	bool isFinished() noexcept override;
	void pause() noexcept override;
	void resume() noexcept override;
	void setPos(FPoint oXYPos, double fZ) noexcept override;
	void setVolume(double fVolume01) noexcept override;

	bool removeCapability(int32_t nCapabilityId) noexcept override;
	bool finishedSound(int32_t nCapabilityId, int32_t nSoundId) noexcept override;
	void stop() noexcept override;

private:
	void calcActive() noexcept;
private:
	StdTheme* m_p1Owner;
	double m_fScaleX;
	double m_fScaleY;
	double m_fScaleZ;

	std::vector< shared_ptr<stmi::PlaybackCapability> > m_aPlaybacks; // Value: capability or null
	std::vector< int32_t > m_aSoundIds; // Size: m_aPlaybacks.size(), Value: nSoundId
	bool m_bRelative;
	// the number of non null capabilities still playing the sound
	int32_t m_nActiveSounds; // if 0 isFinished() returns true
};

} // namespace stmg

#endif	/* STMG_STD_THEME_SOUND_H */

