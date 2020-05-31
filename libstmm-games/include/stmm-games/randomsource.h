/*
 * File:   randomsource.h
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

#ifndef STMG_RANDOM_SOURCE_H
#define STMG_RANDOM_SOURCE_H

#include <stdint.h>

namespace stmg
{

class RandomSource
{
public:
	virtual ~RandomSource() noexcept = default;

	/** Return a random value within a range.
	 * All entities within the game should use the same implementation of this function 
	 * to generate random numbers (and not keep a personal generator alongside) 
	 * so that the sequences can potentially be recorded and reproduced for debugging.
	 * @param nFrom The first of the possible values.
	 * @param nTo The last of the possible values.
	 * @return The random value &gt;= nFrom and &lt;= nTo.
	 */
	virtual int32_t random(int32_t nFrom, int32_t nTo) noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_RANDOM_SOURCE_H */

