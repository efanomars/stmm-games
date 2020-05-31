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
 *
 */
/*
 * File:   fixturevariantKeyActions.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_KEY_ACTIONS_H
#define STMG_TESTING_FIXTURE_KEY_ACTIONS_H

#ifdef STMG_TESTING_COPY_FROM_FAKE
#include "stdconfig.h"
#else
#include <stmm-games/stdconfig.h>
#endif

#include <stmm-input-ev/joystickcapability.h>
#include <stmm-input-ev/keycapability.h>
#include <stmm-input-ev/pointercapability.h>
#include <stmm-input/capability.h>
#include <stmm-input/hardwarekey.h>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include <memory>

namespace stmg
{

using std::shared_ptr;

namespace testing
{


///////////////////////////////////////////////////////////////////////////////
class FixtureVariantKeyActions_AllCapabilityClassesDefaults //DEFAULT
{
};
class FixtureVariantKeyActions_OnlyKeysDefaults
{
};
class FixtureVariantKeyActions_Custom
{
public:
	virtual std::vector<StdConfig::KeyAction> getCustomKeyActions() const = 0;
};
class FixtureVariantKeyActions
{
public:
	virtual ~FixtureVariantKeyActions() = default;
protected:
	std::vector<StdConfig::KeyAction> getKeyActions() const
	{
		auto p0Custom = dynamic_cast<FixtureVariantKeyActions_Custom const*>(this);
		if (p0Custom != nullptr) {
			return p0Custom->getCustomKeyActions();
		} else if (dynamic_cast<FixtureVariantKeyActions_OnlyKeysDefaults const*>(this) != nullptr) {
			std::vector<StdConfig::KeyAction> aKeyActions;
			aKeyActions.push_back({"MoveUp", "Move up", stmi::KeyCapability::getClass(), {stmi::HK_UP, stmi::HK_W, stmi::HK_8}});
			aKeyActions.push_back({"MoveDown", "Move down", stmi::KeyCapability::getClass(), {stmi::HK_DOWN, stmi::HK_S, stmi::HK_2}});
			return aKeyActions;
		} else {
			std::vector<StdConfig::KeyAction> aKeyActions;
			aKeyActions.push_back({{"MoveUp", "Fire"}, "Move up and Fire", {
				{stmi::KeyCapability::getClass(), {stmi::HK_UP, stmi::HK_W, stmi::HK_8}}
				, {stmi::PointerCapability::getClass(), {stmi::HK_SCROLLUP}}
				, {stmi::JoystickCapability::getClass(), {stmi::HK_UP, stmi::HK_BTN_Y}}
			}});
			aKeyActions.push_back({{"MoveDown"}, "Move down", {
				{stmi::KeyCapability::getClass(), {stmi::HK_DOWN, stmi::HK_S, stmi::HK_2}}
				, {stmi::PointerCapability::getClass(), {stmi::HK_SCROLLDOWN}}
				, {stmi::JoystickCapability::getClass(), {stmi::HK_DOWN, stmi::HK_BTN_A}}
			}});
			return aKeyActions;
		}
	}
};


} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_KEY_ACTIONS_H */
