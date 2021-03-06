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
 * File:   xmlprobtilegenparser.h
 */

#ifndef STMG_XML_PROB_TILE_GEN_PARSER_H
#define STMG_XML_PROB_TILE_GEN_PARSER_H

#include <stmm-games/utile/randomtiles.h>

#include <string>

namespace stmg { class ConditionalCtx; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlTraitsParser; }
namespace xmlpp { class Element; }

namespace stmg
{

class XmlProbTileGenParser
{
public:
	XmlProbTileGenParser(XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser);
	explicit XmlProbTileGenParser(const std::string& sRandomNodeName
								, XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser);

	RandomTiles::ProbTileGen parseProbTileGen(ConditionalCtx& oCtx, const xmlpp::Element* p0Element);
	RandomTiles::ProbTraitSets parseRandom(ConditionalCtx& oCtx, const xmlpp::Element* p0Element);
private:
	std::string m_sRandomNodeName;
	XmlConditionalParser& m_oXmlConditionalParser;
	XmlTraitsParser& m_oXmlTraitsParser;
private:
	XmlProbTileGenParser(const XmlProbTileGenParser& oSource) = delete;
	XmlProbTileGenParser& operator=(const XmlProbTileGenParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_PROB_TILE_GEN_PARSER_H */

