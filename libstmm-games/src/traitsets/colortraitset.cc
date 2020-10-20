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
 * File:   colortraitset.cc
 */

#include "traitsets/colortraitset.h"

#include "traitsets/notemptyinttraitset.h"

#include <algorithm>
#include <iterator>

namespace stmg
{

ColorRgbTraitSet::ColorRgbTraitSet(const ColorRedTraitSet& oR, const ColorGreenTraitSet& oG, const ColorBlueTraitSet& oB) noexcept
: m_oR(oR)
, m_oG(oG)
, m_oB(oB)
, m_nTotValuesR(m_oR.getTotValues())
, m_nTotValuesG(m_oG.getTotValues())
, m_nTotValuesB(m_oB.getTotValues())
, m_nTotRGBs(0)
{
	assert(m_nTotValuesR > 0);
	assert(m_nTotValuesG > 0);
	assert(m_nTotValuesB > 0);
}
ColorRgbTraitSet::ColorRgbTraitSet(ColorRedTraitSet&& oR, ColorGreenTraitSet&& oG, ColorBlueTraitSet&& oB) noexcept
: m_oR(std::move(oR))
, m_oG(std::move(oG))
, m_oB(std::move(oB))
, m_nTotValuesR(m_oR.getTotValues())
, m_nTotValuesG(m_oG.getTotValues())
, m_nTotValuesB(m_oB.getTotValues())
, m_nTotRGBs(0)
{
	assert(m_nTotValuesR > 0);
	assert(m_nTotValuesG > 0);
	assert(m_nTotValuesB > 0);
}
ColorRgbTraitSet::ColorRgbTraitSet(const std::vector<std::tuple<uint8_t, uint8_t, uint8_t>>& aValues) noexcept
: m_oR({})
, m_oG({})
, m_oB({})
, m_nTotValuesR(0)
, m_nTotValuesG(0)
, m_nTotValuesB(0)
, m_aRGBs(aValues)
, m_nTotRGBs(static_cast<int32_t>(aValues.size()))
{
	assert(aValues.size() > 0);
}
bool ColorRgbTraitSet::hasEmptyValue() const noexcept
{
	return false;
}
int32_t ColorRgbTraitSet::getTotValues() const noexcept
{
	return m_nTotValuesR * m_nTotValuesG * m_nTotValuesB + m_nTotRGBs;
}
bool ColorRgbTraitSet::setTileTraitValueByIndex(Tile& oTile, int32_t nIdx) const noexcept
{
	if (nIdx < 0) {
		// this trait type cannot be empty
		return false; //----------------------------------------------------
	}
	assert((nIdx >= 0) && (nIdx < getTotValues()));
	if (m_nTotValuesR > 0) {
		const int32_t nIdxR = nIdx / (m_nTotValuesG * m_nTotValuesB);
		assert(nIdxR < m_nTotValuesR);
		const int32_t nRestIdx = nIdx % (m_nTotValuesG * m_nTotValuesB);
		const int32_t nIdxG = nRestIdx / m_nTotValuesB;
		assert(nIdxG < m_nTotValuesG);
		const int32_t nIdxB = nRestIdx % m_nTotValuesB;
		assert(nIdxB < m_nTotValuesB);
		m_oR.setTileTraitValueByIndex(oTile, nIdxR);
		m_oG.setTileTraitValueByIndex(oTile, nIdxG);
		m_oB.setTileTraitValueByIndex(oTile, nIdxB);
	} else {
		const auto& oRGB = m_aRGBs[nIdx];
		const uint8_t nR = std::get<0>(oRGB);
		const uint8_t nG = std::get<1>(oRGB);
		const uint8_t nB = std::get<2>(oRGB);
		oTile.getTileColor().setColorRGB(nR, nG, nB);
	}
	return true;
}
int32_t ColorRgbTraitSet::getIndexOfTileTraitValue(const Tile& oTile) const noexcept
{
	const int32_t nTraitNotInSet = -2;
	//
	if (m_nTotValuesR > 0) {
		const int32_t nIdxR = m_oR.getIndexOfTileTraitValue(oTile);
		if (nIdxR < 0) {
			return nIdxR; //------------------------------------------------
		}
		const int32_t nIdxG = m_oG.getIndexOfTileTraitValue(oTile);
		if (nIdxG < 0) {
			return nIdxG; //------------------------------------------------
		}
		const int32_t nIdxB = m_oB.getIndexOfTileTraitValue(oTile);
		if (nIdxB < 0) {
			return nIdxB; //------------------------------------------------
		}
		int32_t nIdx = (nIdxR * m_nTotValuesG + nIdxG) * m_nTotValuesB + nIdxB;
		return nIdx; //-----------------------------------------------------
	}
	const TileColor& oTC = oTile.getTileColor();
	if (oTC.getColorType() != TileColor::COLOR_TYPE_RGB) {
		return nTraitNotInSet; //-------------------------------------------
	}
	uint8_t nR, nG, nB;
	oTC.getColorRGB(nR, nG, nB);
	auto itFind = std::find_if(m_aRGBs.begin(), m_aRGBs.end(), [&](const std::tuple<uint8_t, uint8_t, uint8_t>& oRGB)
		{
			return (nR == std::get<0>(oRGB)) && (nG == std::get<1>(oRGB)) && (nB == std::get<2>(oRGB));
		});
	if (itFind == m_aRGBs.end()) {
		return nTraitNotInSet; //-------------------------------------------
	}
	return std::distance(m_aRGBs.begin(), itFind);
}

std::pair<bool, int32_t> ColorIndexTraitSet::getTraitValue(const Tile& oTile) const noexcept
{
	const TileColor& oTC = oTile.getTileColor();
	if (oTC.getColorType() != TileColor::COLOR_TYPE_INDEX) {
		return std::make_pair(true, 0);
	}
	return std::make_pair(false, oTC.getColorIndex());
}
void ColorIndexTraitSet::setTileTraitValue(Tile& oTile, int32_t nValue) const noexcept
{
	assert((nValue >= TileColor::COLOR_INDEX_MIN) && (nValue <= TileColor::COLOR_INDEX_MAX));
	TileColor& oTC = oTile.getTileColor();
	oTC.setColorIndex(nValue);
}

std::pair<bool, int32_t> ColorPalTraitSet::getTraitValue(const Tile& oTile) const noexcept
{
	const TileColor& oTC = oTile.getTileColor();
	if (oTC.getColorType() != TileColor::COLOR_TYPE_PAL) {
		return std::make_pair(true, 0);
	}
	return std::make_pair(false, oTC.getColorPal());
}
void ColorPalTraitSet::setTileTraitValue(Tile& oTile, int32_t nValue) const noexcept
{
	assert((nValue >= TileColor::COLOR_PAL_FIRST) && (nValue <= TileColor::COLOR_PAL_LAST));
	TileColor& oTC = oTile.getTileColor();
	oTC.setColorPal(nValue);
}

ColorTraitSet::ColorTraitSet() noexcept
: m_bEmptyColor(true)
, m_nTotValues(1) // the set defines the empty value which is still one value
{
}
ColorTraitSet::ColorTraitSet(std::unique_ptr<ColorRgbTraitSet> refRgbTS) noexcept
: m_bEmptyColor(false)
, m_nTotValues((assert(refRgbTS), refRgbTS->getTotValues()))
, m_refSubColorTraitSet(std::move(refRgbTS))
{
}
ColorTraitSet::ColorTraitSet(std::unique_ptr<ColorPalTraitSet> refPalTS) noexcept
: m_bEmptyColor(false)
, m_nTotValues((assert(refPalTS), refPalTS->getTotValues()))
, m_refSubColorTraitSet(std::move(refPalTS))
{
}
ColorTraitSet::ColorTraitSet(std::unique_ptr<ColorIndexTraitSet> refIndexTS) noexcept
: m_bEmptyColor(false)
, m_nTotValues((assert(refIndexTS), refIndexTS->getTotValues()))
, m_refSubColorTraitSet(std::move(refIndexTS))
{
}
ColorTraitSet::ColorTraitSet(const std::vector<TileColor>& aTileColors) noexcept
: m_bEmptyColor(aTileColors.empty())
, m_nTotValues(static_cast<int32_t>(aTileColors.size()))
, m_aTileColors(aTileColors)
{
}
ColorTraitSet::ColorTraitSet(std::vector<TileColor>&& aTileColors) noexcept
: m_bEmptyColor(aTileColors.empty())
, m_nTotValues(static_cast<int32_t>(aTileColors.size()))
, m_aTileColors(std::move(aTileColors))
{
}
ColorTraitSet& ColorTraitSet::operator=(ColorTraitSet&& oSource) noexcept
{
	m_aTileColors.swap(oSource.m_aTileColors);
	m_bEmptyColor = oSource.m_bEmptyColor;
	m_nTotValues = oSource.m_nTotValues;
	m_refSubColorTraitSet = std::move(oSource.m_refSubColorTraitSet);
	return *this;
}
bool ColorTraitSet::hasEmptyValue() const noexcept
{
	return m_bEmptyColor;
}
int32_t ColorTraitSet::getTotValues() const noexcept
{
	return m_nTotValues;
}
bool ColorTraitSet::setTileTraitValueByIndex(Tile& oTile, int32_t nIdx) const noexcept
{
	assert((nIdx >= -1) && (nIdx < getTotValues()));
	TileColor& oColor = oTile.getTileColor();
	if (nIdx < 0) {
		oColor.clear();
		return true; //-----------------------------------------------------
	}
	if (m_refSubColorTraitSet) {
		return m_refSubColorTraitSet->setTileTraitValueByIndex(oTile, nIdx);
	} else {
		oColor = m_aTileColors[nIdx];
		return true; //-----------------------------------------------------
	}
}
int32_t ColorTraitSet::getIndexOfTileTraitValue(const Tile& oTile) const noexcept
{
	const int32_t nEmptyValueIdx = -1;
	const int32_t nTraitNotInSet = -2;
	const TileColor& oColor = oTile.getTileColor();
	if (m_bEmptyColor) {
		if (oColor.isEmpty()) {
			return -nEmptyValueIdx; //--------------------------------------
		}
		return nTraitNotInSet; //-------------------------------------------
	}
	if (m_refSubColorTraitSet) {
		return m_refSubColorTraitSet->getIndexOfTileTraitValue(oTile); //---
	}
	auto itFind = std::find_if(m_aTileColors.begin(), m_aTileColors.end(), [&](const TileColor& oCurColor)
		{
			return (oCurColor == oColor);
		});
	if (itFind == m_aTileColors.end()) {
		return nTraitNotInSet; //-------------------------------------------
	}
	return std::distance(m_aTileColors.begin(), itFind);
}

void ColorRgbTraitSet::dump(int32_t
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
		std::cout << sIndent << "ColorRgbTraitSet" << '\n';
	}
	m_oR.dump(nIndentSpaces + 2, false);
	m_oG.dump(nIndentSpaces + 2, false);
	m_oB.dump(nIndentSpaces + 2, false);
	#endif //NDEBUG
}

void ColorIndexTraitSet::dump(int32_t
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
		std::cout << sIndent << "ColorIndexTraitSet" << '\n';
	}
	NotEmptyIntTraitSet::dump(nIndentSpaces + 2, false);
	#endif //NDEBUG
}

void ColorPalTraitSet::dump(int32_t
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
		std::cout << sIndent << "ColorPalTraitSet" << '\n';
	}
	NotEmptyIntTraitSet::dump(nIndentSpaces + 2, false);
	#endif //NDEBUG
}

void ColorTraitSet::dump(int32_t
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
		std::cout << sIndent << "ColorTraitSet" << '\n';
	}
	if (m_refSubColorTraitSet) {
		if (m_bEmptyColor) {
			std::cout << sIndent << "  (Empty) ";
		}
		m_refSubColorTraitSet->dump(nIndentSpaces + 2, false);
	} else {
		std::cout << sIndent << "  ";
		if (m_bEmptyColor) {
			std::cout << "(Empty) ";
		}
		std::cout << sIndent;
		for (const auto& oTileColor : m_aTileColors) {
			oTileColor.dump();
			std::cout << "  ";
		}
		std::cout << '\n';
	}
	#endif //NDEBUG
}

} // namespace stmg
