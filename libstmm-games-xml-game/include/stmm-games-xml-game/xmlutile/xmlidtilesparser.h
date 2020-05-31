/*
 * File:   xmlidtilesparser.h
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

#ifndef STMG_XML_ID_TILES_PARSER_H
#define STMG_XML_ID_TILES_PARSER_H

#include <stmm-games/tile.h>

#include <string>
#include <memory>
#include <vector>

namespace stmg { class ConditionalCtx; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlTraitsParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlIdTilesParser
{
public:
	struct IdTiles
	{
		std::string m_sTileIds; /**< Size of string same as m_aTiles.size().
								 * Each char is the id for the corresponding tile in m_aTiles. */
		std::vector<Tile> m_aTiles; /**< The parsed (non empty) tiles. */
		Tile m_oDefaultTile; /**< The default tile. If an id is not defined in m_aTileIds
								 * use this tile. If empty no default is defined.  */
	};
	XmlIdTilesParser(XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser);
	IdTiles parseIdTiles(ConditionalCtx& oCtx, const xmlpp::Element* p0Element);
private:
	void parseIdTile(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
					, std::string::value_type& cId, Tile& oTile);
private:
	XmlConditionalParser& m_oXmlConditionalParser;
	XmlTraitsParser& m_oXmlTraitsParser;
private:
	XmlIdTilesParser(const XmlIdTilesParser& oSource) = delete;
	XmlIdTilesParser& operator=(const XmlIdTilesParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_ID_TILES_PARSER_H */
