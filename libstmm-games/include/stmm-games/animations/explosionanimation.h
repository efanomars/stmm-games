/*
 * File:  explosionanimation.h
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

#ifndef STMG_EXPLOSION_ANIMATION_H
#define STMG_EXPLOSION_ANIMATION_H

#include "levelanimation.h"

#include "tile.h"

#include <stdint.h>

namespace stmg
{

class ExplosionAnimation : public LevelAnimation
{
public:
	struct Init : public LevelAnimation::Init
	{
		Tile m_oTile;/**< The tile that has to explode. Might be igored by view. Can be empty. */
		int32_t m_nLevelPlayer = -1; /**< The controlling player if the tile belongs to a block or -1. Default is -1. */
	};
	explicit ExplosionAnimation(const Init& oInit) noexcept
	: LevelAnimation(oInit)
	, m_oTile(oInit.m_oTile)
	, m_nLevelPlayer(oInit.m_nLevelPlayer)
	{
	}
	/** The tile that has to be exploded.
	 * @return The tile. Can be empty.
	 */
	const Tile& getTile() const noexcept { return m_oTile; }
	/** The controlling player if the tile belongs to a block.
	 * @return The level player. Can be -1.
	 */
	int32_t getLevelPlayer() const noexcept { return m_nLevelPlayer; }

protected:
	/* @See Constructor.
	 */
	void reInit(const Init& oInit) noexcept
	{
		LevelAnimation::reInit(oInit);
		m_oTile = oInit.m_oTile;
		m_nLevelPlayer = oInit.m_nLevelPlayer;
	}

private:
	Tile m_oTile;
	int32_t m_nLevelPlayer;
};

} // namespace stmg

#endif	/* STMG_EXPLOSION_ANIMATION_H */

