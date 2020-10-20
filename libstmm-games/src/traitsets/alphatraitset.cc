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
 * File:   alphatraitset.cc
 */

#include "traitsets/alphatraitset.h"

#include "tile.h"
#include "util/intset.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>

namespace stmg
{

AlphaTraitSet::AlphaTraitSet() noexcept
: IntTraitSet()
, m_bEmptyValueInSet(true)
{
}
AlphaTraitSet::AlphaTraitSet(int32_t nValue) noexcept
: IntTraitSet(nValue)
, m_bEmptyValueInSet(false)
{
	assert((nValue >= TileAlpha::ALPHA_MIN) && (nValue <= TileAlpha::ALPHA_MAX));
}
AlphaTraitSet::AlphaTraitSet(int32_t nFromValue, int32_t nToValue) noexcept
: IntTraitSet(nFromValue, nToValue)
, m_bEmptyValueInSet(false)
{
	assert((nFromValue >= TileAlpha::ALPHA_MIN) && (nFromValue <= TileAlpha::ALPHA_MAX));
	assert((nToValue >= TileAlpha::ALPHA_MIN) && (nToValue <= TileAlpha::ALPHA_MAX));
}
AlphaTraitSet::AlphaTraitSet(int32_t nFromValue, int32_t nToValue, int32_t nStep) noexcept
: IntTraitSet(nFromValue, nToValue, nStep)
, m_bEmptyValueInSet(false)
{
	assert((nFromValue >= TileAlpha::ALPHA_MIN) && (nFromValue <= TileAlpha::ALPHA_MAX));
	assert((nToValue >= TileAlpha::ALPHA_MIN) && (nToValue <= TileAlpha::ALPHA_MAX));
}
AlphaTraitSet::AlphaTraitSet(const std::vector<int32_t>& aValues) noexcept
: IntTraitSet(aValues)
, m_bEmptyValueInSet(false)
{
	#ifndef NDEBUG
	for (auto& nValue : aValues) {
		assert((nValue >= TileAlpha::ALPHA_MIN) && (nValue <= TileAlpha::ALPHA_MAX));
	}
	#endif //NDEBUG
}
AlphaTraitSet::AlphaTraitSet(const IntSet& oIntSet, bool bWithEmptyValue) noexcept
: IntTraitSet(oIntSet)
, m_bEmptyValueInSet(bWithEmptyValue)
{
	checkValues(oIntSet);
}
AlphaTraitSet::AlphaTraitSet(IntSet&& oIntSet, bool bWithEmptyValue) noexcept
: IntTraitSet((checkValues(oIntSet), std::move(oIntSet)))
, m_bEmptyValueInSet(bWithEmptyValue)
{
}
bool AlphaTraitSet::hasEmptyValue() const noexcept
{
	return m_bEmptyValueInSet;
}
int32_t AlphaTraitSet::getTotValues() const noexcept
{
	return IntTraitSet::getTotValues() + (m_bEmptyValueInSet ? 1 : 0);
}
std::pair<bool, int32_t> AlphaTraitSet::getTraitValue(const Tile& oTile) const noexcept
{
	const TileAlpha& oTA = oTile.getTileAlpha();
	if (oTA.isEmpty()) {
		return std::make_pair(true, 0);
	}
	return std::make_pair(false, oTA.getAlpha());
}
void AlphaTraitSet::setTileTraitValue(Tile& oTile, int32_t nValue) const noexcept
{
	assert((nValue >= TileAlpha::ALPHA_MIN) && (nValue <= TileAlpha::ALPHA_MAX));
	TileAlpha& oTA = oTile.getTileAlpha();
	oTA.setAlpha(static_cast<uint8_t>(nValue));
}
void AlphaTraitSet::resetTileTraitValue(Tile& oTile) const noexcept
{
	TileAlpha& oTA = oTile.getTileAlpha();
	oTA.clear();
}
void AlphaTraitSet::checkValues(const IntSet&
								#ifndef NDEBUG
								oIntSet
								#endif //NDEBUG
								) noexcept
{
	#ifndef NDEBUG
	const int32_t nTotValues = oIntSet.size();
	for (int32_t nIdx = 0; nIdx < nTotValues; ++nIdx) {
		const int32_t nValue = oIntSet.getValueByIndex(nIdx);
		assert((nValue >= TileAlpha::ALPHA_MIN) && (nValue <= TileAlpha::ALPHA_MAX));
	}
	#endif
}

void AlphaTraitSet::dump(int32_t
#ifndef NDEBUG
nIndentSpaces
#endif //NDEBUG
, bool
#ifndef NDEBUG
bHeader
#endif //NDEBUG
) const noexcept
{
	#ifndef NDEBUG
	auto sIndent = std::string(nIndentSpaces, ' ');
	if (bHeader) {
		std::cout << sIndent << "AlphaTraitSet" << '\n';
	}
	if (m_bEmptyValueInSet) {
		std::cout << sIndent << "  (Empty) ";
	}
	IntTraitSet::dump(nIndentSpaces + 2, false);
	#endif //NDEBUG
}

} // namespace stmg
