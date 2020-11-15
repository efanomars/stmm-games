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
 * File:   xmlthemeparser.cc
 */

#include "xmlthemeparser.h"
#include "xmlthanimationfactoryparser.h"
#include "xmlmodifierparser.h"
#include "themectx.h"
#include "xmlthwidgetfactoryparser.h"

#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/parserctx.h>
#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlcommonparser.h>

#include <stmm-games-gtk/commontheme.h>
#include <stmm-games-gtk/stdtheme.h>
#include <stmm-games-gtk/stdthememodifier.h>
#include <stmm-games-gtk/themeloader.h>
#include <stmm-games-gtk/modifiers/nextsubpaintermodifier.h>

#include <stmm-games/appconfig.h>
#include <stmm-games/tile.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/util.h>

#include <glibmm/ustring.h>

#include <libxml++/libxml++.h>

#include <cassert>
//#include <iostream>
#include <cstdint>
#include <utility>
#include <algorithm>
#include <iterator>

#include <stdint.h>

namespace stmg
{

static const std::string s_sXmlThemeNodeName = "Theme";
static const std::string s_sXmlThemeInternalNameAttr = "internalName";
static const std::string s_sXmlThemeTestingAttr = "testing";

static const std::string s_sXmlThemeDescriptionNodeName = "Description";
static const std::string s_sXmlThemeSupportsNodeName = "Supports";
static const std::string s_sXmlThemeExtendsNodeName = "Extends";

static const std::string s_sColorsNodeName = "Colors";
static const std::string s_sColorsColorNodeName = "Color";
static const std::string s_sColorsColorNameAttr = "name";
static const std::string s_sColorsColorPalAttr = "pal";
static const std::string s_sColorsColorRgbAttr = "rgb";
static const std::string s_sColorsStdColorsNodeName = "StdColor";

static const std::string s_sFontsNodeName = "Fonts";
static const std::string s_sFontsFontNodeName = "Font";
static const std::string s_sFontsDefaultNodeName = "DefaultFont";
static const std::string s_sFontsFontNameAttr = "name";
static const std::string s_sFontsFontDefineAttr = "define";

static const std::string s_sImagesNodeName = "Images";
static const std::string s_sImageArraysNodeName = "ImageArrays";

static const std::string s_sSoundsNodeName = "Sounds";

static const std::string s_sAnimationsNodeName = "Animations";
static const std::string s_sTileAnisNodeName = "TileAnimations";
static const std::string s_sAssignsNodeName = "Assigns";
static const std::string s_sWidgetsNodeName = "WidgetFactories";

static const std::string s_sPaintersNodeName = "TilePainters";
static const std::string s_sPaintersPainterNodeName = "Painter";
static const std::string s_sPaintersPainterNameAttr = "name";
static const std::string s_sPaintersPainterNameAttrBoard = "PAINTER:BOARD";
static const std::string s_sPaintersPainterNameAttrBlock = "PAINTER:BLOCK";
static const std::string s_sPaintersPainterDefaultAttr = "default";
static const std::string s_sPaintersClonePainterNodeName = "ClonePainter";
static const std::string s_sPaintersClonePainterNameAttr = "name";
static const std::string s_sPaintersClonePainterCloningNameAttr = "cloning";


XmlThemeParser::XmlThemeParser()
: m_oXmlVariantSetParser()
, m_oXmlConditionalParser(m_oXmlVariantSetParser)
, m_oXmlTraitsParser(m_oXmlConditionalParser)
, m_oXmlThemeImageParser(m_oXmlConditionalParser)
, m_oXmlThemeImagesParser(m_oXmlConditionalParser)
, m_oXmlThemeSoundsParser(m_oXmlConditionalParser)
, m_oXmlThemeAniParser(m_oXmlConditionalParser, m_oXmlTraitsParser, m_oXmlThemeImageParser)
, m_oXmlThemeAssignParser(m_oXmlConditionalParser, m_oXmlTraitsParser, m_oXmlThemeImagesParser)
, m_oXmlThemeModifiersParser(m_oXmlConditionalParser, m_oXmlTraitsParser, m_oXmlThemeImageParser)
, m_oXmlThemeWidgetParser(m_oXmlConditionalParser, m_oXmlTraitsParser, m_oXmlThemeImageParser)
{
}
void XmlThemeParser::addXmlModifierParser(unique_ptr<XmlModifierParser> refXmlModifierParser)
{
	assert(refXmlModifierParser);
	m_oXmlThemeModifiersParser.addXmlModifierParser(std::move(refXmlModifierParser));
}
void XmlThemeParser::addXmlThAnimationFactoryParser(unique_ptr<XmlThAnimationFactoryParser> refXmlThAnimationFactoryParser)
{
	assert(refXmlThAnimationFactoryParser);
	m_oXmlThemeAniParser.addXmlThAnimationFactoryParser(std::move(refXmlThAnimationFactoryParser));
}
void XmlThemeParser::addXmlThWidgetFactoryParser(unique_ptr<XmlThWidgetFactoryParser> refXmlThWidgetFactoryParser)
{
	assert(refXmlThWidgetFactoryParser);
	m_oXmlThemeWidgetParser.addXmlThWidgetFactoryParser(std::move(refXmlThWidgetFactoryParser));
}

bool XmlThemeParser::parseXmlThemeInfo(ParserCtx& oCtx, const xmlpp::Element* p0RootElement
										, std::string& sThemeName, std::vector<std::string>& aExtendsThemes, ThemeLoader::ThemeInfo& oThemeInfo)
{
	oCtx.addChecker(p0RootElement);
	parseRoot(oCtx, p0RootElement, sThemeName);

	//
	const auto oPairTesting = XmlCommonParser::getAttributeValue(oCtx, p0RootElement, s_sXmlThemeTestingAttr);
	if (oPairTesting.first) {
		oThemeInfo.m_bTesting = XmlUtil::strToBool(oCtx, p0RootElement, s_sXmlThemeTestingAttr, oPairTesting.second);
	}

	const xmlpp::Element* p0DescEl = XmlCommonParser::parseUniqueElement(oCtx, p0RootElement, s_sXmlThemeDescriptionNodeName, true);
	const xmlpp::TextNode* p0DescText = p0DescEl->get_child_text();
	if (p0DescText == nullptr) {
		throw XmlCommonErrors::error(oCtx, p0DescEl, Util::s_sEmptyString, "Expected text in element '"
																		+ s_sXmlThemeDescriptionNodeName + "'");
	}
	oThemeInfo.m_sThemeDescription = p0DescText->get_content();

	bool bSupportsAppId = false;
	XmlCommonParser::visitNamedElementChildren(oCtx, p0RootElement, s_sXmlThemeSupportsNodeName, [&](const xmlpp::Element* p0SuppoEl)
	{
		const xmlpp::TextNode* p0SuppoText = p0SuppoEl->get_child_text();
		if (p0SuppoText == nullptr) {
			throw XmlCommonErrors::error(oCtx, p0SuppoEl, Util::s_sEmptyString, "Expected app id in element '"
																				+ s_sXmlThemeSupportsNodeName + "'");
		}
		const std::string& sAppId = Util::strStrip(p0SuppoText->get_content());
		if ((!bSupportsAppId) && (oCtx.appConfig()->getAppName() == sAppId)) {
			bSupportsAppId = true;
		}
	});
	aExtendsThemes.clear();
	XmlCommonParser::visitNamedElementChildren(oCtx, p0RootElement, s_sXmlThemeExtendsNodeName, [&](const xmlpp::Element* p0ExteEl)
	{
		const xmlpp::TextNode* p0ExteText = p0ExteEl->get_child_text();
		if (p0ExteText == nullptr) {
			throw XmlCommonErrors::error(oCtx, p0ExteEl, Util::s_sEmptyString, "Expected theme name in element '"
																				+ s_sXmlThemeExtendsNodeName + "'");
		}
		const std::string sExtendsTheme = p0ExteText->get_content();
		if (!sExtendsTheme.empty()) {
			aExtendsThemes.push_back(std::move(sExtendsTheme));
		}
	});
	// do not check child elements since not all are parsed here
	oCtx.removeChecker(p0RootElement, false, true);
	return bSupportsAppId;
}

void XmlThemeParser::parseXmlTheme(const std::vector< unique_ptr<ThemeCtx> >& aCtxs)
{
//std::cout << "XmlThemeParser::parseXmlTheme" << '\n';
	assert(!aCtxs.empty());

	std::string sThemeName;
	int32_t nThemeNr = 0;
	for (auto& refCtx : aCtxs) {
		assert(refCtx);
		const xmlpp::Element* p0RootElement = refCtx->m_p0RootElement;
		//
		refCtx->addChecker(p0RootElement);
		refCtx->addValidChildElementNames(p0RootElement, s_sXmlThemeDescriptionNodeName, s_sXmlThemeSupportsNodeName, s_sXmlThemeExtendsNodeName);
		refCtx->addValidAttrName(p0RootElement, s_sXmlThemeTestingAttr);
		//
		parseRoot(*refCtx, p0RootElement, sThemeName);
		assert(sThemeName == refCtx->m_sCtxThemeName);
		refCtx->m_nThemeNr = nThemeNr;
		++nThemeNr;
	}

	for (auto& refCtx : aCtxs) {
		const xmlpp::Element* p0Element = m_oXmlConditionalParser.parseUniqueElement(*refCtx, refCtx->m_p0RootElement, s_sColorsNodeName, false);
		if (p0Element != nullptr) {
			parseColors(*refCtx, p0Element);
		}
	}

	for (auto& refCtx : aCtxs) {
		const xmlpp::Element* p0Element = m_oXmlConditionalParser.parseUniqueElement(*refCtx, refCtx->m_p0RootElement, s_sFontsNodeName, false);
		if (p0Element != nullptr) {
			parseFonts(*refCtx, p0Element);
		}
	}

	for (auto& refCtx : aCtxs) {
		const xmlpp::Element* p0Element = m_oXmlConditionalParser.parseUniqueElement(*refCtx, refCtx->m_p0RootElement, s_sImageArraysNodeName, false);
		if (p0Element != nullptr) {
			m_oXmlThemeImagesParser.parseImageArrays(*refCtx, p0Element);
		}
	}

	for (auto& refCtx : aCtxs) {
		const xmlpp::Element* p0Element = m_oXmlConditionalParser.parseUniqueElement(*refCtx, refCtx->m_p0RootElement, s_sImagesNodeName, false);
		if (p0Element != nullptr) {
			m_oXmlThemeImagesParser.parseImages(*refCtx, p0Element);
		}
	}

	for (auto& refCtx : aCtxs) {
		const xmlpp::Element* p0Element = m_oXmlConditionalParser.parseUniqueElement(*refCtx, refCtx->m_p0RootElement, s_sSoundsNodeName, false);
		if (p0Element != nullptr) {
			m_oXmlThemeSoundsParser.parseSounds(*refCtx, p0Element);
		}
	}

	for (auto& refCtx : aCtxs) {
		const xmlpp::Element* p0Element = m_oXmlConditionalParser.parseUniqueElement(*refCtx, refCtx->m_p0RootElement, s_sAnimationsNodeName, false);
		if (p0Element != nullptr) {
			m_oXmlThemeAniParser.parseAnimations(*refCtx, p0Element);
		}
	}

	for (auto& refCtx : aCtxs) {
		const xmlpp::Element* p0Element = m_oXmlConditionalParser.parseUniqueElement(*refCtx, refCtx->m_p0RootElement, s_sTileAnisNodeName, false);
		if (p0Element != nullptr) {
			m_oXmlThemeAniParser.parseTileAnis(*refCtx, p0Element);
		}
	}

	for (auto& refCtx : aCtxs) {
		const xmlpp::Element* p0Element = m_oXmlConditionalParser.parseUniqueElement(*refCtx, refCtx->m_p0RootElement, s_sWidgetsNodeName, false);
		if (p0Element != nullptr) {
			m_oXmlThemeWidgetParser.parseTWidgetFactories(*refCtx, p0Element);
		}
	}

	for (auto& refCtx : aCtxs) {
		const xmlpp::Element* p0Element = m_oXmlConditionalParser.parseUniqueElement(*refCtx, refCtx->m_p0RootElement, s_sAssignsNodeName, false);
		if (p0Element != nullptr) {
			m_oXmlThemeAssignParser.parseAssigns(*refCtx, p0Element);
		}
	}

	for (auto& refCtx : aCtxs) {
		const xmlpp::Element* p0Element = m_oXmlConditionalParser.parseUniqueElement(*refCtx, refCtx->m_p0RootElement, s_sPaintersNodeName, false);
		if (p0Element != nullptr) {
			parsePainters(*refCtx, p0Element);
		}
	}

	auto& refManiCtx = aCtxs[0];
	StdTheme& oStdTheme = refManiCtx->theme();
	const int32_t nDefaultPainterIdx = oStdTheme.getDefaultPainterIdx();
	if (nDefaultPainterIdx < 0) {
		const xmlpp::Element* p0Element = m_oXmlConditionalParser.parseUniqueElement(*refManiCtx, refManiCtx->m_p0RootElement, s_sPaintersNodeName, true);
		throw XmlCommonErrors::errorElementExpected(*refManiCtx, p0Element, s_sPaintersPainterNodeName);
	}

	Named& oNamed = refManiCtx->named();
	NamedIndex& oPaintersIndex = oNamed.painters();
	if (oPaintersIndex.getIndex(s_sPaintersPainterNameAttrBoard) < 0) {
		std::cout << "Board painter '" << s_sPaintersPainterNameAttrBoard << "' not explicitely defined" << '\n';
		std::cout << " -> using default painter '" << oPaintersIndex.getName(nDefaultPainterIdx) << "'!" << '\n';
	}
	if (oPaintersIndex.getIndex(s_sPaintersPainterNameAttrBlock) < 0) {
		std::cout << "Block painter '" << s_sPaintersPainterNameAttrBlock << "' not explicitely defined" << '\n';
		std::cout << " -> using default painter '" << oPaintersIndex.getName(nDefaultPainterIdx) << "'!" << '\n';
	}

	for (auto& refCtx : aCtxs) {
		refCtx->removeChecker(refCtx->m_p0RootElement, true);
	}
}
void XmlThemeParser::parsePainters(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sPaintersPainterNodeName, [&](const xmlpp::Element* p0Painter)
	{
		parsePaintersPainter(oCtx, p0Painter);
	});
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sPaintersClonePainterNodeName, [&](const xmlpp::Element* p0ClonePainter)
	{
		parsePaintersClonePainter(oCtx, p0ClonePainter);
	});
	oCtx.removeChecker(p0Element, true);
}
void XmlThemeParser::parsePaintersPainter(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);

	StdTheme& oTheme = oCtx.theme();
	const auto oPairPainterName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sPaintersPainterNameAttr);
	const bool bNameDefined = oPairPainterName.first;
	if (! bNameDefined) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sPaintersPainterNameAttr);
	}
	const std::string& sPainterName = oPairPainterName.second;
	XmlCommonParser::validateName(oCtx, p0Element, s_sPaintersPainterNameAttr, sPainterName, false);

	if (oCtx.localThemePainterNames().getIndex(sPainterName) >= 0) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sPaintersPainterNameAttr
									, Util::stringCompose("A painter with name '%1' is already defined in this file"
														, sPainterName));
	}
	oCtx.localThemePainterNames().addNamedObj(sPainterName, p0Element);

	std::vector< unique_ptr<StdThemeModifier> > aThemeModifiers = m_oXmlThemeModifiersParser.parseModifiers(oCtx, p0Element);

	const int32_t nPainterIdx = oTheme.addPainter(sPainterName, std::move(aThemeModifiers));
	//
	bool bDefaultPainter = false;
	const auto oPairDefault = XmlCommonParser::getAttributeValue(oCtx, p0Element, s_sPaintersPainterDefaultAttr);
	if (oPairDefault.first) {
		bDefaultPainter = XmlUtil::strToBool(oCtx, p0Element, s_sPaintersPainterDefaultAttr, oPairDefault.second);
	}
	if (bDefaultPainter) {
		oTheme.setDefaultPainter(nPainterIdx);
	}

	oCtx.removeChecker(p0Element, false, true);
}
void XmlThemeParser::parsePaintersClonePainter(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlThemeParser::parsePaintersClonePainter " << oCtx.error("").what() << '\n';
	oCtx.addChecker(p0Element);

	StdTheme& oTheme = oCtx.theme();

	const auto oPairPainterName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sPaintersClonePainterNameAttr);
	const bool bNameDefined = oPairPainterName.first;
	if (! bNameDefined) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sPaintersClonePainterNameAttr);
	}
	const std::string& sPainterName = oPairPainterName.second;
	XmlCommonParser::validateName(oCtx, p0Element, s_sPaintersClonePainterNameAttr, sPainterName, false);

	if (oCtx.localThemePainterNames().getIndex(sPainterName) >= 0) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sPaintersClonePainterNameAttr
									, Util::stringCompose("A painter with name '%1' is already defined in this file"
														, sPainterName));
	}

	const auto oPairCloningPainterName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sPaintersClonePainterCloningNameAttr);
	const bool bCloningNameDefined = oPairCloningPainterName.first;
	if (! bCloningNameDefined) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sPaintersClonePainterCloningNameAttr);
	}
	const std::string& sCloningPainterName = oPairCloningPainterName.second;

	const int32_t nCloningIdx = oCtx.localThemePainterNames().getIndex(sCloningPainterName);
	if (nCloningIdx < 0) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sPaintersClonePainterCloningNameAttr
									, Util::stringCompose("A local painter with name '%1' was not defined in this file"
														, sCloningPainterName));
	}
	const xmlpp::Element* p0CloningElement = oCtx.localThemePainterNames().getObj(nCloningIdx);
	std::vector< unique_ptr<StdThemeModifier> > aThemeModifiers = m_oXmlThemeModifiersParser.parseModifiers(oCtx, p0CloningElement);

	oTheme.addPainter(sPainterName, std::move(aThemeModifiers));

	oCtx.localThemePainterNames().addNamedObj(sPainterName, p0CloningElement);

	oCtx.removeChecker(p0Element, true);
}

void XmlThemeParser::parseRoot(ParserCtx& oCtx, const xmlpp::Element* p0RootElement, std::string& sThemeName)
{
//std::cout << "XmlThemeParser::parseRoot" << '\n';
	assert(p0RootElement != nullptr);
	if (! (p0RootElement->get_name() == s_sXmlThemeNodeName)) {
		throw XmlCommonErrors::error(oCtx, p0RootElement, Util::s_sEmptyString, Util::stringCompose("Root element must be %1", s_sXmlThemeNodeName));
	}
	const auto oPairThemeName = XmlCommonParser::getAttributeValue(oCtx, p0RootElement, s_sXmlThemeInternalNameAttr);
	if (!oPairThemeName.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0RootElement, s_sXmlThemeInternalNameAttr);
	}
	sThemeName = oPairThemeName.second;
	if (sThemeName.empty()) {
		throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0RootElement, s_sXmlThemeInternalNameAttr);
	}
}

void XmlThemeParser::parseColors(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	m_oXmlConditionalParser.visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0Color)
	{
		const std::string sName = p0Color->get_name();
		if (sName == s_sColorsColorNodeName) {
			parseColorsColor(oCtx, p0Color);
		} else if (sName == s_sColorsStdColorsNodeName) {
			parseColorsStdColors(oCtx, p0Color);
		} else {
			throw XmlCommonErrors::errorElementInvalid(oCtx, p0Element, sName);
		}
	});
	oCtx.removeChecker(p0Element, false, true);
}
void XmlThemeParser::parseColorsColor(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlThemeParser::parseColorsColor()" << '\n';
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	std::string sColorName;
	const auto oPairColorName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sColorsColorNameAttr);
	const bool bNameDefined = oPairColorName.first;
	if (bNameDefined) {
		sColorName = std::move(oPairColorName.second);
		XmlCommonParser::validateName(oCtx, p0Element, s_sColorsColorNameAttr, sColorName, false);
	}
	int32_t nColorPal = -1;
	const auto oPairColorPal = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sColorsColorPalAttr);
	const bool bPalDefined = oPairColorPal.first;
	if (bPalDefined) {
		const std::string& sColorPal = oPairColorPal.second;
		nColorPal = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sColorsColorPalAttr, sColorPal, false
														, true, TileColor::COLOR_PAL_FIRST, true, TileColor::COLOR_PAL_LAST);
	}
	if ((!bPalDefined) && !bNameDefined) {
		throw XmlCommonErrors::errorAttrEitherMustBeDefined(oCtx, p0Element, s_sColorsColorNameAttr, s_sColorsColorPalAttr);
	}

	const auto oPairColorRgb = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sColorsColorRgbAttr);
	if (!oPairColorRgb.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sColorsColorRgbAttr);
	}
	const std::string& sColorRgb = oPairColorRgb.second;
	const RgbColor oRgb = XmlUtil::strHexToRGB(oCtx, p0Element, s_sColorsColorRgbAttr, sColorRgb);

	if (bPalDefined) {
		oTheme.addColorPal(nColorPal, oRgb.m_nR, oRgb.m_nG, oRgb.m_nB);
	}
	if (bNameDefined) {
		oTheme.addColorName(sColorName, oRgb.m_nR, oRgb.m_nG, oRgb.m_nB);
	}
	oCtx.removeChecker(p0Element, true);
}
void XmlThemeParser::parseColorsStdColors(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlThemeParser::parseColorsStdColors()" << '\n';
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	const int32_t nTotSvgColors = CommonTheme::getTotSvgColors();
	for (int32_t nPal = 0; nPal < nTotSvgColors; ++nPal) {
		const CommonTheme::NamedRgbColor oNamedColor = CommonTheme::getSvgColor(nPal);
		const RgbColor& oColor = oNamedColor.m_oColor;
		oTheme.addColorPal(nPal, oColor.m_nR, oColor.m_nG, oColor.m_nB);
		oTheme.addColorName(oNamedColor.m_sName, oColor.m_nR, oColor.m_nG, oColor.m_nB);
	}
	oCtx.removeChecker(p0Element, true);
}

void XmlThemeParser::parseFonts(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sFontsFontNodeName, [&](const xmlpp::Element* p0Font)
	{
		parseFontsFont(oCtx, p0Font, false);
	});
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sFontsDefaultNodeName, [&](const xmlpp::Element* p0Font)
	{
		parseFontsFont(oCtx, p0Font, true);
	});
	oCtx.removeChecker(p0Element, true);
}
void XmlThemeParser::parseFontsFont(ThemeCtx& oCtx, const xmlpp::Element* p0Element, bool bDefault)
{
//std::cout << "XmlThemeParser::parseFontsFont()" << '\n';
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	std::string sFontName;
	if (!bDefault) {
		const auto oPairFontName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sFontsFontNameAttr);
		if (!oPairFontName.first) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sFontsFontNameAttr);
		}
		sFontName = std::move(oPairFontName.second);
		XmlCommonParser::validateName(oCtx, p0Element, s_sFontsFontNameAttr, sFontName, false);
	}
	const auto oPairFontDefine = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sFontsFontDefineAttr);
	if (! oPairFontDefine.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sFontsFontDefineAttr);
	}
	const std::string& sFontDefine = oPairFontDefine.second;
	if (bDefault) {
		oTheme.setDefaultFont(sFontDefine);
	} else {
		oTheme.addFontName(sFontName, sFontDefine);
	}
	oCtx.removeChecker(p0Element, true);
}

//void XmlThemeParser::parseDrawTile(ThemeCtx& oCtx, const xmlpp::Element* p0Element, std::vector< unique_ptr<StdThemeModifier> >& aModifiers)
//{
//	oCtx.addChecker(p0Element);
//	std::vector< unique_ptr<StdThemeModifier> > aThemeModifiers = m_oXmlThemeModifiersParser.parseModifiers(oCtx, p0Element);
//	bool bHasNextThemeModifier = false;
//	for (auto& refModifer : aThemeModifiers) {
//		if (dynamic_cast<stmg::NextThemeModifier*>(refModifer.get()) != nullptr) {
//			bHasNextThemeModifier = true;
//			break;
//		}
//	}
//	std::move(aThemeModifiers.begin(), aThemeModifiers.end(), std::back_inserter(aModifiers));
//	aModifiers.emplace_back();
//	// parseModifiers already checks the validity of child elements
//	oCtx.removeChecker(p0Element, false, true);
//	return bHasNextThemeModifier;
//}

} // namespace stmg
