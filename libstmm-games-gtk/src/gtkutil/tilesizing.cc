/*
 * File:   tilesizing.cc
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

#include "gtkutil/tilesizing.h"

#include <stmm-games/util/basictypes.h>

#include <cassert>
//#include <iostream>

namespace stmg
{

bool TileSizing::isValid() const noexcept
{
	if (m_nMinW <= 0) {
		return false;
	}
	if (m_nMinW > m_nMaxW) {
		return false;
	}
	if (m_nMinH <= 0) {
		return false;
	}
	if (m_nMinH > m_nMaxH) {
		return false;
	}
	if ((m_fWHRatio < 0.001) || (m_fWHRatio > 1000.0)) {
		return false;
	}
	return true;
}

NSize TileSizing::getBest(int32_t nReqW) const noexcept
{
	assert(isValid());
	//
	int32_t nW;
	if (nReqW < m_nMinW) {
		nW = m_nMinW;
	} else if (nReqW > m_nMaxW) {
		nW = m_nMaxW;
	} else {
		nW = nReqW;
	}
	int32_t nH = 1.0 * nW / m_fWHRatio;
	if ((nH < m_nMinH) || (nH > m_nMaxH)) {
		if (nH < m_nMinH) {
			nH = m_nMinH;
			nW = nH * m_fWHRatio;
			// we ignore if nW gets out of range!
		}
	}
	return NSize{nW, nH};
}

	
} // namespace stmg
