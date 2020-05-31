/*
 * File:   xmlimageparser.h
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

#ifndef STMG_XML_IMAGE_PARSER_H
#define STMG_XML_IMAGE_PARSER_H

#include <string>
#include <memory>

#include <stdint.h>

namespace stmg { class ConditionalCtx; }
namespace stmg { class XmlConditionalParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlImageParser
{
public:
	XmlImageParser(XmlConditionalParser& oXmlConditionalParser);

	/** Parses image id.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param sImgIdAttr The image id attribute.
	 * @param bAttrMandatory Whether either attribute must be defined.
	 * @return The image id index into Named::images() or -1 if not defined.
	 */
	int32_t parseImageId(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
						, const std::string& sImgIdAttr, bool bAttrMandatory);
	/** Parses image id using attribute name defaults.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param bAttrMandatory Whether either attribute must be defined.
	 * @return The image id index into Named::images() or -1 if not defined.
	 */
	int32_t parseImageId(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
						, bool bAttrMandatory);

	static const std::string s_sImagesImageIdAttr; /**< The default image id attribute name. */

protected:
	XmlConditionalParser& m_oXmlConditionalParser;

private:
	XmlImageParser() = delete;
	XmlImageParser(const XmlImageParser& oSource) = delete;
	XmlImageParser& operator=(const XmlImageParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_IMAGE_PARSER_H */

