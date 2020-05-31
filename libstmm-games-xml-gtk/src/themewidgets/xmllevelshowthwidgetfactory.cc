/*
 * File:   xmllevelshowthwidgetfactory.cc
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

#include "themewidgets/xmllevelshowthwidgetfactory.h"

#include "themectx.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "xmlutil/xmlstrconv.h"

#include <stmm-games-gtk/widgets/levelshowthwidgetfactory.h>
#include <stmm-games-gtk/gtkutil/frame.h>
#include <stmm-games-gtk/stdthemewidgetfactory.h>

//#include <cassert>
//#include <iostream>
#include <string>
#include <utility>

namespace stmg { class StdTheme; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sTWidgetsFactoryLevelShowNodeName = "LevelShowFactory";
static const std::string s_sTWidgetsFactoryLevelShowFrameNodeName = "Frame";
static const std::string s_sTWidgetsFactoryLevelShowMinTopAttr = "minTop";
static const std::string s_sTWidgetsFactoryLevelShowMinBottomAttr = "minBottom";
static const std::string s_sTWidgetsFactoryLevelShowMinLeftAttr = "minLeft";
static const std::string s_sTWidgetsFactoryLevelShowMinRightAttr = "minRight";

XmlLevelShowThWidgetFactoryParser::XmlLevelShowThWidgetFactoryParser()
: XmlThWidgetFactoryParser(s_sTWidgetsFactoryLevelShowNodeName)
{
}
unique_ptr<StdThemeWidgetFactory> XmlLevelShowThWidgetFactoryParser::parseWidgetFactory(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
//std::cout << "XmlLevelShowThWidgetFactoryParser::parseWidgetFactory" << '\n';
	Frame oFrame;
	auto p0FrameEl = getXmlConditionalParser()->parseUniqueElement(oCtx, p0Element, s_sTWidgetsFactoryLevelShowFrameNodeName, false);
	if (p0FrameEl != nullptr) {
		oFrame = parseLayoutFrame(oCtx, p0FrameEl);
//std::cout << "XmlLevelShowThWidgetFactoryParser::parseWidgetFactory 1" << '\n';
	}
//std::cout << "XmlLevelShowThWidgetFactoryParser::parseWidgetFactory 2" << '\n';
	double fMinTop = 0;
	const auto oPairMinTop = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryLevelShowMinTopAttr);
	if (oPairMinTop.first) {
		const std::string& sMinTop = oPairMinTop.second;
		fMinTop = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sTWidgetsFactoryLevelShowMinTopAttr, sMinTop, true
														, true, 0.0, false, -1);
	}
	double fMinBottom = 0;
	const auto oPairMinBottom = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryLevelShowMinBottomAttr);
	if (oPairMinBottom.first) {
		const std::string& sMinBottom = oPairMinBottom.second;
		fMinBottom = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sTWidgetsFactoryLevelShowMinBottomAttr, sMinBottom, true
														, true, 0.0, false, -1);
	}
	double fMinLeft = 0;
	const auto oPairMinLeft = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryLevelShowMinLeftAttr);
	if (oPairMinLeft.first) {
		const std::string& sMinLeft = oPairMinLeft.second;
		fMinLeft = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sTWidgetsFactoryLevelShowMinLeftAttr, sMinLeft, true
														, true, 0.0, false, -1);
	}
	double fMinRight = 0;
	const auto oPairMinRight = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryLevelShowMinRightAttr);
	if (oPairMinRight.first) {
		const std::string& sMinRight = oPairMinRight.second;
		fMinRight = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sTWidgetsFactoryLevelShowMinRightAttr, sMinRight, true
														, true, 0.0, false, -1);
	}
	oCtx.removeChecker(p0Element, true);
#ifndef NDEBUG
//std::cout << "XmlLevelShowThWidgetFactoryParser::parseWidgetFactory 3  oCtx DUMP " << '\n';  oCtx.dump();
#endif //NDEBUG
	return std::make_unique<LevelShowThWidgetFactory>(&oTheme, oFrame, fMinTop, fMinBottom, fMinLeft, fMinRight);
}

} // namespace stmg
