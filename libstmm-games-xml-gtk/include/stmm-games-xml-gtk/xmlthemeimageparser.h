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
 * File:   xmlthemeimageparser.h
 */

#ifndef STMG_XML_THEME_IMAGE_PARSER_H
#define STMG_XML_THEME_IMAGE_PARSER_H

#include <stmm-games-xml-base/xmlutil/xmlimageparser.h>

#include <string>
#include <memory>
#include <utility>

namespace stmg { class ThemeCtx; }
namespace stmg { class XmlConditionalParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlThemeImageParser : public XmlImageParser
{
public:
	XmlThemeImageParser(XmlConditionalParser& oXmlConditionalParser);

	/** Parses image attributes.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param sImgFileAttr The image file attribute. Cannot be empty.
	 * @param sImgIdAttr The image id attribute. If empty not parsed.
	 * @param bAttrMandatory Whether either attribute must be defined.
	 * @return The attribute that defines the image and the image file name.
	 */
	std::pair<std::string, std::string> parseImage(ThemeCtx& oCtx, const xmlpp::Element* p0Element
													, const std::string& sImgFileAttr, const std::string& sImgIdAttr
													, bool bAttrMandatory);
	/** Parses image attributes using attribute name defaults.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param bAttrMandatory Whether either attribute must be defined.
	 * @return The attribute that defines the image and the image file name.
	 */
	std::pair<std::string, std::string> parseImage(ThemeCtx& oCtx, const xmlpp::Element* p0Element
													, bool bAttrMandatory);

	static const std::string s_sImagesImageNodeName; /**< The default image node element name. */
	static const std::string s_sImagesImageFileAttr; /**< The default image file attribute name. */

private:
	const std::string& parseCommonImageAttr(ThemeCtx& oCtx, const xmlpp::Element* p0Element
										, const std::string& sImgFileAttr, const std::string& sImgIdAttr
										, bool bAttrMandatory, std::string& sImgFile);

private:
	XmlThemeImageParser() = delete;
	XmlThemeImageParser(const XmlThemeImageParser& oSource) = delete;
	XmlThemeImageParser& operator=(const XmlThemeImageParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_THEME_IMAGE_PARSER_H */

