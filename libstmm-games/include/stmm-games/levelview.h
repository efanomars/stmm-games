/*
 * File:   levelview.h
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

#ifndef STMG_LEVEL_VIEW_H
#define STMG_LEVEL_VIEW_H

#include "levellisteners.h"

#include <memory>

namespace stmg { class LevelAnimation; }
namespace stmg { struct NPoint; }
namespace stmg { struct NRect; }

namespace stmg
{

using std::unique_ptr;
using std::shared_ptr;

class LevelView : public BlocksListener, public BoardListener
{
public:
	/** Tells the view that an area of tiles has to be animated.
	 * The animation takes place in the view ticks following the game tick
	 * in which this function is called.
	 * @param oArea The area to animate. Must be within board and not empty.
	 */
	virtual void boardAnimateTiles(NRect oArea) noexcept = 0;
	/** Tells the view that a tile has to be animated.
	 * The animation takes place in the view ticks following the game tick
	 * in which this function is called.
	 * @param oXY The tile to animate. Must be within board.
	 */
	virtual void boardAnimateTile(NPoint oXY) noexcept = 0;
	/** Creates an animation in the view.
	 * It might fail because the theme has no factory for the given model or because
	 * the animation is already active.
	 * @param refLevelAnimation The model animation. Cannot be null.
	 * @return Returns false if the corresponding ThemeAnimation couldn't be created.
	 */
	virtual bool animationCreate(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept = 0;
	/** Removes animation from view.
	 * @param refLevelAnimation The model animation. Cannot be null.
	 * @return Returns false if the animation already expired or was never added.
	 */
	virtual bool animationRemove(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_LEVEL_VIEW_H */

