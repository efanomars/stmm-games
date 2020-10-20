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
 * File:   xmlgameparser.h
 */

#ifndef STMG_XML_GAME_PARSER_H
#define STMG_XML_GAME_PARSER_H

#include "xmlgameinitparser.h"
#include "xmlblockparser.h"
#include "xmlgameinfoparser.h"
#include "xmllayoutparser.h"

#include <stmm-games-xml-base/xmltraitsparser.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlimageparser.h>
#include <stmm-games-xml-base/xmlutil/xmlvariantsetparser.h>

#include <stmm-games-file/gameloader.h>

#include <stmm-games/event.h>
#include <stmm-games/level.h>
#include <stmm-games/game.h>

#include <vector>
#include <map>
#include <stdexcept>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

namespace stmg { class AppConfig; }
namespace stmg { class AppPreferences; }
namespace stmg { class Block; }
namespace stmg { class ConditionalCtx; }
namespace stmg { class File; }
namespace stmg { class GameCtx; }
namespace stmg { class GameOwner; }
namespace stmg { class Highscore; }
namespace stmg { class Named; }
namespace stmg { class ParserCtx; }
namespace stmg { class XmlEventParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlGameParser : public Game::CreateLevelCallback
{
public:
	/** Constructor adding default parsers.
	 */
	XmlGameParser();
	/** Constructor.
	 * @param bAddDefaultEventParsers Whether to add the standard event parsers.
	 * @param bAddDefaultWidgetParsers Whether to add the standard game widget parsers.
	 */
	XmlGameParser(bool bAddDefaultEventParsers, bool bAddDefaultWidgetParsers);
	virtual ~XmlGameParser();

	/** Add an event parser.
	 * @param refXmlEventParser The event element parser. Cannot be null.
	 */
	void addXmlEventParser(unique_ptr<XmlEventParser> refXmlEventParser);

	/** Parses game information.
	 * This function only parses the basic information about a game. The rest
	 * of the xml tree defining the game is not loaded or checked since it might
	 * depend on AppPreferences.
	 *
	 * The returned game name is used to identify the game, not the game file (name).
	 *
	 * The oGameFile is needed to build the error string if an exception occurs.
	 * @param refAppConfig The config. Cannot be null.
	 * @param oGameFile The file of the xml document. Cannot be undefined.
	 * @param p0RootElement The root element of the xml document. Cannot be null.
	 * @return The game information and the internal game name.
	 * @throws std::runtime_error.
	 */
	std::pair<GameLoader::GameInfo, std::string> parseGameInfo(const shared_ptr<AppConfig>& refAppConfig
																, const File& oGameFile, const xmlpp::Element* p0RootElement);

	/** Parses game.
	 * If refHighscore is non null and was generated from a highscores definition
	 * no longer consistent with the newly parsed one, null is passed to the
	 * Game instance.
	 * @param oFile The file of the xml document. Cannot be undefined.
	 * @param p0RootElement The root element of the xml document. Cannot be null.
	 * @param oGameOwner The game owner callback interface.
	 * @param refAppPreferences The preferences. Cannot be null.
	 * @param oNamed The names from the Theme.
	 * @param refHighscore The pre game highscores. Can be null.
	 * @param oGameInfo The already loaded game information (see parseGameInfo).
	 * @return The game, whether refHighscore was ignored and the new GameInfo.
	 * @throws std::runtime_error.
	 */
	std::tuple<shared_ptr<Game>, bool, GameLoader::GameInfo> parseGame(
												const File& oFile, const xmlpp::Element* p0RootElement, GameOwner& oGameOwner
												, const shared_ptr<AppPreferences>& refAppPreferences
												, const Named& oNamed, const shared_ptr<Highscore>& refHighscore
												, const GameLoader::GameInfo& oGameInfo);

	static const std::string s_sEventRepeatAttr; /**< The repeat attribute. Must be bigger than `0` or not set (repeat forever).*/
	static const std::string s_sEventStepAttr; /**< The step attribute. Must be bigger than `0`, if not set it's `1`. */
	static const std::string s_sEventRandomProbAttr; /**< The (cumulative) random attribute. Must be bigger than `0` and exceed `100000`. */

	XmlLayoutParser& getLayoutParser() { return m_oXmlLayoutParser; }
private:
	shared_ptr<Level> createLevel(Game* p0Game, int32_t nLevel
								, const shared_ptr<AppPreferences>& refPreferences
								, const Level::Init& oInit) noexcept override;
private:
	// attributes used by more than one event type
	int32_t parseEventAttrRepeat(GameCtx& oCtx, const xmlpp::Element* p0EventElement);
	int32_t parseEventAttrStep(GameCtx& oCtx, const xmlpp::Element* p0EventElement);
	int32_t parseEventAttrRandomProb(GameCtx& oCtx, const xmlpp::Element* p0EventElement);
	//
	bool getBlock(GameCtx& oCtx, const std::string& sName, Block& oBlock);

	void parseLevel(GameCtx& oCtx, const xmlpp::Element* p0Element);

	void parseEventsIds(ConditionalCtx& oCtx, const xmlpp::Element* p0Element);
	std::pair<std::string, int32_t> parseEvIdMessage(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory
													, const std::string& sEvIdAttrName, const std::string& sMsgNameAttrName, const std::string& sMsgAttrName);
	void parseEvents(GameCtx& oCtx, const xmlpp::Element* p0Element);
	Event* parseEvent(GameCtx& oCtx, const xmlpp::Element* p0EventElement);
	void parseEventBase(GameCtx& oCtx, const xmlpp::Element* p0Element, Event::Init& oInit);
	Event* integrateAndAdd(GameCtx& oCtx, unique_ptr<Event> refEvent, const xmlpp::Element* p0Element);
	int32_t parseEventBaseMsgName(ConditionalCtx& oCtx, const std::string& sEventName
								, const xmlpp::Element* p0Element, const std::string& sAttr
								, const std::string& sMsgName);
	int32_t parseEventBaseListenerGroupName(GameCtx& oCtx, const std::string& sEventName
											, const xmlpp::Element* p0Element, const std::string& sAttr
											, const std::string& sListenerGroupName);
	int32_t parseEventAttrStart(GameCtx& oCtx, const xmlpp::Element* p0EventElement);
	void parseFilter(GameCtx& oCtx, Event* p0Event, const xmlpp::Element* p0Element, const std::string& sEventName);
	void parseFilterInput(GameCtx& oCtx, Event* p0Event, const xmlpp::Element* p0Element, const std::string& sEventName);
	//
	int32_t parseEventMsgName(GameCtx& oCtx, const std::string& sEventName
							, const xmlpp::Element* p0Element, const std::string& sAttr
							, const std::string& sMsgName);
	int32_t parseEventListenerGroupName(GameCtx& oCtx, const std::string& sEventName
										, const xmlpp::Element* p0Element, const std::string& sAttr
										, const std::string& sListenerGroupName);
	//
	void parseListeners(GameCtx& oCtx, Event* p0Event, const xmlpp::Element* p0Element);
	void parseListener(GameCtx& oCtx, Event* p0Event, const std::string& sEventName, const xmlpp::Element* p0Element);
	bool isReservedChildElementOfEvent(const std::string& sElementName) const;
	//
	static std::runtime_error errorElementAlreadyUsedForOtherEvent(ParserCtx& oCtx, const xmlpp::Element* p0Element);
	static std::runtime_error errorEventWithIdAlreadyDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sId);
	static std::runtime_error errorEventWithIdNotFound(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sEvIdAttrName, const std::string& sId);

private:
	friend class XmlEventParser;
	friend class XmlLayoutParser;
	std::map<std::string, XmlEventParser*> m_oIdEventParser; // Key: event id, Value: the non owning parser to use
	std::vector< unique_ptr<XmlEventParser>> m_aXmlEventParsers;
	std::map<std::string, std::pair<Event*, const xmlpp::Element*>> m_oIdEvent; // Key: event id
	std::vector<std::pair<Event*, const xmlpp::Element*>> m_aAnonEvent;

	XmlVariantSetParser m_oXmlVariantSetParser;
	XmlConditionalParser m_oXmlConditionalParser;
	XmlImageParser m_oXmlImageParser;
	XmlGameInfoParser m_oXmlGameInfoParser;
	XmlTraitsParser m_oXmlTraitsParser;
	XmlLayoutParser m_oXmlLayoutParser;
	XmlGameInitParser m_oXmlGameInitParser;
	XmlBlockParser m_oXmlBlockParser;
private:
	XmlGameParser(const XmlGameParser& oSource) = delete;
	XmlGameParser& operator=(const XmlGameParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_GAME_PARSER_H */

