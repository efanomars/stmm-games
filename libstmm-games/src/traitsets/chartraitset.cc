/*
 * File:   chartraitset.cc
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

#include "traitsets/chartraitset.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <string>
#include <type_traits>

namespace stmg
{

std::pair<bool, int32_t> CharIndexTraitSet::getTraitValue(const Tile& oTile) const noexcept
{
	const TileChar& oTC = oTile.getTileChar();
	if (!oTC.isCharIndex()) {
		return std::make_pair(true, 0);
	}
	return std::make_pair(false, oTC.getCharIndex());
}
void CharIndexTraitSet::setTileTraitValue(Tile& oTile, int32_t nValue) const noexcept
{
	assert((nValue >= TileChar::CHAR_INDEX_MIN) && (nValue <= TileChar::CHAR_INDEX_MAX));
	TileChar& oTC = oTile.getTileChar();
	oTC.setCharIndex(nValue);
}

std::pair<bool, int32_t> CharUcs4TraitSet::getTraitValue(const Tile& oTile) const noexcept
{
	const TileChar& oTC = oTile.getTileChar();
	if (oTC.isCharIndex()) {
		return std::make_pair(true, 0);
	}
	return std::make_pair(false, oTC.getChar());
}
void CharUcs4TraitSet::setTileTraitValue(Tile& oTile, int32_t nValue) const noexcept
{
	assert((nValue >= TileChar::CHAR_UCS4_MIN) && (nValue <= TileChar::CHAR_UCS4_MAX));
	TileChar& oTC = oTile.getTileChar();
	oTC.setChar(nValue);
}

CharTraitSet::CharTraitSet() noexcept
: m_bEmptyChar(true)
, m_nTotValues(1)
{
}
CharTraitSet::CharTraitSet(std::unique_ptr<CharIndexTraitSet>& refITS) noexcept
: m_bEmptyChar(false)
, m_nTotValues((assert(refITS), refITS->getTotValues()))
, m_refSubCharTraitSet(std::move(refITS))
{
}
CharTraitSet::CharTraitSet(std::unique_ptr<CharUcs4TraitSet>& refUTS) noexcept
: m_bEmptyChar(false)
, m_nTotValues((assert(refUTS), refUTS->getTotValues()))
, m_refSubCharTraitSet(std::move(refUTS))
{
}
CharTraitSet::CharTraitSet(std::unique_ptr<CharIndexTraitSet>&& refITS) noexcept
: m_bEmptyChar(false)
, m_nTotValues((assert(refITS), refITS->getTotValues()))
, m_refSubCharTraitSet(std::move(refITS))
{
}
CharTraitSet::CharTraitSet(std::unique_ptr<CharUcs4TraitSet>&& refUTS) noexcept
: m_bEmptyChar(false)
, m_nTotValues((assert(refUTS), refUTS->getTotValues()))
, m_refSubCharTraitSet(std::move(refUTS))
{
}
CharTraitSet::CharTraitSet(const std::vector<TileChar>& aTileChars) noexcept
: m_bEmptyChar(aTileChars.empty())
, m_nTotValues(static_cast<int32_t>(aTileChars.size()))
, m_aTileChars(aTileChars)
{
}
CharTraitSet::CharTraitSet(std::vector<TileChar>&& aTileChars) noexcept
: m_bEmptyChar(aTileChars.empty())
, m_nTotValues(static_cast<int32_t>(aTileChars.size()))
, m_aTileChars(std::move(aTileChars))
{
}
CharTraitSet& CharTraitSet::operator=(CharTraitSet&& oSource) noexcept
{
	m_aTileChars.swap(oSource.m_aTileChars);
	m_bEmptyChar = oSource.m_bEmptyChar;
	m_nTotValues = oSource.m_nTotValues;
	m_refSubCharTraitSet = std::move(oSource.m_refSubCharTraitSet);
	return *this;
}
bool CharTraitSet::hasEmptyValue() const noexcept
{
	return m_bEmptyChar;
}
int32_t CharTraitSet::getTotValues() const noexcept
{
	return m_nTotValues;
}
bool CharTraitSet::setTileTraitValueByIndex(Tile& oTile, int32_t nIdx) const noexcept
{
	assert((nIdx >= -1) && (nIdx < getTotValues()));
	TileChar& oChar = oTile.getTileChar();
	if (nIdx < 0) {
		oChar.clear();
		return true; //-----------------------------------------------------
	}
	if (m_refSubCharTraitSet) {
		return m_refSubCharTraitSet->setTileTraitValueByIndex(oTile, nIdx);
	} else {
		oChar = m_aTileChars[nIdx];
		return true; //-----------------------------------------------------
	}
}
int32_t CharTraitSet::getIndexOfTileTraitValue(const Tile& oTile) const noexcept
{
	const int32_t nEmptyValueIdx = -1;
	const int32_t nTraitNotInSet = -2;
	const TileChar& oChar = oTile.getTileChar();
	if (m_bEmptyChar) {
		if (oChar.isEmpty()) {
			return nEmptyValueIdx; //---------------------------------------
		}
		return nTraitNotInSet; //-------------------------------------------
	}
	if (m_refSubCharTraitSet) {
		return m_refSubCharTraitSet->getIndexOfTileTraitValue(oTile);
	}
	auto itFind = std::find_if(m_aTileChars.begin(), m_aTileChars.end(), [&](const TileChar& oCurChar)
		{
			return (oCurChar == oChar);
		});
	if (itFind == m_aTileChars.end()) {
		return nTraitNotInSet; //-------------------------------------------
	}
	return std::distance(m_aTileChars.begin(), itFind);
}

void CharIndexTraitSet::dump(int32_t
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
	if (bHeader) {
		auto sIndent = std::string(nIndentSpaces, ' ');
		std::cout << sIndent << "CharIndexTraitSet" << '\n';
	}
	NotEmptyIntTraitSet::dump(nIndentSpaces + 2, false);
	#endif //NDEBUG
}

void CharUcs4TraitSet::dump(int32_t
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
	if (bHeader) {
		auto sIndent = std::string(nIndentSpaces, ' ');
		std::cout << sIndent << "CharUcs4TraitSet" << '\n';
	}
	NotEmptyIntTraitSet::dump(nIndentSpaces + 2, false);
	#endif //NDEBUG
}

void CharTraitSet::dump(int32_t
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
		std::cout << sIndent << "CharTraitSet" << '\n';
	}
	if (m_refSubCharTraitSet) {
		if (m_bEmptyChar) {
			std::cout << sIndent << "  (Empty) ";
		}
		m_refSubCharTraitSet->dump(nIndentSpaces + 2, false);
	} else {
		std::cout << sIndent << "  ";
		if (m_bEmptyChar) {
			std::cout << "(Empty) ";
		}
		std::cout << sIndent;
		for (const auto& oTileColor : m_aTileChars) {
			oTileColor.dump();
			std::cout << " ";
		}
		std::cout << '\n';
	}
	#endif //NDEBUG
}

} // namespace stmg
