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
 * File:   xmlgameparser.cc
 */

#include "xmlgameparser.h"

#include "gamectx.h"
#include "gameinitctx.h"
#include "xmleventparser.h"

#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/conditionalctx.h>
#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlcommonparser.h>

#include <stmm-games-file/gameloader.h>

#include <stmm-games/event.h>
#include <stmm-games/util/util.h>
#include <stmm-games/block.h>
#include <stmm-games/game.h>
#include <stmm-games/highscoresdefinition.h>
#include <stmm-games/named.h>
#include <stmm-games/tile.h>
#include <stmm-games/util/namedobjindex.h>
#include <stmm-games/variable.h>
#include <stmm-games/level.h>

#include <glibmm/ustring.h>

#include <libxml++/libxml++.h>

#include <vector>
#include <cassert>
#include <algorithm>
//#include <iostream>
#include <map>
#include <array>
#include <cstdint>
#include <limits>
#include <type_traits>

#include <stdint.h>

namespace stmg { class AppConfig; }
namespace stmg { class AppPreferences; }
namespace stmg { class File; }
namespace stmg { class GameOwner; }
namespace stmg { class Highscore; }
namespace stmg { class ParserCtx; }

namespace stmg
{

const std::string XmlGameParser::s_sEventRepeatAttr = "repeat";
	static const int32_t s_nEventRepeatAttrDefault = -1; //forever
const std::string XmlGameParser::s_sEventStepAttr = "step";
	static const int32_t s_nEventStepAttrDefault = 1;
const std::string XmlGameParser::s_sEventRandomProbAttr = "prob";
	static const int32_t s_nEventRandomProbDefault = 1;
	static const int32_t s_nEventRandomProbMax = 100000;

static const std::string s_sEventsNodeName = "Events";
static const std::string s_sGameBlocksNodeName = "Blocks";

static const std::string s_sEventIdAttr = "evId";
static const std::string s_sEventStartAttr = "start";
static const std::string s_sEventPriorityAttr = "priority";
static const std::string s_sEventIdIsPublicAttr = "publish";

static const std::string s_sEventFilterNodeName = "Filter";
static const std::string s_sEventFilterInputNodeName = "Input";
static const std::string s_sEventFilterInputMsgAttr = "msg";
static const std::string s_sEventFilterInputMsgFromAttr = "fromMsg";
static const std::string s_sEventFilterInputMsgToAttr = "toMsg";
static const std::string s_sEventFilterInputMsgNameAttr = "msgName";
static const std::string s_sEventFilterInputValueAttr = "value";
static const std::string s_sEventFilterInputValueFromAttr = "fromValue";
static const std::string s_sEventFilterInputValueToAttr = "toValue";
static const std::string s_sEventFilterInputNewMsgAttr = "newMsg";
static const std::string s_sEventFilterInputNewMsgNameAttr = "newMsgName";
static const std::string s_sEventFilterInputNewValueAttr = "newValue";
static const std::string s_sEventFilterInputNewValueAddAttr = "valueAdd";
static const std::string s_sEventFilterInputNewValueMultAttr = "valueMult";
static const std::string s_sEventFilterInputNewValueDivAttr = "valueDiv";
static const std::string s_sEventFilterInputNewValueModAttr = "valueMod";
	//static const std::string s_sEventFilterInputNewValueFloorModAttr = "valueFloorMod";
static const std::string s_sEventFilterInputNewValueMinAttr = "valueMin";
static const std::string s_sEventFilterInputNewValueMaxAttr = "valueMax";
static const std::string s_sEventFilterInputNewValuePercAttr = "valuePercent";
static const std::string s_sEventFilterInputNewValuePermillAttr = "valuePermill";
static const std::string s_sEventListenerNodeName = "Listener";
static const std::string s_sEventListenerEventAttr = "event";
static const std::string s_sEventListenerGroupAttr = "group";
static const std::string s_sEventListenerGroupNameAttr = "groupName";
static const std::string s_sEventListenerMsgAttr = "msg";
static const std::string s_sEventListenerMsgNameAttr = "msgName";
	static const std::string s_sEventListenerGroupAttrValueFinished = "FINISHED";
static constexpr std::array<std::string const*, 2> s_aEventReservedChildElements{{&s_sEventFilterNodeName, &s_sEventListenerNodeName}};
//

XmlGameParser::XmlGameParser()
: m_oXmlVariantSetParser()
, m_oXmlConditionalParser(m_oXmlVariantSetParser)
, m_oXmlImageParser(m_oXmlConditionalParser)
, m_oXmlGameInfoParser(m_oXmlVariantSetParser)
, m_oXmlTraitsParser(m_oXmlConditionalParser)
, m_oXmlLayoutParser(m_oXmlTraitsParser, m_oXmlImageParser)
, m_oXmlGameInitParser(m_oXmlGameInfoParser, m_oXmlLayoutParser)
, m_oXmlBlockParser(m_oXmlTraitsParser)
{
	m_oXmlLayoutParser.m_p0XmlGameParser = this;
}
XmlGameParser::~XmlGameParser()
{
}
std::pair<GameLoader::GameInfo, std::string> XmlGameParser::parseGameInfo(const shared_ptr<AppConfig>& refAppConfig
																		, const File& oGameFile, const xmlpp::Element* p0RootElement)
{
	std::string sName;
	GameLoader::GameInfo oGameInfo;
	oGameInfo = m_oXmlGameInfoParser.parseGameInfo(refAppConfig, oGameFile, p0RootElement, sName);
	return std::make_pair(std::move(oGameInfo), std::move(sName));
}
void XmlGameParser::addXmlEventParser(unique_ptr<XmlEventParser> refXmlEventParser)
{
//std::cout << "XmlGameParser::addEventsParser" << '\n';
	assert(refXmlEventParser);
	assert(refXmlEventParser->m_p1Owner == nullptr);
	refXmlEventParser->m_p1Owner = this;
	refXmlEventParser->m_p0XmlConditionalParser = &m_oXmlConditionalParser;
	refXmlEventParser->m_p0XmlTraitsParser = &m_oXmlTraitsParser;
	m_aXmlEventParsers.push_back(std::move(refXmlEventParser));
}

std::tuple<shared_ptr<Game>, bool, GameLoader::GameInfo> XmlGameParser::parseGame(
															const File& oFile, const xmlpp::Element* p0RootElement, GameOwner& oGameOwner
															, const shared_ptr<AppPreferences>& refAppPreferences
															, const Named& oNamed, const shared_ptr<Highscore>& refHighscore
															, const GameLoader::GameInfo& oGameInfo)
{
	assert(refAppPreferences);
	AssignableNamed oMyNamed;
	oMyNamed = oNamed;
	GameInitCtx oCtx(refAppPreferences, oMyNamed, oFile);

	{
		// Pre parse the  event ids in case they might be referenced within widgets
		const xmlpp::Element* p0Events = XmlCommonParser::parseUniqueElement(oCtx, p0RootElement, s_sEventsNodeName, true);
		parseEventsIds(oCtx, p0Events);
	}
	oCtx.addChecker(p0RootElement);
	GameLoader::GameInfo oNewGameInfo = m_oXmlGameInitParser.parseGameInit(oCtx, p0RootElement, refAppPreferences);
	bool bHighscoresIgnored = false;
	const bool bNewHSIsNull = (!oNewGameInfo.m_refHighscoresDefinition);
	const bool bPreHSIsNull = (!oGameInfo.m_refHighscoresDefinition);
	if (! (bNewHSIsNull || bPreHSIsNull)) {
		// both non null
		if (! (oNewGameInfo.m_refHighscoresDefinition == oGameInfo.m_refHighscoresDefinition)) {
			// not same instance
			if ((*oNewGameInfo.m_refHighscoresDefinition) == (*oGameInfo.m_refHighscoresDefinition)) {
				// equivalent
			} else {
				// highscores definition has changed, pre game highscores nullified
				bHighscoresIgnored = true;
			}
		}
	} else if (bNewHSIsNull != bPreHSIsNull) {
		// one is null
		if (bNewHSIsNull) {
			// highscores definition was removed
			bHighscoresIgnored = true;
		} else {
			// there can't be a highscores without highscores definition
			assert(false);
		}
	}
	Level::Init oLevelInit;
	oLevelInit.m_nBoardW = oCtx.boardGetWidth();
	oLevelInit.m_nBoardH = oCtx.boardGetHeight();
	oLevelInit.m_aBoard = oCtx.boardGet();
	oLevelInit.m_nShowW = oCtx.showGetWidth();
	oLevelInit.m_nShowH = oCtx.showGetHeight();
	oLevelInit.m_nShowInitX = oCtx.showGetInitialX();
	oLevelInit.m_nShowInitY = oCtx.showGetInitialY();
	oLevelInit.m_nSubshowW = (oCtx.subshowIsSet() ? oCtx.subshowGetWidth() : -1);
	oLevelInit.m_nSubshowH = (oCtx.subshowIsSet() ? oCtx.subshowGetHeight() : -1);
	oLevelInit.m_nInitialFallEachTicks = oCtx.getInitialFallEachTicks();

	oCtx.addValidChildElementNames(p0RootElement, s_sGameBlocksNodeName, s_sEventsNodeName);
	oCtx.removeChecker(p0RootElement, true);

	NamedObjIndex<Variable::VariableType> oGVT{oCtx.getGameVariableTypes()};
	NamedObjIndex<Variable::VariableType> oTVT{oCtx.getTeamVariableTypes()};
	NamedObjIndex<Variable::VariableType> oMVT{oCtx.getMateVariableTypes()};
	Game::Init oGameInit;
	oGameInit.m_sName = oCtx.getName();
	oGameInit.m_p0GameOwner = &oGameOwner;
	oGameInit.m_oNamed = std::move(oMyNamed);
	oGameInit.m_oGameVariableTypes = std::move(oGVT);
	oGameInit.m_oTeamVariableTypes = std::move(oTVT);
	oGameInit.m_oPlayerVariableTypes = std::move(oMVT);
	oGameInit.m_refLayout = oCtx.getLayout();
	const double fMinGameInterval = oCtx.getMinGameInterval();
	if (fMinGameInterval > 0.0) {
		oGameInit.m_fMinGameInterval = fMinGameInterval;
	}
	const double fInitialGameInterval = oCtx.getInitialGameInterval();
	if (fInitialGameInterval > 0.0) {
		oGameInit.m_fInitialGameInterval = fInitialGameInterval;
	}
	const int32_t nMaxViewTicks = oCtx.getMaxViewTicks();
	if (nMaxViewTicks > 0) {
		oGameInit.m_nMaxViewTicks = nMaxViewTicks;
	}
	const int32_t nAdditionalHighscoresWait = oCtx.getAdditionalHighscoresWait();
	if (nAdditionalHighscoresWait >= 0) {
		oGameInit.m_nAdditionalHighscoresWait = nAdditionalHighscoresWait;
	}
	const auto oTuple = oCtx.getSoundScales();
	if (std::get<0>(oTuple) > 0) {
		oGameInit.m_fSoundScaleX = std::get<0>(oTuple);
	}
	if (std::get<1>(oTuple) > 0) {
		oGameInit.m_fSoundScaleY = std::get<1>(oTuple);
	}
	if (std::get<2>(oTuple) > 0) {
		oGameInit.m_fSoundScaleZ = std::get<2>(oTuple);
	}
	const int32_t nBoardPainterIdx = oCtx.getBoardPainterIdx();
	if (nBoardPainterIdx >= 0) {
		oGameInit.m_nBoardPainterIdx = nBoardPainterIdx;
	}
	const int32_t nBlockPainterIdx = oCtx.getBlockPainterIdx();
	if (nBlockPainterIdx >= 0) {
		oGameInit.m_nBlockPainterIdx = nBlockPainterIdx;
	}
	if (!bHighscoresIgnored) {
		oGameInit.m_refHighscore = refHighscore;
	}
	oGameInit.m_refHighscoresDefinition = oNewGameInfo.m_refHighscoresDefinition;
//std::cout << "XmlGameParser::parseGame --->Highscore adr: " << reinterpret_cast<int64_t>(refHighscore.get()) << '\n';
//std::cout << "XmlGameParser::parseGame --->HighscoreDefinition adr: " << reinterpret_cast<int64_t>(oGameInit.m_refHighscoresDefinition.get()) << '\n';
	auto refGame = std::make_shared<Game>(std::move(oGameInit), *this, oLevelInit);

	GameCtx oGameCtx(oCtx.appPreferences(), oFile, *refGame, oCtx.getGameConstraints());
	oGameCtx.addChecker(p0RootElement);
	//
	const xmlpp::Element* p0Blocks = XmlCommonParser::parseUniqueElement(oGameCtx, p0RootElement, s_sGameBlocksNodeName, false);
	if (p0Blocks != nullptr) {
		m_oXmlBlockParser.parseBlocks(oGameCtx, p0Blocks);
	}
	//
	const int32_t nTotLevels = refGame->getTotLevels();
	for (int32_t nLevel = 0; nLevel < nTotLevels; ++nLevel) {
		oGameCtx.m_nLevel = nLevel;
		const xmlpp::Element* p0Events = XmlCommonParser::parseUniqueElement(oGameCtx, p0RootElement, s_sEventsNodeName, true);
		parseEvents(oGameCtx, p0Events);
	}
	oGameCtx.removeChecker(p0RootElement, false);
	return std::make_tuple(refGame, bHighscoresIgnored, std::move(oNewGameInfo));
}
shared_ptr<Level> XmlGameParser::createLevel(Game* p0Game, int32_t nLevel
											, const shared_ptr<AppPreferences>& refPreferences
											, const Level::Init& oInit) noexcept
{
	//TODO recycling
	return std::make_shared<Level>(p0Game, nLevel, refPreferences, oInit);
}
bool XmlGameParser::getBlock(GameCtx& oCtx, const std::string& sName, Block& oBlock)
{
//std::cout << "XmlGameParser::getBlock  sName=" << sName << '\n';
	const int32_t nIdx = oCtx.getNamedBlocks().getIndex(sName);
	if (nIdx < 0) {
		return false; //--------------------------------------------------------
	}
	oBlock = oCtx.getNamedBlocks().getObj(nIdx);
//#ifndef NDEBUG
//oBlock.dump();
//#endif //NDEBUG
	return true;
}

void XmlGameParser::parseEventsIds(ConditionalCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	m_oXmlConditionalParser.visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0EventElement)
	{
		const auto oPairEventId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0EventElement, s_sEventIdAttr);
		const std::string& sId = oPairEventId.second;
		if (! sId.empty()) {
			std::string sEventName = p0EventElement->get_name();
			for (auto& refXmlEventParser : m_aXmlEventParsers) {
				if (refXmlEventParser->getEventName() == sEventName) {
					m_oIdEventParser[sId] = refXmlEventParser.get();
					break;
				}
			}
		}
	});
	oCtx.removeChecker(p0Element, false);
}
std::runtime_error XmlGameParser::errorEventWithIdNotFound(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sEvIdAttrName, const std::string& sId)
{
	return XmlCommonErrors::error(oCtx, p0Element, s_sEventIdAttr, Util::stringCompose(
										"Event id '%1' with value '%2' not found", sEvIdAttrName, sId));
}
std::pair<std::string, int32_t> XmlGameParser::parseEvIdMessage(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory
																, const std::string& sEvIdAttrName, const std::string& sMsgNameAttrName, const std::string& sMsgAttrName)
{
	assert(p0Element != nullptr);
	assert(! sEvIdAttrName.empty());
	assert(! sMsgNameAttrName.empty());
	assert(! sMsgAttrName.empty());

	const auto oPairEventId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sEvIdAttrName);
	const std::string& sId = oPairEventId.second;
	const bool bIdDefined = oPairEventId.first;

	const auto oPairMsgName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sMsgNameAttrName);
	const std::string& sMsgName = oPairMsgName.second;
	const bool bMsgNameDefined = oPairMsgName.first;

	int32_t nMsg = -1;
	const auto oPairMsg = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sMsgAttrName);
	const bool bMsgDefined = oPairMsg.first;
	if (bMsgDefined) {
		if (bMsgNameDefined) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sMsgAttrName
																	, sMsgNameAttrName);
		}
		const std::string& sMsg = oPairMsg.second;
		nMsg = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, sMsgAttrName, sMsg, false
														, true, -1, false, -1);
	}

	if (bIdDefined != (bMsgNameDefined || bMsgDefined)) {
		if (! bIdDefined) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, sEvIdAttrName);
		} else {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, sMsgNameAttrName);
		}
	}
	if (! bIdDefined) {
		if (bMandatory) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, sEvIdAttrName);
		}
		return std::make_pair("", -1); //---------------------------------------
	}
	const auto itFound = m_oIdEventParser.find(sId);
	if (itFound == m_oIdEventParser.end()) {
		throw errorEventWithIdNotFound(oCtx, p0Element, sEvIdAttrName, sId);
	}
	if (bMsgNameDefined) {
		auto& oXmlEventParser = *(itFound->second);
		nMsg = oXmlEventParser.parseEventMsgName(oCtx, p0Element, sMsgNameAttrName, sMsgName);
	}
	return std::make_pair(sId, nMsg);
}

void XmlGameParser::parseEvents(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlGameParser::parseEvents" << '\n';
	oCtx.addChecker(p0Element);
	m_oIdEvent.clear();
	m_aAnonEvent.clear();
	// first create all event instances
	m_oXmlConditionalParser.visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0EventElement)
	{
		parseEvent(oCtx, p0EventElement);
	});
	oCtx.removeChecker(p0Element, false, true);
	// now add listeners
//std::cout << "XmlGameParser::parseEvents ADDING LISTENERS" << '\n';
//#ifndef NDEBUG
//oCtx.dump();
//#endif //NDEBUG
	for (const auto& oPairId : m_oIdEvent) {
		const auto& oPairEventElement = oPairId.second;
		Event* p0NewEvent = oPairEventElement.first;
		const xmlpp::Element* p0EventElement = oPairEventElement.second;
		parseListeners(oCtx, p0NewEvent, p0EventElement);
	}
	for (const auto& oPairEventElement : m_aAnonEvent) {
		Event* p0NewEvent = oPairEventElement.first;
		const xmlpp::Element* p0EventElement = oPairEventElement.second;
		parseListeners(oCtx, p0NewEvent, p0EventElement);
	}
}
Event* XmlGameParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0EventElement)
{
	oCtx.addChecker(p0EventElement);
	for (auto& p0ChildName : s_aEventReservedChildElements) {
		oCtx.addValidChildElementName(p0EventElement, *p0ChildName);
	}
	const auto& sEventName = p0EventElement->get_name();
//std::cout << "parseEvent event name: " << sEventName << '\n';
	//TODO: simplify following with lambda?
	Event* p0Event = nullptr;
	for (auto& refXmlEventParser : m_aXmlEventParsers) {
		if (refXmlEventParser->getEventName() == sEventName) {
			p0Event = refXmlEventParser->parseEvent(oCtx, p0EventElement);
//std::cout << "XmlGameParser::parseEvent plugin found" << '\n';
			break; // for(refXmlEventParser ------
		}
	}
	if (p0Event == nullptr) {
		throw XmlCommonErrors::errorElementInvalid(oCtx, p0EventElement, sEventName);
	}
	oCtx.removeChecker(p0EventElement, false);
	return p0Event;
}
std::runtime_error XmlGameParser::errorElementAlreadyUsedForOtherEvent(ParserCtx& oCtx, const xmlpp::Element* p0Element)
{
	return XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, "Element already used to parse an already added event");
}
std::runtime_error XmlGameParser::errorEventWithIdAlreadyDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sId)
{
	return XmlCommonErrors::error(oCtx, p0Element, s_sEventIdAttr, Util::stringCompose(
										"Event id '%1' with value '%2' already defined", s_sEventIdAttr, sId));
}
void XmlGameParser::parseEventBase(GameCtx& oCtx, const xmlpp::Element* p0Element, Event::Init& oInit)
{
	oCtx.addChecker(p0Element);
	oInit.m_p0Level = &oCtx.level();
	const auto oPairPriority = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventPriorityAttr);
	if (oPairPriority.first) {
		const std::string& sPriority = oPairPriority.second;
		oInit.m_nPriority = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventPriorityAttr, sPriority, false
														, false, -1, false, -1);
	}
	oCtx.removeChecker(p0Element, false, true);
}
Event* XmlGameParser::integrateAndAdd(GameCtx& oCtx, unique_ptr<Event> refEvent, const xmlpp::Element* p0Element)
{
//std::cout << "XmlGameParser::integrateAndAdd  &refEvent=" << reinterpret_cast<int64_t>(refEvent.get()) << "   p0Element=" << reinterpret_cast<int64_t>(p0Element) << '\n';
	auto p0Event = refEvent.get();
	assert(p0Event != nullptr);
	assert(p0Element != nullptr);

	oCtx.addChecker(p0Element);

	Level& oLevel = oCtx.level();

	const auto oPairEventId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventIdAttr);
	const std::string& sId = oPairEventId.second;
	if (sId.empty()) {
		auto itFoundElement = std::find_if(m_aAnonEvent.begin(), m_aAnonEvent.end(), [p0Element](const std::pair<Event*, const xmlpp::Element*>& oEvPair)
		{
			const xmlpp::Element* p0AddedElement = oEvPair.second;
			return (p0AddedElement == p0Element); // find_if
		});
		if (itFoundElement != m_aAnonEvent.end()) {
			throw errorElementAlreadyUsedForOtherEvent(oCtx, p0Element);
		}
		//
		m_aAnonEvent.push_back(std::make_pair(p0Event, p0Element));
	} else {
		if (m_oIdEvent.find(sId) != m_oIdEvent.end()) {
			throw errorEventWithIdAlreadyDefined(oCtx, p0Element, sId);
		}
		//
		auto itFoundElement = std::find_if(m_oIdEvent.begin(), m_oIdEvent.end(), [p0Element](const std::pair<std::string, std::pair<Event*, const xmlpp::Element*>>& oIdEvent)
		{
			const auto& oEvPair = oIdEvent.second;
			const xmlpp::Element* p0AddedElement = oEvPair.second;
			return (p0AddedElement == p0Element); // find_if
		});
		if (itFoundElement != m_oIdEvent.end()) {
			throw errorElementAlreadyUsedForOtherEvent(oCtx, p0Element);
		}
		//
		m_oIdEvent[sId] = std::make_pair(p0Event, p0Element);
	}
	const int32_t nStart = parseEventAttrStart(oCtx, p0Element);
	// if the event was already added to the level assume also activation was done
	if (!oLevel.hasEvent(p0Event)) {
		bool bIdIsPublic = false;
		if (! sId.empty()) {
			const auto oPairEventIdIsPublic = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventIdIsPublicAttr);
			if (oPairEventIdIsPublic.first) {
				const std::string& sIdIsPublic = oPairEventIdIsPublic.second;
				bIdIsPublic = XmlUtil::strToBool(oCtx, p0Element, s_sEventIdIsPublicAttr, sIdIsPublic);
			}
		}
		oLevel.addEvent((bIdIsPublic ? sId : Util::s_sEmptyString), std::move(refEvent));
		if (nStart >= 0) {
			oLevel.activateEvent(p0Event, nStart);
		} else {
			oLevel.deactivateEvent(p0Event);
		}
	}
	//
	const xmlpp::Element* p0FilterElement = m_oXmlConditionalParser.parseUniqueElement(oCtx, p0Element, s_sEventFilterNodeName, false);
	if (p0FilterElement != nullptr) {
		parseFilter(oCtx, p0Event, p0FilterElement, p0Element->get_name());
	}
	// Take for granted that the Event subclass will check
	oCtx.removeChecker(p0Element, false);
	return p0Event;
}
int32_t XmlGameParser::parseEventBaseMsgName(ConditionalCtx& oCtx, const std::string& /*sEventName*/
											, const xmlpp::Element* p0Element, const std::string& sAttr
											, const std::string& /*sMsgName*/)
{
	throw XmlCommonErrors::errorAttrInvalidValue(oCtx, p0Element, sAttr);
}
int32_t XmlGameParser::parseEventBaseListenerGroupName(GameCtx& oCtx, const std::string& /*sEventName*/
														, const xmlpp::Element* p0Element, const std::string& sAttr
														, const std::string& sListenerGroupName)
{
	if (sListenerGroupName == s_sEventListenerGroupAttrValueFinished) {
		return Event::LISTENER_GROUP_FINISHED;
	}
	throw XmlCommonErrors::errorAttrInvalidValue(oCtx, p0Element, sAttr);
}
int32_t XmlGameParser::parseEventMsgName(GameCtx& oCtx, const std::string& sEventName
										, const xmlpp::Element* p0Element, const std::string& sAttr
										, const std::string& sMsgName)
{
	bool bFound = false;
	int32_t nMsg = -1;
	for (auto& refXmlEventParser : m_aXmlEventParsers) {
		if (refXmlEventParser->getEventName() == sEventName) {
			nMsg = refXmlEventParser->parseEventMsgName(oCtx, p0Element, sAttr, sMsgName);
			bFound = true;
			break; // for(refXmlEventParser ------
		}
	}
	#ifndef NDEBUG
	assert(bFound);
	#else
	static_cast<void>(bFound);
	#endif //NDEBUG
	return nMsg;
}
int32_t XmlGameParser::parseEventListenerGroupName(GameCtx& oCtx, const std::string& sEventName
													, const xmlpp::Element* p0Element, const std::string& sAttr
													, const std::string& sListenerGroupName)
{
	bool bFound = false;
	int32_t nListenerGroup = -2;
	for (auto& refXmlEventParser : m_aXmlEventParsers) {
		if (refXmlEventParser->getEventName() == sEventName) {
			nListenerGroup = refXmlEventParser->parseEventListenerGroupName(oCtx, p0Element, sAttr, sListenerGroupName);
			bFound = true;
			break; // for(refXmlEventParser ------
		}
	}
	#ifndef NDEBUG
	assert(bFound);
	#else
	static_cast<void>(bFound);
	#endif //NDEBUG
	return nListenerGroup;
}
void XmlGameParser::parseFilter(GameCtx& oCtx, Event* p0Event, const xmlpp::Element* p0Element, const std::string& sEventName)
{
	assert(p0Event != nullptr);
	assert(p0Element != nullptr);
	oCtx.addChecker(p0Element);
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sEventFilterInputNodeName, [&](const xmlpp::Element* p0InputElement)
	{
		parseFilterInput(oCtx, p0Event, p0InputElement, sEventName);
	});
	oCtx.removeChecker(p0Element, true);
}
void XmlGameParser::parseFilterInput(GameCtx& oCtx, Event* p0Event, const xmlpp::Element* p0Element, const std::string& sEventName)
{
	assert(p0Event != nullptr);
	assert(p0Element != nullptr);
	oCtx.addChecker(p0Element);
	int32_t nInFromMsg = -1;
	int32_t nInToMsg = std::numeric_limits<int32_t>::max();
	const auto oPairMsgName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventFilterInputMsgNameAttr);
	const bool bMsgNameDefined = oPairMsgName.first;
	if (bMsgNameDefined) {
		const std::string& sMsgName = oPairMsgName.second;
		nInFromMsg = parseEventMsgName(oCtx, sEventName, p0Element, s_sEventFilterInputMsgNameAttr, sMsgName);
		nInToMsg = nInFromMsg;
	}
	const bool bMsgDefined = m_oXmlConditionalParser.parseAttributeFromTo<int32_t>(
														oCtx, p0Element
														, s_sEventFilterInputMsgAttr, s_sEventFilterInputMsgFromAttr, s_sEventFilterInputMsgToAttr
														, false, true, -1, false, -1, nInFromMsg, nInToMsg);
	if (bMsgDefined) {
		if (bMsgNameDefined) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByOthers(oCtx, p0Element, s_sEventFilterInputMsgNameAttr
																	, s_sEventFilterInputMsgFromAttr, s_sEventFilterInputMsgToAttr);
		}
	}
	int32_t nInFromValue = std::numeric_limits<int32_t>::lowest();
	int32_t nInToValue = std::numeric_limits<int32_t>::max();
	m_oXmlConditionalParser.parseAttributeFromTo<int32_t>(oCtx, p0Element
														, s_sEventFilterInputValueAttr, s_sEventFilterInputValueFromAttr, s_sEventFilterInputValueToAttr
														, false, false, -1, false, -1, nInFromValue, nInToValue);
	//
	int32_t nNewMsg = -1;
	const auto oPairNewMsgName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventFilterInputNewMsgNameAttr);
	const bool bNewMsgNameDefined = oPairNewMsgName.first;
	if (bNewMsgNameDefined) {
		const std::string& sNewMsgName = oPairNewMsgName.second;
		nNewMsg = parseEventMsgName(oCtx, sEventName, p0Element, s_sEventFilterInputNewMsgNameAttr, sNewMsgName);
	}
	const auto oPairNewMsg = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventFilterInputNewMsgAttr);
	const bool bNewMsgDefined = oPairNewMsg.first;
	if (bNewMsgDefined) {
		if (bNewMsgNameDefined) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sEventFilterInputNewMsgNameAttr
																	, s_sEventFilterInputNewMsgAttr);
		}
		const std::string& sNewMsg = oPairNewMsg.second;
		nNewMsg = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventFilterInputNewMsgAttr, sNewMsg, false
														, true, -1, false, -1);
	}

	Event::MSG_FILTER_VALUE_OP eOp = Event::MSG_FILTER_VALUE_OP_UNCHANGED;
	std::string sOpAttr;
	int32_t nOperand = 0;
	const auto oPairNewValue = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventFilterInputNewValueAttr);
	if (oPairNewValue.first) {
		const std::string& sNewValue = oPairNewValue.second;
		nOperand = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventFilterInputNewValueAttr, sNewValue, false
														, false, -1, false, -1);
		eOp = Event::MSG_FILTER_VALUE_OP_SET;
		sOpAttr = s_sEventFilterInputNewValueAttr;
	}
	int32_t nAddValue = 0;
	const auto oPairAddValue = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventFilterInputNewValueAddAttr);
	const bool bAddSet = oPairAddValue.first;
	if (bAddSet) {
		if (eOp == Event::MSG_FILTER_VALUE_OP_SET) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sEventFilterInputNewValueAddAttr, s_sEventFilterInputNewValueAttr);
		}
		const std::string& sAddValue = oPairAddValue.second;
		nAddValue = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventFilterInputNewValueAddAttr, sAddValue, false
														, false, -1, false, -1);
		//eOp = Event::MSG_FILTER_VALUE_OP_SET;
		//sOpAttr = s_sEventFilterInputNewValueAddAttr;
	}
	const auto oPairMultValue = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventFilterInputNewValueMultAttr);
	if (oPairMultValue.first) {
		if (eOp != Event::MSG_FILTER_VALUE_OP_UNCHANGED) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sEventFilterInputNewValueMultAttr, sOpAttr);
		}
		const std::string& sMultValue = oPairMultValue.second;
		nOperand = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventFilterInputNewValueMultAttr, sMultValue, false
														, false, -1, false, -1);
		eOp = Event::MSG_FILTER_VALUE_OP_MULT_ADD;
		sOpAttr = s_sEventFilterInputNewValueMultAttr;
	}
	const auto oPairDivValue = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventFilterInputNewValueDivAttr);
	if (oPairDivValue.first) {
		if (eOp != Event::MSG_FILTER_VALUE_OP_UNCHANGED) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sEventFilterInputNewValueDivAttr, sOpAttr);
		}
		const std::string& sDivValue = oPairDivValue.second;
		nOperand = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventFilterInputNewValueDivAttr, sDivValue, false
														, false, -1, false, -1);
		eOp = Event::MSG_FILTER_VALUE_OP_DIV_ADD;
		sOpAttr = s_sEventFilterInputNewValueDivAttr;
	}
	const auto oPairModValue = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventFilterInputNewValueModAttr);
	if (oPairModValue.first) {
		if (eOp != Event::MSG_FILTER_VALUE_OP_UNCHANGED) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sEventFilterInputNewValueModAttr, sOpAttr);
		}
		const std::string& sModValue = oPairModValue.second;
		nOperand = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventFilterInputNewValueModAttr, sModValue, false
														, false, -1, false, -1);
		eOp = Event::MSG_FILTER_VALUE_OP_MOD_ADD;
		sOpAttr = s_sEventFilterInputNewValueModAttr;
	}
	//const auto oPairFloorModValue = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventFilterInputNewValueFloorModAttr);
	//if (oPairFloorModValue.first) {
	//	if (eOp != Event::MSG_FILTER_VALUE_OP_UNCHANGED) {
	//		throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sEventFilterInputNewValueFloorModAttr, sOpAttr);
	//	}
	//	const std::string& sFloorModValue = oPairFloorModValue.second;
	//	nOperand = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventFilterInputNewValueFloorModAttr, sFloorModValue, false
	//													, false, -1, false, -1);
	//	eOp = Event::MSG_FILTER_VALUE_OP_FLOORMOD_ADD;
	//	sOpAttr = s_sEventFilterInputNewValueFloorModAttr;
	//}
	const auto oPairPercValue = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventFilterInputNewValuePercAttr);
	if (oPairPercValue.first) {
		if (eOp != Event::MSG_FILTER_VALUE_OP_UNCHANGED) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sEventFilterInputNewValuePercAttr, sOpAttr);
		}
		const std::string& sPercValue = oPairPercValue.second;
		nOperand = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventFilterInputNewValuePercAttr, sPercValue, false
														, false, -1, false, -1);
		eOp = Event::MSG_FILTER_VALUE_OP_PERCENT_ADD;
		sOpAttr = s_sEventFilterInputNewValuePercAttr;
	}
	const auto oPairPermillValue = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventFilterInputNewValuePermillAttr);
	if (oPairPermillValue.first) {
		if (eOp != Event::MSG_FILTER_VALUE_OP_UNCHANGED) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sEventFilterInputNewValuePermillAttr, sOpAttr);
		}
		const std::string& sPermillValue = oPairPermillValue.second;
		nOperand = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventFilterInputNewValuePermillAttr, sPermillValue, false
														, false, -1, false, -1);
		eOp = Event::MSG_FILTER_VALUE_OP_PERMILL_ADD;
		sOpAttr = s_sEventFilterInputNewValuePermillAttr;
	}
	const auto oPairMinValue = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventFilterInputNewValueMinAttr);
	if (oPairMinValue.first) {
		if (eOp != Event::MSG_FILTER_VALUE_OP_UNCHANGED) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sEventFilterInputNewValueMinAttr, sOpAttr);
		}
		const std::string& sMinValue = oPairMinValue.second;
		nOperand = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventFilterInputNewValueMinAttr, sMinValue, false
														, false, -1, false, -1);
		eOp = Event::MSG_FILTER_VALUE_OP_MIN_ADD;
		sOpAttr = s_sEventFilterInputNewValueMinAttr;
	}
	const auto oPairMaxValue = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventFilterInputNewValueMaxAttr);
	if (oPairMaxValue.first) {
		if (eOp != Event::MSG_FILTER_VALUE_OP_UNCHANGED) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sEventFilterInputNewValueMaxAttr, sOpAttr);
		}
		const std::string& sMaxValue = oPairMaxValue.second;
		nOperand = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventFilterInputNewValueMaxAttr, sMaxValue, false
														, false, -1, false, -1);
		eOp = Event::MSG_FILTER_VALUE_OP_MAX_ADD;
		//sOpAttr = s_sEventFilterInputNewValueMaxAttr;
	}
	if ((eOp == Event::MSG_FILTER_VALUE_OP_UNCHANGED) && bAddSet) {
		eOp = Event::MSG_FILTER_VALUE_OP_MULT_ADD;
		nOperand = 1;
	}
	p0Event->addMsgFilter(nInFromMsg, nInToMsg, nInFromValue, nInToValue, bNewMsgDefined || bNewMsgNameDefined, nNewMsg, eOp, nOperand, nAddValue);
	oCtx.removeChecker(p0Element, true);
}
void XmlGameParser::parseListeners(GameCtx& oCtx, Event* p0Event, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	const std::string sEventName = p0Element->get_name();
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sEventListenerNodeName, [&](const xmlpp::Element* p0ListenerElement)
	{
		parseListener(oCtx, p0Event, sEventName, p0ListenerElement);
	});
	oCtx.removeChecker(p0Element, false);
}
void XmlGameParser::parseListener(GameCtx& oCtx, Event* p0Event, const std::string& sEventName, const xmlpp::Element* p0Element)
{
//std::cout << "XmlGameParser::parseListener" << '\n';
	oCtx.addChecker(p0Element);
	assert(p0Event != nullptr);
	assert(p0Element != nullptr);
	const auto oPairEventId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventListenerEventAttr);
	if (!oPairEventId.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sEventListenerEventAttr);
	}
	const std::string& sEventId = oPairEventId.second;
	if (sEventId.empty()) {
		throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, s_sEventListenerEventAttr);
	}
	const auto itFound = m_oIdEvent.find(sEventId);
	if (itFound == m_oIdEvent.end()) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sEventListenerEventAttr, Util::stringCompose(
														"Event with id '%2' in attribute '%1' not defined"
														, s_sEventListenerEventAttr, sEventId));
	}
//std::cout << "parseListener   sEventId=" << sEventId << '\n';
	Event* p0Listener = itFound->second.first;
	assert(p0Listener != nullptr);
	int32_t nGroup = -1;
	const auto oPairGroupName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventListenerGroupNameAttr);
	const bool bGroupNameDefined = oPairGroupName.first;
	if (bGroupNameDefined) {
		const std::string& sGroupName = oPairGroupName.second;
		nGroup = parseEventListenerGroupName(oCtx, sEventName, p0Element, s_sEventListenerGroupNameAttr, sGroupName);
	}
	const auto oPairGroup = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventListenerGroupAttr);
	const bool bGroupDefined = oPairGroup.first;
	if (bGroupDefined) {
		if (bGroupNameDefined) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sEventListenerGroupAttr
																	, s_sEventListenerGroupNameAttr);
		}
		const std::string& sGroup = oPairGroup.second;
		nGroup = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventListenerGroupAttr, sGroup, false
														, true, -1, false, -1);
	}
	//
	int32_t nMsg = -1;
	const xmlpp::Element* p0ListenerElement = itFound->second.second;
	assert(p0ListenerElement != nullptr);
	const std::string sListenerEventName = p0ListenerElement->get_name();
	const auto oPairMsgName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventListenerMsgNameAttr);
	const bool bMsgNameDefined = oPairMsgName.first;
	if (bMsgNameDefined) {
		const std::string& sMsgName = oPairMsgName.second;
		nMsg = parseEventMsgName(oCtx, sListenerEventName, p0Element, s_sEventListenerMsgNameAttr, sMsgName);
	}
	const auto oPairMsg = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventListenerMsgAttr);
	const bool bMsgDefined = oPairMsg.first;
	if (bMsgDefined) {
		if (bMsgNameDefined) {
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sEventListenerMsgAttr
																	, s_sEventListenerMsgNameAttr);
		}
		const std::string& sMsg = oPairMsg.second;
		nMsg = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventListenerMsgAttr, sMsg, false
														, true, -1, false, -1);
	}
	p0Event->addListener(nGroup, p0Listener, nMsg);
	oCtx.removeChecker(p0Element, true);
}
bool XmlGameParser::isReservedChildElementOfEvent(const std::string& sElementName) const
{
	auto itFound = std::find_if(s_aEventReservedChildElements.begin(), s_aEventReservedChildElements.end()
								, [&](std::string const* p0Str)
	{
		return (*p0Str == sElementName);
	});
	return (itFound != s_aEventReservedChildElements.end());
}

int32_t XmlGameParser::parseEventAttrStart(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	const auto oPairStart = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventStartAttr);
	int32_t nStart = -1;
	if (oPairStart.first) {
		nStart = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventStartAttr, oPairStart.second, false
														, true, 0, false, -1);
	}
	return nStart;
}
int32_t XmlGameParser::parseEventAttrRepeat(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	const auto oPairRepeat = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventRepeatAttr);
	int32_t nRepeat = s_nEventRepeatAttrDefault;
	if (oPairRepeat.first) {
		const std::string& sRepeat = oPairRepeat.second;
		nRepeat = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventRepeatAttr, sRepeat, false
														, true, 1, false, -1);
	}
	return nRepeat;
}
int32_t XmlGameParser::parseEventAttrStep(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	const auto oPairStep = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventStepAttr);
	int32_t nStep = s_nEventStepAttrDefault;
	if (oPairStep.first) {
		const std::string& sStep = oPairStep.second;
		nStep = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventStepAttr, sStep, false
													, true, 1, false, -1);
	}
	return nStep;
}
int32_t XmlGameParser::parseEventAttrRandomProb(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	const auto oPairProb = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sEventRandomProbAttr);
	int32_t nProb = s_nEventRandomProbDefault;
	if (oPairProb.first) {
		const std::string& sProb = oPairProb.second;
		nProb = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventRandomProbAttr, sProb, false
													, true, 1, true, s_nEventRandomProbMax);
	}
	return nProb;
}


} // namespace stmg
