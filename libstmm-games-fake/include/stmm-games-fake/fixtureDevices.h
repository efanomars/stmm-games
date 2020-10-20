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
 * File:   fixtureDevices.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_DEVICES_H
#define STMG_TESTING_FIXTURE_DEVICES_H

#include "fixtureStdConfig.h"

#include <stmm-input-ev/joystickcapability.h>
#include <stmm-input-ev/keycapability.h>
#include <stmm-input-ev/pointercapability.h>
#include <stmm-input-ev/touchcapability.h>
#include <stmm-input-fake/fakedevicemanager.h>
#include <stmm-input-fake/fakejoystickdevice.h>
#include <stmm-input-fake/fakekeydevice.h>
#include <stmm-input-fake/fakepointerdevice.h>
#include <stmm-input-fake/faketouchdevice.h>
#include <stmm-input/device.h>

#include <memory>
#include <cassert>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

namespace testing
{

class DevicesFixture : public StdConfigFixture
{
protected:
	void setup() override
	{
		StdConfigFixture::setup();

		m_nKeyDeviceId0 = m_refDM->simulateNewDevice<stmi::testing::FakeKeyDevice>();
		m_refKeyDevice0 = m_refDM->getDevice(m_nKeyDeviceId0);
		assert(m_refKeyDevice0);
		m_refKeyDevice0->getCapability(m_refKeyCapa0);
		assert(m_refKeyCapa0);

		m_nKeyDeviceId1 = m_refDM->simulateNewDevice<stmi::testing::FakeKeyDevice>();
		m_refKeyDevice1 = m_refDM->getDevice(m_nKeyDeviceId1);
		assert(m_refKeyDevice1);
		m_refKeyDevice1->getCapability(m_refKeyCapa1);
		assert(m_refKeyCapa1);

		m_nPointerDeviceId2 = m_refDM->simulateNewDevice<stmi::testing::FakePointerDevice>();
		m_refPointerDevice2 = m_refDM->getDevice(m_nPointerDeviceId2);
		assert(m_refPointerDevice2);
		m_refPointerDevice2->getCapability(m_refPointerCapa2);
		assert(m_refPointerCapa2);

		m_nJoystickDeviceId3 = m_refDM->simulateNewDevice<stmi::testing::FakeJoystickDevice>();
		m_refJoystickDevice3 = m_refDM->getDevice(m_nJoystickDeviceId3);
		assert(m_refJoystickDevice3);
		m_refJoystickDevice3->getCapability(m_refJoystickCapa3);
		assert(m_refJoystickCapa3);

		m_nTouchDeviceId4 = m_refDM->simulateNewDevice<stmi::testing::FakeTouchDevice>();
		m_refTouchDevice4 = m_refDM->getDevice(m_nTouchDeviceId4);
		assert(m_refTouchDevice4);
		m_refTouchDevice4->getCapability(m_refTouchCapa4);
		assert(m_refTouchCapa4);

		m_nJoystickDeviceId5 = m_refDM->simulateNewDevice<stmi::testing::FakeJoystickDevice>();
		m_refJoystickDevice5 = m_refDM->getDevice(m_nJoystickDeviceId5);
		assert(m_refJoystickDevice5);
		m_refJoystickDevice5->getCapability(m_refJoystickCapa5);
		assert(m_refJoystickCapa5);
	}
	void teardown() override
	{
		m_refKeyDevice0.reset();
		m_refKeyCapa0.reset();
		m_refKeyDevice1.reset();
		m_refKeyCapa1.reset();
		m_refPointerDevice2.reset();
		m_refPointerCapa2.reset();
		m_refJoystickDevice3.reset();
		m_refJoystickCapa3.reset();
		m_refTouchDevice4.reset();
		m_refTouchCapa4.reset();
		m_refJoystickDevice5.reset();
		m_refJoystickCapa5.reset();

		StdConfigFixture::teardown();
	}
public:
	int32_t m_nKeyDeviceId0;
	shared_ptr<stmi::Device> m_refKeyDevice0;
	shared_ptr<stmi::KeyCapability> m_refKeyCapa0;
	int32_t m_nKeyDeviceId1;
	shared_ptr<stmi::Device> m_refKeyDevice1;
	shared_ptr<stmi::KeyCapability> m_refKeyCapa1;
	int32_t m_nPointerDeviceId2;
	shared_ptr<stmi::Device> m_refPointerDevice2;
	shared_ptr<stmi::PointerCapability> m_refPointerCapa2;
	int32_t m_nJoystickDeviceId3;
	shared_ptr<stmi::Device> m_refJoystickDevice3;
	shared_ptr<stmi::JoystickCapability> m_refJoystickCapa3;
	int32_t m_nTouchDeviceId4;
	shared_ptr<stmi::Device> m_refTouchDevice4;
	shared_ptr<stmi::TouchCapability> m_refTouchCapa4;
	int32_t m_nJoystickDeviceId5;
	shared_ptr<stmi::Device> m_refJoystickDevice5;
	shared_ptr<stmi::JoystickCapability> m_refJoystickCapa5;

};

} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_DEVICES_H */
