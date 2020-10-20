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
 * File:   xmlthememodifiersparser.cc
 */

#include "xmlthememodifiersparser.h"

#include "xmlmodifierparser.h"
#include "themectx.h"
#include "xmlthemeimageparser.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games-gtk/stdtheme.h>
#include <stmm-games-gtk/stdthememodifier.h>
#include <stmm-games-gtk/gtkutil/tileani.h>

#include <stmm-games/named.h>
#include <stmm-games/util/namedindex.h>

#include <glibmm/ustring.h>

#include <libxml++/libxml++.h>

#include <vector>
#include <cassert>
//#include <iostream>
#include <memory>
#include <string>
#include <utility>

namespace stmg { class Image; }
namespace stmg { class XmlTraitsParser; }

namespace stmg
{

static const std::string s_sDrawTileModifierTileAniNameAttr = "aniName";
static const std::string s_sDrawTileModifierTileAniElapsedFactorAttr = "aniFactor";
static const std::string s_sDrawTileModifierTileAniInvertAttr = "invert";
static const std::string s_sDrawTileModifierTileAniIdAttr = "aniId";

XmlThemeModifiersParser::XmlThemeModifiersParser(XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser
												, XmlThemeImageParser& oXmlThemeImageParser)
: m_oXmlConditionalParser(oXmlConditionalParser)
, m_oXmlTraitsParser(oXmlTraitsParser)
, m_oXmlThemeImageParser(oXmlThemeImageParser)
{
}

void XmlThemeModifiersParser::addXmlModifierParser(unique_ptr<XmlModifierParser> refXmlModifierParser)
{
	assert(refXmlModifierParser);
	assert(refXmlModifierParser->m_p1Owner == nullptr);
	refXmlModifierParser->m_p1Owner = this;
	m_aModifierParsers.push_back(std::move(refXmlModifierParser));
}

std::vector< unique_ptr<StdThemeModifier> > XmlThemeModifiersParser::parseModifiers(ThemeCtx& oCtx, const xmlpp::Element* p0ParentElement)
{
	return parseModifiers(oCtx, p0ParentElement, std::vector<std::string const*>{});
}
std::vector< unique_ptr<StdThemeModifier> > XmlThemeModifiersParser::parseModifiers(ThemeCtx& oCtx, const xmlpp::Element* p0ParentElement
																	, const std::vector<std::string const*>& aSkipChildNames)
{
	std::vector< unique_ptr<StdThemeModifier> > aModifier;
	m_oXmlConditionalParser.visitElementChildren(oCtx, p0ParentElement, [&](const xmlpp::Element* p0ModifierElement)
	{
		const std::string sChildName = p0ModifierElement->get_name();
		auto itFind = std::find_if(aSkipChildNames.begin(), aSkipChildNames.end(), [&](std::string const* p0ChildName)
		{
			return (sChildName == *p0ChildName);
		});
		if (itFind != aSkipChildNames.end()) {
//std::cout << "XmlLayoutParser::parseChildWidgets() skip sChildName=" << sChildName << '\n';
			oCtx.addValidChildElementName(p0ParentElement, sChildName);
		} else {
			unique_ptr<StdThemeModifier> refModifier = parseModifier(oCtx, p0ModifierElement);
			aModifier.push_back(std::move(refModifier));
		}
	});
	return aModifier;
}
unique_ptr<StdThemeModifier> XmlThemeModifiersParser::parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0ModifierElement)
{
	oCtx.addChecker(p0ModifierElement);
	unique_ptr<StdThemeModifier> refModifier;
	const std::string sModifierName = p0ModifierElement->get_name();
	for (auto& refModifierParser : m_aModifierParsers) {
		if (refModifierParser->getModifierName() == sModifierName) {
			refModifier = refModifierParser->parseModifier(oCtx, p0ModifierElement);
			if (refModifier) {
				break;
			}
		}
	}
	if (!refModifier) {
//std::cout << "parseModifier() unknown sModifierName: " << sModifierName << '\n';
		throw XmlCommonErrors::errorElementInvalid(oCtx, p0ModifierElement, sModifierName);
	}
	oCtx.removeChecker(p0ModifierElement, false);
	return refModifier;
}
shared_ptr<TileAni> XmlThemeModifiersParser::parseModifierTileAniId(ThemeCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	const auto oPairAniId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sDrawTileModifierTileAniIdAttr);
	const bool bAniIdDefined = oPairAniId.first;
	const std::string& sAniId = oPairAniId.second;
	if (bAniIdDefined && !oTheme.hasTileAniId(sAniId)) {
		throw XmlCommonErrors::errorAttrWithValueNotDefined(oCtx, p0Element, s_sDrawTileModifierTileAniIdAttr, sAniId);
	}

	shared_ptr<TileAni> refTileAni;
	if (bAniIdDefined) {
		refTileAni = oTheme.getTileAni(sAniId);
	} else if (bMandatory) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sDrawTileModifierTileAniIdAttr);
	}

	oCtx.removeChecker(p0Element, false);
	return refTileAni;
}
int32_t XmlThemeModifiersParser::parseModifierTileAniName(ThemeCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	int32_t nTileAniNameIdx = -1;
	const auto oPairAniName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sDrawTileModifierTileAniNameAttr);
	const bool bAniNameDefined = oPairAniName.first;
	const std::string& sAniName = oPairAniName.second;
	if (bAniNameDefined) {
		nTileAniNameIdx = oTheme.getNamed().tileAnis().addName(sAniName);
	} else if (bMandatory) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sDrawTileModifierTileAniNameAttr);
	}
	oCtx.removeChecker(p0Element, false);
	return nTileAniNameIdx;
}
double XmlThemeModifiersParser::parseModifierElapsedDefault(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	double fElapsed = -1.0;
	const auto oPairAniElapsed = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sDrawTileModifierTileAniElapsedFactorAttr);
	const bool bElapsedDefined = oPairAniElapsed.first;
	if (bElapsedDefined) {
		const std::string& sAniElapsed = oPairAniElapsed.second;
		fElapsed = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sDrawTileModifierTileAniElapsedFactorAttr, sAniElapsed, true
														, true, -1.0, true, 1.0);
		if (fElapsed < 0.0) {
			fElapsed = -1.0;
		}
	}
	oCtx.removeChecker(p0Element, false);
	return fElapsed;
}
bool XmlThemeModifiersParser::parseModifierInvert(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	bool bInvert = false;
	const auto oPairInvert = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sDrawTileModifierTileAniInvertAttr);
	const bool bInvertDefined = oPairInvert.first;
	if (bInvertDefined) {
		const std::string& sInvert = oPairInvert.second;
		bInvert = XmlUtil::strToBool(oCtx, p0Element, s_sDrawTileModifierTileAniInvertAttr, sInvert);
	}
	oCtx.removeChecker(p0Element, false);
	return bInvert;
}
std::tuple<int32_t, double, bool> XmlThemeModifiersParser::parseTileAniNameDefault(ThemeCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory)
{
	const int32_t nAniIdx = parseModifierTileAniName(oCtx, p0Element, false);
	const double fElapsedDefault = parseModifierElapsedDefault(oCtx, p0Element);
	if (bMandatory && (nAniIdx < 0) && (fElapsedDefault < 0.0)) {
		throw XmlCommonErrors::errorAttrEitherMustBeDefined(oCtx, p0Element, s_sDrawTileModifierTileAniElapsedFactorAttr
																, s_sDrawTileModifierTileAniNameAttr);
	}
	const bool bInvert = parseModifierInvert(oCtx, p0Element);
	return std::make_tuple(nAniIdx, fElapsedDefault, bInvert);
}
std::tuple<int32_t, double, bool, shared_ptr<Image>, shared_ptr<TileAni>> XmlThemeModifiersParser::parseTileAniIdNameImgDefault(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	StdTheme& oTheme = oCtx.theme();

	const auto oPairMaskFile = m_oXmlThemeImageParser.parseImage(oCtx, p0Element, false);
	const std::string& sImgAttr = oPairMaskFile.first;
	const bool bImageDefined = ! sImgAttr.empty();
	const std::string& sMaskFile = oPairMaskFile.second;

	const shared_ptr<TileAni> refTileAni = parseModifierTileAniId(oCtx, p0Element, false);
	const bool bAniIdDefined = refTileAni.operator bool();

	if ((!bAniIdDefined) && !bImageDefined) {
		throw XmlCommonErrors::errorAttrEitherMustBeDefined(oCtx, p0Element, s_sDrawTileModifierTileAniIdAttr
															, XmlThemeImageParser::s_sImagesImageFileAttr);
	}
	const int32_t nAniIdx = parseModifierTileAniName(oCtx, p0Element, false);
	const double fElapsedDefault = parseModifierElapsedDefault(oCtx, p0Element);
	const bool bInvert = parseModifierInvert(oCtx, p0Element);

	shared_ptr<Image> refImg;
	if (bImageDefined) {
		refImg = oTheme.getImageByFileName(sMaskFile);
	}

	if (!bAniIdDefined) {
		if ((nAniIdx >= 0) || (fElapsedDefault >= 0.0) || bInvert) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sDrawTileModifierTileAniIdAttr);
		}
	} else if (!bImageDefined) {
		if (! ((fElapsedDefault >= 0.0) || (nAniIdx >= 0) || refTileAni->getDefaultImage())) {
			throw XmlCommonErrors::errorAttrEitherMustBeDefined(oCtx, p0Element, s_sDrawTileModifierTileAniNameAttr, s_sDrawTileModifierTileAniElapsedFactorAttr);
		}
	} else {
		if (nAniIdx < 0) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sDrawTileModifierTileAniNameAttr);
		}
		if (fElapsedDefault >= 0.0) {
			throw XmlCommonErrors::errorAttrCannotBothBeDefined(oCtx, p0Element, s_sDrawTileModifierTileAniNameAttr, s_sDrawTileModifierTileAniElapsedFactorAttr);
		}
	}

	return std::make_tuple(nAniIdx, fElapsedDefault, bInvert, refImg, refTileAni);
}

} // namespace stmg
