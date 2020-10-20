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
 * File:   testStdConfig.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "stdconfig.h"
#include "options/booloption.h"
#include "options/intoption.h"

#include <stmm-input-fake/stmm-input-fake.h>

namespace stmg
{

using std::shared_ptr;

namespace testing
{

TEST_CASE("testStdConfig, Constructor")
{
	auto refDM = std::make_shared<stmi::testing::FakeDeviceManager>();

	AppConstraints oAppConstraints;
	oAppConstraints.m_bAllowMixedAIHumanTeam = false;
	oAppConstraints.m_nTeamsMin = 1;
	oAppConstraints.m_nTeamsMax = 2;
	oAppConstraints.m_nMatesPerTeamMax = 1;
	oAppConstraints.m_nPlayersMax = 3;
	oAppConstraints.m_nAIMatesPerTeamMax = 0;
	REQUIRE(oAppConstraints.isValid());

	const bool bSoundEnabled = true;
	const bool bPerPlayerSoundAllowed = false;

	const std::vector<StdConfig::KeyAction> aKeyActions{
		{"MoveUp", "Move up", stmi::KeyCapability::getClass(), {stmi::HK_UP, stmi::HK_W, stmi::HK_8}}
		, {"MoveDown", "Move down", stmi::KeyCapability::getClass(), {stmi::HK_DOWN, stmi::HK_S, stmi::HK_2}}
		};
	const std::vector< shared_ptr<Option> > aOptions;
	StdConfig::CapabilityAssignment oCapabilityAssignment;
	oCapabilityAssignment.m_nMaxCapabilitiesExplicitlyAssignedToPlayer = 1000;
	oCapabilityAssignment.m_bCapabilitiesAutoAssignedToActivePlayer = true;

	StdConfig::Init oStdConfigInit;
	oStdConfigInit.m_sAppName = "TestApp";
	oStdConfigInit.m_sAppVersion = "0.0";
	oStdConfigInit.m_refDeviceManager = refDM;
	oStdConfigInit.m_oAppConstraints = oAppConstraints;
	oStdConfigInit.m_oCapabilityAssignment = oCapabilityAssignment;
	oStdConfigInit.m_aKeyActions = aKeyActions;
	oStdConfigInit.m_aOptions = aOptions;
	oStdConfigInit.m_bSoundEnabled = bSoundEnabled;
	oStdConfigInit.m_bSoundPerPlayerAllowed = bPerPlayerSoundAllowed;
	auto refStdConfig = std::make_shared<StdConfig>(std::move(oStdConfigInit));
	REQUIRE(refStdConfig->getDeviceManager() == refDM);
	const AppConstraints& oResAppConstraints = refStdConfig->getAppConstraints();
	REQUIRE(oResAppConstraints.isValid());
	REQUIRE(oResAppConstraints.m_bAllowMixedAIHumanTeam == oAppConstraints.m_bAllowMixedAIHumanTeam);
	REQUIRE(oResAppConstraints.m_nTeamsMin == oAppConstraints.m_nTeamsMin);
	REQUIRE(oResAppConstraints.m_nTeamsMax == oAppConstraints.m_nTeamsMax);
	REQUIRE(oResAppConstraints.m_nMatesPerTeamMax == oAppConstraints.m_nMatesPerTeamMax);
	REQUIRE(oResAppConstraints.m_nPlayersMax == oAppConstraints.m_nPlayersMax);

	REQUIRE(refStdConfig->getTotKeyActions() == 2);
	const int32_t nMoveUpId = refStdConfig->getKeyActionId("MoveUp");
	REQUIRE(nMoveUpId >= 0);
	const int32_t nMoveDownId = refStdConfig->getKeyActionId("MoveDown");
	REQUIRE(nMoveDownId >= 0);

	auto& oKAMoveUp = refStdConfig->getKeyAction(nMoveUpId);
	REQUIRE(oKAMoveUp.getNames().size() == 1);
	REQUIRE(oKAMoveUp.getNames()[0] == "MoveUp");
	REQUIRE(oKAMoveUp.getDescription() == "Move up");
	REQUIRE(oKAMoveUp.getClasses().size() == 1);
	REQUIRE(oKAMoveUp.getClasses()[0] == stmi::KeyCapability::getClass());
	REQUIRE(oKAMoveUp.getClassDefaultKeys(stmi::KeyCapability::getClass()).size() == 3);
	auto& oKAMoveDown = refStdConfig->getKeyAction(nMoveDownId);
	REQUIRE(oKAMoveDown.getNames().size() == 1);
	REQUIRE(oKAMoveDown.getNames()[0] == "MoveDown");
	REQUIRE(oKAMoveDown.getDescription() == "Move down");

	const NamedObjIndex< shared_ptr<Option> >& oGameOptions = refStdConfig->getOptions(OwnerType::GAME);
	const NamedObjIndex< shared_ptr<Option> >& oTeamOptions = refStdConfig->getOptions(OwnerType::TEAM);
	const NamedObjIndex< shared_ptr<Option> >& oPlayerOptions = refStdConfig->getOptions(OwnerType::PLAYER);
	REQUIRE(oGameOptions.size() == 4); // AI + SoundEnabled + PerplayerSound + Volume
	REQUIRE(oTeamOptions.size() == 1);
	REQUIRE(oPlayerOptions.size() == 2); // AI + Volume
	const int32_t nGameAIIdx = oGameOptions.getIndex(refStdConfig->getAIOptionName());
	REQUIRE(nGameAIIdx >= 0);
	const int32_t nTeamAIIdx = oTeamOptions.getIndex(refStdConfig->getAIOptionName());
	REQUIRE(nTeamAIIdx >= 0);
	const int32_t nPlayerAIIdx = oPlayerOptions.getIndex(refStdConfig->getAIOptionName());
	REQUIRE(nPlayerAIIdx >= 0);

	const bool bAllowAI = (oAppConstraints.m_nAIMatesPerTeamMax > 0);
	const auto& refGameAIOption = oGameOptions.getObj(nGameAIIdx);
	REQUIRE(dynamic_cast<BoolOption*>(refGameAIOption.get()) != nullptr);
	REQUIRE(refGameAIOption->isReadonly());
	REQUIRE_FALSE(refGameAIOption->isVisible());
	REQUIRE(refGameAIOption->getDefaultValue() == Variant{bAllowAI});

	const auto& refTeamAIOption = oTeamOptions.getObj(nTeamAIIdx);
	REQUIRE(dynamic_cast<BoolOption*>(refTeamAIOption.get()) != nullptr);
	REQUIRE(refTeamAIOption->isReadonly());
	REQUIRE(refTeamAIOption->isVisible() == bAllowAI);
	REQUIRE(refTeamAIOption->getDesc() == "AI team");

	const auto& refPlayerAIOption = oPlayerOptions.getObj(nPlayerAIIdx);
	REQUIRE(dynamic_cast<BoolOption*>(refPlayerAIOption.get()) != nullptr);
	REQUIRE(refPlayerAIOption->isReadonly() == !bAllowAI);
	REQUIRE(refPlayerAIOption->isVisible() == bAllowAI);
	REQUIRE(refPlayerAIOption->getDefaultValue() == Variant{false});
	REQUIRE(refPlayerAIOption->getDesc() == "AI player");

	const int32_t nGamePerPlayerSoundIdx = oGameOptions.getIndex(refStdConfig->getPerPlayerSoundOptionName());
	REQUIRE(nGamePerPlayerSoundIdx >= 0);

	const auto& refGamePerPlayerSoundOption = oGameOptions.getObj(nGamePerPlayerSoundIdx);
	REQUIRE(dynamic_cast<BoolOption*>(refGamePerPlayerSoundOption.get()) != nullptr);
	REQUIRE(refGamePerPlayerSoundOption->isReadonly() != bPerPlayerSoundAllowed);
	REQUIRE(refGamePerPlayerSoundOption->isVisible() == bPerPlayerSoundAllowed);
	REQUIRE(refGamePerPlayerSoundOption->getDefaultValue() == Variant{false});

	const int32_t nGameSoundVolumeSoundIdx = oGameOptions.getIndex(refStdConfig->getSoundVolumeOptionName());
	REQUIRE(nGameSoundVolumeSoundIdx >= 0);

	const auto& refGameSoundVolumeOption = oGameOptions.getObj(nGameSoundVolumeSoundIdx);
	REQUIRE(dynamic_cast<IntOption*>(refGameSoundVolumeOption.get()) != nullptr);
	REQUIRE(refGameSoundVolumeOption->isReadonly() != bSoundEnabled);
	REQUIRE(refGameSoundVolumeOption->isVisible() == bSoundEnabled);
	REQUIRE(refGameSoundVolumeOption->getDefaultValue() == Variant{bSoundEnabled ? 100 : 0});

	const int32_t nPlayerSoundVolumeSoundIdx = oPlayerOptions.getIndex(refStdConfig->getSoundVolumeOptionName());
	REQUIRE(nPlayerSoundVolumeSoundIdx >= 0);

	const auto& refPlayerSoundVolumeOption = oGameOptions.getObj(nGameSoundVolumeSoundIdx);
	REQUIRE(dynamic_cast<IntOption*>(refPlayerSoundVolumeOption.get()) != nullptr);
	REQUIRE(refPlayerSoundVolumeOption->isReadonly() != bSoundEnabled);
	REQUIRE(refPlayerSoundVolumeOption->isVisible() == bSoundEnabled);
	REQUIRE(refPlayerSoundVolumeOption->getDefaultValue() == Variant{bSoundEnabled ? 100 : 0});
}

TEST_CASE("testStdConfig, Constructor2KeyActions")
{
	auto refDM = std::make_shared<stmi::testing::FakeDeviceManager>();

	AppConstraints oAppConstraints;
	oAppConstraints.m_bAllowMixedAIHumanTeam = true;
	oAppConstraints.m_nTeamsMin = 2;
	oAppConstraints.m_nTeamsMax = 3;
	oAppConstraints.m_nMatesPerTeamMax = 3;
	oAppConstraints.m_nPlayersMax = 5;
	oAppConstraints.m_nAIMatesPerTeamMax = 3;
	REQUIRE(oAppConstraints.isValid());

	const bool bSoundEnabled = true;
	const bool bPerPlayerSoundAllowed = true;

	const std::vector<StdConfig::KeyAction> aKeyActions{
		{"MoveUp", "Move up", stmi::KeyCapability::getClass(), {stmi::HK_UP, stmi::HK_W, stmi::HK_8}}
		, {"MoveDown", "Move down", stmi::KeyCapability::getClass(), {stmi::HK_DOWN, stmi::HK_S, stmi::HK_2}}
		};
	const std::vector< shared_ptr<Option> > aOptions;
	StdConfig::CapabilityAssignment oCapabilityAssignment;
	oCapabilityAssignment.m_nMaxCapabilitiesExplicitlyAssignedToPlayer = 1000;
	oCapabilityAssignment.m_bCapabilitiesAutoAssignedToActivePlayer = true;

	StdConfig::Init oStdConfigInit;
	oStdConfigInit.m_sAppName = "TestApp";
	oStdConfigInit.m_sAppVersion = "0.0";
	oStdConfigInit.m_refDeviceManager = refDM;
	oStdConfigInit.m_oAppConstraints = oAppConstraints;
	oStdConfigInit.m_oCapabilityAssignment = oCapabilityAssignment;
	oStdConfigInit.m_aKeyActions = aKeyActions;
	oStdConfigInit.m_aOptions = aOptions;
	oStdConfigInit.m_bSoundEnabled = bSoundEnabled;
	oStdConfigInit.m_bSoundPerPlayerAllowed = bPerPlayerSoundAllowed;
	auto refStdConfig = std::make_shared<StdConfig>(std::move(oStdConfigInit));
	REQUIRE(refStdConfig->getDeviceManager() == refDM);
	const AppConstraints& oResAppConstraints = refStdConfig->getAppConstraints();
	REQUIRE(oResAppConstraints.isValid());
	REQUIRE(oResAppConstraints.m_bAllowMixedAIHumanTeam == oAppConstraints.m_bAllowMixedAIHumanTeam);
	REQUIRE(oResAppConstraints.m_nTeamsMin == oAppConstraints.m_nTeamsMin);
	REQUIRE(oResAppConstraints.m_nTeamsMax == oAppConstraints.m_nTeamsMax);
	REQUIRE(oResAppConstraints.m_nMatesPerTeamMax == oAppConstraints.m_nMatesPerTeamMax);
	REQUIRE(oResAppConstraints.m_nPlayersMax == oAppConstraints.m_nPlayersMax);

	REQUIRE(refStdConfig->getTotKeyActions() == 2);
	const int32_t nMoveUpId = refStdConfig->getKeyActionId("MoveUp");
	REQUIRE(nMoveUpId >= 0);
	const int32_t nMoveDownId = refStdConfig->getKeyActionId("MoveDown");
	REQUIRE(nMoveDownId >= 0);

	auto& oKAMoveUp = refStdConfig->getKeyAction(nMoveUpId);
	REQUIRE(oKAMoveUp.getNames().size() == 1);
	REQUIRE(oKAMoveUp.getNames()[0] == "MoveUp");
	REQUIRE(oKAMoveUp.getDescription() == "Move up");
	REQUIRE(oKAMoveUp.getClasses().size() == 1);
	REQUIRE(oKAMoveUp.getClasses()[0] == stmi::KeyCapability::getClass());
	REQUIRE(oKAMoveUp.getClassDefaultKeys(stmi::KeyCapability::getClass()).size() == 3);
	auto& oKAMoveDown = refStdConfig->getKeyAction(nMoveDownId);
	REQUIRE(oKAMoveDown.getNames().size() == 1);
	REQUIRE(oKAMoveDown.getNames()[0] == "MoveDown");
	REQUIRE(oKAMoveDown.getDescription() == "Move down");

	const NamedObjIndex< shared_ptr<Option> >& oGameOptions = refStdConfig->getOptions(OwnerType::GAME);
	const NamedObjIndex< shared_ptr<Option> >& oTeamOptions = refStdConfig->getOptions(OwnerType::TEAM);
	const NamedObjIndex< shared_ptr<Option> >& oPlayerOptions = refStdConfig->getOptions(OwnerType::PLAYER);
	REQUIRE(oGameOptions.size() == 4); // AI + SoundEnabled + PerPlayerSound + Game Volume
	REQUIRE(oTeamOptions.size() == 1); // AI
	REQUIRE(oPlayerOptions.size() == 2); // AI + Player Volume
	REQUIRE(refStdConfig->getTotVisibleOptions() == 4);
	REQUIRE(refStdConfig->getTotVisibleOptions(OwnerType::GAME) == 2); // Volume + PerPlayerSound
	REQUIRE(refStdConfig->getTotVisibleOptions(OwnerType::TEAM) == 0);
	REQUIRE(refStdConfig->getTotVisibleOptions(OwnerType::PLAYER) == 2); // AI + Volume
	const int32_t nGameAIIdx = oGameOptions.getIndex(refStdConfig->getAIOptionName());
	REQUIRE(nGameAIIdx >= 0);
	const int32_t nTeamAIIdx = oTeamOptions.getIndex(refStdConfig->getAIOptionName());
	REQUIRE(nTeamAIIdx >= 0);
	const int32_t nPlayerAIIdx = oPlayerOptions.getIndex(refStdConfig->getAIOptionName());
	REQUIRE(nPlayerAIIdx >= 0);

	const bool bAllowAI = (oAppConstraints.m_nAIMatesPerTeamMax > 0);
	const auto& refGameAIOption = oGameOptions.getObj(nGameAIIdx);
	REQUIRE(dynamic_cast<BoolOption*>(refGameAIOption.get()) != nullptr);
	REQUIRE(refGameAIOption->isReadonly());
	REQUIRE_FALSE(refGameAIOption->isVisible());
	REQUIRE(refGameAIOption->getDefaultValue() == Variant{bAllowAI});

	const auto& refTeamAIOption = oTeamOptions.getObj(nTeamAIIdx);
	REQUIRE(dynamic_cast<BoolOption*>(refTeamAIOption.get()) != nullptr);
	REQUIRE(refTeamAIOption->isReadonly());
	REQUIRE(! refTeamAIOption->isVisible());
	REQUIRE(refTeamAIOption->getDesc() == "AI team");

	const auto& refPlayerAIOption = oPlayerOptions.getObj(nPlayerAIIdx);
	REQUIRE(dynamic_cast<BoolOption*>(refPlayerAIOption.get()) != nullptr);
	REQUIRE(refPlayerAIOption->isReadonly() == !bAllowAI);
	REQUIRE(refPlayerAIOption->isVisible() == bAllowAI);
	REQUIRE(refPlayerAIOption->getDefaultValue() == Variant{false});
	REQUIRE(refPlayerAIOption->getDesc() == "AI player");

	const int32_t nGamePerPlayerSoundIdx = oGameOptions.getIndex(refStdConfig->getPerPlayerSoundOptionName());
	REQUIRE(nGamePerPlayerSoundIdx >= 0);

	const auto& refGamePerPlayerSoundOption = oGameOptions.getObj(nGamePerPlayerSoundIdx);
	REQUIRE(dynamic_cast<BoolOption*>(refGamePerPlayerSoundOption.get()) != nullptr);
	REQUIRE(refGamePerPlayerSoundOption->isReadonly() != bPerPlayerSoundAllowed);
	REQUIRE(refGamePerPlayerSoundOption->isVisible() == bPerPlayerSoundAllowed);
	REQUIRE(refGamePerPlayerSoundOption->getDefaultValue() == Variant{false});

	const int32_t nGameSoundVolumeSoundIdx = oGameOptions.getIndex(refStdConfig->getSoundVolumeOptionName());
	REQUIRE(nGameSoundVolumeSoundIdx >= 0);

	const auto& refGameSoundVolumeOption = oGameOptions.getObj(nGameSoundVolumeSoundIdx);
	REQUIRE(dynamic_cast<IntOption*>(refGameSoundVolumeOption.get()) != nullptr);
	REQUIRE(refGameSoundVolumeOption->isReadonly() != bSoundEnabled);
	REQUIRE(refGameSoundVolumeOption->isVisible() == bSoundEnabled);
	REQUIRE(refGameSoundVolumeOption->getDefaultValue() == Variant{bSoundEnabled ? 100 : 0});

	const int32_t nPlayerSoundVolumeSoundIdx = oPlayerOptions.getIndex(refStdConfig->getSoundVolumeOptionName());
	REQUIRE(nPlayerSoundVolumeSoundIdx >= 0);

	const auto& refPlayerSoundVolumeOption = oGameOptions.getObj(nGameSoundVolumeSoundIdx);
	REQUIRE(dynamic_cast<IntOption*>(refPlayerSoundVolumeOption.get()) != nullptr);
	REQUIRE(refPlayerSoundVolumeOption->isReadonly() != bSoundEnabled);
	REQUIRE(refPlayerSoundVolumeOption->isVisible() == bSoundEnabled);
	REQUIRE(refPlayerSoundVolumeOption->getDefaultValue() == Variant{bSoundEnabled ? 100 : 0});
}

} // namespace testing

} // namespace stmg
