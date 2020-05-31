/*
 * File:   xmlstaticgridthanimationfactory.cc
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

#include "themeanimations/xmlstaticgridthanimationfactory.h"

#include "themectx.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "xmlutil/xmlstrconv.h"

#include <stmm-games-gtk/animations/staticgridthanifactory.h>
#include <stmm-games-gtk/stdthemeanimationfactory.h>

//#include <cassert>
//#include <iostream>
#include <string>
#include <utility>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sAnimationsStaticGridNodeName = "StaticGrid";
static const std::string s_sAnimationsStaticGridOpacityAttr = "opacity";

XmlStaticGridThAnimationFactoryParser::XmlStaticGridThAnimationFactoryParser()
: XmlThAnimationFactoryParser(s_sAnimationsStaticGridNodeName)
{
}

unique_ptr<StdThemeAnimationFactory> XmlStaticGridThAnimationFactoryParser::parseAnimationFactory(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);

	double fOpacity = 1.0;
	const auto oPairOpacity = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sAnimationsStaticGridOpacityAttr);
	if (oPairOpacity.first) {
		const std::string& sOpacity = oPairOpacity.second;
		fOpacity = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sAnimationsStaticGridOpacityAttr, sOpacity, true
														, true, 0, true, 1);
	}

	oCtx.removeChecker(p0Element, true);
	return std::make_unique<StaticGridThAniFactory>(&(oCtx.theme()), fOpacity);
}

} // namespace stmg
