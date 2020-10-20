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
 * File:   elapsedmapper.h
 */

#ifndef STMG_ELAPSED_MAPPER_H
#define STMG_ELAPSED_MAPPER_H

#include <stdint.h>

namespace stmg
{

/** Elapsed mapper.
 * A linear function `a * elapsed + b` is applied according to MAPPING_TYPE. If elapsed
 * is `-1.0` the same value is returned, with no further processing. Otherwise
 * an additional optional clamping or shrinking to a min and max value is applied.
 */
class ElapsedMapper {
public:
	enum MAPPING_TYPE
	{
		  MAPPING_TYPE_IDENTITY = 0 /**< The identity mapping, as if m_fA is `1.0` and m_fB is `0.0`. m_fA and m_fB are ignored, but min-max is still applied. */
		, MAPPING_TYPE_OOR_UNDEFINED = 1 /**< When the (linear) function maps outside of `(0.0, 1.0)` the result is undefined (`-1.0`) and all other members are ignored. */
		, MAPPING_TYPE_OOR_CLAMP = 2 /**< When the function maps outside of `(0.0, 1.0)` the 
										* result is clamped to `0.0` for negative values and `1.0` for positive values.
										* Example: `-3.5` is mapped to `0.0`.*/
		, MAPPING_TYPE_OOR_MODULO = 3 /**< When the function maps outside of `(0.0, 1.0)` the result is `y - std::floor(y)`.
										* Example: If 'a * x + b' returns value `1.7`, The result is mapped to `0.7`.
										* `-1.7` on the other hand would be mapped to `0.3`. */
		, MAPPING_TYPE_OOR_ALT_MODULO = 4 /**< Same as MAPPING_TYPE_OOR_MODULO but the result is subtracted to `1.0`
										* if `((int)std::trunc(a * x + b)) % 2` is not `0`.
										* Example: `-0.1` is mapped to '0.1'.
										* Example: `-1.1` is mapped to '0.9'.
										* Example: `-2.1` is mapped to '0.1'. */
	};
	MAPPING_TYPE m_eMappingType = MAPPING_TYPE_IDENTITY; /**< Default is MAPPING_TYPE_IDENTITY. */
	double m_fA = 1.0; /**< The 'a' in the mapping function 'y = a * x + b'. Default is 1.  */
	double m_fB = 0.0; /**< The 'b' in the mapping function 'y = a * x + b'. Default is 0.  */
	double m_fMin = 0.0; /**< The min value. Must be within interval `(0.0, 1.0)`. Default is 0.  */
	double m_fMax = 1.0; /**< The max value. Must be within interval `(0.0, 1.0)` and be bigger than m_fMin. Default is 1.  */
	bool m_bShrink = false; /**< Apply shrinking to (m_fMin, m_fMax) rather than clamping. Default is false. */

public:
	/** The mapping.
	 * @param fElapsed01 The input. Must be a non negative number not bigger than `1.0`.
	 * @return The result. Is either `-1` or a non negative number not bigger than `1.0`.
	 */
	double map(double fElapsed01) const noexcept;
private:
	double applyMinMax(double fElapsed01) const noexcept;
};


} // namespace stmg

#endif	/* STMG_ELAPSED_MAPPER_H */

