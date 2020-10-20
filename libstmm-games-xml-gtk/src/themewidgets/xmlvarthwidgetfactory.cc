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
 * File:   xmlvarthwidgetfactory.cc
 */

#include "themewidgets/xmlvarthwidgetfactory.h"

#include "xmlthemeimageparser.h"
#include "themectx.h"

#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmltraitsparser.h>

#include <stmm-games-gtk/widgets/varthwidgetfactory.h>
#include <stmm-games-gtk/stdtheme.h>
#include <stmm-games-gtk/stdthemewidgetfactory.h>
#include <stmm-games-gtk/gtkutil/frame.h>

#include <stmm-games/tile.h>

//#include <cassert>
//#include <iostream>
#include <string>
#include <utility>

namespace stmg { class Image; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sTWidgetsFactoryVarNodeName = "VarFactory";
static const std::string s_sTWidgetsFactoryVarTitlePreValueAttr = "titlePreValue";
static const std::string s_sTWidgetsFactoryVarTitleNodeName = "Title";
	//static const std::string s_sTWidgetsFactoryVarTitleColorAttr = "color";
	//static const std::string s_sTWidgetsFactoryVarTitleAlphaAttr = "alpha";
	//static const std::string s_sTWidgetsFactoryVarTitleFontAttr = "font";
	//static const std::string s_sTWidgetsFactoryVarTitleBgImg = "imgFile";
static const std::string s_sTWidgetsFactoryVarValueNodeName = "Value";
	//static const std::string s_sTWidgetsFactoryVarValueColorAttr = "color";
	//static const std::string s_sTWidgetsFactoryVarValueAlphaAttr = "alpha";
	//static const std::string s_sTWidgetsFactoryVarValueFontAttr = "font";
	//static const std::string s_sTWidgetsFactoryVarValueBgImgAttr = "imgFile";
static const std::string s_sTWidgetsFactoryVarFrameNodeName = "Frame";

XmlVarThWidgetFactoryParser::XmlVarThWidgetFactoryParser()
: XmlThWidgetFactoryParser(s_sTWidgetsFactoryVarNodeName)
{
}
unique_ptr<StdThemeWidgetFactory> XmlVarThWidgetFactoryParser::parseWidgetFactory(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	bool bTitlePreValue = true;
	const auto oPairTitlePreValue = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sTWidgetsFactoryVarTitlePreValueAttr);
	if (oPairTitlePreValue.first) {
		const std::string& sTitlePreValue = oPairTitlePreValue.second;
		bTitlePreValue = XmlUtil::strToBool(oCtx, p0Element, s_sTWidgetsFactoryVarTitlePreValueAttr, sTitlePreValue);
	}
	TileColor oTitleColor;
	TileAlpha oTitleAlpha;
	TileFont oTitleFont;
	shared_ptr<Image> refTitleBgImg;
	auto p0TitleEl = getXmlConditionalParser()->parseUniqueElement(oCtx, p0Element, s_sTWidgetsFactoryVarTitleNodeName, false);
	if (p0TitleEl != nullptr) {
		oCtx.addChecker(p0TitleEl);
		getXmlTraitsParser()->parseColor(oCtx, p0TitleEl, oTitleColor);
		getXmlTraitsParser()->parseAlpha(oCtx, p0TitleEl, oTitleAlpha);
		getXmlTraitsParser()->parseFont(oCtx, p0TitleEl, oTitleFont);
		const auto oPairImgFile = getXmlThemeImageParser()->parseImage(oCtx, p0TitleEl, false);
		const std::string& sImgAttr = oPairImgFile.first;
		if (! sImgAttr.empty()) {
			const std::string& sImgFile = oPairImgFile.second;
			refTitleBgImg = oTheme.getImageByFileName(sImgFile);
		}
		oCtx.removeChecker(p0TitleEl, true);
	}
	TileColor oValueColor;
	TileAlpha oValueAlpha;
	TileFont oValueFont;
	shared_ptr<Image> refValueBgImg;
	auto p0ValueEl = getXmlConditionalParser()->parseUniqueElement(oCtx, p0Element, s_sTWidgetsFactoryVarValueNodeName, false);
	if (p0ValueEl != nullptr) {
		oCtx.addChecker(p0ValueEl);
		getXmlTraitsParser()->parseColor(oCtx, p0ValueEl, oValueColor);
		getXmlTraitsParser()->parseAlpha(oCtx, p0ValueEl, oValueAlpha);
		getXmlTraitsParser()->parseFont(oCtx, p0ValueEl, oValueFont);
		const auto oPairImgFile = getXmlThemeImageParser()->parseImage(oCtx, p0ValueEl, false);
		const std::string& sImgAttr = oPairImgFile.first;
		if (! sImgAttr.empty()) {
			const std::string& sImgFile = oPairImgFile.second;
			refValueBgImg = oTheme.getImageByFileName(sImgFile);
		}
		oCtx.removeChecker(p0ValueEl, true);
	}
	Frame oFrame;
	auto p0FrameEl = getXmlConditionalParser()->parseUniqueElement(oCtx, p0Element, s_sTWidgetsFactoryVarFrameNodeName, false);
	if (p0FrameEl != nullptr) {
		oFrame = parseLayoutFrame(oCtx, p0FrameEl);
	}
	oCtx.removeChecker(p0Element, true);
	//
	return std::make_unique<VarThWidgetFactory>(&oTheme, bTitlePreValue
					, oTitleColor, oTitleAlpha, oTitleFont
					, refTitleBgImg
					, oValueColor, oValueAlpha, oValueFont
					, refValueBgImg
					, oFrame);
}

} // namespace stmg
