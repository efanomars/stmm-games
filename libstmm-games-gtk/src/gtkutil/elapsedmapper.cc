/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   elapsedmapper.cc
 */

#include "gtkutil/elapsedmapper.h"

#include <cassert>
//#include <iostream>
#include <cmath>

namespace stmg
{

double ElapsedMapper::map(double fElapsed01) const noexcept
{
	assert(((fElapsed01 >= 0.0) && (fElapsed01 <= 1.0)) || (fElapsed01 == -1.0));
	if (fElapsed01 == -1.0) {
		return -1.0; //---------------------------------------------------------
	}
	if (m_eMappingType == MAPPING_TYPE_IDENTITY) {
		return applyMinMax(fElapsed01); //--------------------------------------
	}
	const double fRes = m_fA * fElapsed01 + m_fB;
	if ((fRes >= 0.0) && (fRes <= 1.0)) {
		return applyMinMax(fRes); //--------------------------------------------
	}
	if (m_eMappingType == MAPPING_TYPE_OOR_UNDEFINED) {
		return -1.0; //---------------------------------------------------------
	}
	if (m_eMappingType == MAPPING_TYPE_OOR_CLAMP) {
		if (fRes < 0.0) {
			return applyMinMax(0.0); //-----------------------------------------
		} else {
			return applyMinMax(1.0); //-----------------------------------------
		}
	}
	const double fRes2 = fRes - std::floor(fRes);
	if (m_eMappingType == MAPPING_TYPE_OOR_MODULO) {
		return applyMinMax(fRes2); //-------------------------------------------
	}
	assert(m_eMappingType == MAPPING_TYPE_OOR_ALT_MODULO);
	const int32_t nTrunc = std::floor(fRes);
	if (nTrunc % 2 != 0) {
		return applyMinMax(1.0 - fRes2); //--------------------------------------
	}
	return applyMinMax(fRes2);
}
double ElapsedMapper::applyMinMax(double fElapsed01) const noexcept
{
	if ((m_fMin == 0.0) && (m_fMax == 1.0)) {
		return fElapsed01;
	}
	assert(m_fMin >= 0.0);
	assert(m_fMax <= 1.0);
	if (m_bShrink) {
		const double fDiff = m_fMax - m_fMin;
		if (fDiff == 0.0) {
			return m_fMax; //---------------------------------------------------
		}
		assert(fDiff > 0.0);
		return m_fMin + fDiff * fElapsed01; //----------------------------------
	} else {
		// clamp
		assert(m_fMin <= m_fMax);
		if (fElapsed01 < m_fMin) {
			return m_fMin; //---------------------------------------------------
		}
		if (fElapsed01 > m_fMax) {
			return m_fMax; //---------------------------------------------------
		}
		return fElapsed01;
	}
}

} // namespace stmg
