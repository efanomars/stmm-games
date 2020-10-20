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
 * File:   randomtiles.cc
 */

#include "utile/randomtiles.h"

#include "randomsource.h"

#include <cassert>
#include <algorithm>
#include <utility>

namespace stmg
{

RandomTiles::RandomTiles(RandomSource& oRandomSource, ProbTileGen&& oProbTileGen) noexcept
: m_oRandomSource(oRandomSource)
{
	assert(oProbTileGen.m_aProbTraitSets.size() > 0);
	for (ProbTraitSets& oProbTraitSets : oProbTileGen.m_aProbTraitSets) {
		assert(oProbTraitSets.m_nProb > 0);
		addRandom(oProbTraitSets.m_nProb, std::move(oProbTraitSets.m_aTraitSets));
	}
}
void RandomTiles::addRandom(int32_t nProb, std::vector< std::unique_ptr<TraitSet> >&& aTraitSets) noexcept
{
	assert(aTraitSets.size() > 0);
	std::vector< bool > aTraitSetHasEmptyValue{};
	bool bHasNonEmptyValueSetIdx = false;
	for (auto& refTraitSet : aTraitSets) {
		assert(refTraitSet);
		aTraitSetHasEmptyValue.push_back(refTraitSet->hasEmptyValue());
		const int32_t nTotValues = refTraitSet->getTotValues();
		assert(nTotValues > 0);
		if ((!bHasNonEmptyValueSetIdx) && ((nTotValues > 1) || ((nTotValues == 1) && !refTraitSet->hasEmptyValue()))) {
			bHasNonEmptyValueSetIdx = true;
		}
	}
	assert(bHasNonEmptyValueSetIdx); // at least one trait set must have a non empty value!
	m_oRandom.addRandomPart(nProb, RandomTile{std::move(aTraitSets), std::move(aTraitSetHasEmptyValue)});
}
Tile RandomTiles::createTile() noexcept
{
	const int32_t nRange = m_oRandom.getRandomRange();
	Tile oCurTile;
	getTileFromRandom(oCurTile, nRange);
	return oCurTile;
}

void RandomTiles::getTileFromRandom(Tile& oCurTile, int32_t nRange) noexcept
{
	assert(nRange > 0);
	const int32_t nRandom = m_oRandomSource.random(0, nRange - 1);
	const RandomTile& oRandomTile = m_oRandom.getRandomPartProb(nRandom);

	do {
		const auto& aTraitSets = oRandomTile.m_aTraitSets;
		const int32_t nTotTraitSets = static_cast<int32_t>(aTraitSets.size());
		for (int32_t nTraitSet = 0; nTraitSet < nTotTraitSets; ++nTraitSet) {
			auto& refTraitSet = aTraitSets[nTraitSet];
			const int32_t nTotTraitValues = refTraitSet->getTotValues();
			assert(nTotTraitValues > 0);
			const int32_t nStartIdx = (oRandomTile.m_aTraitSetHasEmptyValue[nTraitSet] ? -1 : 0);
			const int32_t nEndIdx = nStartIdx + nTotTraitValues - 1;
			const int32_t nTraitIdx = m_oRandomSource.random(nStartIdx, nEndIdx);
//std::cout << "RandomTiles::createRandomTiles() nTraitSet=" << nTraitSet << " nStartIdx=" << nStartIdx << "  nEndIdx=" << nEndIdx << " nTraitIdx=" << nTraitIdx << '\n';
			#ifndef NDEBUG
			const bool bOk =
			#endif
			refTraitSet->setTileTraitValueByIndex(oCurTile, nTraitIdx);
			assert(bOk);
		}
		// Since for a RandomTile at least one of the trait sets has a non empty value,
		// the highest probability for all the trait sets to generate an empty tile is 50%
		// (that is: all trait sets have just the empty value except one that has
		// the empty value and a non empty value).
	} while (oCurTile.isEmpty()); // Loop until generated tile is non empty.
}


} // namespace stmg
