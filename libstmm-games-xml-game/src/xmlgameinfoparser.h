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
 * File:   xmlgameinfoparser.h
 */

#ifndef STMG_XML_GAME_INFO_PARSER_H
#define STMG_XML_GAME_INFO_PARSER_H

#include <stmm-games-file/gameloader.h>

#include <stmm-games/prefselector.h>
#include <stmm-games/ownertype.h>
#include <stmm-games/variable.h>

#include <memory>
#include <string>
#include <utility>

namespace stmg { class AppConfig; }
namespace stmg { class ConditionalCtx; }
namespace stmg { class File; }
namespace stmg { class GameConstraints; }
namespace stmg { class GameInfoCtx; }
namespace stmg { class HighscoresDefinition; }
namespace stmg { class ParserCtx; }
namespace stmg { class XmlVariantSetParser; }
namespace stmg { template <class T> class NamedObjIndex; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlGameInfoParser
{
public:
	explicit XmlGameInfoParser(XmlVariantSetParser& oXmlVariantSetParser)
	: m_oXmlVariantSetParser(oXmlVariantSetParser)
	{
	}

	GameLoader::GameInfo parseGameInfo(const shared_ptr<AppConfig>& oAppConfig, const File& oGameFile, const xmlpp::Element* p0Element, std::string& sName);

	const GameLoader::GameInfo& parseGameInfo(GameInfoCtx& oCtx, const xmlpp::Element* p0Element, std::string& sName);

	static OwnerType parseOwnerAttr(ParserCtx& oCtx, const xmlpp::Element* p0Element);

private:
	std::pair<unique_ptr<PrefSelector::Operand>, bool> parseConstraintsOption(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	std::pair<unique_ptr<PrefSelector::Operand>, bool> parseConstraintsTrue(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	std::pair<unique_ptr<PrefSelector::Operand>, bool> parseConstraintsFalse(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	std::pair<unique_ptr<PrefSelector::Operand>, bool> parseConstraintsTraverse(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	std::pair<unique_ptr<PrefSelector::Operand>, bool> parseConstraintsAnd(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	std::pair<unique_ptr<PrefSelector::Operand>, bool> parseConstraintsOr(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, bool bCheckAttrs) const;

	void parseGameConstraints(GameInfoCtx& oCtx, const xmlpp::Element* p0Element, GameConstraints& oGameConstraints);
	void parseVariables(GameInfoCtx& oCtx, const xmlpp::Element* p0Element);
	void parseVariablesVar(GameInfoCtx& oCtx, const xmlpp::Element* p0Element
							, NamedObjIndex<Variable::VariableType>& oVariableTypes);
	shared_ptr<HighscoresDefinition> parseHighscoresDef(GameInfoCtx& oCtx, const xmlpp::Element* p0Element);
	void checkNonSystemVariableDefined(GameInfoCtx& oCtx, const xmlpp::Element* p0Element
										, const std::string& sAttr, const std::string& sVarName);
private:
	XmlVariantSetParser& m_oXmlVariantSetParser;
private:
	XmlGameInfoParser(const XmlGameInfoParser& oSource) = delete;
	XmlGameInfoParser& operator=(const XmlGameInfoParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_GAME_INFO_PARSER_H */

