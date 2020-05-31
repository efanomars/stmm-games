/*
 * File:   xmlthemeimageparser.cc
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


#include "xmlthemeimageparser.h"

#include "themectx.h"
#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>

#include <stmm-games-gtk/stdtheme.h>

#include <stmm-games/util/util.h>

#include <cassert>
//#include <iostream>

namespace xmlpp { class Element; }

namespace stmg
{

const std::string XmlThemeImageParser::s_sImagesImageNodeName = "Image";
const std::string XmlThemeImageParser::s_sImagesImageFileAttr = "imgFile";

XmlThemeImageParser::XmlThemeImageParser(XmlConditionalParser& oXmlConditionalParser)
: XmlImageParser(oXmlConditionalParser)
{
}
std::pair<std::string, std::string> XmlThemeImageParser::parseImage(ThemeCtx& oCtx, const xmlpp::Element* p0Element
																	, const std::string& sImgFileAttr, const std::string& sImgIdAttr
																	, bool bAttrMandatory)
{
	std::string sImgFile;
	const std::string& sAttr = parseCommonImageAttr(oCtx, p0Element
													, sImgFileAttr, sImgIdAttr
													, bAttrMandatory, sImgFile);
	return std::make_pair(sAttr, sImgFile);
}
std::pair<std::string, std::string> XmlThemeImageParser::parseImage(ThemeCtx& oCtx, const xmlpp::Element* p0Element
																	, bool bAttrMandatory)
{
	return parseImage(oCtx, p0Element, s_sImagesImageFileAttr, s_sImagesImageIdAttr, bAttrMandatory);
}

const std::string& XmlThemeImageParser::parseCommonImageAttr(ThemeCtx& oCtx, const xmlpp::Element* p0Element
										, const std::string& sImgFileAttr, const std::string& sImgIdAttr
										, bool bAttrMandatory, std::string& sImgFile)
{
	assert(p0Element != nullptr);
	assert(! sImgFileAttr.empty());
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	bool bImageDefined = false;
	bool bIdDefined = false;
	const auto oPairImgFile = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sImgFileAttr);
	if (oPairImgFile.first) {
		sImgFile = oPairImgFile.second;
		if (!oTheme.knowsImageFile(sImgFile)) {
			throw XmlCommonErrors::error(oCtx, p0Element, sImgFileAttr, Util::stringCompose(
											"Attribute '%1': '%2' file not found", sImgFileAttr, sImgFile));
		}
		bImageDefined = true;
	}
	if (!sImgIdAttr.empty()) {
		const auto oPairImgId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sImgIdAttr);
		if (oPairImgId.first) {
			const std::string& sImgId = oPairImgId.second;
			if (bImageDefined) {
				throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sImgIdAttr, sImgFileAttr);
			}
			if (!oTheme.hasImageId(sImgId)) {
				throw XmlCommonErrors::error(oCtx, p0Element, sImgIdAttr, Util::stringCompose(
										"Attribute '%1': image id '%2' not defined", sImgIdAttr, sImgId));
			}
			sImgFile = oTheme.getImageIdFileName(sImgId);
			bImageDefined = true;
			bIdDefined = true;
		}
	}
	if (bAttrMandatory && !bImageDefined) {
		if (sImgIdAttr.empty()) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, sImgFileAttr);
		} else {
			throw XmlCommonErrors::errorAttrEitherMustBeDefined(oCtx, p0Element, sImgFileAttr, sImgIdAttr);
		}
	}
	oCtx.removeChecker(p0Element, false, true);
	if (bImageDefined) {
		if (bIdDefined) {
			return sImgIdAttr;
		} else {
			return sImgFileAttr;
		}
	} else {
		return Util::s_sEmptyString;
	}
}

} // namespace stmg
