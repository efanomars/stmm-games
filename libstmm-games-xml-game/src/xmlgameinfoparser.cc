/*
 * File:   xmlgameinfoparser.cc
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

#include "xmlgameinfoparser.h"

#include "gameinfoctx.h"

#include <stmm-games-xml-base/conditionalctx.h>
#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlcommonparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/xmlutil/xmlvariantsetparser.h>

#include <stmm-games-file/gameloader.h>
#include <stmm-games-file/gameconstraints.h>

#include <stmm-games/appconfig.h>
#include <stmm-games/highscoresdefinition.h>
#include <stmm-games/named.h>
#include <stmm-games/prefselector.h>
#include <stmm-games/util/namedobjindex.h>
#include <stmm-games/util/util.h>
#include <stmm-games/ownertype.h>
#include <stmm-games/variable.h>

#include <glibmm/ustring.h>

#include <libxml++/libxml++.h>

#include <vector>
#include <cassert>
#include <algorithm>
//#include <iostream>
#include <cstdint>
#include <type_traits>

namespace stmg { class File; }
namespace stmg { class ParserCtx; }

namespace stmg
{

AssignableNamed GameInfoCtx::s_oDummy{};

static const std::string s_sGameNodeName = "Game";
static const std::string s_sGameInternalNameAttr = "internalName";
static const std::string s_sGameDifficultyAttr = "difficulty";
static const std::string s_sGameTestingAttr = "testing";

static const std::string s_sGameMinGameIntervalAttr = "minInterval";
static const std::string s_sGameInitialGameIntervalAttr = "initialInterval";
static const std::string s_sGameMaxViewTicksAttr = "maxViewTicks";
static const std::string s_sGameAdditionalHighscoresWaitAttr = "additionalHighscoresWait";
static const std::string s_sGameSoundScaleXAttr = "soundScaleX";
static const std::string s_sGameSoundScaleYAttr = "soundScaleY";
static const std::string s_sGameSoundScaleZAttr = "soundScaleZ";

static const std::string s_sGameDescriptionNodeName = "Description";

static const std::string s_sGameAuthorNodeName = "Author";
static const std::string s_sGameAuthorNameAttr = "name";
static const std::string s_sGameAuthorEmailAttr = "email";
static const std::string s_sGameAuthorWebSiteAttr = "website";
static const std::string s_sGameAuthorRoleAttr = "role";

static const std::string s_sGameConstraintsNodeName = "Constraints";
static const std::string s_sGameConstraintsTeamsAttr = "teams";
static const std::string s_sGameConstraintsTeamsMinAttr = "teamsMin";
static const std::string s_sGameConstraintsTeamsMaxAttr = "teamsMax";
static const std::string s_sGameConstraintsLevelsAttr = "levels";
static const std::string s_sGameConstraintsLevelsMinAttr = "levelsMin";
static const std::string s_sGameConstraintsLevelsMaxAttr = "levelsMax";
static const std::string s_sGameConstraintsTeamsPerLevelAttr = "teamsPerLevel";
static const std::string s_sGameConstraintsTeamsPerLevelMinAttr = "teamsPerLevelMin";
static const std::string s_sGameConstraintsTeamsPerLevelMaxAttr = "teamsPerLevelMax";
static const std::string s_sGameConstraintsMatesPerTeamAttr = "matesPerTeam";
static const std::string s_sGameConstraintsMatesPerTeamMinAttr = "matesPerTeamMin";
static const std::string s_sGameConstraintsMatesPerTeamMaxAttr = "matesPerTeamMax";
static const std::string s_sGameConstraintsPlayersAttr = "players";
static const std::string s_sGameConstraintsPlayersMinAttr = "playersMin";
static const std::string s_sGameConstraintsPlayersMaxAttr = "playersMax";
static const std::string s_sGameConstraintsAITeamsAttr = "aiTeams";
static const std::string s_sGameConstraintsAITeamsMinAttr = "aiTeamsMin";
static const std::string s_sGameConstraintsAITeamsMaxAttr = "aiTeamsMax";
static const std::string s_sGameConstraintsAllowAIHumanTeamAttr = "allowAIHumanTeam";
static const std::string s_sGameConstraintsAIMatesPerTeamMaxAttr = "aiMatesPerTeamMax";

static const std::string s_sGameConstraintsExprOrNode = "Or";
static const std::string s_sGameConstraintsExprAndNode = "And";
static const std::string s_sGameConstraintsExprTrueNode = "True";
static const std::string s_sGameConstraintsExprFalseNode = "False";
static const std::string s_sGameConstraintsExprOptionNode = "Option";
static const std::string s_sGameConstraintsExprOptionNameAttr = "name";

static const std::string s_sGameConstraintsExprOptionComplementAttr = "not";

static const std::string s_sGameHighscoresDefNodeName = "HighscoresDefinition";
static const std::string s_sGameHighscoresDefMaxScoresAttr = "maxScores";
	static const int32_t s_nGameHighscoresDefMaxScoresAttrDefault = 10;

static const std::string s_sGameHighscoresDefDiscriminatorNodeName = "Discriminator";
static const std::string s_sGameHighscoresDefDiscriminatorTypeAttr = "type";
	static const std::string s_sGameHighscoresDefDiscriminatorTypeAttrTotTeams = "TEAMS";
	static const std::string s_sGameHighscoresDefDiscriminatorTypeAttrTotHumanTeams = "HUMAN_TEAMS";
	static const std::string s_sGameHighscoresDefDiscriminatorTypeAttrTotAITeams = "AI_TEAMS";
	static const std::string s_sGameHighscoresDefDiscriminatorTypeAttrTotMatesHumanTeam = "HUMAN_TEAM_MATES";
	static const std::string s_sGameHighscoresDefDiscriminatorTypeAttrOption = "OPTION";
static const std::string s_sGameHighscoresDefDiscriminatorOptionNameAttr = "optionName";

static const std::string s_sGameHighscoresDefConstraintNodeName = "Constraint";
static const std::string s_sGameHighscoresDefConstraintTypeAttr = "type";
	static const std::string s_sGameHighscoresDefConstraintTypeAttrTotTeams = "TEAMS";
	static const std::string s_sGameHighscoresDefConstraintTypeAttrTotHumanTeams = "HUMAN_TEAMS";
	static const std::string s_sGameHighscoresDefConstraintTypeAttrTotAITeams = "AI_TEAMS";
	static const std::string s_sGameHighscoresDefConstraintTypeAttrTotMatesHumanTeam = "HUMAN_TEAM_MATES";
static const std::string s_sGameHighscoresDefConstraintValueAttr = "value";
static const std::string s_sGameHighscoresDefConstraintMinAttr = "min";
static const std::string s_sGameHighscoresDefConstraintMaxAttr = "max";
//
static const std::string s_sGameHighscoresDefEligibilityNodeName = "Eligibility";
static const std::string s_sGameHighscoresDefEligibilityVarNameAttr = "varName";
static const std::string s_sGameHighscoresDefEligibilityVarValueAttr = "value";
static const std::string s_sGameHighscoresDefEligibilityVarValueMinAttr = "min";
static const std::string s_sGameHighscoresDefEligibilityVarValueMaxAttr = "max";
// s_sGameHighscoresDefEligibilityVarOwnerTypeAttr
static const std::string s_sGameHighscoresDefElementNodeName = "Element";
static const std::string s_sGameHighscoresDefElementDescriptionAttr = "description";
static const std::string s_sGameHighscoresDefElementVarNameAttr = "varName";
static const std::string s_sGameHighscoresDefElementLowestBestAttr = "lowestBest";
	static const bool s_bGameHighscoresDefElementLowestBestAttrDefault = false;
// s_sGameHighscoresDefElementVarOwnerTypeAttr

static const std::string s_sGameOwnerTypeAttr = "owner";
	static const std::string s_sGameOwnerTypeAttrGame = "GAME";
	static const std::string s_sGameOwnerTypeAttrTeam = "TEAM";
	static const std::string s_sGameOwnerTypeAttrPlayer = "PLAYER";
	static const OwnerType s_eGameOwnerTypeAttrDefault = OwnerType::GAME;

static const std::string s_sGameVariablesNodeName = "Variables";
static const std::string s_sGameVariablesVarNodeName = "Var";
static const std::string s_sGameVariablesVarNameAttr = "name";
static const std::string s_sGameVariablesVarInitialAttr = "initial";
static const std::string s_sGameVariablesVarAddTimeAttr = "addTime";
static const std::string s_sGameVariablesVarSubtractTimeAttr = "subtractTime";
static const std::string s_sGameVariablesVarTimeBaseAttr = "timeBase";
	static const std::string s_sGameVariablesVarTimeBaseAttrMillisec = "MILLISEC";
	static const std::string s_sGameVariablesVarTimeBaseAttrSec = "SEC";
	static const std::string s_sGameVariablesVarTimeBaseAttrMin = "MIN";
static const std::string s_sGameVariablesVarTimeFormatAttr = "timeFormat";
	static const std::string s_sGameVariablesVarTimeFormatAttrNumber = "NUMBER";
	static const std::string s_sGameVariablesVarTimeFormatAttrMill = "MILL";
	static const std::string s_sGameVariablesVarTimeFormatAttrMinSec = "MIN_SEC";
	static const std::string s_sGameVariablesVarTimeFormatAttrMinSecMillisec = "MIN_SEC_MILLISEC";


GameLoader::GameInfo XmlGameInfoParser::parseGameInfo(const shared_ptr<AppConfig>& refAppConfig, const File& oGameFile, const xmlpp::Element* p0Element, std::string& sName)
{
	GameInfoCtx oCtx{refAppConfig, oGameFile};
	return parseGameInfo(oCtx, p0Element, sName);
}
const GameLoader::GameInfo& XmlGameInfoParser::parseGameInfo(GameInfoCtx& oCtx, const xmlpp::Element* p0RootElement, std::string& sName)
{
//std::cout << "XmlGameInfoParser::parseGameInfo" << '\n';
	oCtx.addChecker(p0RootElement);
	if (p0RootElement->get_name() != s_sGameNodeName) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0RootElement, s_sGameNodeName);
	}
	const auto oPairName = XmlCommonParser::getAttributeValue(oCtx, p0RootElement, s_sGameInternalNameAttr);
	sName = oPairName.second;
	if (sName.empty()) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0RootElement, s_sGameInternalNameAttr);
	}
	const auto oPairDifficulty = XmlCommonParser::getAttributeValue(oCtx, p0RootElement, s_sGameDifficultyAttr);
	if (oPairDifficulty.first) {
		oCtx.m_oGameInfo.m_nDifficulty = XmlUtil::strToNumber<int32_t>(oCtx, p0RootElement, s_sGameDifficultyAttr, oPairDifficulty.second, false
																				, false, -1, false, -1);
	}
	const auto oPairTesting = XmlCommonParser::getAttributeValue(oCtx, p0RootElement, s_sGameTestingAttr);
	if (oPairTesting.first) {
		oCtx.m_oGameInfo.m_bTesting = XmlUtil::strToBool(oCtx, p0RootElement, s_sGameTestingAttr, oPairTesting.second);
	}

	XmlCommonParser::visitNamedElementChildren(oCtx, p0RootElement, s_sGameDescriptionNodeName, [&](const xmlpp::Element* p0DescEl)
	{
		oCtx.addChecker(p0DescEl);
		const xmlpp::TextNode* p0DescText = p0DescEl->get_child_text();
		const auto sTextLine = ((p0DescText != nullptr) ? p0DescText->get_content() : "");
		if (!oCtx.m_oGameInfo.m_sGameDescription.empty()) {
			oCtx.m_oGameInfo.m_sGameDescription += "\n";
		}
		oCtx.m_oGameInfo.m_sGameDescription += sTextLine;
		oCtx.removeChecker(p0DescEl, true);
	});

	XmlCommonParser::visitNamedElementChildren(oCtx, p0RootElement, s_sGameAuthorNodeName, [&](const xmlpp::Element* p0AuthorEl)
	{
		oCtx.addChecker(p0AuthorEl);
		const auto oPairAuthorName = XmlCommonParser::getAttributeValue(oCtx, p0AuthorEl, s_sGameAuthorNameAttr);
		const auto oPairAuthorEmail = XmlCommonParser::getAttributeValue(oCtx, p0AuthorEl, s_sGameAuthorEmailAttr);
		const auto oPairAuthorWebSite = XmlCommonParser::getAttributeValue(oCtx, p0AuthorEl, s_sGameAuthorWebSiteAttr);
		const auto oPairAuthorRole = XmlCommonParser::getAttributeValue(oCtx, p0AuthorEl, s_sGameAuthorRoleAttr);
		if (oPairAuthorName.second.empty() && oPairAuthorEmail.second.empty()) {
			throw XmlCommonErrors::errorAttrEitherMustBeDefined(oCtx, p0AuthorEl, s_sGameAuthorNameAttr, s_sGameAuthorEmailAttr);
		}
		//Author{m_sName, m_sEmail, m_sRole}
		GameLoader::GameInfo::Author oAuthor;
		oAuthor.m_sName = oPairAuthorName.second;
		oAuthor.m_sEmail = oPairAuthorEmail.second;
		oAuthor.m_sWebSite = oPairAuthorWebSite.second;
		oAuthor.m_sRole = oPairAuthorRole.second;
		oCtx.m_oGameInfo.m_aAuthors.push_back(std::move(oAuthor));
		oCtx.removeChecker(p0AuthorEl, true);
	});

	oCtx.m_oGameInfo.m_oGameConstraints.initFromAppConstraints(oCtx.appConfig()->getAppConstraints());

	const xmlpp::Element* p0Constraints = XmlCommonParser::parseUniqueElement(oCtx, p0RootElement, s_sGameConstraintsNodeName, false);
	if (p0Constraints != nullptr) {
		parseGameConstraints(oCtx, p0Constraints, oCtx.m_oGameInfo.m_oGameConstraints);
	}
	if (! oCtx.m_oGameInfo.m_oGameConstraints.isValid()) {
		throw XmlCommonErrors::error(oCtx, ((p0Constraints != nullptr) ? p0Constraints : p0RootElement), Util::s_sEmptyString
																								, "Constraints not valid");
	}
	if (! oCtx.m_oGameInfo.m_oGameConstraints.isCompatibleWith(oCtx.appConfig()->getAppConstraints())) {
		throw XmlCommonErrors::error(oCtx, ((p0Constraints != nullptr) ? p0Constraints : p0RootElement), Util::s_sEmptyString
																		, "Constraints not compatible with application!!!");
	}
	const xmlpp::Element* p0Variables = XmlCommonParser::parseUniqueElement(oCtx, p0RootElement, s_sGameVariablesNodeName, false);
	if (p0Variables != nullptr) {
		parseVariables(oCtx, p0Variables);
	}
	const xmlpp::Element* p0HighscoresDef = XmlCommonParser::parseUniqueElement(oCtx, p0RootElement, s_sGameHighscoresDefNodeName, false);
	if (p0HighscoresDef != nullptr) {
		oCtx.m_oGameInfo.m_refHighscoresDefinition = parseHighscoresDef(oCtx, p0HighscoresDef);
	}
	oCtx.m_fMinGameInterval = -1.0;
	const auto oPairMinGameInterval = XmlCommonParser::getAttributeValue(oCtx, p0RootElement, s_sGameMinGameIntervalAttr);
	if (oPairMinGameInterval.first) {
		const std::string& sMinGameInterval = oPairMinGameInterval.second;
		oCtx.m_fMinGameInterval = XmlUtil::strToNumber<double>(oCtx, p0RootElement, s_sGameMinGameIntervalAttr, sMinGameInterval
																, false, true, 0.00001, false, -1.0);
	}
	oCtx.m_fInitialGameInterval = -1.0;
	const auto oPairInitialGameInterval = XmlCommonParser::getAttributeValue(oCtx, p0RootElement, s_sGameInitialGameIntervalAttr);
	if (oPairInitialGameInterval.first) {
		const std::string& sInitialGameInterval = oPairInitialGameInterval.second;
		oCtx.m_fInitialGameInterval = XmlUtil::strToNumber<double>(oCtx, p0RootElement, s_sGameInitialGameIntervalAttr, sInitialGameInterval
																	, false, true, 0.00001, false, -1.0);
	}
	oCtx.m_nMaxViewTicks = -1;
	const auto oPairMaxViewTicks = XmlCommonParser::getAttributeValue(oCtx, p0RootElement, s_sGameMaxViewTicksAttr);
	if (oPairMaxViewTicks.first) {
		const std::string& sMaxViewTicks = oPairMaxViewTicks.second;
		oCtx.m_nMaxViewTicks = XmlUtil::strToNumber<int32_t>(oCtx, p0RootElement, s_sGameMaxViewTicksAttr, sMaxViewTicks
															, false, true, 1, false, -1.0);
	}
	oCtx.m_nAdditionalHighscoresWait = -1;
	const auto oPairAdditionalHighscoresWait = XmlCommonParser::getAttributeValue(oCtx, p0RootElement, s_sGameAdditionalHighscoresWaitAttr);
	if (oPairAdditionalHighscoresWait.first) {
		const std::string& sAdditionalHighscoresWait = oPairAdditionalHighscoresWait.second;
		oCtx.m_nAdditionalHighscoresWait = XmlUtil::strToNumber<int32_t>(oCtx, p0RootElement, s_sGameAdditionalHighscoresWaitAttr, sAdditionalHighscoresWait
																		, false, true, 1, false, -1.0);
	}
	oCtx.m_fSoundScaleX = -1.0;
	const auto oPairSoundScaleX = XmlCommonParser::getAttributeValue(oCtx, p0RootElement, s_sGameSoundScaleXAttr);
	if (oPairSoundScaleX.first) {
		const std::string& sSoundScaleX = oPairSoundScaleX.second;
		oCtx.m_fSoundScaleX = XmlUtil::strToNumber<double>(oCtx, p0RootElement, s_sGameSoundScaleXAttr, sSoundScaleX
																	, false, true, 0.00001, false, -1.0);
	}
	oCtx.m_fSoundScaleY = -1.0;
	const auto oPairSoundScaleY = XmlCommonParser::getAttributeValue(oCtx, p0RootElement, s_sGameSoundScaleYAttr);
	if (oPairSoundScaleY.first) {
		const std::string& sSoundScaleY = oPairSoundScaleY.second;
		oCtx.m_fSoundScaleY = XmlUtil::strToNumber<double>(oCtx, p0RootElement, s_sGameSoundScaleYAttr, sSoundScaleY
																	, false, true, 0.00001, false, -1.0);
	}
	oCtx.m_fSoundScaleZ = -1.0;
	const auto oPairSoundScaleZ = XmlCommonParser::getAttributeValue(oCtx, p0RootElement, s_sGameSoundScaleZAttr);
	if (oPairSoundScaleZ.first) {
		const std::string& sSoundScaleZ = oPairSoundScaleZ.second;
		oCtx.m_fSoundScaleZ = XmlUtil::strToNumber<double>(oCtx, p0RootElement, s_sGameSoundScaleZAttr, sSoundScaleZ
																	, false, true, 0.00001, false, -1.0);
	}
	// no child element check since we only parse some elements of the game root element
	oCtx.removeChecker(p0RootElement, false, true);

	return oCtx.m_oGameInfo;
}
static bool parseComplement(ParserCtx& oCtx, const xmlpp::Element* p0Element)
{
	const auto oPairComplement = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sGameConstraintsExprOptionComplementAttr);
	const bool bComplementDefined = oPairComplement.first;
	bool bNot = false;
	if (bComplementDefined) {
		const std::string& sComplement = oPairComplement.second;
		bNot = XmlUtil::strToBool(oCtx, p0Element, s_sGameConstraintsExprOptionComplementAttr, sComplement);
	}
	return bNot;
}

std::pair<unique_ptr<PrefSelector::Operand>, bool> XmlGameInfoParser::parseConstraintsOption(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	oCtx.addChecker(p0Element);

	auto oPairOptionName = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sGameConstraintsExprOptionNameAttr);
	std::string& sOptionName = oPairOptionName.second;
	if (!oPairOptionName.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sGameConstraintsExprOptionNameAttr);
	}
	const auto& refOption = oCtx.appConfig()->getOption(OwnerType::GAME, sOptionName);
	if (! refOption) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sGameConstraintsExprOptionNameAttr
									, Util::stringCompose("Attribute '%1': game option with name '%2' not found", s_sGameConstraintsExprOptionNameAttr
										, sOptionName));
	}
	const bool bNot = parseComplement(oCtx, p0Element);
	auto oVariantSet = m_oXmlVariantSetParser.parseVariantSet(oCtx, p0Element, *refOption);
	auto refOptionCond = std::make_unique<PrefSelector::OptionCond>(bNot, std::move(sOptionName), std::move(oVariantSet));

	oCtx.removeChecker(p0Element, true);
	return std::make_pair(std::move(refOptionCond), false);
}

std::pair<unique_ptr<PrefSelector::Operand>, bool> XmlGameInfoParser::parseConstraintsTrue(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	oCtx.addChecker(p0Element);
	oCtx.removeChecker(p0Element, true);
	return std::make_pair(unique_ptr<PrefSelector::Operand>{}, true);
}
std::pair<unique_ptr<PrefSelector::Operand>, bool> XmlGameInfoParser::parseConstraintsFalse(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	oCtx.addChecker(p0Element);
	oCtx.removeChecker(p0Element, true);
	return std::make_pair(unique_ptr<PrefSelector::Operand>{}, false);
}
std::pair<unique_ptr<PrefSelector::Operand>, bool> XmlGameInfoParser::parseConstraintsTraverse(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	const std::string sConditionName = p0Element->get_name();
	if (sConditionName == s_sGameConstraintsExprOrNode) {
		return parseConstraintsOr(oCtx, p0Element, true); //--------------------
	} else if (sConditionName == s_sGameConstraintsExprAndNode) {
		return parseConstraintsAnd(oCtx, p0Element); //-------------------------
	} else if (sConditionName == s_sGameConstraintsExprOptionNode) {
		return parseConstraintsOption(oCtx, p0Element); //----------------------
	} else if (sConditionName == s_sGameConstraintsExprTrueNode) {
		return parseConstraintsTrue(oCtx, p0Element); //------------------------
	} else if (sConditionName == s_sGameConstraintsExprFalseNode) {
		return parseConstraintsFalse(oCtx, p0Element); //-----------------------
	} else {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString
									, Util::stringCompose("Unknown condition '%1'", sConditionName));
	}
}
std::pair<unique_ptr<PrefSelector::Operand>, bool> XmlGameInfoParser::parseConstraintsAnd(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	oCtx.addChecker(p0Element);

	std::vector<unique_ptr<PrefSelector::Operand>> aConds;
	bool bCondition = true;
	XmlCommonParser::visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0Condition)
	{
		auto oPairCond = parseConstraintsTraverse(oCtx, p0Condition);
		auto& refCond = oPairCond.first;
		if (refCond) {
			aConds.push_back(std::move(refCond));
		} else {
			const bool bCond = oPairCond.second;
			if (! bCond) {
				bCondition = false;
			}
		}
	});
	oCtx.removeChecker(p0Element, false, true);

	if (! bCondition) {
		return std::make_pair(unique_ptr<PrefSelector::Operand>{}, false); //---
	}
	if (aConds.empty()) {
		return std::make_pair(unique_ptr<PrefSelector::Operand>{}, true); //----
	}
	auto refAndCond = std::make_unique<PrefSelector::Operator>(PrefSelector::Operator::OP_TYPE_AND, std::move(aConds));

	return std::make_pair(std::move(refAndCond), false);
}
std::pair<unique_ptr<PrefSelector::Operand>, bool> XmlGameInfoParser::parseConstraintsOr(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, bool bCheckAttrs) const
{
	oCtx.addChecker(p0Element);

	std::vector<unique_ptr<PrefSelector::Operand>> aConds;
	bool bCondition = false;

	XmlCommonParser::visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0Condition)
	{
		auto oPairCond = parseConstraintsTraverse(oCtx, p0Condition);
		auto& refCond = oPairCond.first;
		if (refCond) {
			aConds.push_back(std::move(refCond));
		} else {
			const bool bCond = oPairCond.second;
			if (bCond) {
				bCondition = true;
			}
		}
	});
	oCtx.removeChecker(p0Element, false, bCheckAttrs);

	if (bCondition) {
		return std::make_pair(unique_ptr<PrefSelector::Operand>{}, true); //----
	}
	if (aConds.empty()) {
		return std::make_pair(unique_ptr<PrefSelector::Operand>{}, true); //----
	}
	auto refOrCond = std::make_unique<PrefSelector::Operator>(PrefSelector::Operator::OP_TYPE_OR, std::move(aConds));

	return std::make_pair(std::move(refOrCond), false);
}
void XmlGameInfoParser::parseGameConstraints(GameInfoCtx& oCtx, const xmlpp::Element* p0Element
											, GameConstraints& oGameConstraints)
{
	// Note: Only the AppConstraints members are used to limit the values
	oCtx.addChecker(p0Element);
	XmlCommonParser::parseAttrFromToClamp<int32_t>(oCtx, p0Element
									, s_sGameConstraintsTeamsAttr, s_sGameConstraintsTeamsMinAttr, s_sGameConstraintsTeamsMaxAttr
									, false
									, true, oGameConstraints.m_nTeamsMin, true, oGameConstraints.m_nTeamsMax
									, oGameConstraints.m_nTeamsMin, oGameConstraints.m_nTeamsMax);
	const bool bTeamsPerLevelDefined = XmlCommonParser::parseAttrFromToClamp<int32_t>(oCtx, p0Element
									, s_sGameConstraintsTeamsPerLevelAttr, s_sGameConstraintsTeamsPerLevelMinAttr, s_sGameConstraintsTeamsPerLevelMaxAttr
									, false
									, true, 1, true, oGameConstraints.m_nTeamsMax
									, oGameConstraints.m_nTeamsPerLevelMin, oGameConstraints.m_nTeamsPerLevelMax);
	bool bATIOLDefined = false;
	bool bATIOL = false;
	if (bTeamsPerLevelDefined) {
		bATIOLDefined = (oGameConstraints.m_nTeamsPerLevelMax > 1);
		bATIOL = bATIOLDefined;
	}
	const bool bLevelsDefined = XmlCommonParser::parseAttrFromToClamp<int32_t>(oCtx, p0Element
									, s_sGameConstraintsLevelsAttr, s_sGameConstraintsLevelsMinAttr, s_sGameConstraintsLevelsMaxAttr
									, false
									, true, (bATIOLDefined ? 1 : oGameConstraints.m_nTeamsMin), true, (bATIOLDefined ? 1 : oGameConstraints.m_nTeamsMax)
									, oGameConstraints.m_nLevelsMin, oGameConstraints.m_nLevelsMax);
	if (bLevelsDefined) {
		if (! bATIOLDefined) {
			bATIOLDefined = (oGameConstraints.m_nLevelsMax > 1);
			bATIOL = bATIOLDefined;
		}
	}
	if (oGameConstraints.m_nTeamsMax == 1) {
		// 1 team games considered "All teams in one level"
		bATIOLDefined = true;
		bATIOL = true;
	}
	if (! bATIOLDefined) {
		//assert(oGameConstraints.m_nTeamsMax > 1);
		if (bTeamsPerLevelDefined && bLevelsDefined) {
			// 1 team games considered "All teams in one level"
			//bATIOLDefined = true;
			bATIOL = true;
		} else if (bTeamsPerLevelDefined) {
			//assert(oGameConstraints.m_nTeamsPerLevelMax == 1);
			//bATIOLDefined = true;
			bATIOL = false;
		} else if (bLevelsDefined) {
			//assert(oGameConstraints.m_nLevelsMax == 1);
			//bATIOLDefined = true;
			bATIOL = true;
		} else {
			throw XmlCommonErrors::errorAttrEitherMustBeDefined(oCtx, p0Element, s_sGameConstraintsTeamsPerLevelAttr, s_sGameConstraintsLevelsAttr);
		}
	} else {
		if (bTeamsPerLevelDefined && bLevelsDefined) {
			if ((oGameConstraints.m_nLevelsMax > 1) && (oGameConstraints.m_nTeamsPerLevelMax > 1)) {
				throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, "Attributes '" + s_sGameConstraintsTeamsPerLevelAttr
											+ "' and '" + s_sGameConstraintsLevelsAttr + "' cannot be both > 1");
			}
		} else if (bTeamsPerLevelDefined) {
			oGameConstraints.m_nLevelsMin = 1;
			oGameConstraints.m_nLevelsMax = 1;
		} else if (bLevelsDefined) {
			oGameConstraints.m_nTeamsPerLevelMin = 1;
			oGameConstraints.m_nTeamsPerLevelMax = 1;
		}
	}
	XmlCommonParser::parseAttrFromToClamp<int32_t>(oCtx, p0Element
											, s_sGameConstraintsMatesPerTeamAttr, s_sGameConstraintsMatesPerTeamMinAttr, s_sGameConstraintsMatesPerTeamMaxAttr
											, false
											, true, 1, true, oGameConstraints.m_nMatesPerTeamMax
											, oGameConstraints.m_nMatesPerTeamMin, oGameConstraints.m_nMatesPerTeamMax);
	XmlCommonParser::parseAttrFromToClamp<int32_t>(oCtx, p0Element
											, s_sGameConstraintsPlayersAttr, s_sGameConstraintsPlayersMinAttr, s_sGameConstraintsPlayersMaxAttr
											, false
											, true, (bATIOL ? 1 : oGameConstraints.m_nTeamsMin), true, oGameConstraints.getMaxPlayers()
											, oGameConstraints.m_nPlayersMin, oGameConstraints.m_nPlayersMax);
	const auto oPairAIMatesPerTeamMax = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sGameConstraintsAIMatesPerTeamMaxAttr);
	if (oPairAIMatesPerTeamMax.first) {
		oGameConstraints.m_nAIMatesPerTeamMax = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sGameConstraintsAIMatesPerTeamMaxAttr, oPairAIMatesPerTeamMax.second, false
												, true, 0, true, std::min(oGameConstraints.m_nAIMatesPerTeamMax, oGameConstraints.m_nMatesPerTeamMax));
	}
	const bool bAllowAI = (oGameConstraints.m_nAIMatesPerTeamMax > 0);
	XmlCommonParser::parseAttrFromToClamp<int32_t>(oCtx, p0Element
											, s_sGameConstraintsAITeamsAttr, s_sGameConstraintsAITeamsMinAttr, s_sGameConstraintsAITeamsMaxAttr
											, false
											, true, 0, true, (bAllowAI ? std::max(oGameConstraints.m_nTeamsMax, oGameConstraints.m_nLevelsMax) : 0)
											, oGameConstraints.m_nAITeamsMin, oGameConstraints.m_nAITeamsMax);
	oGameConstraints.m_bAllowMixedAIHumanTeam = bAllowAI;
	const auto oPairAllowAIHumanTeam = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sGameConstraintsAllowAIHumanTeamAttr);
	if (oPairAllowAIHumanTeam.first) {
		oGameConstraints.m_bAllowMixedAIHumanTeam = XmlUtil::strToBool(oCtx, p0Element, s_sGameConstraintsAllowAIHumanTeamAttr, oPairAllowAIHumanTeam.second);
	}
	if (oGameConstraints.m_bAllowMixedAIHumanTeam && !bAllowAI) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sGameConstraintsAllowAIHumanTeamAttr, Util::stringCompose("Attribute %1 conflicts with attribute %2"
																			, s_sGameConstraintsAllowAIHumanTeamAttr, s_sGameConstraintsAIMatesPerTeamMaxAttr));
	}
	//
	auto oPairConstraints = parseConstraintsOr(oCtx, p0Element, false);
	auto& refCond = oPairConstraints.first;
	if (refCond) {
		oGameConstraints.m_refPrefSelector = std::make_unique<PrefSelector>(std::move(refCond));
	} else {
		const bool bCond = oPairConstraints.second;
		if (! bCond) {
			throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, "Constraints condition is always false");
		}
	}
	//
	if (!oGameConstraints.isValid()) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("Invalid constraints"));
	}
	oCtx.removeChecker(p0Element, false, true);
}
OwnerType XmlGameInfoParser::parseOwnerAttr(ParserCtx& oCtx, const xmlpp::Element* p0Element)
{
	const auto oPairType = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sGameOwnerTypeAttr);
	if (!oPairType.first) {
		return s_eGameOwnerTypeAttrDefault; //----------------------------------
	}
	const Glib::ustring sType = Glib::ustring{oPairType.second}.uppercase();
	if (sType.empty()) {
		return s_eGameOwnerTypeAttrDefault;
	}
	if (sType == s_sGameOwnerTypeAttrGame) {
		return OwnerType::GAME;
	} else if (sType == s_sGameOwnerTypeAttrTeam) {
		return OwnerType::TEAM;
	} else if (sType == s_sGameOwnerTypeAttrPlayer) {
		return OwnerType::PLAYER;
	}
	throw XmlCommonErrors::errorAttrInvalidValue(oCtx, p0Element, s_sGameOwnerTypeAttr);
}
void XmlGameInfoParser::parseVariables(GameInfoCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	XmlCommonParser::visitNamedElementChildren(oCtx, p0Element, s_sGameVariablesVarNodeName, [&](const xmlpp::Element* p0VarElement)
	{
		oCtx.addChecker(p0VarElement);
		OwnerType eOwner = parseOwnerAttr(oCtx, p0VarElement);
		if (eOwner == OwnerType::GAME) {
			parseVariablesVar(oCtx, p0VarElement, oCtx.m_oGameVariableTypes);
		} else if (eOwner == OwnerType::TEAM) {
			parseVariablesVar(oCtx, p0VarElement, oCtx.m_oTeamVariableTypes);
		} else if (eOwner == OwnerType::PLAYER) {
			parseVariablesVar(oCtx, p0VarElement, oCtx.m_oMateVariableTypes);
		}
		oCtx.removeChecker(p0VarElement, true);
	});
	oCtx.removeChecker(p0Element, true);
}
void XmlGameInfoParser::parseVariablesVar(GameInfoCtx& oCtx, const xmlpp::Element* p0Element
									, NamedObjIndex<Variable::VariableType>& oVariableTypes)
{
	oCtx.addChecker(p0Element);

	Variable::VariableType oVariableType;

	const auto oPairName = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sGameVariablesVarNameAttr);
	if (!oPairName.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sGameVariablesVarNameAttr);
	}
	std::string sName = Util::strStrip(oPairName.second);
	if (sName.empty()) {
		throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, s_sGameVariablesVarNameAttr);
	}
	const int32_t nIdxGame = oCtx.m_oGameVariableTypes.getIndex(sName);
	const int32_t nIdxTeam = oCtx.m_oTeamVariableTypes.getIndex(sName);
	const int32_t nIdxPlayer = oCtx.m_oMateVariableTypes.getIndex(sName);
	if ((nIdxGame >= 0) || (nIdxTeam >= 0) || (nIdxPlayer >= 0)) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sGameVariablesVarNameAttr, Util::stringCompose(
																"Variable with name '%1' already defined", sName));
	}

	const auto oPairInitial = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sGameVariablesVarInitialAttr);
	if (oPairInitial.first) {
		const std::string& sInitial = oPairInitial.second;
		oVariableType.m_nInitialValue = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sGameVariablesVarInitialAttr
																			, sInitial, false, false, -1, false, -1);
	}
	const auto oPairAddTime = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sGameVariablesVarAddTimeAttr);
	bool bAddTime = false;
	if (oPairAddTime.first) {
		const std::string& sAddTime = oPairAddTime.second;
		bAddTime = XmlUtil::strToBool(oCtx, p0Element, s_sGameVariablesVarAddTimeAttr, sAddTime);
	}
	const auto oPairSubTime = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sGameVariablesVarSubtractTimeAttr);
	bool bSubTime = false;
	if (oPairSubTime.first) {
		const std::string& sSubTime = oPairSubTime.second;
		bSubTime = XmlUtil::strToBool(oCtx, p0Element, s_sGameVariablesVarSubtractTimeAttr, sSubTime);
	}
//std::cout << "XmlGameInfoParser::parseVariablesVar bAddTime=" << bAddTime << " bSubTime=" << bSubTime << '\n';
	if (bAddTime && bSubTime) {
		bAddTime = false;
		bSubTime = false;
	}
	oVariableType.m_bAddTime = bAddTime;
	oVariableType.m_bTimeRelative = (bAddTime || bSubTime);

	const auto oPairTimeBase = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sGameVariablesVarTimeBaseAttr);
	if (oPairTimeBase.first) {
		const std::string& sTimeBase = oPairTimeBase.second;
		oVariableType.m_eTimeBase = [&]() {
			if (sTimeBase == s_sGameVariablesVarTimeBaseAttrMillisec) {
				return Variable::VARIABLE_TIME_BASE_MILLISEC;
			} else if (sTimeBase == s_sGameVariablesVarTimeBaseAttrSec) {
				return Variable::VARIABLE_TIME_BASE_SEC;
			} else if (sTimeBase == s_sGameVariablesVarTimeBaseAttrMin) {
				return Variable::VARIABLE_TIME_BASE_MIN;
			} else {
				throw XmlCommonErrors::errorAttrInvalidValue(oCtx, p0Element, s_sGameVariablesVarTimeBaseAttr);
			}
		}();
	}
	const auto oPairTimeFormat = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sGameVariablesVarTimeFormatAttr);
	if (oPairTimeFormat.first) {
		const std::string& sTimeFormat = oPairTimeFormat.second;
		oVariableType.m_eFormat = [&]() {
			if (sTimeFormat == s_sGameVariablesVarTimeFormatAttrNumber) {
				return Variable::VARIABLE_FORMAT_NUMBER;
			} else if (sTimeFormat == s_sGameVariablesVarTimeFormatAttrMill) {
				return Variable::VARIABLE_FORMAT_MILL;
			} else if (sTimeFormat == s_sGameVariablesVarTimeFormatAttrMinSec) {
				return Variable::VARIABLE_FORMAT_MIN_SEC;
			} else if (sTimeFormat == s_sGameVariablesVarTimeFormatAttrMinSecMillisec) {
				return Variable::VARIABLE_FORMAT_MIN_SEC_MILLISEC;
			} else {
				throw XmlCommonErrors::errorAttrInvalidValue(oCtx, p0Element, s_sGameVariablesVarTimeFormatAttr);
			}
		}();
	}

	#ifndef NDEBUG
	const int32_t nIdx2 = 
	#endif
	oVariableTypes.addNamedObj(sName, oVariableType);
	assert(nIdx2 >= 0);
	oCtx.removeChecker(p0Element, true);
}
shared_ptr<HighscoresDefinition> XmlGameInfoParser::parseHighscoresDef(GameInfoCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	std::vector<HighscoresDefinition::Discriminator> aDiscriminators;
	XmlCommonParser::visitNamedElementChildren(oCtx, p0Element, s_sGameHighscoresDefDiscriminatorNodeName, [&](const xmlpp::Element* p0ChildElement)
	{
		oCtx.addChecker(p0ChildElement);
		HighscoresDefinition::Discriminator oDiscriminator;
		const auto oPairType = XmlCommonParser::getAttributeValue(oCtx, p0ChildElement, s_sGameHighscoresDefDiscriminatorTypeAttr);
		if (!oPairType.first) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0ChildElement, s_sGameHighscoresDefDiscriminatorTypeAttr);
		}
		Glib::ustring sType = Glib::ustring{oPairType.second}.uppercase();
		if (sType == s_sGameHighscoresDefDiscriminatorTypeAttrTotTeams) {
			oDiscriminator.m_eType = HighscoresDefinition::DISCRIMINATOR_TYPE_TOT_TEAMS;
		} else if (sType == s_sGameHighscoresDefDiscriminatorTypeAttrTotHumanTeams) {
			oDiscriminator.m_eType = HighscoresDefinition::DISCRIMINATOR_TYPE_TOT_HUMAN_TEAMS;
		} else if (sType == s_sGameHighscoresDefDiscriminatorTypeAttrTotAITeams) {
			oDiscriminator.m_eType = HighscoresDefinition::DISCRIMINATOR_TYPE_TOT_AI_TEAMS;
		} else if (sType == s_sGameHighscoresDefDiscriminatorTypeAttrTotMatesHumanTeam) {
			oDiscriminator.m_eType = HighscoresDefinition::DISCRIMINATOR_TYPE_TOT_MATES_OF_HUMAN_TEAM;
		} else if (sType == s_sGameHighscoresDefDiscriminatorTypeAttrOption) {
			oDiscriminator.m_eType = HighscoresDefinition::DISCRIMINATOR_TYPE_OPTION;
			oDiscriminator.m_eOptionOwnerType = parseOwnerAttr(oCtx, p0ChildElement);
			const auto oPairOptionName = XmlCommonParser::getAttributeValue(oCtx, p0ChildElement, s_sGameHighscoresDefDiscriminatorOptionNameAttr);
			if (!oPairOptionName.first) {
				throw XmlCommonErrors::errorAttrNotFound(oCtx, p0ChildElement, s_sGameHighscoresDefDiscriminatorOptionNameAttr);
			}
			oDiscriminator.m_sOptionName = oPairOptionName.second;
			const auto& refOption = oCtx.appConfig()->getOption(oDiscriminator.m_eOptionOwnerType, oDiscriminator.m_sOptionName);
			if (!refOption) {
				throw XmlCommonErrors::error(oCtx, p0ChildElement, s_sGameHighscoresDefDiscriminatorOptionNameAttr
											, Util::stringCompose("attribute '%1' has unknown option name for %2"
													, s_sGameHighscoresDefDiscriminatorOptionNameAttr
													, oDiscriminator.m_sOptionName));
			}
		} else {
			throw XmlCommonErrors::errorAttrInvalidValue(oCtx, p0ChildElement, s_sGameHighscoresDefDiscriminatorTypeAttr);
		}
		aDiscriminators.push_back(oDiscriminator);
		oCtx.removeChecker(p0ChildElement, true);
	});

	std::vector<HighscoresDefinition::HighscoreConstraint> aHDConstraints;
	XmlCommonParser::visitNamedElementChildren(oCtx, p0Element, s_sGameHighscoresDefConstraintNodeName, [&](const xmlpp::Element* p0ChildElement)
	{
		oCtx.addChecker(p0ChildElement);
		HighscoresDefinition::HighscoreConstraint oConstraint;
		const auto oPairType = XmlCommonParser::getAttributeValue(oCtx, p0ChildElement, s_sGameHighscoresDefConstraintTypeAttr);
		if (!oPairType.first) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0ChildElement, s_sGameHighscoresDefConstraintTypeAttr);
		}
		Glib::ustring sType = Glib::ustring{oPairType.second}.uppercase();
		if (sType == s_sGameHighscoresDefConstraintTypeAttrTotTeams) {
			oConstraint.m_eType = HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_TEAMS;
		} else if (sType == s_sGameHighscoresDefConstraintTypeAttrTotHumanTeams) {
			oConstraint.m_eType = HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_HUMAN_TEAMS;
		} else if (sType == s_sGameHighscoresDefConstraintTypeAttrTotAITeams) {
			oConstraint.m_eType = HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_AI_TEAMS;
		} else if (sType == s_sGameHighscoresDefConstraintTypeAttrTotMatesHumanTeam) {
			oConstraint.m_eType = HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_MATES_OF_HUMAN_TEAM;
		} else {
			throw XmlCommonErrors::errorAttrInvalidValue(oCtx, p0ChildElement, s_sGameHighscoresDefDiscriminatorTypeAttr);
		}
		XmlCommonParser::parseAttrFromTo<int32_t>(oCtx, p0ChildElement, s_sGameHighscoresDefConstraintValueAttr
												, s_sGameHighscoresDefConstraintMinAttr, s_sGameHighscoresDefConstraintMaxAttr
												, true, true, 0, false, -1, oConstraint.m_nMin, oConstraint.m_nMax);
		aHDConstraints.push_back(oConstraint);
		oCtx.removeChecker(p0ChildElement, true);
	});

	std::vector<HighscoresDefinition::Eligibility> aEligibilitys;
	XmlCommonParser::visitNamedElementChildren(oCtx, p0Element, s_sGameHighscoresDefEligibilityNodeName, [&](const xmlpp::Element* p0ChildElement)
	{
		oCtx.addChecker(p0ChildElement);
		HighscoresDefinition::Eligibility oEligibility;
		const auto oPairVarName = XmlCommonParser::getAttributeValue(oCtx, p0ChildElement, s_sGameHighscoresDefEligibilityVarNameAttr);
		if (!oPairVarName.first) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0ChildElement, s_sGameHighscoresDefEligibilityVarNameAttr);
		}
		oEligibility.m_sVarName = oPairVarName.second;
		checkNonSystemVariableDefined(oCtx, p0ChildElement, s_sGameHighscoresDefEligibilityVarNameAttr, oEligibility.m_sVarName);

		XmlCommonParser::parseAttrFromTo<int32_t>(oCtx, p0ChildElement
												, s_sGameHighscoresDefEligibilityVarValueAttr
												, s_sGameHighscoresDefEligibilityVarValueMinAttr, s_sGameHighscoresDefEligibilityVarValueMaxAttr
												, true, false, -1, false, -1, oEligibility.m_nMin, oEligibility.m_nMax);
		aEligibilitys.push_back(oEligibility);
		oCtx.removeChecker(p0ChildElement, true);
	});

	std::vector<HighscoresDefinition::ScoreElement> aScoreElements;
	XmlCommonParser::visitNamedElementChildren(oCtx, p0Element, s_sGameHighscoresDefElementNodeName, [&](const xmlpp::Element* p0ChildElement)
	{
		oCtx.addChecker(p0ChildElement);
		HighscoresDefinition::ScoreElement oScoreElement;
		const auto oPairVarName = XmlCommonParser::getAttributeValue(oCtx, p0ChildElement, s_sGameHighscoresDefElementVarNameAttr);
		if (!oPairVarName.first) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0ChildElement, s_sGameHighscoresDefElementVarNameAttr);
		}
		oScoreElement.m_sVarName = oPairVarName.second;
		checkNonSystemVariableDefined(oCtx, p0ChildElement, s_sGameHighscoresDefElementVarNameAttr, oScoreElement.m_sVarName);
		//
		const auto oPairDescription = XmlCommonParser::getAttributeValue(oCtx, p0ChildElement, s_sGameHighscoresDefElementDescriptionAttr);
		oScoreElement.m_sVarDescription = oPairDescription.second;
		const auto oPairLowestBest = XmlCommonParser::getAttributeValue(oCtx, p0ChildElement, s_sGameHighscoresDefElementLowestBestAttr);
		if (oPairLowestBest.first) {
			const std::string& sLowestBest = oPairLowestBest.second;
			oScoreElement.m_bLowestBest = XmlUtil::strToBool(oCtx, p0ChildElement, s_sGameHighscoresDefElementLowestBestAttr, sLowestBest);
		} else {
			oScoreElement.m_bLowestBest = s_bGameHighscoresDefElementLowestBestAttrDefault;
		}
		aScoreElements.push_back(oScoreElement);
		oCtx.removeChecker(p0ChildElement, true);
	});
	int32_t nMaxScores = s_nGameHighscoresDefMaxScoresAttrDefault;
	const auto oPairMaxScores = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sGameHighscoresDefMaxScoresAttr);
	if (oPairMaxScores.first) {
		const std::string& sMaxScores = oPairMaxScores.second;
		nMaxScores = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sGameHighscoresDefMaxScoresAttr, sMaxScores, false
															, true, 1, false, -1);
	}
	oCtx.removeChecker(p0Element, true);
	return std::make_shared<HighscoresDefinition>(oCtx.appConfig(), aHDConstraints, aDiscriminators, aEligibilitys, aScoreElements, nMaxScores);
}
void XmlGameInfoParser::checkNonSystemVariableDefined(GameInfoCtx& oCtx, const xmlpp::Element* p0Element
													, const std::string& sAttr, const std::string& sVarName)
{
//std::cout << "XmlGameInfoParser::checkNonSystemVariableDefined  sAttr=" << sAttr << "  sVarName=" << sVarName << '\n';
	if (sVarName.substr(0, 2) == "__") {
		// system: only added when full game instance is created
		return;
	}
	const int32_t nIdxGame = oCtx.m_oGameVariableTypes.getIndex(sVarName);
	if (nIdxGame < 0) {
		const int32_t nIdxTeam = oCtx.m_oTeamVariableTypes.getIndex(sVarName);
		if (nIdxTeam < 0) {
			const int32_t nIdxPlayer = oCtx.m_oMateVariableTypes.getIndex(sVarName);
			if (nIdxPlayer < 0) {
				throw XmlCommonErrors::errorAttrVariableNotDefined(oCtx, p0Element, sAttr, sVarName);
			}
		}
	}
}

} // namespace stmg
