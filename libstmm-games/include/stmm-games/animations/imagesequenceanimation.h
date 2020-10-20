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
 * File:   imagesequenceanimation.h
 */

#ifndef STMG_IMAGE_SEQUENCE_ANIMATION_H
#define STMG_IMAGE_SEQUENCE_ANIMATION_H

#include "levelanimation.h"

namespace stmg { class Level; }

namespace stmg
{

/** Animation model for an image sequence.
 * The image sequence is defined by the view and identified by the
 * animation id (LevelAnimation::Init::m_nAnimationNamedIdx).
 */
class ImageSequenceAnimation : public LevelAnimation
{
public:
	struct Init : public LevelAnimation::Init
	{
	};
	explicit ImageSequenceAnimation(const Init& oInit) noexcept
	: LevelAnimation(oInit)
	{
	}
	Level& level() noexcept
	{
		return LevelAnimation::level();
	}
protected:
	void reInit(const Init& oInit) noexcept
	{
		LevelAnimation::reInit(oInit);
	}
};


} // namespace stmg

#endif	/* STMG_IMAGE_SEQUENCE_ANIMATION_H */

