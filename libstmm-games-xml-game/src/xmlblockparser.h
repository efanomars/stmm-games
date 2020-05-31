/*
 * File:   xmlblockparser.h
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

#ifndef STMG_XML_BLOCK_PARSER_H
#define STMG_XML_BLOCK_PARSER_H

#include <stmm-games-xml-base/xmltraitsparser.h>

#include <stmm-games/block.h>

#include <vector>
#include <string>
#include <tuple>

#include <stdint.h>

namespace stmg { class GameCtx; }
namespace stmg { class XmlConditionalParser; }
namespace xmlpp { class Element; }

namespace stmg
{

class XmlBlockParser
{
public:
	explicit XmlBlockParser(XmlTraitsParser& oXmlTraitsParser)
	: m_oXmlTraitsParser(oXmlTraitsParser)
	, m_oXmlConditionalParser(oXmlTraitsParser.getXmlConditionalParser())
	{
	}

	void parseBlocks(GameCtx& oCtx, const xmlpp::Element* p0Element);

	Block parseBlock(GameCtx& oCtx, const xmlpp::Element* p0Element);

	static const std::string::value_type s_sDefaultEmptyChar;
	static const std::string s_sBlockNodeName;

private:
	Block parseBlock(GameCtx& oCtx, const xmlpp::Element* p0Element, std::string& sName);

//	void parseBricks(GameCtx& oCtx, const xmlpp::Element* p0Element
//					, std::string& aBrickId, std::vector<Tile>& aBrick
//					, Tile& oDefaultBrick);
//	void parseBricksBrick(GameCtx& oCtx, const xmlpp::Element* p0Element
//						, std::string::value_type& uchId, Tile& oBrick);

	void parseShapes(GameCtx& oCtx, const xmlpp::Element* p0Element, std::vector<uint32_t>& aBricks
					, std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > >& aShapeBrickPos
					, bool& bGenerate, int32_t& nWH, int32_t& nTotShapes);
	void parseShape(GameCtx& oCtx, const xmlpp::Element* p0Element, uint32_t cEmpty
					, std::vector<int32_t>& aPosX, std::vector<int32_t>& aPosY
					, std::vector<uint32_t>& aBrickId, uint32_t& cHiddenTo);
	std::string parseLine(GameCtx& oCtx, const xmlpp::Element* p0Element);

private:
	XmlTraitsParser& m_oXmlTraitsParser;
	XmlConditionalParser& m_oXmlConditionalParser;
private:
	XmlBlockParser() = delete;
	XmlBlockParser(const XmlBlockParser& oSource) = delete;
	XmlBlockParser& operator=(const XmlBlockParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_BLOCK_PARSER_H */

