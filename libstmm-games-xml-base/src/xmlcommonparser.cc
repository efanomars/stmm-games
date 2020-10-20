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
 * File:   xmlcommonparser.cc
 */

#include "xmlcommonparser.h"

#include <stmm-games/util/intset.h>
#include <stmm-games/util/util.h>

#include <glib.h>

#include <vector>
#include <cassert>
#include <limits>
//#include <iostream>
#include <cstdint>
#include <type_traits>

namespace stmg
{

static const std::string s_sSeparatorAttr = "separator";
static const std::string s_sSeparatorDefault = ",";

static const std::string s_sTraitNameSeparators = ":-";
static const std::string s_sTraitAnyNameValue = "*";

const std::string XmlCommonParser::s_sAttrNameIgnorePrefix = "_ign_";
const std::string XmlCommonParser::s_sElementNameIgnorePrefix = "_Ign_";

bool XmlCommonParser::validateName(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName
									, const std::string& sName, bool bAllowAny)
{
	if (sName.empty()) {
		throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, sAttrName);
	}
	if (sName.compare(s_sTraitAnyNameValue) == 0) {
		if (!bAllowAny) {
			throw XmlCommonErrors::error(oCtx, p0Element, sAttrName, Util::stringCompose("Attribute '%1': name cannot be any in this context", sAttrName));
		}
		// value is "any name"
		return true; //---------------------------------------------------------
	}
	Glib::ustring sUName{sName};
	{
		const gunichar c = sUName[0];
		if (!Glib::Unicode::isalpha(c)) {
			throw XmlCommonErrors::error(oCtx, p0Element, sAttrName, Util::stringCompose("Attribute '%1': name must start with a letter", sAttrName));
		}
	}
	bool bSeparator = false;
	for (Glib::ustring::const_iterator it = sUName.begin(); it != sUName.end(); ++it) {
		const gunichar c = *it;
		bSeparator = (s_sTraitNameSeparators.find(c) != std::string::npos);
		if (! (bSeparator || Glib::Unicode::isalpha(c) || (Glib::Unicode::isalnum(c)))) {
			throw XmlCommonErrors::error(oCtx, p0Element, sAttrName, Util::stringCompose("Attribute '%1':"
										" name can only contain letters, numbers and one of '%2'", sAttrName, s_sTraitNameSeparators));
		}
	}
	if (bSeparator) {
		throw XmlCommonErrors::error(oCtx, p0Element, sAttrName, Util::stringCompose("Attribute '%1':"
									" name cannot end with separator ('%2')", sAttrName, s_sTraitNameSeparators));
	}
	return false;
}

const xmlpp::Element* XmlCommonParser::parseUniqueElement(ParserCtx& oCtx, const xmlpp::Element* p0Parent
														, const std::string& sName, bool bMandatory)
{
	assert(p0Parent != nullptr);
	assert(!sName.empty());

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
std::pair<bool, std::string> XmlCommonParser::getAttributeValue(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr0)
{
	assert(p0Element != nullptr);
	oCtx.addChecker(p0Element);
	oCtx.addValidAttrNames(p0Element, sAttr0);
	oCtx.removeChecker(p0Element, false);
	auto* p0Attribute = p0Element->get_attribute(sAttr0);
	if (p0Attribute == nullptr) {
		return std::make_pair(false, "");
	}
	return std::make_pair(true, p0Attribute->get_value());
}
std::pair<bool, std::string> XmlCommonParser::getAttributeValue(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr0, const std::string& sAttr1)
{
	assert(p0Element != nullptr);
	oCtx.addChecker(p0Element);
	oCtx.addValidAttrNames(p0Element, sAttr0, sAttr1);
	oCtx.removeChecker(p0Element, false);
	auto* p0Attribute = (sAttr0.empty() ? nullptr : p0Element->get_attribute(sAttr0));
	if (p0Attribute == nullptr) {
		p0Attribute = (sAttr1.empty() ? nullptr : p0Element->get_attribute(sAttr1));
	}
	if (p0Attribute == nullptr) {
		return std::make_pair(false, "");
	}
	return std::make_pair(true, p0Attribute->get_value());
}
std::string XmlCommonParser::parseSeparator(ParserCtx& oCtx, const xmlpp::Element* p0Element
											, const std::string& sSeparatorAttr)
{
	if (sSeparatorAttr.empty()) {
		return s_sSeparatorDefault; //------------------------------------------
	}
	oCtx.addChecker(p0Element);
	oCtx.addValidAttrNames(p0Element, sSeparatorAttr, s_sSeparatorAttr);
	oCtx.removeChecker(p0Element, false);
	const auto oSeparator = getAttributeValue(oCtx, p0Element, sSeparatorAttr, s_sSeparatorAttr);
	if (oSeparator.first) {
		auto sSeparator = Util::strStrip(oSeparator.second);
		if (sSeparator.empty()) {
			throw XmlCommonErrors::error(oCtx, p0Element, sSeparatorAttr, Util::stringCompose("Attribute '%1'"
												" cannot be a whitespace character", sSeparator));
		}
		return sSeparator; //---------------------------------------------------
	}
	return s_sSeparatorDefault;
}
static bool parsIntSetAttrs(ParserCtx& oCtx, const xmlpp::Element* p0Element
							, const std::string& sMultiAttr, const std::string& sSeparatorAttr
							, IntSet& oIntSet
							, const std::string& sAlreadyAttr, const std::string& sAlreadyAttr2
							, bool bMin, int32_t nMin, bool bMax, int32_t nMax)
{
	//
	const std::string sSeparator = XmlCommonParser::parseSeparator(oCtx, p0Element, sSeparatorAttr);
	const auto oInts = XmlCommonParser::getAttributeValue(oCtx, p0Element, sMultiAttr);
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
	XmlUtil::tokenizer(oInts.second, sSeparator, [&](const std::string& sToken)
	{
		const int32_t nValue = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, sMultiAttr, sToken, false
																	, bMin, nMin, bMax, nMax);
		aValues.push_back(nValue);
	});
	oIntSet = IntSet{aValues};
	return true;
}
std::pair<bool, IntSet> XmlCommonParser::parseIntSetAttrs(ParserCtx& oCtx, const xmlpp::Element* p0Element
														, const std::string& sSingleAttr
														, const std::string& sFromAttr, const std::string& sToAttr, const std::string& sStepAttr
														, const std::string& sMultiAttr, const std::string& sSeparatorAttr
														, bool bMin, int32_t nMin, bool bMax, int32_t nMax)
{
	IntSet oIntSet;

	int32_t nDefinedAttr = -1;
	const auto oSingleValue = XmlCommonParser::getAttributeValue(oCtx, p0Element, sSingleAttr);
	const bool bSingleDefined = oSingleValue.first;
	if (bSingleDefined) {
		const int32_t nValue = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, sSingleAttr, oSingleValue.second, false
																	, bMin, nMin, bMax, nMax);
		oIntSet = IntSet{nValue};
		nDefinedAttr = 0;
	}
	int32_t nFromInt = 0;
	int32_t nToInt = 0;
	const auto oFromValue = XmlCommonParser::getAttributeValue(oCtx, p0Element, sFromAttr);
	const bool bFromDefined = oFromValue.first;
	if (bFromDefined) {
		if (bSingleDefined) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sFromAttr, sSingleAttr);
		}
		nFromInt = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, sFromAttr, oFromValue.second, false
														, bMin, nMin, bMax, nMax);
		nDefinedAttr = 1;
	}
	const auto oToValue = XmlCommonParser::getAttributeValue(oCtx, p0Element, sToAttr);
	const bool bToDefined = oToValue.first;
	if (bToDefined) {
		if (bSingleDefined) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sToAttr, sSingleAttr);
		}
		nToInt = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, sToAttr, oToValue.second, false
														, bMin, nMin, bMax, nMax);
		if (!bFromDefined) {
			nFromInt = (bMin ? nMin : std::numeric_limits<int32_t>::lowest());
		}
		nDefinedAttr = 1;
	} else {
		if (bFromDefined) {
			nToInt = (bMax ? nMax : std::numeric_limits<int32_t>::max());
		}
	}
	int32_t nStepInt = 1;
	if (! sStepAttr.empty()) {
		const auto oStepValue = XmlCommonParser::getAttributeValue(oCtx, p0Element, sStepAttr);
		const bool bStepDefined = oStepValue.first;
		if (bStepDefined) {
			if (nDefinedAttr == 0) {
				throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sStepAttr, sSingleAttr);
			}
			if (nDefinedAttr != 1) {
				assert(nDefinedAttr == -1);
				nFromInt = (bMin ? nMin : std::numeric_limits<int32_t>::lowest());
				nToInt = (bMax ? nMax : std::numeric_limits<int32_t>::max());
			}
			nStepInt = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, sStepAttr, oStepValue.second, false
															, false, -1, false, -1);
			if (nStepInt == 0) {
				throw XmlCommonErrors::errorAttrInvalidValue(oCtx, p0Element, sStepAttr);
			}
			nDefinedAttr = 1;
		}
	}
	if (nDefinedAttr == 1) {
		oIntSet = IntSet{nFromInt, nToInt, nStepInt};
	}
	const bool bMultiDefined = parsIntSetAttrs(oCtx, p0Element, sMultiAttr, sSeparatorAttr, oIntSet
								, ((nDefinedAttr < 0) ? "" : ((nDefinedAttr == 0) ? sSingleAttr : sFromAttr)), sToAttr
								, bMin, nMin, bMax, nMax);
	return std::make_pair(((nDefinedAttr >= 0) || bMultiDefined), oIntSet);
}
void XmlCommonParser::assertTrue(bool
								#ifndef NDEBUG
								bCond
								#endif //NDEBUG
								)
{
	assert(bCond);
}

} // namespace stmg
