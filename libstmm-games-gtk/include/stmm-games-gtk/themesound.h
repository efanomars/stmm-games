/*
 * File:   themesound.h
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

#ifndef STMG_THEME_SOUND_H
#define STMG_THEME_SOUND_H

#include <stmm-games/gamesound.h>

#include <stdint.h>

namespace stmg
{

class ThemeSound : public GameSound
{
public:
	/** Signals playback capability removal.
	 * @param nCapabilityId The id of the capability being removed.
	 * @return Whether this theme sound is finished.
	 */
	virtual bool removeCapability(int32_t nCapabilityId) noexcept = 0;
	/** Signals playback sound finished.
	 * @param nCapabilityId The id of the capability of the finished sound.
	 * @param nSoundId The sound id of the finished (sub) sound.
	 * @return Whether this theme sound is finished.
	 */
	virtual bool finishedSound(int32_t nCapabilityId, int32_t nSoundId) noexcept = 0;
	/** Stops the sound.
	 * All sub-sounds are stopped. The reference to this instance can be reset
	 * after this call.
	 */
	virtual void stop() noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_THEME_SOUND_H */
