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
 * File:   xmlthemewidgetparser.cc
 */

#include "xmlthemewidgetparser.h"
#include "xmlthemeimageparser.h"
#include "themectx.h"

#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>

#include <stmm-games-gtk/stdtheme.h>
#include <stmm-games-gtk/stdthemewidgetfactory.h>
#include <stmm-games-gtk/gtkutil/frame.h>
#include <stmm-games-gtk/gtkutil/tilesizing.h>

#include <glibmm/ustring.h>

#include <libxml++/libxml++.h>

#include <vector>
#include <cassert>
//#include <iostream>
#include <cstdint>
#include <utility>

#include <stdint.h>

namespace stmg { class Image; }
namespace stmg { class XmlTraitsParser; }

namespace stmg
{

static const std::string s_sTWidgetsFactoryNameAttr = "name";
static const std::string s_sTWidgetsFactoryAnonymousAttr = "anonymousWidgets";

//const std::string XmlThemeWidgetParser::s_sTWidgetsFactoryFrameNodeName = "Frame";
	static const std::string s_sTWidgetsFactoryFrameLeftAttr = "left";
	static const std::string s_sTWidgetsFactoryFrameRightAttr = "right";
	static const std::string s_sTWidgetsFactoryFrameTopAttr = "top";
	static const std::string s_sTWidgetsFactoryFrameBottomAttr = "bottom";
	static const std::string s_sTWidgetsFactoryFrameBaseImgNodeName = "BaseImg";
	static const std::string s_sTWidgetsFactoryFrameTopLeftImgNodeName = "TopLeftImg";
	static const std::string s_sTWidgetsFactoryFrameTopImgNodeName = "TopImg";
	static const std::string s_sTWidgetsFactoryFrameTopRightImgNodeName = "TopRightImg";
	static const std::string s_sTWidgetsFactoryFrameLeftImgNodeName = "LeftImg";
	static const std::string s_sTWidgetsFactoryFrameCenterImgNodeName = "CenterImg";
	static const std::string s_sTWidgetsFactoryFrameRightImgNodeName = "RightImg";
	static const std::string s_sTWidgetsFactoryFrameBottomLeftImgNodeName = "BottomLeftImg";
	static const std::string s_sTWidgetsFactoryFrameBottomImgNodeName = "BottomImg";
	static const std::string s_sTWidgetsFactoryFrameBottomRightImgNodeName = "BottomRightImg";

//const std::string XmlThemeWidgetParser::s_sTWidgetsFactoryTileSizingNodeName = "TileSize";
	static const std::string s_sTWidgetsFactoryTileSizingMinWAttr = "minW";
	static const std::string s_sTWidgetsFactoryTileSizingMaxWAttr = "maxW";
	static const std::string s_sTWidgetsFactoryTileSizingMinHAttr = "minH";
	static const std::string s_sTWidgetsFactoryTileSizingMaxHAttr = "maxH";
	static const std::string s_sTWidgetsFactoryTileSizingWHRatioAttr = "whRatio";

XmlThemeWidgetParser::XmlThemeWidgetParser(XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser
											, XmlThemeImageParser& oXmlThemeImageParser)
: m_oXmlConditionalParser(oXmlConditionalParser)
, m_oXmlTraitsParser(oXmlTraitsParser)
, m_oXmlThemeImageParser(oXmlThemeImageParser)
{
}
void XmlThemeWidgetParser::addXmlThWidgetFactoryParser(unique_ptr<XmlThWidgetFactoryParser> refWidgetFactoriesParser)
{
	assert(refWidgetFactoriesParser);
	assert(refWidgetFactoriesParser->m_p1Owner == nullptr);
	refWidgetFactoriesParser->m_p1Owner = this;
	m_aXmlThWidgetFactoryParsers.push_back(std::move(refWidgetFactoriesParser));
}

void XmlThemeWidgetParser::parseTWidgetFactories(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	m_oXmlConditionalParser.visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0WidgetElement)
	{
		parseTWidgetsFactory(oCtx, p0WidgetElement);
	});
	// child validity checked in parseTWidgetsFactory
	oCtx.removeChecker(p0Element, false, true);
}
void XmlThemeWidgetParser::parseTWidgetsFactory(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlThemeWidgetParser::parseTWidgetsFactory 0" << '\n';
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	const std::string sWidgetFactoryClassName = p0Element->get_name();

	const auto oPairWidgetName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryNameAttr);
//std::cout << "XmlThemeWidgetParser::parseTWidgetsFactory sWidgetFactoryClassName=" << sWidgetFactoryClassName << '\n';
	if (! oPairWidgetName.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sTWidgetsFactoryNameAttr);
	}
	const std::string& sWidgetName = oPairWidgetName.second;
	if (sWidgetName.empty()) {
		throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, s_sTWidgetsFactoryNameAttr);
	}
	if (oCtx.isMain() && oTheme.hasWidgetFactory(sWidgetName)) {
		throw XmlCommonErrors::errorAttrWithValueAlreadyDefined(oCtx, p0Element, s_sTWidgetsFactoryNameAttr, sWidgetName);
	}
//std::cout << "XmlThemeWidgetParser::parseTWidgetsFactory sWidgetName=" << sWidgetName << '\n';
	bool bAnonymousWidgets = true;
	const auto oPairAnonymous = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryAnonymousAttr);
	const bool bAnonymousDefined = oPairAnonymous.first;
	if (bAnonymousDefined) {
		const std::string& sAnonymous = oPairAnonymous.second;
		bAnonymousWidgets = XmlUtil::strToBool(oCtx, p0Element, s_sTWidgetsFactoryAnonymousAttr, sAnonymous);
	}

	unique_ptr<StdThemeWidgetFactory> refFactory;
	for (auto& refParser : m_aXmlThWidgetFactoryParsers) {
		if (refParser->getFactoryName() == sWidgetFactoryClassName) {
			refFactory = refParser->parseWidgetFactory(oCtx, p0Element);
			if (refFactory) {
				break; // for (refParser) ---
			}
		}
	}
	if (!refFactory) {
		throw XmlCommonErrors::errorElementInvalid(oCtx, p0Element, sWidgetFactoryClassName);
	}
	oCtx.removeChecker(p0Element, true);
	oTheme.addWidgetFactory(sWidgetName, std::move(refFactory), bAnonymousWidgets);
}
Frame XmlThemeWidgetParser::parseLayoutFrame(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();

	Frame oFrame;
	const auto oPairLeft = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryFrameLeftAttr);
	if (oPairLeft.first) {
		const std::string& sLeft = oPairLeft.second;
		oFrame.m_nPixMinLeft = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sTWidgetsFactoryFrameLeftAttr, sLeft, false, true, 0, false, -1);
	}
	const auto oPairRight = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryFrameRightAttr);
	if (oPairRight.first) {
		const std::string& sRight = oPairRight.second;
		oFrame.m_nPixMinRight = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sTWidgetsFactoryFrameRightAttr, sRight, false, true, 0, false, -1);
	}
	const auto oPairTop = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryFrameTopAttr);
	if (oPairTop.first) {
		const std::string& sTop = oPairTop.second;
		oFrame.m_nPixMinTop = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sTWidgetsFactoryFrameTopAttr, sTop, false, true, 0, false, -1);
	}
	const auto oPairBottom = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryFrameBottomAttr);
	if (oPairBottom.first) {
		const std::string& sBottom = oPairBottom.second;
		oFrame.m_nPixMinBottom = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sTWidgetsFactoryFrameBottomAttr, sBottom, false, true, 0, false, -1);
	}
	parseCommonImage(oCtx, p0Element, s_sTWidgetsFactoryFrameBaseImgNodeName, oTheme, oFrame.m_refBaseImg);
	parseCommonImage(oCtx, p0Element, s_sTWidgetsFactoryFrameTopLeftImgNodeName, oTheme, oFrame.m_refTopLeftImg);
	parseCommonImage(oCtx, p0Element, s_sTWidgetsFactoryFrameTopImgNodeName, oTheme, oFrame.m_refTopImg);
	parseCommonImage(oCtx, p0Element, s_sTWidgetsFactoryFrameTopRightImgNodeName, oTheme, oFrame.m_refTopRightImg);
	parseCommonImage(oCtx, p0Element, s_sTWidgetsFactoryFrameLeftImgNodeName, oTheme, oFrame.m_refLeftImg);
	parseCommonImage(oCtx, p0Element, s_sTWidgetsFactoryFrameCenterImgNodeName, oTheme, oFrame.m_refCenterImg);
	parseCommonImage(oCtx, p0Element, s_sTWidgetsFactoryFrameRightImgNodeName, oTheme, oFrame.m_refRightImg);
	parseCommonImage(oCtx, p0Element, s_sTWidgetsFactoryFrameBottomLeftImgNodeName, oTheme, oFrame.m_refBottomLeftImg);
	parseCommonImage(oCtx, p0Element, s_sTWidgetsFactoryFrameBottomImgNodeName, oTheme, oFrame.m_refBottomImg);
	parseCommonImage(oCtx, p0Element, s_sTWidgetsFactoryFrameBottomRightImgNodeName, oTheme, oFrame.m_refBottomRightImg);
	//
	oCtx.removeChecker(p0Element, true);
	return oFrame;
}
void XmlThemeWidgetParser::parseCommonImage(ThemeCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sElementName
							, StdTheme& oTheme, shared_ptr<Image>& m_refImg)
{
	auto p0ImgEl = m_oXmlConditionalParser.parseUniqueElement(oCtx, p0Element, sElementName, false);
	if (p0ImgEl != nullptr) {
		oCtx.addChecker(p0ImgEl);
		const auto oPairImg = m_oXmlThemeImageParser.parseImage(oCtx, p0ImgEl, true);
		const std::string& sImgFile = oPairImg.second;
		m_refImg = oTheme.getImageByFileName(sImgFile);
		oCtx.removeChecker(p0ImgEl, true);
	}
}
TileSizing XmlThemeWidgetParser::parseLayoutTileSizing(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	TileSizing oTileSizing;
	int32_t& nMinW = oTileSizing.m_nMinW;
	int32_t& nMaxW = oTileSizing.m_nMaxW;
	int32_t& nMinH = oTileSizing.m_nMinH;
	int32_t& nMaxH = oTileSizing.m_nMaxH;
	double& fWHRatio = oTileSizing.m_fWHRatio;
	const auto oPairMinW = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryTileSizingMinWAttr);
	if (oPairMinW.first) {
		const std::string& sMinW = oPairMinW.second;
		nMinW = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sTWidgetsFactoryTileSizingMinWAttr, sMinW, false, true, 1, true, 1000);
	}
	const auto oPairMaxW = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryTileSizingMaxWAttr);
	if (oPairMaxW.first) {
		const std::string& sMaxW = oPairMaxW.second;
		nMaxW = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sTWidgetsFactoryTileSizingMaxWAttr, sMaxW, false, true, nMinW, true, 1000);
	}
	const auto oPairMinH = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryTileSizingMinHAttr);
	if (oPairMinH.first) {
		const std::string& sMinH = oPairMinH.second;
		nMinH = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sTWidgetsFactoryTileSizingMinHAttr, sMinH, false, true, 1, true, 1000);
	}
	const auto oPairMaxH = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryTileSizingMaxHAttr);
	if (oPairMaxH.first) {
		const std::string& sMaxH = oPairMaxH.second;
		nMaxH = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sTWidgetsFactoryTileSizingMaxHAttr, sMaxH, false, true, nMinH, true, 1000);
	}
	const auto oPairWHRatio = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryTileSizingWHRatioAttr);
	if (oPairWHRatio.first) {
		const std::string& sWHRatio = oPairWHRatio.second;
		fWHRatio = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sTWidgetsFactoryTileSizingWHRatioAttr, sWHRatio, true
														, true, 0.001, true, 1000.0);
	}
	oCtx.removeChecker(p0Element, true);
	return oTileSizing;
}

} // namespace stmg
