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
 * File:   stdrandomsource.h
 */

#ifndef STMG_STD_RANDOM_SOURCE_H
#define STMG_STD_RANDOM_SOURCE_H

#include "randomsource.h"

#include <random>

#include <stdint.h>

namespace stmg
{

class StdRandomSource : public RandomSource
{
public:
	StdRandomSource() noexcept;
	/** Generate a random integer for an interval.
	 * @param nFrom The start of the interval.
	 * @param nTo The end of the interval. Must be &gt;= nFrom.
	 * @return The random number. Is &gt;= nFrom and &lt;= nTo.
	 */
	int32_t random(int32_t nFrom, int32_t nTo) noexcept override;
private:
	std::mt19937 m_oGen;
	std::uniform_real_distribution<double> m_oDis;
};

} // namespace stmg

#endif	/* STMG_STD_RANDOM_SOURCE_H */

