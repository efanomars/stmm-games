/*
 * File:   xmlthemeaniparser.h
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

#ifndef STMG_XML_THEME_ANI_PARSER_H
#define STMG_XML_THEME_ANI_PARSER_H

#include "xmlthanimationfactoryparser.h"

#include <vector>
#include <memory>

namespace stmg { class ThemeCtx; }
namespace stmg { class TileAni; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlThemeImageParser; }
namespace stmg { class XmlTraitsParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;
using std::shared_ptr;

class XmlThemeAniParser
{
public:
	XmlThemeAniParser(XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser, XmlThemeImageParser& oXmlThemeImageParser);

	void addXmlThAnimationFactoryParser(unique_ptr<XmlThAnimationFactoryParser> refXmlThAnimationFactoryParser);

	void parseAnimations(ThemeCtx& oCtx, const xmlpp::Element* p0Element);

	void parseTileAnis(ThemeCtx& oCtx, const xmlpp::Element* p0Element);

private:
	void parseAnimationsAnimation(ThemeCtx& oCtx, const xmlpp::Element* p0Element);

	void parseTileAnisAni(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
	void parseTileAnisAniImgDur(ThemeCtx& oCtx, const xmlpp::Element* p0Element, TileAni& oTileAni);

private:
	friend class XmlThAnimationFactoryParser;
	XmlConditionalParser& m_oXmlConditionalParser;
	XmlTraitsParser& m_oXmlTraitsParser;
	XmlThemeImageParser& m_oXmlThemeImageParser;

	std::vector<unique_ptr<XmlThAnimationFactoryParser>> m_aXmlThAnimationFactoryParsers;

private:
	XmlThemeAniParser() = delete;
	XmlThemeAniParser(const XmlThemeAniParser& oSource) = delete;
	XmlThemeAniParser& operator=(const XmlThemeAniParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_THEME_ANI_PARSER_H */

