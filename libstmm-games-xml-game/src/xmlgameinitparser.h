/*
 * File:   xmlgameinitparser.h
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

#ifndef STMG_XML_GAME_INIT_PARSER_H
#define STMG_XML_GAME_INIT_PARSER_H

#include <stmm-games-xml-base/xmltraitsparser.h>
#include "xmllayoutparser.h"
#include "xmlutile/xmlidtilesparser.h"

#include <stmm-games-file/gameloader.h>

#include <memory>
#include <string>

namespace stmg { class AppPreferences; }
namespace stmg { class GameInitCtx; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlGameInfoParser; }
namespace xmlpp { class Element; }

namespace stmg
{

class XmlGameInitParser //: private XmlCommonParser
{
public:
	XmlGameInitParser(XmlGameInfoParser& oXmlGameInfoParser, XmlLayoutParser& oXmlLayoutParser)
	: m_oXmlGameInfoParser(oXmlGameInfoParser)
	, m_oXmlLayoutParser(oXmlLayoutParser)
	, m_oXmlTraitsParser(oXmlLayoutParser.getXmlTraitsParser())
	, m_oXmlConditionalParser(m_oXmlTraitsParser.getXmlConditionalParser())
	{
	}

	static const std::string::value_type s_cDefaultEmptyChar;

	/** Parse game data common to all levels.
	 * @param oCtx The context.
	 * @param p0Element The root element. Cannot be null.
	 * @param refAppPreferences The preferences. Cannot be null.
	 * @return The parsed game info for the game.
	 */
	GameLoader::GameInfo parseGameInit(GameInitCtx& oCtx, const xmlpp::Element* p0Element
										, const shared_ptr<AppPreferences>& refAppPreferences);

private:
	void parseLevelInit(GameInitCtx& oCtx, const xmlpp::Element* p0Element);

	void parseBoard(GameInitCtx& oCtx, const xmlpp::Element* p0Element);
	void parseBoardOverlays(GameInitCtx& oCtx, const xmlpp::Element* p0Element);
	void parseBoardOverlaysOverlay(GameInitCtx& oCtx, const xmlpp::Element* p0Element
									, const XmlIdTilesParser::IdTiles& oIdTiles);
	void parseBoardOverlaysOverlayLine(GameInitCtx& oCtx, const xmlpp::Element* p0Element, std::string& sLine);
	void parseBoardSingle(GameInitCtx& oCtx, const xmlpp::Element* p0Element);
	void parseBoardRect(GameInitCtx& oCtx, const xmlpp::Element* p0Element);
	void parseShow(GameInitCtx& oCtx, const xmlpp::Element* p0Element);
	void parseSubshow(GameInitCtx& oCtx, const xmlpp::Element* p0Element);

private:
	XmlGameInfoParser& m_oXmlGameInfoParser;
	XmlLayoutParser& m_oXmlLayoutParser;
	XmlTraitsParser& m_oXmlTraitsParser;
	XmlConditionalParser& m_oXmlConditionalParser;
private:
	XmlGameInitParser() = delete;
	XmlGameInitParser(const XmlGameInitParser& oSource) = delete;
	XmlGameInitParser& operator=(const XmlGameInitParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_GAME_INIT_PARSER_H */

