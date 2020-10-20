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
 * File:   xmlplaintextthanimationfactory.cc
 */

#include "themeanimations/xmlplaintextthanimationfactory.h"

#include "themectx.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/xmltraitsparser.h>

#include <stmm-games-gtk/animations/plaintextthanifactory.h>
#include <stmm-games-gtk/stdthemeanimationfactory.h>

#include <stmm-games/tile.h>

//#include <cassert>
//#include <iostream>
#include <string>
#include <utility>

namespace stmg { class StdTheme; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sAnimationsPlainTextNodeName = "PlainText";
static const std::string s_sAnimationsPlainTextCenterAttr = "center";
static const std::string s_sAnimationsPlainTextFadeInAttr = "fadeIn";
static const std::string s_sAnimationsPlainTextFadeInFactorAttr = "fadeInFactor";
static const std::string s_sAnimationsPlainTextFadeOutAttr = "fadeOut";
static const std::string s_sAnimationsPlainTextFadeOutFactorAttr = "fadeOutFactor";

XmlPlainTextThAnimationFactoryParser::XmlPlainTextThAnimationFactoryParser()
: XmlThAnimationFactoryParser(s_sAnimationsPlainTextNodeName)
{
}

unique_ptr<StdThemeAnimationFactory> XmlPlainTextThAnimationFactoryParser::parseAnimationFactory(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	bool bCenter = true;
	const auto oPairCenter = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sAnimationsPlainTextCenterAttr);
	if (oPairCenter.first) {
		const std::string& sCenter = oPairCenter.second;
		bCenter = XmlUtil::strToBool(oCtx, p0Element, s_sAnimationsPlainTextCenterAttr, sCenter);
	}

	double fFadeIn = 0;
	const auto oPairFadeIn = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sAnimationsPlainTextFadeInAttr);
	const bool bFadeInMilliSet = oPairFadeIn.first;
	if (bFadeInMilliSet) {
		const std::string& sFadeIn = oPairFadeIn.second;
		fFadeIn = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sAnimationsPlainTextFadeInAttr, sFadeIn, false
														, true, 0, false, -1);
	}
	const auto oPairFadeInFactor = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sAnimationsPlainTextFadeInFactorAttr);
	if (oPairFadeInFactor.first) {
		if (bFadeInMilliSet) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sAnimationsPlainTextFadeInFactorAttr, s_sAnimationsPlainTextFadeInAttr);
		}
		const std::string& sFadeInFactor = oPairFadeInFactor.second;
		fFadeIn = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sAnimationsPlainTextFadeInFactorAttr, sFadeInFactor, true
														, true, 0, true, 1);
	}
	double fFadeOut = 0;
	const auto oPairFadeOut = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sAnimationsPlainTextFadeOutAttr);
	const bool bFadeOutMilliSet = oPairFadeOut.first;
	if (bFadeOutMilliSet) {
		const std::string& sFadeOut = oPairFadeOut.second;
		fFadeOut = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sAnimationsPlainTextFadeOutAttr, sFadeOut, false
														, true, 0, false, -1);
	}
	const auto oPairFadeOutFactor = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sAnimationsPlainTextFadeOutFactorAttr);
	if (oPairFadeOutFactor.first) {
		if (bFadeOutMilliSet) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sAnimationsPlainTextFadeOutFactorAttr, s_sAnimationsPlainTextFadeOutAttr);
		}
		const std::string& sFadeOutFactor = oPairFadeOutFactor.second;
		fFadeOut = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sAnimationsPlainTextFadeOutFactorAttr, sFadeOutFactor, true
										, true, 0, true, 1);
	}

	TileColor oColor;
	TileAlpha oAlpha;
	TileFont oFont;
	getXmlTraitsParser()->parseColor(oCtx, p0Element, oColor);
//#ifndef NDEBUG
//std::cout << "XmlThemeAniParser::parseAnimationsPlainText Color: ";
//oColor.dump();
//if (oColor.getColorType() == TileColor::COLOR_TYPE_INDEX) {
//std::cout << "  " << oCtx.named().colors().getName(oColor.getColorIndex());
//}
//std::cout << "" << '\n';
//#endif //NDEBUG
	getXmlTraitsParser()->parseAlpha(oCtx, p0Element, oAlpha);
	getXmlTraitsParser()->parseFont(oCtx, p0Element, oFont);

	oCtx.removeChecker(p0Element, true);
	return std::make_unique<PlainTextThAniFactory>(&oTheme, bCenter
					, oColor, oAlpha, oFont, !bFadeInMilliSet, fFadeIn, !bFadeOutMilliSet, fFadeOut);
}

} // namespace stmg
