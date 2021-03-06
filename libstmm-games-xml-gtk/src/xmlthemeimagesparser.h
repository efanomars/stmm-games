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
 * File:   xmlthemeimagesparser.h
 */

#ifndef STMG_XML_THEME_IMAGES_PARSER_H
#define STMG_XML_THEME_IMAGES_PARSER_H

#include <memory>

namespace stmg { class ThemeCtx; }
namespace stmg { class XmlConditionalParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;


class XmlThemeImagesParser
{
public:
	explicit XmlThemeImagesParser(XmlConditionalParser& oXmlConditionalParser);

	void parseImages(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
	void parseImageArrays(ThemeCtx& oCtx, const xmlpp::Element* p0Element);

private:
	void parseImagesImage(ThemeCtx& oCtx, const xmlpp::Element* p0Element);

	void parseImageArraysSubArray(ThemeCtx& oCtx, const xmlpp::Element* p0Element);
	void parseImageArraysFileArray(ThemeCtx& oCtx, const xmlpp::Element* p0Element);

private:
	XmlConditionalParser& m_oXmlConditionalParser;

private:
	XmlThemeImagesParser() = delete;
	XmlThemeImagesParser(const XmlThemeImagesParser& oSource) = delete;
	XmlThemeImagesParser& operator=(const XmlThemeImagesParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_THEME_IMAGES_PARSER_H */

