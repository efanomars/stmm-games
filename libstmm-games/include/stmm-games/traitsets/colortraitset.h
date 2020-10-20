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
 * File:   colortraitset.h
 */

#ifndef STMG_COLOR_TRAIT_SET_H
#define STMG_COLOR_TRAIT_SET_H

#include "traitsets/notemptyinttraitset.h"

#include "tile.h"
#include "traitset.h"

#include <memory>
#include <cassert>
#include <iostream>
#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <stdint.h>

namespace stmg
{

/** Template param: 0 is for red, 1 for green and 2 for blue.
 */
template<int32_t nChannel>
class ColorChannelTraitSet : public NotEmptyIntTraitSet
{
public:
	using NotEmptyIntTraitSet::NotEmptyIntTraitSet;
	ColorChannelTraitSet() = delete;

	std::pair<bool, int32_t> getTraitValue(const Tile& oTile) const noexcept override
	{
		static_assert((nChannel >= 0) && (nChannel <= 2), "");
		const TileColor& oTC = oTile.getTileColor();
		if (oTC.getColorType() != TileColor::COLOR_TYPE_RGB) {
			return std::make_pair(true, 0); //----------------------------------
		}
		uint8_t nR, nG, nB;
		oTC.getColorRGB(nR, nG, nB);
		uint8_t nRes;
		if (nChannel == 0) {
			nRes = nR;
		} else if (nChannel == 1) {
			nRes = nG;
		} else {
			nRes = nB;
		}
		return std::make_pair(false, nRes);
	}
	void setTileTraitValue(Tile& oTile, int32_t nValue) const noexcept override
	{
		assert((nValue >= 0) && (nValue <= 255));
		TileColor& oTC = oTile.getTileColor();
		uint8_t nR = 0, nG = 0, nB = 0;
		if (oTC.getColorType() == TileColor::COLOR_TYPE_RGB) {
			oTC.getColorRGB(nR, nG, nB);
		}
		if (nChannel == 0) {
			nR = static_cast<uint8_t>(nValue);
		} else if (nChannel == 1) {
			nG = static_cast<uint8_t>(nValue);
		} else {
			nB = static_cast<uint8_t>(nValue);
		}
		oTC.setColorRGB(nR, nG, nB);
	}
	void dump(int32_t
				#ifndef NDEBUG
				nIndentSpaces
				#endif //NDEBUG
				, bool
				#ifndef NDEBUG
				bHeader
				#endif //NDEBUG
				) const noexcept override
	{
		#ifndef NDEBUG
		auto sIndent = std::string(nIndentSpaces, ' ');
		if (bHeader) {
			std::cout << sIndent;
			if (nChannel == 0) {
				std::cout << "ColorRedTraitSet";
			} else if (nChannel == 1) {
				std::cout << "ColorGreenTraitSet";
			} else {
				std::cout << "ColorBlueTraitSet";
			}
			std::cout << '\n';
		}
		NotEmptyIntTraitSet::dump(nIndentSpaces + 2, false);
		#endif //NDEBUG
	}
};
////////////////////////////////////////////////////////////////////////////////
class ColorRedTraitSet final : public ColorChannelTraitSet<0>
{
public:
	using ColorChannelTraitSet<0>::ColorChannelTraitSet;
	ColorRedTraitSet() = delete;
};
////////////////////////////////////////////////////////////////////////////////
class ColorGreenTraitSet final : public ColorChannelTraitSet<1>
{
public:
	using ColorChannelTraitSet<1>::ColorChannelTraitSet;
	ColorGreenTraitSet() = delete;
};
////////////////////////////////////////////////////////////////////////////////
class ColorBlueTraitSet final : public ColorChannelTraitSet<2>
{
public:
	using ColorChannelTraitSet<2>::ColorChannelTraitSet;
	ColorBlueTraitSet() = delete;
};
////////////////////////////////////////////////////////////////////////////////
class ColorRgbTraitSet final : public TraitSet
{
public:
	ColorRgbTraitSet() = delete;
	ColorRgbTraitSet(const ColorRgbTraitSet& oSource) = default;
	ColorRgbTraitSet(ColorRgbTraitSet&& oSource) noexcept = default;
	ColorRgbTraitSet(const ColorRedTraitSet& oR, const ColorGreenTraitSet& oG, const ColorBlueTraitSet& oB) noexcept;
	ColorRgbTraitSet(ColorRedTraitSet&& oR, ColorGreenTraitSet&& oG, ColorBlueTraitSet&& oB) noexcept;
	explicit ColorRgbTraitSet(const std::vector<std::tuple<uint8_t, uint8_t, uint8_t>>& aValues) noexcept;
	bool hasEmptyValue() const noexcept override final;
	int32_t getTotValues() const noexcept override final;
	bool setTileTraitValueByIndex(Tile& oTile, int32_t nIdx) const noexcept override final;
	int32_t getIndexOfTileTraitValue(const Tile& oTile) const noexcept override final;
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
private:
	ColorRedTraitSet m_oR;
	ColorGreenTraitSet m_oG;
	ColorBlueTraitSet m_oB;
	const int32_t m_nTotValuesR;
	const int32_t m_nTotValuesG;
	const int32_t m_nTotValuesB;
	//
	std::vector<std::tuple<uint8_t, uint8_t, uint8_t>> m_aRGBs;
	const int32_t m_nTotRGBs;
};
////////////////////////////////////////////////////////////////////////////////
class ColorIndexTraitSet final : public NotEmptyIntTraitSet
{
public:
	using NotEmptyIntTraitSet::NotEmptyIntTraitSet;
	ColorIndexTraitSet() = delete;

	std::pair<bool, int32_t> getTraitValue(const Tile& oTile) const noexcept override;
	void setTileTraitValue(Tile& oTile, int32_t nValue) const noexcept override;
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
};
////////////////////////////////////////////////////////////////////////////////
class ColorPalTraitSet final : public NotEmptyIntTraitSet
{
public:
	using NotEmptyIntTraitSet::NotEmptyIntTraitSet;
	ColorPalTraitSet() = delete;

	std::pair<bool, int32_t> getTraitValue(const Tile& oTile) const noexcept override;
	void setTileTraitValue(Tile& oTile, int32_t nValue) const noexcept override;
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
};

////////////////////////////////////////////////////////////////////////////////
class ColorTraitSet final : public TraitSet
{
public:
	/** Constructs the color trait set containing exactly the empty value.
	 */
	ColorTraitSet() noexcept;
	explicit ColorTraitSet(std::unique_ptr<ColorRgbTraitSet> refRgbTS) noexcept;
	explicit ColorTraitSet(std::unique_ptr<ColorPalTraitSet> refPalTS) noexcept;
	explicit ColorTraitSet(std::unique_ptr<ColorIndexTraitSet> refIndexTS) noexcept;
	explicit ColorTraitSet(const std::vector<TileColor>& aTileColors) noexcept;
	explicit ColorTraitSet(std::vector<TileColor>&& aTileColors) noexcept;
	ColorTraitSet& operator=(ColorTraitSet&& oSource) noexcept;
	bool hasEmptyValue() const noexcept override final;
	int32_t getTotValues() const noexcept override final;
	bool setTileTraitValueByIndex(Tile& oTile, int32_t nIdx) const noexcept override final;
	int32_t getIndexOfTileTraitValue(const Tile& oTile) const noexcept override final;
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
private:
	bool m_bEmptyColor;
	int32_t m_nTotValues;
	std::unique_ptr<TraitSet> m_refSubColorTraitSet;
	std::vector<TileColor> m_aTileColors;
};

} // namespace stmg

#endif /* STMG_COLOR_TRAIT_SET_H */
