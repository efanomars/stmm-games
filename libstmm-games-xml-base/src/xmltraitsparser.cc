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
 * File:   xmltraitsparser.cc
 */

#include "xmltraitsparser.h"

#include "conditionalctx.h"
#include "xmlcommonerrors.h"
#include "xmlcommonparser.h"
#include "xmlconditionalparser.h"
#include "xmlutil/xmlstrconv.h"

#include <stmm-games/named.h>
#include <stmm-games/tile.h>
#include <stmm-games/traitsets/alphatraitset.h>
#include <stmm-games/traitsets/chartraitset.h>
#include <stmm-games/traitsets/colortraitset.h>
#include <stmm-games/traitsets/fonttraitset.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/intset.h>
#include <stmm-games/util/namedindex.h>
#include <stmm-games/util/util.h>
#include <stmm-games/utile/tileselector.h>

#include <glibmm/ustring.h>

#include <libxml++/libxml++.h>

#include <glib.h>

#include <vector>
#include <cassert>
//#include <iostream>
#include <cstdint>
#include <utility>

#include <stdint.h>

namespace stmg
{

// In XML unicode characters can be defined with
//   &#x00e6; -> &#230;
//   &#48;    -> '0'
//
// In html colors are defined with
//   #FFDD33         -> rgb color
//   rgb(255,221,51) -> NOT SUPPORTED! Use %255^221^51 instead
//   blue
static const std::string s_sTileCharacterAttr = "character"; // "alpha" "'A'" "121" "'&amp;'" "'&apos;'" "0xF2"
static const std::string s_sTileCharAttr = "char"; // "A"
static const std::string s_sTileCharCodeAttr = "charCode"; // "121" "0xF2"
static const std::string s_sTileCharNameAttr = "charName";
static const std::string s_sTileColorAttr = "color"; // "blue" "#F2D209" "4"
static const std::string s_sTileColorPalAttr = "colorPal"; // "77" "8"
static const std::string s_sTileColorRgbAttr = "colorRgb"; // "%1^2^3" "#FFF" "#F078D4"
static const std::string s_sTileColorNameAttr = "colorName"; // "blue" "red"
static const std::string s_sTileFontNameAttr = "fontName"; // "arial"
static const std::string s_sTileAlphaAttr = "alpha"; // "255"
static const std::string s_sPlayerAttr = "player"; // "1"

static const std::string s_sTileCharactersAttr = "characters"; // "alpha,'A',121,'&amp;','&apos;',0xF2"
static const std::string s_sTileCharsAttr = "chars"; // "ABcd01z&quot;&lt;&gt;&#121;&#F2;"
static const std::string s_sTileCharCodesAttr = "charCodes"; // "121,1564,0xF2"
static const std::string s_sTileCharNamesAttr = "charNames"; // "alpha,bomb,hat"
static const std::string s_sTileColorsAttr = "colors"; // "blue,#F2D209,4,%1^254^1"
static const std::string s_sTileColorPalsAttr = "colorPals"; // "3,5,11"
static const std::string s_sTileColorRgbsAttr = "colorRgbs"; // "#F2D209,%1^254^1"
static const std::string s_sTileColorNamesAttr = "colorNames"; // "blue,red,yellow"
static const std::string s_sTileFontNamesAttr = "fontNames"; // "arial,monospace"
static const std::string s_sTileAlphasAttr = "alphas"; // "0,128,255"
static const std::string s_sPlayersAttr = "players"; // "0,2,4"

static const std::string s_sTileCharSeparatorAttr = "charSeparator";
static const std::string s_sTileColorSeparatorAttr = "colorSeparator";
static const std::string s_sTileFontSeparatorAttr = "fontSeparator";
static const std::string s_sTileAlphaSeparatorAttr = "alphaSeparator";
static const std::string s_sTilePlayerSeparatorAttr = "playerSeparator";

static const std::string s_sTileCharacterFromAttr = "fromCharacter";
static const std::string s_sTileCharacterToAttr = "toCharacter";
static const std::string s_sTileCharFromAttr = "fromChar";
static const std::string s_sTileCharToAttr = "toChar";
static const std::string s_sTileCharCodeFromAttr = "fromCharCode";
static const std::string s_sTileCharCodeToAttr = "toCharCode";
static const std::string s_sTileColorFromAttr = "fromColor";
static const std::string s_sTileColorToAttr = "toColor";
static const std::string s_sTileColorPalFromAttr = "fromColorPal";
static const std::string s_sTileColorPalToAttr = "toColorPal";
static const std::string s_sTileColorFromRgbAttr = "fromColorRgb";
static const std::string s_sTileColorToRgbAttr = "toColorRgb";
static const std::string s_sTileAlphaFromAttr = "fromAlpha";
static const std::string s_sTileAlphaToAttr = "toAlpha";
static const std::string s_sPlayerFromAttr = "fromPlayer";
static const std::string s_sPlayerToAttr = "toPlayer";

static const std::string s_sTileSelectorOperatorOrNodeName = "Or";
static const std::string s_sTileSelectorOperatorAndNodeName = "And";
static const std::string s_sTileSelectorOperatorNotNodeName = "Not";
static const std::string s_sTileSelectorTileNodeName = "Tile";
static const std::string s_sTileSelectorTraitCharNodeName = "Char";
static const std::string s_sTileSelectorTraitColorNodeName = "Color";
static const std::string s_sTileSelectorTraitFontNodeName = "Font";
static const std::string s_sTileSelectorTraitAlphaNodeName = "Alpha";
static const std::string s_sTileSelectorTraitPlayerNodeName = "Player";
static const std::string s_sTileSelectorTraitComplementAttr = "not";

static const std::string s_sNameAttrSeparators = ":-";

static const std::string s_sAnyAttrValue = "*";

static const std::string s_sImpossibleAttr = "</>";

static const std::string& chAttr(int32_t nNr, const std::string& sAttr0, const std::string& sAttr1
									, const std::string& sAttr2)
{
	assert((nNr >= 0) && (nNr <= 2));
	switch (nNr) {
		case 0: return sAttr0;
		case 1: return sAttr1;
		case 2: return sAttr2;
		default: return sAttr0;
	}
}
static const std::string& chAttr(int32_t nNr, const std::string& sAttr0, const std::string& sAttr1
									, const std::string& sAttr2, const std::string& sAttr3)
{
	assert((nNr >= 0) && (nNr <= 3));
	switch (nNr) {
		case 0: return sAttr0;
		case 1: return sAttr1;
		case 2: return sAttr2;
		case 3: return sAttr3;
		default: return sAttr0;
	}
}
const std::string& XmlTraitsParser::parseCharAttrs(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
													, const std::string& sCharacterAttr
													, const std::string& sCharAttr
													, const std::string& sCharCodeAttr
													, const std::string& sCharNameAttr
													, TileChar& oTileChar)
{
	oCtx.addChecker(p0Element);
	int32_t nDefinedAttr = -1;
	const auto oPairCharacter = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sCharacterAttr);
	if (oPairCharacter.first) {
		oTileChar = XmlUtil::strToTileChar(oCtx, p0Element, sCharacterAttr, oPairCharacter.second);
		nDefinedAttr = 0;
	}
	const auto oPairChar = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sCharAttr);
	if (oPairChar.first) {
		if (nDefinedAttr >= 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sCharAttr, sCharacterAttr);
		}
		const Glib::ustring sChar = oPairChar.second;
		if (sChar.length() != 1) {
			throw XmlCommonErrors::error(oCtx, p0Element, sCharAttr, "attribute '" + sCharAttr + "'"
													" must be a valid unicode character");
		}
		const uint32_t nCode = static_cast<uint32_t>(sChar[0]);
		if (! ((nCode >= TileChar::CHAR_UCS4_MIN) && (nCode <= TileChar::CHAR_UCS4_MAX))) {
			throw XmlCommonErrors::error(oCtx, p0Element, sCharAttr, Util::stringCompose("Error: attribute '%1'"
											" has invalid unicode value", sCharAttr));
		}
		oTileChar.setChar(nCode);
		nDefinedAttr = 1;
	}
	const auto oPairCode = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sCharCodeAttr);
	if (oPairCode.first) {
		if (nDefinedAttr >= 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sCharCodeAttr
														, ((nDefinedAttr == 0) ? sCharacterAttr: sCharAttr));
		}
		auto& sCode = oPairCode.second;
		const uint32_t nCode = XmlUtil::strToUint32(oCtx, p0Element, sCharCodeAttr, sCode
													, true, TileChar::CHAR_UCS4_MIN, true, TileChar::CHAR_UCS4_MAX);
		oTileChar.setChar(nCode);
		nDefinedAttr = 2;
	}
	const auto oPairName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sCharNameAttr);
	if (oPairName.first) {
		if (nDefinedAttr >= 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sCharNameAttr
										, chAttr(nDefinedAttr, sCharacterAttr, sCharAttr, sCharCodeAttr));
		}
		auto& sName = oPairName.second;
		const std::string sUpName = Glib::ustring{sName}.uppercase();
		XmlCommonParser::validateName(oCtx, p0Element, sCharNameAttr, sUpName, false);
		const int32_t nCharIndex = oCtx.named().chars().addName(sUpName);
		if (nCharIndex > TileChar::CHAR_INDEX_MAX) {
			throw XmlCommonErrors::errorAttrOneTooManyNames(oCtx, p0Element, sCharNameAttr);
		}
		oTileChar.setCharIndex(nCharIndex);
		nDefinedAttr = 3;
	}
	oCtx.removeChecker(p0Element, true);
	if (nDefinedAttr < 0) {
		return Util::s_sEmptyString; //-----------------------------------------
	}
	return chAttr(nDefinedAttr, sCharacterAttr, sCharAttr, sCharCodeAttr, sCharNameAttr);
}
const std::string& XmlTraitsParser::parseColorAttrs(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
													, const std::string& sColorAttr
													, const std::string& sColorPalAttr
													, const std::string& sColorRgbAttr
													, const std::string& sColorNameAttr
													, TileColor& oTileColor)
{
	oCtx.addChecker(p0Element);
	int32_t nDefinedAttr = -1;
	const auto oPairColor = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sColorAttr);
	if (oPairColor.first) {
		oTileColor = XmlUtil::strToTileColor(oCtx, p0Element, sColorAttr, oPairColor.second);
		nDefinedAttr = 0;
	}
	const auto oPairColorPal = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sColorPalAttr);
	if (oPairColorPal.first) {
		if (nDefinedAttr >= 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sColorPalAttr, sColorAttr);
		}
		const uint32_t nColorPal = XmlUtil::strToUint32(oCtx, p0Element, sColorPalAttr, oPairColorPal.second
							, true, TileColor::COLOR_PAL_FIRST, true, TileColor::COLOR_PAL_LAST);
		oTileColor.setColorPal(nColorPal);
		nDefinedAttr = 1;
	}
	const auto oPairColorRGB = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sColorRgbAttr);
	if (oPairColorRGB.first) {
		if (nDefinedAttr >= 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sColorRgbAttr
											, ((nDefinedAttr == 0) ? sColorAttr : sColorPalAttr));
		}
		const RgbColor oRgb = XmlUtil::strHexToRGB(oCtx, p0Element, sColorRgbAttr, oPairColorRGB.second);
		oTileColor.setColorRGB(oRgb.m_nR, oRgb.m_nG, oRgb.m_nB);
		nDefinedAttr = 2;
	}
	const auto oPairColorName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sColorNameAttr);
	if (oPairColorName.first) {
		if (nDefinedAttr >= 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sColorNameAttr
								, chAttr(nDefinedAttr, sColorAttr, sColorPalAttr, sColorRgbAttr));
		}
		//const std::string sUpName = Glib::ustring{sColorName}.uppercase();
		auto& sName = oPairColorName.second;
		XmlCommonParser::validateName(oCtx, p0Element, sColorNameAttr, sName, false);
		const int32_t nColorIndex = oCtx.named().colors().addName(sName);
		oTileColor.setColorIndex(nColorIndex);
		nDefinedAttr = 3;
	}
	oCtx.removeChecker(p0Element, true);
	if (nDefinedAttr < 0) {
		return Util::s_sEmptyString; //-----------------------------------------
	}
	return chAttr(nDefinedAttr, sColorAttr, sColorPalAttr, sColorRgbAttr, sColorNameAttr);
}
bool XmlTraitsParser::parseFontAttrs(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, TileFont& oTileFont)
{
	oCtx.addChecker(p0Element);
	bool bFontDefined = false;
	const auto oPairName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTileFontNameAttr);
	if (oPairName.first) {
		//const std::string sUpName = Glib::ustring{sName}.uppercase();
		auto& sName = oPairName.second;
		XmlCommonParser::validateName(oCtx, p0Element, s_sTileFontNameAttr, sName, false);
		const int32_t nFontIndex = oCtx.named().fonts().addName(sName);
		if (nFontIndex > TileFont::CHAR_FONT_INDEX_MAX) {
			throw XmlCommonErrors::errorAttrOneTooManyNames(oCtx, p0Element, s_sTileFontNameAttr);
		}
		oTileFont.setFontIndex(nFontIndex);
		bFontDefined = true;
	} else {
		oTileFont.clear();
	}
	oCtx.removeChecker(p0Element, true);
	return bFontDefined;
}
bool XmlTraitsParser::parseAlphaAttrs(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
									, const std::string& sAlphaAttr, TileAlpha& oTileAlpha)
{
	oCtx.addChecker(p0Element);
	bool bAlphaDefined = false;
	const auto oPairAlpha = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sAlphaAttr);
	if (oPairAlpha.first) {
		const uint32_t nA = XmlUtil::strToNumber<uint32_t>(oCtx, p0Element, sAlphaAttr, oPairAlpha.second, false
															, true, TileAlpha::ALPHA_MIN, true, TileAlpha::ALPHA_MAX);
		oTileAlpha.setAlpha(static_cast<uint8_t>(nA));
		bAlphaDefined = true;
	} else {
		oTileAlpha.clear();
	}
	oCtx.removeChecker(p0Element, true);
	return bAlphaDefined;
}

void XmlTraitsParser::parseCharsAttrs(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, CharTraitSet& oCharTraitSet
									, const std::string& sAlreadyAttr, const std::string& sAlreadyAttr2)
{
//std::cout << "XmlTraitsParser::parseCharsAttrs" << '\n';
	oCtx.addChecker(p0Element);
	int32_t nDefinedAttr = -1;
	const std::string sSeparator = XmlCommonParser::parseSeparator(oCtx, p0Element, s_sTileCharSeparatorAttr);
	std::string sStr;
	//
	const auto oPairCharacters = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTileCharactersAttr);
	if (oPairCharacters.first) {
		sStr = std::move(oPairCharacters.second);
		nDefinedAttr = 0;
	}
	const auto oPairChars = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTileCharsAttr);
	if (oPairChars.first) {
		if (nDefinedAttr >= 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sTileCharsAttr, s_sTileCharactersAttr);
		}
		sStr = std::move(oPairChars.second);
		nDefinedAttr = 1;
	}
	const auto oPairCodes = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTileCharCodesAttr);
	if (oPairCodes.first) {
		if (nDefinedAttr >= 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sTileCharNamesAttr
									, ((nDefinedAttr == 0) ? s_sTileCharactersAttr : s_sTileCharsAttr));
		}
		sStr = std::move(oPairCodes.second);
		nDefinedAttr = 2;
	}
	const auto oPairNames = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTileCharNamesAttr);
	if (oPairNames.first) {
		if (nDefinedAttr >= 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sTileCharactersAttr
									, chAttr(nDefinedAttr, s_sTileCharactersAttr, s_sTileCharsAttr, s_sTileCharCodesAttr));
		}
		sStr = std::move(oPairNames.second);
		nDefinedAttr = 3;
	}
	oCtx.removeChecker(p0Element, true);
	if (nDefinedAttr == -1) {
		return; //--------------------------------------------------------------
	}
	auto& sTheAttr = chAttr(nDefinedAttr, s_sTileCharactersAttr, s_sTileCharsAttr, s_sTileCharCodesAttr, s_sTileCharNamesAttr);
	if (! sAlreadyAttr.empty()) {
		if (sAlreadyAttr2.empty()) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sTheAttr, sAlreadyAttr);
		} else {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByOthers(oCtx, p0Element, sTheAttr, sAlreadyAttr, sAlreadyAttr2);
		}
	}
	std::vector<TileChar> aTileChars;
	if (nDefinedAttr != 1) {
		XmlUtil::tokenizer(sStr, sSeparator, [&](const std::string& sToken)
		{
			if (sToken.empty()) {
				throw XmlCommonErrors::errorAttrCannotHaveEmptyValue(oCtx, p0Element
									, chAttr(nDefinedAttr, s_sTileCharactersAttr, s_sTileCharsAttr, s_sTileCharCodesAttr, s_sTileCharNamesAttr));
			}
			if (nDefinedAttr != 0) {
				const char c = sToken[0];
				if (nDefinedAttr == 2) {
					if (! ((c >= '0') && (c <= '9'))) {
						throw XmlCommonErrors::errorAttrNotWellFormedNumber(oCtx, p0Element, s_sTileCharCodesAttr);
					}
				} else if (nDefinedAttr == 3) {
					if (!Glib::Unicode::isalpha(c)) {
						throw XmlCommonErrors::error(oCtx, p0Element, s_sTileCharNamesAttr
										, Util::stringCompose("Attribute '%1': name must start with a letter", s_sTileCharNamesAttr));
					}
				} else {
					assert(false);
				}
			}
			TileChar oTileChar = XmlUtil::strToTileChar(oCtx, p0Element, s_sTileCharactersAttr, sToken);
			aTileChars.push_back(oTileChar);
		});
	} else {
		Glib::ustring sUStr(sStr);
		const auto nUSize = sUStr.size();
		for (Glib::ustring::size_type nIdx = 0; nIdx < nUSize; ++nIdx) {
			const gunichar c = sUStr[nIdx];
			const uint32_t nCode = static_cast<uint32_t>(c);
			if (! ((nCode >= TileChar::CHAR_UCS4_MIN) && (nCode <= TileChar::CHAR_UCS4_MAX))) {
				throw XmlCommonErrors::errorAttrUTF8CharInvalid(oCtx, p0Element, s_sTileCharsAttr, std::to_string(nCode));
			}
			TileChar oTileChar;
			oTileChar.setChar(nCode);
			aTileChars.push_back(oTileChar);
		}
	}
	oCharTraitSet = CharTraitSet{aTileChars};
}
void XmlTraitsParser::parseMultiChar(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, CharTraitSet& oCharTraitSet)
{
	oCtx.addChecker(p0Element);
	int32_t nDefinedAttr = -1;
	TileChar oChar;
	const std::string& sOneAttr = parseCharAttrs(oCtx, p0Element, s_sTileCharactersAttr
												, s_sTileCharAttr, s_sTileCharCodeAttr, s_sTileCharNameAttr, oChar);
	if (!sOneAttr.empty()) {
		oCharTraitSet = CharTraitSet{{oChar}};
		nDefinedAttr = 0;
	}
	TileChar oFromChar, oToChar;
	const std::string& sFromAttr = parseCharAttrs(oCtx, p0Element, s_sTileCharacterFromAttr, s_sTileCharFromAttr
												, s_sTileCharCodeFromAttr, s_sImpossibleAttr, oChar);
	if (!sFromAttr.empty()) {
		if (nDefinedAttr >= 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sFromAttr, sOneAttr);
		}
		if (oChar.isCharIndex()) {
			throw XmlCommonErrors::errorAttrCannotDefineRangeOfNames(oCtx, p0Element, sFromAttr);
		}
		oFromChar = oChar;
		nDefinedAttr = 1;
	}
	const std::string& sToAttr = parseCharAttrs(oCtx, p0Element, s_sTileCharacterToAttr, s_sTileCharToAttr
												, s_sTileCharCodeToAttr, s_sImpossibleAttr, oChar);
	if (!sToAttr.empty()) {
		if (nDefinedAttr == 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sToAttr, sOneAttr);
		}
		if (oChar.isCharIndex()) {
			throw XmlCommonErrors::errorAttrCannotDefineRangeOfNames(oCtx, p0Element, sToAttr);
		}
		oToChar = oChar;
		if (nDefinedAttr == -1) {
			oFromChar.setChar(TileChar::CHAR_UCS4_MIN);
		}
		nDefinedAttr = 1;
	} else {
		if (nDefinedAttr == 1) {
			oToChar.setChar(TileChar::CHAR_UCS4_MAX);
		}
	}
	if (nDefinedAttr == 1) {
		oCharTraitSet = CharTraitSet{std::make_unique<CharUcs4TraitSet>(
							static_cast<uint32_t>(oFromChar.getChar()), static_cast<uint32_t>(oToChar.getChar()))};
	}
	//parse chars
	parseCharsAttrs(oCtx, p0Element, oCharTraitSet
					, ((nDefinedAttr < 0) ? "" : ((nDefinedAttr == 0) ? sOneAttr : sFromAttr)), sToAttr);
	oCtx.removeChecker(p0Element, true);
}

void XmlTraitsParser::parseColorsAttrs(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, ColorTraitSet& oColorTraitSet
										, const std::string& sAlreadyAttr, const std::string& sAlreadyAttr2)
{
	oCtx.addChecker(p0Element);
	int32_t nDefinedAttr = -1;
	const std::string sSeparator = XmlCommonParser::parseSeparator(oCtx, p0Element, s_sTileColorSeparatorAttr);
	std::string sStr;
	//
	const auto oPairColors = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTileColorsAttr);
	if (oPairColors.first) {
		sStr = std::move(oPairColors.second);
		nDefinedAttr = 0;
	}
	const auto oPairColorPals = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTileColorPalsAttr);
	if (oPairColorPals.first) {
		if (nDefinedAttr >= 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sTileColorPalsAttr, s_sTileColorsAttr);
		}
		sStr = std::move(oPairColorPals.second);
		nDefinedAttr = 1;
	}
	const auto oPairRgbs = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTileColorRgbsAttr);
	if (oPairRgbs.first) {
		if (nDefinedAttr >= 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sTileColorRgbsAttr
											, ((nDefinedAttr == 0) ? s_sTileColorsAttr : s_sTileColorPalsAttr) );
		}
		sStr = std::move(oPairRgbs.second);
		nDefinedAttr = 2;
	}
	const auto oPairNames = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTileColorNamesAttr);
	if (oPairNames.first) {
		if (nDefinedAttr >= 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sTileColorNamesAttr
							, chAttr(nDefinedAttr, s_sTileColorsAttr, s_sTileColorPalsAttr, s_sTileColorRgbsAttr) );
		}
		sStr = std::move(oPairNames.second);
		nDefinedAttr = 3;
	}
	oCtx.removeChecker(p0Element, true);
	if (nDefinedAttr == -1) {
		return; //--------------------------------------------------------------
	}
	const std::string& sTheAttr = chAttr(nDefinedAttr, s_sTileColorsAttr, s_sTileColorPalsAttr, s_sTileColorRgbsAttr, s_sTileColorNamesAttr);
	if (! sAlreadyAttr.empty()) {
		if (sAlreadyAttr2.empty()) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sTheAttr, sAlreadyAttr);
		} else {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByOthers(oCtx, p0Element, sTheAttr, sAlreadyAttr, sAlreadyAttr2);
		}
	}
	std::vector<TileColor> aTileColors;
	XmlUtil::tokenizer(sStr, sSeparator, [&](const std::string& sToken)
	{
		if (sToken.empty()) {
			throw XmlCommonErrors::errorAttrCannotHaveEmptyValue(oCtx, p0Element, sTheAttr);
		}
		if (nDefinedAttr != 0) {
			const char c = sToken[0];
			if (nDefinedAttr == 1) {
				if ( !((c >= '0') && (c <= '9'))) {
					throw XmlCommonErrors::errorAttrNotWellFormedNumber(oCtx, p0Element, s_sTileColorPalsAttr);
				}
			} else if (nDefinedAttr == 2) {
				if (! ((c == '#') || (c == '%'))) {
					throw XmlCommonErrors::error(oCtx, p0Element, s_sTileColorRgbsAttr
									, Util::stringCompose("attribute '%1' not valid RGB color", s_sTileColorRgbsAttr));
				}
			} else if (nDefinedAttr == 3) {
				if (!Glib::Unicode::isalpha(c)) {
					throw XmlCommonErrors::error(oCtx, p0Element, s_sTileColorNamesAttr
									, Util::stringCompose("Attribute '%1': name must start with a letter", s_sTileColorNamesAttr));
				}
			} else {
				assert(false);
			}
		}
		TileColor oTileColor = XmlUtil::strToTileColor(oCtx, p0Element, sTheAttr, sToken);
		aTileColors.push_back(oTileColor);
	});
	oColorTraitSet = ColorTraitSet{aTileColors};
}
void XmlTraitsParser::parseMultiColor(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
									, bool bAllowRgbFromTo, ColorTraitSet& oColorTraitSet)
{
//std::cout << "XmlCommonParser::parseMultiColor" << '\n';
	oCtx.addChecker(p0Element);
	int32_t nDefinedAttr = -1;
	TileColor oColor;
	const std::string& sOneAttr = parseColorAttrs(oCtx, p0Element, s_sTileColorAttr, s_sTileColorPalAttr
												, s_sTileColorRgbAttr, s_sTileColorNameAttr, oColor);
	if (!sOneAttr.empty()) {
		oColorTraitSet = ColorTraitSet{{oColor}};
		nDefinedAttr = 0;
	}
	TileColor oFromColor, oToColor;
	const std::string& sFromAttr = parseColorAttrs(oCtx, p0Element, s_sTileColorFromAttr
										, s_sTileColorPalFromAttr
										, (bAllowRgbFromTo ? s_sTileColorFromRgbAttr : s_sImpossibleAttr)
										, s_sImpossibleAttr, oColor);
	if (!sFromAttr.empty()) {
		if (nDefinedAttr >= 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sFromAttr, sOneAttr);
		}
		if (oColor.getColorType() == TileColor::COLOR_TYPE_INDEX) {
			throw XmlCommonErrors::errorAttrCannotDefineRangeOfNames(oCtx, p0Element, sFromAttr);
		}
		oFromColor = oColor;
		assert(oFromColor.getColorType() != TileColor::COLOR_TYPE_INDEX);
		nDefinedAttr = 1;
	}
	const std::string& sToAttr = parseColorAttrs(oCtx, p0Element, s_sTileColorToAttr
										, s_sTileColorPalToAttr
										, (bAllowRgbFromTo ? s_sTileColorToRgbAttr : s_sImpossibleAttr)
										, s_sImpossibleAttr, oColor);
	if (!sToAttr.empty()) {
		if (nDefinedAttr == 0) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sToAttr, sOneAttr);
		}
		oToColor = oColor;
		const TileColor::COLOR_TYPE eColorType = oToColor.getColorType();
		if (eColorType == TileColor::COLOR_TYPE_INDEX) {
			throw XmlCommonErrors::errorAttrCannotDefineRangeOfNames(oCtx, p0Element, sToAttr);
		}
		if (nDefinedAttr == -1) { // from not defined
			if (eColorType == TileColor::COLOR_TYPE_PAL) {
				oFromColor.setColorPal(TileColor::COLOR_PAL_FIRST);
			} else if (eColorType == TileColor::COLOR_TYPE_RGB) {
				oFromColor.setColorRGB(0,0,0);
			} else if (eColorType == TileColor::COLOR_TYPE_INDEX) {
				assert(false);
			} else {
				assert(false);
			}
			nDefinedAttr = 1;
		} else {
			if (oFromColor.getColorType() != oToColor.getColorType()) {
				throw XmlCommonErrors::error(oCtx, p0Element, sToAttr, Util::stringCompose(
											"range attribute '%1' type not same as attribute '%2'"
											, sToAttr, sFromAttr));
			}
		}
	} else {
		if (nDefinedAttr == 1) { // from defined
			const TileColor::COLOR_TYPE eColorType = oFromColor.getColorType();
			if (eColorType == TileColor::COLOR_TYPE_PAL) {
				oToColor.setColorPal(TileColor::COLOR_PAL_LAST);
			} else if (eColorType == TileColor::COLOR_TYPE_RGB) {
				oToColor.setColorRGB(255,255,255);
			} else if (eColorType == TileColor::COLOR_TYPE_INDEX) {
				assert(false);
			} else {
				assert(false);
			}
		}
	}
	if (nDefinedAttr == 1) {
		if (oFromColor.getColorType() == TileColor::COLOR_TYPE_PAL) {
			oColorTraitSet = ColorTraitSet{std::make_unique<ColorPalTraitSet>(
							static_cast<uint32_t>(oFromColor.getColorPal()), static_cast<uint32_t>(oToColor.getColorPal()))};
		} else {
			assert(oFromColor.getColorType() == TileColor::COLOR_TYPE_RGB);
			uint8_t nFromR, nFromG, nFromB;
			oFromColor.getColorRGB(nFromR, nFromG, nFromB);
			uint8_t nToR, nToG, nToB;
			oToColor.getColorRGB(nToR, nToG, nToB);
			oColorTraitSet = ColorTraitSet{std::make_unique<ColorRgbTraitSet>(
								ColorRedTraitSet{nFromR, nToR}, ColorGreenTraitSet{nFromG, nToG}, ColorBlueTraitSet{nFromB, nToB})};
		}
	}
	//parse multi
	parseColorsAttrs(oCtx, p0Element, oColorTraitSet
					, ((nDefinedAttr < 0) ? "" : ((nDefinedAttr == 0) ? sOneAttr : sFromAttr)), sToAttr);
	oCtx.removeChecker(p0Element, true);
}

bool XmlTraitsParser::parseFontsAttrs(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, FontTraitSet& oFontTraitSet
							, const std::string& sAlreadyAttr)
{
	oCtx.addChecker(p0Element);

	const std::string sSeparator = XmlCommonParser::parseSeparator(oCtx, p0Element, s_sTileFontSeparatorAttr);
	//
	const auto oPairNames = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTileFontNamesAttr);
	const bool bNamesDefined = oPairNames.first;
	if (bNamesDefined) {
		if (!sAlreadyAttr.empty()) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sTileFontNamesAttr, sAlreadyAttr);
		}
		std::vector<int32_t> aValues;
		XmlUtil::tokenizer(oPairNames.second, sSeparator, [&](const std::string& sToken)
		{
			//const std::string sUpName = Glib::ustring{sToken}.uppercase();
			XmlCommonParser::validateName(oCtx, p0Element, s_sTileFontNamesAttr, sToken, false);
			const int32_t nFontIdx = oCtx.named().fonts().addName(sToken);
			if (! ((nFontIdx >= TileFont::CHAR_FONT_INDEX_MIN) && (nFontIdx <= TileFont::CHAR_FONT_INDEX_MAX))) {
				throw XmlCommonErrors::errorAttrOneTooManyNames(oCtx, p0Element, s_sTileFontNamesAttr);
			}
			aValues.push_back(nFontIdx);
		});
		oFontTraitSet = FontTraitSet{aValues};
	}
	//
	oCtx.removeChecker(p0Element, true);
	return bNamesDefined;
}
void XmlTraitsParser::parseMultiFont(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, FontTraitSet& oFontTraitSet)
{
	oCtx.addChecker(p0Element);
	TileFont oFont;
	const bool bFontDefined = parseFontAttrs(oCtx, p0Element, oFont);
	if (bFontDefined) {
		oFontTraitSet = FontTraitSet{oFont.getFontIndex()};
	}
	const bool bFontMultiDefined = parseFontsAttrs(oCtx, p0Element, oFontTraitSet, (bFontDefined ? s_sTileFontNameAttr : ""));
	if ((!bFontDefined) && !bFontMultiDefined) {
		oFontTraitSet = FontTraitSet{}; // alpha set with empty value!
	}
	oCtx.removeChecker(p0Element, true);
}

void XmlTraitsParser::parseMultiAlpha(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, AlphaTraitSet& oAlphaTraitSet)
{
	oCtx.addChecker(p0Element);
	auto oPairIntSet = m_oXmlConditionalParser.parseIntSetAttributes(oCtx, p0Element, s_sTileAlphaAttr, s_sTileAlphaFromAttr, s_sTileAlphaToAttr
																	, s_sTileAlphasAttr, s_sTileAlphaSeparatorAttr
																	, true, TileAlpha::ALPHA_MIN, true, TileAlpha::ALPHA_MAX);
	if (oPairIntSet.first) {
		oAlphaTraitSet = AlphaTraitSet(std::move(oPairIntSet.second), false);
	} else {
		oAlphaTraitSet = AlphaTraitSet{}; // set containing empty value
	}
	oCtx.removeChecker(p0Element, true);
}
void XmlTraitsParser::parseMultiPlayer(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, IntSet& oIntSet)
{
	oCtx.addChecker(p0Element);
	auto oPairIntSet = m_oXmlConditionalParser.parseIntSetAttributes(oCtx, p0Element, s_sPlayerAttr, s_sPlayerFromAttr, s_sPlayerToAttr
																	, s_sPlayersAttr, s_sTilePlayerSeparatorAttr
																	, true, -1, false, -1);
	if (oPairIntSet.first) {
		oIntSet = std::move(oPairIntSet.second);
	} else {
		oIntSet = IntSet{};
	}
	oCtx.removeChecker(p0Element, true);
}

bool XmlTraitsParser::parseTileSelectorNotAttr(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, bool& bNot)
{
	oCtx.addChecker(p0Element);
	const auto oPairNot = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTileSelectorTraitComplementAttr);
	const bool bNotDefined = oPairNot.first;
	if (bNotDefined) {
		bNot = XmlUtil::strToBool(oCtx, p0Element, s_sTileSelectorTraitComplementAttr, oPairNot.second);
	} else {
		bNot = false;
	}
	oCtx.removeChecker(p0Element, true);
	return bNotDefined;
}
std::unique_ptr<TileSelector::Trait> XmlTraitsParser::parseTileSelectorChar(ConditionalCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlTraitsParser::parseTileSelectorChar" << '\n';
	oCtx.addChecker(p0Element);
	bool bNot = false;
	parseTileSelectorNotAttr(oCtx, p0Element, bNot);
	auto refCharTraitSet = std::make_unique<CharTraitSet>();
	parseMultiChar(oCtx, p0Element, *refCharTraitSet);
	oCtx.removeChecker(p0Element, true);
	return std::make_unique<TileSelector::Trait>(bNot, std::move(refCharTraitSet));
}
std::unique_ptr<TileSelector::Trait> XmlTraitsParser::parseTileSelectorColor(ConditionalCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlCommonParser::parseTileSelectorColor()" << '\n';
	oCtx.addChecker(p0Element);
	bool bNot = false;
	parseTileSelectorNotAttr(oCtx, p0Element, bNot);
	auto refColorTraitSet = std::make_unique<ColorTraitSet>();
	parseMultiColor(oCtx, p0Element, true, *refColorTraitSet);
	oCtx.removeChecker(p0Element, true);
	return std::make_unique<TileSelector::Trait>(bNot, std::move(refColorTraitSet));
}
std::unique_ptr<TileSelector::Trait> XmlTraitsParser::parseTileSelectorFont(ConditionalCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	bool bNot = false;
	parseTileSelectorNotAttr(oCtx, p0Element, bNot);
	auto refFontTraitSet = std::make_unique<FontTraitSet>();
	parseMultiFont(oCtx, p0Element, *refFontTraitSet);
	oCtx.removeChecker(p0Element, true);
	return std::make_unique<TileSelector::Trait>(bNot, std::move(refFontTraitSet));
}
std::unique_ptr<TileSelector::Trait> XmlTraitsParser::parseTileSelectorAlpha(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
																			, bool bMandatory)
{
	oCtx.addChecker(p0Element);
	bool bNot = false;
	parseTileSelectorNotAttr(oCtx, p0Element, bNot);
	auto refAlphaTraitSet = std::make_unique<AlphaTraitSet>();
	parseMultiAlpha(oCtx, p0Element, *refAlphaTraitSet);
	if (refAlphaTraitSet->getTotValues() == 0) {
		if (bMandatory) {
			throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("One of attributes '%1', '%4'"
											" or '%2','%3' has to be defined", s_sTileAlphaAttr, s_sTileAlphaFromAttr
											, s_sTileAlphaToAttr, s_sTileAlphasAttr));
		}
	}
	oCtx.removeChecker(p0Element, true);
	return std::make_unique<TileSelector::Trait>(bNot, std::move(refAlphaTraitSet));
}
std::unique_ptr<TileSelector::Skin> XmlTraitsParser::parseTileSelectorPlayer(ConditionalCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	bool bNot = false;
	parseTileSelectorNotAttr(oCtx, p0Element, bNot);
	auto refPlayerIntSet = std::make_unique<IntSet>();
	parseMultiPlayer(oCtx, p0Element, *refPlayerIntSet);
	if (refPlayerIntSet->size() == 0) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("One of attributes '%1', '%4'"
										" or '%2','%3' has to be defined", s_sPlayerAttr, s_sPlayerFromAttr
										, s_sPlayerToAttr, s_sPlayersAttr));
	}
	oCtx.removeChecker(p0Element, true);
	return std::make_unique<TileSelector::Skin>(bNot, std::move(refPlayerIntSet));
}

std::unique_ptr<TileSelector::Operator> XmlTraitsParser::parseTileSelectorCond(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
																				, TileSelector::Operator::OP_TYPE eOpType)
{
	assert(p0Element != nullptr);
//std::cout << "XmlCommonParser::parseTileSelectorCond()" << '\n';
	oCtx.addChecker(p0Element);

	std::vector< std::unique_ptr<TileSelector::Operand> > aOperands;
	m_oXmlConditionalParser.visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0ItemElement)
	{
		oCtx.addChecker(p0ItemElement);
		const std::string sItemName = p0ItemElement->get_name();
		if ((sItemName == s_sTileSelectorOperatorOrNodeName)
					|| (sItemName == s_sTileSelectorOperatorAndNodeName)
					|| (sItemName == s_sTileSelectorOperatorNotNodeName)) {
			TileSelector::Operator::OP_TYPE eChildOpType;
			if (sItemName == s_sTileSelectorOperatorOrNodeName) {
				eChildOpType = TileSelector::Operator::OP_TYPE_OR;
			} else if (sItemName == s_sTileSelectorOperatorAndNodeName) {
				eChildOpType = TileSelector::Operator::OP_TYPE_AND;
			} else { //if (sItemName == s_sTileSelectorOperatorNotNodeName)
				eChildOpType = TileSelector::Operator::OP_TYPE_NOT;
			}
			aOperands.emplace_back(parseTileSelectorCond(oCtx, p0ItemElement, eChildOpType));
			//if (sItemName == s_sTileSelectorOperatorNotNodeName) {
			//	if (refOp->totOperands() != 1) {
			//		throw oCtx.raise(Util::stringCompose("TileSelector: node element <%1> needs exactly one operand", sItemName));
			//	}
			//}
		} else if (sItemName == s_sTileSelectorTileNodeName) {
			std::vector< std::unique_ptr<TileSelector::Operand> > aTileOperands;
			aTileOperands.emplace_back(parseTileSelectorChar(oCtx, p0ItemElement));
			aTileOperands.emplace_back(parseTileSelectorFont(oCtx, p0ItemElement));
			aTileOperands.emplace_back(parseTileSelectorColor(oCtx, p0ItemElement));
			aTileOperands.emplace_back(parseTileSelectorAlpha(oCtx, p0ItemElement, false));
			//
			aOperands.emplace_back(std::make_unique<TileSelector::Operator>(TileSelector::Operator::OP_TYPE_AND, aTileOperands));
		} else {
			if (sItemName == s_sTileSelectorTraitPlayerNodeName) {
				aOperands.emplace_back(parseTileSelectorPlayer(oCtx, p0ItemElement));
			} else {
				aOperands.emplace_back([&]()
				{
					if (sItemName == s_sTileSelectorTraitCharNodeName) {
						return parseTileSelectorChar(oCtx, p0ItemElement);
					} else if (sItemName == s_sTileSelectorTraitFontNodeName) {
						return parseTileSelectorFont(oCtx, p0ItemElement);
					} else if (sItemName == s_sTileSelectorTraitColorNodeName) {
						return parseTileSelectorColor(oCtx, p0ItemElement);
					} else if (sItemName == s_sTileSelectorTraitAlphaNodeName) {
						return parseTileSelectorAlpha(oCtx, p0ItemElement, true);
					} else {
						throw XmlCommonErrors::error(oCtx, p0ItemElement, Util::s_sEmptyString
													, Util::stringCompose("TileSelector: Unknown item '%1'", sItemName));
					}
				}());
			}
		}
		oCtx.removeChecker(p0ItemElement, false, true);
	});
	if (eOpType == TileSelector::Operator::OP_TYPE_NOT) {
		if (aOperands.size() != 1) {
			throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString
						, Util::stringCompose("TileSelector: node element <%1> needs exactly one operand"
													, s_sTileSelectorOperatorNotNodeName));
		}
	} else if (aOperands.empty()) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString
									, "TileSelector: node element needs at least one operand");
	}
	oCtx.removeChecker(p0Element, false, true);
	return std::make_unique<TileSelector::Operator>(eOpType, aOperands);
}

void XmlTraitsParser::parseTile(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, Tile& oTile)
{
	oCtx.addChecker(p0Element);
//std::cout << "XmlCommonParser::parseTile" << '\n';
	parseChar(oCtx, p0Element, oTile.getTileChar());
	parseColor(oCtx, p0Element, oTile.getTileColor());
	parseFont(oCtx, p0Element, oTile.getTileFont());
	parseAlpha(oCtx, p0Element, oTile.getTileAlpha());
	oCtx.removeChecker(p0Element, true);
}
void XmlTraitsParser::parseChar(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, TileChar& oTileChar)
{
	parseCharAttrs(oCtx, p0Element, s_sTileCharacterAttr, s_sTileCharAttr, s_sTileCharCodeAttr
					, s_sTileCharNameAttr, oTileChar);
}
void XmlTraitsParser::parseColor(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, TileColor& oTileColor)
{
	parseColorAttrs(oCtx, p0Element, s_sTileColorAttr, s_sTileColorPalAttr
					, s_sTileColorRgbAttr, s_sTileColorNameAttr, oTileColor);
}
void XmlTraitsParser::parseFont(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, TileFont& oTileFont)
{
	parseFontAttrs(oCtx, p0Element, oTileFont);
}
void XmlTraitsParser::parseAlpha(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, TileAlpha& oTileAlpha)
{
	parseAlphaAttrs(oCtx, p0Element, s_sTileAlphaAttr, oTileAlpha);
}

void XmlTraitsParser::parseChars(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, CharTraitSet& oTraitSet)
{
	parseMultiChar(oCtx, p0Element, oTraitSet);
}
void XmlTraitsParser::parseColors(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, bool bAllowRgbFromTo, ColorTraitSet& oTraitSet)
{
	parseMultiColor(oCtx, p0Element, bAllowRgbFromTo, oTraitSet);
}
void XmlTraitsParser::parseFonts(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, FontTraitSet& oTraitSet)
{
	parseMultiFont(oCtx, p0Element, oTraitSet);
}
void XmlTraitsParser::parseAlphas(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, AlphaTraitSet& oTraitSet)
{
	parseMultiAlpha(oCtx, p0Element, oTraitSet);
}
void XmlTraitsParser::parsePlayers(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, IntSet& oPlayersSet)
{
	parseMultiPlayer(oCtx, p0Element, oPlayersSet);
}


std::unique_ptr<TileSelector> XmlTraitsParser::parseTileSelectorOr(ConditionalCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlCommonParser::parseTileSelectorOr()" << '\n';
	std::unique_ptr<TileSelector::Operator> refRoot = parseTileSelectorCond(oCtx, p0Element, TileSelector::Operator::OP_TYPE_OR);
	return std::make_unique<TileSelector>(std::move(refRoot));
}
std::unique_ptr<TileSelector> XmlTraitsParser::parseTileSelectorAnd(ConditionalCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlCommonParser::parseTileSelectorAnd()" << '\n';
	std::unique_ptr<TileSelector::Operator> refRoot = parseTileSelectorCond(oCtx, p0Element, TileSelector::Operator::OP_TYPE_AND);
	return std::make_unique<TileSelector>(std::move(refRoot));
}

} // namespace stmg
