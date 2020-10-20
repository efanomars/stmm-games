/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   xmlthemesoundsparser.cc
 */


#include "xmlthemesoundsparser.h"

#include "themectx.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>

#include <stmm-games-gtk/stdtheme.h>

#include <stmm-games/util/util.h>

//#include <cassert>
#include <string>
//#include <iostream>
#include <utility>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sSoundsSoundNodeName = "Sound";
static const std::string s_sSoundsSndIdAttr = "sndId";
static const std::string s_sSoundsSndFileAttr = "sndFile";


XmlThemeSoundsParser::XmlThemeSoundsParser(XmlConditionalParser& oXmlConditionalParser)
: m_oXmlConditionalParser(oXmlConditionalParser)
{
}

void XmlThemeSoundsParser::parseSounds(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sSoundsSoundNodeName, [&](const xmlpp::Element* p0Sound)
	{
		parseSoundsSound(oCtx, p0Sound);
	});
	oCtx.removeChecker(p0Element, true);
}
void XmlThemeSoundsParser::parseSoundsSound(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	const auto oPairSndId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sSoundsSndIdAttr);
	if (! oPairSndId.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sSoundsSndIdAttr);
	}
	const std::string& sSndId = oPairSndId.second;
	if (oCtx.isMain() && oTheme.hasSoundId(sSndId)) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sSoundsSndIdAttr, Util::stringCompose(
								"Sound Id '%1' already defined", sSndId));
	}

	const auto oPairSndFile = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sSoundsSndFileAttr);
	if (! oPairSndFile.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sSoundsSndFileAttr);
	}
	const std::string& sSndFile = oPairSndFile.second;
	if (!oTheme.knowsSoundFile(sSndFile)) {
//std::cout << "XmlThemeSoundsParser::parseSoundsSound() !knows sSndFile=" << sSndFile << '\n';
//oCtx.dump();
		throw XmlCommonErrors::error(oCtx, p0Element, s_sSoundsSndFileAttr, Util::stringCompose(
														"Sound File '%1' not found", sSndFile));
	}
	oCtx.removeChecker(p0Element, true);
	oTheme.addSoundId(sSndId, sSndFile);
}

} // namespace stmg
