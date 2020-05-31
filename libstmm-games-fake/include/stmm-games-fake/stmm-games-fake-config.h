/*
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
/*
 * File:   stmm-games-fake-config.h
 */

#ifndef _STMM_GAMES_FAKE_LIB_CONFIG_
#define _STMM_GAMES_FAKE_LIB_CONFIG_

#include <string>

namespace stmg
{

namespace libconfig
{

namespace fake
{

/** The stmm-games-fake library version.
 * @return The version string. Cannot be empty.
 */
const std::string& getVersion();

} // namespace fake

} // namespace libconfig

} // namespace stmg

#endif	/* _STMM_GAMES_FAKE_LIB_CONFIG_ */

