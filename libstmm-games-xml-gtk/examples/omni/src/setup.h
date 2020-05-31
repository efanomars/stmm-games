/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   setup.h
 */

#ifndef STMG_OMNI_SETUP_H
#define STMG_OMNI_SETUP_H

#include "nlohmann/json.hpp"

#include <string>

namespace stmg { struct MainWindowData; }

namespace stmg
{

using nlohmann::json;

/* The MainWindowData setup.
 * @return The error string or empty if no error.
 */
std::string omniSetup(MainWindowData& oMainWindowData, const json& oConf, const std::string& sOmni, const std::string& sAppVersion
						, bool bTestMode, bool bFullScreen) noexcept;

} // namespace stmg

#endif	/* STMG_OMNI_SETUP_H */

