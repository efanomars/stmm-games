/*
 * File:   xmlvariantsetparser.cc
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

#include "xmlutil/xmlvariantsetparser.h"

#include "xmlcommonparser.h"
#include "xmlutil/xmlstrconv.h"
#include "xmlcommonerrors.h"

#include <stmm-games/option.h>
#include <stmm-games/util/util.h>
#include <stmm-games/util/variant.h>
#include <stmm-games/util/variantset.h>

#include <string>
#include <utility>
#include <vector>

namespace stmg { class ParserCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sGameConstraintsExprOptionValueAttr = "value";
static const std::string s_sGameConstraintsExprOptionFromAttr = "from";
static const std::string s_sGameConstraintsExprOptionToAttr = "to";
static const std::string s_sGameConstraintsExprOptionValuesAttr = "values";

VariantSet XmlVariantSetParser::parseVariantSet(ParserCtx& oCtx, const xmlpp::Element* p0Element, const Option& oOption)
{
	const auto& sSingleAttr = s_sGameConstraintsExprOptionValueAttr;
	const auto& sFromAttr = s_sGameConstraintsExprOptionFromAttr;
	const auto& sToAttr = s_sGameConstraintsExprOptionToAttr;
	const auto& sMultiAttr = s_sGameConstraintsExprOptionValuesAttr;

	Variant oSingleValue{};
	const auto oPairSingle = XmlCommonParser::getAttributeValue(oCtx, p0Element, sSingleAttr);
	const bool bSingleDefined = oPairSingle.first;
	if (bSingleDefined) {
		const std::string& sSingle = oPairSingle.second;
		auto oPair = oOption.getValueFromString(sSingle);
		if (! oPair.second.empty()) {
			throw XmlCommonErrors::error(oCtx, p0Element, sSingleAttr, Util::stringCompose("attribute '%1': %2", sSingleAttr, oPair.second));
		}
		oSingleValue = std::move(oPair.first);
		if (oSingleValue.isNull()) {
			throw XmlCommonErrors::errorAttrInvalidValue(oCtx, p0Element, sSingleAttr);
		}
	}
	Variant oFromValue{};
	const auto oPairFrom = XmlCommonParser::getAttributeValue(oCtx, p0Element, sFromAttr);
	const bool bFromDefined = oPairFrom.first;
	if (bFromDefined) {
		const std::string& sFrom = oPairFrom.second;
		if (bSingleDefined) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sFromAttr, sSingleAttr);
		}
		auto oPair = oOption.getValueFromString(sFrom);
		if (! oPair.second.empty()) {
			throw XmlCommonErrors::error(oCtx, p0Element, sFromAttr, Util::stringCompose("attribute '%1': %2", sFromAttr, oPair.second));
		}
		oFromValue = std::move(oPair.first);
	}
	Variant oToValue{};
	const auto oPairTo = XmlCommonParser::getAttributeValue(oCtx, p0Element, sToAttr);
	const bool bToDefined = oPairTo.first;
	if (bToDefined) {
		const std::string& sTo = oPairTo.second;
		if (bSingleDefined) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sToAttr, sSingleAttr);
		}
		auto oPair = oOption.getValueFromString(sTo);
		if (! oPair.second.empty()) {
			throw XmlCommonErrors::error(oCtx, p0Element, sFromAttr, Util::stringCompose("attribute '%1': %2", sFromAttr, oPair.second));
		}
		oToValue = std::move(oPair.first);
		if (oFromValue.isNull()) {
			oFromValue = oToValue;
		}
	} else {
		if (bFromDefined && (oToValue.isNull())) {
			oToValue = oFromValue;
		}
	}
	const bool bFromToDefined = (bFromDefined || bToDefined);
	if (bFromToDefined) {
		if (oFromValue.getType() != oOption.getDefaultValue().getType()) {
			throw XmlCommonErrors::error(oCtx, p0Element, sFromAttr, Util::stringCompose("attribute '%1': wrong type", sFromAttr));
		}
		if (oFromValue.getType() != oToValue.getType()) {
			throw XmlCommonErrors::error(oCtx, p0Element, sFromAttr, Util::stringCompose("attributes '%1' and '%2' must have same type", sFromAttr));
		}
		if (! oOption.allowsRange()) {
			throw XmlCommonErrors::error(oCtx, p0Element, sFromAttr, Util::stringCompose("attribute '%1': type doesn't allow range", sFromAttr));
		}
		if (oToValue < oFromValue) {
			std::swap(oFromValue, oToValue);
		}
	}
	//
	std::vector<Variant> aValues;
	const std::string sSeparator = XmlCommonParser::parseSeparator(oCtx, p0Element, Util::s_sEmptyString);
	const auto oPairMulti = XmlCommonParser::getAttributeValue(oCtx, p0Element, sMultiAttr);
	if (oPairMulti.first) {
		const std::string& sMulti = oPairMulti.second;
		if (bSingleDefined) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sMultiAttr, sSingleAttr);
		}
		if (bFromToDefined) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByOthers(oCtx, p0Element, sMultiAttr, sFromAttr, sToAttr);
		}
		Variant::TYPE eType = Variant::TYPE_NULL;
		XmlUtil::tokenizer(sMulti, sSeparator, [&](const std::string& sToken)
		{
			auto oPair = oOption.getValueFromString(sToken);
			const auto& sError = oPair.second;
			if (! sError.empty()) {
				throw XmlCommonErrors::error(oCtx, p0Element, sMultiAttr, Util::stringCompose("attribute '%1': %2", sMultiAttr, sError));
			}
			{
				const Variant& oVal = oPair.first;
				if (oVal.isNull()) {
					throw XmlCommonErrors::errorAttrInvalidValue(oCtx, p0Element, sMultiAttr);
				}
				if (eType == Variant::TYPE_NULL) {
					eType = oVal.getType();
				} else {
					if (oVal.getType() != eType) {
						throw XmlCommonErrors::errorAttrInvalidValue(oCtx, p0Element, sMultiAttr);
					}
				}
			}
			aValues.push_back(std::move(oPair.first));
		});
	}
	const bool bMultiDefined = (! aValues.empty());
	const bool bDefined = (bSingleDefined || bFromToDefined || bMultiDefined);
	if (! bDefined) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
				"Either attribute '%1', '%2' or '%3' have to be defined", sSingleAttr, sFromAttr, sToAttr));
	}
	if (bSingleDefined) {
		return VariantSet{oSingleValue}; //-------------------------------------
	}
	if (bFromToDefined) {
		return VariantSet{oFromValue, oToValue}; //-----------------------------
	}
	return VariantSet{aValues};
}

} // namespace stmg
