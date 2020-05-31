/*
 * File:   xmlconditionalparser.cc
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

#include "xmlconditionalparser.h"
#include "xmlutil/xmlvariantsetparser.h"
#include "xmlutil/xmlstrconv.h"

#include <stmm-games/appconfig.h>
#include <stmm-games/util/util.h>
#include <stmm-games/ownertype.h>
#include <stmm-games/apppreferences.h>
#include <stmm-games/util/intset.h>
#include <stmm-games/util/variant.h>
#include <stmm-games/util/variantset.h>

#include <vector>
#include <cassert>
//#include <iostream>
#include <cstdint>
#include <memory>
#include <type_traits>

namespace stmg { class ParserCtx; }


namespace stmg
{

const std::string XmlConditionalParser::s_sConditionalExistIfNode = "ExistIf";
const std::string XmlConditionalParser::s_sConditionalAttrIfNode = "AttrIf";

const std::string XmlConditionalParser::s_sConditionalAttrIfUndefAttr = "_undef_";
const std::string XmlConditionalParser::s_sConditionalAttrIfUndefAttrSeparator = ",";

const std::string XmlConditionalParser::s_sConditionalOwnerTeamAttr = "team";
const std::string XmlConditionalParser::s_sConditionalOwnerMateAttr = "mate";
const std::string XmlConditionalParser::s_sConditionalOwnerPlayerAttr = "player";
	//static const std::string s_sConditionalOwnerLevelAttr = "level";
	//static const std::string s_sConditionalOwnerLevelTeamAttr = "levelTeam";
	//static const std::string s_sConditionalOwnerLevelPlayerAttr = "levelPlayer";
static const std::string s_sConditionalOwnerTypeAttr = "ownerType";
	static const std::string s_sConditionalOwnerTypeAttrGame = "GAME";
	static const std::string s_sConditionalOwnerTypeAttrTeam = "TEAM";
	static const std::string s_sConditionalOwnerTypeAttrPlayer = "PLAYER";

static const std::string s_sConditionalConditionValueAttr = "value";
static const std::string s_sConditionalConditionFromAttr = "from";
static const std::string s_sConditionalConditionToAttr = "to";
static const std::string s_sConditionalConditionValuesAttr = "values";
static const std::string s_sConditionalConditionComplementAttr = "not";

static const std::string s_sConditionalConditionOrNode = "Or";
static const std::string s_sConditionalConditionAndNode = "And";
static const std::string s_sConditionalConditionTrueNode = "True";
static const std::string s_sConditionalConditionFalseNode = "False";
static const std::string s_sConditionalConditionTotTeamsNode = "Teams";
static const std::string s_sConditionalConditionTotHumanTeamsNode = "HumanTeams";
static const std::string s_sConditionalConditionTotAITeamsNode = "AITeams";
static const std::string s_sConditionalConditionTotPlayersNode = "Players";
static const std::string s_sConditionalConditionTotHumanPlayersNode = "HumanPlayers";
static const std::string s_sConditionalConditionTotAIPlayersNode = "AIPlayers";
static const std::string s_sConditionalConditionTotMatesNode = "Mates";
static const std::string s_sConditionalConditionOptionNode = "Option";
static const std::string s_sConditionalConditionOptionNameAttr = "name";
	// For <Events/>
	//static const std::string s_sConditionalConditionLevelNode = "Level";

static bool xOr(bool b1, bool b2) { return (b1 && !b2) || (b2 && !b1); }

XmlConditionalParser::XmlConditionalParser(XmlVariantSetParser& oXmlVariantSetParser)
: m_oXmlVariantSetParser(oXmlVariantSetParser)
{
}
std::runtime_error XmlConditionalParser::errorElementCannotDetermineTeamFromContext(ParserCtx& oCtx, const xmlpp::Element* p0Element)
{
	return XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, "Cannot determine team from context");
}
std::runtime_error XmlConditionalParser::errorElementCannotDeterminePlayerFromContext(ParserCtx& oCtx, const xmlpp::Element* p0Element)
{
	return XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, "Cannot determine player from context");
}
bool XmlConditionalParser::parseIntSetAttributes(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
												, const std::string& sMultiAttr, const std::string& sSeparatorAttr
												, IntSet& oIntSet
												, const std::string& sAlreadyAttr, const std::string& sAlreadyAttr2
												, bool bMin, int32_t nMin, bool bMax, int32_t nMax) const
{
	//
	const std::string sSeparator = XmlCommonParser::parseSeparator(oCtx, p0Element, sSeparatorAttr);
	const auto oInts = getAttributeValue(oCtx, p0Element, sMultiAttr);
	if (! oInts.first) {
		return false; //--------------------------------------------------------
	}
	if (!sAlreadyAttr.empty()) {
		if (sAlreadyAttr2.empty()) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sMultiAttr, sAlreadyAttr);
		} else {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByOthers(oCtx, p0Element, sMultiAttr, sAlreadyAttr, sAlreadyAttr2);
		}
	}

	std::vector<int32_t> aValues;
	const std::string& sInts = oInts.second;
	XmlUtil::tokenizer(sInts, sSeparator, [&](const std::string& sToken)
	{
		const int32_t nValue = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, sMultiAttr, sToken, false
																	, bMin, nMin, bMax, nMax);
		aValues.push_back(nValue);
	});
	oIntSet = IntSet{aValues};
	return true;
}

std::pair<bool, IntSet> XmlConditionalParser::parseIntSetAttributes(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
																	, const std::string& sSingleAttr, const std::string& sFromAttr, const std::string& sToAttr
																	, const std::string& sMultiAttr, const std::string& sSeparatorAttr
																	, bool bMin, int32_t nMin, bool bMax, int32_t nMax) const
{
	IntSet oIntSet;

	int32_t nDefinedAttr = -1;
	const auto oSingleValue = getAttributeValue(oCtx, p0Element, sSingleAttr);
	const bool bSingleDefined = (oSingleValue.first && (sSingleAttr != sMultiAttr));
	if (bSingleDefined) {
		const int32_t nValue = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, sSingleAttr, oSingleValue.second, false
																	, bMin, nMin, bMax, nMax);
		oIntSet = IntSet{nValue};
		nDefinedAttr = 0;
	}
	int32_t nFromInt = 0;
	int32_t nToInt = 0;
	const auto oFromValue = getAttributeValue(oCtx, p0Element, sFromAttr);
	const bool bFromDefined = oFromValue.first;
	if (bFromDefined) {
		if (bSingleDefined) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sFromAttr, sSingleAttr);
		}
		nFromInt = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, sFromAttr, oFromValue.second, false
														, bMin, nMin, bMax, nMax);
		nDefinedAttr = 1;
	}
	const auto oToValue = getAttributeValue(oCtx, p0Element, sToAttr);
	const bool bToDefined = oToValue.first;
	if (bToDefined) {
		if (bSingleDefined) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sToAttr, sSingleAttr);
		}
		nToInt = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, sToAttr, oToValue.second, false
														, bMin, nMin, bMax, nMax);
		if (!bFromDefined) {
			nFromInt = (bMin ? nMin : std::numeric_limits<int32_t>::min());
		}
		nDefinedAttr = 1;
	} else {
		if (bFromDefined) {
			nToInt = (bMax ? nMax : std::numeric_limits<int32_t>::max());
		}
	}
	if (nDefinedAttr == 1) {
		oIntSet = IntSet{nFromInt, nToInt};
	}
	const bool bMultiDefined = parseIntSetAttributes(oCtx, p0Element, sMultiAttr, sSeparatorAttr, oIntSet
								, ((nDefinedAttr < 0) ? "" : ((nDefinedAttr == 0) ? sSingleAttr : sFromAttr))
								, ((nDefinedAttr <= 0) ? "" : sToAttr)
								, bMin, nMin, bMax, nMax);
	return std::make_pair(((nDefinedAttr >= 0) || bMultiDefined), oIntSet);
}

static bool parseComplement(ParserCtx& oCtx, const xmlpp::Element* p0Element)
{
	const auto oPairComplement = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sConditionalConditionComplementAttr);
	const bool bComplementDefined = oPairComplement.first;
	bool bNot = false;
	if (bComplementDefined) {
		const std::string& sComplement = oPairComplement.second;
		bNot = XmlUtil::strToBool(oCtx, p0Element, s_sConditionalConditionComplementAttr, sComplement);
	}
	return bNot;
}

bool XmlConditionalParser::evalConditionOption(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	oCtx.addChecker(p0Element);
	auto p0AppPreferences = oCtx.appPreferences().get();
	// example <Option name="AI" team="1" value="true"/>
	const auto oTuple = parseOwnerExists(oCtx, p0Element);
	const bool bOwnerExists = std::get<0>(oTuple);
	if (! bOwnerExists) {
		oCtx.removeChecker(p0Element, false);
		return false; //--------------------------------------------------------
	}
	const int32_t nTeam = std::get<1>(oTuple);
	const int32_t nMate = std::get<2>(oTuple);
	const auto oPairOptionName = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sConditionalConditionOptionNameAttr);
	const std::string& sOptionName = oPairOptionName.second;
	if (!oPairOptionName.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sConditionalConditionOptionNameAttr);
	}
	const auto oPairOwnerType = parseOwnerType(oCtx, p0Element, Util::s_sEmptyString);
	OwnerType eOwnerType;
	if (oPairOwnerType.first) {
		eOwnerType = oPairOwnerType.second;
		if (eOwnerType == OwnerType::TEAM) {
			if (nTeam < 0) {
				throw errorElementCannotDetermineTeamFromContext(oCtx, p0Element);
			}
		} else if (eOwnerType == OwnerType::PLAYER) {
			if (nMate < 0) {
				throw errorElementCannotDeterminePlayerFromContext(oCtx, p0Element);
			}
		}
	} else {
		if (nTeam < 0) {
			eOwnerType = OwnerType::GAME;
		} else {
			if (nMate < 0) {
				eOwnerType = OwnerType::TEAM;
			} else {
				eOwnerType = OwnerType::PLAYER;
			}
		}
	}
	const auto& refOption = p0AppPreferences->getAppConfig()->getOption(eOwnerType, sOptionName);
	Variant oValue;
	if (!refOption) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sConditionalConditionOptionNameAttr
									, Util::stringCompose("Attribute '%1': %3 option with name '%2' not found", s_sConditionalConditionOptionNameAttr
										, sOptionName
										, ( (eOwnerType == OwnerType::GAME) ? "game" : ( (eOwnerType == OwnerType::TEAM) ? "team" : "player" ) ) ));
	}
	if (eOwnerType == OwnerType::GAME) {
		oValue = p0AppPreferences->getOptionValue(sOptionName);
	} else {
		auto refTeam = p0AppPreferences->getTeam(nTeam);
		if (eOwnerType == OwnerType::TEAM) {
			oValue = refTeam->getOptionValue(sOptionName);
		} else { //if (eOwnerType == OwnerType::PLAYER) {
			oValue = refTeam->getMate(nMate)->getOptionValue(sOptionName);
		}
	}
	const bool bNot = parseComplement(oCtx, p0Element);
	const VariantSet oValidValues = m_oXmlVariantSetParser.parseVariantSet(oCtx, p0Element, *refOption);
	const bool bCondition = xOr(bNot, oValidValues.contains(oValue));
	oCtx.removeChecker(p0Element, true);
	return bCondition;
}
bool XmlConditionalParser::evalConditionTotMates(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	auto p0AppPreferences = oCtx.appPreferences().get();
	oCtx.addChecker(p0Element);

	auto oPairTeam = parseTeamExists(oCtx, p0Element);
	if (! oPairTeam.first) { // doesn't exist
		oCtx.removeChecker(p0Element, false);
		return false; //--------------------------------------------------------
	}
	const int32_t nTeam = oPairTeam.second;
	auto refTeam = p0AppPreferences->getTeam(nTeam);
	const int32_t nTotMates = refTeam->getTotMates();
	auto oPairIntSet = XmlCommonParser::parseIntSetAttrs(oCtx, p0Element
										, s_sConditionalConditionValueAttr
										, s_sConditionalConditionFromAttr, s_sConditionalConditionToAttr, ""
										, s_sConditionalConditionValuesAttr, "", true, 0, false, -1);

	const bool bNot = parseComplement(oCtx, p0Element);
	const bool bCondition = xOr(bNot, (oPairIntSet.second.getIndexOfValue(nTotMates) >= 0));

	oCtx.removeChecker(p0Element, true);
	return bCondition;
}
bool XmlConditionalParser::evalConditionTrue(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	oCtx.addChecker(p0Element);
	oCtx.removeChecker(p0Element, true);
	return true;
}
bool XmlConditionalParser::evalConditionFalse(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	oCtx.addChecker(p0Element);
	oCtx.removeChecker(p0Element, true);
	return false;
}
bool XmlConditionalParser::evalConditionValue(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, int32_t nValue) const
{
	oCtx.addChecker(p0Element);

	const bool bNot = parseComplement(oCtx, p0Element);
	auto oPairIntSet = XmlCommonParser::parseIntSetAttrs(oCtx, p0Element
										, s_sConditionalConditionValueAttr
										, s_sConditionalConditionFromAttr, s_sConditionalConditionToAttr, ""
										, s_sConditionalConditionValuesAttr, "", true, 0, false, -1);
	const bool bCondition = xOr(bNot, (oPairIntSet.second.getIndexOfValue(nValue) >= 0));

	oCtx.removeChecker(p0Element, true);
	return bCondition;
}
bool XmlConditionalParser::evalConditionTraverse(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	auto p0AppPreferences = oCtx.appPreferences().get();
	if (p0AppPreferences == nullptr) {
		//TODO add conditions for XmlThemeParser such as dotPerInch, ScreenSize, etc.
		//TODO generic info that can be gathered from every OS
		oCtx.addChecker(p0Element);
		oCtx.removeChecker(p0Element, true);
		return true; //---------------------------------------------------------
	}

	const std::string sConditionName = p0Element->get_name();
	if (sConditionName == s_sConditionalConditionOrNode) {
		return evalConditionOr(oCtx, p0Element, true); //-----------------------
	} else if (sConditionName == s_sConditionalConditionAndNode) {
		return evalConditionAnd(oCtx, p0Element); //----------------------------
	} else if (sConditionName == s_sConditionalConditionOptionNode) {
		return evalConditionOption(oCtx, p0Element); //-------------------------
	} else if (sConditionName == s_sConditionalConditionTotMatesNode) {
		return evalConditionTotMates(oCtx, p0Element); //-----------------------
	} else if (sConditionName == s_sConditionalConditionTrueNode) {
		return evalConditionTrue(oCtx, p0Element); //---------------------------
	} else if (sConditionName == s_sConditionalConditionFalseNode) {
		return evalConditionFalse(oCtx, p0Element); //--------------------------
	} else {
		int32_t nValue = -1;
		if (sConditionName == s_sConditionalConditionTotTeamsNode) {
			nValue = p0AppPreferences->getTotTeams();
		} else if (sConditionName == s_sConditionalConditionTotHumanTeamsNode) {
			nValue = p0AppPreferences->getTotHumanTeams();
		} else if (sConditionName == s_sConditionalConditionTotAITeamsNode) {
			nValue = p0AppPreferences->getTotAITeams();
		} else if (sConditionName == s_sConditionalConditionTotPlayersNode) {
			nValue = p0AppPreferences->getTotPlayers();
		} else if (sConditionName == s_sConditionalConditionTotHumanPlayersNode) {
			nValue = p0AppPreferences->getTotHumanPlayers();
		} else if (sConditionName == s_sConditionalConditionTotAIPlayersNode) {
			nValue = p0AppPreferences->getTotAIPlayers();
		} else {
			throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString
										, Util::stringCompose("Unknown condition '%1'", sConditionName));
		}
		return evalConditionValue(oCtx, p0Element, nValue); //------------------
	}
}
bool XmlConditionalParser::evalConditionAnd(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	oCtx.addChecker(p0Element);

	auto p0AppPreferences = oCtx.appPreferences().get();
	bool bCondition = true; // logical AND of all children.
	XmlCommonParser::visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0Condition)
	{
		if (p0AppPreferences == nullptr) {
			//TODO add conditions for XmlThemeParser such as dotPerInch, ScreenSize, etc.
			//TODO generic info that can be gathered from every OS
			bCondition = false;
			return; // visitElementChildren -----
		}
		const bool bConditionChild = evalConditionTraverse(oCtx, p0Condition);
		if (bCondition) {
			bCondition = bConditionChild;
		}
	});
	oCtx.removeChecker(p0Element, false, true);
	return bCondition;
}
bool XmlConditionalParser::evalConditionOr(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, bool bCheckAttrs) const
{
	oCtx.addChecker(p0Element);

	auto p0AppPreferences = oCtx.appPreferences().get();
	bool bCondition = false; // logical OR of all children.
	XmlCommonParser::visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0Condition)
	{
		if (p0AppPreferences == nullptr) {
			//TODO add conditions for XmlThemeParser such as dotPerInch, ScreenSize, etc.
			//TODO generic info that can be gathered from every OS
			bCondition = true;
			return; // visitElementChildren -----
		}
		const bool bConditionChild = evalConditionTraverse(oCtx, p0Condition);
		if (! bCondition) {
			bCondition = bConditionChild;
		}
	});
	oCtx.removeChecker(p0Element, false, bCheckAttrs);
	return bCondition;
}
bool XmlConditionalParser::evalCondition(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	assert(p0Element != nullptr);
	return evalConditionOr(oCtx, p0Element, false);
}

std::pair<bool, std::string> XmlConditionalParser::getAttributeValue(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName) const
{
	assert(p0Element != nullptr);
	const xmlpp::Element* p0FirstAttrIf = nullptr;
	XmlCommonParser::visitNamedElementChildren(oCtx, p0Element, s_sConditionalAttrIfNode, [&](const xmlpp::Element * p0AttrIf)
	{
		if (p0FirstAttrIf == nullptr) {
			const bool bOverrideAttrs = evalCondition(oCtx, p0AttrIf);
			if (bOverrideAttrs) {
				p0FirstAttrIf = p0AttrIf;
			}
		}
	});
	if (p0FirstAttrIf != nullptr) {
		const auto oPairAttr = XmlCommonParser::getAttributeValue(oCtx, p0FirstAttrIf, sAttrName);
		if (oPairAttr.first) {
//std::cout << "Attrif override: " << sAttrName << "=" << oPairAttr.second << '\n';
			// Since it could be an overridden attribute need to add to valid attributes of original element
			oCtx.addChecker(p0Element);
			oCtx.addValidAttrName(p0Element, sAttrName);
			oCtx.removeChecker(p0Element, false);
			return oPairAttr; //------------------------------------------------
		} else {
			const auto oPairUndefAttrs = XmlCommonParser::getAttributeValue(oCtx, p0FirstAttrIf, s_sConditionalAttrIfUndefAttr);
			if (oPairUndefAttrs.first) {
				bool bUndef = false;
				XmlUtil::tokenizer(oPairUndefAttrs.second, s_sConditionalAttrIfUndefAttrSeparator, [&](const std::string& sToken)
				{
					if (sToken == sAttrName) {
						bUndef = true;
					}
				});
				if (bUndef) {
					return std::make_pair(false, ""); //------------------------
				}
			}
		}
	}
	return XmlCommonParser::getAttributeValue(oCtx, p0Element, sAttrName);
}

std::pair<bool, int32_t> XmlConditionalParser::parseTeamExists(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	auto p0AppPreferences = oCtx.appPreferences();
	if (p0AppPreferences == nullptr) {
		return std::make_pair(true, -1);
	}

	int32_t nTeam = -1;
	const auto oPairTeam = getAttributeValue(oCtx, p0Element, s_sConditionalOwnerTeamAttr);
	const bool bTeamSet = oPairTeam.first;
	if (bTeamSet) {
		const auto& sTeam = oPairTeam.second;
		if (sTeam.empty()) {
			throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, s_sConditionalOwnerTeamAttr);
		}
		nTeam = XmlUtil::strToNumber(oCtx, p0Element, s_sConditionalOwnerTeamAttr
							, sTeam, false, true, 0, false, -1);
	} else {
		nTeam = oCtx.getContextTeam();
	}
	if (nTeam < 0) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sConditionalOwnerTeamAttr);
	}
	const bool bExists = (nTeam < p0AppPreferences->getTotTeams());
	return std::make_pair(bExists, nTeam);
}

std::tuple<bool, int32_t, int32_t> XmlConditionalParser::parseOwnerExists(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	AppPreferences const * const p0AppPreferences = oCtx.appPreferences().operator->();
	if (p0AppPreferences == nullptr) {
		return std::make_tuple(true, -1, -1);
	}

	const auto oPairTeam = getAttributeValue(oCtx, p0Element, s_sConditionalOwnerTeamAttr);
	const bool bTeamSet = oPairTeam.first;
	int32_t nTeam = -1;
	if (bTeamSet) {
		const auto& sTeam = oPairTeam.second;
		if (!sTeam.empty()) {
			nTeam = XmlUtil::strToNumber(oCtx, p0Element, s_sConditionalOwnerTeamAttr
												, sTeam, false, true, 0, false, -1);
		}
	} else {
		nTeam = oCtx.getContextTeam();
	}
	const auto oPairMate = getAttributeValue(oCtx, p0Element, s_sConditionalOwnerMateAttr);
	const bool bMateSet = oPairMate.first;
	int32_t nMate = -1;
	if (bMateSet) {
		const auto& sMate = oPairMate.second;
		if (!sMate.empty()) {
			if (nTeam < 0) {
				if (p0AppPreferences->getTotTeams() == 1) {
					nTeam = 0;
				}
			}
			nMate = XmlUtil::strToNumber(oCtx, p0Element, s_sConditionalOwnerMateAttr
								, sMate, false, true, 0, false, -1);
		} else {
			//nMate = -1;
		}
	} else if (nTeam >= 0) {
		nMate = oCtx.getContextMate();
	}
	const auto oPairPlayer = getAttributeValue(oCtx, p0Element, s_sConditionalOwnerPlayerAttr);
	const bool bPlayerSet = oPairPlayer.first;
	int32_t nPlayer = -1;
	if (bPlayerSet) {
		if (bMateSet || bTeamSet) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sConditionalOwnerPlayerAttr
					, (bMateSet ? s_sConditionalOwnerMateAttr : s_sConditionalOwnerTeamAttr));
		}
		const auto& sPlayer = oPairPlayer.second;
		if (!sPlayer.empty()) {
			nPlayer = XmlUtil::strToNumber(oCtx, p0Element, s_sConditionalOwnerPlayerAttr
								, sPlayer, false, true, 0, false, -1);
		}
	}
	if (nPlayer >= 0) {
		if (nPlayer >= p0AppPreferences->getTotPlayers()) {
			// bogus team and mate
			nTeam = p0AppPreferences->getTotTeams();
			nMate = nPlayer - p0AppPreferences->getTotPlayers();
		} else {
			auto refPlayer = p0AppPreferences->getPlayer(nPlayer);
			nTeam = refPlayer->getTeam()->get();
			nMate = refPlayer->getMate();
		}
	}
	if ((nMate >= 0) && (nTeam < 0)) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sConditionalOwnerTeamAttr);
	}

	const int32_t nTotTeams = p0AppPreferences->getTotTeams();
	const bool bExists = (nTeam == -1)
						|| ((nMate == -1) && (nTeam < nTotTeams))
						|| ((nMate >= 0) && (nTeam < nTotTeams) && (nMate < p0AppPreferences->getTeam(nTeam)->getTotMates()));
//std::cout << "&&&&&&&&& bExists=" << bExists << "  nTeam=" << nTeam << " nMate=" << nMate << '\n';
	return std::make_tuple(bExists, nTeam, nMate);
}
std::pair<int32_t, int32_t> XmlConditionalParser::parseOwner(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const
{
	return XmlConditionalParser::parseOwner(oCtx, p0Element, OwnerType::GAME);
}
std::pair<int32_t, int32_t> XmlConditionalParser::parseOwner(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, OwnerType eRequireOwnerType) const
{
	AppPreferences const * const p0AppPreferences = oCtx.appPreferences().operator->();
	assert(p0AppPreferences != nullptr);

	const auto oPairTeam = getAttributeValue(oCtx, p0Element, s_sConditionalOwnerTeamAttr);
	const bool bTeamSet = oPairTeam.first;
	int32_t nTeam = -1;
	if (bTeamSet) {
		const auto& sTeam = oPairTeam.second;
//std::cout << "XmlConditionalParser::parseOwner sTeam=" << sTeam << '\n';
		if (!sTeam.empty()) {
			nTeam = XmlUtil::strToNumber(oCtx, p0Element, s_sConditionalOwnerTeamAttr
								, sTeam, false, true, 0, true, p0AppPreferences->getTotTeams() - 1);
		}
	} else {
		//if ((eRequireOwnerType == OwnerType::TEAM) || (eRequireOwnerType == OwnerType::PLAYER)) {
		nTeam = oCtx.getContextTeam();
		//}
	}
//std::cout << "XmlConditionalParser::parseOwner bTeamSet=" << bTeamSet << " nTeam=" << nTeam << '\n';
	const auto oPairMate = getAttributeValue(oCtx, p0Element, s_sConditionalOwnerMateAttr);
	const bool bMateSet = oPairMate.first;
	int32_t nMate = -1;
	if (bMateSet) {
		const auto& sMate = oPairMate.second;
		if (!sMate.empty()) {
			if (nTeam < 0) {
				if (p0AppPreferences->getTotTeams() == 1) {
					nTeam = 0;
				} else {
					throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sConditionalOwnerTeamAttr);
				}
			}
			nMate = XmlUtil::strToNumber(oCtx, p0Element, s_sConditionalOwnerMateAttr
								, sMate, false, true, 0, true, p0AppPreferences->getTeam(nTeam)->getTotMates() - 1);
		} else {
			//nMate = -1;
		}
	} else if (nTeam >= 0) {
		//if (eRequireOwnerType == OwnerType::PLAYER) {
		nMate = oCtx.getContextMate();
		//}
	}
	const auto oPairPlayer = getAttributeValue(oCtx, p0Element, s_sConditionalOwnerPlayerAttr);
	const bool bPlayerSet = oPairPlayer.first;
	int32_t nPlayer = -1;
	if (bPlayerSet) {
		if (bMateSet || bTeamSet) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sConditionalOwnerPlayerAttr
										, (bMateSet ? s_sConditionalOwnerMateAttr : s_sConditionalOwnerTeamAttr));
		}
		const auto& sPlayer = oPairPlayer.second;
		if (!sPlayer.empty()) {
			nPlayer = XmlUtil::strToNumber(oCtx, p0Element, s_sConditionalOwnerPlayerAttr
								, sPlayer, false, true, 0, true, p0AppPreferences->getTotPlayers() - 1);
		}
	}
	if (nPlayer >= 0) {
		auto refPlayer = p0AppPreferences->getPlayer(nPlayer);
		nTeam = refPlayer->getTeam()->get();
		nMate = refPlayer->getMate();
	} else if ((nMate >= 0) && (nTeam < 0)) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sConditionalOwnerTeamAttr);
	}
	if (eRequireOwnerType == OwnerType::PLAYER) {
		if (nMate < 0) {
			throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
												"Player is not defined (by either attributes %1 or %2)"
												, s_sConditionalOwnerMateAttr, s_sConditionalOwnerPlayerAttr));
		}
	} else if (eRequireOwnerType == OwnerType::TEAM) {
		if (nTeam < 0) {
			throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
												"Player is not defined (by attribute %1)"
												, s_sConditionalOwnerTeamAttr));
		}
	}
	return std::make_pair(nTeam, nMate);
}
std::pair<bool, OwnerType> XmlConditionalParser::parseOwnerType(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName) const
{
	const std::string& sOwnerTypeAttr = (sAttrName.empty() ? s_sConditionalOwnerTypeAttr : sAttrName);

	static const std::vector<char const *> s_aOwnerTypeEnumString{s_sConditionalOwnerTypeAttrGame.c_str(), s_sConditionalOwnerTypeAttrTeam.c_str(), s_sConditionalOwnerTypeAttrPlayer.c_str()};
	static const std::vector<OwnerType> s_aOwnerTypeEnumValue{OwnerType::GAME, OwnerType::TEAM, OwnerType::PLAYER};
	//
	const int32_t nIdxOwnerType = getEnumAttributeValue(oCtx, p0Element, sOwnerTypeAttr, s_aOwnerTypeEnumString);
	if (nIdxOwnerType >= 0) {
		return std::make_pair(true, s_aOwnerTypeEnumValue[nIdxOwnerType]);
	} else {
		return std::make_pair(false, OwnerType::GAME);
	}
}
int32_t XmlConditionalParser::getEnumAttributeValue(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName
										, const std::vector<char const *>& aValues) const
{
	const auto oPairValue = getAttributeValue(oCtx, p0Element, sAttrName);
	if (! oPairValue.first) {
		return -1; //-----------------------------------------------------------
	}
	int32_t nIdx = 0;
	for (const auto& sValue : aValues) {
		if (sValue == oPairValue.second) {
			return nIdx; //-----------------------------------------------------
		}
		++nIdx;
	}
	throw XmlCommonErrors::error(oCtx, p0Element, sAttrName, Util::stringCompose("attribute %1: value not valid", sAttrName));
}
const xmlpp::Element* XmlConditionalParser::parseUniqueElement(ConditionalCtx& oCtx, const xmlpp::Element* p0Parent
																, const std::string& sName, bool bMandatory) const
{
	assert(p0Parent != nullptr);

	const xmlpp::Element* p0Found = nullptr;

	visitNamedElementChildren(oCtx, p0Parent, sName, [&](const xmlpp::Element* p0Element)
	{
		if (p0Found != nullptr) {
			throw XmlCommonErrors::errorElementTooMany(oCtx, p0Element, sName);
		}
		p0Found = p0Element;
	});
	if ((p0Found == nullptr) && bMandatory) {
		throw XmlCommonErrors::errorElementExpected(oCtx, dynamic_cast<const xmlpp::Element*>(p0Parent), sName);
	}
	return p0Found;
}
void XmlConditionalParser::assertTrue(bool
									#ifndef NDEBUG
									bCond
									#endif //NDEBUG
									)
{
	assert(bCond);
}

} // namespace stmg
