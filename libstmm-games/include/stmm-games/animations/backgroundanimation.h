/*
 * File:  backgroundanimation.h
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

#ifndef STMG_BACKGROUND_ANIMATION_H
#define STMG_BACKGROUND_ANIMATION_H

#include "levelanimation.h"

#include "util/basictypes.h"
#include "util/direction.h"

#include <stdint.h>

namespace stmg
{

/** Background animation.
 * This animation has a fixed size within its reference system.
 * Within the animation rectangle an image is used as pattern to fill it.
 * The pattern origin (its top left corner) is set relative to the animation
 * top left corner. The image id, position and size can change at runtime.
 */
class BackgroundAnimation : public LevelAnimation
{
public:
	struct Init : public LevelAnimation::Init
	{
	};
	/** Constructor.
	 * @param oInit The initialization parameters.
	 */
	explicit BackgroundAnimation(const Init& oInit) noexcept;
	/** The current image id.
	 * @return The image id or -1 if not set.
	 */
	inline int32_t getImageId() const noexcept { return m_nImgId; }
	/** The current relative position of the image within the animation rectangle.
	 * @return The relative position in tiles.
	 */
	inline FPoint getImageRelPos() const noexcept { return m_oImgRelPos; }
	/** The size of the image.
	 * If one of the returned size components (oSize.m_fW and oNewSize.m_fH) is <= 0
	 * the ratio of the natural size of the image should be used. If both oSize.m_fW
	 * and oSize.m_fH are <= 0 the natural size should be used.
	 * @return The image size in tiles.
	 */
	inline FSize getImageSize() const noexcept { return m_oImgSize; }
	/** Set the repeated image to fill the animation rectangle.
	 * If one of oNewSize.m_fW and oNewSize.m_fH is <= 0 the ratio of the natural size
	 * of the image should be used. If both oNewSize.m_fW and oNewSize.m_fH are <= 0
	 * the the natural size should be used by the view.
	 *
	 * Set the image to -1 to hide temporarily the animation.
	 *
	 * @param nImgId The image id. Must be valid id in `level().getNamed().images() or -1.
	 * @param oNewSize The new size of the image.
	 * @param oImgRelPos The relative position of the image in respect to the position of the animation.
	 */
	void setImage(int32_t nImgId, const FSize& oNewSize, const FPoint& oImgRelPos) noexcept;
	/** Sets the new relative position of the pattern origin.
	 * The new position is reached in a game tick interpolating the view tick
	 * position from the old position.
	 * @param oNewRelPos The new relative position (in tiles).
	 */
	void moveImageTo(const FPoint& oNewRelPos) noexcept;

	/** The position of the image origin within animation during view ticks.
	 * Used to interpolate between current (game tick) position and the new
	 * (next game tick) position.
	 * @param nViewTick Must be &gt;= 0 and &lt; nTotViewTicks.
	 * @param nTotViewTicks Must be &gt; 0.
	 * @return The intermediate relative position.
	 */
	FPoint getImageRelPos(int32_t nViewTick, int32_t nTotViewTicks) const noexcept;

	void boardAfterScroll(Direction::VALUE eDir) noexcept;

protected:
	/** See constructor.
	 */
	void reInit(const Init& oInit) noexcept;

private:
	int32_t m_nImgId;
	FSize m_oImgSize;

	FPoint m_oImgRelPos;

	FPoint m_oPosDelta;
	int32_t m_nPosLastChangeTick;
};

} // namespace stmg

#endif	/* STMG_BACKGROUND_ANIMATION_H */

