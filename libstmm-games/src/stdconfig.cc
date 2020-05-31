/*
 * File:   stdconfig.cc
 *
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

#include "stdconfig.h"

#include "option.h"
#include "options/booloption.h"
#include "options/intoption.h"
#include "util/variant.h"

#include <stmm-input-au/playbackcapability.h>
#include <stmm-input-au/sndmgmtcapability.h>

#include <stmm-input/capability.h>
#include <stmm-input/devicemanager.h>
#include <stmm-input/device.h>
#include <stmm-input/hardwarekey.h>

#include <cassert>
#include <algorithm>
#include <cctype>
#include <iterator>
#include <string>


namespace stmg
{

static const std::string s_sAIOptionName = "AI";
static const std::string s_sSoundEnabledOptionName = "SoundEnabled";
static const std::string s_sPerPlayerSoundOptionName = "PerPlayerSound";
static const std::string s_sSoundVolumeOptionName = "SoundVolume";

////////////////////////////////////////////////////////////////////////////////
StdConfig::KeyAction::KeyAction(const std::vector<std::string>& aKeyActionName, const std::string& sDescription
								, const std::vector< std::pair<stmi::Capability::Class
								, std::vector<stmi::HARDWARE_KEY> > >& aDefaultClassKeys) noexcept
: m_aKeyActionName(aKeyActionName)
, m_sDescription(sDescription)
{
	#ifndef NDEBUG
	// Check key action names are added only once and not empty
	const int32_t nTotKeyActionNames = static_cast<int32_t>(aKeyActionName.size());
	for (int32_t nIdx = 0; nIdx < nTotKeyActionNames; ++nIdx) {
		const auto& sName = aKeyActionName[nIdx];
		assert(!sName.empty());
		assert(std::isalnum(sName[0]));
		for (int32_t nIdx2 = nIdx + 1; nIdx2 < nTotKeyActionNames; ++nIdx2) {
			const auto& sName2 = aKeyActionName[nIdx2];
			assert(sName != sName2);
		}
	}
	// Check defaults don't repeat capability class
	// and default key is valid and is not HK_NULL
	const int32_t nTotDefaultClasses = static_cast<int32_t>(aDefaultClassKeys.size());
	for (int32_t nIdx = 0; nIdx < nTotDefaultClasses; ++nIdx) {
		const stmi::Capability::Class& oClass = aDefaultClassKeys[nIdx].first;
		for (int32_t nIdx2 = nIdx + 1; nIdx2 < nTotDefaultClasses; ++nIdx2) {
			const stmi::Capability::Class& oClass2 = aDefaultClassKeys[nIdx2].first;
			assert(! (oClass == oClass2)); // Can't define defaults for a class more than once
		}
		const std::vector<stmi::HARDWARE_KEY>& aDefaultKeys = aDefaultClassKeys[nIdx].second;
		const int32_t nTotDefaultKeys = static_cast<int32_t>(aDefaultKeys.size());
		for (int32_t nDefIdx = 0; nDefIdx < nTotDefaultKeys; ++nDefIdx) {
			const stmi::HARDWARE_KEY& eKey = aDefaultKeys[nDefIdx];
			if (eKey == stmi::HK_NULL) {
				assert(false); // key cannot be null
			} else {
				const auto& oValidKeys = stmi::HardwareKeys::get();
				assert(std::find(oValidKeys.begin(), oValidKeys.end(), eKey) != oValidKeys.end()); // key not valid
			}
		}
	}
	#endif //NDEBUG
	for (auto& oPair : aDefaultClassKeys) {
		m_aCapaClasses.push_back(oPair.first);
		m_aClassDefaultKeys.push_back(oPair.second);
	}
}
StdConfig::KeyAction::KeyAction(const std::string& sKeyActionName, const std::string& sDescription
					, const stmi::Capability::Class& oCapaClass, const std::vector<stmi::HARDWARE_KEY>& aDefaultClassKeys) noexcept
: KeyAction({sKeyActionName}, sDescription, {{oCapaClass, aDefaultClassKeys}})
{
}
const std::vector<stmi::HARDWARE_KEY>& StdConfig::KeyAction::getClassDefaultKeys(const stmi::Capability::Class& oClass) const noexcept
{
	static const std::vector<stmi::HARDWARE_KEY> s_aEmpty{};
	auto itFind = std::find(m_aCapaClasses.begin(), m_aCapaClasses.end(), oClass);
	if (itFind == m_aCapaClasses.end()) {
		return s_aEmpty;
	}
	auto nClassIdx = std::distance(m_aCapaClasses.begin(), itFind);
	return m_aClassDefaultKeys[nClassIdx];
}

////////////////////////////////////////////////////////////////////////////////
StdConfig::StdConfig(StdConfig::Init&& oInit) noexcept
: m_sAppName(std::move(oInit.m_sAppName))
, m_sAppVersion(std::move(oInit.m_sAppVersion))
, m_refDeviceManager(std::move(oInit.m_refDeviceManager))
, m_oAppConstraints(std::move(oInit.m_oAppConstraints))
, m_oCapabilityAssignment(std::move(oInit.m_oCapabilityAssignment))
, m_aKeyActions(std::move(oInit.m_aKeyActions))
, m_nMaxGamePlayedHistory(oInit.m_nMaxGamePlayedHistory)
, m_bSoundEnabled(oInit.m_bSoundEnabled)
, m_bSoundPerPlayerAllowed(oInit.m_bSoundPerPlayerAllowed)
, m_bTestMode(oInit.m_bTestMode)
{
	assert(! m_sAppName.empty());
	#ifndef NDEBUG
	for (const auto& c : m_sAppName) {
		assert(std::isalnum(c) || (c == '-') || (c == '_'));
	}
	#endif //NDEBUG
	assert(m_sAppName.find_first_of(".|") == std::string::npos);
	assert(! m_sAppVersion.empty());
	assert(m_refDeviceManager);
	assert(m_oAppConstraints.isValid());
	assert(m_oCapabilityAssignment.isValid());
	assert(m_nMaxGamePlayedHistory >= 0);
	//
	if (m_bSoundEnabled) {
		m_refDeviceManager->getCapability(m_refSndMgmtCapability);
		if (m_refSndMgmtCapability) {
			const int32_t nMaxPlaybacks = m_refSndMgmtCapability->getMaxPlaybackDevices();
			if (nMaxPlaybacks <= 0) {
				m_bSoundEnabled = false;
			} else if (nMaxPlaybacks == 1) {
				m_bSoundPerPlayerAllowed = false;
			}
		}
	}
	if (! m_bSoundEnabled) {
		m_bSoundPerPlayerAllowed = false;
	}

	const bool bAllowAI = m_oAppConstraints.allowsAI();
	bool bAIGameDefined = false;
	bool bAITeamDefined = false;
	shared_ptr<BoolOption> refAIOptionPlayer;
	checkAIOptions(oInit.m_aOptions, bAllowAI, bAIGameDefined, bAITeamDefined, refAIOptionPlayer);
	// Add builtin AI options if not defined
	if (! bAIGameDefined) {
		shared_ptr<Option> refAIOptionGame = createAIOption(OwnerType::GAME, "AI allowed", bAllowAI);
		addOption(refAIOptionGame);
	}
	if (! bAITeamDefined) {
		shared_ptr<Option> refAIOptionTeam = createAIOption(OwnerType::TEAM, "AI team", bAllowAI);
		addOption(refAIOptionTeam);
	}
	if (! refAIOptionPlayer) {
		refAIOptionPlayer = createAIOption(OwnerType::PLAYER, "AI player", bAllowAI);
		addOption(refAIOptionPlayer);
	}
	//
	shared_ptr<BoolOption> refPerPlayerSound;
	bool bSoundEnabledDefined = false;
	bool bGameVolumeDefined = false;
	bool bPlayerVolumeDefined = false;
	checkSoundOptions(oInit.m_aOptions, bSoundEnabledDefined, refPerPlayerSound, bGameVolumeDefined, bPlayerVolumeDefined);

	// This is here to ensure that by default
	// - AI options appear before all other options
	// - sound options appear after all other options
	// This can be used in a preferences dialog (for  ex.) to append a list widget that
	// allows to assing the playback device to a certain player just after the
	// volume widget associated with refPlayerVolume (see below)
	addOptions(oInit.m_aOptions);

	// Add the missing built-in options if necessary
	if (! bSoundEnabledDefined){
		auto refSoundEnabledOption = createSoundEnabledOption(m_bSoundEnabled);
		addOption(refSoundEnabledOption);
	}
	if (! refPerPlayerSound) {
		refPerPlayerSound = createPerPlayerSoundOption("Per player sound", m_bSoundPerPlayerAllowed);
		addOption(refPerPlayerSound);
	}
	if (! bGameVolumeDefined) {
		auto refGameVolume = createSoundVolumeOption(OwnerType::GAME, "Volume (0-100)"
													, (m_bSoundPerPlayerAllowed ? shared_ptr<BoolOption>{refPerPlayerSound} : shared_ptr<BoolOption>{})
													, shared_ptr<BoolOption>{}
													, m_bSoundEnabled);
		addOption(refGameVolume);
	}
	if (! bPlayerVolumeDefined) {
		auto refPlayerVolume = createSoundVolumeOption(OwnerType::PLAYER, "Volume (0-100)"
													, (m_bSoundPerPlayerAllowed ? shared_ptr<BoolOption>{refPerPlayerSound} : shared_ptr<BoolOption>{})
													, refAIOptionPlayer
													, m_bSoundEnabled);
		addOption(refPlayerVolume);
	}

	//
	checkOptions(OwnerType::GAME);
	checkOptions(OwnerType::TEAM);
	checkOptions(OwnerType::PLAYER);

	#ifndef NDEBUG
	checkKeyActions();
	#endif //NDEBUG
}

NamedObjIndex< shared_ptr<Option> >& StdConfig::getOptionsInternal(OwnerType eOwnerType) noexcept
{
	switch (eOwnerType) {
		case OwnerType::GAME: return m_oGameOptions;
		case OwnerType::TEAM: return m_oTeamOptions;
		case OwnerType::PLAYER: return m_oPlayerOptions;
		default: assert(false); return m_oGameOptions;
	}
}
void StdConfig::addOptions(const std::vector< shared_ptr<Option> >& aOptions) noexcept
{
	for (auto& refOption : aOptions) {
		addOption(refOption);
	}
}
void StdConfig::addOption(const shared_ptr<Option>& refOption) noexcept
{
	assert(refOption);
	NamedObjIndex< shared_ptr<Option> >& oOptions = getOptionsInternal(refOption->getOwnerType());
	const std::string& sName = refOption->getName();
	assert(oOptions.getIndex(sName) < 0);
	oOptions.addNamedObj(sName, refOption);
}
void StdConfig::checkAIOptions(const std::vector< shared_ptr<Option> >& aOptions, bool
								#ifndef NDEBUG
								bAllowAI
								#endif //NDEBUG
								, bool& bAIGameDefined, bool& bAITeamDefined, shared_ptr<BoolOption>& refAIOptionPlayer) noexcept
{
	for (auto& refOption : aOptions) {
		if (refOption->getName() == StdConfig::getAIOptionName()) {
			assert(bAllowAI); // AI options only allowed if AI players can exist
			assert(! refOption->isSlave()); // cannot have masters
			BoolOption* p0BoolOption = dynamic_cast<BoolOption*>(refOption.get());
			assert( p0BoolOption != nullptr); // must be a BoolOption
			const auto eOwnerType = p0BoolOption->getOwnerType();
			if (eOwnerType == OwnerType::GAME) {
				assert(p0BoolOption->isReadonly()); // The value of this option just tells whether AI players are allowed
				assert(! p0BoolOption->isVisible());
				assert(! bAIGameDefined);
				bAIGameDefined = true;
			} else if (eOwnerType == OwnerType::TEAM) {
				assert(p0BoolOption->isReadonly()); // The value of this option just tells whether a team has AI mates.
				assert(p0BoolOption->isVisible() == bAllowAI); // The option is shown iff AI players are allowed.
				assert(! bAITeamDefined);
				bAITeamDefined = true;
			} else if (eOwnerType == OwnerType::PLAYER) {
				assert(p0BoolOption->isReadonly() == !bAllowAI); // The value of this option can't be readonly if AI players are allowed.
				assert(p0BoolOption->isVisible() == bAllowAI); // The option is shown iff AI players are allowed.
				assert(! refAIOptionPlayer);
				refAIOptionPlayer = std::static_pointer_cast<BoolOption>(refOption);
			} else {
				assert(false);
			}
		}
	}
}
void StdConfig::checkSoundOptions(const std::vector< shared_ptr<Option> >& aOptions, bool& bSoundEnabledDefined
								, shared_ptr<BoolOption>& refPerPlayerSound, bool& bGameVolumeDefined, bool& bPlayerVolumeDefined) noexcept
{
	for (auto& refOption : aOptions) {
		if (refOption->getName() == StdConfig::getSoundEnabledOptionName()) {
			assert(! bSoundEnabledDefined); // There can be only one
			assert(! refOption->isSlave()); // cannot have masters
			assert(refOption->isReadonly());
			assert(refOption->getDefaultValue() == Variant{m_bSoundEnabled});
			#ifndef NDEBUG
			BoolOption* p0BoolOption = dynamic_cast<BoolOption*>(refOption.get());
			#endif //NDEBUG
			assert( p0BoolOption != nullptr); // must be a BoolOption
			assert( OwnerType::GAME == p0BoolOption->getOwnerType());
			bSoundEnabledDefined = true;
		}
	}
	for (auto& refOption : aOptions) {
		if (refOption->getName() == s_sPerPlayerSoundOptionName) {
			assert(m_bSoundEnabled);
			assert(m_bSoundPerPlayerAllowed);
			assert(! refPerPlayerSound); // There can be only one
			assert(! refOption->isSlave()); // cannot have masters
			#ifndef NDEBUG
			BoolOption* p0BoolOption = dynamic_cast<BoolOption*>(refOption.get());
			#endif //NDEBUG
			assert( p0BoolOption != nullptr); // must be a BoolOption
			assert( OwnerType::GAME == p0BoolOption->getOwnerType());
			refPerPlayerSound = std::static_pointer_cast<BoolOption>(refOption);
		}
	}
	for (auto& refOption : aOptions) {
		if (refOption->getName() == StdConfig::getSoundVolumeOptionName()) {
			assert(m_bSoundEnabled);
			IntOption* p0IntOption = dynamic_cast<IntOption*>(refOption.get());
			assert( p0IntOption != nullptr); // must be a IntOption
			if (OwnerType::GAME == p0IntOption->getOwnerType()) {
				assert(! bGameVolumeDefined); // there should be only one game volume option
				bGameVolumeDefined = true;
			} else if (OwnerType::PLAYER == p0IntOption->getOwnerType()) {
				assert(! bPlayerVolumeDefined); // there should be only one player volume option
				assert(m_bSoundPerPlayerAllowed);
				assert(refPerPlayerSound);
				#ifndef NDEBUG
				const auto& aMastersValues = p0IntOption->getMastersValues();
				auto itFind = std::find_if(aMastersValues.begin(), aMastersValues.end(), [&](const auto& oPair)
				{
					return (refPerPlayerSound == oPair.first);
				});
				assert(itFind != aMastersValues.end()); // The PerPlayerSound option must be master
				const std::vector<Variant>& aValues = itFind->second;
				#endif //NDEBUG
				// The PerPlayerSound option must enable the player volume when it is true
				assert(aValues.size() == 1);
				assert(aValues[0] == Variant{true});
				bPlayerVolumeDefined = true;
			} else {
				assert(false); // there can't be a team volume
			}
		}
	}
}
void StdConfig::checkOptions(OwnerType
								#ifndef NDEBUG
								eOwnerType
								#endif //NDEBUG
							) noexcept
{
	#ifndef NDEBUG
	const auto& oOptions = getOptionsInternal(eOwnerType);
	const int32_t nTotOptions = oOptions.size();
	for (int32_t nIdx = 0; nIdx < nTotOptions; ++nIdx) {
		const auto& refOption = oOptions.getObj(nIdx);
		const std::string& sName = refOption->getName();
		assert(sName[0] != '_');
		const auto& aMastersValues = refOption->getMastersValues();
		for (const auto& oPair : aMastersValues) {
			const shared_ptr<Option>& refMaster = oPair.first;
			assert(refMaster);
			// It's a slave
			checkMasterOptionExists(refMaster);
			// check master option is of an same or "superior" type
			if (eOwnerType == OwnerType::GAME) {
				assert(refMaster->getOwnerType() == OwnerType::GAME);
			} else if (eOwnerType == OwnerType::TEAM) {
				assert((refMaster->getOwnerType() == OwnerType::GAME) || (refMaster->getOwnerType() == OwnerType::TEAM));
			}
		}
	}
	#endif //NDEBUG
}
#ifndef NDEBUG
void StdConfig::checkMasterOptionExists(const shared_ptr<Option>& refMaster) const noexcept
{
	// Master must exist
	OwnerType eOwnerType = refMaster->getOwnerType();
	const NamedObjIndex< shared_ptr<Option> >& oOptions = getOptions(eOwnerType);
	// make sure the master was added to StdConfig
	assert(oOptions.getIndex(refMaster->getName()) >= 0);
}
void StdConfig::checkKeyActions() const noexcept
{
	// Check key action names unique
	const int32_t nTotKeyActions = static_cast<int32_t>(m_aKeyActions.size());
	for (int32_t nIdx = 0; nIdx < nTotKeyActions; ++nIdx) {
		const auto& aKeyActionNames = m_aKeyActions[nIdx].m_aKeyActionName;
		// within key action itself
		for (const auto& sCheckName : aKeyActionNames) {
			auto nCount = std::count_if(aKeyActionNames.begin(), aKeyActionNames.end(), [&sCheckName](const std::string& sName)
			{
				return (sName == sCheckName);
			});
			assert(nCount == 1);
		}
		// among other key actions
		for (int32_t nIdx2 = nIdx + 1; nIdx2 < nTotKeyActions; ++nIdx2) {
			auto& aKeyActionNames2 = m_aKeyActions[nIdx2].m_aKeyActionName;
			auto itFind2 = std::find_if(aKeyActionNames.begin(), aKeyActionNames.end(), [&aKeyActionNames2](const std::string& sName)
			{
				auto itFind2 = std::find(aKeyActionNames2.begin(), aKeyActionNames2.end(), sName);
				return (itFind2 != aKeyActionNames2.end());
			});
			assert(itFind2 == aKeyActionNames.end());
		}
	}
}
#endif //NDEBUG

int32_t StdConfig::getTotKeyActions() const noexcept
{
	const int32_t nTotKeyActions = static_cast<int32_t>(m_aKeyActions.size());
	return nTotKeyActions;
}
int32_t StdConfig::getKeyActionId(const std::string& sKeyActionName) const noexcept
{
	assert(!sKeyActionName.empty());
	const int32_t nTotKeyActions = static_cast<int32_t>(m_aKeyActions.size());
	for (int32_t nKeyAction = 0; nKeyAction < nTotKeyActions; ++nKeyAction) {
		const int32_t nTotNames = static_cast<int32_t>(m_aKeyActions[nKeyAction].m_aKeyActionName.size());
		for (int nIdx = 0; nIdx < nTotNames; ++nIdx) {
			if (m_aKeyActions[nKeyAction].m_aKeyActionName[nIdx].compare(sKeyActionName) == 0) {
				return nKeyAction;
			}
		}
	}
//std::cout << "StdAppConfig::getKeyActionId  sKeyActionName=" << sKeyActionName << "  not found!!" << '\n';
	return -1;
}
const StdConfig::KeyAction& StdConfig::getKeyAction(int32_t nKeyActionId) const  noexcept
{
	assert((nKeyActionId >= 0) && (nKeyActionId < static_cast<int32_t>(m_aKeyActions.size())));
	return m_aKeyActions[nKeyActionId];
}

shared_ptr<BoolOption> StdConfig::createAIOption(OwnerType eOwnerType, const std::string& sAIOptionDesc) noexcept
{
	return createAIOption(eOwnerType, sAIOptionDesc, true);
}
shared_ptr<BoolOption> StdConfig::createAIOption(OwnerType eOwnerType, const std::string& sAIOptionDesc, bool bAllowAI) noexcept
{
	if (eOwnerType == OwnerType::GAME) {
		return std::make_shared<BoolOption>(OwnerType::GAME, getAIOptionName(), bAllowAI, sAIOptionDesc
											, /*bReadOnly=*/true, /*bVisible=*/false
											, shared_ptr<Option>{}, std::vector<Variant>{});
	} else if (eOwnerType == OwnerType::TEAM) {
		return std::make_shared<BoolOption>(OwnerType::TEAM, getAIOptionName(), false, sAIOptionDesc
											, /*bReadOnly=*/true, /*bVisible=*/false
											, shared_ptr<Option>{}, std::vector<Variant>{});
	} else if (eOwnerType == OwnerType::PLAYER) {
		return std::make_shared<BoolOption>(OwnerType::PLAYER, getAIOptionName(), false, sAIOptionDesc
											, /*bReadOnly=*/!bAllowAI, /*bVisible=*/bAllowAI
											, shared_ptr<Option>{}, std::vector<Variant>{});
	}
	return shared_ptr<BoolOption>{};
}
const std::string& StdConfig::getAIOptionName() noexcept
{
	return s_sAIOptionName;
}
shared_ptr<BoolOption> StdConfig::createSoundEnabledOption(bool bSoundEnabled) noexcept
{
	return std::make_shared<BoolOption>(OwnerType::GAME, s_sSoundEnabledOptionName, bSoundEnabled, ""
										, /*bReadOnly=*/true, /*bVisible=*/false
										, shared_ptr<Option>{}, std::vector<Variant>{});
}
const std::string& StdConfig::getSoundEnabledOptionName() noexcept
{
	return s_sSoundEnabledOptionName;
}
shared_ptr<BoolOption> StdConfig::createPerPlayerSoundOption(const std::string& sOptionDesc) noexcept
{
	return createPerPlayerSoundOption(sOptionDesc, true);
}
shared_ptr<BoolOption> StdConfig::createPerPlayerSoundOption(const std::string& sOptionDesc, bool bAllowPerPlayerSound) noexcept
{
	return std::make_shared<BoolOption>(OwnerType::GAME, s_sPerPlayerSoundOptionName, false, sOptionDesc
										, /*bReadOnly=*/ ! bAllowPerPlayerSound, /*bVisible=*/bAllowPerPlayerSound
										, shared_ptr<Option>{}, std::vector<Variant>{});
}
const std::string& StdConfig::getPerPlayerSoundOptionName() noexcept
{
	return s_sPerPlayerSoundOptionName;
}
shared_ptr<IntOption> StdConfig::createSoundVolumeOption(OwnerType eOwnerType, const std::string& sOptionDesc
														, const shared_ptr<BoolOption>& refPerPlayerSound, const shared_ptr<BoolOption>& refAIPlayer) noexcept
{
	return createSoundVolumeOption(eOwnerType, sOptionDesc, refPerPlayerSound, refAIPlayer, true);
}
shared_ptr<IntOption> StdConfig::createSoundVolumeOption(OwnerType eOwnerType, const std::string& sOptionDesc
														, const shared_ptr<BoolOption>& refPerPlayerSound, const shared_ptr<BoolOption>& refAIPlayer
														, bool bAllowSound) noexcept
{
	const int32_t nMinValue = 0;
	const int32_t nMaxValue = 100;
	const int32_t nDefaultValue = nMaxValue;
	bool bVisible = bAllowSound;
	assert(! (refPerPlayerSound && ! bAllowSound));
	std::vector<std::pair<shared_ptr<Option>, std::vector<Variant>>> aMastersValues;
	if (eOwnerType == OwnerType::GAME) {
		assert(! refAIPlayer); // The master volume doesn't depend on AI
		if (refPerPlayerSound) {
			assert(bAllowSound);
			assert(refPerPlayerSound->getName() == s_sPerPlayerSoundOptionName);
			assert(! refPerPlayerSound->isSlave());
			// depends on per player being false
			aMastersValues.push_back(std::make_pair(shared_ptr<Option>{refPerPlayerSound}, std::vector<Variant>{Variant{false}}));
		} else {
			// It's not a slave
			if (! bAllowSound) {
				bVisible = false;
			}
		}
	} else if (eOwnerType == OwnerType::PLAYER) {
		if (bAllowSound) {
			// AI option for player must be defined because AI player can't have device with volume
			// So volume option must hame master AI and only be enabled when AI is false
			assert(refAIPlayer);
			assert(refAIPlayer->getName() == StdConfig::getAIOptionName());
			assert(! refAIPlayer->isSlave());
			aMastersValues.emplace_back(refAIPlayer, std::vector<Variant>{Variant{false}});

			if (refPerPlayerSound) {
				assert(refPerPlayerSound->getName() == s_sPerPlayerSoundOptionName);
				assert(! refPerPlayerSound->isSlave());
				aMastersValues.push_back(std::make_pair(shared_ptr<Option>{refPerPlayerSound}, std::vector<Variant>{Variant{true}}));
			} else {
				bVisible = false;
			}
		} else {
			bVisible = false;
		}
	} else {
		assert(false); // There is no volume for teams
		return shared_ptr<IntOption>{};
	}
	return std::make_shared<IntOption>(eOwnerType, getSoundVolumeOptionName(), nDefaultValue, sOptionDesc
										, /*bReadOnly=*/! bAllowSound, bVisible
										, aMastersValues, nMinValue, nMaxValue);
}
const std::string& StdConfig::getSoundVolumeOptionName() noexcept
{
	return s_sSoundVolumeOptionName;
}
int32_t StdConfig::getTotVisibleOptions() const noexcept
{
	return getTotVisibleOptions(OwnerType::GAME) + getTotVisibleOptions(OwnerType::TEAM)
			+ getTotVisibleOptions(OwnerType::PLAYER);
}
int32_t StdConfig::getTotVisibleOptions(OwnerType eOwner) const noexcept
{
	int32_t nTotVisible = 0;
	const NamedObjIndex< shared_ptr<Option> >& oOptions = getOptions(eOwner);
	for (int32_t nIdx = 0; nIdx < oOptions.size(); ++ nIdx) {
		const auto& refOption = oOptions.getObj(nIdx);
		nTotVisible += (refOption->isVisible() ? 1 : 0);
	}
	return nTotVisible;
}
const NamedObjIndex< shared_ptr<Option> >& StdConfig::getOptions(OwnerType eOwner) const noexcept
{
	assert((eOwner >= OwnerType::GAME) && (eOwner <= OwnerType::PLAYER));
	switch (eOwner) {
		case OwnerType::GAME: return m_oGameOptions; //-------------------------
		case OwnerType::TEAM: return m_oTeamOptions; //-------------------------
		case OwnerType::PLAYER: return m_oPlayerOptions; //---------------------
	}
	assert(false);
	return m_oGameOptions;
}
const shared_ptr<Option>& StdConfig::getOption(OwnerType eOwner, const std::string& sName) const noexcept
{
	static shared_ptr<Option> s_refEmptyOption{};
	switch (eOwner) {
	case OwnerType::GAME:
	{
		const int32_t nIdx = m_oGameOptions.getIndex(sName);
		if (nIdx < 0) {
			return s_refEmptyOption; //-----------------------------------------
		}
		return m_oGameOptions.getObj(nIdx);
	}
	break;
	case OwnerType::TEAM:
	{
		const int32_t nIdx = m_oTeamOptions.getIndex(sName);
		if (nIdx < 0) {
			return s_refEmptyOption; //-----------------------------------------
		}
		return m_oTeamOptions.getObj(nIdx);
	}
	break;
	case OwnerType::PLAYER:
	{
		const int32_t nIdx = m_oPlayerOptions.getIndex(sName);
		if (nIdx < 0) {
			return s_refEmptyOption; //-----------------------------------------
		}
		return m_oPlayerOptions.getObj(nIdx);
	}
	break;
	default:
	{
		assert(false);
	}
	}
	return s_refEmptyOption;
}
shared_ptr<stmi::PlaybackCapability> StdConfig::getDefaultPlayback() const noexcept
{
	if (m_refSndMgmtCapability != nullptr) {
		return m_refSndMgmtCapability->getDefaultPlayback();
	}
	shared_ptr<stmi::PlaybackCapability> refFoundCapa;
	const auto aPlaybackDeviceIds = m_refDeviceManager->getDevicesWithCapabilityClass(stmi::PlaybackCapability::getClass());
	for (int32_t nDeviceId : aPlaybackDeviceIds) {
		const auto refDevice = m_refDeviceManager->getDevice(nDeviceId);
		shared_ptr<stmi::PlaybackCapability> refCapa;
		refDevice->getCapability(refCapa);
		assert(refCapa);
		if (! refFoundCapa) {
			refFoundCapa = refCapa;
		}
		if (refCapa->isDefaultDevice()) {
			refFoundCapa = refCapa;
			break;
		}
	}
	return refFoundCapa;
}

} // namespace stmg
