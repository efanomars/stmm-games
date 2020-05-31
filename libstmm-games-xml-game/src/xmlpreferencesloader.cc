/*
 * File:   xmlpreferencesloader.cc
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

#include "xmlpreferencesloader.h"

#include "xmlgamefiles.h"

#include "xmlutilfile.h"

#include <stmm-games-file/allpreferences.h>
#include <stmm-games-file/file.h>

#include <stmm-games/appconfig.h>
#include <stmm-games/option.h>
#include <stmm-games/stdconfig.h>
#include <stmm-games/stdpreferences.h>
#include <stmm-games/ownertype.h>
#include <stmm-games/util/util.h>
#include <stmm-games/util/namedobjindex.h>
#include <stmm-games/util/variant.h>

#include <stmm-input/capability.h>
#include <stmm-input/hardwarekey.h>

#include <glibmm/ustring.h>

#include <libxml++/libxml++.h>

#include <vector>
#include <cassert>
#include <iostream>
#include <exception>
#include <cctype>
#include <algorithm>
#include <cstdint>
#include <list>
#include <stdexcept>
#include <utility>

namespace stmg
{

// <GameAllPreferences appName="jointris" currentGame="jointris" currentTheme="Default">
//   <Option name="Difficulty" value="Easy">
//   <Team name="Team1">
//     <Option name="AllowMateSwap" value="f">
//     <Mate name="Johnny">
//       <Option name="CWRoration" value="t">
//       <KeyAction capabilityClass="Keyboard" capabilityDeviceIdx="0" hardwareKey="127">
//         <KeyActionName name="BlockEvent::Rotate"/>
//         <KeyActionName name="SwapperEvent::MoveUp"/>
//       </KeyAction>
//       <KeyAction capabilityClass="Pointer" capabilityDeviceIdx="0" hardwareKey="543">
//         <KeyActionName name="BlockEvent::Next"/>
//       </KeyAction>
//       <Capability capabilityClass="Joystick" capabilityDeviceIdx="1">
//     </Mate>
//     <Mate name="Ricky">
//       <Option name="CWRoration" value="f">
//       <KeyAction capabilityClass="Keyboard" capabilityDeviceIdx="1" hardwareKey="127">
//         <KeyActionName name="BlockEvent::Rotate"/>
//         <KeyActionName name="SwapperEvent::MoveUp"/>
//       </KeyAction>
//       <KeyAction capabilityClass="Keyboard" capabilityDeviceIdx="1" hardwareKey="89">
//         <KeyActionName name="BlockEvent::Next"/>
//       </KeyAction>
//       <Capability capabilityClass="Joystick" capabilityDeviceIdx="0">
//     </Mate>
//   </Team>
//   <Team name="Team2">
//     ...
//   </Team>
//   <PlayedHistory>
//     <Game name="Classic">
//     <Game name="Pentas">
//   </PlayedHistory>
// </GameAllPreferences>

// WARNING! PlayedHistory games are stored in reverse order!

static const std::string s_sGamePreferencesNodeName = "GameAllPreferences";
static const std::string s_sGamePreferencesAppNameAttr = "appName";
static const std::string s_sGamePreferencesCurrentGameAttr = "currentGame";
static const std::string s_sGamePreferencesCurrentThemeAttr = "currentTheme";
static const std::string s_sPreferencesOptionNodeName = "Option";
static const std::string s_sPreferencesOptionNameAttr = "name";
static const std::string s_sPreferencesOptionValueAttr = "value";
static const std::string s_sPreferencesTeamNodeName = "Team";
static const std::string s_sPreferencesTeamNameAttr = "name";
static const std::string s_sPreferencesTeamPlayerNodeName = "Mate";
static const std::string s_sPreferencesTeamPlayerNameAttr = "name";
static const std::string s_sPreferencesTeamPlayerKeyActionNodeName = "KeyAction";
static const std::string s_sPreferencesTeamPlayerKeyActionCapabilityClassAttr = "capabilityClass";
static const std::string s_sPreferencesTeamPlayerKeyActionCapabilityDeviceIdxAttr = "capabilityDeviceIdx";
static const std::string s_sPreferencesTeamPlayerKeyActionHardwareKeyAttr = "hardwareKey";
static const std::string s_sPreferencesTeamPlayerKeyActionKeyActionNameNodeName = "KeyActionName";
static const std::string s_sPreferencesTeamPlayerKeyActionKeyActionNameNameAttr = "name";
static const std::string s_sPreferencesTeamPlayerCapabilityNodeName = "Capability";
static const std::string s_sPreferencesTeamPlayerCapabilityCapabilityClassAttr = "capabilityClass";
static const std::string s_sPreferencesTeamPlayerCapabilityCapabilityDeviceIdxAttr = "capabilityDeviceIdx";
static const std::string s_sPreferencesPlayedHistoryNodeName = "PlayedHistory";
static const std::string s_sPreferencesPlayedHistoryGameNodeName = "Game";
static const std::string s_sPreferencesPlayedHistoryGameNameAttr = "name";

XmlPreferencesLoader::XmlPreferencesLoader(const shared_ptr<StdConfig>& refStdConfig
										, const shared_ptr<XmlGameFiles>& refXmlGameFiles)
: m_refStdConfig(refStdConfig)
, m_refXmlGameFiles(refXmlGameFiles)
{
	assert(refStdConfig);
	assert(refXmlGameFiles);
}
shared_ptr<AllPreferences> XmlPreferencesLoader::getPreferences() const noexcept
{
	// Create default initialized instance
	auto refAllPrefs = std::make_shared<AllPreferences>(m_refStdConfig);
	refAllPrefs->setEditMode(true);
	const File oPrefFile = m_refXmlGameFiles->getPreferencesFile();
	if (oPrefFile.isDefined() && !oPrefFile.isBuffered()) {
		const std::string& sPath = oPrefFile.getFullPath();
		try {
			xmlpp::DomParser oParser;
			//oParser.set_validate();
			//We just want the text to be resolved/unescaped automatically.
			oParser.set_substitute_entities();
			oParser.parse_file(sPath);
			if (oParser) {
				const xmlpp::Node* p0Node = oParser.get_document()->get_root_node();
				const xmlpp::Element* p0RootElement = dynamic_cast<const xmlpp::Element*>(p0Node);
				if (p0RootElement == nullptr) {
					throw std::runtime_error("Error: root node is not an element");
				}
				if (s_sGamePreferencesNodeName != p0RootElement->get_name()) {
					throw std::runtime_error("Error: root node is not named " + s_sGamePreferencesNodeName);
				}
				if (p0RootElement->get_attribute_value(s_sGamePreferencesAppNameAttr) != m_refStdConfig->getAppName()) {
					throw std::runtime_error("Error: attribute '" + s_sGamePreferencesAppNameAttr + "' should be '" + m_refStdConfig->getAppName() + "'");
				}
				//
				const bool bOk = parseXmlGameAllPreferences(refAllPrefs, p0RootElement);
				if (!bOk) {
					std::cout << "Errors encountered while parsing";
					std::cout << " preferences file '" << sPath << "'";
					std::cout << '\n';
				}
			} else {
				std::cout << "Could not parse";
				std::cout << " preferences file '" << sPath << "'";
				std::cout << '\n';
			}
		} catch(const std::exception& ex) {

			std::cout << "Exception caught";
			std::cout << " loading preferences '" << sPath << "'";
			std::cout << ": " << ex.what() << '\n';
		}
	}
	refAllPrefs->setEditMode(false);
	return refAllPrefs;
}
bool XmlPreferencesLoader::parseXmlGameAllPreferences(const shared_ptr<AllPreferences>& refPrefs
														, const xmlpp::Element* p0RootElement) const
{
	const std::string sCurrentGame = p0RootElement->get_attribute_value(s_sGamePreferencesCurrentGameAttr);
	if (! sCurrentGame.empty()) {
		checkIsValidName(sCurrentGame);
	}
	refPrefs->setGameName(sCurrentGame);
	//
	const std::string sCurrentTheme = p0RootElement->get_attribute_value(s_sGamePreferencesCurrentThemeAttr);
	if (! sCurrentTheme.empty()) {
		checkIsValidName(sCurrentTheme);
	}
	refPrefs->setThemeName(sCurrentTheme);
	// game options
	const NamedObjIndex< shared_ptr<Option> >& oOptions = m_refStdConfig->getOptions(OwnerType::GAME);
	const int32_t nTotGameOptions = oOptions.size();
	const std::vector<Variant> aValues = parseOptions(p0RootElement, "GAME", oOptions);
	for (int32_t nOptionIdx = 0; nOptionIdx < nTotGameOptions; ++nOptionIdx) {
		if (!oOptions.getObj(nOptionIdx)->isReadonly()) {
			refPrefs->setOptionValue(oOptions.getName(nOptionIdx), aValues[nOptionIdx]);
		}
	}
	//
	parseTeams(refPrefs, p0RootElement);
	//
	parsePlayedHistory(refPrefs, p0RootElement);
	return true;
}
void XmlPreferencesLoader::parseTeams(const shared_ptr<AllPreferences>& refPrefs, const xmlpp::Element* p0RootElement) const
{
	int32_t nTotParsedTeams = 0;
	const xmlpp::Node::NodeList oChildren = p0RootElement->get_children(s_sPreferencesTeamNodeName);
	for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
		const xmlpp::Element* p0TeamElement = dynamic_cast<const xmlpp::Element*>(*it);
		if (p0TeamElement == nullptr) {
			continue; // for (it -----
		}
		if (nTotParsedTeams + 1 >= refPrefs->getTotTeams()) {
			refPrefs->setTotTeams(nTotParsedTeams + 1);
		}
		//
		const std::string sTeamName = p0TeamElement->get_attribute_value(s_sPreferencesTeamNameAttr);
		if (sTeamName.empty()) {
			throw std::runtime_error("Error: team name is empty");
		}
		auto& refTeam = refPrefs->getTeamFull(nTotParsedTeams);
		bool bOk = refTeam->setName(sTeamName);
		if (!bOk) {
			// Name already in use. Swap current name with other team.
			const std::string sCurName = refTeam->getName();
			auto& refOtherTeam = refPrefs->getTeamFull(sTeamName);
			refOtherTeam->setName(sCurName);
			bOk = refTeam->setName(sTeamName);
			assert(bOk);
		}
		// team options
		const NamedObjIndex< shared_ptr<Option> >& oOptions = m_refStdConfig->getOptions(OwnerType::TEAM);
		const int32_t nTotTeamOptions = oOptions.size();
		const std::vector<Variant> aValues = parseOptions(p0TeamElement, "TEAM", oOptions);
		for (int32_t nOptionIdx = 0; nOptionIdx < nTotTeamOptions; ++nOptionIdx) {
			if (!oOptions.getObj(nOptionIdx)->isReadonly()) {
				refTeam->setOptionValue(oOptions.getName(nOptionIdx), aValues[nOptionIdx]);
			}
		}
		//
		parseMates(refPrefs, p0TeamElement, refTeam);
		++nTotParsedTeams;
	}
}
void XmlPreferencesLoader::parseMates(const shared_ptr<AllPreferences>& refPrefs, const xmlpp::Element* p0TeamElement
									, const shared_ptr<StdPreferences::Team>& refTeam) const
{
	int32_t nTotParsedMates = 0;
	const xmlpp::Node::NodeList oChildren = p0TeamElement->get_children(s_sPreferencesTeamPlayerNodeName);
	for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
		const xmlpp::Element* p0MateElement = dynamic_cast<const xmlpp::Element*>(*it);
		if (p0MateElement == nullptr) {
			continue; // for (it -----
		}
		const std::string sPlayerName = p0MateElement->get_attribute_value(s_sPreferencesTeamPlayerNameAttr);
		if (sPlayerName.empty()) {
			throw std::runtime_error("Error: player name is empty");
		}
		if (nTotParsedMates + 1 >= refTeam->getTotMates()) {
			const bool bOk = refTeam->setTotMates(nTotParsedMates + 1);
			if (!bOk) {
				throw std::runtime_error("Error: too many mates defined?");
			}
		}
		auto& refPlayer = refTeam->getMateFull(nTotParsedMates);
		bool bOk = refPlayer->setName(sPlayerName);
		if (!bOk) {
			const std::string sCurName = refPlayer->getName();
			auto& refOtherPlayer = refPrefs->getPlayerFull(sPlayerName);
			refOtherPlayer->setName(sCurName);
			bOk = refPlayer->setName(sPlayerName);
			assert(bOk);
		}
		// team options
		const NamedObjIndex< shared_ptr<Option> >& oOptions = m_refStdConfig->getOptions(OwnerType::PLAYER);
		const int32_t nTotPlayerOptions = oOptions.size();
		const std::vector<Variant> aValues = parseOptions(p0MateElement, "PLAYER", oOptions);
		for (int32_t nOptionIdx = 0; nOptionIdx < nTotPlayerOptions; ++nOptionIdx) {
			if (!oOptions.getObj(nOptionIdx)->isReadonly()) {
				refPlayer->setOptionValue(oOptions.getName(nOptionIdx), aValues[nOptionIdx]);
			}
		}
		//
		parseMateCapabilities(refPrefs, p0MateElement, refPlayer);
		parseMateKeyActions(refPrefs, p0MateElement, refPlayer);
		++nTotParsedMates;
	}
}
void XmlPreferencesLoader::parseMateCapabilities(const shared_ptr<AllPreferences>& refPrefs, const xmlpp::Element* p0MateElement
												, const shared_ptr<StdPreferences::Player>& refMate) const
{
	const xmlpp::Node::NodeList oChildren = p0MateElement->get_children(s_sPreferencesTeamPlayerCapabilityNodeName);
	for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
		const xmlpp::Element* p0CapabilityElement = dynamic_cast<const xmlpp::Element*>(*it);
		if (p0CapabilityElement == nullptr) {
			continue; // for (it -----
		}
		const std::string sCapaClass = p0CapabilityElement->get_attribute_value(s_sPreferencesTeamPlayerCapabilityCapabilityClassAttr);
		if (sCapaClass.empty()) {
			throw std::runtime_error("Error: capability class is empty");
		}
		const auto oCapaClass = stmi::Capability::getCapabilityClassIdClass(sCapaClass);
		if (!oCapaClass) {
			throw std::runtime_error("Error: capability class " + sCapaClass + " not found");
		}
		const std::string sCapaIdx = p0CapabilityElement->get_attribute_value(s_sPreferencesTeamPlayerCapabilityCapabilityDeviceIdxAttr);
		const auto oPairCapaIdx = Util::strToNumber<int32_t>(sCapaIdx, false, true, 0, false, -1);
		const std::string& sError = oPairCapaIdx.second;
		if (! sError.empty()) {
			throw std::runtime_error(sError);
		}
		const int32_t nCapaIdx = oPairCapaIdx.first;
		//
		auto refCapa = refPrefs->getCapabilityFromClassDeviceIdx(oCapaClass, nCapaIdx);
		if (!refCapa) {
			// There are not enough devices with the given capability class
		} else {
			refMate->assignCapability(refCapa);
		}
	}
}
void XmlPreferencesLoader::parseMateKeyActions(const shared_ptr<AllPreferences>& refPrefs, const xmlpp::Element* p0MateElement
												, const shared_ptr<StdPreferences::Player>& refMate) const
{
	const xmlpp::Node::NodeList oChildren = p0MateElement->get_children(s_sPreferencesTeamPlayerKeyActionNodeName);
	for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
		const xmlpp::Element* p0KeyActionElement = dynamic_cast<const xmlpp::Element*>(*it);
		if (p0KeyActionElement == nullptr) {
			continue; // for (it -----
		}
		const auto p0CapaAttr = p0KeyActionElement->get_attribute(s_sPreferencesTeamPlayerKeyActionCapabilityClassAttr);
		if (p0CapaAttr == nullptr) {
			// AI player doesn't write capability
			continue;
		}
		const std::string sCapaClass = p0CapaAttr->get_value();
		if (sCapaClass.empty()) {
			throw std::runtime_error("Error: capability class is empty");
		}
		const auto oCapaClass = stmi::Capability::getCapabilityClassIdClass(sCapaClass);
		if (! oCapaClass) {
			std::cout << "Warning: capability class " << sCapaClass << " not found" << '\n';
			// capability class not found: key undefined
			continue;
		}
		const std::string sCapaIdx = p0KeyActionElement->get_attribute_value(s_sPreferencesTeamPlayerKeyActionCapabilityDeviceIdxAttr);
		const auto oPairCapaIdx = Util::strToNumber<int32_t>(sCapaIdx, false, true, 0, false, -1);
		const std::string& sError = oPairCapaIdx.second;
		if (! sError.empty()) {
			throw std::runtime_error(sError);
		}
		const int32_t nCapaIdx = oPairCapaIdx.first;
		//
		shared_ptr<stmi::Capability> refCapa = refPrefs->getCapabilityFromClassDeviceIdx(oCapaClass, nCapaIdx);
		if (! refCapa) {
			// There are not enough devices with the given capability class
			continue; // for (it -----
		}
		const std::string sHK = p0KeyActionElement->get_attribute_value(s_sPreferencesTeamPlayerKeyActionHardwareKeyAttr);
		const auto oPairHK = Util::strToNumber<int32_t>(sHK, false, true, static_cast<int32_t>(stmi::HK_NULL) + 1, false, -1);
		{
		const std::string& sError = oPairHK.second;
		if (! sError.empty()) {
			throw std::runtime_error(sError);
		}
		}
		const int32_t nHK = oPairHK.first;
		//
		const auto eHK = static_cast<stmi::HARDWARE_KEY>(nHK);
		const bool bHKIsValid = stmi::HardwareKeys::isValid(eHK);
		if (!bHKIsValid) {
			throw std::runtime_error("Error: hardware key " + sHK + " not valid");
		}
		const int32_t nKeyActionId = parseKeyActionNames(p0KeyActionElement);
		if (nKeyActionId < 0) {
			throw std::runtime_error("Error: key action names not valid");
		}
		refMate->setKeyValue(nKeyActionId, refCapa.get(), eHK);
	}
}
int32_t XmlPreferencesLoader::parseKeyActionNames(const xmlpp::Element* p0KeyActionElement) const
{
	const xmlpp::Node::NodeList oChildren = p0KeyActionElement->get_children(s_sPreferencesTeamPlayerKeyActionKeyActionNameNodeName);
	for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
		const xmlpp::Element* p0KeyActionNameElement = dynamic_cast<const xmlpp::Element*>(*it);
		if (p0KeyActionNameElement == nullptr) {
			continue; // for (it -----
		}
		const std::string sKAName = p0KeyActionNameElement->get_attribute_value(s_sPreferencesTeamPlayerKeyActionKeyActionNameNameAttr);
		if (sKAName.empty()) {
			throw std::runtime_error("Error: key action name cannot be empty");
		}
		const int32_t nKeyActionId = m_refStdConfig->getKeyActionId(sKAName);
		if (nKeyActionId >= 0) {
			return nKeyActionId;
		}
	}
	return -1;
}
std::vector<Variant> XmlPreferencesLoader::parseOptions(const xmlpp::Element* p0RootElement, const std::string& sOwnerType
														, const NamedObjIndex<shared_ptr<Option>>& oOptions) const
{
	std::vector<Variant> aValues;
	aValues.resize(oOptions.size());
	const xmlpp::Node::NodeList oChildren = p0RootElement->get_children(s_sPreferencesOptionNodeName);
	for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
		const xmlpp::Element* p0OptionElement = dynamic_cast<const xmlpp::Element*>(*it);
		if (p0OptionElement == nullptr) {
			continue; // for (it -----
		}
		const std::string sOptionName = p0OptionElement->get_attribute_value(s_sPreferencesOptionNameAttr);
		const int32_t nOptionIdx = oOptions.getIndex(sOptionName);
		if (nOptionIdx < 0) {
			throw std::runtime_error("Error: invalid " + sOwnerType + " option name '" + sOptionName + "'");
		}
		const std::string sOptionValue = p0OptionElement->get_attribute_value(s_sPreferencesOptionValueAttr);
		auto& refOption = oOptions.getObj(nOptionIdx);
		const auto oPair = refOption->getValueFromCode(sOptionValue);
		if (! oPair.second.empty()) {
			throw std::runtime_error("Error: " + oPair.second);
		}
		aValues[nOptionIdx] = oPair.first;
	}
	return aValues;
}
void XmlPreferencesLoader::checkIsValidName(const std::string& sName) const
{
	bool bValid = true;
	if (sName.empty()) {
		bValid = false;
	} else {
		for (const auto& c : sName) {
			if (!(std::isalnum(c) || (c == '-') || (c == '_'))) {
				bValid = false;
				break; //for ----
			}
		}
	}
	if (!bValid) {
		throw std::runtime_error("Error: invalid name '" + sName + "'");
	}
}
void XmlPreferencesLoader::parsePlayedHistory(const shared_ptr<AllPreferences>& refPrefs, const xmlpp::Element* p0RootElement) const
{
	const xmlpp::Element* p0Found = nullptr;
	const xmlpp::Node::NodeList oChildren = p0RootElement->get_children(s_sPreferencesPlayedHistoryNodeName);
	for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
		const xmlpp::Element* p0HistoryElement = dynamic_cast<const xmlpp::Element*>(*it);
		if (p0HistoryElement == nullptr) {
			continue; // for (it -----
		}
		if (p0Found != nullptr) {
			throw std::runtime_error("Error: at most one element '" + s_sPreferencesPlayedHistoryNodeName + "' expected");
		}
		p0Found = p0HistoryElement;
	}
	if (p0Found != nullptr) {
		parsePlayedHistoryGames(refPrefs, p0Found);
	}
}
void XmlPreferencesLoader::parsePlayedHistoryGames(const shared_ptr<AllPreferences>& refPrefs, const xmlpp::Element* p0Element) const
{
	const xmlpp::Node::NodeList oChildren = p0Element->get_children(s_sPreferencesPlayedHistoryGameNodeName);
	for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
		const xmlpp::Element* p0GameElement = dynamic_cast<const xmlpp::Element*>(*it);
		if (p0GameElement == nullptr) {
			continue; // for (it -----
		}
		const std::string sGameName = p0GameElement->get_attribute_value(s_sPreferencesPlayedHistoryGameNameAttr);
		checkIsValidName(sGameName);
		refPrefs->addGameToPlayedHistory(sGameName);
	}	
}

shared_ptr<AllPreferences> XmlPreferencesLoader::getPreferencesCopy(const shared_ptr<AllPreferences>& refAllPreferences) const noexcept
{
	assert(refAllPreferences);
	// no recycling
	return std::make_shared<AllPreferences>(*refAllPreferences);
}
bool XmlPreferencesLoader::updatePreferences(const shared_ptr<AllPreferences>& refAllPreferences) noexcept
{
	assert(refAllPreferences);
	assert(refAllPreferences->getStdConfig() == m_refStdConfig);
	const File oPrefsFile = m_refXmlGameFiles->getPreferencesFile();
	if ((!oPrefsFile.isDefined()) || oPrefsFile.isBuffered()) {
		return false; //--------------------------------------------------------
	}
	// Create the document
	xmlpp::DomParser oParser;
	xmlpp::Document* p0Document = oParser.get_document();
	if (p0Document == nullptr) {
		return false; //--------------------------------------------------------
	}
	xmlpp::Element* p0RootElement = p0Document->create_root_node(s_sGamePreferencesNodeName);
	p0RootElement->set_attribute(s_sGamePreferencesAppNameAttr, refAllPreferences->getAppConfig()->getAppName());
	p0RootElement->set_attribute(s_sGamePreferencesCurrentGameAttr, refAllPreferences->getGameName());
	p0RootElement->set_attribute(s_sGamePreferencesCurrentThemeAttr, refAllPreferences->getThemeName());
	writeOptions(refAllPreferences, OwnerType::GAME
				, shared_ptr<StdPreferences::Team>{}, shared_ptr<StdPreferences::Player>{}, p0RootElement);
	writeTeams(refAllPreferences, p0RootElement);
	writePlayedHistory(refAllPreferences, p0RootElement);

//std::cout << "XmlHighscoresLoader::updateHighscore File=" << oHSFile.getFullPath() << '\n';
	try {
		XmlUtil::makePath(oPrefsFile);
		p0Document->write_to_file_formatted(oPrefsFile.getFullPath());
	} catch (const xmlpp::exception& ) {
		return false; //--------------------------------------------------------
	}
	return true;
}
void XmlPreferencesLoader::writeOptions(const shared_ptr<AllPreferences>& refAllPreferences, const OwnerType& eOwnerType
										, const shared_ptr<StdPreferences::Team>& refTeam, const shared_ptr<StdPreferences::Player>& refMate
										, xmlpp::Element* p0RootElement)
{
	const NamedObjIndex<shared_ptr<Option>> oOptions = refAllPreferences->getAppConfig()->getOptions(eOwnerType);
	const int32_t nTotOptions = oOptions.size();
	for (int32_t nIdx = 0; nIdx < nTotOptions; ++nIdx) {
		xmlpp::Element* p0OptionElement = p0RootElement->add_child(s_sPreferencesOptionNodeName);
		const shared_ptr<Option>& refOption = oOptions.getObj(nIdx);
		const auto& sName = refOption->getName();
		const Variant oValue = [&](const OwnerType& eOwnerType)
		{
			if (eOwnerType == OwnerType::GAME) {
				assert(!refTeam);
				assert(!refMate);
				return refAllPreferences->getOptionValue(sName);
			} else if (eOwnerType == OwnerType::TEAM) {
				assert(refTeam);
				assert(!refMate);
				return refTeam->getOptionValue(sName);
			} else {
				assert(eOwnerType == OwnerType::PLAYER);
				assert(!refTeam);
				assert(refMate);
				return refMate->getOptionValue(sName);
			}
		}(eOwnerType);
		p0OptionElement->set_attribute(s_sPreferencesOptionNameAttr, sName);
		p0OptionElement->set_attribute(s_sPreferencesOptionValueAttr, refOption->getValueAsCode(oValue));
	}
}
void XmlPreferencesLoader::writeTeams(const shared_ptr<AllPreferences>& refAllPreferences, xmlpp::Element* p0RootElement)
{
	const int32_t nTotTeams = refAllPreferences->getTotTeams();
	for (int32_t nTeam = 0; nTeam < nTotTeams; ++nTeam) {
		xmlpp::Element* p0TeamElement = p0RootElement->add_child(s_sPreferencesTeamNodeName);
		const auto& refTeam = refAllPreferences->getTeamFull(nTeam);
		p0TeamElement->set_attribute(s_sPreferencesTeamNameAttr, refTeam->getName());
		writeOptions(refAllPreferences, OwnerType::TEAM
					, refTeam, shared_ptr<StdPreferences::Player>{}, p0TeamElement);
		writeMates(refAllPreferences, refTeam, p0TeamElement);
	}

}
void XmlPreferencesLoader::writeMates(const shared_ptr<AllPreferences>& refAllPreferences
									, const shared_ptr<StdPreferences::Team>& refTeam, xmlpp::Element* p0TeamElement)
{
	const int32_t nTotMates = refTeam->getTotMates();
	for (int32_t nMate = 0; nMate < nTotMates; ++nMate) {
		xmlpp::Element* p0MateElement = p0TeamElement->add_child(s_sPreferencesTeamPlayerNodeName);
		const auto& refMate = refTeam->getMateFull(nMate);
		p0MateElement->set_attribute(s_sPreferencesTeamPlayerNameAttr, refMate->getName());
		writeOptions(refAllPreferences, OwnerType::PLAYER
					, shared_ptr<StdPreferences::Team>{}, refMate, p0MateElement);
		writeKeyActions(refAllPreferences, refMate, p0MateElement);
		writeCapabilities(refAllPreferences, refMate, p0MateElement);
	}
}
void XmlPreferencesLoader::writeKeyActions(const shared_ptr<AllPreferences>& refAllPreferences
											, const shared_ptr<StdPreferences::Player>& refMate, xmlpp::Element* p0MateElement)
{
	const int32_t nTotKeyActions = m_refStdConfig->getTotKeyActions();
	for (int32_t nKeyAction = 0; nKeyAction < nTotKeyActions; ++nKeyAction) {
		xmlpp::Element* p0KAElement = p0MateElement->add_child(s_sPreferencesTeamPlayerKeyActionNodeName);
		const auto oPair = refMate->getKeyValue(nKeyAction);
		const stmi::Capability* p0Capability = oPair.first;
		if (p0Capability == nullptr) {
			// is AI or no capability
			continue;
		}
		const stmi::HARDWARE_KEY eHK = oPair.second;
		const int32_t nHK = static_cast<int32_t>(eHK);
		const int32_t nDevIdx = refAllPreferences->getCapabilityClassDeviceIdx(p0Capability);
		assert(nDevIdx >= 0);
		p0KAElement->set_attribute(s_sPreferencesTeamPlayerKeyActionCapabilityClassAttr, p0Capability->getCapabilityClass().getId());
		p0KAElement->set_attribute(s_sPreferencesTeamPlayerKeyActionCapabilityDeviceIdxAttr, std::to_string(nDevIdx));
		p0KAElement->set_attribute(s_sPreferencesTeamPlayerKeyActionHardwareKeyAttr, std::to_string(nHK));
		const auto& oKeyAction = m_refStdConfig->getKeyAction(nKeyAction);
		const auto& aNames = oKeyAction.getNames();
		for (const std::string& sName : aNames) {
			xmlpp::Element* p0NameElement = p0KAElement->add_child(s_sPreferencesTeamPlayerKeyActionKeyActionNameNodeName);
			p0NameElement->set_attribute(s_sPreferencesTeamPlayerKeyActionKeyActionNameNameAttr, sName);
		}
	}
}
void XmlPreferencesLoader::writeCapabilities(const shared_ptr<AllPreferences>& refAllPreferences
											, const shared_ptr<StdPreferences::Player>& refMate, xmlpp::Element* p0MateElement)
{
	auto aMateCapas = refMate->getCapabilities();
	for (const auto& refCapa : aMateCapas) {
		xmlpp::Element* p0CapaElement = p0MateElement->add_child(s_sPreferencesTeamPlayerCapabilityNodeName);
		assert(refCapa);
		const int32_t nDevIdx = refAllPreferences->getCapabilityClassDeviceIdx(refCapa.get());
		assert(nDevIdx >= 0);
		p0CapaElement->set_attribute(s_sPreferencesTeamPlayerCapabilityCapabilityClassAttr, refCapa->getCapabilityClass().getId());
		p0CapaElement->set_attribute(s_sPreferencesTeamPlayerCapabilityCapabilityDeviceIdxAttr, std::to_string(nDevIdx));
	}
}
void XmlPreferencesLoader::writePlayedHistory(const shared_ptr<AllPreferences>& refAllPreferences, xmlpp::Element* p0RootElement)
{
	const std::vector<std::string>& aGameNames = refAllPreferences->getPlayedGameHistory();
	if (aGameNames.empty()) {
		return;
	}
	xmlpp::Element* p0HistoryElement = p0RootElement->add_child(s_sPreferencesPlayedHistoryNodeName);
	assert(p0HistoryElement);
	for (int32_t nIdx = static_cast<int32_t>(aGameNames.size()) - 1; nIdx >= 0; --nIdx) {
		xmlpp::Element* p0GameElement = p0HistoryElement->add_child(s_sPreferencesPlayedHistoryGameNodeName);
		p0GameElement->set_attribute(s_sPreferencesPlayedHistoryGameNameAttr, aGameNames[nIdx]);
	}
}

} // namespace stmg
