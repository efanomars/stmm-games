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
 * File:   xmlthemeassignparser.h
 */

#ifndef STMG_XML_THEME_ASSIGN_PARSER_H
#define STMG_XML_THEME_ASSIGN_PARSER_H

#include <memory>

namespace stmg { class ThemeCtx; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlThemeImagesParser; }
namespace stmg { class XmlTraitsParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlThemeAssignParser
{
public:
	XmlThemeAssignParser(XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser, XmlThemeImagesParser& oXmlThemeImagesParser);

	void parseAssigns(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
private:
	void parseAssign(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
private:
	XmlConditionalParser& m_oXmlConditionalParser;
	XmlTraitsParser& m_oXmlTraitsParser;
	//XmlThemeImagesParser& m_oXmlThemeImagesParser;

private:
	XmlThemeAssignParser() = delete;
	XmlThemeAssignParser(const XmlThemeAssignParser& oSource) = delete;
	XmlThemeAssignParser& operator=(const XmlThemeAssignParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_THEME_ASSIGN_PARSER_H */

