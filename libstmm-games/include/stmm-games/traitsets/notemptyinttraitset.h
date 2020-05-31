/*
 * File:   notemptyinttraitset.h
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

#ifndef STMG_NOT_EMPTY_INT_TRAIT_SET_H
#define STMG_NOT_EMPTY_INT_TRAIT_SET_H

#include "traitsets/inttraitset.h"

#include "util/intset.h"

#include <vector>
#include <cassert>
#include <utility>

#include <stdint.h>

namespace stmg { class Tile; }

namespace stmg
{

/** Integer set that cannot be empty and cannot contain the empty value.
 */
class NotEmptyIntTraitSet : public IntTraitSet
{
public:
	NotEmptyIntTraitSet() noexcept = delete;
	explicit NotEmptyIntTraitSet(int32_t nValue) noexcept
	: IntTraitSet(nValue)
	{
	}
	NotEmptyIntTraitSet(int32_t nFromValue, int32_t nToValue) noexcept
	: IntTraitSet(nFromValue, nToValue)
	{
	}
	NotEmptyIntTraitSet(int32_t nFromValue, int32_t nToValue, int32_t nStep) noexcept
	: IntTraitSet(nFromValue, nToValue, nStep)
	{
	}
	explicit NotEmptyIntTraitSet(const std::vector<int32_t>& aValues) noexcept
	: IntTraitSet(aValues)
	{
		assert(!aValues.empty());
	}
	explicit NotEmptyIntTraitSet(const IntSet& oIntSet) noexcept
	: IntTraitSet(oIntSet)
	{
		assert(oIntSet.size() > 0);
	}
	explicit NotEmptyIntTraitSet(IntSet&& oIntSet) noexcept
	: IntTraitSet(std::move(oIntSet))
	{
		assert(oIntSet.size() > 0);
	}
	NotEmptyIntTraitSet(const NotEmptyIntTraitSet& oSource) noexcept = default;
	NotEmptyIntTraitSet(NotEmptyIntTraitSet&& oSource) noexcept = default;
	NotEmptyIntTraitSet& operator=(const NotEmptyIntTraitSet& oSource) noexcept = default;
	NotEmptyIntTraitSet& operator=(NotEmptyIntTraitSet&& oSource) noexcept = default;

	bool hasEmptyValue() const noexcept override final
	{
		return false;
	}
	int32_t getTotValues() const noexcept override final
	{
		return IntTraitSet::getTotValues();
	}
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	void resetTileTraitValue(Tile& /*oTile*/) const noexcept override final
	{
		assert(false);
	}
};

} // namespace stmg

#endif /* STMG_NOT_EMPTY_INT_TRAIT_SET_H */
