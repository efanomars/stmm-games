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
 * File:   randomtiles.h
 */

#ifndef STMG_RANDOM_TILES_H
#define STMG_RANDOM_TILES_H

#include "traitset.h"
#include "tile.h"

#include "util/randomparts.h"

#include <vector>
#include <memory>

#include <stdint.h>

namespace stmg { class RandomSource; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class RandomTiles
{
public:
	struct ProbTraitSets
	{
		int32_t m_nProb = 10; /**< Weight for the random trait sets m_aTraitSets. Must be &gt; 0. Default is 10. */
		std::vector< unique_ptr<TraitSet> > m_aTraitSets; /**< The tile trait sets from which to generate tiles. Cannot be empty. */
	};
	struct ProbTileGen
	{
		std::vector< ProbTraitSets > m_aProbTraitSets; /**< The new tile weigthed probabilities. Cannot be empty. */
	};
	/** Constructor.
	 * At least one of the trait sets in m_aProbTraitSets must have a non empty value.
	 * @param oRandomSource The random numbers source. The caller must ensure outlives this instance.
	 * @param oProbTileGen The new tile random distribution.
	 */
	RandomTiles(RandomSource& oRandomSource, ProbTileGen&& oProbTileGen) noexcept;

	/** Create a tile.
	 * @return The tile. Is not empty.
	 */
	Tile createTile() noexcept;

private:
	void addRandom(int32_t nProb, std::vector< std::unique_ptr<TraitSet> >&& aTraitSets) noexcept;

	struct RandomTile
	{
		std::vector< std::unique_ptr<TraitSet> > m_aTraitSets;
		std::vector< bool > m_aTraitSetHasEmptyValue; // Size: m_aTraitSets.size())
	};

	void getTileFromRandom(Tile& oCurTile, int32_t nRange) noexcept;

private:
	RandomSource& m_oRandomSource;

	RandomParts<RandomTile> m_oRandom;
private:
	RandomTiles() = delete;
	RandomTiles(const RandomTiles& oSource) = delete;
	RandomTiles& operator=(const RandomTiles& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_RANDOM_TILES_H */

