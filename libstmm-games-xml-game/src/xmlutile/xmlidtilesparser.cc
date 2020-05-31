/*
 * File:   xmlidtilesparser.cc
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

#include "xmlutile/xmlidtilesparser.h"

#include "conditionalctx.h"
#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmltraitsparser.h>

#include <stmm-games/util/util.h>
#include <stmm-games/tile.h>

#include <utility>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sIdTilesTileNodeName = "Tile";
static const std::string s_sIdTilesTileTileIdAttr = "idChar";
static const std::string s_sIdTilesDefaultTileNodeName = "DefaultTile";

XmlIdTilesParser::XmlIdTilesParser(XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser)
: m_oXmlConditionalParser(oXmlConditionalParser)
, m_oXmlTraitsParser(oXmlTraitsParser)
{
}
XmlIdTilesParser::IdTiles XmlIdTilesParser::parseIdTiles(ConditionalCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	IdTiles oIdTiles;
	//
	const xmlpp::Element* p0DefaultTileElement = m_oXmlConditionalParser.parseUniqueElement(oCtx, p0Element, s_sIdTilesDefaultTileNodeName, false);
	if (p0DefaultTileElement != nullptr) {
		m_oXmlTraitsParser.parseTile(oCtx, p0DefaultTileElement, oIdTiles.m_oDefaultTile);
	}

	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sIdTilesTileNodeName, [&](const xmlpp::Element* p0IdTile)
	{
		std::string::value_type cId;
		Tile oTile;
		parseIdTile(oCtx, p0IdTile, cId, oTile);
		const auto nFound = oIdTiles.m_sTileIds.find(cId);
		if (nFound != std::string::npos) {
			throw XmlCommonErrors::error(oCtx, p0IdTile, s_sIdTilesTileTileIdAttr, Util::stringCompose("Tile id"
														" '%1' was already defined", std::string(1, cId)));
		}
		oIdTiles.m_sTileIds.append(1, cId);
		oIdTiles.m_aTiles.push_back(oTile);
	});
	oCtx.removeChecker(p0Element, true);
	return oIdTiles;
}

void XmlIdTilesParser::parseIdTile(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
									, std::string::value_type& cId, Tile& oTile)
{
	oCtx.addChecker(p0Element);
	const auto oPairId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sIdTilesTileTileIdAttr);
	const std::string sId = (oPairId.first ? oPairId.second : "");
	if (sId.size() != 1) {
		const std::string& sAttr = s_sIdTilesTileTileIdAttr;
		throw XmlCommonErrors::error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1' must be a single (ascii) character", sAttr));
	}
	cId = sId[0];
	m_oXmlTraitsParser.parseTile(oCtx, p0Element, oTile);
	//
	oCtx.removeChecker(p0Element, true);
}


} // namespace stmg
