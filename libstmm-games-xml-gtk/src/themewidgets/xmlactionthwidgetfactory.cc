/*
 * File:   xmlactionthwidgetfactory.cc
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

#include "themewidgets/xmlactionthwidgetfactory.h"

#include "themectx.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmltraitsparser.h>

#include <stmm-games-gtk/widgets/actionthwidgetfactory.h>
#include <stmm-games-gtk/gtkutil/frame.h>
#include <stmm-games-gtk/stdthemewidgetfactory.h>

#include <stmm-games/tile.h>

//#include <cassert>
//#include <iostream>
#include <string>

namespace stmg { class StdTheme; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sTWidgetsFactoryActionNodeName = "ActionFactory";
	//static const std::string s_sTWidgetsFactoryKeyActionColorAttr = "color";
	//static const std::string s_sTWidgetsFactoryKeyActionAlphaAttr = "alpha";
	//static const std::string s_sTWidgetsFactoryKeyActionFont Attr= "font";
	//static const std::string s_sTWidgetsFactoryKeyActionBgImgAttr = "imgFile";
	static const std::string s_sTWidgetsFactoryActionFrameNodeName = "Frame";
	//static const std::string s_sTWidgetsFactoryActionTextToIconTextAttr = "text";
	//static const std::string s_sTWidgetsFactoryKeyActionTextToIconImgAttr = "imgFile";

XmlActionThWidgetFactoryParser::XmlActionThWidgetFactoryParser()
: XmlThWidgetFactoryParser(s_sTWidgetsFactoryActionNodeName)
{
}
unique_ptr<StdThemeWidgetFactory> XmlActionThWidgetFactoryParser::parseWidgetFactory(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	TileColor oTextColor;
	TileAlpha oTextAlpha;
	TileFont oTextFont;
	getXmlTraitsParser()->parseColor(oCtx, p0Element, oTextColor);
	getXmlTraitsParser()->parseAlpha(oCtx, p0Element, oTextAlpha);
	getXmlTraitsParser()->parseFont(oCtx, p0Element, oTextFont);
	Frame oFrame;
	auto p0FrameEl = getXmlConditionalParser()->parseUniqueElement(oCtx, p0Element, s_sTWidgetsFactoryActionFrameNodeName, false);
	if (p0FrameEl != nullptr) {
		oFrame = parseLayoutFrame(oCtx, p0FrameEl);
	}
	oCtx.removeChecker(p0Element, true);
	return std::make_unique<ActionThWidgetFactory>(&oTheme, oTextColor, oTextAlpha, oTextFont, oFrame);
}

} // namespace stmg
