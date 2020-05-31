/*
 * File:   xmlhighscoresloader.cc
 *
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

#include "xmlhighscoresloader.h"

#include "xmlutilfile.h"
#include "xmlgamefiles.h"

#include <stmm-games-file/file.h>

#include <stmm-games/util/util.h>
#include <stmm-games/appconfig.h>
#include <stmm-games/apppreferences.h>
#include <stmm-games/highscoresdefinition.h>
#include <stmm-games/variable.h>
#include <stmm-games/highscore.h>

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>

#include <vector>
#include <cassert>
#include <iostream>
#include <exception>
#include <cstdint>
#include <list>
#include <stdexcept>
#include <utility>

namespace stmg
{

// <GameHighscores appName="jointris" gameName="Classic">
//   <Highscores code="Easy_1" title="Difficulty: Easy, Teams: 1">
//     <Score team="foo" format="3">
//       <Value value="21212"  format="2">
//       <Value value="34"     format="5">
//     </Score>
//     <Score team="bar">
//       <Value value="18999"  format="2">
//       <Value value="45"     format="5">
//     </Score>
//     <Score team="boo">
//       <Value value="15103"  format="2">
//       <Value value="35"     format="5">
//     </Score>
//     <Score team="loo">
//       <Value value="15103"  format="2">
//       <Value value="38"     format="5">
//     </Score>
//   </Highscores>
//   <Highscores code="Medium_1" title="Difficulty: Medium, Teams: 1">
//      ...
//   </Highscores>
// </GameHighscores>

static const std::string s_sGameHighscoresNodeName = "GameHighscores";
static const std::string s_sGameHighscoresAppNameAttr = "appName";
static const std::string s_sGameHighscoresGameNameAttr = "gameName";
static const std::string s_sHighscoresNodeName = "Highscores";
static const std::string s_sHighscoresCodeAttr = "code";
static const std::string s_sHighscoresTitleAttr = "title";
static const std::string s_sHighscoresScoreNodeName = "Score";
static const std::string s_sHighscoresScoreTeamAttr = "team";
static const std::string s_sHighscoresScoreValueNodeName = "Value";
static const std::string s_sHighscoresScoreValueValueAttr = "value";
static const std::string s_sHighscoresScoreValueFormatAttr = "format";

XmlHighscoresLoader::XmlHighscoresLoader(const shared_ptr<AppConfig>& refAppConfig
										, const shared_ptr<XmlGameFiles>& refXmlGameFiles)
: m_refAppConfig(refAppConfig)
, m_refXmlGameFiles(refXmlGameFiles)
{
	assert(refAppConfig);
	assert(refXmlGameFiles);
}
shared_ptr<Highscore> XmlHighscoresLoader::getHighscore(const std::string& sGameName, const AppPreferences& oPreferences
														, const shared_ptr<HighscoresDefinition>& refHighscoresDefinition) const noexcept
{
	assert(refHighscoresDefinition);
	const File oHSFile = m_refXmlGameFiles->getHighscoreFile(sGameName);
	if ((!oHSFile.isDefined()) || oHSFile.isBuffered()) {
		return shared_ptr<Highscore>{}; //--------------------------------------
	}
	const auto oPairCode = refHighscoresDefinition->getCodeFromPreferences(oPreferences);
	if (!oPairCode.first) {
		return shared_ptr<Highscore>{}; //--------------------------------------
	}
	const auto oPairTitle = refHighscoresDefinition->getTitleFromPreferences(oPreferences);
	if (!oPairTitle.first) {
		return shared_ptr<Highscore>{}; //--------------------------------------
	}
		
	auto aHSs = parseGameHighscores(refHighscoresDefinition, oHSFile, sGameName
									, false, oPairCode.second, oPairTitle.second);
	assert(aHSs.size() == 1);
	return std::move(aHSs[0]);
}
std::vector<shared_ptr<Highscore>> XmlHighscoresLoader::getHighscores(const std::string& sGameName
																	, const shared_ptr<HighscoresDefinition>& refHighscoresDefinition) const noexcept
{
	assert(refHighscoresDefinition);
	const File oHSFile = m_refXmlGameFiles->getHighscoreFile(sGameName);
	if ((!oHSFile.isDefined()) || oHSFile.isBuffered()) {
		return std::vector<shared_ptr<Highscore>>{}; //--------------------------------------
	}
	return parseGameHighscores(refHighscoresDefinition, oHSFile, sGameName, true, "", "");
}
std::vector<shared_ptr<Highscore>> XmlHighscoresLoader::parseGameHighscores(
											const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
											, const File& oHSFile, const std::string& sGameName
											, bool bAll, const std::string& sCode, const std::string& sTitle) const
{
	const std::string& sPath = oHSFile.getFullPath();
//std::cout << "XmlHighscoresLoader::parseGameHighscores()  sPath=" << sPath << '\n';
//std::cout << "XmlHighscoresLoader::parseGameHighscores()  sCode=" << sCode << "  sTitle=" << sTitle << '\n';
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
			if (s_sGameHighscoresNodeName != p0RootElement->get_name()) {
				throw std::runtime_error("Error: root node is not named " + s_sGameHighscoresNodeName);
			}
			if (p0RootElement->get_attribute_value(s_sGameHighscoresAppNameAttr) != m_refAppConfig->getAppName()) {
				throw std::runtime_error("Error: attribute '" + s_sGameHighscoresAppNameAttr + "' should be '" + m_refAppConfig->getAppName() + "'");
			}
			if (p0RootElement->get_attribute_value(s_sGameHighscoresGameNameAttr) != sGameName) {
				throw std::runtime_error("Error: attribute '" + s_sGameHighscoresGameNameAttr + "' should be '" + sGameName + "'");
			}
			auto aHSs = parseXmlGameHighscores(refHighscoresDefinition, bAll, sCode, p0RootElement);
			if (!bAll) {
				if (aHSs.empty()) {
					aHSs.push_back(std::make_shared<Highscore>(refHighscoresDefinition, sCode, sTitle));
				}
			}
			return aHSs;  //----------------------------------------------------
		} else {
			std::cout << "Could not parse";
			std::cout << " highscores file '" << sPath << "'";
			std::cout << '\n';
		}
	} catch(const std::exception& ex) {

		std::cout << "Exception caught";
		std::cout << " loading highscores '" << sPath << "'";
		std::cout << ": " << ex.what() << '\n';
	}
	if (bAll) {
		return std::vector<shared_ptr<Highscore>>{};
	} else {
		// create new highscores
		return {std::make_shared<Highscore>(refHighscoresDefinition, sCode, sTitle)};
	}
}
int32_t XmlHighscoresLoader::findHighscoreWithCode(const std::vector<shared_ptr<Highscore>>& aHighscores
													, const std::string& sCode) const
{
	for (int32_t nIdx = 0; nIdx < static_cast<int32_t>(aHighscores.size()); ++nIdx) {
		const shared_ptr<Highscore>& refHighscore = aHighscores[nIdx];
		if (refHighscore->getCodeString() == sCode) {
			return nIdx; //-----------------------------------------------------
		}
	}
	return -1;
}
std::vector<shared_ptr<Highscore>> XmlHighscoresLoader::parseXmlGameHighscores(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
																				, bool bAll, const std::string& sCode
																				, const xmlpp::Element* p0RootElement) const
{
	std::vector<shared_ptr<Highscore>> aHighscores;
	const xmlpp::Node::NodeList oChildren = p0RootElement->get_children(s_sHighscoresNodeName);
	for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
		const xmlpp::Element* p0HighscoresElement = dynamic_cast<const xmlpp::Element*>(*it);
		if (p0HighscoresElement == nullptr) {
			continue; // for (it -----
		}
		if (!bAll) {
			if (p0HighscoresElement->get_attribute_value(s_sHighscoresCodeAttr) != sCode) {
				continue; // for (it -----
			}
		}
		auto refNewHighscore = parseXmlHighscores(refHighscoresDefinition, p0HighscoresElement);
		if (refNewHighscore) {
			const int32_t nIdx = findHighscoreWithCode(aHighscores, refNewHighscore->getCodeString());
			if (nIdx < 0) {
				aHighscores.push_back(refNewHighscore);
			}
		}
	}
	return aHighscores;
}
shared_ptr<Highscore> XmlHighscoresLoader::parseXmlHighscores(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
															, const xmlpp::Element* p0HighscoresElement) const
{
	const std::string sCode = p0HighscoresElement->get_attribute_value(s_sHighscoresCodeAttr);
	if (! refHighscoresDefinition->isValidCode(sCode)) {
		return {};
	}
	const std::string sTitle = p0HighscoresElement->get_attribute_value(s_sHighscoresTitleAttr);
	std::vector<Highscore::Score> aScores;
	const xmlpp::Node::NodeList oChildren = p0HighscoresElement->get_children(s_sHighscoresScoreNodeName);
	for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
		const xmlpp::Element* p0ScoreElement = dynamic_cast<const xmlpp::Element*>(*it);
		if (p0ScoreElement == nullptr) {
			continue; // for (it -----
		}
		Highscore::Score oScore = parseScore(refHighscoresDefinition, p0ScoreElement);
		if (! oScore.m_sTeam.empty()) {
			if (static_cast<int32_t>(aScores.size()) < refHighscoresDefinition->getMaxScores()) {
				aScores.push_back(std::move(oScore));
			}
		}
	}
	return std::make_shared<Highscore>(refHighscoresDefinition, sCode, sTitle, aScores);
}
Highscore::Score XmlHighscoresLoader::parseScore(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
												, const xmlpp::Element* p0ScoreElement) const
{
	Highscore::Score oScore;
	auto* p0TeamAttribute = p0ScoreElement->get_attribute(s_sHighscoresScoreTeamAttr);
	if (p0TeamAttribute == nullptr) {
		return oScore; //-------------------------------------------------------
	}
	const auto sTeamName = p0TeamAttribute->get_value();
	if (sTeamName.empty()) {
		return oScore; //-------------------------------------------------------
	}

	const auto& aScoreValues = refHighscoresDefinition->getScoreElements();
	//const int32_t nTotScoreValues = static_cast<int32_t>(aScoreValues.size());
	const xmlpp::Node::NodeList oChildren = p0ScoreElement->get_children(s_sHighscoresScoreValueNodeName);
	if (oChildren.size() != aScoreValues.size()) {
		return oScore; //-------------------------------------------------------
	}
	for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
		const xmlpp::Element* p0ValueElement = dynamic_cast<const xmlpp::Element*>(*it);
		if (p0ValueElement == nullptr) {
			continue; // for (it -----
		}
		auto* p0ValueAttribute = p0ValueElement->get_attribute(s_sHighscoresScoreValueValueAttr);
		if (p0ValueAttribute == nullptr) {
			return oScore; //---------------------------------------------------
		}
		const auto sValue = p0ValueAttribute->get_value();
		int32_t nValue = 0;
		{
			auto oPairValue = Util::strToNumber<int32_t>(sValue, false, false, -1, false, -1);
			if (! oPairValue.second.empty()) {
				return oScore; //-----------------------------------------------
			}
			nValue = oPairValue.first;
		}
		//
		auto* p0FormatAttribute = p0ValueElement->get_attribute(s_sHighscoresScoreValueFormatAttr);
		if (p0FormatAttribute == nullptr) {
			return oScore; //---------------------------------------------------
		}
		const auto sFormat = p0FormatAttribute->get_value();
		int32_t nFormat = 0;
		{
			auto oPairValue = Util::strToNumber<int32_t>(sFormat, false, false, -1, false, -1);
			if (! oPairValue.second.empty()) {
				return oScore; //-----------------------------------------------
			}
			nFormat = oPairValue.first;
		}
		auto eFormat = static_cast<Variable::VARIABLE_FORMAT>(nFormat);
		auto oValue = Variable::Value::create(nValue, eFormat);
		oScore.m_aValues.push_back(oValue);
	}
	if (oScore.m_aValues.size() != aScoreValues.size()) {
		oScore.m_sTeam.clear();
		oScore.m_aValues.clear();
	} else {
		oScore.m_sTeam = sTeamName;
	}
	return oScore;
}
bool XmlHighscoresLoader::updateHighscore(const std::string& sGameName, const AppPreferences& oPreferences
										, const Highscore& oHighscore) noexcept
{
//std::cout << "XmlHighscoresLoader::updateHighscore sGameName=" << sGameName << '\n';
	auto& refHighscoresDefinition = oHighscore.getHighscoresDefinition();
	const File oHSFile = m_refXmlGameFiles->getHighscoreFile(sGameName);
	if ((!oHSFile.isDefined()) || oHSFile.isBuffered()) {
		return false; //--------------------------------------------------------
	}
	auto aHighscores = parseGameHighscores(refHighscoresDefinition, oHSFile, sGameName, true, "", "");
//std::cout << "XmlHighscoresLoader::updateHighscore old aHighscores.size()=" << aHighscores.size() << '\n';

	const auto& sCode = oHighscore.getCodeString();
	const int32_t nIdx = findHighscoreWithCode(aHighscores, sCode);
	if (nIdx >= 0) {
		// erase the old highscores for the sCode
		aHighscores.erase(aHighscores.begin() + nIdx);
	}
	// Recreate the whole document
	xmlpp::DomParser oParser;
	xmlpp::Document* p0Document = oParser.get_document();
	if (p0Document == nullptr) {
		return false; //--------------------------------------------------------
	}
	xmlpp::Element* p0RootElement = p0Document->create_root_node(s_sGameHighscoresNodeName);
	p0RootElement->set_attribute(s_sGameHighscoresAppNameAttr, oPreferences.getAppConfig()->getAppName());
	p0RootElement->set_attribute(s_sGameHighscoresGameNameAttr, sGameName);
	for (const auto& refHighscore : aHighscores) {
		writeHighscores(p0RootElement, *refHighscore);
	}
	writeHighscores(p0RootElement, oHighscore);

//std::cout << "XmlHighscoresLoader::updateHighscore File=" << oHSFile.getFullPath() << '\n';
	try {
		XmlUtil::makePath(oHSFile);
		p0Document->write_to_file_formatted(oHSFile.getFullPath());
	} catch (const xmlpp::exception& ) {
		return false; //--------------------------------------------------------
	}
//std::cout << "XmlHighscoresLoader::updateHighscore File=" << oHSFile.getFullPath() << "  SUCCESS" << '\n';
	return true;
}
void XmlHighscoresLoader::writeHighscores(xmlpp::Element* p0RootElement, const Highscore& oHighscore)
{
	xmlpp::Element* p0HighscoresElement = p0RootElement->add_child(s_sHighscoresNodeName);
	p0HighscoresElement->set_attribute(s_sHighscoresCodeAttr, oHighscore.getCodeString());
	p0HighscoresElement->set_attribute(s_sHighscoresTitleAttr, oHighscore.getTitleString());

	const int32_t nTotScores = oHighscore.getTotScores();
	for (int32_t nPos = 0; nPos < nTotScores; ++nPos) {
		const Highscore::Score& oScore = oHighscore.getScore(nPos);
		xmlpp::Element* p0ScoreElement = p0HighscoresElement->add_child(s_sHighscoresScoreNodeName);
		p0ScoreElement->set_attribute(s_sHighscoresScoreTeamAttr, oScore.m_sTeam);
		for (const auto& oValue : oScore.m_aValues) {
			xmlpp::Element* p0ValueElement = p0ScoreElement->add_child(s_sHighscoresScoreValueNodeName);
			p0ValueElement->set_attribute(s_sHighscoresScoreValueValueAttr, std::to_string(oValue.get()));
			p0ValueElement->set_attribute(s_sHighscoresScoreValueFormatAttr, std::to_string(static_cast<int32_t>(oValue.getFormat())));
		}	
	}
}

} // namespace stmg
