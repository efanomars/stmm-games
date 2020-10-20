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
 * File:   commontheme.h
 */

#ifndef STMG_COMMON_THEME_H
#define STMG_COMMON_THEME_H

#include <stmm-games/util/basictypes.h>

#include <memory>
#include <string>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

namespace CommonTheme
{

struct NamedRgbColor
{
	std::string m_sName; /**< The name of the color. */
	RgbColor m_oColor; /**< The color. */
};

/** The number of available named colors.
 * @return The total number of named colors.
 */
int32_t getTotSvgColors() noexcept;
/** Get named svg color.
 * @param nPal The color index (or palette number). Must be &gt;= 0 and &lt; getTotSvgColors().
 * @return The named color.
 */
const NamedRgbColor& getSvgColor(int32_t nPal) noexcept;

} // namespace CommonTheme

} // namespace stmg

#endif	/* STMG_COMMON_THEME_H */

