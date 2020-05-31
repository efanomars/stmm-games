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
 * File:   fixtureStdPreferences.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_STD_PREFERENCES_H
#define STMG_TESTING_FIXTURE_STD_PREFERENCES_H

#include "fixtureStdConfig.h"

#include "fixturevariantPrefsDevices.h"
#include "fixturevariantPrefsPlayers.h"

#ifdef STMG_TESTING_COPY_FROM_FAKE
#include "stdpreferences.h"
#else
#include <stmm-games/stdpreferences.h>
#endif

#include <stmm-input-fake/fakedevicemanager.h>
#include <stmm-input-fake/fakejoystickdevice.h>
#include <stmm-input-fake/fakekeydevice.h>
#include <stmm-input-fake/fakepointerdevice.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>
#include <cassert>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

namespace testing
{

class StdPreferencesFixture : public StdConfigFixture
							, public FixtureVariantDevicesKeys, public FixtureVariantDevicesJoystick
							, public FixtureVariantPrefsPlayers
{
protected:
	void setup() override
	{
		StdConfigFixture::setup();
		//
		assert(m_refDM);
		const int32_t nTotDevicesKeys = FixtureVariantDevicesKeys::getDevicesKeys();
		for (int32_t nIdx = 0; nIdx < nTotDevicesKeys; ++nIdx) {
			m_aKeyDeviceIds.push_back(m_refDM->simulateNewDevice<stmi::testing::FakeKeyDevice>());
		}
		//
		m_nPointerDeviceId = m_refDM->simulateNewDevice<stmi::testing::FakePointerDevice>();
		//
		const int32_t nTotDevicesJoystick = FixtureVariantDevicesJoystick::getDevicesJoystick();
		for (int32_t nIdx = 0; nIdx < nTotDevicesJoystick; ++nIdx) {
			m_aKeyDeviceIds.push_back(m_refDM->simulateNewDevice<stmi::testing::FakeJoystickDevice>());
		}

		m_refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);
		// Adds players,. If variant not compatible with app constraints asserts!
		FixtureVariantPrefsPlayers::initStdPreferences(m_refPrefs);
	}
	void teardown() override
	{
		m_refPrefs.reset();
		StdConfigFixture::teardown();
	}
public:
	std::vector< int32_t > m_aKeyDeviceIds;
	int32_t m_nPointerDeviceId;
	std::vector< int32_t > m_aJoystickDeviceIds;
	//
	shared_ptr<StdPreferences> m_refPrefs;
};

} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_STD_PREFERENCES_H */
