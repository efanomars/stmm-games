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
 * File:   xmlblockparser.cc
 */

#include "xmlblockparser.h"

#include "gamectx.h"
#include "xmlutile/xmlidtilesparser.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games/util/util.h>
#include <stmm-games/tile.h>
#include <stmm-games/util/namedobjindex.h>
#include <stmm-games/block.h>

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>

#include <vector>
#include <cassert>
#include <algorithm>
//#include <iostream>
#include <set>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <utility>


namespace stmg
{

const std::string XmlBlockParser::s_sBlockNodeName = "Block";
static const std::string s_sBlockInternalNameAttr = "internalName";
static const std::string s_sBlockDescriptionNodeName = "Description";
static const std::string s_sBlockBricksNodeName = "Bricks";
static const std::string s_sBlockBricksDefaultBrickNodeName = "DefaultTile";
static const std::string s_sBlockBricksBrickNodeName = "Tile";
static const std::string s_sBlockBricksBrickIdAttr = "idChar";
static const std::string s_sBlockShapesNodeName = "Shapes";
static const std::string s_sBlockShapesSeqsAttr = "seqs";
static const std::string s_sBlockShapesWHAttr = "wh";
static const std::string s_sBlockShapesEmptyCharAttr = "emptyId";
static const std::string s_sBlockShapeNodeName = "Shape";
static const std::string s_sBlockShapeHiddenToAttr = "hiddenTo";
static const std::string s_sBlockShapeLineNodeName = "Line";

const std::string::value_type XmlBlockParser::s_sDefaultEmptyChar = ' ';


void XmlBlockParser::parseBlocks(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sBlockNodeName, [&](const xmlpp::Element* p0Block)
	{
		std::string sName;
		Block oBlock = parseBlock(oCtx, p0Block, sName);
//#ifndef NDEBUG
//std::cout << "parseBlocks  ";
//oBlock.dump();
//#endif //NDEBUG
		if (sName.empty()) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Block, s_sBlockInternalNameAttr);
		}
		oCtx.getNamedBlocks().addNamedObj(std::move(sName), std::move(oBlock));
	});
	oCtx.removeChecker(p0Element, true);
}

Block XmlBlockParser::parseBlock(GameCtx& oCtx, const xmlpp::Element* p0RootElement)
{
	std::string sUnnamed;
	return parseBlock(oCtx, p0RootElement, sUnnamed);
}
Block XmlBlockParser::parseBlock(GameCtx& oCtx, const xmlpp::Element* p0RootElement, std::string& sName)
{
	assert(p0RootElement != nullptr);
	oCtx.addChecker(p0RootElement);
	oCtx.addValidChildElementNames(p0RootElement, s_sBlockDescriptionNodeName);
	const auto oPairName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0RootElement, s_sBlockInternalNameAttr);
	if (oPairName.first) {
		sName = oPairName.second;
	} else {
		sName.clear();
	}

	const xmlpp::Element* p0Shapes = m_oXmlConditionalParser.parseUniqueElement(oCtx, p0RootElement, s_sBlockShapesNodeName, true);

	std::vector<uint32_t> aBrickIds;
	std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > > aShapeBrickPos;
	bool bGen;
	int32_t nWH;
	int32_t nTotShapes;

	parseShapes(oCtx, p0Shapes, aBrickIds
				, aShapeBrickPos
				, bGen, nWH, nTotShapes);

	const xmlpp::Element* p0Bricks = m_oXmlConditionalParser.parseUniqueElement(oCtx, p0RootElement, s_sBlockBricksNodeName, true);

	XmlIdTilesParser oXmlIdTilesParser{m_oXmlConditionalParser, m_oXmlTraitsParser};
	const XmlIdTilesParser::IdTiles oIdTiles = oXmlIdTilesParser.parseIdTiles(oCtx, p0Bricks);

	const std::string& sDefinedBrickId = oIdTiles.m_sTileIds;
	const std::vector<Tile>& aDefinedBrick = oIdTiles.m_aTiles;
	const Tile& oDefaultBrick = oIdTiles.m_oDefaultTile;
	const bool bDefaultBrickDefined = ! oDefaultBrick.isEmpty();
//	std::string aDefinedBrickId;
//	std::vector<Tile> aDefinedBrick;
//	Tile oDefaultBrick;
//
//	parseBricks(oCtx, p0Bricks, aDefinedBrickId, aDefinedBrick, oDefaultBrick);

	std::vector<Tile> aBrick;
	const int32_t nTotBricks = static_cast<int32_t>(aBrickIds.size());
	aBrick.resize(nTotBricks);
	for (int32_t nIdx = 0; nIdx < nTotBricks; ++nIdx) {
		std::string::value_type cId = aBrickIds[nIdx];
		const auto nFoundPos = sDefinedBrickId.find(cId);
		if (nFoundPos == std::string::npos) {
			if (! bDefaultBrickDefined) {
				throw XmlCommonErrors::error(oCtx, p0Bricks, s_sBlockBricksNodeName, Util::stringCompose("element '%1' was not defined."
																				, s_sBlockBricksDefaultBrickNodeName));
			}
			aBrick[nIdx] = oDefaultBrick;
		} else {
			aBrick[nIdx] = aDefinedBrick[nFoundPos];
		}
	}
//std::cout << "nTotBricks=" << nTotBricks << '\n';

	oCtx.removeChecker(p0RootElement, true);
	if (bGen) {
		//const std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > >& aShapeBrickPos;
		assert(aShapeBrickPos.size() == 1);
		return Block(nTotBricks, aBrick, aShapeBrickPos[0], nWH);
	} else {
		return Block(nTotBricks, aBrick, nTotShapes, aShapeBrickPos);
	}
}
static std::runtime_error errorAttrSingleCharOrUndefined(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr)
{
	return XmlCommonErrors::error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1' must be a single character or undefined", sAttr));
}
void XmlBlockParser::parseShapes(GameCtx& oCtx, const xmlpp::Element* p0Element, std::vector<uint32_t>& aBricks
				, std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > >& aShapeBrickPos
				, bool& bGenerate, int32_t& nWH, int32_t& nTotShapes)
{
	oCtx.addChecker(p0Element);
	const auto oPairSeqs = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sBlockShapesSeqsAttr);
	const std::string sSeqs = (oPairSeqs.first ? oPairSeqs.second : "");
	int32_t nTotSeqs = 0;
	bGenerate = false;
	if (!sSeqs.empty()) {
		nTotSeqs = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sBlockShapesSeqsAttr, sSeqs, false, true, 0, false, -1);
	}
	if (nTotSeqs == 0) {
		bGenerate = true;
		nTotSeqs = 1;
	}
//std::cout << "nTotSeqs=" << nTotSeqs << '\n';
	const auto oPairWH = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sBlockShapesWHAttr);
	const std::string sWH = (oPairWH.first ? oPairWH.second : "");
	nWH = 0;
	if (!sWH.empty()) {
		nWH = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sBlockShapesWHAttr, sWH, false, true, 0, false, -1);
	}
//std::cout << "nWH=" << nWH << '\n';
	std::string::value_type cEmpty = 0;
	const auto oPairEmptyChar = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sBlockShapesEmptyCharAttr);
	const std::string sEmptyChar = (oPairEmptyChar.first ? oPairEmptyChar.second : "");
	if (sEmptyChar.length() > 1) {
		throw errorAttrSingleCharOrUndefined(oCtx, p0Element, s_sBlockShapesEmptyCharAttr);
	} else if (!sEmptyChar.empty()) {
		cEmpty = sEmptyChar.at(0);
	}
	if (cEmpty == 0) {
		cEmpty = s_sDefaultEmptyChar;
	}
//std::cout << "cEmpty='" << cEmpty << "'" << '\n';
	nTotShapes = 0;
	std::vector< std::vector<int32_t> > aaPosX;
	std::vector< std::vector<int32_t> > aaPosY;
	std::vector<std::vector<uint32_t>> aaBrickId;
	std::vector<uint32_t> aHiddenTo;
	std::set<uint32_t> oBrickIds;
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sBlockShapeNodeName, [&](const xmlpp::Element* p0Shape)
	{
		std::vector<int32_t> aPosX;
		std::vector<int32_t> aPosY;
		std::vector<uint32_t> aBrickId;
		uint32_t cHiddenTo;
		parseShape(oCtx, p0Shape, cEmpty, aPosX, aPosY, aBrickId, cHiddenTo);
		aaPosX.push_back(aPosX);
		aaPosY.push_back(aPosY);
		aaBrickId.push_back(aBrickId);
		for (auto cUtf8 : aBrickId) {
			oBrickIds.insert(cUtf8);
		}
		aHiddenTo.push_back(cHiddenTo);
		++nTotShapes;
//std::cout << " NODE->cHiddenTo=" << cHiddenTo << '\n';
	});
//std::cout << "nTotShapes=" << nTotShapes << " (" << bGenerate << ")" << '\n';
	if (nTotShapes < 1) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("Expected element '%1'"
																			, s_sBlockShapeNodeName));
	}
	if (bGenerate) {
		if (nTotShapes != 1) {
			throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
														"expected exactly one element '%1'", s_sBlockShapeNodeName));
		}
	} else if (nTotShapes != nTotSeqs) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("Attribute '%1'"
													" does not match with nr of child nodes with name '%2'"
													, s_sBlockShapesSeqsAttr, s_sBlockShapeNodeName));
	}
	const int32_t nTotBricks = static_cast<int32_t>(oBrickIds.size());
	if (nTotBricks <= 0) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, "shape has no bricks");
	}
	aBricks.resize(0);
	aShapeBrickPos.resize(nTotShapes);
	for (auto& aBrickPos : aShapeBrickPos) {
		aBrickPos.resize(nTotBricks);
	}
//std::cout << "nTotBricks=" << nTotBricks << '\n';
	for (int32_t nBrick = 0; nBrick < nTotBricks; ++nBrick) {
//std::cout << "nBrick=" << nBrick << '\n';
		uint32_t cBrick = *(oBrickIds.begin());
//std::cout << "cBrick=" << cBrick << '\n';
		oBrickIds.erase(cBrick);
//std::cout << "->oBrickIds.size()=" << oBrickIds.size() << '\n';
		aBricks.push_back(cBrick);
		for (int32_t nCurShape = 0; nCurShape < nTotShapes; ++nCurShape) {
			bool bHidden = false;
//std::cout << "--->aaBrickId[nCurShape]=" << aaBrickId[nCurShape] << '\n';
			auto& aBrickId = aaBrickId[nCurShape];
			auto itFound = std::find(aBrickId.begin(), aBrickId.end(), cBrick);
			if (itFound == aBrickId.end()) {
//std::cout << "   >aHiddenTo[nCurShape]=" <<aHiddenTo[nCurShape] << '\n';
				itFound = std::find(aBrickId.begin(), aBrickId.end(), aHiddenTo[nCurShape]);
				bHidden = true;
				if (itFound == aBrickId.end()) {
					throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
												"Undefined bricks in element '%1', require attribute '%2'"
												, s_sBlockShapeNodeName, s_sBlockShapeHiddenToAttr));
				}
			}
			const auto nFound = std::distance(aBrickId.begin(), itFound);
			aShapeBrickPos[nCurShape][nBrick] = std::make_tuple(!bHidden, aaPosX[nCurShape][nFound], aaPosY[nCurShape][nFound]);
		}
	}
	assert(oBrickIds.size() == 0);
	oCtx.removeChecker(p0Element, true);
}
void XmlBlockParser::parseShape(GameCtx& oCtx, const xmlpp::Element* p0Element, uint32_t cEmpty
		, std::vector<int32_t>& aPosX, std::vector<int32_t>& aPosY
		, std::vector<uint32_t>& aBrickId, uint32_t& cHiddenTo)
{
//std::cout << "XmlBlockParser::parseShape(" << cEmpty << ")" << '\n';
	oCtx.addChecker(p0Element);
	const auto oPairHiddenTo = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sBlockShapeHiddenToAttr);
	const std::string sHiddenTo = (oPairHiddenTo.first ? oPairHiddenTo.second : "");
	if (sHiddenTo.length() > 1) {
		throw errorAttrSingleCharOrUndefined(oCtx, p0Element, s_sBlockShapeHiddenToAttr);
	} else if (sHiddenTo.empty()) {
		cHiddenTo = 0;
	} else {
		cHiddenTo = sHiddenTo.at(0);
	}
//std::cout << "  cHiddenTo='" << cHiddenTo << "'" << '\n';
	bool bAddedOne = false;
	int32_t nPosY = 0;
	bool hideToFound = false;
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sBlockShapeLineNodeName, [&](const xmlpp::Element* p0Line)
	{
		Glib::ustring sLine = parseLine(oCtx, p0Line);
		int32_t nPosX = 0;
		for (Glib::ustring::iterator itC = sLine.begin(); itC != sLine.end(); ++itC) {
			const Glib::ustring::value_type cUtf8 = *itC;
			if (cUtf8 != cEmpty) {
				aPosX.push_back(nPosX);
				aPosY.push_back(nPosY);
				auto itFound = std::find(aBrickId.begin(), aBrickId.end(), cUtf8);
				if (itFound != aBrickId.end()) {
					throw XmlCommonErrors::error(oCtx, p0Line, Util::s_sEmptyString, Util::stringCompose(
												"Tile id '%1' already defined for shape"
												, std::string(1, cUtf8)));
				}
				if (cHiddenTo == cUtf8) {
					hideToFound = true;
				}
				aBrickId.push_back(cUtf8);
			}
			++nPosX;
		}
		++nPosY;
		bAddedOne = true;
	});
	if (!bAddedOne) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0Element, s_sBlockShapeLineNodeName);
	}
	if ((cHiddenTo != 0) && (!hideToFound)) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("tile id defined in '%1'"
																	" was not defined in shape", s_sBlockShapeHiddenToAttr));
	}
	oCtx.removeChecker(p0Element, true);
}
std::string XmlBlockParser::parseLine(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	// no attributes, no child elements!
	oCtx.removeChecker(p0Element, true);
	const xmlpp::TextNode* p0LineText = p0Element->get_child_text();
	if (p0LineText == nullptr) {
		return "";
	} else {
		return p0LineText->get_content();
	}
}

} // namespace stmg
