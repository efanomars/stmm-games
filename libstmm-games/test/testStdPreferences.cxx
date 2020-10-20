/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   testStdPreferences.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "stdconfig.h"
#include "stdpreferences.h"

#include "stmm-games-fake/fixtureStdConfig.h"
#include "stmm-games-fake/fixtureDevices.h"

#include <stmm-input-fake/stmm-input-fake.h>

#include <iostream>

namespace stmg
{

using std::shared_ptr;

namespace testing
{

class LocalStdConfigFixture : public StdConfigFixture
							, public FixtureVariantOptions_Game_GameSpeed<1,3>
							, public FixtureVariantOptions_Team_AllowMateSwap
							, public FixtureVariantOptions_Player_Rotation
{
protected:
	void setup() override
	{
		StdConfigFixture::setup();
	}
	void teardown() override
	{
		StdConfigFixture::teardown();
	}
};

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<LocalStdConfigFixture>, "Constructor")
{
	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);
	REQUIRE(refPrefs->getAppConfig() == m_refStdConfig);
	REQUIRE(refPrefs->isInEditMode());
	REQUIRE(refPrefs->getOptionValue("GameSpeed").getType() == Variant::TYPE_INT);
	REQUIRE(refPrefs->getOptionValue("GameSpeed").getInt() == 1);
//std::cout << "testStdPreferences 0" << '\n';
	REQUIRE(refPrefs->getTotTeams() == refPrefs->getAppConfig()->getAppConstraints().m_nTeamsMin);
	assert(refPrefs->getTotTeams() == 1);
	shared_ptr<AppPreferences::PrefTeam> refTeam0 = refPrefs->getTeam(0);
	REQUIRE(refTeam0.operator bool());
	REQUIRE(refPrefs->getTeam(refTeam0->getName()) == refTeam0);
	REQUIRE_FALSE(refTeam0->isAI());
	REQUIRE(refTeam0->getOptionValue("AllowSwap").getType() == Variant::TYPE_BOOL);
	REQUIRE(refTeam0->getOptionValue("AllowSwap").getBool() == false);
	REQUIRE(refTeam0->getTotMates() == 1);
	shared_ptr<AppPreferences::PrefPlayer> refMate0 = refTeam0->getMate(0);
	REQUIRE(refMate0.operator bool());
	REQUIRE_FALSE(refMate0->isAI());
	REQUIRE(refMate0->getMate() == 0);
	REQUIRE(refMate0->getTeam() == refTeam0);
	REQUIRE(refMate0->getCapabilities().empty());
	REQUIRE(refMate0->getOptionValue("CWRotation").getType() == Variant::TYPE_BOOL);
	REQUIRE(refMate0->getOptionValue("CWRotation").getBool() == false);
	//
	REQUIRE(refPrefs->getTotPlayers() == 1);
	REQUIRE(refMate0->get() == 0);
}
////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<LocalStdConfigFixture>, "SetOption")
{
	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);
	shared_ptr<StdPreferences::Team> refTeam0 = refPrefs->getTeamFull(0);
	shared_ptr<StdPreferences::Player> refMate0 = refTeam0->getMateFull(0);
	#ifndef NDEBUG
	bool bOk =
	#endif
	refPrefs->setOptionValue("GameSpeed", Variant{3});
	assert(bOk);
	REQUIRE(refPrefs->getOptionValue("GameSpeed") == Variant{3});
	#ifndef NDEBUG
	bOk =
	#endif
	refTeam0->setOptionValue("AllowSwap", Variant{true});
	assert(bOk);
	REQUIRE(refTeam0->getOptionValue("AllowSwap") == Variant{true});
	#ifndef NDEBUG
	bOk =
	#endif
	refMate0->setOptionValue("CWRotation", Variant{true});
	assert(bOk);
	REQUIRE(refMate0->getOptionValue("CWRotation") == Variant{true});
}
////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<LocalStdConfigFixture>, "OneCapabilityOnePlayer")
{
	const int32_t nKeyDeviceId = m_refDM->simulateNewDevice<stmi::testing::FakeKeyDevice>();
	shared_ptr<stmi::Device> refKeyDevice = m_refDM->getDevice(nKeyDeviceId);
	assert(refKeyDevice);
	shared_ptr<stmi::KeyCapability> refKeyCapa;
	refKeyDevice->getCapability(refKeyCapa);
	assert(refKeyCapa);
	//
	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);
	//
	const auto refPlayer0 = refPrefs->getPlayerFull(0);
	const int32_t nMoveUpId = m_refStdConfig->getKeyActionId("MoveUp");
	auto oPair = refPlayer0->getKeyValue(nMoveUpId);
	stmi::Capability* p0Capa = oPair.first;
	stmi::HARDWARE_KEY eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa.get());
	REQUIRE(eKey == stmi::HK_UP);
	const int32_t nMoveDownId = m_refStdConfig->getKeyActionId("MoveDown");
	oPair = refPlayer0->getKeyValue(nMoveDownId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa.get());
	REQUIRE(eKey == stmi::HK_DOWN);
}

////////////////////////////////////////////////////////////////////////////////
class OneMatesPerTeamStdConfigFixture : public StdConfigFixture
										, public FixtureVariantMatesPerTeamMax_One
{
protected:
	void setup() override
	{
		StdConfigFixture::setup();
	}
	void teardown() override
	{
		StdConfigFixture::teardown();
	}
};
TEST_CASE_METHOD(STFX<OneMatesPerTeamStdConfigFixture>, "OneCapabilityTwoTeamsTwoPlayers")
{
	const int32_t nKeyDeviceId1 = m_refDM->simulateNewDevice<stmi::testing::FakeKeyDevice>();
	shared_ptr<stmi::Device> refKeyDevice1 = m_refDM->getDevice(nKeyDeviceId1);
	assert(refKeyDevice1);
	shared_ptr<stmi::KeyCapability> refKeyCapa1;
	refKeyDevice1->getCapability(refKeyCapa1);
	assert(refKeyCapa1);
	//
	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);
	//
	const bool bDone = refPrefs->setTotPlayers(2);
	REQUIRE(bDone);

	REQUIRE(refPrefs->getTotPlayers() == 2);
	REQUIRE(refPrefs->getTotTeams() == 2);
	REQUIRE(refPrefs->getTotAIPlayers() == 0);
	REQUIRE(refPrefs->getTotAITeams() == 0);

	const auto refPlayer0 = refPrefs->getPlayerFull(0);
	const int32_t nMoveUpId = m_refStdConfig->getKeyActionId("MoveUp");
	auto oPair = refPlayer0->getKeyValue(nMoveUpId);
	stmi::Capability* p0Capa = oPair.first;
	stmi::HARDWARE_KEY eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa1.get());
	REQUIRE(eKey == stmi::HK_UP);
	//
	const int32_t nMoveDownId = m_refStdConfig->getKeyActionId("MoveDown");
	oPair = refPlayer0->getKeyValue(nMoveDownId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa1.get());
	REQUIRE(eKey == stmi::HK_DOWN);

	const auto refPlayer1 = refPrefs->getPlayerFull(1);
	oPair = refPlayer1->getKeyValue(nMoveUpId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa1.get());
	REQUIRE(eKey == stmi::HK_W);
	//
	oPair = refPlayer1->getKeyValue(nMoveDownId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa1.get());
	REQUIRE(eKey == stmi::HK_S);

	shared_ptr<AppPreferences::PrefPlayer> refResPlayer;
	int32_t nResKeyActionId;
	const bool bFound1 = refPrefs->getPlayerKeyActionFromCapabilityKey(refKeyCapa1->getId(), stmi::HK_W, refResPlayer, nResKeyActionId);
	REQUIRE(bFound1);
	REQUIRE(refResPlayer == refPlayer1);
	REQUIRE(nResKeyActionId == nMoveUpId);

	const bool bFound0 = refPrefs->getPlayerKeyActionFromCapabilityKey(refKeyCapa1->getId(), stmi::HK_DOWN, refResPlayer, nResKeyActionId);
	REQUIRE(bFound0);
	REQUIRE(refResPlayer == refPlayer0);
	REQUIRE(nResKeyActionId == nMoveDownId);

	const bool bFoundN = refPrefs->getPlayerKeyActionFromCapabilityKey(refKeyCapa1->getId(), stmi::HK_LEFT, refResPlayer, nResKeyActionId);
	REQUIRE_FALSE(bFoundN);

	const bool bFoundP = refPrefs->getCapabilityPlayer(refKeyCapa1->getId(), refResPlayer);
	REQUIRE_FALSE(bFoundP);

	const bool bDoneO = refPrefs->setTotPlayers(3);
	REQUIRE_FALSE(bDoneO);

	const bool bDoneO2 = refPrefs->setTotTeams(3);
	REQUIRE_FALSE(bDoneO2);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<LocalStdConfigFixture>, "TwoCapabilitiesOneTeamThreePlayers")
{
	const int32_t nKeyDeviceId1 = m_refDM->simulateNewDevice<stmi::testing::FakeKeyDevice>();
	shared_ptr<stmi::Device> refKeyDevice1 = m_refDM->getDevice(nKeyDeviceId1);
	assert(refKeyDevice1);
	shared_ptr<stmi::KeyCapability> refKeyCapa1;
	refKeyDevice1->getCapability(refKeyCapa1);
	assert(refKeyCapa1);

	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);
	//
	const int32_t nKeyDeviceId2 = m_refDM->simulateNewDevice<stmi::testing::FakeKeyDevice>();
	shared_ptr<stmi::Device> refKeyDevice2 = m_refDM->getDevice(nKeyDeviceId2);
	assert(refKeyDevice2);
	shared_ptr<stmi::KeyCapability> refKeyCapa2;
	refKeyDevice2->getCapability(refKeyCapa2);
	assert(refKeyCapa2);

	const bool bDone = refPrefs->setTotPlayers(2);
	REQUIRE(bDone);

	REQUIRE(refPrefs->getTotPlayers() == 2);
	REQUIRE(refPrefs->getTotTeams() == 1);
	REQUIRE(refPrefs->getTotAIPlayers() == 0);
	REQUIRE(refPrefs->getTotAITeams() == 0);

	//--- player 0
	const auto refPlayer0 = refPrefs->getPlayerFull(0);
	const int32_t nMoveUpId = m_refStdConfig->getKeyActionId("MoveUp");
	auto oPair = refPlayer0->getKeyValue(nMoveUpId);
	stmi::Capability* p0Capa = oPair.first;
	stmi::HARDWARE_KEY eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa1.get());
	REQUIRE(eKey == stmi::HK_UP);
	//
	const int32_t nMoveDownId = m_refStdConfig->getKeyActionId("MoveDown");
	oPair = refPlayer0->getKeyValue(nMoveDownId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa1.get());
	REQUIRE(eKey == stmi::HK_DOWN);

	//--- player 1
	const auto refPlayer1 = refPrefs->getPlayerFull(1);
	oPair = refPlayer1->getKeyValue(nMoveUpId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa2.get());
	REQUIRE(eKey == stmi::HK_UP);
	//
	oPair = refPlayer1->getKeyValue(nMoveDownId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa2.get());
	REQUIRE(eKey == stmi::HK_DOWN);

	//
	const bool bDone2 = refPrefs->setTotPlayers(3);
	REQUIRE(bDone2);

	//--- player 2
	const auto refPlayer2 = refPrefs->getPlayerFull(2);
	oPair = refPlayer2->getKeyValue(nMoveUpId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa1.get());
	REQUIRE(eKey == stmi::HK_W);
	//
	oPair = refPlayer2->getKeyValue(nMoveDownId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa1.get());
	REQUIRE(eKey == stmi::HK_S);

	//
	REQUIRE(refPrefs->getTotPlayers() == 3);
	REQUIRE(refPrefs->getTotTeams() == 1);

	//
	shared_ptr<AppPreferences::PrefPlayer> refResPlayer;
	int32_t nResKeyActionId;
	const bool bFound0 = refPrefs->getPlayerKeyActionFromCapabilityKey(refKeyCapa2->getId(), stmi::HK_DOWN, refResPlayer, nResKeyActionId);
	REQUIRE(bFound0);
	REQUIRE(refResPlayer == refPlayer1);
	REQUIRE(nResKeyActionId == nMoveDownId);

	// remove capa 2
	const bool bRemoved2 = m_refDM->simulateRemoveDevice(nKeyDeviceId2);
	REQUIRE(bRemoved2);

	// verify player 1 changed capability keys
	oPair = refPlayer1->getKeyValue(nMoveUpId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa1.get());
	REQUIRE(eKey == stmi::HK_8);
	//
	oPair = refPlayer1->getKeyValue(nMoveDownId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa1.get());
	REQUIRE(eKey == stmi::HK_2);

	// remove two players
	const bool bDone3 = refPrefs->setTotPlayers(1);
	REQUIRE(bDone3);

	//
	REQUIRE(refPrefs->getTotPlayers() == 1);
	REQUIRE(refPrefs->getTotTeams() == 1);

	const bool bFound1 = refPrefs->getPlayerKeyActionFromCapabilityKey(refKeyCapa2->getId(), stmi::HK_DOWN, refResPlayer, nResKeyActionId);
	REQUIRE_FALSE(bFound1);

	const auto refPlayer0b = refPrefs->getPlayerFull(0);
	REQUIRE(refPlayer0 == refPlayer0b);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<LocalStdConfigFixture>, "RuntimeCapabilities")
{
	const int32_t nKeyDeviceId1 = m_refDM->simulateNewDevice<stmi::testing::FakeKeyDevice>();
	shared_ptr<stmi::Device> refKeyDevice1 = m_refDM->getDevice(nKeyDeviceId1);
	assert(refKeyDevice1);
	shared_ptr<stmi::KeyCapability> refKeyCapa1;
	refKeyDevice1->getCapability(refKeyCapa1);
	assert(refKeyCapa1);

	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);

	const int32_t nKeyDeviceId2 = m_refDM->simulateNewDevice<stmi::testing::FakeKeyDevice>();
	shared_ptr<stmi::Device> refKeyDevice2 = m_refDM->getDevice(nKeyDeviceId2);
	assert(refKeyDevice2);
	shared_ptr<stmi::KeyCapability> refKeyCapa2;
	refKeyDevice2->getCapability(refKeyCapa2);
	assert(refKeyCapa2);

	const int32_t nPointerDeviceId3 = m_refDM->simulateNewDevice<stmi::testing::FakePointerDevice>();
	shared_ptr<stmi::Device> refKeyPointerDevice3 = m_refDM->getDevice(nPointerDeviceId3);
	assert(refKeyPointerDevice3);
	shared_ptr<stmi::PointerCapability> refPointerCapa3;
	refKeyPointerDevice3->getCapability(refPointerCapa3);
	assert(refPointerCapa3);

	refPrefs->setTotPlayers(3);

	refPrefs->setEditMode(false);

	const bool bDone1 = m_refDM->simulateRemoveDevice(nKeyDeviceId1);
	REQUIRE(bDone1);

	//--- player 0
	const auto refPlayer0 = refPrefs->getPlayerFull(0);
	const int32_t nMoveUpId = m_refStdConfig->getKeyActionId("MoveUp");
	auto oPair = refPlayer0->getKeyValue(nMoveUpId);
	stmi::Capability* p0Capa = oPair.first;
	stmi::HARDWARE_KEY eKey = oPair.second;
	REQUIRE(p0Capa == nullptr);
	REQUIRE(eKey == stmi::HK_UP);
	//
	const int32_t nMoveDownId = m_refStdConfig->getKeyActionId("MoveDown");
	oPair = refPlayer0->getKeyValue(nMoveDownId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == nullptr);
	REQUIRE(eKey == stmi::HK_DOWN);

	const int32_t nKeyDeviceId1b = m_refDM->simulateNewDevice<stmi::testing::FakeKeyDevice>();
	shared_ptr<stmi::Device> refKeyDevice1b = m_refDM->getDevice(nKeyDeviceId1b);
	assert(refKeyDevice1b);
	shared_ptr<stmi::KeyCapability> refKeyCapa1b;
	refKeyDevice1b->getCapability(refKeyCapa1b);
	assert(refKeyCapa1b);

	oPair = refPlayer0->getKeyValue(nMoveUpId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa1b.get());
	REQUIRE(eKey == stmi::HK_UP);

	oPair = refPlayer0->getKeyValue(nMoveDownId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa1b.get());
	REQUIRE(eKey == stmi::HK_DOWN);

	const bool bDone3 = m_refDM->simulateRemoveDevice(nKeyDeviceId1b);
	REQUIRE(bDone3);

	refPrefs->setEditMode(true);

	oPair = refPlayer0->getKeyValue(nMoveUpId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa2.get());
	REQUIRE(eKey == stmi::HK_W);

	oPair = refPlayer0->getKeyValue(nMoveDownId);
	p0Capa = oPair.first;
	eKey = oPair.second;
	REQUIRE(p0Capa == refKeyCapa2.get());
	REQUIRE(eKey == stmi::HK_S);
}
////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<LocalStdConfigFixture>, "MovePlayers")
{
	const int32_t nKeyDeviceId1 = m_refDM->simulateNewDevice<stmi::testing::FakeKeyDevice>();
	shared_ptr<stmi::Device> refKeyDevice1 = m_refDM->getDevice(nKeyDeviceId1);
	assert(refKeyDevice1);
	shared_ptr<stmi::KeyCapability> refKeyCapa1;
	refKeyDevice1->getCapability(refKeyCapa1);
	assert(refKeyCapa1);

	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);

	const int32_t nKeyDeviceId2 = m_refDM->simulateNewDevice<stmi::testing::FakeKeyDevice>();
	shared_ptr<stmi::Device> refKeyDevice2 = m_refDM->getDevice(nKeyDeviceId2);
	assert(refKeyDevice2);
	shared_ptr<stmi::KeyCapability> refKeyCapa2;
	refKeyDevice2->getCapability(refKeyCapa2);
	assert(refKeyCapa2);

	const int32_t nPointerDeviceId3 = m_refDM->simulateNewDevice<stmi::testing::FakePointerDevice>();
	shared_ptr<stmi::Device> refKeyPointerDevice3 = m_refDM->getDevice(nPointerDeviceId3);
	assert(refKeyPointerDevice3);
	shared_ptr<stmi::PointerCapability> refPointerCapa3;
	refKeyPointerDevice3->getCapability(refPointerCapa3);
	assert(refPointerCapa3);

	refPrefs->setTotTeams(2);

	REQUIRE(refPrefs->getTotPlayers() == 2);
	REQUIRE(refPrefs->getTotTeams() == 2);

	refPrefs->setTotPlayers(4);

	REQUIRE(refPrefs->getTotPlayers() == 4);
	REQUIRE(refPrefs->getTotTeams() == 2);

	auto refPlayer2 = refPrefs->getPlayerFull(2);
	REQUIRE(refPlayer2->getMate() == 1);
	REQUIRE(refPlayer2->getTeamFull()->get() == 1);
	auto& sPlayer2 = refPlayer2->getName();

	bool bMovedUp = refPrefs->playerMoveUp(sPlayer2);
	REQUIRE(bMovedUp);

	REQUIRE(refPlayer2->getMate() == 0);
	REQUIRE(refPlayer2->getTeamFull()->get() == 1);

	bMovedUp = refPrefs->playerMoveUp(sPlayer2);
	REQUIRE(bMovedUp);

	REQUIRE(refPlayer2->getMate() == 0);
	REQUIRE(refPlayer2->getTeamFull()->get() == 0);

	bMovedUp = refPrefs->playerMoveUp(sPlayer2);
	REQUIRE_FALSE(bMovedUp);

	bool bMovedDown = refPrefs->playerMoveDown(sPlayer2);
	REQUIRE(bMovedDown);

	REQUIRE(refPlayer2->getMate() == 0);
	REQUIRE(refPlayer2->getTeamFull()->get() == 1);

	bMovedDown = refPrefs->playerMoveDown(sPlayer2);
	REQUIRE(bMovedDown);

	REQUIRE(refPlayer2->getMate() == 1);
	REQUIRE(refPlayer2->getTeamFull()->get() == 1);

	bMovedDown = refPrefs->playerMoveDown(sPlayer2);
	REQUIRE(bMovedDown);

	REQUIRE(refPlayer2->getMate() == 2);
	REQUIRE(refPlayer2->getTeamFull()->get() == 1);

	bMovedDown = refPrefs->playerMoveDown(sPlayer2);
	REQUIRE_FALSE(bMovedDown);
}
////////////////////////////////////////////////////////////////////////////////
class DevicesFixtureThreeMatesPerTeam : public DevicesFixture
										, public FixtureVariantAIMatesPerTeamMax_One
										, public FixtureVariantTeamsMax_Three
										, public FixtureVariantPlayersMax_Height
{
protected:
	void setup() override
	{
		DevicesFixture::setup();
	}
	void teardown() override
	{
		DevicesFixture::teardown();
	}
};
TEST_CASE_METHOD(STFX<DevicesFixtureThreeMatesPerTeam>, "MoveTeams")
{
	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);

	REQUIRE(m_refStdConfig->getAppConstraints().getMaxTeams() == 3);
	REQUIRE(m_refStdConfig->getAppConstraints().getMaxPlayers() == 8);

	bool bDone = refPrefs->setTotTeams(3);
	REQUIRE(bDone);
	bDone = refPrefs->setTotPlayers(7);
	REQUIRE(bDone);

	auto refTeam0 = refPrefs->getTeamFull(0);
	REQUIRE(refTeam0->getTotMates() == 1);
	auto refTeam1 = refPrefs->getTeamFull(1);
	REQUIRE(refTeam1->getTotMates() == 3);
	auto refTeam2 = refPrefs->getTeamFull(2);
	REQUIRE(refTeam2->getTotMates() == 3);

	auto refPlayerX = refTeam2->getMateFull(2);

	bDone = refPrefs->playerTeamUp(refPlayerX->getName());
	REQUIRE_FALSE(bDone);

	auto refPlayerY = refTeam1->getMateFull(1);

	bDone = refPrefs->playerTeamUp(refPlayerY->getName());
	REQUIRE(bDone);
	REQUIRE(refTeam0->getTotMates() == 2);
	REQUIRE(refTeam1->getTotMates() == 2);
	REQUIRE(refTeam2->getTotMates() == 3);
	REQUIRE(refPlayerY->getTeamFull() == refTeam0);
	REQUIRE(refPlayerY->getMate() == 1);

	bDone = refPrefs->playerTeamUp(refPlayerX->getName());
	REQUIRE(bDone);
	REQUIRE(refTeam0->getTotMates() == 2);
	REQUIRE(refTeam1->getTotMates() == 3);
	REQUIRE(refTeam2->getTotMates() == 2);
	REQUIRE(refPlayerX->getTeam() == refTeam1);
	REQUIRE(refPlayerX->getMate() == 2);

	bDone = refPrefs->playerTeamUp(refPlayerX->getName());
	REQUIRE(bDone);
	REQUIRE(refTeam0->getTotMates() == 3);
	REQUIRE(refTeam1->getTotMates() == 2);
	REQUIRE(refTeam2->getTotMates() == 2);
	REQUIRE(refPlayerX->getTeam() == refTeam0);
	REQUIRE(refPlayerX->getMate() == 2);

	bDone = refPrefs->playerTeamUp(refPlayerX->getName());
	REQUIRE_FALSE(bDone);

	auto refPlayerZ = refTeam0->getMateFull(0);

	bDone = refPrefs->playerTeamDown(refPlayerZ->getName());
	REQUIRE(bDone);
	REQUIRE(refTeam0->getTotMates() == 2);
	REQUIRE(refTeam1->getTotMates() == 3);
	REQUIRE(refTeam2->getTotMates() == 2);
	REQUIRE(refPlayerZ->getTeam() == refTeam1);
	REQUIRE(refPlayerZ->getMate() == 2);
	REQUIRE(refPlayerY->getTeamFull() == refTeam0);
	REQUIRE(refPlayerY->getMate() == 0);
	REQUIRE(refPlayerX->getTeamFull() == refTeam0);
	REQUIRE(refPlayerX->getMate() == 1);

	bDone = refPrefs->playerTeamDown(refPlayerZ->getName());
	REQUIRE(bDone);
	REQUIRE(refTeam0->getTotMates() == 2);
	REQUIRE(refTeam1->getTotMates() == 2);
	REQUIRE(refTeam2->getTotMates() == 3);

	bDone = refPrefs->playerTeamDown(refPlayerZ->getName());
	REQUIRE_FALSE(bDone);
}
TEST_CASE_METHOD(STFX<DevicesFixtureThreeMatesPerTeam>, "MoveAINoMixed")
{
	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);

	bool bDone = refPrefs->setTotPlayers(2);
	REQUIRE(bDone);
	bDone = refPrefs->setTotTeams(2);
	REQUIRE(bDone);
	bDone = refPrefs->setTotPlayers(4);
	REQUIRE(bDone);

	auto refTeam0 = refPrefs->getTeamFull(0);
	auto refTeam1 = refPrefs->getTeamFull(1);
	REQUIRE(refTeam0->getTotMates() == 2);
	REQUIRE(refTeam1->getTotMates() == 2);

	auto refPlayer2 = refTeam1->getMateFull(0);
	REQUIRE_FALSE(refPlayer2->isAI());
	bDone = refPlayer2->setAI(true);
	REQUIRE_FALSE(bDone); // no mixed teams allowed!

	bDone = refPrefs->playerTeamUp(refPlayer2->getName());
	REQUIRE(bDone);

	REQUIRE(refTeam0->getTotMates() == 3);
	REQUIRE(refTeam1->getTotMates() == 1);

	auto refPlayer3 = refTeam1->getMateFull(0);
	bDone = refPlayer3->setAI(true);
	REQUIRE(bDone);

	bDone = refPrefs->playerTeamDown(refPlayer2->getName());
	REQUIRE_FALSE(bDone);

	bDone = refPrefs->playerMoveDown(refPlayer2->getName());
	REQUIRE_FALSE(bDone);
}
TEST_CASE_METHOD(STFX<DevicesFixtureThreeMatesPerTeam>, "SetName")
{
	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);

	bool bDone = refPrefs->setTotPlayers(2);
	REQUIRE(bDone);
	bDone = refPrefs->setTotTeams(2);
	REQUIRE(bDone);
	bDone = refPrefs->setTotPlayers(4);
	REQUIRE(bDone);

	auto refTeam0 = refPrefs->getTeamFull(0);
	auto refTeam1 = refPrefs->getTeamFull(1);

	auto refPlayer0 = refTeam0->getMateFull(0);
	auto refPlayer2 = refTeam1->getMateFull(0);

	bDone = refPlayer0->setName("Holly");
	REQUIRE(bDone);
	bDone = refPlayer0->setName("Holly");
	REQUIRE(bDone);

	bDone = refPlayer2->setName("Holly");
	REQUIRE_FALSE(bDone);
}
////////////////////////////////////////////////////////////////////////////////
class DevicesFixtureThreeMatesPerMixedTeam : public DevicesFixtureThreeMatesPerTeam
											, public FixtureVariantAllowMixedAIHumanTeam_True
{
};
TEST_CASE_METHOD(STFX<DevicesFixtureThreeMatesPerMixedTeam>, "MoveAIMixed")
{
	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);

	bool bDone = refPrefs->setTotPlayers(2);
	REQUIRE(bDone);
	bDone = refPrefs->setTotTeams(2);
	REQUIRE(bDone);
	bDone = refPrefs->setTotPlayers(4);
	REQUIRE(bDone);

	auto refTeam0 = refPrefs->getTeamFull(0);
	auto refTeam1 = refPrefs->getTeamFull(1);
	REQUIRE(refTeam0->getTotMates() == 2);
	REQUIRE(refTeam1->getTotMates() == 2);

	auto refPlayer2 = refTeam1->getMateFull(0);
	REQUIRE_FALSE(refPlayer2->isAI());
	bDone = refPlayer2->setAI(true);
	REQUIRE(bDone);

	auto refPlayer0 = refTeam0->getMateFull(0);
	REQUIRE_FALSE(refPlayer0->isAI());
	bDone = refPlayer0->setAI(true);
	REQUIRE(bDone);

	bDone = refPrefs->playerTeamUp(refPlayer2->getName());
	REQUIRE(bDone);
	REQUIRE_FALSE(refPlayer2->isAI()); // transformed to human!
	bDone = refPlayer2->setOptionValue(m_refStdConfig->getAIOptionName(), Variant{true});
	REQUIRE_FALSE(bDone);

	bDone = refPrefs->playerTeamDown(refPlayer2->getName());
	REQUIRE(bDone);
	REQUIRE(refPlayer2->getTeam() == refTeam1);
	REQUIRE(refTeam1->getTotMates() == 2);
	REQUIRE(refTeam1->getTotAIMates() == 0);
	REQUIRE(refPlayer2->getMate() == 1);
	bDone = refPlayer2->setAI(true);
	REQUIRE(bDone);

	auto refPlayer3 = refTeam1->getMateFull(0);
	REQUIRE(refPlayer3.operator bool());
	REQUIRE_FALSE(refPlayer3->isAI());
	bDone = refPrefs->playerTeamUp(refPlayer3->getName());
	REQUIRE(bDone);
	REQUIRE(refTeam1->getTotMates() == 1);
	REQUIRE(refTeam1->getTotAIMates() == 1);
}
////////////////////////////////////////////////////////////////////////////////
class DevicesFixtureThreeMatesPerTeamTwo : public DevicesFixtureThreeMatesPerTeam
										, public FixtureVariantTeamsMin_Two
{
};
TEST_CASE_METHOD(STFX<DevicesFixtureThreeMatesPerTeamTwo>, "AddAndRemoveTeam")
{
	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);

	REQUIRE(refPrefs->getTotTeams() == 2);
	REQUIRE(refPrefs->getTotPlayers() == 2);
	bool bDone = refPrefs->setTotTeams(2);
	REQUIRE(bDone);
	bDone = refPrefs->setTotTeams(1);
	REQUIRE_FALSE(bDone);
	REQUIRE(refPrefs->getTotTeams() == 2);
	bDone = refPrefs->setTotTeams(3);
	REQUIRE(refPrefs->getTotTeams() == 3);
	REQUIRE(refPrefs->getTotPlayers() == 3);
	bDone = refPrefs->setTotTeams(2);
	REQUIRE(bDone);
	REQUIRE(refPrefs->getTotTeams() == 2);
	REQUIRE(refPrefs->getTotPlayers() == 2);
}
TEST_CASE_METHOD(STFX<DevicesFixtureThreeMatesPerTeamTwo>, "AddAndRemovePlayer")
{
	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);

	REQUIRE(refPrefs->getTotTeams() == 2);
	REQUIRE(refPrefs->getTotPlayers() == 2);

	bool bDone = refPrefs->setTotPlayers(3);
	REQUIRE(bDone);
	REQUIRE(refPrefs->getTotPlayers() == 3);
	REQUIRE(refPrefs->getTotTeams() == 2);

	bDone = refPrefs->setTotPlayers(2);
	REQUIRE(bDone);
	REQUIRE(refPrefs->getTotPlayers() == 2);
	REQUIRE(refPrefs->getTotTeams() == 2);
}
TEST_CASE_METHOD(STFX<DevicesFixtureThreeMatesPerTeamTwo>, "SetKeyActionValue")
{
	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);

	REQUIRE(refPrefs->getTotTeams() == 2);
	REQUIRE(refPrefs->getTotPlayers() == 2);

	bool bDone = refPrefs->setTotPlayers(5);
	REQUIRE(bDone);
	REQUIRE(refPrefs->getTotPlayers() == 5);
	REQUIRE(refPrefs->getTotTeams() == 2);

	auto refTeam0 = refPrefs->getTeamFull(0);
	auto refTeam1 = refPrefs->getTeamFull(1);
	REQUIRE(refTeam0->getTotMates() == 2);
	REQUIRE(refTeam1->getTotMates() == 3);

	const int32_t nTotKeyActions = m_refStdConfig->getTotKeyActions();
	REQUIRE(nTotKeyActions >= 2);
	auto refPlayer2 = refTeam1->getMateFull(0);
	auto oPair2 = refPlayer2->getKeyValue(0);
	stmi::Capability* p0Capa2 = oPair2.first;
	stmi::HARDWARE_KEY eKey2 = oPair2.second;

	auto refPlayer3 = refTeam1->getMateFull(1);
	auto oPair3 = refPlayer3->getKeyValue(0);
	stmi::Capability* p0Capa3 = oPair3.first;
	stmi::HARDWARE_KEY eKey3 = oPair3.second;

	bool bSwap = refPlayer3->setKeyValue(0, p0Capa2, eKey2);
	REQUIRE(bSwap);

	oPair3 = refPlayer3->getKeyValue(0);
	stmi::Capability* p0Capa3b = oPair3.first;
	stmi::HARDWARE_KEY eKey3b = oPair3.second;
	REQUIRE(p0Capa3b == p0Capa2);
	REQUIRE(eKey3b == eKey2);

	oPair2 = refPlayer2->getKeyValue(0);
	stmi::Capability* p0Capa2b = oPair2.first;
	stmi::HARDWARE_KEY eKey2b = oPair2.second;
	REQUIRE(p0Capa2b == p0Capa3);
	REQUIRE(eKey2b == eKey3);

	bSwap = refPlayer3->setKeyValue(0, p0Capa3, stmi::HK_BACKSPACE);
	REQUIRE_FALSE(bSwap);
}
TEST_CASE_METHOD(STFX<DevicesFixtureThreeMatesPerTeamTwo>, "UnAssignCapability")
{
	auto refPrefs = std::make_shared<StdPreferences>(m_refStdConfig);

	bool bDone = refPrefs->setTotPlayers(7);
	REQUIRE(bDone);
	REQUIRE(refPrefs->getTotPlayers() == 7);
	REQUIRE(refPrefs->getTotTeams() == 3);

	auto refTeam0 = refPrefs->getTeamFull(0);
	auto refTeam1 = refPrefs->getTeamFull(1);
	auto refTeam2 = refPrefs->getTeamFull(2);

	auto refPlayer0 = refTeam0->getMateFull(0);
	auto refPlayer1 = refTeam1->getMateFull(0);
	auto refPlayer2 = refTeam1->getMateFull(1);
	auto refPlayer3 = refTeam2->getMateFull(0);
	bDone = refPlayer0->assignCapability(m_refJoystickCapa3);
	REQUIRE(bDone);
//std::cout << "UnAssignCapability1" << '\n';
	bDone = refPlayer1->assignCapability(m_refJoystickCapa3);
	REQUIRE(bDone);
	bDone = refPlayer1->assignCapability(m_refJoystickCapa5);
	REQUIRE(bDone);
	bDone = refPlayer1->assignCapability(m_refKeyCapa0);
	REQUIRE(bDone);
	bDone = refPlayer2->assignCapability(m_refPointerCapa2);
	REQUIRE(bDone);
	bDone = refPlayer3->assignCapability(m_refTouchCapa4);
	REQUIRE(bDone);
	REQUIRE(refPlayer0->getCapabilities().empty());
	REQUIRE(refPlayer1->getCapabilities().size() == 2);
	//
	REQUIRE(((refPlayer1->getCapabilities()[0] == m_refJoystickCapa5)
				|| (refPlayer1->getCapabilities()[0] == m_refKeyCapa0)));
	REQUIRE(((refPlayer1->getCapabilities()[1] == m_refJoystickCapa5)
				|| (refPlayer1->getCapabilities()[1] == m_refKeyCapa0)));
	REQUIRE(refPlayer1->getCapabilities()[0] != refPlayer1->getCapabilities()[1]);
	//
	REQUIRE(refPlayer2->getCapabilities().size() == 1);
	REQUIRE(refPlayer2->getCapabilities()[0] == m_refPointerCapa2);
	REQUIRE(refPlayer3->getCapabilities().size() == 1);
	REQUIRE(refPlayer3->getCapabilities()[0] == m_refTouchCapa4);
	//
	bDone = refPlayer0->unassignCapability(m_refTouchCapa4);
	REQUIRE_FALSE(bDone);
	bDone = refPlayer2->unassignCapability(m_refPointerCapa2);
	REQUIRE(bDone);
	REQUIRE(refPlayer2->getCapabilities().empty());
	bDone = refPlayer1->unassignCapability(m_refJoystickCapa3);
	REQUIRE_FALSE(bDone);
	bDone = refPlayer1->unassignCapability(m_refJoystickCapa5);
	REQUIRE(bDone);
	REQUIRE(refPlayer1->getCapabilities().size() == 1);
	REQUIRE(refPlayer1->getCapabilities()[0] == m_refKeyCapa0);
	bDone = refPlayer1->unassignCapability(m_refKeyCapa0);
	REQUIRE(bDone);
	REQUIRE(refPlayer1->getCapabilities().empty());
}

} // namespace testing

} // namespace stmg
