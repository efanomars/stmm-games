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
 * File:   chartraitset.h
 */

#ifndef STMG_CHAR_TRAIT_SET_H
#define STMG_CHAR_TRAIT_SET_H

#include "traitsets/notemptyinttraitset.h"

#include "tile.h"
#include "traitset.h"

#include <memory>
#include <utility>
#include <vector>

#include <stdint.h>

namespace stmg
{

////////////////////////////////////////////////////////////////////////////////
class CharIndexTraitSet final : public NotEmptyIntTraitSet
{
public:
	using NotEmptyIntTraitSet::NotEmptyIntTraitSet;
	CharIndexTraitSet() = delete;

	std::pair<bool, int32_t> getTraitValue(const Tile& oTile) const noexcept override;
	void setTileTraitValue(Tile& oTile, int32_t nValue) const noexcept override;
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
};
////////////////////////////////////////////////////////////////////////////////
class CharUcs4TraitSet final : public NotEmptyIntTraitSet
{
public:
	using NotEmptyIntTraitSet::NotEmptyIntTraitSet;
	CharUcs4TraitSet() = delete;

	std::pair<bool, int32_t> getTraitValue(const Tile& oTile) const noexcept override;
	void setTileTraitValue(Tile& oTile, int32_t nValue) const noexcept override;
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
};
////////////////////////////////////////////////////////////////////////////////
class CharTraitSet final : public TraitSet
{
public:
	/** Constructs the char trait set containing exactly the empty value.
	 */
	CharTraitSet() noexcept;
	explicit CharTraitSet(std::unique_ptr<CharIndexTraitSet>& refITS) noexcept;
	explicit CharTraitSet(std::unique_ptr<CharUcs4TraitSet>& refUTS) noexcept;
	explicit CharTraitSet(std::unique_ptr<CharIndexTraitSet>&& refITS) noexcept;
	explicit CharTraitSet(std::unique_ptr<CharUcs4TraitSet>&& refUTS) noexcept;
	explicit CharTraitSet(const std::vector<TileChar>& aTileChars) noexcept;
	explicit CharTraitSet(std::vector<TileChar>&& aTileChars) noexcept;
	CharTraitSet& operator=(CharTraitSet&& oSource) noexcept;
	bool hasEmptyValue() const noexcept override final;
	int32_t getTotValues() const noexcept override final;
	bool setTileTraitValueByIndex(Tile& oTile, int32_t nIdx) const noexcept override final;
	int32_t getIndexOfTileTraitValue(const Tile& oTile) const noexcept override final;
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
private:
	bool m_bEmptyChar;
	int32_t m_nTotValues;
	std::unique_ptr<TraitSet> m_refSubCharTraitSet;
	std::vector<TileChar> m_aTileChars;
};

} // namespace stmg

#endif /* STMG_CHAR_TRAIT_SET_H */
