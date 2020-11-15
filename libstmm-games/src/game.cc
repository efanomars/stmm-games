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
 * File:   game.cc
 */

#include "game.h"
#include "stdrandomsource.h"
#include "gameowner.h"
#include "gameview.h"

#include "level.h"
#include "highscore.h"
#include "appconfig.h"
#include "apppreferences.h"
#include "highscoresdefinition.h"
#include "keyactionevent.h"
#include "layout.h"
#include "util/basictypes.h"

#include <stmm-input/event.h>
#include <stmm-input/capability.h>
#include <stmm-input/hardwarekey.h>
#include <stmm-input/xyevent.h>

#include <vector>
#include <cassert>
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <iostream>

#include <ctype.h>

namespace stmi { class Accessor; }

namespace stmg
{

std::atomic<int32_t> Game::s_nNewIdCounter(0);

static const std::string s_sGameVarIdGameOverTime = "__GameOverTime";
static const std::string s_sGameVarIdWinnerTeam = "__WinnerTeam";
static const std::string s_sTeamVarIdFinishedTime = "__TeamFinishedTime";
static const std::string s_sTeamVarIdRank = "__Rank";
static const std::string s_sTeamVarIdHighscoreRank = "__HighscoreRank";
static const std::string s_sTeamVarIdStatus = "__TeamStatus";
static const std::string s_sPlayerVarIdFinishedTime = "PlayerVarIdFinishedTime";
static const std::string s_sPlayerVarIdStatus = "__PlayerStatus";

static const std::string s_sDefaultBoardPainter = "PAINTER:BOARD";
static const std::string s_sDefaultBlockPainter = "PAINTER:BLOCK";

int32_t Game::getNewGameId() noexcept
{
	const int32_t nNewId = ++s_nNewIdCounter;
	return nNewId;
}

Game::Game(Init&& oInit, CreateLevelCallback& oCreateLevelCallback, const Level::Init& oLevelInit) noexcept
{
	reInit(std::move(oInit), oCreateLevelCallback, oLevelInit);
}

void Game::reInit(Init&& oInit, CreateLevelCallback& oCreateLevelCallback, const Level::Init& oLevelInit) noexcept
{
//std::cout << "Game::reInit()" << '\n';
	m_nGameId = -1; // set when game is started

	m_sName = std::move(oInit.m_sName);

	m_nMaxViewTicks = oInit.m_nMaxViewTicks;
	assert(m_nMaxViewTicks > 0);
	m_nAdditionalHighscoresWait = oInit.m_nAdditionalHighscoresWait;
	assert(m_nAdditionalHighscoresWait >= 0);

	assert(oInit.m_p0GameOwner != nullptr);
	assert(oInit.m_refLayout);
	assert(! m_sName.empty());
	#ifndef NDEBUG
	for (const auto& c : m_sName) {
		assert(std::isalnum(c) || (c == '-') || (c == '_'));
	}
	#endif //NDEBUG

	m_bGameEnded = true;
	m_bGameEndedEmitted = false;

	m_bInGameTick = false;
	m_nTick = 0;

	if (!oInit.m_refRandomSource) {
		m_refRandomSource = std::make_unique<StdRandomSource>();
	} else {
		m_refRandomSource = std::move(oInit.m_refRandomSource);
	}

	m_p0GameOwner = std::move(oInit.m_p0GameOwner);
	m_p0GameView = nullptr;
	m_refPrefs = oInit.m_refLayout->getPrefs();
	const auto& refAppConfig = m_refPrefs->getAppConfig();
	m_bSoundEnabled = refAppConfig->soundEnabled();

	m_fMinGameInterval = std::max(1.0, oInit.m_fMinGameInterval);
	m_fLastInterval = std::max(oInit.m_fInitialGameInterval, oInit.m_fMinGameInterval);
	m_fNextInterval = m_fLastInterval;
	m_fElapsedTime = 0.0;

	m_fSoundScaleX = oInit.m_fSoundScaleX;
	m_fSoundScaleY = oInit.m_fSoundScaleY;
	m_fSoundScaleZ = oInit.m_fSoundScaleZ;

	m_bAllTeamsInOneLevel = oInit.m_refLayout->isAllTeamsInOneLevel();

	m_oNamed = std::move(oInit.m_oNamed);

	m_nBoardPainterIdx = oInit.m_nBoardPainterIdx;
	if (m_nBoardPainterIdx < 0) {
		m_nBoardPainterIdx = m_oNamed.painters().getIndex(s_sDefaultBoardPainter);
	}
	m_nBlockPainterIdx = oInit.m_nBlockPainterIdx;
	if (m_nBlockPainterIdx < 0) {
		m_nBlockPainterIdx = m_oNamed.painters().getIndex(s_sDefaultBlockPainter);
	}

	m_oGameVariableTypes = std::move(oInit.m_oGameVariableTypes);
	m_oTeamVariableTypes = std::move(oInit.m_oTeamVariableTypes);
	m_oPlayerVariableTypes = std::move(oInit.m_oPlayerVariableTypes);
	// Check no system names used for variables.
	assert(m_oGameVariableTypes.findIndexNameStarts("__") < 0);
	assert(m_oTeamVariableTypes.findIndexNameStarts("__") < 0);
	assert(m_oPlayerVariableTypes.findIndexNameStarts("__") < 0);

	Variable::VariableType oVarType;
	oVarType.m_nInitialValue = -1;
	oVarType.m_bReadOnly = true;
	//
	m_nGameVarIdGameOverTime = m_oGameVariableTypes.addNamedObj(s_sGameVarIdGameOverTime, oVarType); // -1: still playing
	m_nGameVarIdWinnerTeam = m_oGameVariableTypes.addNamedObj(s_sGameVarIdWinnerTeam, oVarType); // The team number according to preferences
	//
	m_nTeamVarIdFinishedTime = m_oTeamVariableTypes.addNamedObj(s_sTeamVarIdFinishedTime, oVarType); // -1: still playing
	m_nTeamVarIdRank = m_oTeamVariableTypes.addNamedObj(s_sTeamVarIdRank, oVarType); // Starting from 0 (the winner), 1, ...
	m_nTeamVarIdHighscoreRank = m_oTeamVariableTypes.addNamedObj(s_sTeamVarIdHighscoreRank, oVarType); // Starting from 0 (the winner), 1, ...
	oVarType.m_nInitialValue = GameProxy::VAR_VALUE_TEAM_STATUS_PLAYING;
	m_nTeamVarIdStatus = m_oTeamVariableTypes.addNamedObj(s_sTeamVarIdStatus, oVarType);
	//
	oVarType.m_nInitialValue = -1;
	m_nPlayerVarIdFinishedTime = m_oPlayerVariableTypes.addNamedObj(s_sPlayerVarIdFinishedTime,oVarType); // -1: still playing
	oVarType.m_nInitialValue = GameProxy::VAR_VALUE_PLAYER_STATUS_PLAYING;
	m_nPlayerVarIdStatus = m_oPlayerVariableTypes.addNamedObj(s_sPlayerVarIdStatus, oVarType);

	#ifndef NDEBUG
	const int32_t nTotGameVarTypes = m_oGameVariableTypes.size();
	for (int32_t nGameVarIdx = 0; nGameVarIdx < nTotGameVarTypes; ++nGameVarIdx) {
		const std::string& sGameVarName = m_oGameVariableTypes.getName(nGameVarIdx);
		//
		const int32_t nTotTeamVarTypes = m_oTeamVariableTypes.size();
		for (int32_t nTeamVarIdx = 0; nTeamVarIdx < nTotTeamVarTypes; ++nTeamVarIdx) {
			const std::string& sTeamVarName = m_oTeamVariableTypes.getName(nTeamVarIdx);
			assert(sGameVarName != sTeamVarName);
		}
		//
		const int32_t nTotPlayerVarTypes = m_oPlayerVariableTypes.size();
		for (int32_t nPlayerVarIdx = 0; nPlayerVarIdx < nTotPlayerVarTypes; ++nPlayerVarIdx) {
			const std::string& sPlayerVarName = m_oPlayerVariableTypes.getName(nPlayerVarIdx);
			assert(sGameVarName != sPlayerVarName);
		}
	}
	const int32_t nTotTeamVarTypes = m_oTeamVariableTypes.size();
	for (int32_t nTeamVarIdx = 0; nTeamVarIdx < nTotTeamVarTypes; ++nTeamVarIdx) {
		const std::string& sTeamVarName = m_oTeamVariableTypes.getName(nTeamVarIdx);
		//
		const int32_t nTotPlayerVarTypes = m_oPlayerVariableTypes.size();
		for (int32_t nPlayerVarIdx = 0; nPlayerVarIdx < nTotPlayerVarTypes; ++nPlayerVarIdx) {
			const std::string& sPlayerVarName = m_oPlayerVariableTypes.getName(nPlayerVarIdx);
			assert(sTeamVarName != sPlayerVarName);
		}
	}
	#endif //NDEBUG

	m_oGameVariables.init(m_oGameVariableTypes, this);

	const int32_t nTotPrefTeams = m_refPrefs->getTotTeams();
	const int32_t nTotLevels = (m_bAllTeamsInOneLevel ? 1 : nTotPrefTeams);
	m_aLevel.clear();
	m_aLevel.resize(nTotLevels);
	int32_t nLevel = 0;
	for (auto& refLevel : m_aLevel) {
		refLevel = oCreateLevelCallback.createLevel(this, nLevel, m_refPrefs, oLevelInit);
		assert(refLevel);
		assert(nLevel == refLevel->getLevel());
		//
		refLevel->variablesInit(m_oTeamVariableTypes, m_oPlayerVariableTypes);
		++nLevel;
	}

	m_refLayout = std::move(oInit.m_refLayout);
//#ifndef NDEBUG
//if (!m_refLayout->isValid()) {
//std::cout << "Game::reInit Layout Error  " << m_refLayout->getErrorString() << '\n';
//}
//#endif //NDEBUG
	assert(m_refLayout->isValid());
	m_aOpenKeyActions.resize(refAppConfig->getTotKeyActions());
	m_bHasKeyActions = !m_aOpenKeyActions.empty();
	m_bIsEventAssignedToActivePlayer = refAppConfig->isEventAssignedToActivePlayer();
	//
	m_refLayout->setGame(this);

	m_refHighscoresDefinition = std::move(oInit.m_refHighscoresDefinition);
	m_refPreGameHighscore = std::move(oInit.m_refHighscore);
	if (m_refPreGameHighscore) {
		if (m_refHighscoresDefinition) {
			// must be equivalent
			assert((*m_refPreGameHighscore->getHighscoresDefinition()) == *m_refHighscoresDefinition);
		} else {
			m_refHighscoresDefinition = m_refPreGameHighscore->getHighscoresDefinition();
		}
	}
	if (!m_refHighscoresDefinition) {
		setDefaultHighscoreDefinition(nTotPrefTeams);
	}
	assert(m_refHighscoresDefinition->getMaxScores() >= m_refPrefs->getTotTeams());
//#ifndef NDEBUG
//std::cout << "Game::reInit  HighscoresDefinition dump" << '\n';
//m_refHighscoresDefinition->dump();
//#endif //NDEBUG
}
void Game::setDefaultHighscoreDefinition(int32_t nTotScores) noexcept
{
	HighscoresDefinition::ScoreElement oDummy;
	std::vector<HighscoresDefinition::ScoreElement> aScoreElements;
	// Make a highscore definition which contains just variable rank
	static shared_ptr<HighscoresDefinition> s_refDefaultHD = (
		oDummy.m_sVarName = s_sTeamVarIdRank
		, oDummy.m_bLowestBest = true
		, aScoreElements.push_back(oDummy)
		, std::make_shared<HighscoresDefinition>(m_refPrefs->getAppConfig()
												, std::vector<HighscoresDefinition::HighscoreConstraint>{}
												, std::vector<HighscoresDefinition::Discriminator>{}
												, std::vector<HighscoresDefinition::Eligibility>{}
												, aScoreElements, nTotScores)
		);
	m_refHighscoresDefinition = s_refDefaultHD;
}
void Game::start() noexcept
{
	if (! m_bGameEnded) {
		return;
	}

	m_nGameId = getNewGameId();
	m_bGameEnded = false; // true when m_nRankCompleted becomes bigger than m_nRankFailed
	m_bGameEndedEmitted = false;
	const int32_t nTotTeams = m_refPrefs->getTotTeams();
	m_nRankCompleted = 1; // The winner has rank 1
	m_nRankFailed = nTotTeams;
	m_nTick = 0;
	m_fElapsedTime = 0.0;
	//
	if (!m_refInGameHighscore) {
		m_refInGameHighscore = std::make_unique<RecycledHighscore>(m_refHighscoresDefinition, "", "");
	} else {
		m_refInGameHighscore->reInit(m_refHighscoresDefinition, "", "");
	}

	m_oGameVariables.init(m_oGameVariableTypes, this);
	//
	for (auto& refLevel : m_aLevel) {
		refLevel->variablesInit(m_oTeamVariableTypes, m_oPlayerVariableTypes);
		//
		refLevel->gameStart();
	}

	calcGameInterval();
	m_fLastInterval = m_fNextInterval;

	assert(m_refHighscoresDefinition->isCompatibleWithGame(*this).empty());
}
int32_t Game::getPlayer(int32_t nLevel, int32_t nLevelPlayer) noexcept
{
	assert((nLevel >= 0) && (nLevel < getTotLevels()));
	if (m_bAllTeamsInOneLevel) {
		assert((nLevelPlayer >= 0) && (nLevelPlayer < m_refPrefs->getTotPlayers()));
		return nLevelPlayer;
	} else {
		return m_refPrefs->getTeam(nLevel)->getMate(nLevelPlayer)->get();
	}
}
const Variable& Game::variable(int32_t nId, int32_t nLevel, int32_t nLevelTeam, int32_t nMate) const noexcept
{
//std::cout << "Game(" << reinterpret_cast<int64_t>(this) << ")::variable()" << '\n';
	auto p0NotConstThis = const_cast<Game*>(this);
	return p0NotConstThis->variable(nId, nLevel, nLevelTeam, nMate);
}
Variable& Game::variable(int32_t nId, int32_t nLevel, int32_t nLevelTeam, int32_t nMate) noexcept
{
//std::cout << "Game(" << reinterpret_cast<int64_t>(this) << ")::variable()" << '\n';
	if (nLevel == -1) {
		assert((nMate == -1) && (nLevelTeam == -1));
		return m_oGameVariables.getVariable(nId);
	} else {
		assert((nLevel >= 0) && (nLevel < getTotLevels()));
		return m_aLevel[nLevel]->variable(nId, nLevelTeam, nMate);
	}
}
const Variable& Game::variable(int32_t nId) const noexcept
{
	return m_oGameVariables.getVariable(nId);
}
Variable& Game::variable(int32_t nId) noexcept
{
	return m_oGameVariables.getVariable(nId);
}

std::pair<int32_t, OwnerType> Game::variableIdAndOwner(const std::string& sName) const noexcept
{
	OwnerType eOwnerType;
	int32_t nVarId = m_oGameVariableTypes.getIndex(sName);
	if (nVarId >= 0) {
		eOwnerType = OwnerType::GAME;
	} else {
		nVarId = m_oTeamVariableTypes.getIndex(sName);
		if (nVarId >= 0) {
			eOwnerType = OwnerType::TEAM;
		} else {
			nVarId = m_oPlayerVariableTypes.getIndex(sName);
			if (nVarId >= 0) {
				eOwnerType = OwnerType::PLAYER;
			} else {
				assert(nVarId == -1);
				eOwnerType = OwnerType::GAME;
			}
		}
	}
	return std::make_pair(nVarId, eOwnerType);
}
bool Game::hasVariableId(int32_t nId, OwnerType eOwnerType) const noexcept
{
	switch (eOwnerType) {
	case OwnerType::GAME:
	{
		return (nId >= 0) && (nId < static_cast<int32_t>(m_oGameVariableTypes.size()));
	}
	case OwnerType::TEAM:
	{
		return (nId >= 0) && (nId < static_cast<int32_t>(m_oTeamVariableTypes.size()));
	}
	case OwnerType::PLAYER:
	{
		return (nId >= 0) && (nId < static_cast<int32_t>(m_oPlayerVariableTypes.size()));
	}
	default:
	{
		return false;
	}
	}
}
const shared_ptr<Layout>& Game::getLayout() const noexcept
{
	return m_refLayout;
}

int32_t Game::random(int32_t nFrom, int32_t nTo) noexcept
{
	assert(nFrom <= nTo);
	return m_refRandomSource->random(nFrom, nTo);
}
void Game::createKeyActionFromXYEvent(int32_t nLevel, int32_t nLevelTeam, int32_t nMate
									, int32_t nKeyActionId, const shared_ptr<stmi::Event>& refXYEvent) noexcept
{
	assert(isInGameTick());
	assert(dynamic_cast<stmi::XYEvent*>(refXYEvent.get()) != nullptr);
	auto p0XYEvent = static_cast<stmi::XYEvent*>(refXYEvent.get());
	const int64_t nXYGrabId = p0XYEvent->getXYGrabId();
	stmi::Event::AS_KEY_INPUT_TYPE eType;
	switch (p0XYEvent->getXYGrabType()) {
	case stmi::XYEvent::XY_GRAB: eType = stmi::Event::AS_KEY_PRESS; break;
	case stmi::XYEvent::XY_UNGRAB: eType = stmi::Event::AS_KEY_RELEASE; break;
	case stmi::XYEvent::XY_UNGRAB_CANCEL: eType = stmi::Event::AS_KEY_RELEASE_CANCEL; break;
	default: return; //---------------------------------------------------------
	}

	createKeyAction(nLevel, nLevelTeam, nMate, nKeyActionId, eType, nXYGrabId, refXYEvent);
}
void Game::createKeyAction(int32_t nLevel, int32_t nLevelTeam, int32_t nMate
							, int32_t nKeyActionId, stmi::Event::AS_KEY_INPUT_TYPE eType
							, int64_t nXYGrabId, const shared_ptr<stmi::Event>& refEvent) noexcept
{
	assert((nLevel >= 0) && (nLevelTeam >= 0) && (nMate >= 0));
	assert((nKeyActionId >= 0) && (nKeyActionId < static_cast<int32_t>(m_aOpenKeyActions.size())));
	auto& aOpenKeyActions = m_aOpenKeyActions[nKeyActionId];
	const int32_t nCapabilityId = refEvent->getCapabilityId();
	if (eType == stmi::Event::AS_KEY_PRESS) {
		auto itFind = std::find_if(aOpenKeyActions.begin(), aOpenKeyActions.end(), [&](const OpenKeyAction& oOKA)
			{
				return (oOKA.m_nLevel == nLevel) && (oOKA.m_nLevelTeam == nLevelTeam) && (oOKA.m_nMate == nMate);
			});
		if (itFind != aOpenKeyActions.end()) {
			// A key action for the same player is already on
			return; //----------------------------------------------------------
		}
		aOpenKeyActions.push_back(OpenKeyAction{nLevel, nLevelTeam, nMate, nCapabilityId, nXYGrabId});
	} else if ((eType == stmi::Event::AS_KEY_RELEASE) || (eType == stmi::Event::AS_KEY_RELEASE_CANCEL)) {
		auto itFind = std::find_if(aOpenKeyActions.begin(), aOpenKeyActions.end(), [&](const OpenKeyAction& oOKA)
		{
			return (oOKA.m_nLevel == nLevel) && (oOKA.m_nLevelTeam == nLevelTeam) && (oOKA.m_nMate == nMate)
					&& (oOKA.m_nCapabilityId == nCapabilityId) && (oOKA.m_nXYGrabId == nXYGrabId);
		});
		if (itFind == aOpenKeyActions.end()) {
			return; //----------------------------------------------------------
		}
		auto nIdx = std::distance(aOpenKeyActions.begin(), itFind);
		std::swap(aOpenKeyActions[nIdx], aOpenKeyActions.back());
		aOpenKeyActions.pop_back();
	} else {
		return; //--------------------------------------------------------------
	}
	shared_ptr<KeyActionEvent> refKAEvent;
	for (auto& refFreeEvent : m_aInputRecycle) {
		if (refFreeEvent.use_count() == 1) {
			refKAEvent = refFreeEvent;
			break; //for ------
		}
	}
	const int64_t nTimeUsec = refEvent->getTimeUsec();
	if (!refKAEvent) {
		refKAEvent = std::make_shared<KeyActionEvent>(nTimeUsec, shared_ptr<stmi::Accessor>{}, refEvent->getCapability()
													, eType, nKeyActionId);
		m_aInputRecycle.push_back(refKAEvent);
	} else {
		refKAEvent->setTimeUsec(nTimeUsec);
		refKAEvent->setType(eType);
		refKAEvent->setKeyAction(nKeyActionId);
		refKAEvent->setCapability(refEvent->getCapability());
	}
	level(nLevel)->handleKeyActionInput(nLevelTeam, nMate, refKAEvent);
}
void Game::handleInput(const shared_ptr<stmi::Event>& refEvent) noexcept
{
//std::cout << "Game::handleInput  " << refEvent->getTimeUsec() << '\n';
	if (m_bGameEnded) {
		return;
	}
	const bool bDispatch = isInGameTick();
	if (bDispatch) {
		// Within the game tick there's no need to queue
		dispatchInput(refEvent);
	} else {
		// queue the event
		m_aInputQueue.push_back(refEvent);
	}
}
void Game::dispatchInput(const shared_ptr<stmi::Event>& refEvent) noexcept
{
//std::cout << "Game::dispatchInput  " << refEvent->getTimeUsec() << "  m_bHasKeyActions=" << m_bHasKeyActions << '\n';
	if (m_bHasKeyActions) {
		stmi::HARDWARE_KEY eKey;
		stmi::Event::AS_KEY_INPUT_TYPE eType;
		bool bMoreThanOne;
		if (refEvent->getAsKey(eKey, eType, bMoreThanOne)) {
			static const int64_t nKeyXYGrabId = -7777; // invalid grab id
			int32_t nPrefTeam, nMate;
			int32_t nKeyActionId;
			const int32_t nCapabilityId = refEvent->getCapabilityId();
//std::cout << "Game::dispatchInput  nCapabilityId=" << nCapabilityId << '\n';
			const bool bIsAction = m_refPrefs->getPlayerKeyActionFromCapabilityKey(nCapabilityId, eKey
																					, nPrefTeam, nMate, nKeyActionId);
			if (bIsAction) {
//std::cout << "Game::dispatchInput  isKeyAction nKeyActionId=" << nKeyActionId << '\n';
				int32_t nLevel, nLevelTeam;
				convertPrefToLevelTeam(nPrefTeam, nLevel, nLevelTeam);
				#ifndef NDEBUG
				shared_ptr<stmi::Capability> refCapability = refEvent->getCapability();
				assert(refCapability->getId() == nCapabilityId);
				#endif //NDEBUG
				createKeyAction(nLevel, nLevelTeam, nMate, nKeyActionId, eType, nKeyXYGrabId, refEvent);
			}
			if (bMoreThanOne) {
				std::vector< std::pair<stmi::HARDWARE_KEY, stmi::Event::AS_KEY_INPUT_TYPE> > aKeys = refEvent->getAsKeys();
				auto itPair = aKeys.begin();
				++itPair;
				for (; itPair != aKeys.end(); ++itPair) {
					const bool bIsAction = m_refPrefs->getPlayerKeyActionFromCapabilityKey(nCapabilityId, itPair->first
																							, nPrefTeam, nMate, nKeyActionId);
					if (bIsAction) {
						int32_t nLevel, nLevelTeam;
						convertPrefToLevelTeam(nPrefTeam, nLevel, nLevelTeam);
						#ifndef NDEBUG
						shared_ptr<stmi::Capability> refCapability = refEvent->getCapability();
						assert(refCapability->getId() == nCapabilityId);
						#endif //NDEBUG
						createKeyAction(nLevel, nLevelTeam, nMate, nKeyActionId, itPair->second, nKeyXYGrabId, refEvent);
					}
				}
			}
		}
	}
	const auto& oEvClass = refEvent->getEventClass();
	const bool bIsXYEvent = oEvClass.isXYEvent();
	if (bIsXYEvent) {
		//assert(dynamic_cast<stmi::XYEvent*>(refEvent.get()));
		if (m_p0GameView != nullptr) {
			m_p0GameView->handleXYEvent(refEvent);
		}
		return; //--------------------------------------------------------------
	}
	int32_t nLevel = -1;
	int32_t nLevelTeam = -1;
	int32_t nMate = -1;
	int32_t nPrefTeam = -1;
	const int32_t nCapabilityId = refEvent->getCapabilityId();
	const bool bAssigned = m_refPrefs->getCapabilityPlayer(nCapabilityId, nPrefTeam, nMate);
	if (bAssigned) {
		convertPrefToLevelTeam(nPrefTeam, nLevel, nLevelTeam);
	} else if (!m_bIsEventAssignedToActivePlayer) {
		// the event is ignored
		return; //----------------------------------------------------------
	} else {
		const bool bFoundPlayer = getUniqueActiveHumanPlayer(nLevel, nLevelTeam, nMate);
		if (!bFoundPlayer) {
			// no human player is active
			return; //------------------------------------------------------
		}
	}
	level(nLevel)->handleInput(nLevelTeam, nMate, refEvent);
}
void Game::dispatchInputs() noexcept
{
	// Note: during dispatch the queue might grow (that's why it's a std::deque)
	auto itQueue = m_aInputQueue.begin();
	while (itQueue != m_aInputQueue.end()) {
		auto& refEvent = *itQueue;
		dispatchInput(refEvent);
		++itQueue;
	}
	m_aInputQueue.clear();
}
bool Game::getUniqueActiveHumanPlayer(int32_t& nTeam, int32_t& nMate) noexcept
{
	int32_t nLevel = (m_bAllTeamsInOneLevel ? 0 : nTeam);
	int32_t nLevelTeam = (m_bAllTeamsInOneLevel ? nTeam : 0);
	const bool bFoundPlayer = getUniqueActiveHumanPlayer(nLevel, nLevelTeam, nMate);
	if (!bFoundPlayer) {
		return false; //--------------------------------------------------------
	}
	nTeam = (m_bAllTeamsInOneLevel ? nLevelTeam : nLevel);
	return true;
}
bool Game::getUniqueActiveHumanPlayer(int32_t& nLevel, int32_t& nLevelTeam, int32_t& nMate) noexcept
{
	int32_t nFromLevel = 0;
	int32_t nToLevel = static_cast<int32_t>(m_aLevel.size()) - 1;
	int32_t nOnlyLevelTeam = -1;
	int32_t nOnlyMate = -1;
	if (nLevel >= 0) {
		nFromLevel = nLevel;
		nToLevel = nLevel;
		if (nLevelTeam >= 0) {
			nOnlyLevelTeam = nLevelTeam;
			if (nMate >= 0) {
				nOnlyMate = nMate;
			}
		}
	}
	int32_t nFoundLevel = -1;
	int32_t nFoundLevelTeam = -1;
	int32_t nFoundMate = -1;
	for (int32_t nCurLevel = nFromLevel; nCurLevel <= nToLevel; ++nCurLevel) {
		const auto& aAHPlayers = level(nCurLevel)->getActiveHumanPlayers();
		const int32_t nAHPlayers = static_cast<int32_t>(aAHPlayers.size());
		for (int32_t nAHIdx = 0; nAHIdx < nAHPlayers; ++nAHIdx) {
			int32_t nCurLevelTeam = aAHPlayers[nAHIdx].first;
			int32_t nCurMate = aAHPlayers[nAHIdx].second;
			if (((nOnlyLevelTeam == -1) || (nCurLevelTeam == nOnlyLevelTeam))
					&& ((nOnlyMate == -1) || (nCurMate == nOnlyMate))){
				if (nFoundLevel >= 0) {
					// there is more than one active player
					return false; //--------------------------------------------
				}
				nFoundLevel = nCurLevel;
				nFoundLevelTeam = nCurLevelTeam;
				nFoundMate = nCurMate;
			}
		}
	}
	if (nFoundLevel < 0){
		return false; //--------------------------------------------------------
	}
	nLevel = nFoundLevel;
	nLevelTeam = nFoundLevelTeam;
	nMate = nFoundMate;
	return true;
}
void Game::convertPrefToLevelTeam(int32_t nPrefTeam, int32_t& nLevel, int32_t& nLevelTeam) noexcept
{
	if (m_bAllTeamsInOneLevel) {
		nLevel = 0;
		nLevelTeam = nPrefTeam;
	} else { // One team per level
		nLevel = nPrefTeam;
		nLevelTeam = 0;
	}
}

void Game::handleTimer() noexcept
{
//std::cout << "Game::handleTimer(" << gameElapsed() << ")"<< '\n';
	m_fLastInterval = m_fNextInterval;
	for (auto& refLevel : m_aLevel) {
		refLevel->handlePreTimer();
	}
	//
	m_bInGameTick = true;
	dispatchInputs();
	// handles blocks
	const int32_t nTotLevels = static_cast<int32_t>(m_aLevel.size());
	if (nTotLevels == 1) {
		auto& refLevel = m_aLevel[0];
		refLevel->handleTimer();
		refLevel->handleTimerEvents();
	} else {
		for (auto& refLevel : m_aLevel) {
			refLevel->handleTimer();
		}
		// More than one level: events can call othersSend and trigger events
		// to be handled in this game tick
		int32_t nLevel = 0;
		do {
			auto& refLevel = m_aLevel[nLevel];
			if (refLevel->eventsQueueIsDirty()) {
				refLevel->handleTimerEvents();
				// restart
				nLevel = 0;
			} else {
				++nLevel;
			}
		} while (nLevel < nTotLevels);
	}
	m_bInGameTick = false;
	//
	for (auto& refLevel : m_aLevel) {
		assert(! refLevel->eventsQueueIsDirty());
		refLevel->handlePostTimer();
	}
	if (m_bGameEnded && !m_bGameEndedEmitted) {
		m_bGameEndedEmitted = true;
		m_p0GameOwner->gameEnded();
	}
	++m_nTick;
	m_fElapsedTime += m_fLastInterval;
}
void Game::interrupt(GameProxy::INTERRUPT_TYPE eInterruptType) noexcept
{
	if (m_bGameEnded) {
		return;
	}
	m_p0GameOwner->gameInterrupt(eInterruptType);
}
void Game::changedInterval(int32_t nLevel) noexcept
{
	#ifndef NDEBUG
	const int32_t nTotLevels = static_cast<int32_t>(m_aLevel.size());
	#endif //NDEBUG
	assert((nLevel >= 0) && (nLevel < nTotLevels));
	assert(isInGameTick());
	double fNewInterval = m_aLevel[nLevel]->getInterval();
	if (fNewInterval < m_fMinGameInterval) {
		fNewInterval = m_fMinGameInterval;
	}
	if (fNewInterval < m_fNextInterval) {
		// getting faster
		m_fNextInterval = std::max(m_fMinGameInterval, fNewInterval);
//std::cout << "Game::changedInterval faster m_fNextInterval=" << m_fNextInterval << '\n';
	} else if (fNewInterval > m_fNextInterval) {
		// the game could be slowing down
		calcGameInterval();
//std::cout << "Game::changedInterval slower? m_fNextInterval=" << m_fNextInterval << '\n';
	}
}
void Game::calcGameInterval() noexcept
{
	// choose shortest interval of all levels: the fastest dictates pace
	double fSmallestInterval = 1000 * 60 * 60 * 24;
	for (auto& refLevel : m_aLevel) {
		fSmallestInterval = std::min<double>(fSmallestInterval, refLevel->getInterval());
	}
	m_fNextInterval = std::max(m_fMinGameInterval, fSmallestInterval);
}

bool Game::isRunning() const noexcept
{
	return !m_bGameEnded;
}
void Game::end() noexcept
{
	const int32_t nTotLevels = static_cast<int32_t>(m_aLevel.size());
	for (int32_t nCurLevel = 0; nCurLevel < nTotLevels; ++nCurLevel) {
		const int32_t nTotLevelTeams = m_aLevel[nCurLevel]->getTotLevelTeams();
		for (int32_t nCurLevelTeam = 0; nCurLevelTeam < nTotLevelTeams; ++nCurLevelTeam) {
			m_aLevel[nCurLevel]->gameStatusFailed(nCurLevelTeam, false, false);
		}
	}
	m_bGameEnded = true;
	m_bGameEndedEmitted = true; // do not emit!
}
int32_t Game::gameStatusPlayingTeams() noexcept
{
	if (m_bGameEnded) {
		return 0;
	}
	int32_t nTotPlayingTeams = 0;
	const int32_t nTotLevels = static_cast<int32_t>(m_aLevel.size());
	for (int32_t nCurLevel = 0; nCurLevel < nTotLevels; ++nCurLevel) {
		const int32_t nTotLevelTeams = m_aLevel[nCurLevel]->getTotLevelTeams();
		for (int32_t nCurLevelTeam = 0; nCurLevelTeam < nTotLevelTeams; ++nCurLevelTeam) {
			if (m_aLevel[nCurLevel]->gameStatusTeamIsPlaying(nCurLevelTeam)) {
				++nTotPlayingTeams;
			}
		}
	}
	assert((m_nRankFailed + 1 - m_nRankCompleted) == nTotPlayingTeams);
	return nTotPlayingTeams;
}
int32_t Game::gameStatusCompleted(int32_t nCompletedLevel, int32_t nLevelTeam, bool bForceOthersFailed, bool bCreateTexts) noexcept
{
	assert(!m_bGameEnded);
	//
	const int32_t nRank = m_nRankCompleted;
	if (nRank == 1) {
		const int32_t nCompletedPrefTeam = (m_bAllTeamsInOneLevel ? nLevelTeam : nCompletedLevel);
		m_oGameVariables.getVariable(m_nGameVarIdWinnerTeam).forceSet(nCompletedPrefTeam);
	}
	++m_nRankCompleted;
	if (bForceOthersFailed) {
		const int32_t nTotLevels = static_cast<int32_t>(m_aLevel.size());
		for (int32_t nCurLevel = 0; nCurLevel < nTotLevels; ++nCurLevel) {
			if (nCurLevel == nCompletedLevel) {
				continue; // for(nCurLevel ------
			}
			const int32_t nTotLevelTeams = m_aLevel[nCurLevel]->getTotLevelTeams();
			for (int32_t nCurLevelTeam = 0; nCurLevelTeam < nTotLevelTeams; ++nCurLevelTeam) {
				m_aLevel[nCurLevel]->gameStatusFailed(nCurLevelTeam, false, bCreateTexts);
			}
		}
	}
	if ((!m_bGameEnded) && (gameStatusPlayingTeams() == 0)) {
		gameHasEnded();
	}
	return nRank;
}
int32_t Game::gameStatusFailed(int32_t nFailedLevel, int32_t nLevelTeam, bool bForceOtherCompleted, bool bCreateTexts) noexcept
{
	assert(!m_bGameEnded);

	const int32_t nRank = m_nRankFailed;
	if (nRank == 1) {
		// The last team to fail the level can be the winner!
		const int32_t nFailedPrefTeam = (m_bAllTeamsInOneLevel ? nLevelTeam : nFailedLevel);
		m_oGameVariables.getVariable(m_nGameVarIdWinnerTeam).forceSet(nFailedPrefTeam);
	}
	--m_nRankFailed;

	if (bForceOtherCompleted) {
		int32_t nFoundLevel = -1;
		int32_t nFoundLevelTeam = -1;
		int32_t nTotTeamsStillPlaying = 0;
		const int32_t nTotLevels = static_cast<int32_t>(m_aLevel.size());
		for (int32_t nCurLevel = 0; nCurLevel < nTotLevels; ++nCurLevel) {
			if (nCurLevel == nFailedLevel) {
				continue; // for(nCurLevel ------
			}
			const int32_t nTotLevelTeams = m_aLevel[nCurLevel]->getTotLevelTeams();
			for (int32_t nCurLevelTeam = 0; nCurLevelTeam < nTotLevelTeams; ++nCurLevelTeam) {
				if (m_aLevel[nCurLevel]->gameStatusTeamIsPlaying(nCurLevelTeam)) {
					if (nFoundLevelTeam < 0) {
						nFoundLevel = nCurLevel;
						nFoundLevelTeam = nCurLevelTeam;
					}
					++nTotTeamsStillPlaying;
				}
			}
			if (nTotTeamsStillPlaying == 1) {
				assert(nFoundLevel >= 0);
				assert(nFoundLevelTeam >= 0);
				m_aLevel[nFoundLevel]->gameStatusCompleted(nFoundLevelTeam, false, bCreateTexts);
			}
		}
	}
	if ((!m_bGameEnded) && (gameStatusPlayingTeams() == 0)) {
		gameHasEnded();
	}
	return nRank;
}
void Game::gameHasEnded() noexcept
{
	m_oGameVariables.getVariable(m_nGameVarIdGameOverTime).forceSet(gameElapsedMillisec());
	// Set highscore rank
	assert(m_refInGameHighscore);
	m_refInGameHighscore->includeGame(*this, false);
	int32_t nTotScores = m_refInGameHighscore->getTotScores();
	for (int32_t nPosition = 0; nPosition < nTotScores; ++nPosition) {
		const Highscore::Score& oScore = m_refInGameHighscore->getScore(nPosition);
		const int32_t nPrefTeam = oScore.m_nTeam;
//std::cout << "Game::gameHasEnded()  nPrefTeam=" << nPrefTeam << "  nPosition=" << nPosition << '\n';
		assert(nPrefTeam >= 0);
		const int32_t nLevel = (m_bAllTeamsInOneLevel ? 0 : nPrefTeam);
		const int32_t nLevelTeam = (m_bAllTeamsInOneLevel ? nPrefTeam : 0);
		Variable& oHSRankVar = m_aLevel[nLevel]->variable(m_nTeamVarIdHighscoreRank, nLevelTeam, -1);
		oHSRankVar.forceSet(nPosition + 1);
//std::cout << "Game::gameHasEnded()  nLevel=" << nLevel << "  nLevelTeam=" << nLevelTeam << "  nPosition=" << nPosition << '\n';
	}
	m_oGameVariables.inhibit();
	m_bGameEnded = true;
	m_bGameEndedEmitted = false;
}
void Game::gameStatusTechnical(int32_t nBadLevel, const std::vector<std::string>& aIssue) noexcept
{
	if (m_bGameEnded) {
		return;
	}
	std::cout << "Technical game over - Bad level: " << nBadLevel << '\n';
	std::cout << "                      Game tick: " << m_nTick << '\n';
	for (const auto& sLine : aIssue) {
		std::cout << sLine << '\n';
	}
	m_bGameEnded = true;
	m_bGameEndedEmitted = false;
}

void Game::othersSend(int32_t nSenderLevel, int32_t nMsg, int32_t nValue) noexcept
{
//std::cout << "Game::othersSend  nMsg=" << nMsg << "  nValue=" << nValue << '\n';
	const int32_t nTotLevels = static_cast<int32_t>(m_aLevel.size());
	for (int32_t nLevel = 0; nLevel < nTotLevels; ++nLevel) {
		if (nSenderLevel != nLevel) {
			auto& refLevel = m_aLevel[nLevel];
			refLevel->othersReceive(nSenderLevel, nMsg, nValue);
		}
	}
}
shared_ptr<GameSound> Game::createSound(int32_t nSoundIdx, int32_t nTeam, int32_t nMate
										, FPoint oXYPos, double fZPos, bool bListenerRelative
										, double fVolume01, bool bLooping) noexcept
{
	if ((! m_bSoundEnabled) || (m_p0GameView == nullptr)) {
		return shared_ptr<GameSound>{};
	}
	return m_p0GameView->createSound(nSoundIdx, nTeam, nMate, oXYPos, fZPos, bListenerRelative, fVolume01, bLooping);
}
void Game::preloadSound(int32_t nSoundIdx) noexcept
{
	if ((! m_bSoundEnabled) || (m_p0GameView == nullptr)) {
		return;
	}
	m_p0GameView->preloadSound(nSoundIdx);
}
bool Game::removeSound(const shared_ptr<GameSound>& refSound) noexcept
{
	if ((! m_bSoundEnabled) || (m_p0GameView == nullptr)) {
		return false;
	}
	return m_p0GameView->removeSound(refSound);
}

} // namespace stmg
