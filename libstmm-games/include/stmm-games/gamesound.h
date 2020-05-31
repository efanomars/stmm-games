/*
 * File:   gamesound.h
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

#ifndef STMG_GAME_SOUND_H
#define STMG_GAME_SOUND_H

namespace stmg { struct FPoint; }

namespace stmg
{

class GameSound
{
public:
	/** Tells whether the sound has finished.
	 * When all the associated devices are removed this also returns true.
	 * @return Whether sound no longer playing.
	 */
	virtual bool isFinished() noexcept = 0;
	/** Pause the sound.
	 */
	virtual void pause() noexcept = 0;
	/** Resume the sound.
	 */
	virtual void resume() noexcept = 0;
	/** Change sound position.
	 * @param oXYPos The new x,y position.
	 * @param fZ The z position
	 */
	virtual void setPos(FPoint oXYPos, double fZ) noexcept = 0;
	/** Set the volume.
	 * @param fVolume01 The volume. Value 0 is inaudible, 1 is maximum volume.
	 */
	virtual void setVolume(double fVolume01) noexcept = 0;

	virtual ~GameSound() noexcept = default;
};


} // namespace stmg

#endif /* STMG_GAME_SOUND_H */

