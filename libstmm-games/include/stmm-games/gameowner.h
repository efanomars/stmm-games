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
 * File:   gameowner.h
 */

#ifndef STMG_GAME_OWNER_H
#define STMG_GAME_OWNER_H

#include "gameproxy.h"

namespace stmg
{

class GameOwner
{
public:
	virtual ~GameOwner() noexcept = default;
	/** Tells the owner the game has ended.
	 * This function expects the owner to add the scores of the team(s) to the
	 * highscores if available.
	 */
	virtual void gameEnded() noexcept = 0;
	//
	/** Tells owner to possibly interrupt the current game.
	 * For INTERRUPT_PAUSE the owner will in turn stop calling Game::handleTimer()
	 * and Game::handleInput() for as long as the game remains paused.
	 *
	 * The owner should also momentarily pause the game for all interrupt that
	 * ask the "Are you sure" question.
	 * @param eInterruptType The interrupt type.
	 */
	virtual void gameInterrupt(GameProxy::INTERRUPT_TYPE eInterruptType) noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_GAME_OWNER_H */

