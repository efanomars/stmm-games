/*
 * File:   inttraitset.h
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

#ifndef STMG_INT_TRAIT_SET_H
#define STMG_INT_TRAIT_SET_H

#include "traitset.h"
#include "util/intset.h"

#include <vector>
#include <utility>

#include <stdint.h>

namespace stmg { class Tile; }

namespace stmg
{

/** Integer set that can be empty and can contain the empty value as long as
 * it is representable as an integer.
 */
class IntTraitSet : public TraitSet //, private IntSet
{
public:
	/** Whether the trait set contains the empty value.
	 * Default implementation returns false.
	 * If a subclass of this class can contain the empty value override the function.
	 * @return Whether the set contains the empty value.
	 */
	bool hasEmptyValue() const noexcept override
	{
		return false;
	}
	int32_t getTotValues() const noexcept override
	{
		return m_oIntSet.size();
	}
	bool setTileTraitValueByIndex(Tile& oTile, int32_t nIdx) const noexcept override final
	{
		if (nIdx < -1) {
			return false; //----------------------------------------------------
		}
		const bool bHasEmptyValue = hasEmptyValue();
		if (nIdx >= getTotValues() - (bHasEmptyValue ? 1 : 0)) {
			return false; //----------------------------------------------------
		}
		if (nIdx == -1) {
			if (bHasEmptyValue) {
				resetTileTraitValue(oTile);
				return true; //-------------------------------------------------
			}
			return false; //----------------------------------------------------
		}
//std::cout << " ++++++ getTotValues()=" << getTotValues() << "  nIdx=" << nIdx << '\n';
		const int32_t nValue = m_oIntSet.getValueByIndex(nIdx);
		setTileTraitValue(oTile, nValue);
		return true;
	}
	int32_t getIndexOfTileTraitValue(const Tile& oTile) const noexcept override final
	{
		const int32_t nEmptyValueIdx = -1;
		const int32_t nTraitNotInSet = -2;
		const auto oPair = getTraitValue(oTile);
		const bool bEmptyValue = oPair.first;
		if (bEmptyValue) {
			// Tile's trait empty
			if (hasEmptyValue()) {
				// The trait set contains the empty value.
				return nEmptyValueIdx; //---------------------------------------
			}
			return nTraitNotInSet; //-------------------------------------------
		}
		const int32_t nValue = oPair.second;
		const int32_t nIdx = m_oIntSet.getIndexOfValue(nValue);
		if (nIdx < 0) {
			return nTraitNotInSet; //-------------------------------------------
		}
		return nIdx;
	}
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	/** Constructs empty trait set.
	 */
	IntTraitSet() noexcept
	: m_oIntSet()
	{
	}
	/** Constructs trait set with one value.
	 * @param nValue The value.
	 */
	explicit IntTraitSet(int32_t nValue) noexcept
	: m_oIntSet(nValue)
	{
	}
	/** Constructs trait set with ordered value range.
	 * Value at index 0 is nFromValue, the last is nToValue.
	 * Parameter nFromValue can be &gt; nToValue.
	 * @param nFromValue The start value.
	 * @param nToValue The end value.
	 */
	IntTraitSet(int32_t nFromValue, int32_t nToValue) noexcept
	: m_oIntSet(nFromValue, nToValue)
	{
	}
	/** Constructs trait set with ordered stepped value range.
	 * Value at index 0 is `nFromValue`, value at index 1 is `nFromValue + nStep`
	 * provided it isn't past nToValue, etc.
	 * @param nFromValue The start value.
	 * @param nToValue The end value.
	 * @param nStep The step between generated values. Can be &lt; 0.
	 */
	IntTraitSet(int32_t nFromValue, int32_t nToValue, int32_t nStep) noexcept
	: m_oIntSet(nFromValue, nToValue, nStep)
	{
	}
	/** Constructs trait set with array of values.
	 * @param aValues The values.
	 */
	explicit IntTraitSet(const std::vector<int32_t>& aValues) noexcept
	: m_oIntSet(aValues)
	{
	}
	/** Constructs trait set from an integer set.
	 * @param oIntSet The set.
	 */
	explicit IntTraitSet(const IntSet& oIntSet) noexcept
	: m_oIntSet(oIntSet)
	{
	}
	/** Constructs trait set from an integer set.
	 * @param oIntSet The set.
	 */
	explicit IntTraitSet(IntSet&& oIntSet) noexcept
	: m_oIntSet(std::move(oIntSet))
	{
	}
	IntTraitSet(const IntTraitSet& oSource) noexcept = default;
	IntTraitSet(IntTraitSet&& oSource) noexcept = default;
	IntTraitSet& operator=(const IntTraitSet& oSource) noexcept = default;
	IntTraitSet& operator=(IntTraitSet&& oSource) noexcept = default;
	bool hasValue(int32_t nValue) const noexcept
	{
		return (m_oIntSet.getIndexOfValue(nValue) >= 0);
	}
	/** Either the value of the tile's trait or empty.
	 * @param oTile The tile
	 * @return The pair of bool telling whether value empty and the value if not empty.
	 */
	virtual std::pair<bool, int32_t> getTraitValue(const Tile& oTile) const noexcept = 0;
	/** Sets the tile's trait value.
	 * @param oTile The tile to modify.
	 * @param nValue The new value.
	 */
	virtual void setTileTraitValue(Tile& oTile, int32_t nValue) const noexcept = 0;
	/** Sets the tile's trait to empty.
	 * @param oTile The tile to modify.
	 */
	virtual void resetTileTraitValue(Tile& oTile) const noexcept = 0;
private:
	IntSet m_oIntSet;
};

} // namespace stmg

#endif /* STMG_INT_TRAIT_SET_H */

