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
 * File:   stmm-games-xml-base-config.h
 */

#ifndef STMG_STMM_GAMES_XML_BASE_LIB_CONFIG_H
#define STMG_STMM_GAMES_XML_BASE_LIB_CONFIG_H

#include <string>
#include <vector>

namespace stmg
{

namespace libconfig
{

namespace xmlbase
{

/** The stmm-games-xml-base library version.
 * @return The version string. Cannot be empty.
 */
const std::string& getVersion();


} // namespace xmlbase

} // namespace libconfig

} // namespace stmg

#endif	/* STMG_STMM_GAMES_XML_BASE_LIB_CONFIG_H */

