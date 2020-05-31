/*
 * File:   stdview.cc
 *
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

#include "stdview.h"
#include "game.h"
#include "stdlevelview.h"

#include "levelshowthemewidget.h"
#include "theme.h"
#include "themewidget.h"
#include "themewidgetinteractive.h"
#include "themesound.h"
#include "themecontext.h"

#include <stmm-games/appconfig.h>
#include <stmm-games/apppreferences.h>
#include <stmm-games/gamewidget.h>
#include <stmm-games/layout.h>
#include <stmm-games/named.h>
#include <stmm-games/stdconfig.h>
#include <stmm-games/stdpreferences.h>
#include <stmm-games/util/namedindex.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/variant.h>

#include <stmm-input-au/sndfinishedevent.h>
#include <stmm-input-ev/devicemgmtevent.h>
#include <stmm-input-au/playbackcapability.h>

#include <stmm-input/event.h>
#include <stmm-input/xyevent.h>
#include <stmm-input/devicemanager.h>
#include <stmm-input/device.h>
#include <stmm-input/callifs.h>

#include <gtkmm.h>

//#include <iostream>
#include <vector>
#include <cassert>
#include <limits>
#include <algorithm>
#include <iterator>
#include <string>

namespace stmg { class GameSound; }

namespace stmg
{

static const std::string s_sDestructBoardTileAnimationName = "View:DestroyBoardTile";
static const std::string s_sDestructBrickAnimationName = "View:DestroyBlockBrick";

StdView::StdView() noexcept
: m_bAssignXYEventToActivePlayer(false)
, m_bSoundEnabled(false)
, m_bPerPlayerSound(false)
, m_bIsSubshows(false)
, m_fWHTileRatio(1.0)
, m_nTotLevels(0)
, m_nTotHumanPlayers(0)
, m_nTotPlayersWithPlayback(0)
, m_nTotAbsActiveSounds(0)
, m_nDestructBoardTileAnimation(-1)
, m_nDestructBrickAnimation(-1)
, m_nTileW(-1)
, m_nTileH(-1)
, m_nPixW(-1)
, m_bCheckNewPlaybackDevices(false)
, m_bFirstDrawAfterInitialization(false)
, m_nViewTick(-1)
, m_nTotViewTicks(-1)
, m_eViewStatus(VIEW_STATUS_INVALID)
{
}

std::pair<bool, NRect> StdView::reInit(const shared_ptr<Game>& refGame, const shared_ptr<StdPreferences>& refPrefs
										, const shared_ptr<Theme>& refTheme, NSize oPixSize
										, const Glib::RefPtr<Pango::Context>& refPaCtx) noexcept
{
//std::cout << "StdView[" << reinterpret_cast<int64_t>(this) <<  "]::reInit" << '\n';

	m_refGame = refGame;
	m_refPrefs = refPrefs;
	m_refTheme = refTheme;
	m_refStdConfig = m_refPrefs->getStdConfig();
	m_bSoundEnabled = m_refStdConfig->soundEnabled();
	m_refPaCtx = refPaCtx;

	m_refThemeCtx.reset();
	m_refThemeCtx = m_refTheme->createContext(oPixSize, true
											, m_refGame->getSoundScaleX(), m_refGame->getSoundScaleY(), m_refGame->getSoundScaleZ()
											, getFontContext());

	m_refGame->setGameView(this);

	m_fWHTileRatio = m_refTheme->getTileWHRatio(); //TODO also ask level
//std::cout << "StdView::reInit m_fWHTileRatio=" << m_fWHTileRatio << '\n';
	auto refRootTW = m_refTheme->createWidget(m_refGame->getLayout()->getRoot(), m_fWHTileRatio
											, m_refPaCtx);
	if (!refRootTW) {
		return std::make_pair(false, NRect{}); //-------------------------------
	}
//std::cout << "StdView::reInit root TW:" << reinterpret_cast<int64_t>(refRootTW.get()) << '\n';
	m_refViewLayout = std::make_unique<StdViewLayout>(m_refGame->getLayout(), refRootTW, m_refGame->prefs());
	if (!m_refViewLayout->isValid()) {
		return std::make_pair(false, NRect{}); //-------------------------------
	}
//std::cout << "StdView::reInit layout valid" << '\n';
	const bool bIsAllTeamsInOneLevel = m_refViewLayout->getGameLayout()->isAllTeamsInOneLevel();
	m_bIsSubshows = m_refViewLayout->getGameLayout()->isSubshows();
	assert(bIsAllTeamsInOneLevel == m_refGame->isAllTeamsInOneLevel());

	const int32_t nTotPlayers = m_refPrefs->getTotPlayers();

	m_nTotPlayersWithPlayback = 0;
	m_nUniqueHumanPrefPlayer = -1;
	m_nUniqueHumanPrefTeam = -1;
	m_nUniqueHumanMate = -1;
	if (m_bSoundEnabled) {
		const Variant oPerPlayerSound = m_refPrefs->getOptionValue(m_refStdConfig->getPerPlayerSoundOptionName());
		assert(oPerPlayerSound.getType() == Variant::TYPE_BOOL);
		m_bPerPlayerSound = oPerPlayerSound.getBool();
		//
		m_nTotHumanPlayers = m_refPrefs->getTotHumanPlayers();

		if (! m_bPerPlayerSound) {
			// global sound
			const int32_t nTotHumanTeams = m_refPrefs->getTotHumanTeams();
			const int32_t nTotTeams = m_refPrefs->getTotTeams();
			if (nTotHumanTeams == 1) {
				for (int32_t nTeam = 0; nTeam < nTotTeams; ++nTeam) {
					const auto& refTeam = m_refPrefs->getTeamFull(nTeam);
					if (! refTeam->isAI()) {
						m_nUniqueHumanPrefTeam = nTeam;
						break;
					}
				}
			}
			if (m_nTotHumanPlayers == 1) {
				for (int32_t nPlayer = 0; nPlayer < nTotPlayers; ++nPlayer) {
					const auto& refPlayer = m_refPrefs->getPlayerFull(nPlayer);
					if (! refPlayer->isAI()) {
						m_nUniqueHumanPrefPlayer = nPlayer;
						m_nUniqueHumanMate = refPlayer->getMate();
						break;
					}
				}
			}
		}
	} else {
		m_bPerPlayerSound = false;
		m_nTotHumanPlayers = -1;
	}
	// create level views if needed
	m_nTotLevels = m_refGame->getTotLevels();
	const int32_t nCurLevelViews = static_cast<int32_t>(m_aLevelViews.size());
	if (m_nTotLevels > nCurLevelViews) {
		m_aLevelViews.resize(m_nTotLevels);
		for (int32_t nLevel = nCurLevelViews; nLevel < m_nTotLevels; ++nLevel) {
			m_aLevelViews[nLevel] = std::make_unique<StdLevelView>();
		}
	}
	// initialize level views
	const auto& aAllLSW = m_refViewLayout->getLevelShowThemeWidgets();
	for (int32_t nLevel = 0; nLevel < m_nTotLevels; ++nLevel) {
		auto& refLV = m_aLevelViews[nLevel];
		std::vector< shared_ptr<LevelShowThemeWidget> > aLSW;
		if (bIsAllTeamsInOneLevel) {
			if (m_bIsSubshows) {
				for (int32_t nPlayer = 0; nPlayer < nTotPlayers; ++nPlayer) {
					auto& refPrefPlayer = m_refPrefs->getPlayer(nPlayer);
					const int32_t nTeam = refPrefPlayer->getTeam()->get();
					const int32_t nMate = refPrefPlayer->getMate();
					auto itFind = std::find_if(aAllLSW.begin(), aAllLSW.end(),
						[&](const shared_ptr<LevelShowThemeWidget>& refLSW)
						{
							auto& refModel = refLSW->getModel();
							return (refModel->getTeam() == nTeam)
									&& (refModel->getMate() == nMate);
						});
					assert(itFind != aAllLSW.end());
					aLSW.push_back(*itFind);
				}
			} else {
				assert(aAllLSW.size() == 1);
				#ifndef NDEBUG
				auto& refModel = aAllLSW[0]->getModel();
				#endif //NDEBUG
				assert(refModel->getTeam() < 0);
				assert(refModel->getMate() < 0);
				aLSW.push_back(aAllLSW[0]);
			}
		} else {
			if (m_bIsSubshows) {
				auto& refPrefTeam = m_refPrefs->getTeam(nLevel);
				const int32_t nTotMates = refPrefTeam->getTotMates();
				for (int32_t nMate = 0; nMate < nTotMates; ++nMate) {
					auto itFind = std::find_if(aAllLSW.begin(), aAllLSW.end(),
						[&](const shared_ptr<LevelShowThemeWidget>& refLSW)
						{
							auto& refModel = refLSW->getModel();
							return (refModel->getTeam() == nLevel)
									&& (refModel->getMate() == nMate);
						});
					assert(itFind != aAllLSW.end());
					aLSW.push_back(*itFind);
				}
			} else {
				assert(static_cast<int32_t>(aAllLSW.size()) == m_nTotLevels);
				#ifndef NDEBUG
				auto& refModel = aAllLSW[nLevel]->getModel();
				#endif //NDEBUG
				assert(refModel->getTeam() == nLevel);
				assert(refModel->getMate() < 0);
				aLSW.push_back(aAllLSW[nLevel]);
			}
		}
		refLV->reInit(m_bIsSubshows, aLSW, m_refGame, nLevel, refTheme, m_refThemeCtx, this);
	}

	m_bAssignXYEventToActivePlayer = m_refStdConfig->isEventAssignedToActivePlayer();

	m_aOpenSequences.clear();

	m_nViewTick = -1;
	m_nTotViewTicks = -1;
	m_eViewStatus = VIEW_STATUS_INVALID;
	//m_oTickTileAnis.clear();

	m_nDestructBoardTileAnimation = m_refTheme->getNamed().animations().getIndex(s_sDestructBoardTileAnimationName);
	m_nDestructBrickAnimation = m_refTheme->getNamed().animations().getIndex(s_sDestructBrickAnimationName);

	const NRect oActive = setSize(oPixSize, true);
//#ifndef NDEBUG
//std::cout << "View Layout:" << '\n'; // DO NOT REMOVE THIS
//m_refViewLayout->dump();
////std::cout << '\n';
//#endif //NDEBUG
	return std::make_pair(true, oActive);
}
void StdView::checkNewPlaybackDevices() noexcept
{
	if (m_bPerPlayerSound) {
		const int32_t nTotPlayers = m_refPrefs->getTotPlayers();
		for (int32_t nPlayer = 0; nPlayer < nTotPlayers ; ++nPlayer) {
			auto& refPlayerPlayback = m_aPlayerPlaybacks[nPlayer];
			if (refPlayerPlayback) {
				// already has a capability
				continue; // for nPlayer
			}
			const shared_ptr<StdPreferences::Player>& refPlayer = m_refPrefs->getPlayerFull(nPlayer);
			const bool bAssigned = refPlayer->getCapability(refPlayerPlayback);
			if (! bAssigned) {
				continue; // for nPlayer
			}
			m_aPlayerPlaybackDatas[nPlayer].m_oCurrentListenerPos = FPoint{std::numeric_limits<float>::lowest() / 2, std::numeric_limits<float>::lowest() / 2};
			++m_nTotPlayersWithPlayback;
		}
	} else {
		auto& refGlobalPlayback = m_aPlayerPlaybacks[0];
		if (! refGlobalPlayback) {
			refGlobalPlayback = m_refStdConfig->getDefaultPlayback();
			m_aPlayerPlaybackDatas[0].m_oCurrentListenerPos = FPoint{std::numeric_limits<float>::lowest() / 2, std::numeric_limits<float>::lowest() / 2};
		}
	}
}
void StdView::gameStarted() noexcept
{
	if (! m_bSoundEnabled) {
		return;
	}
	//
	m_aPlayerPlaybacks.clear();
	m_aPlayerPlaybacks.resize(m_bPerPlayerSound ? m_refPrefs->getTotPlayers() : 1);
	m_aPlayerPlaybackDatas.clear();
	m_aPlayerPlaybackDatas.resize(m_aPlayerPlaybacks.size());
	//
	assert(m_nTotAbsActiveSounds == 0);
	assert(m_aActiveSounds.empty());

	const auto refDM = m_refStdConfig->getDeviceManager();
	if (! m_refSndFinishedListener) {
		m_refSndFinishedListener = std::make_shared<stmi::EventListener>([&](const std::shared_ptr<stmi::Event>& refEvent)
		{
			handleSndFinishedEvent(refEvent);
		});
		m_refSndFinishedCallIf = std::make_shared<stmi::CallIfEventClass>(stmi::SndFinishedEvent::getClass());
	}
	refDM->addEventListener(m_refSndFinishedListener, m_refSndFinishedCallIf);
	//
	if (! m_refDeviceMgmtListener) {
		m_refDeviceMgmtListener = std::make_shared<stmi::EventListener>([&](const std::shared_ptr<stmi::Event>& refEvent)
		{
			handleDeviceMgmtEvent(refEvent);
		});
		m_refDeviceMgmtCallIf = std::make_shared<stmi::CallIfEventClass>(stmi::DeviceMgmtEvent::getClass());
	}
	refDM->addEventListener(m_refDeviceMgmtListener, m_refDeviceMgmtCallIf);

	checkNewPlaybackDevices();
}
void StdView::gamePaused() noexcept
{
	if (m_bSoundEnabled) {
		// pause all sounds
		for (auto& refPBCapa : m_aPlayerPlaybacks) {
			if (refPBCapa) {
				refPBCapa->pauseDevice();
			}
		}
	}
}
void StdView::gameResumed() noexcept
{
	if (m_bSoundEnabled) {
		// resume all sounds
		for (auto& refPBCapa : m_aPlayerPlaybacks) {
			if (refPBCapa) {
				refPBCapa->resumeDevice();
			}
		}
	}
}
void StdView::gameEnded() noexcept
{
	if (! m_bSoundEnabled) {
		return;
	}
//std::cout << "StdView::gameEnded" << '\n';
	const auto& refDM = m_refStdConfig->getDeviceManager();
	refDM->removeEventListener(m_refDeviceMgmtListener, false);
	refDM->removeEventListener(m_refSndFinishedListener, false);
	// stop all sounds, remove devices
	for (auto& oSoundData : m_aActiveSounds) {
		auto& refThemeSound = oSoundData.m_refThemeSound;
		refThemeSound->stop();
	}
	m_aActiveSounds.clear();
	m_nTotAbsActiveSounds = 0;
	m_aPlayerPlaybacks.clear();
}
void StdView::handleDeviceMgmtEvent(const shared_ptr<stmi::Event>& refDeviceMgmtEvent) noexcept
{
	if (! m_bSoundEnabled) {
		return;
	}
	auto p0DeviceMgmtEvent = static_cast<stmi::DeviceMgmtEvent*>(refDeviceMgmtEvent.get());
	const stmi::DeviceMgmtEvent::DEVICE_MGMT_TYPE eType = p0DeviceMgmtEvent->getDeviceMgmtType();
	const bool bPerPlayerSound = m_bPerPlayerSound;
	if (eType != stmi::DeviceMgmtEvent::DEVICE_MGMT_REMOVED) {
		// stmi::DeviceMgmtEvent::DEVICE_MGMT_ADDED || stmi::DeviceMgmtEvent::DEVICE_MGMT_CHANGED
		if ((bPerPlayerSound && (m_nTotPlayersWithPlayback < m_nTotHumanPlayers)) || (! bPerPlayerSound)) {
			m_bCheckNewPlaybackDevices = true;
		}
		return; //--------------------------------------------------------------
	}
	const int32_t nCapaId = p0DeviceMgmtEvent->getCapabilityId();
	// remove active sounds using this capability
	m_aActiveSounds.erase(std::remove_if(m_aActiveSounds.begin(), m_aActiveSounds.end(), [&](auto& oSoundData)
	{
		if (! bPerPlayerSound) {
			if (oSoundData.m_nUniqueCapabilityId != nCapaId) {
				return false;
			}
		}
		const bool bFinished = oSoundData.m_refThemeSound->removeCapability(nCapaId);
		if (bFinished) {
			if (! oSoundData.m_bIsListenerRelative) {
				--m_nTotAbsActiveSounds;
			}
		}
		return bFinished;
	}), m_aActiveSounds.end());
	// remove from assigned devices
	auto itFind = std::find_if(m_aPlayerPlaybacks.begin(), m_aPlayerPlaybacks.end(), [&](const shared_ptr<stmi::PlaybackCapability>& refCapa)
	{
		return (refCapa && (refCapa->getId() == nCapaId));
	});
	if (itFind == m_aPlayerPlaybacks.end()) {
		return; //--------------------------------------------------------------
	}
	const auto nCapaIdx = std::distance(m_aPlayerPlaybacks.begin(), itFind);
	m_aPlayerPlaybacks[nCapaIdx].reset();
	if (bPerPlayerSound) {
		--m_nTotPlayersWithPlayback;
	}
}
void StdView::handleSndFinishedEvent(const shared_ptr<stmi::Event>& refSndFinishedEvent) noexcept
{
//std::cout << "StdView::handleSndFinishedEvent  " << '\n';
	if (! m_bSoundEnabled) {
		return;
	}
	assert(dynamic_cast<stmi::SndFinishedEvent*>(refSndFinishedEvent.get()) != nullptr);
	auto p0SndFinishedEvent = static_cast<stmi::SndFinishedEvent*>(refSndFinishedEvent.get());
	const int32_t nCapaId = p0SndFinishedEvent->getCapabilityId();
	const int32_t nSoundId = p0SndFinishedEvent->getSoundId();
	const bool bPerPlayerSound = m_bPerPlayerSound;
	const int32_t nTotActiveSounds = static_cast<int32_t>(m_aActiveSounds.size());
	int32_t nCurSound = 0;
	for ( ; nCurSound < nTotActiveSounds; ++nCurSound) {
		auto& oSoundData = m_aActiveSounds[nCurSound];
		if (! bPerPlayerSound) {
			if (oSoundData.m_nUniqueCapabilityId != nCapaId) {
				continue; // for ---
			}
		}
		auto& refThemeSound = oSoundData.m_refThemeSound;
		const bool bFinished = refThemeSound->finishedSound(nCapaId, nSoundId);
		if (bFinished) {
			if (! oSoundData.m_bIsListenerRelative) {
				--m_nTotAbsActiveSounds;
			}
			break; // for ---
		}
	}
	if (nCurSound == nTotActiveSounds) {
		return; //--------------------------------------------------------------
	}
	// remove
	if (nCurSound < nTotActiveSounds - 1) {
		m_aActiveSounds[nCurSound] = std::move(m_aActiveSounds[nTotActiveSounds - 1]);
	}
	m_aActiveSounds.pop_back();
}

std::pair<bool, NRect> StdView::resizeCanvas(NSize oPixSize) noexcept
{
	if (!isReady()) {
		return std::make_pair(false, NRect{});
	}
	if ((m_nPixW == oPixSize.m_nW) && (m_nPixH == oPixSize.m_nH)) {
		return std::make_pair(false, NRect{});
	}
	NRect oBoundingBox = setSize(oPixSize, false);
	//// when this function is called the game is either not running or is paused!!!
	//// => sync is not called
	return std::make_pair(true, oBoundingBox);
}
NRect StdView::setSize(NSize oPixSize, bool bInit) noexcept
{
//std::cout << "StdView[" << reinterpret_cast<int64_t>(this) <<  "]";
//std::cout << "::setSize({" << oPixSize.m_nW << "," << oPixSize.m_nH << "}, bInit=" << bInit << ")" << '\n';
	assert(isReady());

	if (bInit) {
		m_bFirstDrawAfterInitialization = true;
	}
	m_nPixW = oPixSize.m_nW;
	m_nPixH = oPixSize.m_nH;

	NRect oActiveRect = calcLayout();

	const int32_t nTileW = m_nTileW;
	const int32_t nTileH = m_nTileH;

	m_refThemeCtx->setTileSize(NSize{nTileW, nTileH});

//std::cout << "StdView::setSize m_nTileW: " << m_nTileW << ", m_nTileH: "<< m_nTileH << '\n';
	for (int32_t nLevel = 0; nLevel < m_nTotLevels; ++nLevel) {
		m_aLevelViews[nLevel]->onSizeChanged(nTileW, nTileH);
	}
	return oActiveRect;
//std::cout << "StdView::setSize end" << '\n';
}
NRect StdView::calcLayout() noexcept
{
//std::cout << "StdView::calcLayout()" << '\n';
	NSize oBestSize = m_refTheme->getBestTileSize(0);

//std::cout << "StdView::calcLayout() 2 m_fWHTileRatio=" << m_fWHTileRatio << " oBestSize.m_nW=" << oBestSize.m_nW << '\n';
	assert(m_refViewLayout);
	int32_t nLayoutConfig;
	m_refViewLayout->calcLayout(m_nPixW, m_nPixH, oBestSize.m_nW, m_fWHTileRatio, m_nTileW, m_nTileH, nLayoutConfig);

	bool bBRSet = false;
	NRect oBRect;
	const auto& aCanChange = m_refViewLayout->getCanChangeThemeWidgets();
	for (auto& refTW : aCanChange) {
		const NPoint oTWPos = refTW->getPos();
		const NSize oTWSize = refTW->getSize();
		NRect oTWRect;
		oTWRect.m_nX = oTWPos.m_nX;
		oTWRect.m_nY = oTWPos.m_nY;
		oTWRect.m_nW = oTWSize.m_nW;
		oTWRect.m_nH = oTWSize.m_nH;
		if (!bBRSet) {
			oBRect = oTWRect;
			bBRSet = true;
		} else {
			oBRect = NRect::boundingRect(oBRect, oTWRect);
		}
	}
	const auto& aLSs = m_refViewLayout->getLevelShowThemeWidgets();
	for (auto& refTW : aLSs) {
		const NPoint oTWPos = refTW->getPos();
		const NSize oTWSize = refTW->getSize();
		NRect oTWRect;
		oTWRect.m_nX = oTWPos.m_nX;
		oTWRect.m_nY = oTWPos.m_nY;
		oTWRect.m_nW = oTWSize.m_nW;
		oTWRect.m_nH = oTWSize.m_nH;
		if (!bBRSet) {
			oBRect = oTWRect;
			bBRSet = true;
		} else {
			oBRect = NRect::boundingRect(oBRect, oTWRect);
		}
	}
	return oBRect;
}
void StdView::handleXYEvent(const shared_ptr<stmi::Event>& refXYEvent) noexcept
{
//std::cout << "StdView::handleXYEvent" << '\n';
	const int32_t nCapabilityId = refXYEvent->getCapabilityId();

	auto p0Event = refXYEvent.get();
	assert(dynamic_cast<stmi::XYEvent*>(p0Event) != nullptr);
	auto p0XYEvent = static_cast<stmi::XYEvent*>(p0Event);
	//
	const auto nGrabId = p0XYEvent->getXYGrabId();
	const stmi::XYEvent::XY_GRAB_INPUT_TYPE eType = p0XYEvent->getXYGrabType();
	auto& aInteractiveTWs = m_refViewLayout->getInteractiveWidgets();
	auto itFind = std::find_if(m_aOpenSequences.begin(), m_aOpenSequences.end(), [&](const OpenSequence& oSeq)
			{
				return (oSeq.m_nGrabId == nGrabId) && (oSeq.m_nCapabilityId == nCapabilityId);
			});
	if (itFind != m_aOpenSequences.end()) {
		// it is an ongoing sequence
		OpenSequence& oSeq = *itFind;
		aInteractiveTWs[oSeq.m_nInteractiveIdx]->handleXYInput(refXYEvent, oSeq.m_nTeam, oSeq.m_nMate);
		const bool bCloseSeq = ((eType == stmi::XYEvent::XY_UNGRAB) || (eType == stmi::XYEvent::XY_UNGRAB_CANCEL));
		if (bCloseSeq) {
//std::cout << "StdView::handleXYEvent  XY_UNGRAB X=" << p0XYEvent->getX() << "  Y=" << p0XYEvent->getY() << '\n';
			const int32_t nIdx = std::distance(m_aOpenSequences.begin(), itFind);
			const int32_t nLastIdx = static_cast<int32_t>(m_aOpenSequences.size()) - 1;
			if (nIdx < nLastIdx) {
				m_aOpenSequences[nIdx] = std::move(m_aOpenSequences[nLastIdx]);
			}
			m_aOpenSequences.pop_back();
		}
		return; //--------------------------------------------------------------
	}
	;
	if (eType == stmi::XYEvent::XY_GRAB) {
//std::cout << "StdView::handleXYEvent  XY_GRAB X=" << p0XYEvent->getX() << "  Y=" << p0XYEvent->getY() << '\n';
		// new input sequence
		// Note that multiple sequences can be opened for the same interactive widget
		// It's up to the widget itself to possibly limit the sequences
		m_aOpenSequences.emplace_back(nCapabilityId);
		OpenSequence& oOpenSequence = m_aOpenSequences.back();
		oOpenSequence.m_nGrabId = nGrabId;
		const bool bSent = handleXYEvent(refXYEvent, p0XYEvent, nCapabilityId
										, aInteractiveTWs
										, oOpenSequence.m_nTeam, oOpenSequence.m_nMate
										, oOpenSequence.m_nInteractiveIdx);
		if (!bSent) {
			m_aOpenSequences.pop_back();
			return; //----------------------------------------------------------
		}
	} else if (eType == stmi::XYEvent::XY_HOVER) {
		int32_t nResTeam = -1;
		int32_t nResMate = -1;
		int32_t nResTWIdx = std::numeric_limits<int32_t>::min();
		handleXYEvent(refXYEvent, p0XYEvent, nCapabilityId
					, aInteractiveTWs
					, nResTeam, nResMate, nResTWIdx);
	}
}
bool StdView::handleXYEvent(const shared_ptr<stmi::Event>& refXYEvent, stmi::XYEvent* p0XYEvent, int32_t nCapabilityId
							, const std::vector< shared_ptr<ThemeWidgetInteractive> >& aInteractiveTWs
							, int32_t& nResTeam, int32_t& nResMate, int32_t& nInteractiveIdx) noexcept
{
//if (p0XYEvent->getXYGrabType() != stmi::XYEvent::XY_HOVER) {
//std::cout << "StdView::handleXYEvent " << p0XYEvent->getXYGrabId() << '\n';
//}

	// look if the capability that generated event is assigned to a player
	const bool bAssigned = m_refPrefs->getCapabilityPlayer(nCapabilityId, nResTeam, nResMate);

	// look if pressed within an interactive widget
	const int32_t nX = p0XYEvent->getX();
	const int32_t nY = p0XYEvent->getY();
	if (bAssigned) {
		// use the context of the capability player
		nInteractiveIdx = m_refViewLayout->getInteractiveWidget(nX, nY, nResTeam, nResMate);
	} else {
		nInteractiveIdx = m_refViewLayout->getInteractiveWidget(nX, nY);
	}
	if (nInteractiveIdx < 0) {
//if (p0XYEvent->getXYGrabType() != stmi::XYEvent::XY_HOVER) {
//std::cout << "StdView::handleXYEvent NOT INTERACTIVE  bAssigned=" << bAssigned << " nX=" << nX << " nY=" << nY << '\n';
//}
		return false; //--------------------------------------------------------
	}
	auto& refITW = aInteractiveTWs[nInteractiveIdx];

	if (!bAssigned) {
		assert(nResTeam == -1);
		assert(nResMate == -1);
		// Maybe the widget can determine the target player
		const ThemeWidget* p0TW = refITW->getThemeWidget();
		auto& refGameWidget = p0TW->getModel();
		nResTeam = refGameWidget->getTeam();
		nResMate = refGameWidget->getMate();
		if ((nResMate < 0) && m_bAssignXYEventToActivePlayer) {
			// The widget might define the team, but not the player
			if (nResTeam < 0) {
				if (m_nUniqueHumanPrefTeam >= 0) {
					nResTeam = m_nUniqueHumanPrefTeam;
					nResMate = m_nUniqueHumanMate;
				}
			} else if (nResTeam == m_nUniqueHumanPrefTeam) {
				nResMate = m_nUniqueHumanMate;
			}
			if (nResMate < 0) {
				// At least two human players, see if one is active
				// (is controlling a level block)
				m_refGame->getUniqueActiveHumanPlayer(nResTeam, nResMate);
			}
		}
	}
//if (p0XYEvent->getXYGrabType() != stmi::XYEvent::XY_HOVER) {
//std::cout << "StdView::handleXYEvent bAssigned = " << bAssigned << "  m_bAssignXYEventToActivePlayer=" << m_bAssignXYEventToActivePlayer << '\n';
//std::cout << "StdView::handleXYEvent nResMate=" << nResMate << "  nResTeam=" << nResTeam << '\n';
//}
	return refITW->handleXYInput(refXYEvent, nResTeam, nResMate);
}
void StdView::lazilySetListenerPosIfNecessary(bool bListenerRelative) noexcept
{
	if (bListenerRelative || (m_nTotAbsActiveSounds > 0)) {
		return;
	}
	// This is the first non listener relative sound:
	// set listener position for all capabilities before playing it
	for (int32_t nLevel = 0; nLevel < m_nTotLevels; ++nLevel) {
		auto& refLV = m_aLevelViews[nLevel];
		if (m_bIsSubshows) {
			refLV->setSoundListenersToSubshowCenter();
		} else {
			refLV->setSoundListenersToShowCenter();
		}
	}
}
shared_ptr<GameSound> StdView::createSound(int32_t nSoundIdx, int32_t nTeam, int32_t nMate
											, FPoint oXYPos, double fZPos, bool bListenerRelative
											, double fVolume01, bool bLooping) noexcept
{
//std::cout << "StdView::createSound nSoundIdx = " << nSoundIdx << "  team=" << nTeam << "  mate=" << nMate << "  bListenerRelative=" << bListenerRelative << '\n';
	shared_ptr<ThemeSound> refThemeSound;
	if (m_bPerPlayerSound && (m_nTotPlayersWithPlayback <= 0)) {
		return refThemeSound; //------------------------------------------------
	}
	int32_t nUniqueCapabilityId = -1;
	if (m_bPerPlayerSound) {
		if (nTeam < 0) {
			// heard by all players
			// Note that if the sound is not listener relative it is placed
			// in possibly many boards at the (same) given position
			lazilySetListenerPosIfNecessary(bListenerRelative);
			refThemeSound = m_refThemeCtx->createSound(nSoundIdx, m_aPlayerPlaybacks, oXYPos, fZPos, bListenerRelative, fVolume01, bLooping);
		} else {
			// only heard by a specific team or player
			std::vector< shared_ptr<stmi::PlaybackCapability> > aCapas;
			const int32_t nTotPlayers = m_refPrefs->getTotPlayers();
			for (int32_t nPlayer = 0; nPlayer < nTotPlayers ; ++nPlayer) {
				auto& refPlayerCapa = m_aPlayerPlaybacks[nPlayer];
				if (! refPlayerCapa) {
					// this player has no capability, can't create sound
					continue; // for nPlayer
				}
				const shared_ptr<StdPreferences::Player>& refPlayer = m_refPrefs->getPlayerFull(nPlayer);
				if (refPlayer->getTeamFull()->get() != nTeam) {
					// wrong team
					continue; // for nPlayer
				}
				if ((nMate >= 0) && (refPlayer->getMate() != nMate)) {
					// wrong mate
					continue; // for nPlayer
				}
				aCapas.push_back(refPlayerCapa);
			}
			if (! aCapas.empty()) {
				lazilySetListenerPosIfNecessary(bListenerRelative);
				refThemeSound = m_refThemeCtx->createSound(nSoundIdx, aCapas, oXYPos, fZPos, bListenerRelative, fVolume01, bLooping);
			}
		}
 	} else {
		assert(m_aPlayerPlaybacks.size() == 1);
		if (m_aPlayerPlaybacks[0]) {
			// global sound device available
			if (nTeam < 0) {
				if (m_bIsSubshows && (! bListenerRelative) && (m_nUniqueHumanPrefPlayer >= 0)) {
					// This is a case that is very complicated to make somewhat right,
					// so just transform the board sound to a listener relative sound at listener position.
					// The thought behind it is: if a board has more than one (human) subshow
					// it's not clear where a board sound is placed relative to the (global) listener since
					// for one subshow it might be to the left and for the other to the right
					// and we do not want to play the same sound twice. So the compromise is:
					// just play it at listener position even though the sound might originate
					// in a part of the board that isn't even shown by the subshows!
					bListenerRelative = true;
					oXYPos = FPoint{0.0, 0.0};
				}
//std::cout << "StdView::createSound Global for all  m_nTotAbsActiveSounds=" << m_nTotAbsActiveSounds << '\n';
				lazilySetListenerPosIfNecessary(bListenerRelative);

				refThemeSound = m_refThemeCtx->createSound(nSoundIdx, m_aPlayerPlaybacks, oXYPos, fZPos, bListenerRelative, fVolume01, bLooping);
				nUniqueCapabilityId = m_aPlayerPlaybacks[0]->getId();
			} else {
				// team and mate specific sound are not played by global device,
				// unless there is only one non AI team or only one non AI player and it happens
				// to be the one selected by nTeam and nMate.
				if ((m_nUniqueHumanPrefTeam == nTeam) && ((m_nUniqueHumanMate == nMate) || (nMate < 0))){
//std::cout << "StdView::createSound global for team or mate  m_nTotAbsActiveSounds=" << m_nTotAbsActiveSounds << '\n';
					lazilySetListenerPosIfNecessary(bListenerRelative);
					// Note:
					refThemeSound = m_refThemeCtx->createSound(nSoundIdx, m_aPlayerPlaybacks, oXYPos, fZPos, bListenerRelative, fVolume01, bLooping);
					nUniqueCapabilityId = m_aPlayerPlaybacks[0]->getId();
				}
			}
		}
	}
	if (refThemeSound) {
		SoundData oSoundData;
		oSoundData.m_refThemeSound = refThemeSound;
		oSoundData.m_nUniqueCapabilityId = nUniqueCapabilityId;
		oSoundData.m_bIsListenerRelative = bListenerRelative;
		m_aActiveSounds.push_back(std::move(oSoundData));
		if (! bListenerRelative) {
			++m_nTotAbsActiveSounds;
		}
	}
	return refThemeSound;
}
void StdView::preloadSound(int32_t nSoundIdx) noexcept
{
	m_refThemeCtx->preloadSound(nSoundIdx, m_aPlayerPlaybacks);
}
bool StdView::removeSound(const shared_ptr<GameSound>& refSound) noexcept
{
	assert(refSound);
	auto itFind = std::find_if(m_aActiveSounds.begin(), m_aActiveSounds.end(), [&](const SoundData& oSoundData)
	{
		return (oSoundData.m_refThemeSound == refSound);
	});
	if (itFind == m_aActiveSounds.end()) {
		return false; //--------------------------------------------------------
	}
	SoundData& oSoundData = *itFind;
	oSoundData.m_refThemeSound->stop();
	// remove
	const int32_t nIdx = std::distance(m_aActiveSounds.begin(), itFind);
	const auto nTotActiveSounds = static_cast<int32_t>(m_aActiveSounds.size());
	if (nIdx < nTotActiveSounds - 1) {
		m_aActiveSounds[nIdx] = std::move(m_aActiveSounds[nTotActiveSounds - 1]);
	}
	m_aActiveSounds.pop_back();
	return false;
}
bool StdView::isReady() const noexcept
{
	return m_refTheme.operator bool();
}

void StdView::beforeGameTick() noexcept
{
//std::cout << "StdView::beforeGameTick()" << '\n';
	m_eViewStatus = VIEW_STATUS_GAME_TICK;
	for (int32_t nLevel = 0; nLevel < m_nTotLevels; ++nLevel) {
		m_aLevelViews[nLevel]->beforeGameTick();
	}
}
void StdView::sync(double
					#ifndef NDEBUG
					fViewInterval
					#endif //NDEBUG
					, int32_t nTotViewTicks) noexcept
{
	assert(isReady());
	assert(nTotViewTicks > 0);
	assert(fViewInterval > 0.0);

//std::cout << "StdView::sync  nViewInterval=" << nViewInterval << "  nTotViewTicks=" << nTotViewTicks << '\n';
	m_eViewStatus = VIEW_STATUS_SYNC;
	m_nViewTick = 0;
	m_nTotViewTicks = nTotViewTicks;

	if (m_bCheckNewPlaybackDevices) {
		// implies m_bSoundEnabled
		checkNewPlaybackDevices();
		m_bCheckNewPlaybackDevices = false;
	}
}

void StdView::drawStep(int32_t
						#ifndef NDEBUG
						nViewTick
						#endif //NDEBUG
						, const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	if (m_eViewStatus == VIEW_STATUS_ASYNC_REDRAW) {
//std::cout << "StdView::drawStep skip (because of preceding AsyncRedraw)" << '\n';
		return;
	}
	m_eViewStatus = VIEW_STATUS_VIEW_TICK;
//#ifndef NDEBUG
//if (! (m_nViewTick == nViewTick)) {
//std::cout << "StdView::drawStep ERROR m_nViewTick=" << m_nViewTick << "  nViewTick=" << nViewTick << "  m_nTotViewTicks=" << m_nTotViewTicks << '\n';
//}
//#endif //NDEBUG
	assert(m_nViewTick == nViewTick);
	drawStep(refCc);
}
void StdView::drawStep(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	assert(isReady());

	assert(m_nTotViewTicks > 0);
	assert((m_nViewTick >= 0) && (m_nViewTick < m_nTotViewTicks));

	const bool bAsyncRedraw = (m_eViewStatus == VIEW_STATUS_ASYNC_REDRAW);
//std::cout << "StdView::drawStep m_nViewTick=" << m_nViewTick << "  m_nTotViewTicks=" << m_nTotViewTicks << "  bAsyncRedraw=" << bAsyncRedraw << '\n';
//std::cout << "                  m_bFirstDrawAfterInitialization=" << m_bFirstDrawAfterInitialization << '\n';

	if (m_bFirstDrawAfterInitialization || bAsyncRedraw) {
		// redraw all widgets
//std::cout << "StdView::drawStep draw" << '\n';
		m_refViewLayout->draw(refCc);
		m_bFirstDrawAfterInitialization = false;
	} else if (m_nViewTick == 0) {
//std::cout << "StdView::drawStep drawIfChanged" << '\n';
		// view tick 0 cannot be skipped:
		// redraw widgets changed in game tick
		m_refViewLayout->drawIfChanged(refCc);
	}
	for (int32_t nLevel = 0; nLevel < m_nTotLevels; ++nLevel) {
		m_aLevelViews[nLevel]->drawStepToBuffers(m_nViewTick, m_nTotViewTicks, bAsyncRedraw);
	}
	for (int32_t nLevel = 0; nLevel < m_nTotLevels; ++nLevel) {
		m_aLevelViews[nLevel]->drawBuffers(refCc);
	}

	++m_nViewTick;
	assert(m_nViewTick <= m_nTotViewTicks);
}
void StdView::redraw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
//std::cout << "StdView::redraw isReady=" << isReady() << '\n';
	if (!isReady()) {
		return; //--------------------------------------------------------------
	}

	m_eViewStatus = VIEW_STATUS_ASYNC_REDRAW;

	if (m_nTotViewTicks <= 0) {
		return; //--------------------------------------------------------------
	}
	m_nViewTick = m_nTotViewTicks - 1;

	drawStep(refCc);
}
#ifndef NDEBUG
void StdView::dump(bool /*bTickTileAnis*/) const noexcept
{
	//	if (bTickTileAnis) { //TODO
	//		std::cout << "StdView::dump()  TickTileAnis (nX,nY)" << '\n';
	//		for (auto& nXY : m_oTickTileAnis) {
	//			const int32_t nX = Coords::getXFromXY(nXY);
	//			const int32_t nY = Coords::getYFromXY(nXY);
	//			std::cout << "    (" << nX << "," << nY << ")";
	//			std::cout << '\n';
	//		}
	//	}
}
#endif //NDEBUG

} // namespace stmg
