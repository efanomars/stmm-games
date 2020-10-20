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
 * File:   level.cc
 */

#include "level.h"

#include "event.h"
#include "game.h"
#include "tileanimator.h"

#include "block.h"
#include "layout.h"
#include "named.h"
#include "util/util.h"
#include "util/coords.h"
#include "util/namedindex.h"
#include "utile/tilecoords.h"
#include "utile/tilerect.h"

#include <cassert>
#include <limits>
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <iterator>
#include <tuple>
#include <type_traits>

namespace stmg { class KeyActionEvent; }
namespace stmi { class Event; }


namespace stmg
{

void Level::Cell::clear(int32_t nTileAnis) noexcept
{
//std::cout << "Cell::clear()" << nTileAnis << '\n';
	m_oTile.clear();
	clearOnlyTileAnis(nTileAnis);
}
void Level::Cell::clearOnlyTileAnis(int32_t nTileAnis) noexcept
{
	assert(nTileAnis >= 0);
	m_aCellAni.resize(nTileAnis);
	for (int32_t nIdx=0; nIdx < nTileAnis; ++nIdx) {
		m_aCellAni[nIdx].m_p0TileAnimator = nullptr;
		m_aCellAni[nIdx].m_nHash = -1;
	}
}

const int32_t Level::s_nZObjectZShowText = 100000;
const int32_t Level::s_nZObjectZGameOver = std::numeric_limits<int32_t>::max();

Recycler<Level::PrivateTextAnimation, TextAnimation> Level::s_oTextAnimationRecycler{};
Recycler<TileCoords> Level::s_oTileCoordsRecycler{};

Level::Level(Game* p0Game, int32_t nLevel, const shared_ptr<AppPreferences>& refPreferences, const Init& oInit) noexcept
{
	reInit(p0Game, nLevel, refPreferences, oInit);
}
void Level::deInit() noexcept
{
	m_aEvents.clear();
	m_oActiveEvents.clear();
	m_oInactiveEvents.clear();
	m_nOthersNestedCalls = 0;
	m_oOthersListeners.clear();
	m_aDelayedLevAniIds.clear();
	m_aDelayedScrAniIds.clear();
	m_aDelayedLevAnis.clear();
	m_aDelayedScrAnis.clear();
	m_oActiveLevAnis.clear();
	m_oActiveScrolledAnis.clear();
}
Event* Level::getEventById(const std::string& sId) noexcept
{
	const int32_t nIdx = m_oEventIds.getIndex(sId);
	if (nIdx < 0) {
		return nullptr;
	}
	return m_oEventIds.getObj(nIdx);
}
void Level::addEvent(const std::string& sId, unique_ptr<Event> refEvent) noexcept
{
	const bool bHasName = ! sId.empty();
	if (bHasName) {
		assert(m_oEventIds.getIndex(sId) < 0);
	}
	Event* p0Event = refEvent.get();
	addEvent(std::move(refEvent));
	if (bHasName) {
		m_oEventIds.addNamedObj(sId, p0Event);
	}
}
void Level::addEvent(unique_ptr<Event> refEvent) noexcept
{
	assert(refEvent);
	assert(refEvent->m_p0Level == this);
	//
	m_aEvents.push_back({});
	refEvent.swap(m_aEvents.back());

	Event* p0Event = m_aEvents.back().get();
	p0Event->setIsActive(false);
	// Add to inactive events
	m_oInactiveEvents.push_back(p0Event);
}
bool Level::hasEvent(Event const * p0Event) const noexcept
{
	assert(p0Event != nullptr);
	const auto itFound = std::find_if(m_aEvents.begin(), m_aEvents.end(), [&](const unique_ptr<Event>& refCurEvent)
	{
		return (refCurEvent.get() == p0Event);
	});
	return (itFound != m_aEvents.end());
}
std::vector< unique_ptr<Event> > Level::extractAllEvents() noexcept
{
	auto aEvents = std::move(m_aEvents);
	deInit();
	return aEvents;
}

void Level::reInit(Game* p0Game, int32_t nLevel, const shared_ptr<AppPreferences>& refPreferences, const Init& oInit) noexcept
{
	deInit();
	assert(p0Game != nullptr);
	assert(nLevel >= 0);
	assert(refPreferences);
	assert((oInit.m_nBoardW > 0) && (oInit.m_nBoardW < 32000));
	assert((oInit.m_nBoardH > 0) && (oInit.m_nBoardH < 32000));
	assert((oInit.m_nShowW > 0) && (oInit.m_nShowW <= oInit.m_nBoardW));
	assert((oInit.m_nShowH > 0) && (oInit.m_nShowH <= oInit.m_nBoardH));
	assert((oInit.m_nSubshowW <= 0) || (oInit.m_nSubshowW <= oInit.m_nShowW));
	assert((oInit.m_nSubshowH <= 0) || (oInit.m_nSubshowH <= oInit.m_nShowH));
	assert(((oInit.m_nSubshowW <= 0) && (oInit.m_nSubshowH <= 0)) || ((oInit.m_nSubshowW > 0) && (oInit.m_nSubshowH > 0)));
	assert(oInit.m_nInitialFallEachTicks > 0);

	m_oOwner.m_p0Game = p0Game;

	m_p0Game = p0Game;
	m_nLevelInGame = nLevel;
	m_p0View = nullptr;

	m_refPreferences = refPreferences;
	m_bAllTeamsInOneLevel = m_p0Game->isAllTeamsInOneLevel();
	m_nTotLevelTeams = (m_bAllTeamsInOneLevel ? m_refPreferences->getTotTeams() : 1);
	m_nTotLevelPlayers = (m_bAllTeamsInOneLevel ? m_refPreferences->getTotPlayers() : m_refPreferences->getTeam(m_nLevelInGame)->getTotMates());

	m_aTeamData.resize(m_nTotLevelTeams);
	m_aLevelPlayer.resize(m_nTotLevelPlayers);
	int32_t nLevelPlayer = 0;
	for (int32_t nCurTeam = 0; nCurTeam < m_nTotLevelTeams; ++nCurTeam) {
		const int32_t nPrefTeam = m_bAllTeamsInOneLevel ? nCurTeam : m_nLevelInGame;
		TeamData& oTeamData = m_aTeamData[nCurTeam];
		oTeamData.m_refPrefTeam = m_refPreferences->getTeam(nPrefTeam);
		oTeamData.m_nTotTeammates = oTeamData.m_refPrefTeam->getTotMates();
		oTeamData.m_nTotInControl = 0;
		oTeamData.m_aTeammate.resize(oTeamData.m_nTotTeammates);
		oTeamData.m_oOrderedControllable.clear();
		for (int32_t nCurMate = 0; nCurMate < oTeamData.m_nTotTeammates; ++nCurMate) {
			MateData& oMateData = oTeamData.m_aTeammate[nCurMate];
			oMateData.m_refPrefPlayer = oTeamData.m_refPrefTeam->getMate(nCurMate);
			oMateData.m_p0Controlled = nullptr;
			oMateData.m_nLastTimeInControl = std::numeric_limits<int32_t>::lowest() + nCurMate;
			oMateData.m_nLevelPlayer = nLevelPlayer;
			m_aLevelPlayer[nLevelPlayer] = &oMateData;
			++nLevelPlayer;
		}
		oTeamData.m_bGameEnded = false;
	}
	m_aHelperMinTeam.resize(m_nTotLevelPlayers);
	m_aHelperMinTeammate.resize(m_nTotLevelPlayers);

	m_nW = oInit.m_nBoardW;
	m_nH = oInit.m_nBoardH;
	m_nTotTileAnis = -1;
	m_bBoardAllowOnlyModify = false;
	m_bBlockDisallowNestedPlayerChanges = false;

	m_oShow.m_bIsSubshow = false;
	m_oShow.m_nShowW = oInit.m_nShowW;
	m_oShow.m_nShowH = oInit.m_nShowH;
	m_oShow.m_oXY = FPoint{static_cast<double>(oInit.m_nShowInitX), static_cast<double>(oInit.m_nShowInitY)};
	m_oShow.m_fShowMaxX = oInit.m_nBoardW - oInit.m_nShowW;
	m_oShow.m_fShowMaxY = oInit.m_nBoardH - oInit.m_nShowH;
	m_oShow.m_p0Level = this;

	m_bSubshowMode = ((oInit.m_nSubshowW > 0) && (oInit.m_nSubshowH > 0));

	for (int32_t nCurTeam = 0; nCurTeam < m_nTotLevelTeams; ++nCurTeam) {
		TeamData& oTeamData = m_aTeamData[nCurTeam];
		for (int32_t nCurMate = 0; nCurMate < oTeamData.m_nTotTeammates; ++nCurMate) {
			MateData& oMateData = oTeamData.m_aTeammate[nCurMate];
			LevelShow& oSubshow = oMateData.m_oSubshow;
			oSubshow.m_bIsSubshow = true;
			if (m_bSubshowMode) {
				oSubshow.m_nShowW = oInit.m_nSubshowW;
				oSubshow.m_nShowH = oInit.m_nSubshowH;
				oSubshow.m_fShowMaxX = m_oShow.m_nShowW - oInit.m_nSubshowW;
				oSubshow.m_fShowMaxY = m_oShow.m_nShowH - oInit.m_nSubshowH;
			} else {
				oSubshow.m_nShowW = 0;
				oSubshow.m_nShowH = 0;
				oSubshow.m_fShowMaxX = -1;
				oSubshow.m_fShowMaxY = -1;
			}
			oSubshow.m_oXY = FPoint{0.0, 0.0};
			oSubshow.m_p0Level = this;
		}
	}

	m_aBoard.resize(m_nW * m_nH, Cell());
	m_aOwner.resize(m_nW * m_nH, nullptr);
	const int32_t nTotCellsToCopy = std::min(m_nW * m_nH, static_cast<int32_t>(oInit.m_aBoard.size()));
	for (int32_t nIdx = 0; nIdx <  nTotCellsToCopy; ++nIdx) {
		m_aBoard[nIdx].m_oTile = oInit.m_aBoard[nIdx];
		m_aOwner[nIdx] = nullptr;
	}

	m_fInterval = m_p0Game->gameInterval();
	m_nFallEachTicks = oInit.m_nInitialFallEachTicks;
//std::cout << "Level::reInit m_nFallEachTicks = " << m_nFallEachTicks << '\n';
	m_nScrolledUnique = 0;
	m_nTotGameEndedTeams = -1;
	m_bDirtyEvents = false;
}

int32_t Level::getLevel() const noexcept
{
	return m_nLevelInGame;
}
int32_t Level::getTotLevelPlayers() const noexcept
{
	return m_nTotLevelPlayers;
}
int32_t Level::getTotLevelTeams() const noexcept
{
	return m_nTotLevelTeams;
}

void Level::gameStart() noexcept
{
	assert(!game().isInGameTick());

	m_nTotGameEndedTeams = 0;

	// At this point the Named instance was filled (by the events)
	// and it's possible to resize the TileAni vector of Cell
	// Since more TileAni names could be added (by mistake) by the Events
	// at runtime (before they are added to the game's levels or more
	// practically past their constructor or reInit function),
	// the number of idxs is saved in a variable so that trying to set
	// the TileAnimator for these wrongly added names would fire an assert
	m_nTotTileAnis = game().getNamed().tileAnis().size();
//std::cout << "Level::gameStart() nTotAnis=" << nTotAnis << " m_nW=" << m_nW << " m_nH=" << m_nH << '\n';
	for (int32_t nIdx = 0; nIdx < m_nW * m_nH; ++nIdx) {
		std::vector<CellAni>& aCellAni = m_aBoard[nIdx].m_aCellAni;
		aCellAni.resize(m_nTotTileAnis);
		for (int32_t nAniIdx = 0; nAniIdx < m_nTotTileAnis; ++nAniIdx) {
			aCellAni[nAniIdx].m_p0TileAnimator = nullptr;
			aCellAni[nAniIdx].m_nHash = -1;
		}
	}
}
GameProxy& Level::game() noexcept
{
	return m_oOwner;
}
const GameProxy& Level::game() const noexcept
{
	return m_oOwner;
}
void Level::setLevelView(LevelView* p0View) noexcept
{
	assert(!game().isInGameTick());
	m_p0View = p0View;
}
LevelBlock* Level::getControlled(int32_t nLevelTeam, int32_t nMate) noexcept
{
	assert(game().isInGameTick());
	assert(nLevelTeam < m_nTotLevelTeams);
	TeamData& oTeamData = m_aTeamData[nLevelTeam];
	assert((nMate >= 0) && (nMate < oTeamData.m_nTotTeammates));
	MateData& oMateData = oTeamData.m_aTeammate[nMate];
	return oMateData.m_p0Controlled;
}
void Level::handleInput(int32_t nLevelTeam, int32_t nMate, const shared_ptr<stmi::Event>& refEvent) noexcept
{
//std::cout << "Level::handleInput  nLevelTeam=" << nLevelTeam << "  nMate=" << nMate << '\n';
	LevelBlock* p0Controlled = getControlled(nLevelTeam, nMate);
	if (p0Controlled != nullptr) {
		p0Controlled->handleInput(refEvent);
	}
}
void Level::handleKeyActionInput(int32_t nLevelTeam, int32_t nMate, const shared_ptr<KeyActionEvent>& refEvent) noexcept
{
	LevelBlock* p0Controlled = getControlled(nLevelTeam, nMate);
	if (p0Controlled != nullptr) {
		p0Controlled->handleKeyActionInput(refEvent);
	}
}
const std::vector< std::pair<int32_t, int32_t> >& Level::getActiveHumanPlayers() noexcept
{
	m_aActiveHumanPlayers.clear();
	for (int32_t nLevelTeam = 0; nLevelTeam < m_nTotLevelTeams; ++nLevelTeam) {
		TeamData& oTeamData = m_aTeamData[nLevelTeam];
		for (int32_t nMate = 0; nMate < oTeamData.m_nTotTeammates; ++nMate) {
			MateData& oMateData = oTeamData.m_aTeammate[nMate];
			if ((oMateData.m_p0Controlled != nullptr) && (!oMateData.m_refPrefPlayer->isAI())) {
				m_aActiveHumanPlayers.emplace_back(nLevelTeam, nMate);
			}
		}
	}
	return m_aActiveHumanPlayers;
}
void Level::handlePreTimer() noexcept
{
	assert(!game().isInGameTick());
	eventsQueueSetDirty(true);
}
void Level::handleTimer() noexcept
{
	assert(game().isInGameTick());
	const int32_t nGameTick = game().gameElapsed();
	const int32_t nFallEachTicks = m_nFallEachTicks;
//std::cout << "Level::handleTimer() nGameTick=" << nGameTick << '\n';
	{
	auto itBlock = m_oAllLevelBlocks.begin();
	while (itBlock != m_oAllLevelBlocks.end()) {
		LevelBlock* pLevelBlock = itBlock->second;
		if (pLevelBlock->m_nLastTimerCall < nGameTick) {
			pLevelBlock->m_nLastTimerCall = nGameTick;
			pLevelBlock->handleTimer();
			// handleTimer() may have caused additions or deletions
			// in m_oAllLevelBlocks: restart!
			itBlock = m_oAllLevelBlocks.begin();
		} else {
			++itBlock;
		}
	}
	}
	if (nGameTick % nFallEachTicks == 0) {
		auto itBlock = m_oAllLevelBlocks.begin();
		while (itBlock != m_oAllLevelBlocks.end()) {
			LevelBlock* pLevelBlock = itBlock->second;
			if (pLevelBlock->m_nLastFallCall < nGameTick) {
				pLevelBlock->m_nLastFallCall = nGameTick;
				pLevelBlock->fall();
				// fall() may have caused additions or deletions
				// in m_aLevelBlocks: restart!
				itBlock = m_oAllLevelBlocks.begin();
			} else {
				++itBlock;
			}
		}
	}
}
void Level::handleTimerEvents() noexcept
{
	assert(game().isInGameTick());
	const int32_t nGameTick = game().gameElapsed();
//std::cout << "Level::handleTimerEvents() nGameTick=" << nGameTick << '\n';
	auto it = m_oActiveEvents.begin();
	while (it != m_oActiveEvents.end()) {
		Event* p0Event = *it;
//std::cout << "Level::handleTimer() p0Event->getTriggerTime()=" << p0Event->getTriggerTime() << '\n';
		if (p0Event->getTriggerTime() > nGameTick) {
			break;
		}
		triggerEvent(p0Event, 0, 0, nullptr);
		//restart
		it = m_oActiveEvents.begin();
	}
	eventsQueueSetDirty(false);
}
void Level::handlePostTimer() noexcept
{
	assert(!game().isInGameTick());
	animationStartDelayed();
}

void Level::triggerEvent(Event* p0Event, int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept
{
	assert(p0Event != nullptr);
	assert(nMsg >= -1);
	if ((p0TriggeringEvent != nullptr) && p0Event->hasMsgFilters()) {
		if (!p0Event->filterMsg(nMsg, nValue)) {
			return; //----------------------------------------------------------
		}
	}
	deactivateEvent(p0Event, true);
	eventsQueueSetDirty(true);
	p0Event->trigger(nMsg, nValue, p0TriggeringEvent);
}
void Level::eventsQueueSetDirty(bool bDirty) noexcept
{
	m_bDirtyEvents = bDirty;
}
bool Level::eventsQueueIsDirty() const noexcept
{
	return m_bDirtyEvents;
}
void Level::othersReceive(int32_t
						#ifndef NDEBUG
						nSenderLevel
						#endif //NDEBUG
						, int32_t nMsg, int32_t nValue) noexcept
{
//std::cout << "Level::othersReceive(" << nMsg << ", " << nValue << ")    nSenderLevel=" << nSenderLevel << '\n';
	assert(nSenderLevel != getLevel());
	++m_nOthersNestedCalls;
	if (m_nOthersNestedCalls >= 10) {
		std::cout << "Level::othersReceive: Too many nested calls ... ignoring" << '\n';
	} else {
		for (const auto& p0OthersListener : m_oOthersListeners) {
			assert(p0OthersListener != nullptr);
			triggerEvent(p0OthersListener, nMsg, nValue, p0OthersListener);
		}
	}
	--m_nOthersNestedCalls;
}
bool Level::othersAddListener(Event* p0Listener) noexcept
{
//std::cout << "Level::othersAddListener()" << '\n';
	assert(p0Listener != nullptr);
	const auto itFound = std::find(m_oOthersListeners.begin(), m_oOthersListeners.end(), p0Listener);
	const bool bAlreadyAdded = (itFound != m_oOthersListeners.end());
	if (!bAlreadyAdded) {
		m_oOthersListeners.insert(m_oOthersListeners.begin(), p0Listener);
	}
	return !bAlreadyAdded;
}
bool Level::othersRemoveListener(Event* p0Listener) noexcept
{
//std::cout << "Level::othersRemoveListener()" << '\n';
	assert(p0Listener != nullptr);
	const bool bFound = Util::listExtract<Event*>(m_oOthersListeners, p0Listener);
	return bFound;
}

int32_t Level::getFallEachTicks() noexcept
{
	return m_nFallEachTicks;
}
void Level::setFallEachTicks(int32_t nFallEachTicks) noexcept
{
	if (nFallEachTicks < 1) {
		m_nFallEachTicks = 1;
	} else {
		m_nFallEachTicks = nFallEachTicks;
	}
}

int32_t Level::boardWidth() const noexcept
{
	return m_nW;
}
int32_t Level::boardHeight() const noexcept
{
	return m_nH;
}
void Level::boardSetTile(int32_t nX, int32_t nY, const Tile& oTile) noexcept
{
	if (boardGetTile(nX, nY) == oTile) {
		// tile unchanged
		return;
	}
	shared_ptr<TileCoords> refTileCoords;
	s_oTileCoordsRecycler.create(refTileCoords, 1);
	refTileCoords->add(nX, nY, oTile);
	boardModify(*refTileCoords);
}
const Tile& Level::boardGetTile(int32_t nX, int32_t nY) const noexcept
{
	assert((nX >= 0) && (nX < boardWidth()));
	assert((nY >= 0) && (nY < boardHeight()));
	return m_aBoard[calcIndex(nX,nY)].m_oTile;
}
int32_t Level::getNrTileAniAttrs() const noexcept
{
	return m_nTotTileAnis;
}
double Level::boardGetTileAniElapsed(int32_t nX, int32_t nY, int32_t nIdxTileAni, int32_t nViewTick, int32_t nTotTicks) const noexcept
{
	assert((nX >= 0) && (nX < boardWidth()));
	assert((nY >= 0) && (nY < boardHeight()));
	assert((nIdxTileAni >= 0) && (nIdxTileAni < getNrTileAniAttrs()));
	const Cell& oCell = m_aBoard[calcIndex(nX,nY)];
	const std::vector<CellAni>& aCellAni = oCell.m_aCellAni;
	assert(static_cast<int32_t>(aCellAni.size()) == getNrTileAniAttrs());
	const CellAni& oCellAni = aCellAni[nIdxTileAni];
	const TileAnimator* p0TileAnimator = oCellAni.m_p0TileAnimator;
	if (p0TileAnimator == nullptr) {
		return TileAnimator::s_fInactiveElapsed;
	}
	return p0TileAnimator->getElapsed01(oCellAni.m_nHash, nX, nY, nIdxTileAni, nViewTick, nTotTicks);
}
void Level::boardSetTileAnimator(int32_t nX, int32_t nY, int32_t nIdxTileAni, TileAnimator* p0TileAnimator, int32_t nHash) noexcept
{
	assert((nX >= 0) && (nX < boardWidth()));
	assert((nY >= 0) && (nY < boardHeight()));
	assert((nIdxTileAni >= 0) && (nIdxTileAni < getNrTileAniAttrs()));
	Cell& oCell = m_aBoard[calcIndex(nX,nY)];
	std::vector<CellAni>& aCellAni = oCell.m_aCellAni;
	assert(static_cast<int32_t>(aCellAni.size()) == getNrTileAniAttrs());
	CellAni& oCellAni = aCellAni[nIdxTileAni];
	oCellAni.m_p0TileAnimator = p0TileAnimator;
	oCellAni.m_nHash = nHash;
}
const TileAnimator* Level::boardGetTileAnimator(int32_t nX, int32_t nY, int32_t nIdxTileAni) const noexcept
{
//std::cout << "Level::boardGetTileAnimator() nX=" << nX << " nY=" << nY << " nIdxTileAni=" << nIdxTileAni << '\n';
	assert((nX >= 0) && (nX < boardWidth()));
	assert((nY >= 0) && (nY < boardHeight()));
//if (! ((nIdxTileAni >= 0) && (nIdxTileAni < getNrTileAniAttrs()))) {
//std::cout << "Level::boardGetTileAnimator FATAL ERROR!" << '\n';
//}
	assert((nIdxTileAni >= 0) && (nIdxTileAni < getNrTileAniAttrs()));
	const Cell& oCell = m_aBoard[calcIndex(nX,nY)];
	const std::vector<CellAni>& aCellAni = oCell.m_aCellAni;
	assert(static_cast<int32_t>(aCellAni.size()) == getNrTileAniAttrs());
	const CellAni& oCellAni = aCellAni[nIdxTileAni];
	return oCellAni.m_p0TileAnimator;
}

void Level::boardSetOwner(int32_t nX, int32_t nY, LevelBlock* pLevelBlock) noexcept
{
	assert((nX >= 0) && (nX < boardWidth()));
	assert((nY >= 0) && (nY < boardHeight()));
	m_aOwner[calcIndex(nX,nY)] = pLevelBlock;
}
LevelBlock* Level::boardGetOwner(int32_t nX, int32_t nY) const noexcept
{
	assert((nX >= 0) && (nX < boardWidth()));
	assert((nY >= 0) && (nY < boardHeight()));
	return m_aOwner[calcIndex(nX,nY)];
}
void Level::boardScrollAddListener(BoardScrollListener* p0Listener) noexcept
{
	assert(p0Listener != nullptr);
	m_oBoardScrollListenerStk.addListener(p0Listener);
}
void Level::boardScrollRemoveListener(BoardScrollListener* p0Listener) noexcept
{
	assert(p0Listener != nullptr);
	m_oBoardScrollListenerStk.removeListener(p0Listener);
}
void Level::boardAddListener(BoardListener* p0Listener) noexcept
{
//std::cout << "Level::boardAddListener()" << '\n';
	assert(p0Listener != nullptr);
	m_oBoardListenerStk.addListener(p0Listener);
	m_oBoardScrollListenerStk.addListener(p0Listener);
	m_oBoaBloListenerStk.addListener(p0Listener);
}
void Level::boardRemoveListener(BoardListener* p0Listener) noexcept
{
//std::cout << "Level::boardRemoveListener()" << '\n';
	assert(p0Listener != nullptr);
	m_oBoardListenerStk.removeListener(p0Listener);
	m_oBoardScrollListenerStk.removeListener(p0Listener);
	m_oBoaBloListenerStk.removeListener(p0Listener);
}
template< class TTT >
void Level::boardMoveVector(std::vector<TTT>& aVec, Direction::VALUE eDir
							, NRect oArea
							, int32_t& nInsertX, int32_t& nInsertY) noexcept
{
	const auto& nX = oArea.m_nX;
	const auto& nY = oArea.m_nY;
	const auto& nW = oArea.m_nW;
	const auto& nH = oArea.m_nH;
	nInsertX = -1;
	nInsertY = -1;
	if ((eDir == Direction::UP) || (eDir == Direction::DOWN)) {
		if (eDir == Direction::DOWN) {
			nInsertY = nY;
			for (int32_t nCurY = nY + nH - 1; nCurY > nInsertY; --nCurY) {
				for (int32_t nCurX = nX; nCurX < nX + nW; ++nCurX) {
					aVec[calcIndex(nCurX, nCurY)] = aVec[calcIndex(nCurX, nCurY - 1)];
				}
			}
		} else {
			nInsertY = nY + nH - 1;
			for (int32_t nCurY = nY; nCurY < nInsertY; ++nCurY) {
				for (int32_t nCurX = nX; nCurX < nX + nW; ++nCurX) {
					aVec[calcIndex(nCurX, nCurY)] = aVec[calcIndex(nCurX, nCurY + 1)];
				}
			}
		}
	} else {
		assert((eDir == Direction::RIGHT) || (eDir == Direction::LEFT));
		if (eDir == Direction::RIGHT) {
			nInsertX = nX;
			for (int32_t nCurX = nX + nW - 1; nCurX > nInsertX; --nCurX) {
				for (int32_t nCurY = nY; nCurY < nY + nH; ++nCurY) {
					aVec[calcIndex(nCurX, nCurY)] = aVec[calcIndex(nCurX - 1, nCurY)];
				}
			}
		} else {
			nInsertX = nX + nW - 1;
			for (int32_t nCurX = nX; nCurX < nInsertX; ++nCurX) {
				for (int32_t nCurY = nY; nCurY < nY + nH; ++nCurY) {
					aVec[calcIndex(nCurX, nCurY)] = aVec[calcIndex(nCurX + 1, nCurY)];
				}
			}
		}
	}
}
void Level::boardSetInserted(NRect oArea
							, int32_t nInsertX, int32_t nInsertY, const shared_ptr<TileRect>& refTiles) noexcept
{
	const auto& nX = oArea.m_nX;
	const auto& nY = oArea.m_nY;
	const auto& nW = oArea.m_nW;
	const auto& nH = oArea.m_nH;
	const bool bEmptyTiles = !refTiles;
	if (nInsertY >= 0) {
		assert(nInsertX < 0);
		assert(bEmptyTiles || (refTiles->getW() >= nW));
		for (int32_t nC = 0; nC < nW; ++nC) {
			const int32_t nCalcIdx = calcIndex(nX + nC, nInsertY);
			Cell& oCell = m_aBoard[nCalcIdx];
			oCell.clearOnlyTileAnis(getNrTileAniAttrs());
			if (!bEmptyTiles) {
				oCell.m_oTile = refTiles->get({nC, 0});
			} else {
				oCell.m_oTile.clear();
			}
		}
	} else {
		assert(nInsertX >= 0);
		assert(bEmptyTiles || (refTiles->getH() >= nH));
		for (int32_t nC = 0; nC < nH; ++nC) {
			const int32_t nCalcIdx = calcIndex(nInsertX, nY + nC);
			Cell& oCell = m_aBoard[nCalcIdx];
			oCell.clearOnlyTileAnis(getNrTileAniAttrs());
			if (!bEmptyTiles) {
				oCell.m_oTile = refTiles->get({0, nC});
			} else {
				oCell.m_oTile.clear();
			}
		}
	}
}
void Level::boardScroll(Direction::VALUE eDir, const shared_ptr<TileRect>& refTiles) noexcept
{
//std::cout << "Level::boardScroll()" << '\n';
	if (m_bBoardAllowOnlyModify) {
		assert(false);
		return;
	}
	m_bBoardAllowOnlyModify = true;

	auto itPreCalled = m_oBoardScrollListenerStk.grabPreCalled();
	m_oBoardScrollListenerStk.callPre(itPreCalled, &BoardScrollListener::boardPreScroll, eDir, refTiles);

	// Check none of the removed cells has m_pOwner still set!?
	//for (auto& oLBPair : m_oAllLevelBlocks) {
	int32_t nDelX, nDelY, nDelW, nDelH;
	if ((eDir == Direction::UP) || (eDir == Direction::DOWN)) {
		if (eDir == Direction::DOWN) {
			nDelY = m_nH - 1;
		} else {
			nDelY = 0;
		}
		nDelH = 1;
		nDelX = 0;
		nDelW = m_nW;
	} else {
		assert((eDir == Direction::RIGHT) || (eDir == Direction::LEFT));
		if (eDir == Direction::RIGHT) {
			nDelX = m_nW - 1;
		} else {
			nDelX = 0;
		}
		nDelW = 1;
		nDelY = 0;
		nDelH = m_nH;
	}
	for (int32_t nCurX = nDelX; nCurX < nDelX + nDelW; ++nCurX) {
		for (int32_t nCurY = nDelY; nCurY < nDelY + nDelH; ++nCurY) {
			int32_t nCurIdx = calcIndex(nCurX, nCurY);
			LevelBlock* p0LevelBlock = m_aOwner[nCurIdx];
			if (p0LevelBlock != nullptr) {
				bool bDone = p0LevelBlock->freeze();
				if (!bDone) {
					bDone = p0LevelBlock->remove();
					if (!bDone) {
						bDone = p0LevelBlock->destroy();
					}
				}
				if ((!bDone) || (m_aOwner[nCurIdx] != nullptr)) {
					gameStatusTechnical(std::vector<std::string>{"Level::boardScroll", "A LevelBlock was scrolled out of the board!"});
					//TODO m_oBoardScrollListenerStk.freePreCalled(itPreCalled);
					// but currently asserts Post has been called
					return; //--------------------------------------------------
				}
			}
		}
	}

	if (m_p0View != nullptr) {
		m_p0View->boardPreScroll(eDir, refTiles);
	}

	++m_nScrolledUnique;
	const uint32_t nScrolledUnique = m_nScrolledUnique;

	int32_t nInsertX, nInsertY;
	NRect oArea;
	oArea.m_nW = m_nW;
	oArea.m_nH = m_nH;
	boardMoveVector(m_aBoard, eDir, oArea, nInsertX, nInsertY);
	boardSetInserted(oArea, nInsertX, nInsertY, refTiles);
	boardMoveVector(m_aOwner, eDir, oArea, nInsertX, nInsertY);
	if (nInsertY >= 0) {
		assert(nInsertX < 0);
		for (int32_t nC = 0; nC < oArea.m_nW; ++nC) {
			const int32_t nCalcIdx = calcIndex(oArea.m_nX + nC, nInsertY);
			m_aOwner[nCalcIdx] = nullptr;
		}
	} else {
		assert(nInsertX >= 0);
		for (int32_t nC = 0; nC < oArea.m_nH; ++nC) {
			const int32_t nCalcIdx = calcIndex(nInsertX, oArea.m_nY + nC);
			m_aOwner[nCalcIdx] = nullptr;
		}
	}

	// Move LevelBlock if blockIsAutoScrolled()
	const int32_t nDeltaX = Direction::deltaX(eDir);
	const int32_t nDeltaY = Direction::deltaY(eDir);
	for (auto& oLBPair : m_oAllLevelBlocks) {
		//const int32_t nId = oLBPair.first;
		LevelBlock* p0LevelBlock = oLBPair.second;
		assert(p0LevelBlock != nullptr);
		if (p0LevelBlock->blockIsAutoScrolled()) {
			p0LevelBlock->m_nScrolledUnique = nScrolledUnique;
			p0LevelBlock->m_nPosX += nDeltaX;
			p0LevelBlock->m_nPosY += nDeltaY;
		}
	}
	if (!m_oActiveScrolledAnis.empty()) {
		for (auto& oIdSAPair : m_oActiveScrolledAnis) {
			shared_ptr<LevelAnimation>& refLevelAnimation = oIdSAPair.second;
			refLevelAnimation->m_nScrolledUnique = nScrolledUnique;
			refLevelAnimation->m_oPos.m_fX += nDeltaX;
			refLevelAnimation->m_oPos.m_fY += nDeltaY;
		}
	}

	if (m_p0View != nullptr) {
		m_p0View->boardPostScroll(eDir);
	}

	m_oBoardScrollListenerStk.callPost(itPreCalled, &BoardScrollListener::boardPostScroll, eDir);
	m_oBoardScrollListenerStk.freePreCalled(itPreCalled);

	m_bBoardAllowOnlyModify = false;

	for (auto& oLBPair : m_oAllLevelBlocks) {
		LevelBlock* p0LevelBlock = oLBPair.second;
		assert(p0LevelBlock != nullptr);
		if (p0LevelBlock->blockIsAutoScrolled() && (p0LevelBlock->m_nScrolledUnique == nScrolledUnique)) {
			p0LevelBlock->onScrolled(eDir);
		}
	}
	for (auto& oIdSAPair : m_oActiveScrolledAnis) {
		shared_ptr<LevelAnimation>& refLevelAnimation = oIdSAPair.second;
		if (refLevelAnimation->m_nScrolledUnique == nScrolledUnique) {
			refLevelAnimation->onScrolled(eDir);
		}
	}
}
void Level::boardInsert(Direction::VALUE eDir, NRect oArea, const shared_ptr<TileRect>& refTiles) noexcept
{
//std::cout << "Level::boardInsert()" << '\n';
	assert((oArea.m_nX >= 0) && (oArea.m_nW > 0) && (oArea.m_nX + oArea.m_nW <= boardWidth()));
	assert((oArea.m_nY >= 0) && (oArea.m_nH > 0) && (oArea.m_nY + oArea.m_nH <= boardHeight()));
	if (m_bBoardAllowOnlyModify) {
		assert(false);
		return;
	}
	m_bBoardAllowOnlyModify = true;

	auto itPreCalled = m_oBoardListenerStk.grabPreCalled();
	m_oBoardListenerStk.callPre(itPreCalled, &BoardListener::boardPreInsert, eDir, oArea, refTiles);

	if (m_p0View != nullptr) {
		m_p0View->boardPreInsert(eDir, oArea, refTiles);
	}

	int32_t nInsertX, nInsertY;
	boardMoveVector(m_aBoard, eDir, oArea, nInsertX, nInsertY);
	boardSetInserted(oArea, nInsertX, nInsertY, refTiles);

	if (m_p0View != nullptr) {
		m_p0View->boardPostInsert(eDir, oArea);
	}

	m_oBoardListenerStk.callPost(itPreCalled, &BoardListener::boardPostInsert, eDir, oArea);
	m_oBoardListenerStk.freePreCalled(itPreCalled);

	m_bBoardAllowOnlyModify = false;
}
void Level::boardModify(const TileCoords& oTileCoords) noexcept
{
//std::cout << "Level::boardModify  oTileCoords.size()=" << oTileCoords.size() << '\n';

	auto itPreCalled = m_oBoardListenerStk.grabPreCalled();
	m_oBoardListenerStk.callPre(itPreCalled, &BoardListener::boardPreModify, oTileCoords);

	if (m_p0View != nullptr) {
		m_p0View->boardPreModify(oTileCoords);
	}

	TileCoords::const_iterator it = oTileCoords.begin();
	const TileCoords::const_iterator itEnd = oTileCoords.end();
	while (it != itEnd) {
		const int32_t nCalcIdx = calcIndex(it.x(), it.y());
		Cell& oCell = m_aBoard[nCalcIdx];
		oCell.m_oTile = it.getTile();
		it.next();
	}

	if (m_p0View != nullptr) {
		m_p0View->boardPostModify(oTileCoords);
	}

	m_oBoardListenerStk.callPost(itPreCalled, &BoardListener::boardPostModify, oTileCoords);
	m_oBoardListenerStk.freePreCalled(itPreCalled);
}
void Level::boardDestroy(const Coords& oCoords) noexcept
{
//std::cout << "Level::boardDestroy(Coords)" << '\n';
	assert(oCoords.size() >= 0);

	auto itPreCalled = m_oBoardListenerStk.grabPreCalled();
	m_oBoardListenerStk.callPre(itPreCalled, &BoardListener::boardPreDestroy, oCoords);

	if (m_p0View != nullptr) {
		m_p0View->boardPreDestroy(oCoords);
	}

	// set cells to empty
	for (Coords::const_iterator it = oCoords.begin(); it != oCoords.end(); it.next()) {
		const int32_t nX = it.x();
		const int32_t nY = it.y();
		m_aBoard[calcIndex(nX, nY)].m_oTile.clear();
		// leaves tile animations intact!
	}

	if (m_p0View != nullptr) {
		m_p0View->boardPostDestroy(oCoords);
	}

	m_oBoardListenerStk.callPost(itPreCalled, &BoardListener::boardPostDestroy, oCoords);
	m_oBoardListenerStk.freePreCalled(itPreCalled);
}
void Level::boabloOwnerBlockSet(LevelBlock& oLevelBlock) noexcept
{
//std::cout << "Level::boabloOwnerBlockSet()" << '\n';
	const int32_t nShape = oLevelBlock.m_nShapeId;
	const int32_t nPosX = oLevelBlock.m_nPosX;
	const int32_t nPosY = oLevelBlock.m_nPosY;
	const Block& oBlock = oLevelBlock.m_oBlock;
	auto aBrickId = oBlock.brickIds();
	for (auto& nBrickId : aBrickId) {
		if (oBlock.shapeBrickVisible(nShape, nBrickId)) {
			const int32_t nX = nPosX + oBlock.shapeBrickPosX(nShape, nBrickId);
			const int32_t nY = nPosY + oBlock.shapeBrickPosY(nShape, nBrickId);
			boardSetOwner(nX, nY, &oLevelBlock);
		}
	}
}
void Level::boabloOwnerBlockClear(LevelBlock& oLevelBlock) noexcept
{
//std::cout << "Level::boardOwnerBlockReset()" << '\n';
	const int32_t nShape = oLevelBlock.m_nShapeId;
	const int32_t nPosX = oLevelBlock.m_nPosX;
	const int32_t nPosY = oLevelBlock.m_nPosY;
	const Block& oBlock = oLevelBlock.m_oBlock;
	auto aBrickId = oBlock.brickIds();
	for (auto& nBrickId : aBrickId) {
		if (oBlock.shapeBrickVisible(nShape, nBrickId)) {
			const int32_t nX = nPosX + oBlock.shapeBrickPosX(nShape, nBrickId);
			const int32_t nY = nPosY + oBlock.shapeBrickPosY(nShape, nBrickId);
			assert((boardGetOwner(nX, nY) == &oLevelBlock) || (boardGetOwner(nX, nY) == nullptr));
			boardSetOwner(nX, nY, nullptr);
		}
	}
}
LevelShow& Level::showGet() noexcept
{
	return m_oShow;
}
LevelShow& Level::subshowGet(int32_t nLevelTeam, int32_t nMate) noexcept
{
	assert((nLevelTeam >= 0) && (nLevelTeam < m_nTotLevelTeams));
	TeamData& oTeamData = m_aTeamData[nLevelTeam];
	assert((nMate >= 0) && (nMate < oTeamData.m_nTotTeammates));
	return oTeamData.m_aTeammate[nMate].m_oSubshow;
}
LevelShow& Level::subshowGet(int32_t nLevelPlayer) noexcept
{
	assert((nLevelPlayer >= 0) && (nLevelPlayer < m_nTotLevelPlayers));
	return m_aLevelPlayer[nLevelPlayer]->m_oSubshow;
}
void Level::activateEvent(Event* p0Event, int32_t nStart) noexcept
{
//std::cout << "Level::activateEvent  nStart: " << nStart << '\n';
	assert(p0Event != nullptr);
	assert(this == p0Event->m_p0Level);
	assert(nStart >= 0);
	#ifndef NDEBUG
	const bool bFound =
	#endif
	Util::listExtract<Event*>((p0Event->isActive() ? m_oActiveEvents : m_oInactiveEvents), p0Event);
	assert(bFound);
	p0Event->setIsActive(true);
	p0Event->setTriggerTime(nStart);

	auto it = m_oActiveEvents.begin();
	while (it != m_oActiveEvents.end()) {
		Event* p0OtherEvent = (*it);
		const int32_t nOtherStart = p0OtherEvent->getTriggerTime();
		if (! ((nOtherStart < nStart) || ((nOtherStart == nStart) && (p0OtherEvent->getPriority() > p0Event->getPriority())))) {
			// The priority of the activated event is higher, insert it before the other event
			break;
		}
		++it;
	}
	m_oActiveEvents.insert(it, p0Event);
}
void Level::deactivateEvent(Event* p0Event) noexcept
{
//std::cout << "Level::deactivateEvent" << '\n';
	deactivateEvent(p0Event, false);
}
void Level::deactivateEvent(Event* p0Event, bool bPreserveTriggerTime) noexcept
{
//std::cout << "Level::deactivateEvent" << '\n';
	assert(p0Event != nullptr);
	assert(this == p0Event->m_p0Level);
	if (!p0Event->isActive()) {
		// already inactive
		assert(Util::listContains<Event*>(m_oInactiveEvents, p0Event));
		return; //--------------------------------------------------------------
	}
	#ifndef NDEBUG
	const bool bFound =
	#endif
	Util::listExtract<Event*>(m_oActiveEvents, p0Event);
	assert(bFound);
	assert(!Util::listContains<Event*>(m_oInactiveEvents, p0Event));
	p0Event->setIsActive(false);
	if (! bPreserveTriggerTime) {
		p0Event->setTriggerTime(-1);
	}
	m_oInactiveEvents.push_back(p0Event);
}

bool Level::blockAdd(LevelBlock* p0LevelBlock, LevelBlock::MGMT_TYPE eMgmtType) noexcept
{
//std::cout << "Level::blockAdd" << '\n';
	assert(p0LevelBlock != nullptr);
	const bool bAutoOwner = ((eMgmtType == LevelBlock::MGMT_TYPE_AUTO_OWNER) || (eMgmtType == LevelBlock::MGMT_TYPE_AUTO_STRICT_OWNER));
	assert((eMgmtType == LevelBlock::MGMT_TYPE_NORMAL) || (eMgmtType == LevelBlock::MGMT_TYPE_AUTO_SCROLL) || bAutoOwner);
	const bool bAutoStrictOwner = (eMgmtType == LevelBlock::MGMT_TYPE_AUTO_STRICT_OWNER);
	if (bAutoOwner) {
		if (!blockCanPlaceOnBoard(*p0LevelBlock, bAutoStrictOwner)) {
			return false; //----------------------------------------------------
		}
	}
	p0LevelBlock->m_eMgmtType = eMgmtType;

	assert(! p0LevelBlock->m_bNestedModificationLock);
	p0LevelBlock->m_bNestedModificationLock = true;

	auto itPreCalled = m_oBlocksListenerStk.grabPreCalled();
	m_oBlocksListenerStk.callPre(itPreCalled, &BlocksListener::blockPreAdd, *p0LevelBlock);

	if (m_p0View != nullptr) {
		m_p0View->blockPreAdd(*p0LevelBlock);
	}

	blockAddCommon(p0LevelBlock);
	if (bAutoOwner) {
		boabloOwnerBlockSet(*p0LevelBlock);
	}

	if (m_p0View != nullptr) {
		m_p0View->blockPostAdd(*p0LevelBlock);
	}

	m_oBlocksListenerStk.callPost(itPreCalled, &BlocksListener::blockPostAdd, *p0LevelBlock);
	m_oBlocksListenerStk.freePreCalled(itPreCalled);

	p0LevelBlock->m_bNestedModificationLock = false;

	p0LevelBlock->onAdded();

	return true;
}
bool Level::boabloUnfreeze(const Coords& oCoords
							, LevelBlockCreator& oCreator
							, LevelBlock::MGMT_TYPE eMgmtType) noexcept
{
//std::cout << "Level::boabloUnfreeze" << '\n';
	assert(oCoords.size() > 0);
	const bool bAutoOwner = ((eMgmtType == LevelBlock::MGMT_TYPE_AUTO_OWNER) || (eMgmtType == LevelBlock::MGMT_TYPE_AUTO_STRICT_OWNER));
	assert((eMgmtType == LevelBlock::MGMT_TYPE_NORMAL) || (eMgmtType == LevelBlock::MGMT_TYPE_AUTO_SCROLL) || bAutoOwner);
	//const bool bAutoStrictOwner = (eMgmtType == LevelBlock::MGMT_TYPE_AUTO_STRICT_OWNER);
	if (bAutoOwner) {
		if (!coordsCanPlaceOnBoard(oCoords, false)) {
			return false; //----------------------------------------------------
		}
	}

	auto itPreCalled = m_oBoaBloListenerStk.grabPreCalled();
	m_oBoaBloListenerStk.callPre(itPreCalled, &BoaBloListener::boabloPreUnfreeze, oCoords);

	if (m_p0View != nullptr) {
		m_p0View->boabloPreUnfreeze(oCoords);
	}

	std::vector< std::tuple<bool, int32_t, int32_t> > aBrickPos;
	std::vector<Tile> aBrick;

	const int32_t nTotBricks = oCoords.size();
	aBrickPos.resize(nTotBricks);
	aBrick.resize(nTotBricks);
	const NRect oRect = oCoords.getMinMax();
	const int32_t nPosX = oRect.m_nX;
	const int32_t nPosY = oRect.m_nY;
	int32_t nBrick = 0;
	for (Coords::const_iterator it = oCoords.begin(); it != oCoords.end(); it.next()) {
		const int32_t nX = it.x();
		const int32_t nY = it.y();
//std::cout << "      :boabloUnfreeze  nX=" << nX << "  nY=" << nY << '\n';
		aBrickPos[nBrick] = std::make_tuple(true, nX - nPosX, nY - nPosY);
		const Tile& oTile = boardGetTile(nX, nY);
		assert(!oTile.isEmpty());
		aBrick[nBrick] = oTile;
		if (bAutoOwner) {
			assert(boardGetOwner(nX, nY) == nullptr);
		}
		boardSetTile(nX, nY, Tile::s_oEmptyTile);
		++nBrick;
	}
	assert(nBrick == nTotBricks);
	Block oBlock(nTotBricks, aBrick, 1, {aBrickPos});

	LevelBlock* p0LevelBlock = oCreator.create();

	assert(p0LevelBlock != nullptr);
	p0LevelBlock->m_oBlock = std::move(oBlock);
	p0LevelBlock->m_nPosX = nPosX;
	p0LevelBlock->m_nPosY = nPosY;
	p0LevelBlock->m_nShapeId = p0LevelBlock->m_oBlock.shapeFirst();
	p0LevelBlock->m_eMgmtType = eMgmtType;

	blockAddCommon(p0LevelBlock);
	if (bAutoOwner) {
		boabloOwnerBlockSet(*p0LevelBlock);
	}

	p0LevelBlock->m_bNestedModificationLock = true;

	if (m_p0View != nullptr) {
		m_p0View->boabloPostUnfreeze(*p0LevelBlock);
	}

	m_oBoaBloListenerStk.callPost(itPreCalled, &BoaBloListener::boabloPostUnfreeze, *p0LevelBlock);
	m_oBoaBloListenerStk.freePreCalled(itPreCalled);

	p0LevelBlock->m_bNestedModificationLock = false;

	p0LevelBlock->onUnfreezed();

	return true;
}
void Level::blockAddCommon(LevelBlock* p0LevelBlock) noexcept
{
//std::cout << "Level::blockAddCommon() id=" << p0LevelBlock->blockGetId() << '\n';
	assert(p0LevelBlock != nullptr);
	assert(p0LevelBlock->m_p0Level == nullptr);

	p0LevelBlock->resetCaches();
	p0LevelBlock->resetPrivate(getNrTileAniAttrs());
	p0LevelBlock->m_p0Level = this;

	const int32_t nId = p0LevelBlock->blockGetId();
	assert(m_oAllLevelBlocks.find(nId) == m_oAllLevelBlocks.end());
	m_oAllLevelBlocks[nId] = p0LevelBlock;

	//p0LevelBlock->m_bControllable = p0LevelBlock->isPlayerControllable();
	if (!p0LevelBlock->m_bControllable) {
		return; //--------------------------------------------------------------
	}

	const int32_t nControllerTeam = p0LevelBlock->getControllerTeam();
	blockAddToControllable(p0LevelBlock, nControllerTeam, -1);
	blockAssignControlToLongestWaitingMate(p0LevelBlock, nControllerTeam, -1);
}
void Level::blockAddToControllable(LevelBlock* p0LevelBlock, int32_t nControllerTeam, int32_t nNotTeam) noexcept
{
	const int32_t nExclusiveTeam = nControllerTeam;
	int32_t nFromTeam = nExclusiveTeam;
	int32_t nToTeam = nExclusiveTeam;
	if (nExclusiveTeam < 0) {
		nFromTeam = 0;
		nToTeam = m_nTotLevelTeams - 1;
	} else {
		assert(nExclusiveTeam < m_nTotLevelTeams);
	}
	p0LevelBlock->m_aItOrderedList.resize(m_nTotLevelTeams);
	//
	for (int32_t nCurTeam = nFromTeam; nCurTeam <= nToTeam; ++nCurTeam) {
		if (nCurTeam == nNotTeam) {
			continue; // for ---
		}
		TeamData& oTeamData = m_aTeamData[nCurTeam];
		// Add to oTeamData.m_oOrderedControllable
		std::list<LevelBlock*>& oOrderedList = oTeamData.m_oOrderedControllable;
		auto itLB = oOrderedList.begin();
		while ((itLB != oOrderedList.end()) && (!orderLevelBlocks(p0LevelBlock, *itLB))) {
			++itLB;
		}
		p0LevelBlock->m_aItOrderedList[nCurTeam] = oOrderedList.insert(itLB, p0LevelBlock);
	}
}
void Level::blockAssignControlToLongestWaitingMate(LevelBlock* p0LevelBlock, int32_t nControllerTeam, int32_t nNotTeam) noexcept
{
	const int32_t nExclusiveTeam = nControllerTeam;
	int32_t nFromTeam = nExclusiveTeam;
	int32_t nToTeam = nExclusiveTeam;
	if (nExclusiveTeam < 0) {
		nFromTeam = 0;
		nToTeam = m_nTotLevelTeams - 1;
	} else {
		assert(nExclusiveTeam < m_nTotLevelTeams);
	}
	int32_t nTotMinPlayers = 0;
	int32_t nMinLastInControlTime = std::numeric_limits<int32_t>::max();
	for (int32_t nCurTeam = nFromTeam; nCurTeam <= nToTeam; ++nCurTeam) {
		if (nCurTeam == nNotTeam) {
			continue; // for ---
		}
		TeamData& oTeamData = m_aTeamData[nCurTeam];
//std::cout << " Level::blockAssignControlToLongestWaitingMate nCurTeam=" << nCurTeam << " oTeamData.m_nTotInControl=" << oTeamData.m_nTotInControl << '\n';
//std::cout << "                                               oTeamData.m_nTotTeammates=" << oTeamData.m_nTotTeammates << '\n';
		if (oTeamData.m_nTotInControl < oTeamData.m_nTotTeammates) {
			// There is a mate that has no level block
			for (int32_t nCurMate = 0; nCurMate < oTeamData.m_nTotTeammates; ++nCurMate) {
				MateData& oMateData = oTeamData.m_aTeammate[nCurMate];
				if (oMateData.m_p0Controlled == nullptr) {
					const int32_t nLastTime = oMateData.m_nLastTimeInControl;
					if (nLastTime < nMinLastInControlTime) {
						nTotMinPlayers = 0;
						m_aHelperMinTeam[nTotMinPlayers] = nCurTeam;
						m_aHelperMinTeammate[nTotMinPlayers] = nCurMate;
						++nTotMinPlayers;
						nMinLastInControlTime = nLastTime;
					} else if (nLastTime == nMinLastInControlTime) {
						assert(nTotMinPlayers < m_nTotLevelPlayers);
						m_aHelperMinTeam[nTotMinPlayers] = nCurTeam;
						m_aHelperMinTeammate[nTotMinPlayers] = nCurMate;
						++nTotMinPlayers;
					}
				}
			}
		}
	}
	if (nTotMinPlayers <= 0) {
		// there is none ready to take control of the newly added block
		return; //--------------------------------------------------------------
	}
	int32_t nMinPlayerIdx = 0;
	if (nTotMinPlayers > 1) {
		nMinPlayerIdx = game().random(0, nTotMinPlayers - 1);
	}
	const int32_t nFindTeam = m_aHelperMinTeam[nMinPlayerIdx];
	const int32_t nFindMate = m_aHelperMinTeammate[nMinPlayerIdx];

	blockAssignToMate(p0LevelBlock, nFindTeam, nFindMate, false);
}
void Level::blockAssignToMate(LevelBlock* p0LevelBlock, int32_t nTeam, int32_t nMate, bool bWasntAssigned) noexcept
{
	TeamData& oTeamData = m_aTeamData[nTeam];
	MateData& oMateData = oTeamData.m_aTeammate[nMate];
	oMateData.m_p0Controlled = p0LevelBlock;
	oMateData.m_nLastTimeInControl = game().gameElapsed();
	++oTeamData.m_nTotInControl;
	p0LevelBlock->m_nTeam = nTeam;
	p0LevelBlock->m_nTeammate = nMate;
	const int32_t nOldPlayer = (bWasntAssigned ? -1 : p0LevelBlock->m_nPlayer);
	const int32_t nNewPlayer = oMateData.m_nLevelPlayer;
	p0LevelBlock->m_nPlayer = nNewPlayer;

	informBlockChangePlayer(p0LevelBlock, nOldPlayer);
}

void Level::blockRemove(LevelBlock* p0LevelBlock) noexcept
{
	assert(p0LevelBlock != nullptr);
	assert(! p0LevelBlock->m_bNestedModificationLock);

	const LevelBlock::MGMT_TYPE eMgmtType = p0LevelBlock->m_eMgmtType;
	const bool bAutoOwner = ((eMgmtType == LevelBlock::MGMT_TYPE_AUTO_OWNER) || (eMgmtType == LevelBlock::MGMT_TYPE_AUTO_STRICT_OWNER));

	if (p0LevelBlock->m_bNestedModificationLock) {
		assert(false);
		return;
	}

	p0LevelBlock->m_bNestedModificationLock = true;

	auto itPreCalled = m_oBlocksListenerStk.grabPreCalled();
	m_oBlocksListenerStk.callPre(itPreCalled, &BlocksListener::blockPreRemove, *p0LevelBlock);

	if (m_p0View != nullptr) {
		m_p0View->blockPreRemove(*p0LevelBlock);
	}

	if (bAutoOwner) {
		boabloOwnerBlockClear(*p0LevelBlock);
	}
	blockRemoveCommon(p0LevelBlock);

	if (m_p0View != nullptr) {
		m_p0View->blockPostRemove(*p0LevelBlock);
	}

	m_oBlocksListenerStk.callPost(itPreCalled, &BlocksListener::blockPostRemove, *p0LevelBlock);
	m_oBlocksListenerStk.freePreCalled(itPreCalled);

	p0LevelBlock->m_bNestedModificationLock = false;
}
void Level::blockDestroy(LevelBlock* p0LevelBlock) noexcept
{
	assert(p0LevelBlock != nullptr);
	assert(! p0LevelBlock->m_bNestedModificationLock);

	const LevelBlock::MGMT_TYPE eMgmtType = p0LevelBlock->m_eMgmtType;
	const bool bAutoOwner = ((eMgmtType == LevelBlock::MGMT_TYPE_AUTO_OWNER) || (eMgmtType == LevelBlock::MGMT_TYPE_AUTO_STRICT_OWNER));

	if (p0LevelBlock->m_bNestedModificationLock) {
		assert(false);
		return;
	}

	p0LevelBlock->m_bNestedModificationLock = true;

	auto itPreCalled = m_oBlocksListenerStk.grabPreCalled();
	m_oBlocksListenerStk.callPre(itPreCalled, &BlocksListener::blockPreDestroy, *p0LevelBlock);

	if (m_p0View != nullptr) {
		m_p0View->blockPreDestroy(*p0LevelBlock);
	}

	if (bAutoOwner) {
		boabloOwnerBlockClear(*p0LevelBlock);
	}
	blockRemoveCommon(p0LevelBlock);

	if (m_p0View != nullptr) {
		m_p0View->blockPostDestroy(*p0LevelBlock);
	}

	m_oBlocksListenerStk.callPost(itPreCalled, &BlocksListener::blockPostDestroy, *p0LevelBlock);
	m_oBlocksListenerStk.freePreCalled(itPreCalled);

	p0LevelBlock->m_bNestedModificationLock = false;
}
void Level::blockFreeze(LevelBlock* p0LevelBlock) noexcept
{
	assert(p0LevelBlock != nullptr);
	assert( !p0LevelBlock->m_bNestedModificationLock );

	const LevelBlock::MGMT_TYPE eMgmtType = p0LevelBlock->m_eMgmtType;
	const bool bAutoOwner = ((eMgmtType == LevelBlock::MGMT_TYPE_AUTO_OWNER) || (eMgmtType == LevelBlock::MGMT_TYPE_AUTO_STRICT_OWNER));

	if (p0LevelBlock->m_bNestedModificationLock) {
		assert(false);
		return;
	}

	p0LevelBlock->m_bNestedModificationLock = true;

	auto itPreCalled = m_oBoaBloListenerStk.grabPreCalled();
	m_oBoaBloListenerStk.callPre(itPreCalled, &BoaBloListener::boabloPreFreeze, *p0LevelBlock);

	if (m_p0View != nullptr) {
		m_p0View->boabloPreFreeze(*p0LevelBlock);
	}

	Coords oCoords;
	const int32_t nPosX = p0LevelBlock->m_nPosX;
	const int32_t nPosY = p0LevelBlock->m_nPosY;
	const int32_t nShape = p0LevelBlock->m_nShapeId;
	const Block& oBlock = p0LevelBlock->m_oBlock;
	auto aBrickId = oBlock.brickIds();
	for (auto& nBrickId : aBrickId) {
		if (oBlock.shapeBrickVisible(nShape, nBrickId)) {
			const int32_t nX = nPosX + oBlock.shapeBrickPosX(nShape, nBrickId);
			const int32_t nY = nPosY + oBlock.shapeBrickPosY(nShape, nBrickId);
			oCoords.add(nX, nY);
			const int32_t nCalcIdx = calcIndex(nX, nY);
			Cell& oCell = m_aBoard[nCalcIdx];
			oCell.m_oTile = oBlock.brick(nBrickId);
		}
	}

	if (bAutoOwner) {
		boabloOwnerBlockClear(*p0LevelBlock);
	}
	blockRemoveCommon(p0LevelBlock);

	if (m_p0View != nullptr) {
		m_p0View->boabloPostFreeze(oCoords);
	}

	m_oBoaBloListenerStk.callPost(itPreCalled, &BoaBloListener::boabloPostFreeze, oCoords);
	m_oBoaBloListenerStk.freePreCalled(itPreCalled);

	p0LevelBlock->m_bNestedModificationLock = false;
}
void Level::blockRemoveCommon(LevelBlock* p0LevelBlock) noexcept
{
//std::cout << "Level::blockRemoveCommon() id=" << p0LevelBlock->blockGetId() << '\n';
//std::cout << "                       size()=" << m_oAllLevelBlocks.size() << '\n';

	const int32_t nId = p0LevelBlock->blockGetId();
	auto itRemove = m_oAllLevelBlocks.find(nId);
	assert(itRemove != m_oAllLevelBlocks.end());
	m_oAllLevelBlocks.erase(itRemove);

	LevelBlock* p0NewControlled = nullptr;
	const int32_t nPlayer = p0LevelBlock->m_nPlayer;
	const int32_t nTeam = p0LevelBlock->m_nTeam;
	const int32_t nTeammate = p0LevelBlock->m_nTeammate;
//std::cout << "                       nPlayer=" << nPlayer << '\n';

	if (nPlayer >= 0) {
		const auto oPair = blockUnassignControlFromMate(p0LevelBlock, -1);
		assert(oPair.first);
		p0NewControlled = oPair.second;
	}
	if (p0LevelBlock->m_bControllable) {
		// remove from the ordered lists
		blockRemoveFromControllable(p0LevelBlock, -1);
	}
	if (nPlayer >= 0) {
		informBlockChangePlayer(p0LevelBlock, nPlayer);
		// assign next block to player
		if (p0NewControlled != nullptr) {
			blockAssignToMate(p0NewControlled, nTeam, nTeammate, true);
		}
	}

	p0LevelBlock->m_eMgmtType = LevelBlock::MGMT_TYPE_INVALID;
	p0LevelBlock->m_p0Level = nullptr;
}
std::pair<bool, LevelBlock*> Level::blockUnassignControlFromMate(LevelBlock* p0LevelBlock, int32_t nExceptIfTeam) noexcept
{
//std::cout << "Level::blockUnassignControlFromMate() id=" << p0LevelBlock->blockGetId() << '\n';
	const int32_t nTeam = p0LevelBlock->m_nTeam;
//std::cout << "                                nTeam=" << nTeam << "  nExceptIfTeam=" << nExceptIfTeam << '\n';
	if ((nTeam == -1) || (nTeam == nExceptIfTeam)) {
		return std::make_pair(false, nullptr);
	}
	const int32_t nTeammate = p0LevelBlock->m_nTeammate;

	LevelBlock* p0NewControlled = nullptr;

	// reset control
	p0LevelBlock->m_nTeam = -1;
	p0LevelBlock->m_nTeammate = -1;
	p0LevelBlock->m_nPlayer = -1;

	TeamData& oTeamData = m_aTeamData[nTeam];
	MateData& oMateData = oTeamData.m_aTeammate[nTeammate];
	assert(oMateData.m_p0Controlled == p0LevelBlock);
	oMateData.m_p0Controlled = nullptr;
	//oTeamData.m_aLastTimeTeammateInControl[nTeammate] = game().gameElapsed();
	--oTeamData.m_nTotInControl;
//std::cout << "                                oTeamData.m_nTotInControl=" << oTeamData.m_nTotInControl << '\n';

	// find another block to control
	auto itInOrderedList = p0LevelBlock->m_aItOrderedList[nTeam];
	auto itNext = itInOrderedList;
	do {
		++itNext;
		if (itNext == oTeamData.m_oOrderedControllable.end()) {
			itNext = oTeamData.m_oOrderedControllable.begin();
		}
	} while ((*itNext)->m_nPlayer >= 0); // exits when block not owned
	if (itNext != itInOrderedList) {
		p0NewControlled = *itNext;
	}
	return std::make_pair(true, p0NewControlled);
}
void Level::blockRemoveFromControllable(LevelBlock* p0LevelBlock, int32_t nNotTeam) noexcept
{
	assert(p0LevelBlock->m_bControllable);

	// remove from the ordered lists
	const int32_t nExclusiveTeam = p0LevelBlock->m_nControllerTeam;
	int32_t nFromTeam = nExclusiveTeam;
	int32_t nToTeam = nExclusiveTeam;
	if (nExclusiveTeam < 0) { // any team
		nFromTeam = 0;
		nToTeam = m_nTotLevelTeams - 1;
	}
	for (int32_t nCurTeam = nFromTeam; nCurTeam <= nToTeam; ++nCurTeam) {
		if (nCurTeam == nNotTeam) {
			continue; //----
		}
		TeamData& oTeamData = m_aTeamData[nCurTeam];
		oTeamData.m_oOrderedControllable.erase(p0LevelBlock->m_aItOrderedList[nCurTeam]);
	}
}

void Level::blockFuse(LevelBlock* p0Master, LevelBlock* p0Victim) noexcept
{
	assert(p0Master != nullptr);
	assert( !p0Master->m_bNestedModificationLock );
	assert(p0Victim != nullptr);
	assert( !p0Victim->m_bNestedModificationLock );
//std::cout << "Level(" << p0Master->blockGetId() << ")::blockFuse()  p0Victim id=" << p0Victim->blockGetId() << '\n';

	const LevelBlock::MGMT_TYPE eMasterMgmtType = p0Master->m_eMgmtType;
	const bool bMasterAutoOwner = ((eMasterMgmtType == LevelBlock::MGMT_TYPE_AUTO_OWNER) || (eMasterMgmtType == LevelBlock::MGMT_TYPE_AUTO_STRICT_OWNER));

	const LevelBlock::MGMT_TYPE eVictimMgmtType = p0Victim->m_eMgmtType;
	const bool bVictimAutoOwner = ((eVictimMgmtType == LevelBlock::MGMT_TYPE_AUTO_OWNER) || (eVictimMgmtType == LevelBlock::MGMT_TYPE_AUTO_STRICT_OWNER));

	if ((p0Master->m_bNestedModificationLock) || (p0Victim->m_bNestedModificationLock)) {
		assert(false);
		return;
	}

	p0Master->m_bNestedModificationLock = true;
	p0Victim->m_bNestedModificationLock = true;

	auto itPreCalled = m_oBlocksListenerStk.grabPreCalled();
	m_oBlocksListenerStk.callPre(itPreCalled, &BlocksListener::blockPreFuse, *p0Master, *p0Victim);

	if (m_p0View != nullptr) {
		m_p0View->blockPreFuse(*p0Master, *p0Victim);
	}

	boabloOwnerBlockClear(*p0Master);
	int32_t nPosXDelta;
	int32_t nPosYDelta;
	std::unordered_map<int32_t, int32_t> oMasterBrickIds;
	std::unordered_map<int32_t, int32_t> oVictimBrickIds;
	Block oNewBlock(p0Master->m_nShapeId, p0Master->m_oBlock
					, p0Victim->m_nShapeId, p0Victim->m_oBlock
					, p0Victim->m_nPosX - p0Master->m_nPosX, p0Victim->m_nPosY - p0Master->m_nPosY
					, nPosXDelta, nPosYDelta
					, oMasterBrickIds, oVictimBrickIds);
	p0Master->m_oBlock.swap(oNewBlock);

	p0Master->m_nShapeId = 0;
	p0Master->m_nPosX += nPosXDelta;
	p0Master->m_nPosY += nPosYDelta;

	p0Master->resetPrivateJustTileAnis(getNrTileAniAttrs());
	p0Master->resetCaches();

	if (bVictimAutoOwner) {
		boabloOwnerBlockClear(*p0Victim);
	}
	blockRemoveCommon(p0Victim);

	if (bMasterAutoOwner) {
		boabloOwnerBlockSet(*p0Master);
	}

	repositionLevelBlock(p0Master);


	if (m_p0View != nullptr) {
		m_p0View->blockPostFuse(*p0Master, *p0Victim, oMasterBrickIds, oVictimBrickIds);
	}

	m_oBlocksListenerStk.callPost(itPreCalled, &BlocksListener::blockPostFuse, *p0Master, *p0Victim, oMasterBrickIds, oVictimBrickIds);
	m_oBlocksListenerStk.freePreCalled(itPreCalled);

	p0Master->m_bNestedModificationLock = false;
	p0Victim->m_bNestedModificationLock = false;

	p0Master->onFusedWith(*p0Victim);
}

void Level::blockCycleControl(int32_t nPlayer) noexcept
{
	blockCycleControl(nPlayer, nullptr);
}
void Level::blockCycleControl(int32_t nPlayer, LevelBlock* p0Preferred) noexcept
{
	assert(nPlayer >= 0);
	int32_t nTeam;
	int32_t nTeammate;
	m_refPreferences->getTeammate(nPlayer, nTeam, nTeammate);
	assert(nTeam < m_nTotLevelTeams);
	TeamData& oTeamData = m_aTeamData[nTeam];
	assert(nTeammate < oTeamData.m_nTotTeammates);
	MateData& oMateData = oTeamData.m_aTeammate[nTeammate];
	LevelBlock* p0LevelBlock = oMateData.m_p0Controlled;
	if (p0LevelBlock == nullptr) {
		return; //--------------------------------------------------------------
	}
	blockCycleControl(p0LevelBlock, p0Preferred);
}
void Level::blockCycleControl(LevelBlock* p0LevelBlock) noexcept
{
	blockCycleControl(p0LevelBlock, nullptr);
}
void Level::blockCycleControl(LevelBlock* p0LevelBlock, LevelBlock* p0Preferred) noexcept
{
	assert(p0LevelBlock != nullptr);

	const int32_t nPlayer = p0LevelBlock->m_nPlayer;
	if (nPlayer < 0) {
		return; //--------------------------------------------------------------
	}
	const int32_t nTeam = p0LevelBlock->m_nTeam;
	const int32_t nTeammate = p0LevelBlock->m_nTeammate;

	if ((p0Preferred != nullptr) &&
				( (p0Preferred->m_nPlayer >= 0) // controlled by another player
				|| (!p0Preferred->m_bControllable) // not controllable
					// wrong team
				|| ((p0Preferred->m_nControllerTeam >= 0) && (p0Preferred->m_nControllerTeam != nTeam)) )
			) {
		p0Preferred = nullptr;
	}

	TeamData& oTeamData = m_aTeamData[nTeam];
	LevelBlock* p0NewControlled = nullptr;
	// find another block to control
	auto itInOrderedList = p0LevelBlock->m_aItOrderedList[nTeam];
	if (p0Preferred != nullptr) {
		p0NewControlled = p0Preferred;
	} else {
		auto itNext = itInOrderedList;
		do {
			++itNext;
			if (itNext == oTeamData.m_oOrderedControllable.end()) {
				itNext = oTeamData.m_oOrderedControllable.begin();
			}
		} while ((itNext != itInOrderedList) && ((*itNext)->m_nPlayer >= 0));
		if (itNext != itInOrderedList) {
			p0NewControlled = *itNext;
		}
	}
	if (p0NewControlled == nullptr) {
		return; //--------------------------------------------------------------
	}

	// reset control
	p0LevelBlock->m_nTeam = -1;
	p0LevelBlock->m_nTeammate = -1;
	p0LevelBlock->m_nPlayer = -1;

	MateData& oMateData = oTeamData.m_aTeammate[nTeammate];
	assert(oMateData.m_p0Controlled == p0LevelBlock);
	oMateData.m_p0Controlled = nullptr;
	oMateData.m_nLastTimeInControl = game().gameElapsed();
	--oTeamData.m_nTotInControl;

	informBlockChangePlayer(p0LevelBlock, nPlayer);

	oMateData.m_p0Controlled = p0NewControlled;
	++oTeamData.m_nTotInControl;
	assert(oTeamData.m_nTotInControl <= oTeamData.m_nTotTeammates);
	p0NewControlled->m_nTeam = nTeam;
	p0NewControlled->m_nTeammate = nTeammate;
	p0NewControlled->m_nPlayer = nPlayer;

	informBlockChangePlayer(p0NewControlled, -1);
}
void Level::blockSetControllable(LevelBlock* p0LevelBlock, bool bControllable, int32_t nControllerTeam) noexcept
{
//std::cout << "Level::blockSetControllable bControllable=" << bControllable << "  nControllerTeam=" << nControllerTeam << '\n';
	assert(p0LevelBlock != nullptr);
	if (bControllable) {
		assert(nControllerTeam >= -1);
	}
	const bool bWasControllable = p0LevelBlock->m_bControllable;
	const int32_t nOldControllerTeam = p0LevelBlock->m_nControllerTeam;
	const bool bControllabilityChanged = (bControllable != bWasControllable);
	if (bControllable) {
		assert((nControllerTeam >= -1) && (nControllerTeam < m_nTotLevelTeams));
		if (! bControllabilityChanged) {
			if (nOldControllerTeam == nControllerTeam) {
				return; //------------------------------------------------------
			}
			if (nControllerTeam == -1) {
				// enlarging controllability, the level block doesn't need to
				// drop current controller
				// add level block to all teams except current controller's
				blockAddToControllable(p0LevelBlock, nControllerTeam, nOldControllerTeam);
				p0LevelBlock->m_bControllable = true;
				p0LevelBlock->m_nControllerTeam = nControllerTeam;
			} else if (nOldControllerTeam == -1) {
				// restricting controllability, if the team of the controller player
				// isn't nControllerTeam drop current controller
				// remove from controllable of other teams
				const int32_t nOldTeam = p0LevelBlock->m_nTeam;
				const int32_t nOldTeammate = p0LevelBlock->m_nTeammate;
				const int32_t nOldPlayer = p0LevelBlock->m_nPlayer;
				LevelBlock* p0NewControlled = nullptr;
				const auto oPair = blockUnassignControlFromMate(p0LevelBlock, nControllerTeam);
				if (oPair.first) {
					p0NewControlled = oPair.second;
					informBlockChangePlayer(p0LevelBlock, nOldPlayer);
				}
				blockRemoveFromControllable(p0LevelBlock, nOldTeam);
				p0LevelBlock->m_bControllable = true;
				p0LevelBlock->m_nControllerTeam = nControllerTeam;
				if (p0NewControlled != nullptr) {
					// assign next block to player
					blockAssignToMate(p0NewControlled, nOldTeam, nOldTeammate, true);
				}
			} else {
				// allowed team changed: drop current controller
				// remove from team of old team
				// add to team of new team
				const int32_t nOldTeam = p0LevelBlock->m_nTeam;
				const int32_t nOldTeammate = p0LevelBlock->m_nTeammate;
				const int32_t nOldPlayer = p0LevelBlock->m_nPlayer;
				LevelBlock* p0NewControlled = nullptr;
				const auto oPair = blockUnassignControlFromMate(p0LevelBlock, nControllerTeam);
				if (oPair.first) {
					p0NewControlled = oPair.second;
					informBlockChangePlayer(p0LevelBlock, nOldPlayer);
				}
				blockRemoveFromControllable(p0LevelBlock, -1);
				blockAddToControllable(p0LevelBlock, nControllerTeam, -1);
				p0LevelBlock->m_bControllable = true;
				p0LevelBlock->m_nControllerTeam = nControllerTeam;
				if (p0NewControlled != nullptr) {
					// assign next block to the defrauded player
					blockAssignToMate(p0NewControlled, nOldTeam, nOldTeammate, true);
				}
			}
		} else {
			assert(! bWasControllable);
//std::cout << " !bWasControllable  to  bControllable"  << '\n';
			blockAddToControllable(p0LevelBlock, nControllerTeam, -1);
			p0LevelBlock->m_bControllable = true;
			p0LevelBlock->m_nControllerTeam = nControllerTeam;
			blockAssignControlToLongestWaitingMate(p0LevelBlock, nControllerTeam, -1);
		}
	} else {
		if (! bControllabilityChanged) {
			return; //----------------------------------------------------------
		}
		// from controllable to not controllable
		const int32_t nOldTeam = p0LevelBlock->m_nTeam;
		const int32_t nOldTeammate = p0LevelBlock->m_nTeammate;
		const int32_t nOldPlayer = p0LevelBlock->m_nPlayer;
		LevelBlock* p0NewControlled = nullptr;
		const auto oPair = blockUnassignControlFromMate(p0LevelBlock, -1);
		if (oPair.first) {
			p0NewControlled = oPair.second;
			informBlockChangePlayer(p0LevelBlock, nOldPlayer);
		}
		blockRemoveFromControllable(p0LevelBlock, -1);
		p0LevelBlock->m_bControllable = false;
		p0LevelBlock->m_nControllerTeam = -1;
		if (p0NewControlled != nullptr) {
			// assign next block to the defrauded player
			blockAssignToMate(p0NewControlled, nOldTeam, nOldTeammate, true);
		}
	}
	p0LevelBlock->onControllabilityChanged();
}
LevelBlock* Level::blocksGet(int32_t nId) noexcept
{
	auto itF = m_oAllLevelBlocks.find(nId);
	if (itF == m_oAllLevelBlocks.end()) {
		return nullptr;
	} else {
		return itF->second;
	}
}
std::vector<int32_t> Level::blocksGetAllIds() noexcept
{
	std::vector<int32_t> aSet;
	for (auto& it : m_oAllLevelBlocks) {
		aSet.push_back(it.first);
	}
	return aSet;
}
std::vector<LevelBlock*> Level::blocksGetAll() noexcept
{
	std::vector<LevelBlock*> aSet;
	for (auto& it : m_oAllLevelBlocks) {
		aSet.push_back(it.second);
	}
	return aSet;
}

bool Level::coordsCanPlaceOnBoard(const Coords& oCoords, bool bStrict) const noexcept
{
	for (Coords::const_iterator it = oCoords.begin(); it != oCoords.end(); it.next()) {
		const int32_t nBoardX = it.x();
		const int32_t nBoardY = it.y();
		if ((nBoardX < 0) || (nBoardX >= m_nW)
				|| (nBoardY >= m_nH) || (nBoardY < 0)) {
			return false;
		}
		if (bStrict && !boardGetTile(nBoardX, nBoardY).isEmpty()) {
			return false;
		}
		LevelBlock* p0BoardBlock = boardGetOwner(nBoardX, nBoardY);
		if (p0BoardBlock != nullptr) {
			return false;
		}
	}
	return true;
}
bool Level::blockCanPlaceOnBoard(const LevelBlock& oLevelBlock, bool bStrict) const noexcept
{
//std::cout << "Level::blockCanPlaceOnBoard";
	const Block& oBlock = oLevelBlock.m_oBlock;
	const int32_t nShape = oLevelBlock.m_nShapeId;
	const int32_t nPosX = oLevelBlock.m_nPosX;
	const int32_t nPosY = oLevelBlock.m_nPosY;
//std::cout << " nShape=" << nShape << " nPosX=" << nPosX << " nPosY=" << nPosY;
	auto aBrickId = oBlock.brickIds();
	for (auto& nBrickId : aBrickId) {
		const bool bVisible = oBlock.shapeBrickVisible(nShape, nBrickId);
		if (bVisible) {
			const int32_t nBoardX = nPosX + oBlock.shapeBrickPosX(nShape, nBrickId);
			const int32_t nBoardY = nPosY + oBlock.shapeBrickPosY(nShape, nBrickId);
//std::cout << " nBoardX=" << nBoardX << " nBoardY=" << nBoardY << " nBrickId=" << nBrickId;
			if ((nBoardX < 0) || (nBoardX >= m_nW)
					|| (nBoardY >= m_nH) || (nBoardY < 0)) {
//std::cout << " Brick outside board" << '\n';
				return false;
			}
			if (bStrict && !boardGetTile(nBoardX, nBoardY).isEmpty()) {
//std::cout << " Strict: not empty tile" << '\n';
				return false;
			}
			LevelBlock* p0BoardBlock = boardGetOwner(nBoardX, nBoardY);
			if ((p0BoardBlock != nullptr) && (&oLevelBlock != p0BoardBlock)) {
//std::cout << " Wrong Owner" << '\n';
				return false;
			}
		}
	}
//std::cout << " OK" << '\n';
	return true;
}
bool Level::blockIntersectsArea(const LevelBlock& oLevelBlock, int32_t nX, int32_t nY, int32_t nW, int32_t nH) const noexcept
{
	const Block& oBlock = oLevelBlock.m_oBlock;
	const int32_t nShape = oLevelBlock.m_nShapeId;
	const int32_t nPosX = oLevelBlock.m_nPosX;
	const int32_t nPosY = oLevelBlock.m_nPosY;
	auto aBrickId = oBlock.brickIds();
	for (auto& nBrickId : aBrickId) {
		const bool bVisible = oBlock.shapeBrickVisible(nShape, nBrickId);
		if (bVisible) {
			const int32_t nBoardX = nPosX + oBlock.shapeBrickPosX(nShape, nBrickId);
			const int32_t nBoardY = nPosY + oBlock.shapeBrickPosY(nShape, nBrickId);
			if ((nBoardX >= nX) && (nBoardX < nX + nW)
					&& (nBoardY >= nY) && (nBoardY < nY+nH)) {
				return true;
			}
		}
	}
	return false;
}
bool Level::blockMoveIsWithinArea(const LevelBlock& oLevelBlock, int32_t nMoveX, int32_t nMoveY
									, int32_t nX, int32_t nY, int32_t nW, int32_t nH) const noexcept
{
	const Block& oBlock = oLevelBlock.m_oBlock;
	const int32_t nShape = oLevelBlock.m_nShapeId;
	const int32_t nPosX = oLevelBlock.m_nPosX + nMoveX;
	const int32_t nPosY = oLevelBlock.m_nPosY + nMoveY;
	auto aBrickId = oBlock.brickIds();
	for (auto& nBrickId : aBrickId) {
		const bool bVisible = oBlock.shapeBrickVisible(nShape, nBrickId);
		if (bVisible) {
			const int32_t nBoardX = nPosX + oBlock.shapeBrickPosX(nShape, nBrickId);
			const int32_t nBoardY = nPosY + oBlock.shapeBrickPosY(nShape, nBrickId);
			if (!((nBoardX >= nX) && (nBoardX < nX + nW)
					&& (nBoardY >= nY) && (nBoardY < nY+nH))) {
				return false;
			}
		}
	}
	return true;
}

void Level::repositionLevelBlock(LevelBlock* p0LevelBlock) noexcept
{
	const int32_t nPlayer = p0LevelBlock->m_nPlayer;
	if (nPlayer >= 0) {
		#ifndef NDEBUG
		const int32_t nTeam = p0LevelBlock->m_nTeam;
		#endif //NDEBUG
		const int32_t nExclusiveTeam = p0LevelBlock->m_nControllerTeam;
		int32_t nFromTeam = nExclusiveTeam;
		int32_t nToTeam = nExclusiveTeam;
		if (nExclusiveTeam < 0) { // any team
			nFromTeam = 0;
			nToTeam = m_nTotLevelTeams - 1;
		} else {
			assert(nTeam == nExclusiveTeam);
		}
		for (int32_t nCurTeam = nFromTeam; nCurTeam <= nToTeam; ++nCurTeam) {
			TeamData& oTeamData = m_aTeamData[nCurTeam];
			std::list<LevelBlock*>& oOrderedList = oTeamData.m_oOrderedControllable;
			std::list<LevelBlock*>::iterator& itOrderedList = p0LevelBlock->m_aItOrderedList[nCurTeam];
			// reorder
			auto itCur = itOrderedList;
			while (itCur != oOrderedList.begin()) {
				--itCur;
				if (!orderLevelBlocks(*itOrderedList, *itCur)) {
					++itCur;
					break; // while
				}
			}
			if (itCur != itOrderedList) {
				oOrderedList.erase(itOrderedList);
				itOrderedList = oOrderedList.insert(itCur, p0LevelBlock);
				continue; // for
			}
			++itCur;
			while (itCur != oOrderedList.end()) {
				if (orderLevelBlocks(*itOrderedList, *itCur)) {
					break; // while
				}
				++itCur;
			}
			--itCur;
			if (itCur != itOrderedList) {
				++itCur;
				oOrderedList.erase(itOrderedList);
				itOrderedList = oOrderedList.insert(itCur, p0LevelBlock);
			}
		}
	}
}
void Level::blocksAddListener(BlocksListener* p0Listener) noexcept
{
//std::cout << "Level::blocksAddListener()" << '\n';
	assert(p0Listener != nullptr);
	m_oBlocksListenerStk.addListener(p0Listener);
	m_oBoaBloListenerStk.addListener(p0Listener);
}
void Level::blocksRemoveListener(BlocksListener* p0Listener) noexcept
{
//std::cout << "Level::blocksRemoveListener()" << '\n';
	assert(p0Listener != nullptr);
	m_oBlocksListenerStk.removeListener(p0Listener);
	m_oBoaBloListenerStk.removeListener(p0Listener);
}
void Level::blocksAddBricksIdListener(BlocksBricksIdListener* p0Listener) noexcept
{
	assert(p0Listener != nullptr);
	m_oBlocksBricksIdListenerStk.addListener(p0Listener);
	m_oBlocksListenerStk.addListener(p0Listener);
	m_oBoaBloListenerStk.addListener(p0Listener);
}
void Level::blocksRemoveBricksIdListener(BlocksBricksIdListener* p0Listener) noexcept
{
	assert(p0Listener != nullptr);
	m_oBlocksBricksIdListenerStk.removeListener(p0Listener);
	m_oBlocksListenerStk.removeListener(p0Listener);
	m_oBoaBloListenerStk.removeListener(p0Listener);
}
void Level::blocksAddPlayerChangeListener(BlocksPlayerChangeListener* p0Listener) noexcept
{
	assert(std::find(m_aBlocksPlayerChangeListener.begin(), m_aBlocksPlayerChangeListener.end(), p0Listener) == m_aBlocksPlayerChangeListener.end());
	m_aBlocksPlayerChangeListener.push_back(p0Listener);
}
void Level::blocksRemovePlayerChangeListener(BlocksPlayerChangeListener* p0Listener) noexcept
{
	auto itFind = std::find(m_aBlocksPlayerChangeListener.begin(), m_aBlocksPlayerChangeListener.end(), p0Listener);
	assert(itFind != m_aBlocksPlayerChangeListener.end());
	m_aBlocksPlayerChangeListener.erase(itFind);
}
void Level::informBlockChangePlayer(LevelBlock* p0LevelBlock, int32_t nOldPlayer) noexcept
{
	if (p0LevelBlock->m_nPlayer == nOldPlayer) {
		return;
	}
	if (m_bBlockDisallowNestedPlayerChanges) {
		assert(false);
		return;
	}
	m_bBlockDisallowNestedPlayerChanges = true;
	p0LevelBlock->onPlayerChanged();
	for (BlocksPlayerChangeListener*& p0Listener : m_aBlocksPlayerChangeListener) {
		p0Listener->blockChangedPlayer(*p0LevelBlock, nOldPlayer);
	}
	m_bBlockDisallowNestedPlayerChanges = false;
}

shared_ptr<GameWidget> Level::widgetsGet(const std::string& sWidgetName, int32_t nLevelTeam, int32_t nMate) noexcept
{
	const int32_t nTeam = (m_p0Game->isAllTeamsInOneLevel() ? nLevelTeam : m_nLevelInGame);
	return m_p0Game->getLayout()->getWidgetNamed(sWidgetName, nTeam, nMate);
}

void Level::animationAdd(const shared_ptr<LevelAnimation>& refLevelAnimation, LevelAnimation::REFSYS eRefSys
							, double fDelayMillisec) noexcept
{
//std::cout << "Level::animationAdd  id=" << refLevelAnimation->getId() << '\n';
	animationPrivAddCommon(refLevelAnimation, eRefSys, fDelayMillisec);
}
void Level::animationPrivAddCommon(const shared_ptr<LevelAnimation>& refLevelAnimation, LevelAnimation::REFSYS eRefSys
								, double fDelayMillisec) noexcept
{
//std::cout << "Level::animationPrivAddCommon  id=" << refLevelAnimation->getId() << '\n';
	assert(refLevelAnimation);
	assert((eRefSys == LevelAnimation::REFSYS_BOARD) || (eRefSys == LevelAnimation::REFSYS_SHOW)
			|| (eRefSys >= LevelAnimation::REFSYS_SUBSHOW));
	assert(fDelayMillisec >= 0.0);
	const int32_t nLevAniId = refLevelAnimation->getId();
	refLevelAnimation->m_eRefSys = eRefSys;
	if (fDelayMillisec > 0.0) {
		// might be delayed, postpone activation to handlePostTimer()
		refLevelAnimation->m_fStartedMillisec = -fDelayMillisec;
		m_aDelayedLevAniIds.push_back(nLevAniId);
		m_aDelayedLevAnis.push_back(refLevelAnimation);
		return; // -----------
	}
	animationPrivAdd(nLevAniId, refLevelAnimation, 0.0);
}
void Level::animationAddScrolled(const shared_ptr<LevelAnimation>& refLevelAnimation, double fDelayMillisec) noexcept
{
	assert(refLevelAnimation);
	assert(fDelayMillisec >= 0.0);
	const int32_t nLevAniId = refLevelAnimation->getId();
	refLevelAnimation->m_eRefSys = LevelAnimation::REFSYS_BOARD;
	if (fDelayMillisec > 0.0) {
		// might be delayed, postpone activation to handlePostTimer()
		refLevelAnimation->m_fStartedMillisec = -fDelayMillisec;
		m_aDelayedScrAniIds.push_back(nLevAniId);
		m_aDelayedScrAnis.push_back(refLevelAnimation);
		return; // -----------
	}
	animationPrivAddScrolled(nLevAniId, refLevelAnimation, 0.0);
}
void Level::animationPrivAdd(int32_t nLevAniId, const shared_ptr<LevelAnimation>& refLevelAnimation, double fDelay) noexcept
{
	const double fGameInterval = game().gameInterval();
	assert((fDelay >= 0) && (fDelay < fGameInterval));
//std::cout << "Level::animationPrivAdd  id=" << nLevAniId << '\n';
	#ifndef NDEBUG
	auto oRes =
	#endif //NDEBUG
	m_oActiveLevAnis.insert(std::make_pair(nLevAniId, refLevelAnimation));
	assert(oRes.second); // Make sure not called twice by mistake
	const bool bInGameTick = game().isInGameTick();
	const double fGameMillisec = game().gameElapsedMillisec() - (bInGameTick ? 0.0 : fGameInterval);
	refLevelAnimation->m_fStartedMillisec = fGameMillisec + fDelay;
	refLevelAnimation->setLevel(this);
	//
	if (m_p0View != nullptr) {
		m_p0View->animationCreate(refLevelAnimation);
	}
}
void Level::animationPrivAddScrolled(int32_t nLevAniId, const shared_ptr<LevelAnimation>& refLevelAnimation, double fDelay) noexcept
{
	assert(fDelay >= 0);
//std::cout << "Level::animationPrivAddScrolled  id=" << nLevAniId << '\n';
	#ifndef NDEBUG
	auto oRes =
	#endif //NDEBUG
	m_oActiveScrolledAnis.insert(std::make_pair(nLevAniId, refLevelAnimation));
	assert(oRes.second);
	animationPrivAdd(nLevAniId, refLevelAnimation, fDelay);
}
void Level::animationStartDelayed() noexcept
{
	// Now that interval is settled can decide whether to start delayed
	const double fGameInterval = game().gameInterval();
	for (auto itDLA = m_aDelayedLevAnis.begin(); itDLA != m_aDelayedLevAnis.end(); ) {
		shared_ptr<LevelAnimation>& refLevelAnimation = *itDLA;
		double fDelayMillisec = -refLevelAnimation->m_fStartedMillisec;
		if (fDelayMillisec < fGameInterval) {
			// will start in this game interval
			// copy before erasing: this is done to have consistent state before calling animationPrivAdd
			shared_ptr<LevelAnimation> refTemp = std::move(refLevelAnimation);
			refLevelAnimation = std::move(m_aDelayedLevAnis.back());
			m_aDelayedLevAnis.pop_back();
			const auto nIdx = std::distance(m_aDelayedLevAnis.begin(), itDLA);
			m_aDelayedLevAniIds[nIdx] = std::move(m_aDelayedLevAniIds.back());
			m_aDelayedLevAniIds.pop_back();
			//
			const int32_t nLevAniId = refTemp->getId();
			animationPrivAdd(nLevAniId, refTemp, fDelayMillisec);
		} else {
			// wait another game interval or tick
			if (-refLevelAnimation->m_fStartedMillisec < LevelAnimation::s_fDurationInfinity) { //TODO what is this?
				refLevelAnimation->m_fStartedMillisec += fGameInterval; // = -((-m_fStartedMillisec) - fGameInterval)
			}
			++itDLA;
		}
	}
	for (auto itDSA = m_aDelayedScrAnis.begin(); itDSA != m_aDelayedScrAnis.end(); ) {
		shared_ptr<LevelAnimation>& refLevelAnimation = *itDSA;
		double fDelayMillisec = -refLevelAnimation->m_fStartedMillisec;
		if (fDelayMillisec < fGameInterval) {
			// will start in this game interval
			// copy before erasing: this is done to have consistent state before calling animationPrivAdd
			shared_ptr<LevelAnimation> refTemp = std::move(refLevelAnimation);
			refLevelAnimation = std::move(m_aDelayedScrAnis.back());
			m_aDelayedScrAnis.pop_back();
			const auto nIdx = std::distance(m_aDelayedScrAnis.begin(), itDSA);
			m_aDelayedScrAniIds[nIdx] = std::move(m_aDelayedScrAniIds.back());
			m_aDelayedScrAniIds.pop_back();
			//
			const int32_t nLevAniId = refTemp->getId();
			animationPrivAddScrolled(nLevAniId, refTemp, fDelayMillisec);
		} else {
			// wait another game interval or tick
			if (-refLevelAnimation->m_fStartedMillisec < LevelAnimation::s_fDurationInfinity) { //TODO what is this?
				refLevelAnimation->m_fStartedMillisec += fGameInterval; // = -((-m_fStartedMillisec) - fGameInterval)
			}
			++itDSA;
		}
	}
}
void Level::animationRemove(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refLevelAnimation);
	const int32_t nLevAniId = refLevelAnimation->getId();
//std::cout << "Level::animationRemove  id=" << nLevAniId << '\n';
	if (refLevelAnimation->m_p0Level == nullptr) {
		// either no longer active or delayed
		if (refLevelAnimation->m_fStartedMillisec < 0.0) { // || (refLevelAnimation->m_fStartedTicks < 0.0) {
			// delayed
			const auto itDLA = std::find(m_aDelayedLevAniIds.begin(), m_aDelayedLevAniIds.end(), nLevAniId);
			if (itDLA != m_aDelayedLevAniIds.end()) {
				*itDLA = std::move(m_aDelayedLevAniIds.back());
				m_aDelayedLevAniIds.pop_back();
				const auto nIdx = std::distance(m_aDelayedLevAniIds.begin(), itDLA);
				m_aDelayedLevAnis[nIdx] = std::move(m_aDelayedLevAnis.back());
				m_aDelayedLevAnis.pop_back();
			} else {
				const auto itDSA = std::find(m_aDelayedScrAniIds.begin(), m_aDelayedScrAniIds.end(), nLevAniId);
				assert(itDSA != m_aDelayedScrAniIds.end());
				*itDSA = std::move(m_aDelayedScrAniIds.back());
				m_aDelayedScrAniIds.pop_back();
				const auto nIdx = std::distance(m_aDelayedScrAniIds.begin(), itDSA);
				m_aDelayedScrAnis[nIdx] = std::move(m_aDelayedScrAnis.back());
				m_aDelayedScrAnis.pop_back();
			}
			refLevelAnimation->m_fStartedMillisec = -LevelAnimation::s_fDurationInfinity;
		}
		return; // -------------------------------------------------------------
	}
	//
	refLevelAnimation->setLevel(nullptr);
	auto itLA = m_oActiveLevAnis.find(nLevAniId);
	assert(itLA != m_oActiveLevAnis.end());
	m_oActiveLevAnis.erase(itLA);
	auto itSA = m_oActiveScrolledAnis.find(nLevAniId);
	if (itSA != m_oActiveScrolledAnis.end()) {
		m_oActiveScrolledAnis.erase(itSA);
	}
	//
	if (m_p0View != nullptr) {
		m_p0View->animationRemove(refLevelAnimation);
	}
}

void Level::animationCreateShowTextCommon(LevelAnimation::REFSYS eRefSys, int32_t nNamedIdx, std::vector<std::string>&& aLines
											, double fTextSize
											, double fX, double fY, double fW, double fH, int32_t nZ, int32_t nDuration) noexcept
{
	shared_ptr<TextAnimation> refShowText;
	TextAnimation::Init oInit;
	oInit.m_nAnimationNamedIdx = nNamedIdx;
	oInit.m_fDuration = nDuration;
	oInit.m_oPos = {fX, fY};
	oInit.m_oSize = {fW, fH};
	oInit.m_nZ = nZ;
	oInit.m_aLines = std::move(aLines);
	oInit.m_fFontHeight = fTextSize;
	s_oTextAnimationRecycler.create(refShowText, std::move(oInit));
//std::cout << "Level::animationCreateShowTextCommon  id=" << refShowText->getId() << "   nNamedIdx=" << nNamedIdx << '\n';

	animationPrivAddCommon(refShowText, eRefSys, 0.0);
}

Variable& Level::variable(int32_t nId, int32_t nLevelTeam, int32_t nMate) noexcept
{
	if (nLevelTeam == -1) {
		assert(nMate == -1);
		return game().variable(nId);
	} else {
		assert((nLevelTeam >= 0) && (nLevelTeam < m_nTotLevelTeams));
		TeamData& oTeamData = m_aTeamData[nLevelTeam];
		if (nMate == -1) {
//std::cout << "Level::variable  id=" << nId << "   oTeamData.m_oVariables.getTotVariables()=" << oTeamData.m_oVariables.getTotVariables() << '\n';
			return oTeamData.m_oVariables.getVariable(nId);
		} else {
			assert((nMate >= 0) && (nMate < oTeamData.m_nTotTeammates));
			MateData& oMateData = oTeamData.m_aTeammate[nMate];
			return oMateData.m_oVariables.getVariable(nId);
		}
	}
}
const Variable& Level::variable(int32_t nId, int32_t nLevelTeam, int32_t nMate) const  noexcept
{
	auto p0NotConstThis = const_cast<Level*>(this);
	return p0NotConstThis->variable(nId, nLevelTeam, nMate);
}
void Level::variablesInit(const NamedObjIndex<Variable::VariableType>& oTeamVariableTypes
						, const NamedObjIndex<Variable::VariableType>& oMateVariableTypes) noexcept
{
	// Since variables are actually managed by Game
	// and its reInit() is called after the reInit() of the levels,
	// variables are initialized here rather than in reInit()
	for (int32_t nCurTeam = 0; nCurTeam < m_nTotLevelTeams; ++nCurTeam) {
		TeamData& oTeamData = m_aTeamData[nCurTeam];
		oTeamData.m_oVariables.init(oTeamVariableTypes, m_p0Game);
		for (int32_t nCurMate = 0; nCurMate < oTeamData.m_nTotTeammates; ++nCurMate) {
			MateData& oMateData = oTeamData.m_aTeammate[nCurMate];
			oMateData.m_oVariables.init(oMateVariableTypes, m_p0Game);
		}
	}
}
void Level::variablesInhibit(int32_t nLevelTeam, int32_t nMate) noexcept
{
	assert((nLevelTeam >= 0) || (nMate < 0));
	for (int32_t nCurTeam = 0; nCurTeam < m_nTotLevelTeams; ++nCurTeam) {
		if (nLevelTeam >= 0) {
			if (nCurTeam != nLevelTeam) {
				continue; // for (nCurTeam)
			}
		}
		TeamData& oTeamData = m_aTeamData[nCurTeam];
		oTeamData.m_oVariables.inhibit();
		for (int32_t nCurMate = 0; nCurMate < oTeamData.m_nTotTeammates; ++nCurMate) {
			if (nMate >= 0) {
				if (nCurMate != nMate) {
					continue; // for (nCurMate)
				}
			}
			MateData& oMateData = oTeamData.m_aTeammate[nCurMate];
			oMateData.m_oVariables.inhibit();
		}
	}
}

// TODO Maybe possible to redefine these strings (passed to init)
const std::string Level::s_sGameOverPlayerOut = "You're out!";
const std::string Level::s_sGameOverWinOne = "GAME OVER!";
const std::string Level::s_sGameOverOneLevelWinOne = "WINNER";
const std::string Level::s_sGameOverOneLevelWinMany = "WINNERS";
const std::string Level::s_sGameOverManyLevelsWin = "WINNER!";
const std::string Level::s_sGameOverLoseOne = "GAME OVER!";
const std::string Level::s_sGameOverLoseMany = "YOU LOST!";
const std::string Level::s_sGameOverTechnical = "ERROR!";

bool Level::gameStatusPlayerOut(int32_t nLevelTeam, int32_t nMate, bool bForceOtherCompleted, bool bCreateTexts) noexcept
{
//std::cout << "gameStatusPlayerOut nLevelTeam=" << nLevelTeam << " nMate=" << nMate << '\n';
	assert((nLevelTeam >= 0) && (nLevelTeam < m_nTotLevelTeams));
	TeamData& oTeamData = m_aTeamData[nLevelTeam];
	if (oTeamData.m_oVariables.getVariable(m_p0Game->getTeamVarIdFinishedTime()).get() >= 0) {
		// The team already won or lost, ignore
		return false; //--------------------------------------------------------
	}
	const int32_t nTotTeammates = static_cast<int32_t>(oTeamData.m_aTeammate.size());
	assert((nMate >= 0) && (nMate < nTotTeammates));
	MateData& oMateData = m_aTeamData[nLevelTeam].m_aTeammate[nMate];
	auto& oVarPlayerFinishedTime = oMateData.m_oVariables.getVariable(m_p0Game->getPlayerVarIdFinishedTime());
	if (oVarPlayerFinishedTime.get() >= 0) {
		return false; //--------------------------------------------------------
	}
	oVarPlayerFinishedTime.forceSet(m_p0Game->gameElapsedMillisec());
	oMateData.m_oVariables.getVariable(m_p0Game->getPlayerVarIdStatus()).forceSet(GameProxy::VAR_VALUE_PLAYER_STATUS_OUT);
	variablesInhibit(nLevelTeam, nMate);
	// see if there are mates left playing
	int32_t nTotPlayersOut = 0;
	for (int32_t nCurMate = 0; nCurMate < nTotTeammates; ++nCurMate) {
		if (oTeamData.m_aTeammate[nCurMate].m_oVariables.getVariable(m_p0Game->getPlayerVarIdFinishedTime()).get() >= 0) {
			++nTotPlayersOut;
		}
	}
	if (nTotPlayersOut == nTotTeammates) {
		gameStatusFailed(nLevelTeam, bForceOtherCompleted, bCreateTexts);
	} else if (bCreateTexts && m_bSubshowMode) {
		std::vector<std::string> aText{};
		aText.push_back(s_sGameOverPlayerOut);
		const int32_t nSubShowW = oMateData.m_oSubshow.getW();
		const int32_t nSubShowH = oMateData.m_oSubshow.getH();
		const double fSubShowX = 0.0;
		const double fSubShowY = 0.0;
		animationCreateShowTextCommon(static_cast<LevelAnimation::REFSYS>(LevelAnimation::REFSYS_SUBSHOW + oMateData.m_nLevelPlayer)
									, -1, std::move(aText), 10000.0, fSubShowX, fSubShowY, nSubShowW, nSubShowH
									, s_nZObjectZGameOver, 4000);
	}
	return true;
}
bool Level::gameStatusTeamIsPlaying(int32_t nLevelTeam) const noexcept
{
	assert((nLevelTeam >= 0) && (nLevelTeam < m_nTotLevelTeams));
	const TeamData& oTeamData = m_aTeamData[nLevelTeam];
	auto& oVarTeamFinishedTime = oTeamData.m_oVariables.getVariable(m_p0Game->getTeamVarIdFinishedTime());
	return (oVarTeamFinishedTime.get() < 0);
}
void Level::gameStatusSetTeamStatus(int32_t nLevelTeam, bool bCompleted) noexcept
{
	assert((nLevelTeam >= 0) && (nLevelTeam < m_nTotLevelTeams));
	TeamData& oTeamData = m_aTeamData[nLevelTeam];
	auto& oVarTeamFinishedTime = oTeamData.m_oVariables.getVariable(m_p0Game->getTeamVarIdFinishedTime());
	const int32_t nElapsedMillisec = m_p0Game->gameElapsedMillisec();
	oVarTeamFinishedTime.forceSet(nElapsedMillisec);

	auto& oVarTeamStatus = oTeamData.m_oVariables.getVariable(m_p0Game->getTeamVarIdStatus());
	oVarTeamStatus.forceSet(bCompleted ? GameProxy::VAR_VALUE_TEAM_STATUS_COMPLETED : GameProxy::VAR_VALUE_TEAM_STATUS_FAILED);

	variablesInhibit(nLevelTeam, -1); // also inhibits the mates!
	const int32_t nTotTeammates = oTeamData.m_nTotTeammates;
	for (int32_t nMate = 0; nMate < nTotTeammates; ++nMate) {
		MateData& oMateData = oTeamData.m_aTeammate[nMate];
		auto& oVarPlayerStatus = oMateData.m_oVariables.getVariable(m_p0Game->getPlayerVarIdStatus());
		oVarPlayerStatus.forceSet(bCompleted ? GameProxy::VAR_VALUE_PLAYER_STATUS_COMPLETED : GameProxy::VAR_VALUE_PLAYER_STATUS_FAILED);

		auto& oVarPlayerFinishedTime = oMateData.m_oVariables.getVariable(m_p0Game->getPlayerVarIdFinishedTime());
		oVarPlayerFinishedTime.forceSet(nElapsedMillisec);
//std::cout << "gameStatusSetTeamStatus nLevelTeam=" << nLevelTeam << "  nMate=" << nMate << "  nElapsedMillisec=" << nElapsedMillisec << '\n';
	}
}
void Level::gameStatusSetTeamRank(int32_t nLevelTeam, int32_t nRank) noexcept
{
	assert((nLevelTeam >= 0) && (nLevelTeam < m_nTotLevelTeams));
	TeamData& oTeamData = m_aTeamData[nLevelTeam];

	auto& oVarTeamRank = oTeamData.m_oVariables.getVariable(m_p0Game->getTeamVarIdRank());
	oVarTeamRank.forceSet(nRank);
}
bool Level::gameStatusCompleted(int32_t nLevelTeam, bool bForceOthersFailed, bool bCreateTexts) noexcept
{
	assert((nLevelTeam >= 0) && (nLevelTeam < m_nTotLevelTeams));
	TeamData& oTeamData = m_aTeamData[nLevelTeam];
	auto& oVarTeamFinishedTime = oTeamData.m_oVariables.getVariable(m_p0Game->getTeamVarIdFinishedTime());
	if (oVarTeamFinishedTime.get() >= 0) {
		// The team already finished, ignore
		return false; //--------------------------------------------------------
	}
	//
	gameStatusSetTeamStatus(nLevelTeam, true);
	//
	assert(m_nTotGameEndedTeams < m_nTotLevelTeams);
	++m_nTotGameEndedTeams;

	// possibly force teams in other levels to lose
	const int32_t nRank = m_p0Game->gameStatusCompleted(m_nLevelInGame, nLevelTeam, bForceOthersFailed, bCreateTexts);
	assert(nRank >= 1);

	gameStatusSetTeamRank(nLevelTeam, nRank);

	if ((!bCreateTexts) || ((!m_bSubshowMode) && (nRank != 1))) {
		// In show mode only the game winner creates texts
		return true; //---------------------------------------------------------
	}
	//
	if (nRank != 1) {
		// create texts for winner!
		gameStatusLoserTexts(oTeamData);
		return true; //---------------------------------------------------------
	}
	gameStatusWinnerTexts(oTeamData);
	return true;
}
void Level::gameStatusWinnerTexts(TeamData& oTeamData) noexcept
{
	std::vector<std::string> aText;

	const int32_t nTotTeammates = oTeamData.m_nTotTeammates;
	const int32_t nTotLevels = (m_bAllTeamsInOneLevel ? 1 : m_refPreferences->getTotTeams());
	if ((nTotLevels == 1) && (m_nTotLevelTeams == 1)) {
		aText.push_back(s_sGameOverWinOne);
	} else if (m_bAllTeamsInOneLevel && !m_bSubshowMode) {
		assert(nTotLevels == 1);
		assert(m_nTotLevelTeams > 1);
		if (nTotTeammates == 1) {
			aText.push_back(s_sGameOverOneLevelWinOne);
		} else {
			aText.push_back(s_sGameOverOneLevelWinMany);
		}
		for (int32_t nMate = 0; nMate < nTotTeammates; ++nMate) {
			const std::string& sName = oTeamData.m_aTeammate[nMate].m_refPrefPlayer->getName();
			aText.push_back(sName);
		}
	} else {
		aText.push_back(s_sGameOverManyLevelsWin);
	}
	if (m_bSubshowMode) {
		for (int32_t nMate = 0; nMate < nTotTeammates; ++nMate) {
			auto aTextCopy = aText;
			MateData& oMateData = oTeamData.m_aTeammate[nMate];
			const int32_t nSubShowW = oMateData.m_oSubshow.getW();
			const int32_t nSubShowH = oMateData.m_oSubshow.getH();
			const double fSubShowX = 0.0;
			const double fSubShowY = 0.0;
			animationCreateShowTextCommon(static_cast<LevelAnimation::REFSYS>(LevelAnimation::REFSYS_SUBSHOW + oMateData.m_nLevelPlayer)
										, -1, std::move(aTextCopy), 10001.0, fSubShowX, fSubShowY, nSubShowW, nSubShowH
										, s_nZObjectZGameOver, -1);
		}
	} else {
		const int32_t nShowW = m_oShow.getW();
		const int32_t nShowH = m_oShow.getH();
		const double fShowX = 0.0;
		const double fShowY = 0.0;
		//TODO typeIdx "GAMEOVER"
		animationCreateShowTextCommon(LevelAnimation::REFSYS_SHOW, -1, std::move(aText), 10000.0, fShowX, fShowY, nShowW, nShowH
										, s_nZObjectZGameOver, -1);
	}
}
bool Level::gameStatusFailed(int32_t nLevelTeam, bool bForceOtherCompleted, bool bCreateTexts) noexcept
{
//std::cout << "Level::gameStatusFailed nLevel=" << getLevel() << "  nLevelTeam=" << nLevelTeam << '\n';
	assert((nLevelTeam >= 0) && (nLevelTeam < m_nTotLevelTeams));
	TeamData& oTeamData = m_aTeamData[nLevelTeam];
	auto& oVarTeamFinishedTime = oTeamData.m_oVariables.getVariable(m_p0Game->getTeamVarIdFinishedTime());
	if (oVarTeamFinishedTime.get() >= 0) {
		// The team already won or lost, ignore
		return false; //--------------------------------------------------------
	}
	//
	gameStatusSetTeamStatus(nLevelTeam, false);
	//
	assert(m_nTotGameEndedTeams < m_nTotLevelTeams);
	++m_nTotGameEndedTeams;

	const int32_t nRank = m_p0Game->gameStatusFailed(m_nLevelInGame, nLevelTeam, bForceOtherCompleted, bCreateTexts);
	assert(nRank >= 1);

	gameStatusSetTeamRank(nLevelTeam, nRank);

	if ((!bCreateTexts) || ((!m_bSubshowMode) && m_bAllTeamsInOneLevel && (nRank != 1))) {
		// In show mode only the game winner creates texts
		return true; //---------------------------------------------------------
	}
	if (nRank == 1) {
		// create texts for winner!
		gameStatusWinnerTexts(oTeamData);
		return true; //---------------------------------------------------------
	}
	gameStatusLoserTexts(oTeamData);
	return true;
}
void Level::gameStatusLoserTexts(TeamData& oTeamData) noexcept
{
	std::vector<std::string> aText;

	const int32_t nTotTeammates = oTeamData.m_nTotTeammates;
	const int32_t nTotLevels = (m_bAllTeamsInOneLevel ? 1 : m_refPreferences->getTotTeams());
	if ((nTotLevels == 1) && (m_nTotLevelTeams == 1)) {
		aText.push_back(s_sGameOverLoseOne);
	} else {
		assert(m_bSubshowMode || (!m_bAllTeamsInOneLevel));
		aText.push_back(s_sGameOverLoseMany);
	}
	if (m_bSubshowMode) {
		for (int32_t nMate = 0; nMate < nTotTeammates; ++nMate) {
			auto aTextCopy = aText;
			MateData& oMateData = oTeamData.m_aTeammate[nMate];
			const int32_t nSubShowW = oMateData.m_oSubshow.getW();
			const int32_t nSubShowH = oMateData.m_oSubshow.getH();
			const double fSubShowX = 0.0;
			const double fSubShowY = 0.0;
			animationCreateShowTextCommon(static_cast<LevelAnimation::REFSYS>(LevelAnimation::REFSYS_SUBSHOW + oMateData.m_nLevelPlayer)
										, -1, std::move(aTextCopy), 10001.0, fSubShowX, fSubShowY, nSubShowW, nSubShowH
										, s_nZObjectZGameOver, -1);
		}
	} else {
		const int32_t nShowW = m_oShow.getW();
		const int32_t nShowH = m_oShow.getH();
		const double fShowX = 0.0;
		const double fShowY = 0.0;
		animationCreateShowTextCommon(LevelAnimation::REFSYS_SHOW, -1, std::move(aText), 10000.0, fShowX, fShowY, nShowW, nShowH
										, s_nZObjectZGameOver, -1);
	}
}
void Level::gameStatusTechnical(const std::vector<std::string>& aIssue) noexcept
{
	auto aText = aIssue;
	aText.insert(aText.begin(), s_sGameOverTechnical);
	if (m_bSubshowMode) {
		const int32_t nLevelTeam = 0;
		const int32_t nMate = 0;
		const TeamData& oTeamData = m_aTeamData[nLevelTeam];
		const MateData& oMateData = oTeamData.m_aTeammate[nMate];
		const int32_t nSubShowW = oMateData.m_oSubshow.getW();
		const int32_t nSubShowH = oMateData.m_oSubshow.getH();
		const double fSubShowX = 0.0;
		const double fSubShowY = 0.0;
		animationCreateShowTextCommon(static_cast<LevelAnimation::REFSYS>(LevelAnimation::REFSYS_SUBSHOW + oMateData.m_nLevelPlayer)
									, -1, std::move(aText), 10001.0, fSubShowX, fSubShowY, nSubShowW, nSubShowH
									, s_nZObjectZGameOver, -1);
	} else {
		const int32_t nShowW = m_oShow.getW();
		const int32_t nShowH = m_oShow.getH();
		const double fShowX = 0.0;
		const double fShowY = 0.0;
		animationCreateShowTextCommon(LevelAnimation::REFSYS_SHOW, -1, std::move(aText), 10000.0, fShowX, fShowY, nShowW, nShowH
										, s_nZObjectZGameOver, -1);
	}

	m_p0Game->gameStatusTechnical(m_nLevelInGame, aIssue);
}

double Level::getInterval() noexcept
{
	return m_fInterval;
}
void Level::setInterval(double fMillisec) noexcept
{
//std::cout << "Level::setInterval() fMillisec=" << fMillisec << '\n';
	assert(fMillisec > 0.0);
	if (fMillisec == m_fInterval) {
		return;
	}
	m_fInterval = fMillisec;
	game().changedInterval(m_nLevelInGame);
}

#ifndef NDEBUG
void Level::dump(bool bDumpBoard, bool bDumpLevelBlocks, bool bDumpTileAni, bool bDumpCharNames, bool bDumpColorNames, bool bDumpFontNames) const noexcept
{
	if (bDumpBoard) {
		const int32_t nTotTileAni = getNrTileAniAttrs();
		std::cout << "Level::dump()  (nX,nY) = (Char,Color,Alpha,Font)" << '\n';
		for (int32_t nY = 0; nY < boardHeight(); ++nY) {
			for (int32_t nX = 0; nX < boardWidth(); ++nX) {
				std::cout << "    (" << nX << "," << nY << ") = ";
				const Tile& oTile = boardGetTile(nX, nY);
				oTile.dump();
				if (bDumpLevelBlocks) {
					std::cout << "         o";
					LevelBlock* p0Owner = boardGetOwner(nX, nY);
					if (p0Owner == nullptr) {
						std::cout << "(-)";
					} else {
						std::cout << "(" << p0Owner->blockGetId() << ")";
					}
				}
				if (bDumpTileAni) {
					std::cout << "         ";
					for (int32_t nAni = 0; nAni < nTotTileAni; ++nAni) {
						if (boardGetTileAnimator(nX, nY, nAni) != nullptr) {
							std::cout << "  i" << nAni;
						}
					}
				}
				std::cout << '\n';
			}
		}
	}
	if (bDumpLevelBlocks) {
		std::cout << "Level::dump()  LevelBlock[Id]=(PosX, PosY, ShapeId)  bricks (RelX,RelY,BrickId).." << '\n';
		std::cout << "                       contacts DIR (RelX,RelY,BrickId).." << '\n';
		for (auto& it : m_oAllLevelBlocks) {
			const int32_t nId = it.first;
			LevelBlock* p0LevelBlock = it.second;
			const int32_t nShapeId = p0LevelBlock->m_nShapeId;
			std::cout << "    LevelBlock[" << nId << "]=(" << p0LevelBlock->m_nPosX << "," << p0LevelBlock->m_nPosY << "," << nShapeId;
			std::cout << ")  bricks ";
			const Block& oBlock = p0LevelBlock->m_oBlock;
			auto aBrickId = oBlock.brickIds();
			for (auto& nBrickId : aBrickId) {
				std::cout << " (" << oBlock.shapeBrickPosX(nShapeId, nBrickId) << "," << oBlock.shapeBrickPosY(nShapeId, nBrickId) << "," << nBrickId << ")";
			}
			std::cout << '\n';
			for (int32_t nDir = 0; nDir < 4; ++nDir) {
				Direction::VALUE eDir = static_cast<Direction::VALUE>(nDir);
				std::cout << "                        contacts ";
				switch (eDir) {
					case Direction::UP: std::cout << "UP    "; break;
					case Direction::DOWN: std::cout << "DOWN  "; break;
					case Direction::LEFT: std::cout << "LEFT  "; break;
					case Direction::RIGHT: std::cout << "RIGHT "; break;
					default: assert(false);
				}
				auto aContacts = oBlock.shapeContacts(nShapeId, static_cast<Direction::VALUE>(nDir));
				for (Block::Contact& oContact : aContacts) {
					std::cout << " (" << oContact.m_nRelX << "," << oContact.m_nRelY << "," << oContact.m_nBrickId << ")";
				}
				std::cout << '\n';
			}
		}
	}
	if (bDumpTileAni) {
		std::cout << "Level::dump()  TileAni[i]= NAME" << '\n';
		const int32_t nNamedTileAnis = getNamed().tileAnis().size();
		for (int32_t nAni = 0; nAni < nNamedTileAnis; ++nAni) {
			std::cout << "    TileAni[" << nAni << "]= " << getNamed().tileAnis().getName(nAni) << '\n';
		}
	}
	if (bDumpCharNames) {
		std::cout << "Level::dump()  Char[i]= NAME" << '\n';
		const int32_t nNamedChars = getNamed().chars().size();
		for (int32_t nChar = 0; nChar < nNamedChars; ++nChar) {
			std::cout << "    Char[" << nChar << "]= " << getNamed().chars().getName(nChar) << '\n';
		}
	}
	if (bDumpColorNames) {
		std::cout << "Level::dump()  Color[i]= NAME" << '\n';
		const int32_t nNamedColors = getNamed().colors().size();
		for (int32_t nColor = 0; nColor < nNamedColors; ++nColor) {
			std::cout << "    Color[" << nColor << "]= " << getNamed().colors().getName(nColor) << '\n';
		}
	}
	if (bDumpFontNames) {
		std::cout << "Level::dump()  Font[i]= NAME" << '\n';
		const int32_t nNamedFonts = getNamed().fonts().size();
		for (int32_t nFont = 0; nFont < nNamedFonts; ++nFont) {
			std::cout << "    Font[" << nFont << "]= " << getNamed().fonts().getName(nFont) << '\n';
		}
	}
}
#endif //NDEBUG

} // namespace stmg
