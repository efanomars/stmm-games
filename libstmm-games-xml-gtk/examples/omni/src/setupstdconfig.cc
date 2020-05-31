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
 * File:   setupstdconfig.cc
 */

#include "setupstdconfig.h"
#include "jsonstrings.h"
#include "inputstrings.h"

#include <stmm-games/stdconfig.h>
#include <stmm-games/appconstraints.h>
#include <stmm-games/options/booloption.h>
#include <stmm-games/options/intoption.h>
#include <stmm-games/options/enumoption.h>

#include <stmm-input-ev/stmm-input-ev.h>

#include <stmm-input/devicemanager.h>

//#include <iostream>
#include <cassert>
#include <string>
#include <memory>

namespace stmi { class DeviceManager; }


namespace stmg
{

using std::shared_ptr;

static const omni::InputStrings s_oInputStrings{};

std::string omniSetupStdConfig(shared_ptr<StdConfig>& refStdConfig, const json& oConf, const shared_ptr<stmi::DeviceManager>& refDeviceManager
								, const std::string& sOmni, const std::string& sAppVersion, bool bTestMode) noexcept
{
	assert(refStdConfig.get() == nullptr);

	StdConfig::Init oStdConfigInit;
	oStdConfigInit.m_sAppName = sOmni;
	oStdConfigInit.m_sAppVersion = sAppVersion;
	oStdConfigInit.m_bTestMode = bTestMode;

	oStdConfigInit.m_refDeviceManager = refDeviceManager;

	const json& oConstr = oConf[jas::s_sConfConstraints];
	AppConstraints& oAppConstraints = oStdConfigInit.m_oAppConstraints;
	oAppConstraints.m_nTeamsMin = oConstr[jas::s_sConfConstraintsTeamsMin].get<int32_t>();
	oAppConstraints.m_nTeamsMax = oConstr[jas::s_sConfConstraintsTeamsMax].get<int32_t>(); 
	oAppConstraints.m_nMatesPerTeamMax = oConstr[jas::s_sConfConstraintsMatesPerTeamMax].get<int32_t>(); 
	oAppConstraints.m_nPlayersMax = oConstr[jas::s_sConfConstraintsPlayersMax].get<int32_t>(); 
	oAppConstraints.m_nAIMatesPerTeamMax = oConstr[jas::s_sConfConstraintsAIMatesPerTeamMax].get<int32_t>(); 
	oAppConstraints.m_bAllowMixedAIHumanTeam = oConstr[jas::s_sConfConstraintsAllowMixedAIHumanTeam].get<bool>(); 

	const json& oCapaAss = oConf[jas::s_sConfAssign];
	StdConfig::CapabilityAssignment& oCapabilityAssignment = oStdConfigInit.m_oCapabilityAssignment;
	oCapabilityAssignment.m_bAllCapabilityClasses = oCapaAss[jas::s_sConfAssignAllCapabilityClasses].get<bool>();
	for (std::string sClassId : oCapaAss[jas::s_sConfAssignCapabilityClasses]) {
		stmi::Capability::Class oCapaClass = stmi::Capability::getCapabilityClassIdClass(sClassId);
		if (! oCapaClass) {
			const std::string sError = "Error: Capability class id not registered: " + sClassId;
			return sError; //-------------------------------------------------------
		}
		oCapabilityAssignment.m_aCapabilityClasses.push_back(oCapaClass);
	}
	oCapabilityAssignment.m_nMaxCapabilitiesExplicitlyAssignedToPlayer = oCapaAss[jas::s_sConfAssignMaxCapabilitiesExplicitlyAssignedToPlayer].get<int32_t>();
	oCapabilityAssignment.m_bMoreCapabilitiesPerClassAssignedToPlayer = oCapaAss[jas::s_sConfAssignMoreCapabilitiesPerClassAssignedToPlayer].get<bool>();
	oCapabilityAssignment.m_bCapabilitiesAutoAssignedToActivePlayer = oCapaAss[jas::s_sConfAssignCapabilitiesAutoAssignedToActivePlayer].get<bool>();

	const json& oKeyActs = oConf[jas::s_sConfKeyActions];
	for (const json& oJKeyAction : oKeyActs) {
		std::vector<std::string> aNames;
		for (std::string sKAName : oJKeyAction[jas::s_sConfKeyActionsKeyActionNames]) {
			aNames.push_back(sKAName);
		}
		std::string sKADesc = oJKeyAction[jas::s_sConfKeyActionsKeyActionDescription].get<std::string>();
		std::vector< std::pair< stmi::Capability::Class, std::vector< stmi::HARDWARE_KEY >>> aDefaultClassLeys;
		const json& oKADefaultClassKeys = oJKeyAction[jas::s_sConfKeyActionsKeyActionDefaultClassKeys];
		for (const json& oKADefaultClassKey : oKADefaultClassKeys) {
			std::string sClassId = oKADefaultClassKey[jas::s_sConfKeyActionsCapabilityClassId];
			stmi::Capability::Class oCapaClass = stmi::Capability::getCapabilityClassIdClass(sClassId);
			if (! oCapaClass) {
				const std::string sError = "Error: Capability class id not registered: " + sClassId;
				return sError; //---------------------------------------------------
			}
			std::vector<stmi::HARDWARE_KEY> aHardwareKeys;
			for (std::string sKey : oKADefaultClassKey[jas::s_sConfKeyActionsDefaultKeys]) {
				std::pair<bool, stmi::HARDWARE_KEY> oPair = s_oInputStrings.getStringKey(sKey);
				if (! oPair.first) {
					const std::string sError = "Error: hardware key not valid: " + sKey;
					return sError; //-----------------------------------------------
				}
				aHardwareKeys.push_back(oPair.second);
			}
			aDefaultClassLeys.push_back(std::make_pair(std::move(oCapaClass), std::move(aHardwareKeys)));
		}
		oStdConfigInit.m_aKeyActions.emplace_back(aNames, sKADesc, aDefaultClassLeys);
	}

	auto oConvertToVariant = [&](bool bIsBool, const json& oJValue)
	{
		if (bIsBool) {
			const bool bValue = oJValue.get<bool>();
			return Variant{bValue};
		} else {
			assert(oJValue.is_number());
			const int32_t nValue = oJValue.get<int32_t>();
			return Variant{nValue};
		}
	};

	auto& aOptions = oStdConfigInit.m_aOptions;
	const json& oJOptions = oConf[jas::s_sConfOptions];
	for (const json& oJOption : oJOptions) {
		const std::string sOptionType = oJOption[jas::s_sConfOptionsOptionType];
		const bool bOptionIsBoolean = (sOptionType == jas::s_sConfOptionsOptionTypeBool);
		
		const std::string sOwnerType = oJOption[jas::s_sConfOptionsOwnerType];
		OwnerType eOwnerType = OwnerType::GAME;
		if (sOwnerType == jas::s_sConfOptionsOwnerTypeGame) {
		} else if (sOwnerType == jas::s_sConfOptionsOwnerTypeTeam) {
			eOwnerType = OwnerType::TEAM;
		} else if (sOwnerType == jas::s_sConfOptionsOwnerTypePlayer) {
			eOwnerType = OwnerType::PLAYER;
		} else {
			const std::string sError = "Error: wrong owner type: " + sOwnerType;
			return sError; //-------------------------------------------------------
		}
		const std::string sName = oJOption[jas::s_sConfOptionsOptionName];
		if (sName.empty()) {
			const std::string sError = "Error: option name cannot be empty";
			return sError; //-------------------------------------------------------
		}
		const json& oJDefaultValue = oJOption[jas::s_sConfOptionsDefaultValue];
		const Variant oDefaultValue = oConvertToVariant(bOptionIsBoolean, oJDefaultValue);

		const std::string sDescription = oJOption[jas::s_sConfOptionsDescription];
		const bool bIsReadOnly = oJOption[jas::s_sConfOptionsReadOnly].get<bool>();
		const bool bIsVisible = oJOption[jas::s_sConfOptionsVisible].get<bool>();

		std::vector< std::pair< shared_ptr< Option >, std::vector< Variant >>> aMastersValues;
		for (const json& oJMasterValues : oJOption[jas::s_sConfOptionsMasters]) {
			const std::string sMasterName = oJMasterValues[jas::s_sConfOptionsMastersMasterName];
			const auto itFind = std::find_if(aOptions.begin(), aOptions.end(), [&](const shared_ptr<Option>& refOption)
			{
				return (refOption->getName() == sMasterName);
			});
			if (itFind == aOptions.end()) {
				const std::string sError = "Error: master option name not found " + sMasterName;
				return sError; //---------------------------------------------------
			}
			auto refMaster = *itFind;
			std::vector<Variant> aMasterValues;
			const bool bMasterIsBoolean = (refMaster->getDefaultValue().getType() == Variant::TYPE_BOOL);
			for (const json& oJValue : oJMasterValues[jas::s_sConfOptionsMastersMasterValues]) {
				const Variant oMasterValue = oConvertToVariant(bMasterIsBoolean, oJValue);
				aMasterValues.push_back(oMasterValue);
			}
			aMastersValues.push_back(std::make_pair(refMaster, aMasterValues));
		}

		shared_ptr<Option> refNewOption;
		if (bOptionIsBoolean) {
			refNewOption = std::make_shared<BoolOption>(eOwnerType, sName, oDefaultValue.getBool(), sDescription, bIsReadOnly, bIsVisible, aMastersValues);
		} else if (sOptionType == jas::s_sConfOptionsOptionTypeInt) {
			const int32_t nMin = oJOption[jas::s_sConfOptionsIntOptionMin].get<int32_t>();
			const int32_t nMax = oJOption[jas::s_sConfOptionsIntOptionMax].get<int32_t>();
			refNewOption = std::make_shared<IntOption>(eOwnerType, sName, oDefaultValue.getInt(), sDescription, bIsReadOnly, bIsVisible, aMastersValues, nMin, nMax);
		} else {
			std::vector<std::tuple<int32_t, std::string, std::string>> aEnums;
			for (const json& oJEnum : oJOption[jas::s_sConfOptionsEnumOptionEnums]) {
				const int32_t nEnumValue = oJEnum[jas::s_sConfOptionsEnumOptionEnumValue].get<int32_t>();
				const std::string sEnumName = oJEnum[jas::s_sConfOptionsEnumOptionEnumName].get<std::string>();
				const std::string sEnumDesc = oJEnum[jas::s_sConfOptionsEnumOptionEnumDesc].get<std::string>();
				aEnums.push_back(std::make_tuple(nEnumValue, sEnumName, sEnumDesc));
			}
			refNewOption = std::make_shared<EnumOption>(eOwnerType, sName, oDefaultValue.getInt(), sDescription, bIsReadOnly, bIsVisible, aMastersValues, aEnums);
		}
		aOptions.push_back(refNewOption);
	}

	oStdConfigInit.m_nMaxGamePlayedHistory = oConf[jas::s_sConfMaxGamePlayedHistory].get<int32_t>();
	oStdConfigInit.m_bSoundEnabled = oConf[jas::s_sConfSoundEnabled].get<bool>();
	oStdConfigInit.m_bSoundPerPlayerAllowed = oConf[jas::s_sConfSoundPerPlayerAllowed].get<bool>();

	//
	refStdConfig = std::make_shared<StdConfig>(std::move(oStdConfigInit));

	return "";
}

} //namespace stmg

