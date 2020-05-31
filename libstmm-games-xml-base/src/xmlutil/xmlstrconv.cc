/*
 * File:   xmlstrconv.cc
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

#include "xmlutil/xmlstrconv.h"

#include "xmlcommonparser.h"
#include "parserctx.h"

#include <stmm-games/util/basictypes.h>
#include <stmm-games/named.h>
#include <stmm-games/tile.h>
#include <stmm-games/util/namedindex.h>
#include <stmm-games/util/util.h>

#include <glibmm/ustring.h>
#include <glib.h>

#include <cassert>
//#include <iostream>
#include <cstdint>
#include <utility>
#include <limits>

namespace xmlpp { class Element; }

namespace stmg
{

namespace XmlUtil
{

static const std::string s_sHexDigits = "0123456789ABCDEF";

uint32_t strToUint32(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName
									, const std::string& sInt, bool bMin, uint32_t nMin, bool bMax, uint32_t nMax)
{
	const auto nSize = sInt.size();
	if (nSize == 0) {
		throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, sAttrName);
	}
	if (nSize >= 2) {
		const gunichar c0 = sInt[0];
		const gunichar c1 = sInt[1];
		if ((c0 == '0') && ((c1 == 'x') || (c1 == 'X'))) {
			// hex number 0xF34
			const std::string sHex = Glib::ustring{sInt}.uppercase();
			const auto nHexSize = static_cast<uint32_t>(sHex.size());
			uint64_t nRes = 0;
			uint32_t nIdx = 0;
			for (; nIdx < nHexSize; ++nIdx) {
				auto nQ = s_sHexDigits.find(sHex[nIdx]);
				if (nQ == std::string::npos) {
					break; // while ----
				}
				nRes = (nRes << 4) | static_cast<uint8_t>(nQ);
				if ((bMax && (nRes > nMax)) || (nRes > static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()))) {
					throw XmlCommonErrors::error(oCtx, p0Element, sAttrName, Util::stringCompose("Attribute '%1' wrong (possibly too big) hex number", sAttrName));
				}
			}
			if ((nIdx < nHexSize) || (nHexSize == 0)) {
				throw XmlCommonErrors::error(oCtx, p0Element, sAttrName, Util::stringCompose(
						"Attribute '%1' contains invalid hex number", sAttrName));
			}
			if (bMin && (nRes < nMin)) {
				throw XmlCommonErrors::error(oCtx, p0Element, sAttrName, Util::stringCompose(
						"Attribute '%1' must be >= than %2", sAttrName, std::to_string(nMin)));
			}
			return static_cast<uint32_t>(nRes); //------------------------------
		}
	}
	//
	return strToNumber<uint32_t>(oCtx, p0Element, sAttrName, sInt, false, bMin, nMin, bMax, nMax);
}

TileChar strToTileChar(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName
									, const std::string& sChar)
{
	const auto nSize = sChar.size();
	if (nSize == 0) {
		throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, sAttrName);
	}
	TileChar oTileChar;
	const gunichar c = sChar[0];
	if (c == '\'') {
		Glib::ustring sUChar = sChar;
		const auto nUSize = sUChar.size();
		if ((nUSize != 3) || (sUChar[2] != '\'')) {
			throw XmlCommonErrors::error(oCtx, p0Element, sAttrName, Util::stringCompose("Attribute '%1' wrong unicode char", sAttrName));
		}
		const gunichar cRes = sUChar[1];
		const auto nCode = static_cast<uint32_t>(cRes);
		if (! ((nCode >= TileChar::CHAR_UCS4_MIN) && (nCode <= TileChar::CHAR_UCS4_MAX))) {
			throw XmlCommonErrors::errorAttrUTF8CharInvalid(oCtx, p0Element, sAttrName, std::to_string(nCode));
		}
		oTileChar.setChar(nCode);
	} else if ((c >= '0') && (c <= '9')) {
		// char code
		const uint32_t nCode = strToUint32(oCtx, p0Element, sAttrName, sChar, true, TileChar::CHAR_UCS4_MIN, true, TileChar::CHAR_UCS4_MAX);
		oTileChar.setChar(nCode);
	} else {
		// char name
		XmlCommonParser::validateName(oCtx, p0Element, sAttrName, sChar, false);
		const int32_t nCharIdx = oCtx.named().chars().addName(sChar);
		if (nCharIdx > TileChar::CHAR_INDEX_MAX) {
			throw XmlCommonErrors::errorAttrOneTooManyNames(oCtx, p0Element, sAttrName);
		}
		assert(nCharIdx >= TileChar::CHAR_INDEX_MIN);
		oTileChar.setCharIndex(nCharIdx);
	}
	return oTileChar;
}

TileColor strToTileColor(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName
										, const std::string& sColor)
{
	const auto nSize = sColor.size();
	if (nSize == 0) {
		throw oCtx.error("Error: attribute " + sAttrName + " cannot be empty");
	}
	TileColor oTileColor;
	const gunichar c = sColor[0];
	if (c == '#') {
		// "#F0A80D"
		const RgbColor oRgb = strHexToRGB(oCtx, p0Element, sAttrName, sColor);
		oTileColor.setColorRGB(oRgb.m_nR, oRgb.m_nG, oRgb.m_nB);
	} else if (c == '%') {
		// "%nR^nG^nB"
		const std::size_t nFoundDash1Pos = sColor.find('^', 1);
		std::size_t nFoundDash2Pos = std::string::npos;
		if (nFoundDash1Pos != std::string::npos) {
			nFoundDash2Pos = sColor.find('^', nFoundDash1Pos + 1);
		}
		if (nFoundDash2Pos != std::string::npos) {
			throw oCtx.error("Error: attribute " + sAttrName + " malformed, format is '%r^g^b'");
		}
		std::string sR = sColor.substr(1, nFoundDash1Pos - 1);
		const uint8_t nR = static_cast<uint8_t>(strToNumber<int32_t>(oCtx, p0Element, sAttrName, sR, false, true, 0, true, 255));
		std::string sG = sColor.substr(nFoundDash1Pos + 1, nFoundDash2Pos - (nFoundDash1Pos + 1));
		const uint8_t nG = static_cast<uint8_t>(strToNumber<int32_t>(oCtx, p0Element, sAttrName, sG, false, true, 0, true, 255));
		std::string sB = sColor.substr(nFoundDash2Pos + 1, nSize - (nFoundDash2Pos + 1));
		const uint8_t nB = static_cast<uint8_t>(strToNumber<int32_t>(oCtx, p0Element, sAttrName, sB, false, true, 0, true, 255));
		oTileColor.setColorRGB(nR, nG, nB);
	} else if ((c >= '0') && (c <= '9')) {
		// color pal
		const int32_t nPal = strToNumber<int32_t>(oCtx, p0Element, sAttrName, sColor, false, true, TileColor::COLOR_PAL_FIRST, true, TileColor::COLOR_PAL_LAST);
		oTileColor.setColorPal(nPal);
	} else {
		// color name
		XmlCommonParser::validateName(oCtx, p0Element, sAttrName, sColor, false);
		const int32_t nColorIdx = oCtx.named().colors().addName(sColor);
		if (nColorIdx > TileColor::COLOR_INDEX_MAX) {
			throw XmlCommonErrors::errorAttrOneTooManyNames(oCtx, p0Element, sAttrName);
		}
		assert(nColorIdx >= TileColor::COLOR_INDEX_MIN);
		oTileColor.setColorIndex(nColorIdx);
	}
	return oTileColor;
}
RgbColor strHexToRGB(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName
							, const std::string& sHexNr)
{
	bool bFormatError = false;
	int32_t nHexIdx;
	const std::string sHex = Glib::ustring{sHexNr}.uppercase();
	const std::string::size_type nLen = sHex.length();
	if ((nLen == 1+6) && (sHex.at(0) == '#')) {
		nHexIdx = 1;
	} else {
		bFormatError = true;
	}
	if (!bFormatError) {
		if (sHex.find_first_not_of(s_sHexDigits, nHexIdx) != std::string::npos) {
			bFormatError = true;
		}
	}
	if (bFormatError) {
		throw XmlCommonErrors::errorAttrHexColorFormat(oCtx, p0Element, sAttrName);
	}
	auto nR1 = s_sHexDigits.find(sHex[nHexIdx + 0]); assert(nR1 != std::string::npos);
	auto nR2 = s_sHexDigits.find(sHex[nHexIdx + 1]); assert(nR2 != std::string::npos);
	auto nG1 = s_sHexDigits.find(sHex[nHexIdx + 2]); assert(nG1 != std::string::npos);
	auto nG2 = s_sHexDigits.find(sHex[nHexIdx + 3]); assert(nG2 != std::string::npos);
	auto nB1 = s_sHexDigits.find(sHex[nHexIdx + 4]); assert(nB1 != std::string::npos);
	auto nB2 = s_sHexDigits.find(sHex[nHexIdx + 5]); assert(nB2 != std::string::npos);
	RgbColor oRgb;
	oRgb.m_nR = (static_cast<uint8_t>(nR1) << 4) | static_cast<uint8_t>(nR2);
	oRgb.m_nG = (static_cast<uint8_t>(nG1) << 4) | static_cast<uint8_t>(nG2);
	oRgb.m_nB = (static_cast<uint8_t>(nB1) << 4) | static_cast<uint8_t>(nB2);
	return oRgb;
}

bool strToBool(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName, const std::string& sTF)
{
	auto oPair = Util::strToBool(sTF);
	std::string& sError = oPair.second;
	if (! sError.empty()) {
		throw XmlCommonErrors::error(oCtx, p0Element, sAttrName, (sAttrName.empty() ? "" : "Attribute '" + sAttrName + "':") + sError);
	}
	return oPair.first;
}

void assertTrue(bool
								#ifndef NDEBUG
								bCond
								#endif //NDEBUG
								)
{
	assert(bCond);
}

} // namespace XmlUtil

} // namespace stmg
