/*
 * File:   xmlthemesoundsparser.h
 *
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

#ifndef STMG_XML_THEME_SOUNDS_PARSER_H
#define STMG_XML_THEME_SOUNDS_PARSER_H

#include <memory>

namespace stmg { class ThemeCtx; }
namespace stmg { class XmlConditionalParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;


class XmlThemeSoundsParser
{
public:
	explicit XmlThemeSoundsParser(XmlConditionalParser& oXmlConditionalParser);

	void parseSounds(ThemeCtx& oCtx, const xmlpp::Element* p0Element);

private:
	void parseSoundsSound(ThemeCtx& oCtx, const xmlpp::Element* p0Element);

private:
	XmlConditionalParser& m_oXmlConditionalParser;

private:
	XmlThemeSoundsParser() = delete;
	XmlThemeSoundsParser(const XmlThemeSoundsParser& oSource) = delete;
	XmlThemeSoundsParser& operator=(const XmlThemeSoundsParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_THEME_SOUNDS_PARSER_H */

