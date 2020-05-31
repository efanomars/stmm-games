/*
 * File:   xmlthememodifiersparser.h
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

#ifndef STMG_XML_THEME_MODIFIERS_PARSER_H
#define STMG_XML_THEME_MODIFIERS_PARSER_H

#include "xmlmodifierparser.h"

#include <vector>
#include <memory>
#include <tuple>

#include <stdint.h>

namespace stmg { class Image; }
namespace stmg { class StdThemeModifier; }
namespace stmg { class ThemeCtx; }
namespace stmg { class TileAni; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlThemeImageParser; }
namespace stmg { class XmlTraitsParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlThemeModifiersParser
{
public:
	XmlThemeModifiersParser(XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser, XmlThemeImageParser& oXmlThemeImageParser);

	void addXmlModifierParser(unique_ptr<XmlModifierParser> refXmlModifierParser);

	std::vector< unique_ptr<StdThemeModifier> > parseModifiers(ThemeCtx& oCtx, const xmlpp::Element* p0ParentElement);
	unique_ptr<StdThemeModifier> parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element);

private:
	friend class XmlModifierParser;

	shared_ptr<TileAni> parseModifierTileAniId(ThemeCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory);
	int32_t parseModifierTileAniName(ThemeCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory);
	double parseModifierElapsedDefault(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
	bool parseModifierInvert(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
	// bMandatory: either ani name or default must be defined
	std::tuple<int32_t, double, bool> parseTileAniNameDefault(ThemeCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory);
	//
	std::tuple<int32_t, double, bool, shared_ptr<Image>, shared_ptr<TileAni>> parseTileAniIdNameImgDefault(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
private:
	XmlConditionalParser& m_oXmlConditionalParser;
	XmlTraitsParser& m_oXmlTraitsParser;
	XmlThemeImageParser& m_oXmlThemeImageParser;

	std::vector< unique_ptr<XmlModifierParser> > m_aModifierParsers;

private:
	XmlThemeModifiersParser() = delete;
	XmlThemeModifiersParser(const XmlThemeModifiersParser& oSource) = delete;
	XmlThemeModifiersParser& operator=(const XmlThemeModifiersParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_THEME_MODIFIERS_PARSER_H */

