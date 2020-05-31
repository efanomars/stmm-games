/*
 * File:   stdrandomsource.cc
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

#include "stdrandomsource.h"

#include <cassert>

namespace stmg
{

StdRandomSource::StdRandomSource() noexcept
: m_oGen(std::random_device{}())
, m_oDis(0, 1) // [0.0 .. 1.0[
{
}

int32_t StdRandomSource::random(int32_t nFrom, int32_t nTo) noexcept
{
	assert(nFrom <= nTo);
	const double fRan = m_oDis(m_oGen);
	const int64_t nRange = static_cast<int64_t>(nTo) - static_cast<int64_t>(nFrom) + 1;
	assert(nRange >= 1);
	assert(nRange <= (static_cast<int64_t>(1) << 32));
//std::cout << " ????????? fRan = " << fRan << "  nFrom=" << nFrom << "  nTo=" << nTo << "  nRange=" << nRange << '\n';
	return static_cast<int32_t>(static_cast<int64_t>(nFrom) + static_cast<int64_t>(fRan * nRange));
}

} // namespace stmg
