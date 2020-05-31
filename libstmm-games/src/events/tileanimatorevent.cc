/*
 * File:   tileanimatorevent.cc
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

#include "events/tileanimatorevent.h"

#include "gameproxy.h"
#include "level.h"
#include "levelblock.h"
#include "named.h"
#include "tile.h"
#include "util/coords.h"
#include "util/namedindex.h"
#include "utile/tilerect.h"
#include "util/util.h"

#include <vector>
#include <cassert>
#include <limits>
#include <algorithm>
//#include <iostream>
#include <string>
#include <utility>

namespace stmg { class TileCoords; }

namespace stmg
{

const int32_t TileAnimatorEvent::s_nMaxTry = 3;

TileAnimatorEvent::TileAnimatorEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_oInit(std::move(oInit))
{
	commonInit();
}

void TileAnimatorEvent::reInit(Init&& oInit) noexcept
{
//std::cout << "TileAnimatorEvent::init" << '\n';
//std::cout << "                  nInitialWaitFrom=" << nInitialWaitFrom << "   nInitialWaitTo=" << nInitialWaitTo << '\n';
//std::cout << "                  nDurationFrom=" << nDurationFrom << "   nDurationTo=" << nDurationTo << '\n';
//std::cout << "                  nPauseFrom=" << nPauseFrom << "   nPauseTo=" << nPauseTo << '\n';
//std::cout << "                  nTotCountFrom=" << nTotCountFrom << "   nTotCountTo=" << nTotCountTo << '\n';
	Event::reInit(std::move(oInit));
	m_oInit = std::move(oInit);

	commonInit();
}
void TileAnimatorEvent::commonInit() noexcept
{
	m_nBoardWidth = level().boardWidth();
	m_nBoardHeight = level().boardHeight();

	m_eState = TILE_ANIMATOR_STATE_ACTIVATE;
	m_nCounter = 0;
	m_nAnis = -1;
	m_nStartedRunning = 0;

	if (m_oInit.m_bDoBoard) {
		assert((m_oInit.m_oArea.m_nX >= 0) && (m_oInit.m_oArea.m_nY >= 0));
		assert((m_oInit.m_oArea.m_nX + 1 <= m_nBoardWidth) && (m_oInit.m_oArea.m_nY + 1 <= m_nBoardHeight));
		if (m_oInit.m_oArea.m_nW <= 0) {
			m_oInit.m_oArea.m_nW = m_nBoardWidth - m_oInit.m_oArea.m_nX;
		}
		if (m_oInit.m_oArea.m_nH <= 0) {
			m_oInit.m_oArea.m_nH = m_nBoardHeight - m_oInit.m_oArea.m_nY;
		}
		assert((m_oInit.m_oArea.m_nW > 0) && (m_oInit.m_oArea.m_nH > 0) 
			&& (m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW <= m_nBoardWidth) && (m_oInit.m_oArea.m_nY + m_oInit.m_oArea.m_nH <= m_nBoardHeight));
	}
	assert((m_oInit.m_nRepeat > 0) || (m_oInit.m_nRepeat == -1));
	assert((m_oInit.m_nAniNameIdx >= 0) && (m_oInit.m_nAniNameIdx < level().getNamed().tileAnis().size()));
	assert(m_oInit.m_bDoBoard || m_oInit.m_bDoBlocks);
	assert((m_oInit.m_oInitialWait.m_oTicks.m_nFrom >= 0) && (m_oInit.m_oInitialWait.m_oTicks.m_nFrom <= m_oInit.m_oInitialWait.m_oTicks.m_nTo));
	assert((m_oInit.m_oInitialWait.m_oMillisec.m_nFrom >= 0) && (m_oInit.m_oInitialWait.m_oMillisec.m_nFrom <= m_oInit.m_oInitialWait.m_oMillisec.m_nTo));
	assert((m_oInit.m_oDuration.m_oTicks.m_nFrom >= 0) && (m_oInit.m_oDuration.m_oTicks.m_nFrom <= m_oInit.m_oDuration.m_oTicks.m_nTo));
	assert((m_oInit.m_oDuration.m_oMillisec.m_nFrom >= 0) && (m_oInit.m_oDuration.m_oMillisec.m_nFrom <= m_oInit.m_oDuration.m_oMillisec.m_nTo));
	assert((m_oInit.m_oPause.m_oTicks.m_nFrom >= 0) && (m_oInit.m_oPause.m_oTicks.m_nFrom <= m_oInit.m_oPause.m_oTicks.m_nTo));
	assert((m_oInit.m_oPause.m_oMillisec.m_nFrom >= 0) && (m_oInit.m_oPause.m_oMillisec.m_nFrom <= m_oInit.m_oPause.m_oMillisec.m_nTo));
	assert((m_oInit.m_oTotCount.m_nFrom > 0) && (m_oInit.m_oTotCount.m_nFrom <= m_oInit.m_oTotCount.m_nTo));
	assert((m_oInit.m_nMaxParallel > 0) || (m_oInit.m_nMaxParallel == -1));
}

void TileAnimatorEvent::trigger(int32_t /*nMsg*/, int32_t /*nValue*/, Event* p0TriggeringEvent) noexcept
{
//std::cout << "TileAnimatorEvent::trigger" << '\n';
	//TODO
	// ACTIVATE activate event
	// INIT init and register listeners
	// RUN
	// last RUN deactivate, unregister, state to INIT
	//          send messages to finished-group listeners
	Level& oLevel = level();
	auto& oGame = oLevel.game();
	const int32_t nGameTick = oGame.gameElapsed();
	const double fGameInterval = oGame.gameInterval();
	switch (m_eState)
	{
		case TILE_ANIMATOR_STATE_ACTIVATE:
		{
//std::cout << "TileAnimatorEvent::trigger   TILE_ANIMATOR_STATE_ACTIVATE" << '\n';
			if (p0TriggeringEvent != nullptr) {
				oLevel.activateEvent(this, nGameTick);
				break;
			}
			if (m_oInit.m_bDoBoard) {
				m_aRows.resize(m_oInit.m_oArea.m_nH);
				const int32_t nFromY = m_oInit.m_oArea.m_nY;
				const int32_t nToNY = m_oInit.m_oArea.m_nY + m_oInit.m_oArea.m_nH;
				for (int32_t nY = nFromY; nY < nToNY; ++nY) {
					shared_ptr<BlockData> refBlockData = recyclePopBlockData();
					const int32_t nRow = nY - m_oInit.m_oArea.m_nY;
					refBlockData->m_nRow = nRow;
					m_aRows[nRow] = refBlockData;
				}
			}
			m_eState = TILE_ANIMATOR_STATE_INIT;
		} // fallthrough
		case TILE_ANIMATOR_STATE_INIT:
		{
			if (m_oInit.m_bDoBoard) {
				const int32_t nFromY = m_oInit.m_oArea.m_nY;
				const int32_t nToNY = m_oInit.m_oArea.m_nY + m_oInit.m_oArea.m_nH;
				for (int32_t nY = nFromY; nY < nToNY; ++nY) {
					boardSegmentAddSelected(nY, m_oInit.m_oArea.m_nX, m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW);
				}
				oLevel.boardAddListener(this);
			}
			if (m_oInit.m_bDoBlocks) {
				std::vector<LevelBlock*> aLBs = oLevel.blocksGetAll();
				for (auto& p0LevelBlock : aLBs) {
					assert(p0LevelBlock != nullptr);
					blockAddSelected(*p0LevelBlock);
				}
				oLevel.blocksAddBricksIdListener(this);
			}
			m_nCounter = 0;
			m_nAnis = 0;
			m_eState = TILE_ANIMATOR_STATE_RUN;
		} // fallthrough
		case TILE_ANIMATOR_STATE_RUN:
		{
			if (p0TriggeringEvent != nullptr) {
				break; //switch
			}
			const bool bTerminate = (m_oInit.m_nRepeat != -1) && (m_nCounter >= m_oInit.m_nRepeat);
			if (bTerminate && (m_nAnis <= 0)) {
				m_eState = TILE_ANIMATOR_STATE_DEAD; // TODO TILE_ANIMATOR_STATE_INIT?
				if (m_oInit.m_bDoBoard) {
					oLevel.boardRemoveListener(this);
				}
				if (m_oInit.m_bDoBlocks) {
					oLevel.blocksRemoveBricksIdListener(this);
				}
				informListeners(LISTENER_GROUP_FINISHED, 0);
			} else {
				++m_nCounter;
				m_nStartedRunning = 0;
				if (!bTerminate) {
					checkNewTileAnis(nGameTick, fGameInterval);
				}
				animate(nGameTick, fGameInterval);
				oLevel.activateEvent(this, nGameTick + 1);
				if (m_nStartedRunning > 0) {
					informListeners(LISTENER_GROUP_TILEANI_STARTED, m_nStartedRunning);
				}
			}
		}
		break;
		case TILE_ANIMATOR_STATE_DEAD:
		{
		}
		break;
		default:
		{
			assert(false);
		}
	}
}

bool TileAnimatorEvent::createTileAniBoard(int32_t nNotAniIdx, int32_t nGameTick, double fGameInterval, int32_t nX, int32_t nY
											, shared_ptr<SelectedTile>& refSelectedTile) noexcept
{
//std::cout << "TileAnimatorEvent::createTileAniBoard  nX=" << nX << "  nY=" << nY << '\n';
	assert((nX >= m_oInit.m_oArea.m_nX) && (nX < m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW));
	assert((nY >= m_oInit.m_oArea.m_nY) && (nY < m_oInit.m_oArea.m_nY + m_oInit.m_oArea.m_nH));
	if (level().boardGetTileAnimator(nX, nY, m_oInit.m_nAniNameIdx) != nullptr) {
//std::cout << "TileAnimatorEvent::createTileAniBoard  Exit already animated" << '\n';
		return false; //--------------------------------------------------------
	}
	// create new TileAni
	shared_ptr<TileAni> refTileAni = createTileAni(refSelectedTile.operator->(), nGameTick, fGameInterval);
	refSelectedTile->m_refTileAni = refTileAni;
	auto itSelectedTile = insertInWaitingAnis(refSelectedTile);
	++m_nAnis;
	refSelectedTile->m_nIdxInGlobalNotAnis = -1;
	refSelectedTile->m_itGlobalAniContainer = itSelectedTile;
	level().boardSetTileAnimator(nX, nY, m_oInit.m_nAniNameIdx, refTileAni.operator->(), nX);
	// remove
	removeFromNotAnis(nNotAniIdx);
	return true;
}
bool TileAnimatorEvent::createTileAniBlock(int32_t nNotAniIdx, int32_t nGameTick, double fGameInterval, LevelBlock* p0LevelBlock, int32_t nBrick
												, shared_ptr<SelectedTile>& refSelectedTile) noexcept
{
	if (p0LevelBlock->blockGetTileAnimator(nBrick, m_oInit.m_nAniNameIdx) != nullptr) {
		return false;
	}
	// create new TileAni
	shared_ptr<TileAni> refTileAni = createTileAni(refSelectedTile.operator->(), nGameTick, fGameInterval);
	refSelectedTile->m_refTileAni = refTileAni;
	auto itSelectedTile = insertInWaitingAnis(refSelectedTile);
	++m_nAnis;
	refSelectedTile->m_nIdxInGlobalNotAnis = -1;
	refSelectedTile->m_itGlobalAniContainer = itSelectedTile;
	p0LevelBlock->blockSetTileAnimator(nBrick, m_oInit.m_nAniNameIdx, refTileAni.operator->(), nBrick);
	// remove
	removeFromNotAnis(nNotAniIdx);
	return true;
}
void TileAnimatorEvent::checkNewTileAnis(int32_t nGameTick, double fGameInterval) noexcept
{
//std::cout << "TileAnimatorEvent::checkNewTileAnis(" << nGameTick << ")   m_nAnis=" << m_nAnis << '\n';
	//TODO share code for (m_nMaxParallel == -1) and (m_nMaxParallel > 0)
	auto& oGame = level().game();
	if (m_oInit.m_nMaxParallel == -1) {
		int32_t nCur = 0;
		while (nCur < static_cast<int32_t>(m_aAllNotAnis.size())) {
			shared_ptr<SelectedTile>& refSelectedTile = m_aAllNotAnis[nCur];
			assert(refSelectedTile->m_nIdxInGlobalNotAnis == nCur);
			assert(!refSelectedTile->m_refTileAni);
			assert(refSelectedTile->m_p0Container != nullptr);
			BlockData& oBlockData = *(refSelectedTile->m_p0Container);
			LevelBlock* p0LevelBlock = oBlockData.m_p0LevelBlock;
			if (p0LevelBlock == nullptr) {
				const int32_t nX = refSelectedTile->m_nXOrBrick;
				const int32_t nY = oBlockData.m_nRow + m_oInit.m_oArea.m_nY;
				const bool bCreated = createTileAniBoard(nCur, nGameTick, fGameInterval, nX, nY, refSelectedTile);
				if (!bCreated) {
					++nCur;
				}
			} else {
				const int32_t nBrick = refSelectedTile->m_nXOrBrick;
				const bool bCreated = createTileAniBlock(nCur, nGameTick, fGameInterval, p0LevelBlock, nBrick, refSelectedTile);
				if (!bCreated) {
					++nCur;
				}
			}
		}
	} else {
		while (m_nAnis < m_oInit.m_nMaxParallel) {
			const int32_t nNotAnis = static_cast<int32_t>(m_aAllNotAnis.size());
//std::cout << "TileAnimatorEvent::checkNewTileAnis nNotAnis=" << nNotAnis << '\n';
			if (nNotAnis <= 0) {
				// There aren't selected "not yet animated" tiles to animate
				break; // while (m_nAnis < m_nMaxParallel)
			}
			int32_t nTry = s_nMaxTry; // TODO maybe std::min(s_nMaxTry, nTotNotAni)
			while (nTry > 0) {
				const int32_t nChosenNotAni = oGame.random(0, nNotAnis - 1);
				shared_ptr<SelectedTile>& refSelectedTile = m_aAllNotAnis[nChosenNotAni];
				assert(refSelectedTile->m_nIdxInGlobalNotAnis == nChosenNotAni);
				assert(!refSelectedTile->m_refTileAni);
				assert(refSelectedTile->m_p0Container != nullptr);
				BlockData& oBlockData = *(refSelectedTile->m_p0Container);
				LevelBlock* p0LevelBlock = oBlockData.m_p0LevelBlock;
				if (p0LevelBlock == nullptr) {
					const int32_t nX = refSelectedTile->m_nXOrBrick;
					const int32_t nY = oBlockData.m_nRow + m_oInit.m_oArea.m_nY;
					const bool bCreated = createTileAniBoard(nChosenNotAni, nGameTick, fGameInterval, nX, nY, refSelectedTile);
					if (bCreated) {
						break; // while (nTry > 0)
					} else {
						--nTry;
					}
				} else {
					const int32_t nBrick = refSelectedTile->m_nXOrBrick;
					const bool bCreated = createTileAniBlock(nChosenNotAni, nGameTick, fGameInterval, p0LevelBlock, nBrick, refSelectedTile);
					if (bCreated) {
						break; // while (nTry > 0)
					} else {
						--nTry;
					}
				}
			}
			if (nTry == 0) {
//std::cout << "TileAnimatorEvent::checkNewTileAnis 3 tries!" << '\n';
				break; // while (m_nAnis < m_nMaxParallel)
			}
		}
	}
}
void TileAnimatorEvent::animate(int32_t nGameTick, double fGameInterval) noexcept
{
//std::cout << "TileAnimatorEvent::animate(" << nGameTick << ")    m_nAnis=" << m_nAnis << '\n';
	if (m_nAnis <= 0) {
		return;
	}
	Level& oLevel = level();
	auto& oGame = oLevel.game();
	{
	// handle waiting anis
	auto itWaiting = m_oWaitingAnis.begin();
	while (itWaiting != m_oWaitingAnis.end()) {
//std::cout << "TileAnimatorEvent::animate(" << nGameTick << ")    m_nAnis=" << m_nAnis << '\n';
		shared_ptr<SelectedTile>& refSelectedTile = *itWaiting;
		assert(refSelectedTile->m_itGlobalAniContainer == itWaiting);
		shared_ptr<TileAni>& refTileAni = refSelectedTile->m_refTileAni;
		assert(refTileAni);
		assert(refTileAni->m_bWaiting);
		if (refTileAni->m_nNextStartTick > nGameTick) {
//std::cout << "TileAnimatorEvent::animate    refTileAni->m_nNextStartTick=" << refTileAni->m_nNextStartTick << " > nGameTick=" << nGameTick << '\n';
			// since ordered ascending the following are all > nGameTick => stop
			break; // while (itWaiting != m_oNotRunningAnis.end())
		}
		// start tile ani
		refTileAni->m_bWaiting = false;
		const NRange oDuration = m_oInit.m_oDuration.getCumulatedTicksRange(fGameInterval);
		const int32_t nDuration = oGame.random(oDuration.m_nFrom, oDuration.m_nTo);
		refTileAni->m_nTotTicks = std::max(1, nDuration);
		refTileAni->m_nCountdown = nDuration;
		// add to m_oRunningAnis
//std::cout << "TileAnimatorEvent::animate    m_oRunningAnis.push_front" << '\n';
		m_oRunningAnis.push_front(refSelectedTile);
		refSelectedTile->m_itGlobalAniContainer = m_oRunningAnis.begin();
		// extract from m_oNotRunningAnis
		itWaiting = m_oWaitingAnis.erase(itWaiting);
		++m_nStartedRunning;
	}
	}
	// handle running anis
	auto itRunning = m_oRunningAnis.begin();
	while (itRunning != m_oRunningAnis.end()) {
		shared_ptr<SelectedTile>& refSelectedTile = *itRunning;
		assert(refSelectedTile);
		assert(refSelectedTile->m_itGlobalAniContainer == itRunning);
		assert(refSelectedTile->m_refTileAni);
		shared_ptr<TileAni>& refTileAni = refSelectedTile->m_refTileAni;
		assert(refTileAni);
		TileAni& oTileAni = *refTileAni;
		assert(!oTileAni.m_bWaiting);
		assert(refSelectedTile->m_p0Container != nullptr);
		BlockData& oBlockData = *(refSelectedTile->m_p0Container);
		LevelBlock* p0LevelBlock = oBlockData.m_p0LevelBlock;
		if (p0LevelBlock == nullptr) {
			const int32_t nX = refSelectedTile->m_nXOrBrick;
			const int32_t nY = oBlockData.m_nRow + m_oInit.m_oArea.m_nY;
			assert((nX >= m_oInit.m_oArea.m_nX) && (nX < m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW));
			assert((nY >= m_oInit.m_oArea.m_nY) && (nY < m_oInit.m_oArea.m_nY + m_oInit.m_oArea.m_nH));
			const int32_t nOldCountdown = oTileAni.m_nCountdown;
			--oTileAni.m_nCountdown;
//std::cout << "TileAnimatorEvent::animate board      nX=" << nX << "  nY=" << nY << '\n';
//std::cout << "TileAnimatorEvent::animate board      oTileAni.m_nCountdown=" << oTileAni.m_nCountdown << "  oTileAni.m_nTotTicks=" << oTileAni.m_nTotTicks << '\n';
			const bool bEnded = (nOldCountdown <= 0);
			if (bEnded) {
				++oTileAni.m_nCount;
//std::cout << "TileAnimatorEvent::animate board  ENDED    oTileAni.m_nCount=" << oTileAni.m_nCount << '\n';
				const bool bDelete = (oTileAni.m_nCount == oTileAni.m_nTotCount);
				if (bDelete) {
//std::cout << "TileAnimatorEvent::animate board  DELETED" << '\n';
					assert(oLevel.boardGetTileAnimator(nX, nY, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
					oLevel.boardSetTileAnimator(nX, nY, m_oInit.m_nAniNameIdx, nullptr, 0);
					oLevel.boardAnimateTile(NPoint{nX, nY});
					// add to m_aAllNotAnis
					addToNotAnis(refSelectedTile);
					refTileAni.reset();
					// erase
					itRunning = m_oRunningAnis.erase(itRunning);
					// refSelectedTile->m_itGlobalAniContainer becomes invalid
					--m_nAnis;
				} else {
					oTileAni.m_bWaiting = true;
					const NRange oPause = m_oInit.m_oPause.getCumulatedTicksRange(fGameInterval);
					const int32_t nPause = oGame.random(oPause.m_nFrom, oPause.m_nTo);
//std::cout << "TileAnimatorEvent::animate board  PAUSE   nPause=" << nPause << '\n';
					if (nPause <= std::numeric_limits<int32_t>::max() - nGameTick) {
						oTileAni.m_nNextStartTick = nGameTick + nPause;
					} else {
						oTileAni.m_nNextStartTick = std::numeric_limits<int32_t>::max();
					}
					oLevel.boardAnimateTile(NPoint{nX, nY});
					// add to m_oWaitingAnis
					auto itSelectedTile = insertInWaitingAnis(refSelectedTile);
					refSelectedTile->m_itGlobalAniContainer = itSelectedTile;
					// extract from not m_oRunningAnis
					itRunning = m_oRunningAnis.erase(itRunning);
				}
			} else {
//std::cout << "TileAnimatorEvent::animate board  ANIMATE" << '\n';
				oLevel.boardAnimateTile(NPoint{nX, nY});
				++itRunning;
			}
		} else {
			const int32_t nBrick = refSelectedTile->m_nXOrBrick;
			const int32_t nOldCountdown = oTileAni.m_nCountdown;
			--oTileAni.m_nCountdown;
			const bool bEnded = (nOldCountdown <= 0);
			if (bEnded) {
				++oTileAni.m_nCount;
				const bool bDelete = (oTileAni.m_nCount == oTileAni.m_nTotCount);
				if (bDelete) {
					assert(p0LevelBlock->blockGetTileAnimator(nBrick, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
					p0LevelBlock->blockSetTileAnimator(nBrick, m_oInit.m_nAniNameIdx, nullptr, 0);
					//TODO oLevel.blockAnimateTile(oBloData.m_p0LevelBlock, nBrick);
					// add to m_aAllNotAnis
					addToNotAnis(refSelectedTile);
					refTileAni.reset();
					// erase
					itRunning = m_oRunningAnis.erase(itRunning);
					// refSelectedTile->m_itGlobalAniContainer becomes invalid
					--m_nAnis;
				} else {
					oTileAni.m_bWaiting = true;
					const NRange oPause = m_oInit.m_oPause.getCumulatedTicksRange(fGameInterval);
					const int32_t nPause = oGame.random(oPause.m_nFrom, oPause.m_nTo);
					if (nPause <= std::numeric_limits<int32_t>::max() - nGameTick) {
						oTileAni.m_nNextStartTick = nGameTick + nPause;
					} else {
						oTileAni.m_nNextStartTick = std::numeric_limits<int32_t>::max();
					}
					// add to m_oWaitingAnis
					auto itSelectedTile = insertInWaitingAnis(refSelectedTile);
					refSelectedTile->m_itGlobalAniContainer = itSelectedTile;
					// extract from not m_oRunningAnis
					itRunning = m_oRunningAnis.erase(itRunning);
				}
			} else {
				// TODO add following method to Level! We shouldn't assume that blocks are redrawn each tick
				//oLevel.blockAnimateTile(oBloData.m_p0LevelBlock, nIdx, 1, 1);
				++itRunning;
			}
		}
	}
}

std::list< shared_ptr<TileAnimatorEvent::SelectedTile> >::iterator TileAnimatorEvent::insertInWaitingAnis(const shared_ptr<SelectedTile>& refSelectedTile) noexcept
{
	if (m_oWaitingAnis.empty()) {
		// E
		// ^
		// B
		m_oWaitingAnis.push_back(refSelectedTile);
		// S--E
		// ^
		// BR
		return m_oWaitingAnis.begin(); //---------------------------------------
	}
	auto itCur = m_oWaitingAnis.end();
	// ...K--E
	//       ^
	//       C
	--itCur;
	// ...K--E
	//    ^
	//    C
	if (refSelectedTile->m_refTileAni->m_nNextStartTick >= (*itCur)->m_refTileAni->m_nNextStartTick) {
		// S >= K
		m_oWaitingAnis.push_back(refSelectedTile);
		// ...K--S--E
		//    ^
		//    C
		++itCur;
		// ...K--S--E
		//       ^
		//       CR
		return itCur; //--------------------------------------------------------
	}
	// S < K
	while (itCur != m_oWaitingAnis.begin()) {
		// ...J--K...
		//       ^
		//       C
		--itCur;
		// ...J--K...
		//    ^
		//    C
		if (refSelectedTile->m_refTileAni->m_nNextStartTick >= (*itCur)->m_refTileAni->m_nNextStartTick) {
			// S >= J
			++itCur;
			// ...J--S--K...
			//       ^  ^
			//       R  C
			return m_oWaitingAnis.insert(itCur, refSelectedTile);  //-----------
		}
		// S < J
		// (rename J as K)
		// ...K...
		//    ^
		//    C
	}
	// K...
	// ^
	// BC
	m_oWaitingAnis.push_front(refSelectedTile);
	// S--K...
	// ^  ^
	// BR C
	return m_oWaitingAnis.begin();
}
void TileAnimatorEvent::removeFromNotAnis(int32_t nToRemoveIdx) noexcept
{
	const int32_t nNotAnis = static_cast<int32_t>(m_aAllNotAnis.size());
	if (nToRemoveIdx < nNotAnis - 1) {
		// remove from m_aAllNotAnis by taking the last and inserting it here
		m_aAllNotAnis[nToRemoveIdx] = m_aAllNotAnis[nNotAnis - 1];
		m_aAllNotAnis[nToRemoveIdx]->m_nIdxInGlobalNotAnis = nToRemoveIdx;
	}
	m_aAllNotAnis[nNotAnis - 1].reset();
	m_aAllNotAnis.resize(nNotAnis - 1);
//#ifndef NDEBUG
//{
//const int32_t nNotAnis = m_aAllNotAnis.size();
//std::cout << "TileAnimatorEvent::removeFromNotAnis()   after nNotAnis = " << nNotAnis << "   nToRemoveIdx=" << nToRemoveIdx << '\n';
//for (int32_t nIdx = 0; nIdx < nNotAnis; ++nIdx){
//if (m_aAllNotAnis[nIdx]->m_nIdxInGlobalNotAnis != nIdx) {
//std::cout << "              FAILED             m_aAllNotAnis[nIdx]->m_nIdxInGlobalNotAnis = " << m_aAllNotAnis[nIdx]->m_nIdxInGlobalNotAnis << "   nIdx=" << nIdx << '\n';
//assert(false);
//}
//}
//}
//#endif
}
void TileAnimatorEvent::addToNotAnis(const shared_ptr<SelectedTile>& refSelectedTile) noexcept
{
	const int32_t nNotAnis = static_cast<int32_t>(m_aAllNotAnis.size());
	m_aAllNotAnis.push_back(refSelectedTile);
	refSelectedTile->m_nIdxInGlobalNotAnis = nNotAnis;
}

shared_ptr<TileAnimatorEvent::TileAni> TileAnimatorEvent::createTileAni(SelectedTile* p0SelectedTile, int32_t nGameTick, double fGameInterval) noexcept
{
	assert(p0SelectedTile != nullptr);

	auto& oGame = level().game();
	shared_ptr<TileAni> refTileAni;
	m_oTileAniRecycler.create(refTileAni);
	TileAni& oTileAni = *refTileAni;
	oTileAni.m_p0SelectedTile = p0SelectedTile;
	const NRange oInitialWait = m_oInit.m_oInitialWait.getCumulatedTicksRange(fGameInterval);
	const int32_t nInitialWait = oGame.random(oInitialWait.m_nFrom, oInitialWait.m_nTo);
	oTileAni.m_bWaiting = true;
	oTileAni.m_nNextStartTick = nGameTick + nInitialWait;
	oTileAni.m_nCount = 0;
	oTileAni.m_nTotCount = oGame.random(m_oInit.m_oTotCount.m_nFrom, m_oInit.m_oTotCount.m_nTo);
	oTileAni.m_nTotTicks = -1;
	oTileAni.m_nCountdown = -1;
	oTileAni.m_nAniNameIdx = m_oInit.m_nAniNameIdx;
//std::cout << "TileAnimatorEvent::TileAni::createTileAni()   m_nInitialWaitFrom = " << m_oInit.m_nInitialWaitFrom << "   m_nInitialWaitTo=" << m_oInit.m_nInitialWaitTo << '\n';
//std::cout << "TileAnimatorEvent::TileAni::createTileAni()   nGameTick = " << nGameTick << "   nInitialWait=" << nInitialWait << '\n';
	return refTileAni;
}

double TileAnimatorEvent::TileAni::getCommonElapsed(int32_t nAni, int32_t nViewTick, int32_t nTotViewTicks) const noexcept
{
//std::cout << "TileAnimatorEvent::TileAni::getCommonElapsed()   nViewTick = " << nViewTick << "   nTotViewTicks=" << nTotViewTicks << '\n';
	assert(nTotViewTicks > 0);
	assert((nViewTick >= 0) && (nViewTick < nTotViewTicks));
	if (nAni != m_nAniNameIdx) {
		assert(false);
		return TileAnimator::s_fInactiveElapsed;
	}
	if (m_bWaiting) {
		return TileAnimator::s_fInactiveElapsed;
	}
	const int32_t nTotSteps = m_nTotTicks;
	const int32_t nCountdown = m_nCountdown + 1;
//std::cout << "TileAnimatorEvent::TileAni::getCommonElapsed() nTotSteps=" << nTotSteps << " nCountdown=" << nCountdown << '\n';
	assert(nTotSteps > 0);
	assert((nCountdown > 0) && (nCountdown <= nTotSteps));
	const double fElapsed = 1.0 + (- nCountdown + (nViewTick + 0.5) / nTotViewTicks) / nTotSteps;
	assert((fElapsed >= 0.0) && (fElapsed <= 1.0));
	return fElapsed;
}
double TileAnimatorEvent::TileAni::getElapsed01(int32_t /*nHash*/, int32_t
												#ifndef NDEBUG
												nX
												#endif //NDEBUG
												, int32_t
												#ifndef NDEBUG
												nY
												#endif //NDEBUG
												, int32_t nAni, int32_t nViewTick, int32_t nTotTicks) const noexcept
{
	assert(m_p0SelectedTile != nullptr);
	assert(m_p0SelectedTile->m_nXOrBrick == nX);
	assert(m_p0SelectedTile->m_p0Container != nullptr);
	assert(m_p0SelectedTile->m_p0Container->m_p0TileAnimatorEvent != nullptr);
	assert(m_p0SelectedTile->m_p0Container->m_nRow + m_p0SelectedTile->m_p0Container->m_p0TileAnimatorEvent->m_oInit.m_oArea.m_nY == nY);
	assert(m_p0SelectedTile->m_p0Container->m_p0LevelBlock == nullptr);
	return getCommonElapsed(nAni, nViewTick, nTotTicks);
}
double TileAnimatorEvent::TileAni::getElapsed01(int32_t /*nHash*/, const LevelBlock&
									#ifndef NDEBUG
									oLevelBlock
									#endif //NDEBUG
									, int32_t /*nBrickIdx*/
									, int32_t nAni, int32_t nViewTick, int32_t nTotTicks) const noexcept
{
	assert(m_p0SelectedTile != nullptr);
	assert(m_p0SelectedTile->m_p0Container != nullptr);
	assert(m_p0SelectedTile->m_p0Container->m_p0LevelBlock == &oLevelBlock);
	return getCommonElapsed(nAni, nViewTick, nTotTicks);
}

void TileAnimatorEvent::boabloPreFreeze(LevelBlock& oLevelBlock) noexcept
{
	blockPreDeleteCommon(oLevelBlock);// TODO transfer Block's tile anis to Board (if m_bDoBoard)
}
void TileAnimatorEvent::boabloPostFreeze(const Coords& oCoords) noexcept
{
	boardPostModify(oCoords);
}
void TileAnimatorEvent::boabloPreUnfreeze(const Coords& oCoords) noexcept
{
	boardCoordsRemoveSelected(oCoords); 
	//TODO transfer Board's tile anis to Block (if m_bDoBlocks)
}
void TileAnimatorEvent::boabloPostUnfreeze(LevelBlock& oLevelBlock) noexcept
{
	blockPostInsertCommon(oLevelBlock);
}
void TileAnimatorEvent::boardPreScroll(Direction::VALUE eDir, const shared_ptr<TileRect>& refTiles) noexcept
{
	NRect oArea;
	oArea.m_nW = m_nBoardWidth;
	oArea.m_nH = m_nBoardHeight;
	boardPreInsert(eDir, oArea, refTiles);
}
void TileAnimatorEvent::boardPostScroll(Direction::VALUE eDir) noexcept
{
	NRect oArea;
	oArea.m_nW = m_nBoardWidth;
	oArea.m_nH = m_nBoardHeight;
	boardPostInsert(eDir, oArea);
}
void TileAnimatorEvent::boardPreInsert(Direction::VALUE eDir, NRect /*oArea*/, const shared_ptr<TileRect>& refTiles) noexcept
{
//std::cout << "TileAnimatorEvent::boardPreInsert(eDir=" << eDir << ")" << '\n';
	if (eDir == Direction::DOWN) {
		if ((refTiles) && !TileRect::isAllEmptyTiles(*refTiles)) {
			level().gameStatusTechnical(std::vector<std::string>{"TileAnimatorEvent::boardPreInsert()","DOWN only supports empty tiles insertion"});
			return;
		}
	}
}
void TileAnimatorEvent::boardPostInsert(Direction::VALUE eDir, NRect oArea) noexcept
{
//std::cout << "TileAnimatorEvent::boardPostInsert(eDir=" << eDir << ")" << '\n';
	// test whether m_nAreaXYWH intersects with params nXYWH
	//if ((m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW <= nX) || (nX + nW <= m_oInit.m_oArea.m_nX) || (m_oInit.m_oArea.m_nY + m_oInit.m_oArea.m_nH <= nY) || (nY + nH <= m_oInit.m_oArea.m_nY)) {
	if (! NRect::doIntersect(oArea, m_oInit.m_oArea)) {
		return;
	}
	if (!((eDir == Direction::UP) || (eDir == Direction::DOWN))) {
		level().gameStatusTechnical(std::vector<std::string>{"TileAnimatorEvent::boardPostInsert()","Only DOWN and UP supported"});
		return;
	}
	const auto& nX = oArea.m_nX;
	const auto& nY = oArea.m_nY;
	const auto& nW = oArea.m_nW;
	const auto& nH = oArea.m_nH;
	if (nY != 0) {
		level().gameStatusTechnical(std::vector<std::string>{"TileAnimatorEvent::boardPostInsert()","Only nY=0 supported"});
		return;
	}
	if ( !((m_oInit.m_oArea.m_nX >= nX) && (m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW <= nX + nW)) ) {
		std::vector<std::string> aText;
		aText.push_back("TileAnimatorEvent::boardPostInsert()");
		aText.push_back(Util::stringCompose("(areaX,areaW)=(%1,%2) must be subset of (%3,%4)", m_oInit.m_oArea.m_nX, m_oInit.m_oArea.m_nW, nX, nW));
		level().gameStatusTechnical(aText);
		return;
	}
	if (eDir == Direction::DOWN) {
		boardPostDeleteDown(nY + nH - 1, nX, nW);
	} else{
		boardPostInsertUp(nY + nH - 1, nX, nW);
	}
}

void TileAnimatorEvent::boardPostDeleteDown(int32_t nDelY, int32_t nX, int32_t nW) noexcept
{
//std::cout << "TileAnimatorEvent::boardPostDeleteDown()  nDelY=" << nDelY << '\n';
	if ((nDelY < m_oInit.m_oArea.m_nY) || (m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW <= nX) || (nX + nW <= m_oInit.m_oArea.m_nX)) {
		return;
	}
	if ( !((m_oInit.m_oArea.m_nX >= nX) && (m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW <= nX + nW)) ) {
		std::vector<std::string> aText;
		aText.push_back("TileAnimatorEvent::boardPostDeleteDown()");
		aText.push_back(Util::stringCompose("(areaX,areaW)=(%1,%2) must be subset of (%3,%4)", m_oInit.m_oArea.m_nX, m_oInit.m_oArea.m_nW, nX, nW));
		level().gameStatusTechnical(aText);
		return;
	}
	// remove
	const int32_t nRemoveY = std::min(m_oInit.m_oArea.m_nY + m_oInit.m_oArea.m_nH - 1, nDelY);
	shared_ptr<BlockData>& refRemoveRow = m_aRows[nRemoveY - m_oInit.m_oArea.m_nY];
	std::unordered_map<int32_t, shared_ptr<SelectedTile> >& oSelTiles = refRemoveRow->m_oSelected;
	auto itSel = oSelTiles.begin();
	while (itSel != oSelTiles.end()) {
		int32_t nSx = itSel->first;
		shared_ptr<SelectedTile>& refSelected = itSel->second;
		assert(refSelected);
		const shared_ptr<TileAni>& refTileAni = refSelected->m_refTileAni;
		if (refSelected->m_nIdxInGlobalNotAnis < 0) {
			// Running Ani
			assert(refTileAni);
			if (nRemoveY > nDelY) { // if (nRemoveY == nDelY) then segment's TileAnimators are already removed from board by level
				assert(level().boardGetTileAnimator(nSx, nRemoveY + 1, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
				level().boardSetTileAnimator(nSx, nRemoveY + 1, m_oInit.m_nAniNameIdx, nullptr, 0);
				level().boardAnimateTile(NPoint{nSx, nRemoveY + 1});
			}
			const bool bWaiting = refTileAni->m_bWaiting;
			if (bWaiting) {
				m_oWaitingAnis.erase(refSelected->m_itGlobalAniContainer);
			} else {
				m_oRunningAnis.erase(refSelected->m_itGlobalAniContainer);
			}
			--m_nAnis;
		} else {
			assert(!refTileAni);
			removeFromNotAnis(refSelected->m_nIdxInGlobalNotAnis);
		}
		recyclePushBackSelectedTile(refSelected);
		oSelTiles.erase(itSel);
		itSel = oSelTiles.begin();
	}
	shared_ptr<BlockData> refClearBlockData = refRemoveRow;

	// move down
	const int32_t nRecreateY = m_oInit.m_oArea.m_nY;
	for (int32_t nCurY = nRemoveY; nCurY > nRecreateY; --nCurY) {
		shared_ptr<BlockData>& refRowUp = m_aRows[nCurY - 1 - m_oInit.m_oArea.m_nY];
		shared_ptr<BlockData>& refRow = m_aRows[nCurY - m_oInit.m_oArea.m_nY];
		refRow = refRowUp;
		refRow->m_nRow = nCurY - m_oInit.m_oArea.m_nY; // rewrite
	}

	//recreate
	shared_ptr<BlockData>& refRecreateRow = m_aRows[nRecreateY - m_oInit.m_oArea.m_nY];
	refRecreateRow = refClearBlockData;
	refRecreateRow->m_nRow = nRecreateY - m_oInit.m_oArea.m_nY;
	assert(refRecreateRow->m_oSelected.empty());
	if (nRecreateY > 0) { // row 0 is filled with empty tiles which cannot be selected by this class
		boardSegmentAddSelected(nRecreateY, m_oInit.m_oArea.m_nX, m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW);
	}
}
void TileAnimatorEvent::boardPostInsertUp(int32_t nInsY, int32_t nX, int32_t nW) noexcept
{
//std::cout << "TileAnimatorEvent::boardPostInsertUp()  nInsY=" << nInsY << '\n';
	if ((nInsY < m_oInit.m_oArea.m_nY) || (m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW <= nX) || (nX + nW <= m_oInit.m_oArea.m_nX)) {
		return;
	}
	if ( !((m_oInit.m_oArea.m_nX >= nX) && (m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW <= nX + nW)) ) {
		std::vector<std::string> aText;
		aText.push_back("TileAnimatorEvent::boardPostInsertUp()");
		aText.push_back(Util::stringCompose("(areaX,areaW)=(%1,%2) must be subset of (%3,%4)", m_oInit.m_oArea.m_nX, m_oInit.m_oArea.m_nW, nX, nW));
		level().gameStatusTechnical(aText);
		return;
	}
	// remove
	const int32_t nRemoveY = m_oInit.m_oArea.m_nY;
	shared_ptr<BlockData>& refRemoveRow = m_aRows[nRemoveY - m_oInit.m_oArea.m_nY];
	std::unordered_map<int32_t, shared_ptr<SelectedTile> >& oSelTiles = refRemoveRow->m_oSelected;
	auto itSel = oSelTiles.begin();
	while (itSel != oSelTiles.end()) {
		int32_t nSx = itSel->first;
		shared_ptr<SelectedTile>& refSelected = itSel->second;
		const shared_ptr<TileAni>& refTileAni = refSelected->m_refTileAni;
		if (refSelected->m_nIdxInGlobalNotAnis < 0) {
			// Running Ani
			assert(refTileAni);
			if (nRemoveY > 0) { // if (nRemoveY == 0) then segment's TileAnimators are already removed from board by level
				assert(level().boardGetTileAnimator(nSx, nRemoveY - 1, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
				level().boardSetTileAnimator(nSx, nRemoveY - 1, m_oInit.m_nAniNameIdx, nullptr, 0);
				level().boardAnimateTile(NPoint{nSx, nRemoveY - 1}); 
			}
			const bool bWaiting = refTileAni->m_bWaiting;
			if (bWaiting) {
				m_oWaitingAnis.erase(refSelected->m_itGlobalAniContainer);
			} else {
				m_oRunningAnis.erase(refSelected->m_itGlobalAniContainer);
			}
			--m_nAnis;
		} else {
			assert(!refTileAni);
			removeFromNotAnis(refSelected->m_nIdxInGlobalNotAnis);
		}
		recyclePushBackSelectedTile(refSelected);
		oSelTiles.erase(itSel);
		itSel = oSelTiles.begin();
	}
	shared_ptr<BlockData> refClearBlockData = refRemoveRow;

	// move up
	const int32_t nRecreateY = std::min(nInsY, m_oInit.m_oArea.m_nY + m_oInit.m_oArea.m_nH - 1);
	for (int32_t nCurY = nRemoveY; nCurY < nRecreateY; ++nCurY) {
		shared_ptr<BlockData>& refRow = m_aRows[nCurY - m_oInit.m_oArea.m_nY];
		shared_ptr<BlockData>& refRowDown = m_aRows[nCurY + 1 - m_oInit.m_oArea.m_nY];
		refRow = refRowDown;
		refRow->m_nRow = nCurY - m_oInit.m_oArea.m_nY; // rewrite
	}

	//recreate
	shared_ptr<BlockData>& refRecreateRow = m_aRows[nRecreateY - m_oInit.m_oArea.m_nY];
	refRecreateRow = refClearBlockData;
	refRecreateRow->m_nRow = nRecreateY - m_oInit.m_oArea.m_nY;
	assert(refRecreateRow->m_oSelected.empty());
	{
		boardSegmentAddSelected(nRecreateY, m_oInit.m_oArea.m_nX, m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW);
	}
}
void TileAnimatorEvent::boardCoordsRemoveSelected(const Coords& oCoords) noexcept
{
	for (auto it = oCoords.begin(); it != oCoords.end(); it.next()) {
		const int32_t nX = it.x();
		const int32_t nY = it.y();
		if ((nY >= m_oInit.m_oArea.m_nY) && (nY < m_oInit.m_oArea.m_nY + m_oInit.m_oArea.m_nH) 
					&& (nX >= m_oInit.m_oArea.m_nX) && (nX < m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW)) {
			boardSegmentRemoveSelected(nY, nX, nX + 1);
		}
	}
}
void TileAnimatorEvent::boardPreDestroy(const Coords& /*oCoords*/) noexcept
{
//std::cout << "TileAnimatorEvent::boardPreDestroy()" << '\n';
}
void TileAnimatorEvent::boardPostDestroy(const Coords& oCoords) noexcept
{
//std::cout << "TileAnimatorEvent::boardPostDestroy()" << '\n';
	boardCoordsRemoveSelected(oCoords);
}
void TileAnimatorEvent::boardPreModify(const TileCoords& /*oTileCoords*/) noexcept
{
}
void TileAnimatorEvent::boardPostModify(const Coords& oCoords) noexcept
{
//std::cout << "TileAnimatorEvent::boardPostModify()  nX=" << nX << "  nY=" << nY << '\n';
	const auto oArea = oCoords.getMinMax();
	const int32_t nFromX = std::max(m_oInit.m_oArea.m_nX, oArea.m_nX);
	const int32_t nToNX = std::min(m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW, oArea.m_nX + oArea.m_nW); // [nFromX,nToNX[
	if (nFromX >= nToNX) {
		// no intersection
		return;
	}
	const int32_t nFromY = std::max(m_oInit.m_oArea.m_nY, oArea.m_nY);
	const int32_t nToNY = std::min(m_oInit.m_oArea.m_nY + m_oInit.m_oArea.m_nH, oArea.m_nY + oArea.m_nH); // [nFromY,nToNY[
	if (nFromY >= nToNY) {
		// no intersection
		return;
	}
	for (int32_t nCurY = nFromY; nCurY < nToNY; ++nCurY) {
		// TODO do these two together
		boardSegmentRemoveSelected(nCurY, nFromX, nToNX);
		boardSegmentAddSelected(nCurY, nFromX, nToNX);
	}
}

void TileAnimatorEvent::boardSegmentRemoveSelected(int32_t nRemoveY, int32_t nFromX, int32_t nToNX) noexcept
{
	shared_ptr<BlockData>& refModifyRow = m_aRows[nRemoveY - m_oInit.m_oArea.m_nY];
	std::unordered_map<int32_t, shared_ptr<SelectedTile> >& oSelTiles = refModifyRow->m_oSelected;
	for (int32_t nCurX = nFromX; nCurX < nToNX; ++nCurX) {
		auto itSel = oSelTiles.find(nCurX);
		if (itSel == oSelTiles.end()) {
			continue; // for ----
		}
		shared_ptr<SelectedTile>& refSelected = itSel->second;
		assert(refSelected);
		const shared_ptr<TileAni>& refTileAni = refSelected->m_refTileAni;
		if (refSelected->m_nIdxInGlobalNotAnis < 0) {
			// Ani
			assert(refTileAni);
			assert(level().boardGetTileAnimator(nCurX, nRemoveY, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
			level().boardSetTileAnimator(nCurX, nRemoveY, m_oInit.m_nAniNameIdx, nullptr, 0);
			level().boardAnimateTile(NPoint{nCurX, nRemoveY});
			const bool bWaiting = refTileAni->m_bWaiting;
			if (bWaiting) {
				m_oWaitingAnis.erase(refSelected->m_itGlobalAniContainer);
			} else {
				m_oRunningAnis.erase(refSelected->m_itGlobalAniContainer);
			}
			--m_nAnis;
		} else {
			assert(!refTileAni);
			removeFromNotAnis(refSelected->m_nIdxInGlobalNotAnis);
		}
		recyclePushBackSelectedTile(refSelected);
		assert(!refTileAni);
		oSelTiles.erase(itSel);
	}
}
void TileAnimatorEvent::boardSegmentAddSelected(int32_t nY, int32_t nFromX, int32_t nToNX) noexcept
{
//std::cout << "TileAnimatorEvent::boardSegmentAddSelected nY=" << nY << " nFromX=" << nFromX << " nToNX=" << nToNX << '\n';
	BlockData& oRow = *(m_aRows[nY - m_oInit.m_oArea.m_nY]);
	for (int32_t nCurX = nFromX; nCurX < nToNX; ++nCurX) {
		const Tile& oTile = level().boardGetTile(nCurX, nY);
		if ((!oTile.isEmpty()) && ((!m_oInit.m_refSelect) || m_oInit.m_refSelect->select(oTile))) {
			shared_ptr<SelectedTile> refSelected = recyclePopSelectedTile();
			assert(!refSelected->m_refTileAni);
			addToNotAnis(refSelected);
			oRow.m_oSelected[nCurX] = refSelected;
			refSelected->m_p0Container = &oRow;
			refSelected->m_nXOrBrick = nCurX;
		}
	}
}

bool TileAnimatorEvent::blockAddSelectedBrick(LevelBlock& oLevelBlock, int32_t nBrickId, shared_ptr<BlockData>& refBlockData) noexcept
{
//std::cout << "TileAnimatorEvent::blockAddSelectedBrick" << '\n';
	assert(refBlockData->m_p0LevelBlock == &oLevelBlock);
	std::unordered_map<int32_t, shared_ptr<SelectedTile> >& oSels = refBlockData->m_oSelected;
	const Tile& oTile = oLevelBlock.blockBrickTile(nBrickId);
	assert(!oTile.isEmpty());
	const bool bIsSelected = ((!m_oInit.m_refSelect) || m_oInit.m_refSelect->select(oTile));
	if (bIsSelected) {
		if (oLevelBlock.blockBrickVisible(nBrickId)) {
			shared_ptr<SelectedTile> refSelected = recyclePopSelectedTile();
			addToNotAnis(refSelected);
			oSels[nBrickId] = refSelected;
			refSelected->m_p0Container = refBlockData.operator->();
			refSelected->m_nXOrBrick = nBrickId;
		} else {
			//oSels[nBrickId] = shared_ptr<SelectedTile>()
			assert(oSels.find(nBrickId) != oSels.end());
			assert(!oSels[nBrickId]);
		}
	}
	return bIsSelected;
}
void TileAnimatorEvent::blockAddSelected(LevelBlock& oLevelBlock) noexcept
{
//std::cout << "TileAnimatorEvent::blockAddSelected" << '\n';

	shared_ptr<BlockData> refBlockData = recyclePopBlockData();
	refBlockData->m_nRow = -1;
	refBlockData->m_p0LevelBlock = &oLevelBlock;
	#ifndef NDEBUG
	std::unordered_map<int32_t, shared_ptr<SelectedTile> >& oSels = refBlockData->m_oSelected;
	#endif //NDEBUG
	assert(oSels.empty());
	const int32_t nBlockId = oLevelBlock.blockGetId();
	auto& aBrickId = oLevelBlock.blockBrickIds();
	for (auto& nBrickId : aBrickId) {
		blockAddSelectedBrick(oLevelBlock, nBrickId, refBlockData);
	}
	m_oBlocks[nBlockId] = refBlockData;
}

void TileAnimatorEvent::blockPreAdd(const LevelBlock& /*oBlock*/) noexcept
{
}
void TileAnimatorEvent::blockPostAdd(LevelBlock& oLevelBlock) noexcept
{
//std::cout << "TileAnimatorEvent::blockPostAdd  id=" << oLevelBlock.blockGetId() << '\n';
	blockPostInsertCommon(oLevelBlock);
}
void TileAnimatorEvent::blockPostInsertCommon(LevelBlock& oLevelBlock) noexcept
{
	if (!m_oInit.m_bDoBlocks) {
		return;
	}
	assert(m_oBlocks.find(oLevelBlock.blockGetId()) == m_oBlocks.end()); //not already added
	blockAddSelected(oLevelBlock);
}
void TileAnimatorEvent::blockPreRemove(LevelBlock& oLevelBlock) noexcept
{
//std::cout << "TileAnimatorEvent::blockPreRemove id=" << oLevelBlock.blockGetId() << '\n';
	blockPreDeleteCommon(oLevelBlock);
}
void TileAnimatorEvent::blockPreDeleteCommon(LevelBlock& oLevelBlock) noexcept
{
	if (!m_oInit.m_bDoBlocks) {
		return;
	}
	auto itF = m_oBlocks.find(oLevelBlock.blockGetId());
	assert(itF != m_oBlocks.end());
	shared_ptr<BlockData>& refBlockData = itF->second;
	assert(refBlockData->m_p0LevelBlock == &oLevelBlock);
	std::unordered_map<int32_t, shared_ptr<SelectedTile> >& oSels = refBlockData->m_oSelected;
	auto itSels = oSels.begin();
	while (itSels != oSels.end()) {
		const int32_t nBrick = itSels->first;
		shared_ptr<SelectedTile>& refSelected = itSels->second;
		if (refSelected) {
			// visible
			const shared_ptr<TileAni>& refTileAni = refSelected->m_refTileAni;
			if (refSelected->m_nIdxInGlobalNotAnis < 0) {
				// Ani
				assert(refTileAni);
				assert(refBlockData->m_p0LevelBlock->blockGetTileAnimator(nBrick, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
				refBlockData->m_p0LevelBlock->blockSetTileAnimator(nBrick, m_oInit.m_nAniNameIdx, nullptr, 0);
				//TODO level().blockAnimateTile(refBlockData->m_p0LevelBlock, nBrick); 
				const bool bWaiting = refTileAni->m_bWaiting;
				if (bWaiting) {
					m_oWaitingAnis.erase(refSelected->m_itGlobalAniContainer);
				} else {
					m_oRunningAnis.erase(refSelected->m_itGlobalAniContainer);
				}
				--m_nAnis;
			} else {
				assert(!refTileAni);
				removeFromNotAnis(refSelected->m_nIdxInGlobalNotAnis);
			}
			recyclePushBackSelectedTile(refSelected);
		}
		oSels.erase(itSels);
		itSels = oSels.begin();
	}
	recyclePushBackBlockData(refBlockData);
	m_oBlocks.erase(itF);
}
void TileAnimatorEvent::blockPostRemove(const LevelBlock& oLevelBlock) noexcept
{
	blockPostDeleteCommon(oLevelBlock);
}
void TileAnimatorEvent::blockPostDeleteCommon(const LevelBlock& /*oLevelBlock*/) noexcept
{
}
void TileAnimatorEvent::blockPreDestroy(LevelBlock& oBlock) noexcept
{
//std::cout << "TileAnimatorEvent::blockPreDestroy" << '\n';
	blockPreDeleteCommon(oBlock);
}
void TileAnimatorEvent::blockPostDestroy(const LevelBlock& oBlock) noexcept
{
//std::cout << "TileAnimatorEvent::blockPostDestroy" << '\n';
	blockPostDeleteCommon(oBlock);
}

void TileAnimatorEvent::blockPreFuse(LevelBlock& /*oFusedToBlock*/, LevelBlock& /*oFusedBlock*/) noexcept
{
}
void TileAnimatorEvent::blockPostFuse(LevelBlock& oResBlock, const LevelBlock& oFusedBlock
									, const std::unordered_map<int32_t, int32_t>& oFusedToBrickIds
									, const std::unordered_map<int32_t, int32_t>& oFusedBrickIds) noexcept
{
	if (!m_oInit.m_bDoBlocks) {
		return;
	}
//std::cout << "TileAnimatorEvent::blockPostFuse" << '\n';

	auto itResBlock = m_oBlocks.find(oResBlock.blockGetId());
	assert(itResBlock != m_oBlocks.end());
	shared_ptr<BlockData>& refResBlockData = itResBlock->second;
	assert(refResBlockData->m_p0LevelBlock == &oResBlock);
	std::unordered_map<int32_t, shared_ptr<SelectedTile> >& oResSels = refResBlockData->m_oSelected;

	auto itFusedBlock = m_oBlocks.find(oFusedBlock.blockGetId());
	assert(itFusedBlock != m_oBlocks.end());
	shared_ptr<BlockData>& refFusedBlockData = itFusedBlock->second;
	assert(refFusedBlockData->m_p0LevelBlock == &oFusedBlock);
	//const int32_t nFusedShape = oFusedBlock.getShape();
	std::unordered_map<int32_t, shared_ptr<SelectedTile> >& oFusedSels = refFusedBlockData->m_oSelected;

	shared_ptr<BlockData> refNewBlockData = recyclePopBlockData();
	//refNewBlockData->m_aSelectedBricks.clear();
	refNewBlockData->m_nRow = -1;
	refNewBlockData->m_p0LevelBlock = refResBlockData->m_p0LevelBlock;
	std::unordered_map<int32_t, shared_ptr<SelectedTile> >& oNewSels = refNewBlockData->m_oSelected;
	assert(oNewSels.empty());

	for (auto& oOldToResBrickId : oFusedToBrickIds) {
		const int32_t nOldBrickId = oOldToResBrickId.first;
		const int32_t nNewBrickId = oOldToResBrickId.second;
		auto itS = oResSels.find(nOldBrickId);
		if (itS != oResSels.end()) {
			// was selected
			shared_ptr<SelectedTile>& refSelected = itS->second;
			assert(refSelected); // The fusion is only of visible bricks
			assert(refSelected->m_nXOrBrick == nOldBrickId);
			shared_ptr<TileAni> refTileAni = refSelected->m_refTileAni;
			if (refSelected->m_nIdxInGlobalNotAnis < 0) {
				assert(refTileAni);
				// was animated
				// set callback (removed by Level)
				oResBlock.blockSetTileAnimator(nNewBrickId, m_oInit.m_nAniNameIdx, refTileAni.operator->(), nNewBrickId);
			}
			// point to the new container
			refSelected->m_p0Container = refNewBlockData.operator->();
			refSelected->m_nXOrBrick = nNewBrickId;
			// add to the new container
			oNewSels[nNewBrickId] = refSelected;
			// remove from the old container
			oResSels.erase(itS);
		}
	}
	for (auto& oFusedToResBrickId : oFusedBrickIds) {
		const int32_t nFusedBrickId = oFusedToResBrickId.first;
		const int32_t nNewBrickId = oFusedToResBrickId.second;
		auto itS = oFusedSels.find(nFusedBrickId);
		if (itS != oFusedSels.end()) {
			// was selected
			shared_ptr<SelectedTile>& refSelected = itS->second;
			assert(refSelected); // The fusion is only of visible bricks
			assert(refSelected->m_nXOrBrick == nFusedBrickId);
			shared_ptr<TileAni> refTileAni = refSelected->m_refTileAni;
			if (refSelected->m_nIdxInGlobalNotAnis < 0) {
				assert(refTileAni);
				// was animated
				// set callback (removed by Level)
				oResBlock.blockSetTileAnimator(nNewBrickId, m_oInit.m_nAniNameIdx, refTileAni.operator->(), nNewBrickId);
			}
			// point to the new container
			refSelected->m_p0Container = refNewBlockData.operator->();
			refSelected->m_nXOrBrick = nNewBrickId;
			// add to the new container
			oNewSels[nNewBrickId] = refSelected;
			// remove from the old container
			oFusedSels.erase(itS);
		}
	}
	refResBlockData.swap(refNewBlockData);

	recyclePushBackBlockData(refNewBlockData);
	recyclePushBackBlockData(refFusedBlockData);
	m_oBlocks.erase(itFusedBlock);
}
void TileAnimatorEvent::blockPreModify(LevelBlock& oBlock
										, const std::vector<int32_t>& aDeleteBrickId
										, const std::vector<int32_t>& /*aModifyPosBrickId*/
										, const std::vector<int32_t>& /*aModifyTileBrickId*/
										, bool /*bAddsBricks*/) noexcept
{
	if (!m_oInit.m_bDoBlocks) {
		return;
	}
//std::cout << "TileAnimatorEvent::blockPreModify" << '\n';

	auto itF = m_oBlocks.find(oBlock.blockGetId());
	assert(itF != m_oBlocks.end());
	shared_ptr<BlockData>& refBlockData = itF->second;
	assert(refBlockData->m_p0LevelBlock == &oBlock);
	std::unordered_map<int32_t, shared_ptr<SelectedTile> >& oSels = refBlockData->m_oSelected;
	//
	for (auto& nBrickId : aDeleteBrickId) {
		auto itS = oSels.find(nBrickId);
		if (itS == oSels.end()) {
			// not selected (therefore not animated)
			continue; //for
		}
		shared_ptr<SelectedTile>& refSelected = itS->second;
		if (refSelected) {
			// is visible
			assert(oBlock.blockBrickVisible(nBrickId));
			assert(refSelected->m_nXOrBrick == nBrickId);
			shared_ptr<TileAni> refTileAni = refSelected->m_refTileAni;
			// remove tile animation
			if (refSelected->m_nIdxInGlobalNotAnis < 0) {
				assert(refTileAni);
				const bool bWaiting = refTileAni->m_bWaiting;
				if (bWaiting) {
					m_oWaitingAnis.erase(refSelected->m_itGlobalAniContainer);
				} else {
					m_oRunningAnis.erase(refSelected->m_itGlobalAniContainer);
				}
				--m_nAnis;
			} else {
				assert(!refTileAni);
				removeFromNotAnis(refSelected->m_nIdxInGlobalNotAnis);
			}
		}
		recyclePushBackSelectedTile(refSelected);
		oSels.erase(itS);
	}
}
void TileAnimatorEvent::blockPostModify(LevelBlock& oBlock
										, const std::vector<int32_t>& /*aDeletedBrickId*/
										, const std::vector<int32_t>& aModifiedPosBrickId
										, const std::vector<int32_t>& aModifiedTileBrickId
										, const std::vector<int32_t>& aAddedBrickId) noexcept
{
	if (!m_oInit.m_bDoBlocks) {
		return;
	}
//std::cout << "TileAnimatorEvent::blockPostModify" << '\n';

	auto itF = m_oBlocks.find(oBlock.blockGetId());
	assert(itF != m_oBlocks.end());
	shared_ptr<BlockData>& refBlockData = itF->second;
	assert(refBlockData->m_p0LevelBlock == &oBlock);
	std::unordered_map<int32_t, shared_ptr<SelectedTile> >& oSels = refBlockData->m_oSelected;
	//
	for (auto& nBrickId : aModifiedTileBrickId) {
		auto itS = oSels.find(nBrickId);
		const bool bWasSelected = (itS != oSels.end());
		const Tile& oTile = oBlock.blockBrickTile(nBrickId);
		assert(!oTile.isEmpty());
		const bool bIsSelected = ((!m_oInit.m_refSelect) || m_oInit.m_refSelect->select(oTile));
		if (bWasSelected == bIsSelected) {
			continue; //for
		}
		if (bIsSelected) {
			// add
			if (oBlock.blockBrickVisible(nBrickId)) {
				shared_ptr<SelectedTile> refSelected = recyclePopSelectedTile();
				addToNotAnis(refSelected);
				oSels[nBrickId] = refSelected;
				refSelected->m_p0Container = refBlockData.operator->();
				refSelected->m_nXOrBrick = nBrickId;
			} else {
				oSels[nBrickId] = shared_ptr<SelectedTile>();
			}
		} else {
			// was selected: remove
			shared_ptr<SelectedTile>& refSelected = itS->second;
			if (refSelected) {
				// was visible
				assert(refSelected->m_nXOrBrick == nBrickId);
				shared_ptr<TileAni> refTileAni = refSelected->m_refTileAni;
				if (refSelected->m_nIdxInGlobalNotAnis < 0) {
					assert(refTileAni);
					const bool bWaiting = refTileAni->m_bWaiting;
					if (bWaiting) {
						m_oWaitingAnis.erase(refSelected->m_itGlobalAniContainer);
					} else {
						m_oRunningAnis.erase(refSelected->m_itGlobalAniContainer);
					}
					oBlock.blockSetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx, nullptr, 0);
					--m_nAnis;
				} else {
					assert(!refTileAni);
					removeFromNotAnis(refSelected->m_nIdxInGlobalNotAnis);
				}
				recyclePushBackSelectedTile(refSelected);
			}
			oSels.erase(itS);
		}
	}
	//
	for (auto& nBrickId : aModifiedPosBrickId) {
		auto itS = oSels.find(nBrickId);
		const bool bIsSelected = (itS != oSels.end());
		if (!bIsSelected) {
			continue; //for
		}
		shared_ptr<SelectedTile>& refSelected = itS->second;
		const bool bWasVisible = (refSelected ? true : false);
		const bool bIsVisible = oBlock.blockBrickVisible(nBrickId);
		if (!bIsVisible) {
			// is not visible
			if (bWasVisible) {
				// was visible => remove tile ani
				assert(refSelected->m_nXOrBrick == nBrickId);
				shared_ptr<TileAni> refTileAni = refSelected->m_refTileAni;
				if (refSelected->m_nIdxInGlobalNotAnis < 0) {
					assert(refTileAni);
					const bool bWaiting = refTileAni->m_bWaiting;
					if (bWaiting) {
						m_oWaitingAnis.erase(refSelected->m_itGlobalAniContainer);
					} else {
						m_oRunningAnis.erase(refSelected->m_itGlobalAniContainer);
					}
					oBlock.blockSetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx, nullptr, 0);
					--m_nAnis;
				} else {
					assert(!refTileAni);
					removeFromNotAnis(refSelected->m_nIdxInGlobalNotAnis);
				}
				// set to selected but not visible
				recyclePushBackSelectedTile(refSelected);
				refSelected.reset();
			}
		} else {
			// brick is visible
			if (bWasVisible) {
				// was visible
				assert(refSelected->m_nXOrBrick == nBrickId);
				shared_ptr<TileAni> refTileAni = refSelected->m_refTileAni;
				if (refSelected->m_nIdxInGlobalNotAnis < 0) {
					assert(refTileAni);
					// was animated
					// keep animation, restore callback (removed from Level)
					oBlock.blockSetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx, refTileAni.operator->(), nBrickId);
				}
			} else {
				// was hidden
				// add to NotAnis
				refSelected = recyclePopSelectedTile();
				refSelected->m_p0Container = refBlockData.operator->();
				refSelected->m_nXOrBrick = nBrickId;
				addToNotAnis(refSelected);
				assert(!refSelected->m_refTileAni);
			}
		}
	}
	//
	for (auto& nBrickId : aAddedBrickId) {
		#ifndef NDEBUG
		auto itS = oSels.find(nBrickId);
		const bool bWasSelected = (itS != oSels.end());
		#endif //NDEBUG
		assert(!bWasSelected);
		blockAddSelectedBrick(oBlock, nBrickId, refBlockData);
	}
}

void TileAnimatorEvent::recyclePushBackSelectedTile(const shared_ptr<SelectedTile>& refSelected) noexcept
{
//std::cout << "TileAnimatorEvent::recyclePushBackSelectedTile  &refSelected=" << (int64_t)refSelected.operator->() << '\n';
	assert(refSelected);
	shared_ptr<TileAni>& refTileAni = refSelected->m_refTileAni;
	if (refTileAni) {
		refTileAni.reset();
	}
	m_oInactiveSelectedTile.push_front(refSelected);
}

shared_ptr<TileAnimatorEvent::SelectedTile> TileAnimatorEvent::recyclePopSelectedTile() noexcept
{
	for (auto it = m_oInactiveSelectedTile.begin(); it != m_oInactiveSelectedTile.end(); ++it) {
		shared_ptr<SelectedTile>& ref = *it;
//std::cout << "TileAnimatorEvent::recyclePopSelectedTile  &refSelected=" << (int64_t)ref.operator->() << '\n';
		assert(ref.use_count() == 1);
		if (ref.use_count() == 1) {
			shared_ptr<SelectedTile> refTemp = ref;
			m_oInactiveSelectedTile.erase(it);
			return refTemp;
		}
	}
	shared_ptr<SelectedTile> refTemp = shared_ptr<SelectedTile>(new SelectedTile());
//std::cout << "TileAnimatorEvent::recyclePopSelectedTile  NEW &refSelected=" << (int64_t)refTemp.operator->() << '\n';
	return refTemp;
}

void TileAnimatorEvent::recyclePushBackBlockData(const shared_ptr<BlockData>& refBlockData) noexcept
{
	assert(refBlockData);
	std::unordered_map<int32_t, shared_ptr<SelectedTile> >& oSels = refBlockData->m_oSelected;
	auto itS = oSels.begin();
	while (itS != oSels.end()) {
		shared_ptr<SelectedTile>& refSelected = itS->second;
		if (refSelected) {
			recyclePushBackSelectedTile(refSelected);
			refSelected.reset();
		}
		oSels.erase(itS);
		itS = oSels.begin();
	}
	m_oInactiveBlockData.push_front(refBlockData);
}

shared_ptr<TileAnimatorEvent::BlockData> TileAnimatorEvent::recyclePopBlockData() noexcept
{
	for (auto it = m_oInactiveBlockData.begin(); it != m_oInactiveBlockData.end(); ++it) {
		shared_ptr<BlockData>& ref = *it;
		assert(ref.use_count() == 1);
		if (ref.use_count() == 1) {
			shared_ptr<BlockData> refTemp = ref;
			m_oInactiveBlockData.erase(it);
			return refTemp;
		}
	}
	shared_ptr<BlockData> refTemp = shared_ptr<BlockData>(new BlockData(this));
	return refTemp;
}

} // namespace stmg
