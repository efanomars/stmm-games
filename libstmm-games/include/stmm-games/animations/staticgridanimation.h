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
 * File:   staticgridanimation.h
 */

#ifndef STMG_STATIC_GRID_ANIMATION_H
#define STMG_STATIC_GRID_ANIMATION_H

#include "levelanimation.h"

#include "util/basictypes.h"

#include <vector>

#include <stdint.h>

namespace stmg
{

/** Static grid animation.
 *
 */
class StaticGridAnimation : public LevelAnimation
{
public:
	struct ImageSpan
	{
		int32_t m_nImgId = -1; /**< The image to be painted to m_oRect. */
		NPoint m_oPos; /**< The position in cells within the grid.
						 * Must be completely within the grid. Size must be &gt; 0. */
		NSize m_oSpanSize; /**< The number of cells the image should cover. Default: {1,1}. */
		NSize m_oRepeatSize; /**< How many times the image has to be repeated for
								 * each axis. Default: {1,1}. */
	};
	struct LocalInit
	{
		NSize m_oCellsSize; /**< The size in cells into which the animation is partitioned.
							 * A non positive value for width or height means 1. */
		std::vector<ImageSpan> m_aSpans; /**< Must not be empty. */
	};
	struct Init : public LevelAnimation::Init, public LocalInit
	{
	};
	/** Constructor.
	 * Expects oInit.m_oSize to be defined (in tiles).
	 * @param oInit The initialization data.
	 */
	explicit StaticGridAnimation(Init&& oInit) noexcept;

	inline const std::vector<ImageSpan>& getSpans() const noexcept { return m_oLocalInit.m_aSpans; }
	inline NSize getGridCellsSize() const noexcept { return m_oLocalInit.m_oCellsSize; }

	/** Stops the animation.
	 * This signals the view (of the this animation) to remove itself but not
	 * necessarily immediately, allowing for instance a fading effect.
	 * After stop() is called, a call to LevelView::animationRemove() can be omitted. */
	void stop() noexcept;

	inline bool isStopped() const noexcept { return m_bIsStopped; }
protected:
	void reInit(Init&& oInit) noexcept;
private:
	void commonInit() noexcept;
private:
	LocalInit m_oLocalInit;
	bool m_bIsStopped;
};




} // namespace stmg

#endif	/* STMG_STATIC_GRID_ANIMATION_H */

