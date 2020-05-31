/*
 * File:   gameview.h
 *
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

#ifndef STMG_GAME_VIEW_H
#define STMG_GAME_VIEW_H

#include <memory>

#include <stdint.h>

namespace stmg { class GameSound; }
namespace stmg { struct FPoint; }
namespace stmi { class Event; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class GameView
{
public:
	/** Forward an XYEvent to the view.
	 * This function is called while dispatching the queued inputs.
	 * @param refXYEvent The event to handle. Cannot be null.
	 */
	virtual void handleXYEvent(const shared_ptr<stmi::Event>& refXYEvent) noexcept = 0;
	/** Create a game sound.
	 * The team or mate select the player(s) that should hear the sound.
	 * When nTeam is not -1, the sound can only be heard in per player sound mode
	 * or there is only one human player and nTeamand nMate select it (or its team).
	 * @param nSoundIdx The sound index according to Named::sounds(). Must be valid.
	 * @param nTeam The team or -1 if a sound directed to all.
	 * @param nMate The mate or -1 if a sound directed to all or team.
	 * @param oXYPos The position of the sound on the board. Absolute or relative to the listener.
	 * @param fZPos The z position of the sound.
	 * @param bListenerRelative Whether oXYPos is relative to the listener.
	 * @param fVolume01 The volume. Must be from 0.0 (inaudible) to 1.0 (max).
	 * @param bLooping Whether the sound should continuously repeat.
	 * @return The created sound or null if not supported.
	 */
	virtual shared_ptr<GameSound> createSound(int32_t nSoundIdx, int32_t nTeam, int32_t nMate
											, FPoint oXYPos, double fZPos, bool bListenerRelative
											, double fVolume01, bool bLooping) noexcept = 0;
	/** Pre-load a sound.
	 * Makes sure that subsequent calls to createSound with th e same nSoundIdx
	 * are as fast as possible.
	 * @param nSoundIdx The sound index according to Named::sounds(). Must be valid.
	 */
	virtual void preloadSound(int32_t nSoundIdx) noexcept = 0;
	/** Remove a sound.
	 * In order for the sound to be recycled the caller should also remove reference to it.
	 * @param refSound The sound. Cannot be  null.
	 * @return Whether the sound was still active.
	 */
	virtual bool removeSound(const shared_ptr<GameSound>& refSound) noexcept = 0;

	virtual ~GameView() noexcept = default;
};


} // namespace stmg

#endif /* STMG_GAME_VIEW_H */

