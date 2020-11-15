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
 * File:   xmlgameinitparser.cc
 */

#include "xmlgameinitparser.h"

#include "gameinitctx.h"
#include "xmlgameinfoparser.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games-file/gameloader.h>

#include <stmm-games/util/util.h>
#include <stmm-games/level.h>
#include <stmm-games/tile.h>

#include <glibmm/ustring.h>

#include <libxml++/libxml++.h>

#include <vector>
#include <cassert>
//#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace stmg { class AppPreferences; }
namespace stmg { class ParserCtx; }

namespace stmg
{

static const std::string s_sLevelNodeName = "Level";
static const std::string s_sLevelBoardNodeName = "Board";
static const std::string s_sLevelBoardWidthAttr = "width";
	static const int32_t s_nLevelBoardWidthMin = 2;
	static const int32_t s_nLevelBoardWidthMax = 1000;
static const std::string s_sLevelBoardHeightAttr = "height";
	static const int32_t s_nLevelBoardHeightMin = 3;
	static const int32_t s_nLevelBoardHeightMax = 1000;
static const std::string s_sLevelBoardOverlaysNodeName = "Overlays";
static const std::string s_sLevelBoardOverlaysTilesNodeName = "Tiles";
static const std::string s_sLevelBoardOverlaysOverlayNodeName = "Overlay";
static const std::string s_sLevelBoardOverlaysOverlayXPosAttr = "x";
static const std::string s_sLevelBoardOverlaysOverlayYPosAttr = "y";
static const std::string s_sLevelBoardOverlaysOverlayEmptyCharAttr = "emptyId";
static const std::string s_sLevelBoardOverlaysOverlayTransparentCharAttr = "transparentId";
static const std::string s_sLevelBoardOverlaysOverlayLineNodeName = "Line";
static const std::string s_sLevelBoardSingleNodeName = "Single";
static const std::string s_sLevelBoardSingleXAttr = "x";
static const std::string s_sLevelBoardSingleYAttr = "y";
static const std::string s_sLevelBoardRectNodeName = "Rect";
static const std::string s_sLevelBoardRectXAttr = "x";
static const std::string s_sLevelBoardRectYAttr = "y";
static const std::string s_sLevelBoardRectWAttr = "w";
static const std::string s_sLevelBoardRectHAttr = "h";
static const std::string s_sLevelShowNodeName = "Show";
static const std::string s_sLevelShowWidthAttr = "width";
static const std::string s_sLevelShowHeightAttr = "height";
static const std::string s_sLevelShowInitXAttr = "initx";
static const std::string s_sLevelShowInitYAttr = "inity";
static const std::string s_sLevelSubshowNodeName = "Subshow";
static const std::string s_sLevelSubshowWidthAttr = "width";
static const std::string s_sLevelSubshowHeightAttr = "height";
static const std::string s_sLevelFallEachTicksAttr = "initialFallEachTicks";


const std::string::value_type XmlGameInitParser::s_cDefaultEmptyChar = ' ';

static std::runtime_error errorAttrMustBeSingleCharOrUndefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr)
{
	return XmlCommonErrors::error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1'"
															" must be a single character or undefined", sAttr));
}
static std::runtime_error errorElementTileNotDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sNodeName)
{
	throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("Tile not defined in %1", sNodeName));
}


GameLoader::GameInfo XmlGameInitParser::parseGameInit(GameInitCtx& oCtx, const xmlpp::Element* p0RootElement
													, const shared_ptr<AppPreferences>& refAppPreferences)
{
	static_cast<void>(refAppPreferences.get());
//std::cout << "XmlGameInitParser::parseGameInit" << '\n';
	assert(refAppPreferences);
	oCtx.addChecker(p0RootElement);
	std::string sName;
	const GameLoader::GameInfo& oGameInfo = m_oXmlGameInfoParser.parseGameInfo(oCtx, p0RootElement, sName);
	//oCtx.gameInfoSet(oGameInfo);
	const xmlpp::Element* p0Level = m_oXmlConditionalParser.parseUniqueElement(oCtx, p0RootElement, s_sLevelNodeName, true);
	parseLevelInit(oCtx, p0Level);

	const xmlpp::Element* p0Widgets = m_oXmlConditionalParser.parseUniqueElement(oCtx, p0RootElement, XmlLayoutParser::s_sGameWidgetsNodeName, false);
	const xmlpp::Element* p0Layout = m_oXmlConditionalParser.parseUniqueElement(oCtx, p0RootElement, XmlLayoutParser::s_sGameLayoutNodeName, false);
	auto refLayout = m_oXmlLayoutParser.parseLayout(oCtx, p0Widgets, p0Layout);

	oCtx.setName(sName);
	oCtx.setLayout(refLayout);

	oCtx.removeChecker(p0RootElement, true);
	return oGameInfo;
}

void XmlGameInitParser::parseLevelInit(GameInitCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	const xmlpp::Element* p0Board = m_oXmlConditionalParser.parseUniqueElement(oCtx, p0Element, s_sLevelBoardNodeName, true);
	parseBoard(oCtx, p0Board);

	const xmlpp::Element* p0Show = m_oXmlConditionalParser.parseUniqueElement(oCtx, p0Element, s_sLevelShowNodeName, false);
	if (p0Show != nullptr) {
		parseShow(oCtx, p0Show);
	}
	const xmlpp::Element* p0Subshow = m_oXmlConditionalParser.parseUniqueElement(oCtx, p0Element, s_sLevelSubshowNodeName, false);
	if (p0Subshow != nullptr) {
		parseSubshow(oCtx, p0Subshow);
	}
	const auto oPairFallEachTicks = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelFallEachTicksAttr);
	int32_t nInitialFallEachTicks = Level::Init{}.m_nInitialFallEachTicks;
	if (oPairFallEachTicks.first) {
		nInitialFallEachTicks = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelFallEachTicksAttr, oPairFallEachTicks.second, false
																	, true, 1, false, -1);
	}
//std::cout << "XmlGameInitParser::parseLevelInit nInitialFallEachTicks=" << nInitialFallEachTicks << '\n';
	oCtx.setInitialFallEachTicks(nInitialFallEachTicks);
	oCtx.removeChecker(p0Element, true);
}
void XmlGameInitParser::parseBoard(GameInitCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	const auto oPairBoardWidth = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelBoardWidthAttr);
	int32_t nBoardWidth = s_nLevelBoardWidthMin;
	if (oPairBoardWidth.first) {
		nBoardWidth = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelBoardWidthAttr, oPairBoardWidth.second, false
															, true, s_nLevelBoardWidthMin, true, s_nLevelBoardWidthMax);
	}
	const auto oPairBoardHeight =  m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelBoardHeightAttr);
	int32_t nBoardHeight = s_nLevelBoardHeightMin;
	if (oPairBoardHeight.first) {
		nBoardHeight = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelBoardHeightAttr, oPairBoardHeight.second, false
									, true, s_nLevelBoardHeightMin, true, s_nLevelBoardHeightMax);
	}

	oCtx.boardSetSize(nBoardWidth, nBoardHeight);

	m_oXmlConditionalParser.visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0Filler)
	{
		const std::string sFillerName = p0Filler->get_name();
		if (sFillerName == s_sLevelBoardOverlaysNodeName) {
			parseBoardOverlays(oCtx, p0Filler);
		} else if (sFillerName == s_sLevelBoardSingleNodeName) {
			parseBoardSingle(oCtx, p0Filler);
		} else if (sFillerName == s_sLevelBoardRectNodeName) {
			parseBoardRect(oCtx, p0Filler);
		} else {
			throw XmlCommonErrors::errorElementInvalid(oCtx, p0Element, sFillerName);
		}
	});
	// Child elements already checked in visitElementChildren
	oCtx.removeChecker(p0Element, false, true);
}
void XmlGameInitParser::parseBoardOverlays(GameInitCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlGameInitParser::parseBoardOverlays()" << '\n';
	oCtx.addChecker(p0Element);
	const xmlpp::Element* p0Tiles = m_oXmlConditionalParser.parseUniqueElement(oCtx, p0Element, s_sLevelBoardOverlaysTilesNodeName, true);

	XmlIdTilesParser oXmlIdTilesParser{m_oXmlConditionalParser, m_oXmlTraitsParser};
	XmlIdTilesParser::IdTiles oIdTiles = oXmlIdTilesParser.parseIdTiles(oCtx, p0Tiles);

	int32_t nTotOverlays = 0;
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sLevelBoardOverlaysOverlayNodeName, [&](const xmlpp::Element* p0Overlay)
	{
		parseBoardOverlaysOverlay(oCtx, p0Overlay, oIdTiles); //aTileIds, aTiles, oDefaultTile
		++nTotOverlays;
	});
	if (nTotOverlays < 1) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0Element, s_sLevelBoardOverlaysOverlayNodeName);
	}
	oCtx.removeChecker(p0Element, true);
}
void XmlGameInitParser::parseBoardOverlaysOverlay(GameInitCtx& oCtx, const xmlpp::Element* p0Element
												, const XmlIdTilesParser::IdTiles& oIdTiles)
{
//std::cout << "XmlGameInitParser::parseBoardOverlaysOverlay()" << '\n';
	oCtx.addChecker(p0Element);
	const auto oPairXPos =  m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelBoardOverlaysOverlayXPosAttr);
	int32_t nPosX = 0;
	if (oPairXPos.first) {
		nPosX = XmlUtil::strToNumber(oCtx, p0Element, s_sLevelBoardOverlaysOverlayXPosAttr, oPairXPos.second, false
							, true, 0, true,  oCtx.boardGetWidth() - 1);
	}
	const auto oPairYPos =  m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelBoardOverlaysOverlayYPosAttr);
	int32_t nPosY = 0;
	if (oPairYPos.first) {
		nPosY = XmlUtil::strToNumber(oCtx, p0Element, s_sLevelBoardOverlaysOverlayYPosAttr, oPairYPos.second, false
							, true, 0, true,  oCtx.boardGetHeight() - 1);
	}
	std::string::value_type cEmpty = 0;
	const auto oPairEmptyChar = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelBoardOverlaysOverlayEmptyCharAttr);
	if (oPairEmptyChar.first) {
		const std::string& sEmptyChar = oPairEmptyChar.second;
		if (sEmptyChar.size() > 1) {
			throw errorAttrMustBeSingleCharOrUndefined(oCtx, p0Element, s_sLevelBoardOverlaysOverlayEmptyCharAttr);
		} else if (!sEmptyChar.empty()) {
			cEmpty = sEmptyChar[0];
		}
	}
	if (cEmpty == 0) {
		cEmpty = s_cDefaultEmptyChar;
	}
	std::string::value_type cTransparent = 0;
	const auto oPairTransparent = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelBoardOverlaysOverlayTransparentCharAttr);
	if (oPairTransparent.first) {
		const std::string& sTransparent = oPairTransparent.second;
		if (sTransparent.size() > 1) {
			throw errorAttrMustBeSingleCharOrUndefined(oCtx, p0Element, s_sLevelBoardOverlaysOverlayTransparentCharAttr);
		} else if (!sTransparent.empty()) {
			cTransparent = sTransparent.at(0);
		}
	}

	const std::string& sTileIds = oIdTiles.m_sTileIds;
	const auto& aTiles = oIdTiles.m_aTiles;
	const Tile& oDefaultTile = oIdTiles.m_oDefaultTile;
	bool bAddedOne = false;
	const Tile& oEmptyTile = Tile::s_oEmptyTile;
	int32_t nLine = 0;
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sLevelBoardOverlaysOverlayLineNodeName, [&](const xmlpp::Element* p0Line)
	{
		bAddedOne = true;
		std::string sLine;
		parseBoardOverlaysOverlayLine(oCtx, p0Line, sLine);
		const int32_t nY = nPosY + nLine;
		const int32_t nLineLen = sLine.length();
		for (int32_t nIdx = 0; nIdx < nLineLen; ++nIdx) {
			const int32_t nX = nPosX + nIdx;
			if ((nX < oCtx.boardGetWidth()) && (nY < oCtx.boardGetHeight())) {
				const std::string::value_type cChId = sLine[nIdx];
//std::cout << "parseBoardOverlaysOverlay ch=" << chId << '\n';
				if (cChId == cEmpty) {
					oCtx.boardSetTile(nX, nY, oEmptyTile);
				} else if (cChId == cTransparent) {
				} else {
					const auto nFoundPos = sTileIds.find(cChId);
					const bool bCharFound = (nFoundPos != std::string::npos);
					const Tile& oTile = (bCharFound ? aTiles[nFoundPos] : oDefaultTile);
					oCtx.boardSetTile(nX, nY, oTile);
				}
			}
		}
		++nLine;
	});
	if (!bAddedOne) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0Element, s_sLevelBoardOverlaysOverlayLineNodeName);
	}
	oCtx.removeChecker(p0Element, true);
}
void XmlGameInitParser::parseBoardOverlaysOverlayLine(GameInitCtx& oCtx, const xmlpp::Element* p0Element, std::string& sLine)
{
//std::cout << "XmlGameInitParser::parseBoardOverlaysOverlayLine()" << '\n';
	oCtx.addChecker(p0Element);
	const xmlpp::TextNode* p0LineText = p0Element->get_child_text();
	if (p0LineText == nullptr) {
		sLine.clear();
	} else {
		sLine = p0LineText->get_content();
	}
	oCtx.removeChecker(p0Element, true);
}

void XmlGameInitParser::parseBoardSingle(GameInitCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "parseBoardSingle" << '\n';
	oCtx.addChecker(p0Element);
	const auto oPairSingleX = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelBoardSingleXAttr);
	int32_t nX = 0;
	if (oPairSingleX.first) {
		nX = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelBoardSingleXAttr, oPairSingleX.second, false
										, true, 0, true, oCtx.boardGetWidth() - 1);
	} else {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sLevelBoardSingleXAttr);
	}
	const auto oPairSingleY = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelBoardSingleYAttr);
	int32_t nY = 0;
	if (oPairSingleY.first) {
		nY = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelBoardSingleYAttr, oPairSingleY.second, false
										, true, 0, true, oCtx.boardGetHeight() - 1);
	} else {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sLevelBoardSingleYAttr);
	}
	Tile oTile;
	m_oXmlTraitsParser.parseTile(oCtx, p0Element, oTile);
	if (oTile.isEmpty()) {
		throw errorElementTileNotDefined(oCtx, p0Element, s_sLevelBoardSingleNodeName);
	}
	oCtx.boardSetTile(nX, nY, oTile);
	oCtx.removeChecker(p0Element, true);
}
void XmlGameInitParser::parseBoardRect(GameInitCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "parseBoardRect" << '\n';
	oCtx.addChecker(p0Element);
	const int32_t nBoardW = oCtx.boardGetWidth();
	const int32_t nBoardH = oCtx.boardGetHeight();
	//
	const auto oPairRectX = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelBoardRectXAttr);
	int32_t nX = 0;
	if (oPairRectX.first) {
		nX = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelBoardRectXAttr, oPairRectX.second, false
													, true, 0, true, nBoardW - 1);
	}

	const auto oPairRectY = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelBoardRectYAttr);
	int32_t nY = 0;
	if (oPairRectY.first) {
		nY = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelBoardRectYAttr, oPairRectY.second, false
													, true, 0, true, nBoardH - 1);
	}

	const auto oPairRectW = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelBoardRectWAttr);
	int32_t nW = nBoardW - nX;
	if (oPairRectW.first) {
		nW = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelBoardRectWAttr, oPairRectW.second, false
													, true, 1, true, nBoardW);
	}

	const auto oPairRectH = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelBoardRectHAttr);
	int32_t nH = nBoardH - nY;
	if (oPairRectH.first) {
		nH = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelBoardRectHAttr, oPairRectH.second, false
													, true, 1, true, nBoardH);
	}

	Tile oTile;
	m_oXmlTraitsParser.parseTile(oCtx, p0Element, oTile);
	if (oTile.isEmpty()) {
		throw errorElementTileNotDefined(oCtx, p0Element, s_sLevelBoardRectNodeName);
	}
	for (int32_t nXX = 0; nXX < nW; ++nXX) {
		for (int32_t nYY = 0; nYY < nH; ++nYY) {
			if ((nX + nXX < 0) || (nX + nXX >= nBoardW)
					|| (nY + nYY < 0) || (nY + nYY >= nBoardH)) {
				throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
													"Element '%1' out of bounds", s_sLevelBoardRectNodeName));
			}
			oCtx.boardSetTile(nX + nXX, nY + nYY, oTile);
		}
	}
	oCtx.removeChecker(p0Element, true);
}
void XmlGameInitParser::parseShow(GameInitCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlGameInitParser::parseShow" << '\n';
	oCtx.addChecker(p0Element);
	const int32_t nBoardW = oCtx.boardGetWidth();
	const int32_t nBoardH = oCtx.boardGetHeight();

	const auto oPairShowW = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelShowWidthAttr);
	int32_t nShowW = nBoardW;
	if (oPairShowW.first) {
		nShowW = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelShowWidthAttr, oPairShowW.second, false
										, true, s_nLevelBoardWidthMin, true, nBoardW);
	}
	const auto oPairShowH = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelShowHeightAttr);
	int32_t nShowH = nBoardH;
	if (oPairShowH.first) {
		nShowH = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelShowHeightAttr, oPairShowH.second, false
										, true, s_nLevelBoardHeightMin, true, nBoardH);
	}
	const auto oPairShowInitX = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelShowInitXAttr);
	int32_t nShowInitX = 0;
	if (oPairShowInitX.first) {
		nShowInitX = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelShowInitXAttr, oPairShowInitX.second, false
										, true, 0, true, nBoardW - nShowW - 1);
	}
	const auto oPairShowInitY = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelShowInitYAttr);
	int32_t nShowInitY = 0;
	if (oPairShowInitY.first) {
		nShowInitY = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelShowInitYAttr, oPairShowInitY.second, false
										, true, 0, true, nBoardH - nShowH - 1);
	}
	oCtx.showSet(nShowW, nShowH, nShowInitX, nShowInitY);
	oCtx.removeChecker(p0Element, true);
}
void XmlGameInitParser::parseSubshow(GameInitCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlGameInitParser::parseSubshow" << '\n';
	oCtx.addChecker(p0Element);
	const int32_t nShowW = oCtx.showGetWidth();
	const int32_t nShowH = oCtx.showGetHeight();

	const auto oPairSubshowW =  m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelSubshowWidthAttr);
	int32_t nSubshowW = nShowW;
	if (oPairSubshowW.first) {
		nSubshowW = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelSubshowWidthAttr, oPairSubshowW.second, false
										, true, 1, true, nShowW);
	}
	const auto oPairSubshowH =  m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sLevelSubshowHeightAttr);
	int32_t nSubshowH = nShowH;
	if (oPairSubshowH.first) {
		nSubshowH = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sLevelSubshowHeightAttr, oPairSubshowH.second, false
										, true, 1, true, nShowH);
	}
	oCtx.subshowSet(nSubshowW, nSubshowH);
	oCtx.removeChecker(p0Element, true);
}

} // namespace stmg
