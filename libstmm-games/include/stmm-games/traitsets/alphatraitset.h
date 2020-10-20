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
 * File:   alphatraitset.h
 */

#ifndef STMG_ALPHA_TRAIT_SET_H
#define STMG_ALPHA_TRAIT_SET_H

#include "traitsets/inttraitset.h"

//#include <iostream>
#include <utility>
#include <vector>

#include <stdint.h>

namespace stmg { class IntSet; }
namespace stmg { class Tile; }


namespace stmg
{

class AlphaTraitSet final : public IntTraitSet
{
public:
	/** Constructs set containing the empty value.
	 * If you want to construct the empty set use `AlphaTraitSet({})` instead.
	 */
	AlphaTraitSet() noexcept;
	explicit AlphaTraitSet(int32_t nValue) noexcept;
	AlphaTraitSet(int32_t nFromValue, int32_t nToValue) noexcept;
	AlphaTraitSet(int32_t nFromValue, int32_t nToValue, int32_t nStep) noexcept;
	explicit AlphaTraitSet(const std::vector<int32_t>& aValues) noexcept;
	AlphaTraitSet(const IntSet& oIntSet, bool bWithEmptyValue) noexcept;
	AlphaTraitSet(IntSet&& oIntSet, bool bWithEmptyValue) noexcept;
	AlphaTraitSet(const AlphaTraitSet& oSource) noexcept = default;
	AlphaTraitSet(AlphaTraitSet&& oSource) noexcept = default;
	AlphaTraitSet& operator=(const AlphaTraitSet& oSource) noexcept = default;
	AlphaTraitSet& operator=(AlphaTraitSet&& oSource) noexcept = default;

	bool hasEmptyValue() const noexcept override final;
	int32_t getTotValues() const noexcept override final;

	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	std::pair<bool, int32_t> getTraitValue(const Tile& oTile) const noexcept override;
	void setTileTraitValue(Tile& oTile, int32_t nValue) const noexcept override;
	void resetTileTraitValue(Tile& oTile) const noexcept override;
private:
	static void checkValues(const IntSet& oIntSet) noexcept;
private:
	bool m_bEmptyValueInSet;
};

} // namespace stmg

#endif /* STMG_ALPHA_TRAIT_SET_H */
