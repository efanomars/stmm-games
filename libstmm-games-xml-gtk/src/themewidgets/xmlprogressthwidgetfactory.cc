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
 * File:   xmlprogressthwidgetfactory.cc
 */

#include "themewidgets/xmlprogressthwidgetfactory.h"

#include "themectx.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmltraitsparser.h>

#include <stmm-games-gtk/widgets/progressthwidgetfactory.h>
#include <stmm-games-gtk/stdthemewidgetfactory.h>
#include <stmm-games-gtk/gtkutil/frame.h>

#include <stmm-games/tile.h>

//#include <cassert>
//#include <iostream>
#include <string>

namespace stmg { class StdTheme; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sTWidgetsFactoryProgressNodeName = "ProgressFactory";
static const std::string s_sTWidgetsFactoryProgressNormalNodeName = "Normal";
static const std::string s_sTWidgetsFactoryProgressDangerNodeName = "Danger";
static const std::string s_sTWidgetsFactoryProgressFrameNodeName = "Frame";

XmlProgressThWidgetFactoryParser::XmlProgressThWidgetFactoryParser()
: XmlThWidgetFactoryParser(s_sTWidgetsFactoryProgressNodeName)
{
}
unique_ptr<StdThemeWidgetFactory> XmlProgressThWidgetFactoryParser::parseWidgetFactory(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	TileColor oNormalColor;
	auto p0NormalEl = getXmlConditionalParser()->parseUniqueElement(oCtx, p0Element, s_sTWidgetsFactoryProgressNormalNodeName, false);
	if (p0NormalEl != nullptr) {
		oCtx.addChecker(p0NormalEl);
		getXmlTraitsParser()->parseColor(oCtx, p0NormalEl, oNormalColor);
		oCtx.removeChecker(p0NormalEl, true);
	}
	TileColor oDangerColor;
	auto p0DangerEl = getXmlConditionalParser()->parseUniqueElement(oCtx, p0Element, s_sTWidgetsFactoryProgressDangerNodeName, false);
	if (p0DangerEl != nullptr) {
		oCtx.addChecker(p0DangerEl);
		getXmlTraitsParser()->parseColor(oCtx, p0DangerEl, oDangerColor);
		oCtx.removeChecker(p0DangerEl, true);
	}
	Frame oFrame;
	auto p0FrameEl = getXmlConditionalParser()->parseUniqueElement(oCtx, p0Element, s_sTWidgetsFactoryProgressFrameNodeName, false);
	if (p0FrameEl != nullptr) {
		oFrame = parseLayoutFrame(oCtx, p0FrameEl);
	}
	oCtx.removeChecker(p0Element, true);
	//
	return std::make_unique<ProgressThWidgetFactory>(&oTheme, oNormalColor, oDangerColor, oFrame);
}

} // namespace stmg
