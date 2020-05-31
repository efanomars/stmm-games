/*
 * File:   traitset.h
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

#ifndef STMG_TRAIT_SET_H
#define STMG_TRAIT_SET_H

#include <stdint.h>

namespace stmg
{

class Tile;


/** Ordered set of a tile's trait values.
 * A trait value is of type int32_t, but it can also be 'empty'.
 * The values are ordered and have an index. The index of the empty value,
 * if present, is always `-1`.
 */
class TraitSet
{
public:
	virtual ~TraitSet() noexcept = default;
	/** Get the total number of values in this tile trait indexed set.
	 * The empty value, if present, is also counted.
	 * @return The number of values. Is &gt;= 0.
	 */
	virtual int32_t getTotValues() const noexcept = 0;
	/** Whether the set contains the empty value.
	 * @return Whether the set contains the empty value.
	 */
	virtual bool hasEmptyValue() const noexcept = 0;
	/** Sets the trait of a tile to the value identified by an index.
	 * If the set contains the empty value the valid indexes are
	 * &gt;=-1 and &lt; (getTotValues()-1).
	 * 
	 * If the set doesn't contain the empty value the valid indexes are
	 * &gt;=0 and &lt; getTotValues().
	 * 
	 * If the -1 index is passed for a set without empty value, the tile is 
	 * left untouched and `false` is returned, otherwise `true` is returned.
	 * @param oTile The tile to modify.
	 * @param nIdx The index of the value in the set.
	 * @return Whether the operation was successful.
	 */
	virtual bool setTileTraitValueByIndex(Tile& oTile, int32_t nIdx) const noexcept = 0;
	/** Gets the index of the value of the tile's trait in the set.
	 * If the tile's trait is empty and the set contains the empty value
	 * index -1 is returned.
	 * @param oTile The tile the trait's value of which the index of is needed.
	 * @return The index or -2 if trait value not in the trait set.
	 */
	virtual int32_t getIndexOfTileTraitValue(const Tile& oTile) const noexcept = 0;
	/** Dumps the trait set.
	 * This only works if NDEBUG macro is not defined.
	 */
	virtual void dump(int32_t nIndentSpaces, bool bHeader) const noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_TRAIT_SET_H */

