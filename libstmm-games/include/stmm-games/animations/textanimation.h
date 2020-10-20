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
 * File:   textanimation.h
 */

#ifndef STMG_TEXT_ANIMATION_H
#define STMG_TEXT_ANIMATION_H

#include "levelanimation.h"

#include <vector>
#include <string>

#include <utility>

namespace stmg
{


class TextAnimation : public LevelAnimation
{
public:
	struct LocalInit
	{
		std::vector<std::string> m_aLines; /**< The text shown by the animation. */
		double m_fFontHeight; /**< The font height in tile heights. */
	};
	struct Init : public LevelAnimation::Init, public LocalInit
	{
	};
	/** Constructor.
	 * @param oInit The initialization parameters.
	 */
	explicit TextAnimation(Init&& oInit) noexcept
	: LevelAnimation(oInit)
	, m_oLocalData(std::move(oInit))
	{
	}

	inline double getFontHeight() const noexcept { return m_oLocalData.m_fFontHeight; }
	inline const std::vector<std::string>& getText() const noexcept { return m_oLocalData.m_aLines; }

protected:
	void reInit(Init&& oInit) noexcept
	{
		LevelAnimation::reInit(oInit);
		m_oLocalData = std::move(oInit);
	}

private:
	friend class Level;
	LocalInit m_oLocalData;
};


} // namespace stmg

#endif	/* STMG_TEXT_ANIMATION_H */
