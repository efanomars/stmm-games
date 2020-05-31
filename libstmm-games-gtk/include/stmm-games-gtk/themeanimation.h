/*
 * File:   themeanimation.h
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

#ifndef STMG_THEME_ANIMATION_H
#define STMG_THEME_ANIMATION_H

#include <stdint.h>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

class ThemeAnimation
{
public:
	virtual ~ThemeAnimation() noexcept = default;
		// Calculates the bounding rect in pixels of what is drawn by draw(...)
		// Returns width 0 if the drawing is all over the place
	//	virtual NRect getRect(int32_t nViewTick, int32_t nTotViewTicks) = 0;
	/** Get the z value at a view tick.
	 * @param nViewTick The view tick. Is &gt;= 0 and &lt; nTotViewTicks.
	 * @param nTotViewTicks The total number of view ticks for the game interval. Is &gt; 0.
	 * @return The z value.
	 */
	virtual int32_t getZ(int32_t nViewTick, int32_t nTotViewTicks) noexcept = 0;
	/** Signals to the view that the theme animation can be started.
	 * While animations are added during game ticks, the vier really starts
	 * to draw them at view tick granularity.
	 * @param nViewTick The view tick. Is &gt;= 0 and &lt; nTotViewTicks.
	 * @param nTotViewTicks The total number of view ticks for the game interval. Is &gt; 0.
	 * @return Whether started in the view tick.
	 */
	virtual bool isStarted(int32_t nViewTick, int32_t nTotViewTicks) noexcept = 0;
	/** Signals to the view that the theme animation has finished and can be removed.
	 * If the returned value is true the implementation must also remove any
	 * reference to the LevelAnimation so that it can be recycled.
	 *
	 * If the returned value is true the view won't call onRemoved().
	 * @param nViewTick The view tick. Is &gt;= 0 and &lt; nTotViewTicks.
	 * @param nTotViewTicks The total number of view ticks for the game interval. Is &gt; 0.
	 * @return Whether the animation is finished and must be removed.
	 */
	virtual bool isDone(int32_t nViewTick, int32_t nTotViewTicks) noexcept = 0;
	/** Called by the view to draw the animation.
	 * refCC points at the center of the animation.
	 * @param nViewTick The view tick. Is &gt;= 0 and &lt; nTotViewTicks.
	 * @param nTotViewTicks The total number of view ticks for the game interval. Is &gt; 0.
	 * @param refCc The drawing context. Is not null.
	 */
	virtual void draw(int32_t nViewTick, int32_t nTotViewTicks, const Cairo::RefPtr<Cairo::Context>& refCc) noexcept = 0;
	/** Called by view when animation is removed from the model.
	 * The implementation must remove any reference to the LevelAnimation
	 * so that it can be recycled.
	 *
	 * Isn't called if isDone() was called by the view returning true.
	 */
	virtual void onRemoved() noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_THEME_ANIMATION_H */

