/*
 * File:   coords.h
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

#ifndef STMG_COORDS_H
#define STMG_COORDS_H

#include "basictypes.h"
#include "util.h"

#include <unordered_map>
#include <utility>

#include <stdint.h>

namespace stmg
{

/** Coords class.
 * Beware! This class is final, only meant to be subclassed by TileCoords.
 */
class Coords
{
public:
	/** Constructor.
	 */
	Coords() noexcept;
	/** Constructor.
	 * @param nAtLeastSize The indicative minimum number of unique values.
	 */
	explicit Coords(int32_t nAtLeastSize) noexcept;

	/** Clears the instance.
	 * All iterators become invalid.
	 */
	void reInit() noexcept;
	/** Clears the instance.
	 * All iterators become invalid.
	 * @param nAtLeastSize The indicative minimum number of unique values.
	 */
	void reInit(int32_t nAtLeastSize) noexcept;
	/** The number of (unique) coords in the instance.
	 * @return The size.
	 */
	int32_t size() const noexcept;
	/** Whether set is empty.
	 * @return Whether empty.
	 */
	bool isEmpty() const noexcept;
	/** Add a coord.
	 * Might invalidate iterators.
	 * @param nX The x.
	 * @param nY The y.
	 */
	void add(int32_t nX, int32_t nY) noexcept;
	/** Add a coord.
	 * Might invalidate iterators.
	 * @param oXY The coord to add.
	 */
	void add(NPoint oXY) noexcept;
	/** Add all cords in a rectangle.
	 * Might invalidate iterators.
	 *
	 * Example: add(1,2,2,1) adds coords (1,2), (2,2).
	 * @param nX The x.
	 * @param nY The y.
	 * @param nW The width. Must be &gt; 0.
	 * @param nH The height. Must be &gt; 0.
	 */
	void addRect(int32_t nX, int32_t nY, int32_t nW, int32_t nH) noexcept;
	/** Add all cords in a rectangle.
	 * See addRect(int32_t nX, int32_t nY, int32_t nW, int32_t nH).
	 * @param oRect The rectangle.
	 */
	void addRect(NRect oRect) noexcept;
	/** Whether the instance contains a coord.
	 * @param nX The x.
	 * @param nY The y.
	 * @return Whether the instance contains a ccord.
	 */
	bool contains(int32_t nX, int32_t nY) const noexcept;
	/** Whether the instance contains a coord.
	 * @param oXY The coord.
	 * @return Whether the instance contains a ccord.
	 */
	bool contains(NPoint oXY) const noexcept; 
	/** Removes a coord if it exists.
	 * @param nX The x.
	 * @param nY The y.
	 * @return Whether the coord existed.
	 */
	bool remove(int32_t nX, int32_t nY) noexcept;
	/** Removes a coord if it exists.
	 * @param oXY The coord.
	 * @return Whether the coord existed.
	 */
	bool remove(NPoint oXY) noexcept;
	/** Remove all coords within a rect.
	 * @param nX The x.
	 * @param nY The y.
	 * @param nW The width. Must be &gt; 0.
	 * @param nH The height. Must be &gt; 0.
	 */
	void removeInRect(int32_t nX, int32_t nY, int32_t nW, int32_t nH) noexcept;
	/** Add all cords in a rectangle.
	 * See removeInRect(int32_t nX, int32_t nY, int32_t nW, int32_t nH).
	 * @param oRect The rectangle.
	 */
	void removeInRect(NRect oRect) noexcept;
	/** Get the smallest recangle containing all the coords.
	 * @return The rectangle. Has width 0 if this instance is empty.
	 */
	NRect getMinMax() const noexcept;
	/** Add another coords to this instance.
	 * @param oCoords Thee coords to add.
	 */
	void add(const Coords& oCoords) noexcept;
	/** Remove all the coords of another instance,
	 * @param oCoords The coords to remove,
	 */
	void remove(const Coords& oCoords) noexcept;

public:
	friend class const_iterator;
	/** Coords iterator.
	 */
	class const_iterator
	{
	public:
		/** The current x position.
		 * @return The x.
		 */
		inline int32_t x() const noexcept { return Util::unpackPointFromInt64(m_it->first).m_nX; }
		/** The current y position.
		 * @return The y.
		 */
		inline int32_t y() const noexcept { return Util::unpackPointFromInt64(m_it->first).m_nY; }
		/** The current position.
		 * @return The position.
		 */
		inline NPoint point() const noexcept { return Util::unpackPointFromInt64(m_it->first); }
		/** Move the iterator to the next coord.
		 */
		inline void next() noexcept { ++m_it; };
		/** Tells whether the iterator point to the same position as another.
		 * @param it The other iterator.
		 * @return Whether same position within Coords.
		 */
		inline bool operator==(const const_iterator& it) const noexcept { return (m_it == it.m_it); }
		/** Opposite of const_iterator::operator==().
		 */
		inline bool operator!=(const const_iterator& it) const noexcept { return !(m_it == it.m_it); }
	protected:
		friend class TileCoords;
		inline int32_t get() const noexcept { return m_it->second; }
	private:
		friend class Coords;
		explicit const_iterator(std::unordered_map<int64_t, int32_t>::const_iterator it) noexcept;
	private:
		std::unordered_map<int64_t, int32_t>::const_iterator m_it;
	};

	inline const_iterator begin() const noexcept
	{
		return const_iterator(m_oXY.begin());
	}
	inline Coords::const_iterator end() const noexcept
	{
		return const_iterator(m_oXY.end());
	}

	bool remove(const const_iterator& it) noexcept;

protected:
	explicit Coords(bool bTileCoords) noexcept;
	Coords(bool bTileCoords, int32_t nAtLeastSize) noexcept;
	/** Returns the iterator.
	 * @param nX The x.
	 * @param nY The y.
	 * @return A valid iterator, Coords::end() if not found.
	 */
	const_iterator find(int32_t nX, int32_t nY) const noexcept;
	
	int32_t& getOrCreate(NPoint oXY) noexcept;
	/* Reinitialization.
	 * @param nAtLeastSize
	 */
	void clearData(int32_t nAtLeastSize) noexcept;
private:
	void addData(int32_t nX, int32_t nY) noexcept;
	std::unordered_map<int64_t, int32_t>::iterator removePriv(const std::unordered_map<int64_t, int32_t>::const_iterator& itFind) noexcept;

private:
	mutable std::unordered_map<int64_t, int32_t> m_oXY; // <oXY, index>
	mutable NRect m_oBoundingRect;
	bool m_bTileCoords;
};

} // namespace stmg

#endif	/* STMG_COORDS_H */

