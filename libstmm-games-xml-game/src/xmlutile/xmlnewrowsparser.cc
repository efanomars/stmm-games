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
 * File:   xmlnewrowsparser.cc
 */

#include "xmlutile/xmlnewrowsparser.h"

#include "gamectx.h"
#include "xmlutile/xmlprobtilegenparser.h"
#include "xmlutile/xmlidtilesparser.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/xmltraitsparser.h>

#include <stmm-games/level.h>
#include <stmm-games/game.h>
#include <stmm-games/tile.h>
#include <stmm-games/util/util.h>
#include <stmm-games/util/intset.h>
#include <stmm-games/utile/newrows.h>
#include <stmm-games/utile/randomtiles.h>

#include <libxml++/libxml++.h>

#include <glibmm/ustring.h>

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <utility>

#include <ctype.h>

namespace stmg
{

static const std::string s_sNewRowsRandomTilesNodeName = "RandomTiles";
static const std::string s_sNewRowsRandomTilesRandomIdAttr = "randomId";
static const std::string s_sNewRowsTilesNodeName = "Tiles";
static const std::string s_sNewRowsNewRowNodeName = "NewRow";

static const std::string s_sNewRowsNewRowFixedNodeName = "Fixed";
static const std::string s_sNewRowsNewRowFixedEmptyIdAttr = "emptyId";
	static const std::string::value_type s_cDefaultEmptyChar = ' ';
static const std::string s_sNewRowsNewRowSingleNodeName = "Single";
static const std::string s_sNewRowsNewRowRandNodeName = "Rand";
static const std::string s_sNewRowsNewRowRandRandomIdAttr = s_sNewRowsRandomTilesRandomIdAttr;
static const std::string s_sNewRowsNewRowLeaveEmptyAttr = "leaveEmpty";
static const std::string s_sNewRowsNewRowFillAttr = "fill";
static const std::string s_sNewRowsNewRowPositionAttr = "position";
static const std::string s_sNewRowsNewRowFromPositionAttr = "fromPosition";
static const std::string s_sNewRowsNewRowToPositionAttr = "toPosition";
static const std::string s_sNewRowsNewRowPositionsAttr = "positions";

NewRows XmlNewRowsParser::parseNewRows(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	NewRows::Init oNewRowsInit;
	XmlProbTileGenParser oXmlProbTileGenParser{m_oXmlConditionalParser, m_oXmlTraitsParser};
	std::vector<std::string> aRandomIds;
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sNewRowsRandomTilesNodeName, [&](const xmlpp::Element* p0RandomTilesElement)
	{
		oCtx.addChecker(p0RandomTilesElement);
		//
		const auto oPairId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0RandomTilesElement, s_sNewRowsRandomTilesRandomIdAttr);
		if (oPairId.first) {
			aRandomIds.push_back(oPairId.second);
		} else {
			aRandomIds.emplace_back();
		}
		//
		RandomTiles::ProbTileGen oProbTileGen = oXmlProbTileGenParser.parseProbTileGen(oCtx, p0RandomTilesElement);
		oNewRowsInit.m_aRandomTiles.push_back(std::move(oProbTileGen));
		//
		oCtx.removeChecker(p0RandomTilesElement, false, true);
	});
	//
	XmlIdTilesParser::IdTiles oXmlIdTiles;
	const xmlpp::Element* p0Tiles = m_oXmlConditionalParser.parseUniqueElement(oCtx, p0Element, s_sNewRowsTilesNodeName, false);
	if (p0Tiles != nullptr) {
		XmlIdTilesParser oXmlIdTilesParser{m_oXmlConditionalParser, m_oXmlTraitsParser};
		oXmlIdTiles = oXmlIdTilesParser.parseIdTiles(oCtx, p0Tiles);
	}
	//
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sNewRowsNewRowNodeName, [&](const xmlpp::Element* p0NewRowElement)
	{
		NewRows::NewRowGen oNewRowGen = parseNewRow(oCtx, p0NewRowElement, oXmlIdTiles, aRandomIds);
		oNewRowsInit.m_aNewRowGens.push_back(std::move(oNewRowGen));
	});
	if (oNewRowsInit.m_aNewRowGens.empty()) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0Element, s_sNewRowsNewRowNodeName);
	}
	//
	oCtx.removeChecker(p0Element, true);
	//
	return NewRows{oCtx.game(), std::move(oNewRowsInit)};
}

NewRows::NewRowGen XmlNewRowsParser::parseNewRow(GameCtx& oCtx, const xmlpp::Element* p0Element
													, const XmlIdTilesParser::IdTiles& oXmlIdTiles, const std::vector<std::string>& aRandomIds)
{
	oCtx.addChecker(p0Element);
	NewRows::NewRowGen oNewRowGen;
	m_oXmlConditionalParser.visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0DistrElement)
	{
		const std::string sDistrName = p0DistrElement->get_name();
		unique_ptr<NewRows::Distr> refDistr = [&]()
		{
			if (sDistrName == s_sNewRowsNewRowFixedNodeName) {
				return parseDistrFixed(oCtx, p0DistrElement, oXmlIdTiles);
			} else if (sDistrName == s_sNewRowsNewRowSingleNodeName) {
				return parseDistrSingle(oCtx, p0DistrElement);
			} else if (sDistrName == s_sNewRowsNewRowRandNodeName) {
				return parseDistrRand(oCtx, p0DistrElement, aRandomIds);
			} else {
				throw XmlCommonErrors::error(oCtx, p0DistrElement, Util::s_sEmptyString, Util::stringCompose(
												"Element %1 is not a new row tile distr", sDistrName));
			}
		}();
		oCtx.addValidChildElementName(p0Element, sDistrName);
		oNewRowGen.m_aDistrs.emplace_back(std::move(refDistr));
	});
	if (oNewRowGen.m_aDistrs.empty()) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0Element, s_sNewRowsNewRowFixedNodeName);
	}
	oCtx.removeChecker(p0Element, true);
	return oNewRowGen;
}
static std::runtime_error errorAttrSingleCharOrUndefined(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr)
{
	return XmlCommonErrors::error(oCtx, p0Element, sAttr, Util::stringCompose("Attribute '%1' must be a single character or undefined", sAttr));
}

unique_ptr<NewRows::Distr> XmlNewRowsParser::parseDistrFixed(GameCtx& oCtx, const xmlpp::Element* p0Element
																, const XmlIdTilesParser::IdTiles& oXmlIdTiles)
{
	oCtx.addChecker(p0Element);
	auto refDistrFixed = std::make_unique<NewRows::DistrFixed>();
		//std::vector<Tile> m_aTiles; /**< At least one tile must be non empty. */
	const xmlpp::TextNode* p0RowText = p0Element->get_child_text();
	std::string sRow = ((p0RowText == nullptr) ? Util::s_sEmptyString : std::string{p0RowText->get_content()});
	//
	std::string::value_type cEmpty = 0;
	const auto oPairEmptyChar = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sNewRowsNewRowFixedEmptyIdAttr);
	const std::string sEmptyChar = (oPairEmptyChar.first ? oPairEmptyChar.second : "");
	if (sEmptyChar.size() > 1) {
		throw errorAttrSingleCharOrUndefined(oCtx, p0Element, s_sNewRowsNewRowFixedEmptyIdAttr);
	} else if (!sEmptyChar.empty()) {
		cEmpty = sEmptyChar.at(0);
	}
	if (cEmpty == 0) {
		cEmpty = s_cDefaultEmptyChar;
	}
	//
	const bool bDefaultDefined = (! oXmlIdTiles.m_oDefaultTile.isEmpty());
	bool bContainsNonSpace = false;
	//const auto nRowSize = sRow.size();
	for (const auto& c : sRow) {
		if (std::isspace(c) && (c != ' ')) {
			throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
												"Element %1 row contains an illegal whitespace character", s_sNewRowsNewRowFixedNodeName));
		}
		if (c != cEmpty) {
			const auto nPos = oXmlIdTiles.m_sTileIds.find(c);
			const bool bTileFound = (nPos != std::string::npos);
			if ((! bTileFound) && ! bDefaultDefined) {
				throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
													"Element %1 row contains undefined character '%2'", s_sNewRowsNewRowFixedNodeName, std::string{c}));
			}
			refDistrFixed->m_aTiles.push_back(bTileFound ? oXmlIdTiles.m_aTiles[nPos] : oXmlIdTiles.m_oDefaultTile);
			bContainsNonSpace = true;
		} else {
			refDistrFixed->m_aTiles.push_back(Tile::s_oEmptyTile);
		}
	}
	if (! bContainsNonSpace) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
											"Element %1 row must define at least one non empty tile", s_sNewRowsNewRowFixedNodeName));
	}
	oCtx.removeChecker(p0Element, true);
	return refDistrFixed;
}
unique_ptr<NewRows::Distr> XmlNewRowsParser::parseDistrSingle(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);

	IntSet oPositions = parsePositions(oCtx, p0Element);
	const bool bPositionsDefined = (oPositions.size() > 0);
	//
	int32_t nLeaveEmpty = (bPositionsDefined ? NewRows::DistrRandPosSingle{}.m_nLeaveEmpty : NewRows::DistrRandSingle{}.m_nLeaveEmpty);
	nLeaveEmpty = parseLeaveEmpty(oCtx, p0Element, nLeaveEmpty);
	//
	int32_t nFill = (bPositionsDefined ? NewRows::DistrRandPosSingle{}.m_nFill : NewRows::DistrRandSingle{}.m_nFill);
	nFill = parseFill(oCtx, p0Element, nFill);
	//
	Tile oTile;
	m_oXmlTraitsParser.parseTile(oCtx, p0Element, oTile);

	unique_ptr<NewRows::Distr> refDistr;
	if (bPositionsDefined) {
		auto refDistrPosSingle = std::make_unique<NewRows::DistrRandPosSingle>();
		refDistrPosSingle->m_nLeaveEmpty = nLeaveEmpty;
		refDistrPosSingle->m_nFill = nFill;
		refDistrPosSingle->m_oTile = oTile;
		refDistrPosSingle->m_oPositions = std::move(oPositions);
		refDistr = std::move(refDistrPosSingle);
	} else {
		auto refDistrSingle = std::make_unique<NewRows::DistrRandSingle>();
		refDistrSingle->m_nLeaveEmpty = nLeaveEmpty;
		refDistrSingle->m_nFill = nFill;
		refDistrSingle->m_oTile = oTile;
		refDistr = std::move(refDistrSingle);
	}
	oCtx.removeChecker(p0Element, true);
	return refDistr;
}
unique_ptr<NewRows::Distr> XmlNewRowsParser::parseDistrRand(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::vector<std::string>& aRandomIds)
{
	oCtx.addChecker(p0Element);

	IntSet oPositions = parsePositions(oCtx, p0Element);
	const bool bPositionsDefined = (oPositions.size() > 0);
	//
	int32_t nLeaveEmpty = (bPositionsDefined ? NewRows::DistrRandPosTiles{}.m_nLeaveEmpty : NewRows::DistrRandTiles{}.m_nLeaveEmpty);
	nLeaveEmpty = parseLeaveEmpty(oCtx, p0Element, nLeaveEmpty);
	//
	int32_t nFill = (bPositionsDefined ? NewRows::DistrRandPosTiles{}.m_nFill : NewRows::DistrRandTiles{}.m_nFill);
	nFill = parseFill(oCtx, p0Element, nFill);
	//
	const auto oPairRandomId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sNewRowsNewRowRandRandomIdAttr);
	if (! oPairRandomId.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sNewRowsNewRowRandRandomIdAttr);
	}
	const std::string& sRandomId = oPairRandomId.second;
	const auto itFind = std::find(aRandomIds.begin(), aRandomIds.end(), sRandomId);
	int32_t nRandomIdx = -1;
	if (itFind == aRandomIds.end()) {
		const int32_t nTotRandIds = static_cast<int32_t>(aRandomIds.size());
		nRandomIdx = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sNewRowsNewRowRandRandomIdAttr, sRandomId, false
															, true, 0, true, nTotRandIds - 1);
	} else {
		nRandomIdx = std::distance(aRandomIds.begin(), itFind);
	}
	//
	unique_ptr<NewRows::Distr> refDistr;
	if (bPositionsDefined) {
		auto refDistrPosTiles = std::make_unique<NewRows::DistrRandPosTiles>();
		refDistrPosTiles->m_nLeaveEmpty = nLeaveEmpty;
		refDistrPosTiles->m_nFill = nFill;
		refDistrPosTiles->m_nRandomTilesIdx = nRandomIdx;
		refDistrPosTiles->m_oPositions = std::move(oPositions);
		refDistr = std::move(refDistrPosTiles);
	} else {
		auto refDistrTiles = std::make_unique<NewRows::DistrRandTiles>();
		refDistrTiles->m_nLeaveEmpty = nLeaveEmpty;
		refDistrTiles->m_nFill = nFill;
		refDistrTiles->m_nRandomTilesIdx = nRandomIdx;
		refDistr = std::move(refDistrTiles);
	}
	//
	oCtx.removeChecker(p0Element, true);
	return refDistr;
}
IntSet XmlNewRowsParser::parsePositions(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	const int32_t nBoardW = oCtx.level().boardWidth();
	const auto oPairPositions = m_oXmlConditionalParser.parseIntSetAttributes(oCtx, p0Element, s_sNewRowsNewRowPositionAttr
								, s_sNewRowsNewRowFromPositionAttr, s_sNewRowsNewRowToPositionAttr, s_sNewRowsNewRowPositionsAttr
								, "", true, 0, true, nBoardW - 1);
	if (oPairPositions.first) {
		const IntSet& oPositions = oPairPositions.second;
		return oPositions; //---------------------------------------------------
	}
	return IntSet{};
}
int32_t XmlNewRowsParser::parseLeaveEmpty(GameCtx& oCtx, const xmlpp::Element* p0Element, int32_t nDefault)
{
	int32_t nLeaveEmpty = nDefault;
	const auto oPairLeaveEmpty = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sNewRowsNewRowLeaveEmptyAttr);
	if (oPairLeaveEmpty.first) {
		const std::string& sLeaveEmpty = oPairLeaveEmpty.second;
		nLeaveEmpty = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sNewRowsNewRowLeaveEmptyAttr, sLeaveEmpty, false
																		, true, 0, true, oCtx.level().boardWidth() - 1);
	}
	return nLeaveEmpty;
}
int32_t XmlNewRowsParser::parseFill(GameCtx& oCtx, const xmlpp::Element* p0Element, int32_t nDefault)
{
	int32_t nFill = nDefault;
	const auto oPairFill = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sNewRowsNewRowFillAttr);
	if (oPairFill.first) {
		const std::string& sFill = oPairFill.second;
		nFill = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sNewRowsNewRowFillAttr, sFill, false
																		, true, -1, true, oCtx.level().boardWidth() - 1);
	}
	return nFill;
}

} // namespace stmg
