/*
 * File:   xmlthemeimagesparser.cc
 *
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


#include "xmlthemeimagesparser.h"

#include "xmlutil/xmlstrconv.h"
#include "xmlthemeimageparser.h"
#include "themectx.h"
#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "xmlutil/xmlimageparser.h"

#include <stmm-games-gtk/stdtheme.h>

#include <stmm-games/util/util.h>

#include <vector>
//#include <cassert>
#include <string>
//#include <iostream>
#include <cstdint>
#include <utility>

#include <stdint.h>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sImageArraysArrayIdAttr = "arrayId";
static const std::string s_sImageArraysSubArrayNodeName = "SubArray";
static const std::string s_sImageArraysSubArrayArrayIdAttr = s_sImageArraysArrayIdAttr;
static const std::string s_sImageArraysSubArrayImgFileAttr = "imgFile";
static const std::string s_sImageArraysSubArraySubImgWAttr = "subImgW";
static const std::string s_sImageArraysSubArraySubImgHAttr = "subImgH";
static const std::string s_sImageArraysSubArraySpacingXAttr = "spacingX";
static const std::string s_sImageArraysSubArraySpacingYAttr = "spacingY";
static const std::string s_sImageArraysSubArrayPerRowAttr = "perRow";
static const std::string s_sImageArraysSubArrayArraySizeAttr = "arraySize";
static const std::string s_sImageArraysFileArrayNodeName = "FileArray";
static const std::string s_sImageArraysFileArrayArrayIdAttr = s_sImageArraysArrayIdAttr;
static const std::string s_sImageArraysFileArrayImgFileAttr = "imgFile";
static const std::string s_sImageArraysFileArraySobstStrAttr = "sobstStr";
	static const std::string s_sImageArraysFileArraySobstStrDefault = "%%";
static const std::string s_sImageArraysFileArrayFromCharAttr = "fromChar";
static const std::string s_sImageArraysFileArrayToCharAttr = "toChar";
	static const int32_t s_nImageArraysFileArrayFromToMinChar = 65;
	static const int32_t s_nImageArraysFileArrayFromToMaxChar = 127;
static const std::string s_sImageArraysFileArrayFromIntAttr = "fromInt";
static const std::string s_sImageArraysFileArrayToIntAttr = "toInt";
static const std::string s_sImageArraysFileArrayMinLenAttr = "minLen";
static const std::string s_sImageArraysFileArrayDefaultImgFileAttr = "defaultImgFile";


XmlThemeImagesParser::XmlThemeImagesParser(XmlConditionalParser& oXmlConditionalParser)
: m_oXmlConditionalParser(oXmlConditionalParser)
{
}
void XmlThemeImagesParser::parseImageArrays(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sImageArraysSubArrayNodeName, [&](const xmlpp::Element* p0SubArray)
	{
		parseImageArraysSubArray(oCtx, p0SubArray);
	});
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sImageArraysFileArrayNodeName, [&](const xmlpp::Element* p0FileArray)
	{
		parseImageArraysFileArray(oCtx, p0FileArray);
	});
	oCtx.removeChecker(p0Element, true);
}
void XmlThemeImagesParser::parseImageArraysSubArray(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlThemeImagesParser::parseImageArraysSubArray()" << '\n';
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	const auto oPairArrayId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sImageArraysSubArrayArrayIdAttr);
	if (! oPairArrayId.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sImageArraysSubArrayArrayIdAttr);
	}
	const std::string& sArrayId = oPairArrayId.second;
	if (oCtx.isMain() && oTheme.hasArray(sArrayId)) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sImageArraysSubArrayArrayIdAttr, Util::stringCompose(
							"Attribute %1: Image ArrayId '%2' already defined", s_sImageArraysSubArrayArrayIdAttr, sArrayId));
	}

	const auto oPairImgFile = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sImageArraysSubArrayImgFileAttr);
	if (! oPairImgFile.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sImageArraysSubArrayImgFileAttr);
	}
	const std::string& sImgFile = oPairImgFile.second;
	if (!oTheme.knowsImageFile(sImgFile)) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sImageArraysSubArrayImgFileAttr, Util::stringCompose(
											"Image File '%1' not defined", sImgFile));
	}
	const auto oPairSubImgW = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sImageArraysSubArraySubImgWAttr);
	if (! oPairSubImgW.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sImageArraysSubArraySubImgWAttr);
	}
	const std::string& sSubImgW = oPairSubImgW.second;
	const int32_t nSubImgW = XmlUtil::strToNumber(oCtx, p0Element, s_sImageArraysSubArraySubImgWAttr, sSubImgW, false
														, true, 6, false, -1);
	const auto oPairSubImgH = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sImageArraysSubArraySubImgHAttr);
	if (! oPairSubImgH.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sImageArraysSubArraySubImgHAttr);
	}
	const std::string& sSubImgH = oPairSubImgH.second;
	const int32_t nSubImgH = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sImageArraysSubArraySubImgHAttr, sSubImgH, false
																	, true, 6, false, -1);
	int32_t nSpacingX = 0;
	const auto oPairSpacingX = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sImageArraysSubArraySpacingXAttr);
	if (oPairSpacingX.first) {
		const std::string& sSpacingX = oPairSpacingX.second;
		nSpacingX = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sImageArraysSubArraySpacingXAttr, sSpacingX, false
														, true, 0, false, -1);
	}
	int32_t nSpacingY = 0;
	const auto oPairSpacingY = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sImageArraysSubArraySpacingYAttr);
	if (oPairSpacingY.first) {
		const std::string& sSpacingY = oPairSpacingY.second;
		nSpacingY = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sImageArraysSubArraySpacingYAttr, sSpacingY, false
														, true, 0, false, -1);
	}
	int32_t nPerRow = 10;
	const auto oPairPerRow = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sImageArraysSubArrayPerRowAttr);
	if (oPairPerRow.first) {
		const std::string& sPerRow = oPairPerRow.second;
		nPerRow = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sImageArraysSubArrayPerRowAttr, sPerRow, false
														, true, 1, false, -1);
	}
	const auto oPairArraySize = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sImageArraysSubArrayArraySizeAttr);
	if (! oPairArraySize.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sImageArraysSubArrayArraySizeAttr);
	}
	const std::string& sArraySize = oPairArraySize.second;
	const int32_t nArraySize = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sImageArraysSubArrayArraySizeAttr, sArraySize, false
																	, true, 1, false, -1);
	oCtx.removeChecker(p0Element, true);
	oTheme.addSubArray(sArrayId, sImgFile, nSubImgW, nSubImgH, nSpacingX, nSpacingY, nPerRow, nArraySize);

}
void XmlThemeImagesParser::parseImageArraysFileArray(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlThemeImagesParser::parseImageArraysFileArray()" << '\n';
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	const auto oPairArrayId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sImageArraysFileArrayArrayIdAttr);
	if (! oPairArrayId.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sImageArraysFileArrayArrayIdAttr);
	}
	const std::string& sArrayId = oPairArrayId.second;
	if (oCtx.isMain() && oTheme.hasArray(sArrayId)) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sImageArraysSubArrayArrayIdAttr, Util::stringCompose(
							"Attribute %1: Image ArrayId '%2' already defined", s_sImageArraysSubArrayArrayIdAttr, sArrayId));
	}
	const auto oPairImgFile = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sImageArraysFileArrayImgFileAttr);
	if (! oPairImgFile.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sImageArraysFileArrayImgFileAttr);
	}
	const std::string& sImgFile = oPairImgFile.second;
	const auto oPairSobstStr = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sImageArraysFileArraySobstStrAttr);
	const std::string& sSobstStr = (oPairSobstStr.first ? oPairSobstStr.second : s_sImageArraysFileArraySobstStrDefault);
	const auto nSobstPos = sImgFile.find(sSobstStr, 0);
	if (nSobstPos == std::string::npos) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sImageArraysFileArraySobstStrAttr, Util::stringCompose(
						"Attribute '%1' is not contained in '%2'", s_sImageArraysFileArraySobstStrAttr, s_sImageArraysFileArrayImgFileAttr));
	}
	const std::string sLeftImgFile = sImgFile.substr(0, nSobstPos);
	const std::string sRightImgFile = sImgFile.substr(nSobstPos + sSobstStr.size(), sImgFile.size() - nSobstPos - sSobstStr.size());

	std::string sDefaultImgFile;
	const auto oPairDefaultImgFile = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sImageArraysFileArrayDefaultImgFileAttr);
	if (oPairDefaultImgFile.first) {
		sDefaultImgFile = oPairDefaultImgFile.second;
		if (sDefaultImgFile.empty()) {
			throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, s_sImageArraysFileArrayDefaultImgFileAttr);
		} else if (! oTheme.knowsImageFile(sDefaultImgFile)) {
			throw XmlCommonErrors::error(oCtx, p0Element, s_sImageArraysFileArrayDefaultImgFileAttr, Util::stringCompose(
												"Default image File '%1' not defined", sDefaultImgFile));
		}
	}

	char cFrom = 'A';
	char cTo = 'A';
	const bool bCharFromToDefined = m_oXmlConditionalParser.parseAttributeFromTo<char>(oCtx, p0Element
							, "</>", s_sImageArraysFileArrayFromCharAttr,s_sImageArraysFileArrayToCharAttr
							, false, true, s_nImageArraysFileArrayFromToMinChar, true, s_nImageArraysFileArrayFromToMaxChar, cFrom, cTo);

	int32_t nFrom = 0;
	int32_t nTo = 0;
	const bool bIntFromToDefined = m_oXmlConditionalParser.parseAttributeFromTo<int32_t>(oCtx, p0Element
							, "</>", s_sImageArraysFileArrayFromIntAttr,s_sImageArraysFileArrayToIntAttr
							, false, true, 0, false, -1, nFrom, nTo);
	if (bCharFromToDefined && bIntFromToDefined) {
		throw XmlCommonErrors::errorAttrAlreadyDefinedByOthers(oCtx, p0Element, s_sImageArraysFileArrayFromIntAttr
											, s_sImageArraysFileArrayFromCharAttr, s_sImageArraysFileArrayToCharAttr);
	}
	//
	int32_t nMinLen = 0;
	const auto oPairMinLen = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sImageArraysFileArrayMinLenAttr);
	if (oPairMinLen.first) {
		if (bCharFromToDefined) {
			throw XmlCommonErrors::error(oCtx, p0Element, s_sImageArraysFileArrayMinLenAttr, Util::stringCompose("Attribute %1"
					" is not compatible with attribute %2" 
					, s_sImageArraysFileArrayMinLenAttr, s_sImageArraysFileArrayFromCharAttr));
		}
		const std::string& sMinLen = oPairMinLen.second;
		nMinLen = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sImageArraysFileArrayMinLenAttr, sMinLen, false
														, true, 0, true, 9);
	}
	if (bCharFromToDefined) {
		nFrom = static_cast<int32_t>(cFrom);
		nTo = static_cast<int32_t>(cTo);
	} else if (!bIntFromToDefined) {
		throw XmlCommonErrors::errorAttrEitherMustBeDefined(oCtx, p0Element, s_sImageArraysFileArrayFromIntAttr
											, s_sImageArraysFileArrayFromCharAttr);
	}
	const int32_t nStep = ((nFrom <= nTo) ? 1 : -1);
	std::string sFile;
	std::vector<std::string> aFileNames;
	for (int32_t nCur = nFrom; nCur != nTo + nStep; nCur += nStep) {
		if (bCharFromToDefined) {
			sFile = sLeftImgFile;
			sFile.append(1, static_cast<char>(nCur));
			sFile.append(sRightImgFile);
		} else {
			std::string sNr = std::to_string(nCur);
			const int32_t nNrLen = static_cast<int32_t>(sNr.size());
			if (nNrLen < nMinLen) {
				sNr = std::string(nMinLen - nNrLen, '0') + sNr;
			}
			sFile = sLeftImgFile + sNr + sRightImgFile;
		}
		if (! oTheme.knowsImageFile(sFile)) {
			if (sDefaultImgFile.empty()) {
				throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
										"Image (Array) File '%1' not found", sFile));
			} else {
				sFile = sDefaultImgFile;
			}
		}
		aFileNames.push_back(std::move(sFile));
	}
	oCtx.removeChecker(p0Element, true);
	oTheme.addFileArray(sArrayId, aFileNames);
}

void XmlThemeImagesParser::parseImages(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, XmlThemeImageParser::s_sImagesImageNodeName, [&](const xmlpp::Element* p0Image)
	{
		parseImagesImage(oCtx, p0Image);
	});
	oCtx.removeChecker(p0Element, true);
}
void XmlThemeImagesParser::parseImagesImage(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlThemeImagesParser::parseImagesImage()" << '\n';
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	const auto oPairImgId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, XmlThemeImageParser::s_sImagesImageIdAttr);
	if (! oPairImgId.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, XmlThemeImageParser::s_sImagesImageIdAttr);
	}
	const std::string& sImgId = oPairImgId.second;
	if (oCtx.isMain() && oTheme.hasImageId(sImgId)) {
		throw XmlCommonErrors::error(oCtx, p0Element, XmlThemeImageParser::s_sImagesImageIdAttr, Util::stringCompose(
								"Image Id '%1' already defined", sImgId));
	}

	const auto oPairImgFile = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, XmlThemeImageParser::s_sImagesImageFileAttr);
	if (! oPairImgFile.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, XmlThemeImageParser::s_sImagesImageFileAttr);
	}
	const std::string& sImgFile = oPairImgFile.second;
	if (!oTheme.knowsImageFile(sImgFile)) {
		throw XmlCommonErrors::error(oCtx, p0Element, XmlThemeImageParser::s_sImagesImageFileAttr, Util::stringCompose(
														"Image File '%1' not found", sImgFile));
	}
	oCtx.removeChecker(p0Element, true);
	oTheme.addImageId(sImgId, sImgFile);
}

} // namespace stmg
