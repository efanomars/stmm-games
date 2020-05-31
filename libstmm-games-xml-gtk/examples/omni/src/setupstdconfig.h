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
 * File:   setupstdconfig.h
 */

#ifndef STMG_OMNI_SETUP_STD_CONFIG_H
#define STMG_OMNI_SETUP_STD_CONFIG_H

#include "nlohmann/json.hpp"

#include <string>
#include <memory>

namespace stmg { class StdConfig; }
namespace stmi { class DeviceManager; }

namespace stmg
{

using nlohmann::json;

using std::shared_ptr;

/* The StdConfig setup.
 */
std::string omniSetupStdConfig(shared_ptr<StdConfig>& refStdConfig, const json& oConf, const shared_ptr<stmi::DeviceManager>& refDeviceManager
								, const std::string& sOmni, const std::string& sAppVersion, bool bTestMode) noexcept;

} // namespace stmg

#endif	/* STMG_OMNI_SETUP_STD_CONFIG_H */

