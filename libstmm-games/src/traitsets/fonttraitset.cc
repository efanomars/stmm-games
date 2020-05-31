/*
 * File:   fonttraitset.cc
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

#include "traitsets/fonttraitset.h"

#include "tile.h"
#include "util/intset.h"

#include <cassert>
#include <iostream>
#include <cstdint>
#include <string>


namespace stmg
{

FontTraitSet::FontTraitSet() noexcept
: IntTraitSet()
, m_bEmptyValueInSet(true)
{
}
FontTraitSet::FontTraitSet(int32_t nValue) noexcept
: IntTraitSet(nValue)
, m_bEmptyValueInSet(false)
{
	assert((nValue >= TileFont::CHAR_FONT_INDEX_MIN) && (nValue <= TileFont::CHAR_FONT_INDEX_MAX));
}
FontTraitSet::FontTraitSet(int32_t nFromValue, int32_t nToValue) noexcept
: IntTraitSet(nFromValue, nToValue)
, m_bEmptyValueInSet(false)
{
	assert((nFromValue >= TileFont::CHAR_FONT_INDEX_MIN) && (nFromValue <= TileFont::CHAR_FONT_INDEX_MAX));
	assert((nToValue >= TileFont::CHAR_FONT_INDEX_MIN) && (nToValue <= TileFont::CHAR_FONT_INDEX_MAX));
}
FontTraitSet::FontTraitSet(int32_t nFromValue, int32_t nToValue, int32_t nStep) noexcept
: IntTraitSet(nFromValue, nToValue, nStep)
, m_bEmptyValueInSet(false)
{
	assert((nFromValue >= TileFont::CHAR_FONT_INDEX_MIN) && (nFromValue <= TileFont::CHAR_FONT_INDEX_MAX));
	assert((nToValue >= TileFont::CHAR_FONT_INDEX_MIN) && (nToValue <= TileFont::CHAR_FONT_INDEX_MAX));
}
FontTraitSet::FontTraitSet(const std::vector<int32_t>& aValues) noexcept
: IntTraitSet(aValues)
, m_bEmptyValueInSet(false)
{
	#ifndef NDEBUG
	for (auto& nValue : aValues) {
		assert((nValue >= TileAlpha::ALPHA_MIN) && (nValue <= TileAlpha::ALPHA_MAX));
	}
	#endif //NDEBUG
}
FontTraitSet::FontTraitSet(const IntSet& oIntSet, bool bWithEmptyValue) noexcept
: IntTraitSet(oIntSet)
, m_bEmptyValueInSet(bWithEmptyValue)
{
	checkValues(oIntSet);
}
FontTraitSet::FontTraitSet(IntSet&& oIntSet, bool bWithEmptyValue) noexcept
: IntTraitSet((checkValues(oIntSet), std::move(oIntSet)))
, m_bEmptyValueInSet(bWithEmptyValue)
{
}

bool FontTraitSet::hasEmptyValue() const noexcept
{
	return m_bEmptyValueInSet;
}
int32_t FontTraitSet::getTotValues() const noexcept
{
	return IntTraitSet::getTotValues() + (m_bEmptyValueInSet ? 1 : 0);
}

std::pair<bool, int32_t> FontTraitSet::getTraitValue(const Tile& oTile) const noexcept
{
	const TileFont& oTF = oTile.getTileFont();
	if (oTF.isEmpty()) {
		return std::make_pair(true, 0);
	}
	return std::make_pair(false, oTF.getFontIndex());
}
void FontTraitSet::setTileTraitValue(Tile& oTile, int32_t nValue) const noexcept
{
	assert((nValue >= TileFont::CHAR_FONT_INDEX_MIN) && (nValue <= TileFont::CHAR_FONT_INDEX_MAX));
	TileFont& oTF = oTile.getTileFont();
	oTF.setFontIndex(nValue);
}
void FontTraitSet::resetTileTraitValue(Tile& oTile) const noexcept
{
	TileFont& oTF = oTile.getTileFont();
	oTF.clear();
}
void FontTraitSet::checkValues(const IntSet&
								#ifndef NDEBUG
								oIntSet
								#endif //NDEBUG
								) noexcept
{
	#ifndef NDEBUG
	const auto nTotValues = oIntSet.size();
	for (int32_t nIdx = 0; nIdx < nTotValues; ++nIdx) {
		const int32_t nValue = oIntSet.getValueByIndex(nIdx);
		assert((nValue >= TileFont::CHAR_FONT_INDEX_MIN) && (nValue <= TileFont::CHAR_FONT_INDEX_MAX));
	}
	#endif
}

void FontTraitSet::dump(int32_t
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
		std::cout << sIndent << "FontTraitSet" << '\n';
	}
	if (m_bEmptyValueInSet) {
		std::cout << sIndent << "  (Empty) " << '\n';
	}
	IntTraitSet::dump(nIndentSpaces + 2, false);
	#endif //NDEBUG
}

} // namespace stmg
