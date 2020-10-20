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
 * File:   tileanimatorevent.cc
 */

#include "events/tileanimatorevent.h"

#include "gameproxy.h"
#include "level.h"
#include "levelblock.h"
#include "named.h"
#include "tile.h"

#include "utile/tilerect.h"
#include "utile/tilecoords.h"

#include "util/coords.h"
#include "util/namedindex.h"
#include "util/util.h"
#include "util/helpers.h"

#include <vector>
#include <cassert>
#include <limits>
#include <algorithm>
//#include <iostream>
#include <string>
#include <utility>


namespace stmg
{

const int32_t TileAnimatorEvent::s_nMaxTry = 3;

static const std::vector<int32_t> s_aEmptyVector{};

template <class TValue>
void vectorRemoveIndex(std::vector<TValue>& oV, int32_t nIdx) noexcept
{
	oV[nIdx] = oV[oV.size() - 1];
	oV.pop_back();
}

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
void TileAnimatorEvent::deInit() noexcept
{
	m_aNotAnisPos.clear();
	m_aWaitingAnisPos.clear();
	m_aWaitingAnisStart.clear();
	m_aWaitingAnis.clear();
	m_aRunningAnisPos.clear();
	m_aRunningAnis.clear();

	m_eState = TILE_ANIMATOR_STATE_ACTIVATE;
	m_nCounter = 0;
}

void TileAnimatorEvent::trigger(int32_t /*nMsg*/, int32_t /*nValue*/, Event* p0TriggeringEvent) noexcept
{
	//TODO
	// ACTIVATE activate event
	// INIT init and register listeners
	// RUN
	// last RUN deactivate, unregister, state to INIT
	//          send messages to finished-group listeners
	Level& oLevel = level();
	auto& oGame = oLevel.game();
	const int32_t nGameTick = oGame.gameElapsed();
//std::cout << "TileAnimatorEvent::trigger   nGameTick " << nGameTick << '\n';
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
			m_eState = TILE_ANIMATOR_STATE_INIT;
		} // fallthrough
		case TILE_ANIMATOR_STATE_INIT:
		{
			if (m_oInit.m_bDoBoard) {
				boardAddSelected(m_oInit.m_oArea);
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
			m_eState = TILE_ANIMATOR_STATE_RUN;
		} // fallthrough
		case TILE_ANIMATOR_STATE_RUN:
		{
			if (p0TriggeringEvent != nullptr) {
				oLevel.activateEvent(this, nGameTick + 1);
				break; //switch
			}
			const bool bTerminate = (m_oInit.m_nRepeat != -1) && (m_nCounter >= m_oInit.m_nRepeat);
			if (bTerminate && (m_aWaitingAnisPos.size() + m_aRunningAnisPos.size() == 0)) {
				//m_eState = TILE_ANIMATOR_STATE_DEAD; // TODO TILE_ANIMATOR_STATE_INIT?
				if (m_oInit.m_bDoBoard) {
					oLevel.boardRemoveListener(this);
				}
				if (m_oInit.m_bDoBlocks) {
					oLevel.blocksRemoveBricksIdListener(this);
				}
				deInit();
				informListeners(LISTENER_GROUP_FINISHED, 0);
			} else {
				++m_nCounter;
				m_nStartedRunning = 0;
				if (!bTerminate) {
					checkNewTileAnis(nGameTick, fGameInterval);
				}
				animate(nGameTick, fGameInterval);
				//
				oLevel.activateEvent(this, nGameTick + 1);
				//
				if (m_nStartedRunning > 0) {
					informListeners(LISTENER_GROUP_TILEANI_STARTED, m_nStartedRunning);
				}
			}
		}
		break;
		//case TILE_ANIMATOR_STATE_DEAD:
		//{
		//}
		//break;
		default:
		{
			assert(false);
		}
	}
}

bool TileAnimatorEvent::createTileAniBoard(int32_t nNotAniIdx, int32_t nGameTick, double fGameInterval, int32_t nX, int32_t nY) noexcept
{
//std::cout << "TileAnimatorEvent::createTileAniBoard  nX=" << nX << "  nY=" << nY << '\n';
	Level& oLevel = level();
	auto& oGame = oLevel.game();
	assert((nX >= m_oInit.m_oArea.m_nX) && (nX < m_oInit.m_oArea.m_nX + m_oInit.m_oArea.m_nW));
	assert((nY >= m_oInit.m_oArea.m_nY) && (nY < m_oInit.m_oArea.m_nY + m_oInit.m_oArea.m_nH));
	if (oLevel.boardGetTileAnimator(nX, nY, m_oInit.m_nAniNameIdx) != nullptr) {
//std::cout << "TileAnimatorEvent::createTileAniBoard  Exit already animated" << '\n';
		return false; //--------------------------------------------------------
	}
	// create new TileAni
	shared_ptr<TileAni> refTileAni = createTileAni(nullptr);
	oLevel.boardSetTileAnimator(nX, nY, m_oInit.m_nAniNameIdx, refTileAni.operator->(), nX);

	const NRange oInitialWait = m_oInit.m_oInitialWait.getCumulatedTicksRange(fGameInterval);
	const int32_t nInitialWait = oGame.random(oInitialWait.m_nFrom, oInitialWait.m_nTo);
	const int32_t nNextStartTick = nGameTick + nInitialWait;
	// insert in waiting
	m_aWaitingAnisPos.push_back(NPoint{nX, nY});
	m_aWaitingAnisStart.push_back(nNextStartTick);
	m_aWaitingAnis.push_back(std::move(refTileAni));
//	++m_nAnis;
	// remove
	vectorRemoveIndex(m_aNotAnisPos, nNotAniIdx);
	return true;
}
bool TileAnimatorEvent::createTileAniBlock(int32_t nNotAniIdx, int32_t nGameTick, double fGameInterval, LevelBlock* p0LevelBlock, int32_t nBrick) noexcept
{
	Level& oLevel = level();
	auto& oGame = oLevel.game();
	if (p0LevelBlock->blockGetTileAnimator(nBrick, m_oInit.m_nAniNameIdx) != nullptr) {
		return false;
	}
	// create new TileAni
	shared_ptr<TileAni> refTileAni = createTileAni(p0LevelBlock);
	p0LevelBlock->blockSetTileAnimator(nBrick, m_oInit.m_nAniNameIdx, refTileAni.operator->(), nBrick);
	//
	const NRange oInitialWait = m_oInit.m_oInitialWait.getCumulatedTicksRange(fGameInterval);
	const int32_t nInitialWait = oGame.random(oInitialWait.m_nFrom, oInitialWait.m_nTo);
	const int32_t nNextStartTick = nGameTick + nInitialWait;
	// insert in waiting
	m_aWaitingAnisPos.push_back(NPoint{-1 - nBrick, p0LevelBlock->blockGetId()});
	m_aWaitingAnisStart.push_back(nNextStartTick);
	m_aWaitingAnis.push_back(std::move(refTileAni));
	// remove
	vectorRemoveIndex(m_aNotAnisPos, nNotAniIdx);
	return true;
}
void TileAnimatorEvent::checkNewTileAnis(int32_t nGameTick, double fGameInterval) noexcept
{
//std::cout << "TileAnimatorEvent::checkNewTileAnis(" << nGameTick << ")   m_aNotAnisPos.size()=" << m_aNotAnisPos.size() << '\n';
	Level& oLevel = level();
	//TODO share code for (m_nMaxParallel == -1) and (m_nMaxParallel > 0)
	auto& oGame = level().game();
	if (m_oInit.m_nMaxParallel == -1) {
		int32_t nCur = 0;
		while (nCur < static_cast<int32_t>(m_aNotAnisPos.size())) {
			const NPoint oXY = m_aNotAnisPos[nCur];
			const int32_t nXOrBrick = oXY.m_nX;
			if (nXOrBrick >= 0) {
				const bool bCreated = createTileAniBoard(nCur, nGameTick, fGameInterval, oXY.m_nX, oXY.m_nY);
				if (! bCreated) {
					++nCur;
				}
			} else {
				const int32_t nBrickId = - 1 - nXOrBrick;
				const int32_t nLBId = oXY.m_nY;
				LevelBlock* p0LevelBlock = oLevel.blocksGet(nLBId);
				assert(p0LevelBlock != nullptr);
				const bool bCreated = createTileAniBlock(nCur, nGameTick, fGameInterval, p0LevelBlock, nBrickId);
				if (! bCreated) {
					++nCur;
				}
			}
		}
	} else {
		while (static_cast<int32_t>(m_aWaitingAnisPos.size() + m_aRunningAnisPos.size()) < m_oInit.m_nMaxParallel) {
			const int32_t nNotAnis = static_cast<int32_t>(m_aNotAnisPos.size());
//std::cout << "TileAnimatorEvent::checkNewTileAnis nNotAnis=" << nNotAnis << '\n';
			if (nNotAnis <= 0) {
				// There aren't selected "not yet animated" tiles to animate
				break; // while (m_nAnis < m_nMaxParallel)
			}
			int32_t nTry = s_nMaxTry; // TODO maybe std::min(s_nMaxTry, nTotNotAni)
			while (nTry > 0) {
				const int32_t nChosenNotAni = oGame.random(0, nNotAnis - 1);
				const NPoint oXY = m_aNotAnisPos[nChosenNotAni];
				const int32_t nXOrBrick = oXY.m_nX;
				if (nXOrBrick >= 0) {
					const bool bCreated = createTileAniBoard(nChosenNotAni, nGameTick, fGameInterval, oXY.m_nX, oXY.m_nY);
					if (bCreated) {
						break; // while (nTry > 0)
					} else {
						--nTry;
					}
				} else {
					const int32_t nBrick = - 1 - nXOrBrick;
					const int32_t nLBId = oXY.m_nY;
					LevelBlock* p0LevelBlock = oLevel.blocksGet(nLBId);
					assert(p0LevelBlock != nullptr);
					const bool bCreated = createTileAniBlock(nChosenNotAni, nGameTick, fGameInterval, p0LevelBlock, nBrick);
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
	if (m_aWaitingAnisPos.size() + m_aRunningAnisPos.size() == 0) {
		return;
	}
//std::cout << "TileAnimatorEvent::animate(" << nGameTick << ")  Waiting=" << m_aWaitingAnisPos.size() << " Running=" << m_aRunningAnisPos.size() << '\n';
	Level& oLevel = level();
	auto& oGame = oLevel.game();
	{
	int32_t nCurIdx = 0;
	while (true) {
		const int32_t nTotWaiting = static_cast<int32_t>(m_aWaitingAnisStart.size());
		if (nCurIdx >= nTotWaiting) {
			break;
		}
		if (m_aWaitingAnisStart[nCurIdx] <= nGameTick) {
			shared_ptr<TileAni>& refTileAni = m_aWaitingAnis[nCurIdx];
			// start tile ani
			refTileAni->m_bWaiting = false;
			const NRange oDuration = m_oInit.m_oDuration.getCumulatedTicksRange(fGameInterval);
			const int32_t nDuration = oGame.random(oDuration.m_nFrom, oDuration.m_nTo);
			refTileAni->m_nTotTicks = std::max(1, nDuration);
			refTileAni->m_nCountdown = nDuration;
			//
			m_aRunningAnisPos.push_back(std::move(m_aWaitingAnisPos[nCurIdx]));
			m_aWaitingAnisPos[nCurIdx] = m_aWaitingAnisPos[nTotWaiting - 1];
			m_aWaitingAnisPos.pop_back();
			//
			m_aWaitingAnisStart[nCurIdx] = m_aWaitingAnisStart[nTotWaiting - 1];
			m_aWaitingAnisStart.pop_back();
			//
			m_aRunningAnis.push_back(std::move(m_aWaitingAnis[nCurIdx]));
			m_aWaitingAnis[nCurIdx] = m_aWaitingAnis[nTotWaiting - 1];
			m_aWaitingAnis.pop_back();

			++m_nStartedRunning;
		} else {
			++nCurIdx;
		}
	}
	}
	// handle running anis
	int32_t nCurIdx = 0;
	while (true) {
		const int32_t nTotRunning = static_cast<int32_t>(m_aRunningAnis.size());
		if (nCurIdx >= nTotRunning) {
			break;
		}
		shared_ptr<TileAni>& refTileAni = m_aRunningAnis[nCurIdx];
		assert(refTileAni);
		TileAni& oTileAni = *refTileAni;
		assert(! oTileAni.m_bWaiting);
		LevelBlock* p0LevelBlock = oTileAni.m_p0LevelBlock;
		if (p0LevelBlock == nullptr) {
			// can be moved from
			NPoint& oXY = m_aRunningAnisPos[nCurIdx];
			assert(m_oInit.m_oArea.containsPoint(oXY));
			const int32_t nOldCountdown = oTileAni.m_nCountdown;
			--oTileAni.m_nCountdown;
			const bool bEnded = (nOldCountdown <= 0);
			if (bEnded) {
				const int32_t nX = oXY.m_nX;
				const int32_t nY = oXY.m_nY;
				//
				++oTileAni.m_nCount;
				const bool bDelete = (oTileAni.m_nCount == oTileAni.m_nTotCount);
				if (bDelete) {
					assert(oLevel.boardGetTileAnimator(nX, nY, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
					oLevel.boardSetTileAnimator(nX, nY, m_oInit.m_nAniNameIdx, nullptr, 0);
					oLevel.boardAnimateTile(oXY);
					m_aNotAnisPos.push_back(std::move(oXY));
					//
				} else {
					oTileAni.m_bWaiting = true;
					const NRange oPause = m_oInit.m_oPause.getCumulatedTicksRange(fGameInterval);
					const int32_t nPause = oGame.random(oPause.m_nFrom, oPause.m_nTo);
					const int32_t nNextStartTick = [&]()
					{
						if (nPause <= std::numeric_limits<int32_t>::max() - nGameTick) {
							return nGameTick + nPause;
						} else {
							return std::numeric_limits<int32_t>::max();
						}
					}();
					oLevel.boardAnimateTile(oXY);
					// add to waiting
					m_aWaitingAnisPos.push_back(std::move(oXY));
					m_aWaitingAnisStart.push_back(nNextStartTick);
					m_aWaitingAnis.push_back(std::move(refTileAni));
				}
				// remove from running
				m_aRunningAnisPos[nCurIdx] = std::move(m_aRunningAnisPos[nTotRunning - 1]);
				m_aRunningAnisPos.pop_back();
				m_aRunningAnis[nCurIdx] = std::move(m_aRunningAnis[nTotRunning - 1]);
				m_aRunningAnis.pop_back();
			} else {
				oLevel.boardAnimateTile(oXY);
				++nCurIdx;
			}
		} else {
			NPoint& oBrickLBId = m_aRunningAnisPos[nCurIdx];
			const int32_t nBrickId = -1 - oBrickLBId.m_nX;
			#ifndef NDEBUG
			const int32_t nLBId = oBrickLBId.m_nY;
			assert(nLBId == p0LevelBlock->blockGetId());
			#endif //NDEBUG
			const int32_t nOldCountdown = oTileAni.m_nCountdown;
			--oTileAni.m_nCountdown;
			const bool bEnded = (nOldCountdown <= 0);
			if (bEnded) {
				++oTileAni.m_nCount;
				const bool bDelete = (oTileAni.m_nCount == oTileAni.m_nTotCount);
				if (bDelete) {
					assert(p0LevelBlock->blockGetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
					p0LevelBlock->blockSetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx, nullptr, 0);
					//TODO oLevel.blockAnimateTile(oBloData.m_p0LevelBlock, nBrick);
					m_aNotAnisPos.push_back(std::move(oBrickLBId));
					//
				} else {
					oTileAni.m_bWaiting = true;
					const NRange oPause = m_oInit.m_oPause.getCumulatedTicksRange(fGameInterval);
					const int32_t nPause = oGame.random(oPause.m_nFrom, oPause.m_nTo);
					const int32_t nNextStartTick = [&]()
					{
						if (nPause <= std::numeric_limits<int32_t>::max() - nGameTick) {
							return nGameTick + nPause;
						} else {
							return std::numeric_limits<int32_t>::max();
						}
					}();
					// oLevel.blockAnimateTile(oXY);
					// add to waiting
					m_aWaitingAnisPos.push_back(std::move(oBrickLBId));
					m_aWaitingAnisStart.push_back(nNextStartTick);
					m_aWaitingAnis.push_back(std::move(refTileAni));
				}
				// remove from running
				m_aRunningAnisPos[nCurIdx] = std::move(m_aRunningAnisPos[nTotRunning - 1]);
				m_aRunningAnisPos.pop_back();
				m_aRunningAnis[nCurIdx] = std::move(m_aRunningAnis[nTotRunning - 1]);
				m_aRunningAnis.pop_back();
			} else {
				// TODO add following method to Level! We shouldn't assume that blocks are redrawn each tick
				//oLevel.blockAnimateTile(oBloData.m_p0LevelBlock, nIdx, 1, 1);
				++nCurIdx;
			}
		}
	}
}

shared_ptr<TileAnimatorEvent::TileAni> TileAnimatorEvent::createTileAni(LevelBlock* p0LevelBlock) noexcept
{
	auto& oGame = level().game();
	shared_ptr<TileAni> refTileAni;
	m_oTileAniRecycler.create(refTileAni);
	TileAni& oTileAni = *refTileAni;
	oTileAni.m_p0LevelBlock = p0LevelBlock;
	oTileAni.m_bWaiting = true;
	oTileAni.m_nCount = 0;
	oTileAni.m_nTotCount = oGame.random(m_oInit.m_oTotCount.m_nFrom, m_oInit.m_oTotCount.m_nTo);
	oTileAni.m_nTotTicks = -1;
	oTileAni.m_nCountdown = -1;
	oTileAni.m_nAniNameIdx = m_oInit.m_nAniNameIdx;
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
	const double fElapsed = 1.0 + (- nCountdown + (0.5 + nViewTick) / nTotViewTicks) / nTotSteps;
	assert((fElapsed >= 0.0) && (fElapsed <= 1.0));
	return fElapsed;
}
double TileAnimatorEvent::TileAni::getElapsed01(int32_t /*nHash*/, int32_t
												#ifndef NDEBUG
												/*nX*/
												#endif //NDEBUG
												, int32_t
												#ifndef NDEBUG
												/*nY*/
												#endif //NDEBUG
												, int32_t nAni, int32_t nViewTick, int32_t nTotTicks) const noexcept
{
	assert(m_p0LevelBlock == nullptr);
	return getCommonElapsed(nAni, nViewTick, nTotTicks);
}
double TileAnimatorEvent::TileAni::getElapsed01(int32_t /*nHash*/, const LevelBlock&
									#ifndef NDEBUG
									oLevelBlock
									#endif //NDEBUG
									, int32_t /*nBrickIdx*/
									, int32_t nAni, int32_t nViewTick, int32_t nTotTicks) const noexcept
{
	assert(m_p0LevelBlock == &oLevelBlock);
	return getCommonElapsed(nAni, nViewTick, nTotTicks);
}

void TileAnimatorEvent::boardRemoveSelected(const NRect& oRect) noexcept
{
	Level& oLevel = level();
	int32_t nIdx = 0;
	while (nIdx < static_cast<int32_t>(m_aNotAnisPos.size())) {
		const NPoint& oXY = m_aNotAnisPos[nIdx];
		if ((oXY.m_nX < 0) || (! oRect.containsPoint(oXY))) {
			++nIdx;
			continue;
		}
		vectorRemoveIndex(m_aNotAnisPos, nIdx);
	}
	nIdx = 0;
	while (nIdx < static_cast<int32_t>(m_aWaitingAnisPos.size())) {
		const NPoint& oXY = m_aWaitingAnisPos[nIdx];
		if ((oXY.m_nX < 0) || (! oRect.containsPoint(oXY))) {
			++nIdx;
			continue;
		}
		#ifndef NDEBUG
		shared_ptr<TileAni>& refTileAni = m_aWaitingAnis[nIdx];
		#endif //NDEBUG
		assert(oLevel.boardGetTileAnimator(oXY.m_nX, oXY.m_nY, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
		oLevel.boardSetTileAnimator(oXY.m_nX, oXY.m_nY, m_oInit.m_nAniNameIdx, nullptr, 0);
		// do not animate !
		vectorRemoveIndex(m_aWaitingAnisPos, nIdx);
		vectorRemoveIndex(m_aWaitingAnisStart, nIdx);
		vectorRemoveIndex(m_aWaitingAnis, nIdx);
	}
	nIdx = 0;
	while (nIdx < static_cast<int32_t>(m_aRunningAnisPos.size())) {
		const NPoint& oXY = m_aRunningAnisPos[nIdx];
		if ((oXY.m_nX < 0) || (! oRect.containsPoint(oXY))) {
			++nIdx;
			continue;
		}
		#ifndef NDEBUG
		shared_ptr<TileAni>& refTileAni = m_aRunningAnis[nIdx];
		#endif //NDEBUG
		assert(oLevel.boardGetTileAnimator(oXY.m_nX, oXY.m_nY, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
		oLevel.boardSetTileAnimator(oXY.m_nX, oXY.m_nY, m_oInit.m_nAniNameIdx, nullptr, 0);
		oLevel.boardAnimateTile(oXY);
		
		vectorRemoveIndex(m_aRunningAnisPos, nIdx);
		vectorRemoveIndex(m_aRunningAnis, nIdx);
	}
}
void TileAnimatorEvent::boardRemoveSelected(const Coords& oCoords) noexcept
{
	Level& oLevel = level();
	int32_t nIdx = 0;
	while (nIdx < static_cast<int32_t>(m_aNotAnisPos.size())) {
		const NPoint& oXY = m_aNotAnisPos[nIdx];
		if ((oXY.m_nX < 0) || (! oCoords.contains(oXY))) {
			++nIdx;
			continue;
		}
		vectorRemoveIndex(m_aNotAnisPos, nIdx);
	}
	nIdx = 0;
	while (nIdx < static_cast<int32_t>(m_aWaitingAnisPos.size())) {
		const NPoint& oXY = m_aWaitingAnisPos[nIdx];
		if ((oXY.m_nX < 0) || (! oCoords.contains(oXY))) {
			++nIdx;
			continue;
		}
		#ifndef NDEBUG
		shared_ptr<TileAni>& refTileAni = m_aWaitingAnis[nIdx];
		#endif //NDEBUG
		assert(oLevel.boardGetTileAnimator(oXY.m_nX, oXY.m_nY, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
		oLevel.boardSetTileAnimator(oXY.m_nX, oXY.m_nY, m_oInit.m_nAniNameIdx, nullptr, 0);
		// do not animate !
		vectorRemoveIndex(m_aWaitingAnisPos, nIdx);
		vectorRemoveIndex(m_aWaitingAnisStart, nIdx);
		vectorRemoveIndex(m_aWaitingAnis, nIdx);
	}
	nIdx = 0;
	while (nIdx < static_cast<int32_t>(m_aRunningAnisPos.size())) {
		const NPoint& oXY = m_aRunningAnisPos[nIdx];
		if ((oXY.m_nX < 0) || (! oCoords.contains(oXY))) {
			++nIdx;
			continue;
		}
		#ifndef NDEBUG
		shared_ptr<TileAni>& refTileAni = m_aRunningAnis[nIdx];
		#endif //NDEBUG
		assert(oLevel.boardGetTileAnimator(oXY.m_nX, oXY.m_nY, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
		oLevel.boardSetTileAnimator(oXY.m_nX, oXY.m_nY, m_oInit.m_nAniNameIdx, nullptr, 0);
		oLevel.boardAnimateTile(oXY);
		vectorRemoveIndex(m_aRunningAnisPos, nIdx);
		vectorRemoveIndex(m_aRunningAnis, nIdx);
	}
}
void TileAnimatorEvent::boardAddSelected(const NRect& oRect) noexcept
{
	auto& oLevel = level();
	for (int32_t nY = oRect.m_nY; nY < oRect.m_nY + oRect.m_nH; ++nY) {
		for (int32_t nX = oRect.m_nX; nX < oRect.m_nX + oRect.m_nW; ++nX) {
			const Tile& oTile = oLevel.boardGetTile(nX, nY);
			if ((!oTile.isEmpty()) && ((!m_oInit.m_refSelect) || m_oInit.m_refSelect->select(oTile))) {
				m_aNotAnisPos.push_back(NPoint{nX,  nY});
			}
		}
	}
}
void TileAnimatorEvent::boardAddSelected(const Coords& oCoords) noexcept
{
	auto& oLevel = level();
	for (Coords::const_iterator it = oCoords.begin(); it != oCoords.end(); it.next()) {
		const int32_t nX = it.x();
		const int32_t nY = it.y();
		const Tile& oTile = oLevel.boardGetTile(nX, nY);
		if ((!oTile.isEmpty()) && ((!m_oInit.m_refSelect) || m_oInit.m_refSelect->select(oTile))) {
			m_aNotAnisPos.push_back(NPoint{nX,  nY});
		}
	}
}
void TileAnimatorEvent::boardMoveSelected(const NRect& oRect, Direction::VALUE eDir) noexcept
{
	const int32_t nDx = Direction::deltaX(eDir);
	const int32_t nDy = Direction::deltaY(eDir);
	for (NPoint& oXY : m_aNotAnisPos) {
		if ((oXY.m_nX < 0) || (! oRect.containsPoint(oXY))) {
			continue;
		}
		oXY.m_nX += nDx;
		oXY.m_nY += nDy;
	}
	#ifndef NDEBUG
	auto& oLevel = level();
	int32_t nIdx = 0;
	#endif //NDEBUG
	for (NPoint& oXY : m_aWaitingAnisPos) {
		if ((oXY.m_nX < 0) || (! oRect.containsPoint(oXY))) {
			#ifndef NDEBUG
			++nIdx;
			#endif //NDEBUG
			continue;
		}
		oXY.m_nX += nDx;
		oXY.m_nY += nDy;
		#ifndef NDEBUG
		shared_ptr<TileAni>& refTileAni = m_aWaitingAnis[nIdx];
		assert(oLevel.boardGetTileAnimator(oXY.m_nX, oXY.m_nY, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
		++nIdx;
		#endif //NDEBUG
	}
	#ifndef NDEBUG
	nIdx = 0;
	#endif //NDEBUG
	for (NPoint& oXY : m_aRunningAnisPos) {
		if ((oXY.m_nX < 0) || (! oRect.containsPoint(oXY))) {
			#ifndef NDEBUG
			++nIdx;
			#endif //NDEBUG
			continue;
		}
		oXY.m_nX += nDx;
		oXY.m_nY += nDy;
		#ifndef NDEBUG
		shared_ptr<TileAni>& refTileAni = m_aRunningAnis[nIdx];
		assert(oLevel.boardGetTileAnimator(oXY.m_nX, oXY.m_nY, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
		oLevel.boardAnimateTile(oXY);
		++nIdx;
		#endif //NDEBUG
	}
}

void TileAnimatorEvent::boabloPreFreeze(LevelBlock& oLevelBlock) noexcept
{
	if (m_oInit.m_bDoBlocks) {
		blockRemoveSelected(oLevelBlock, s_aEmptyVector);// TODO transfer Block's tile anis to Board (if m_bDoBoard)
	}
}
void TileAnimatorEvent::boabloPostFreeze(const Coords& oCoords) noexcept
{
	if (m_oInit.m_bDoBoard) {
		boardAddSelected(oCoords);
	}
}
void TileAnimatorEvent::boabloPreUnfreeze(const Coords& oCoords) noexcept
{
	if (m_oInit.m_bDoBoard) {
		boardRemoveSelected(oCoords);
	}
	//TODO transfer Board's tile anis to Block (if m_bDoBlocks)
}
void TileAnimatorEvent::boabloPostUnfreeze(LevelBlock& oLevelBlock) noexcept
{
	if (m_oInit.m_bDoBlocks) {
		blockAddSelected(oLevelBlock);
	}
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
void TileAnimatorEvent::boardPreInsert(Direction::VALUE eDir, NRect oRect, const shared_ptr<TileRect>& /*refTiles*/) noexcept
{
	const NRect oRemoveArea = Helpers::boardInsertInAreaRemove(oRect, eDir, m_oInit.m_oArea);
	if (oRemoveArea.m_nW > 0) {
		boardRemoveSelected(oRemoveArea);
	}
}
void TileAnimatorEvent::boardPostInsert(Direction::VALUE eDir, NRect oRect) noexcept
{
	const NRect oMovedRect = Helpers::boardInsertInAreaMovingPre(oRect, eDir, m_oInit.m_oArea);
	boardMoveSelected(oMovedRect, eDir);
	const NRect oAddRect = Helpers::boardInsertInAreaAdd(oRect, eDir, m_oInit.m_oArea);
	boardAddSelected(oAddRect);
}

void TileAnimatorEvent::boardPreDestroy(const Coords& oCoords) noexcept
{
//std::cout << "TileAnimatorEvent::boardPreDestroy()" << '\n';
	boardRemoveSelected(oCoords);
}
void TileAnimatorEvent::boardPostDestroy(const Coords& /*oCoords*/) noexcept
{
//std::cout << "TileAnimatorEvent::boardPostDestroy()" << '\n';
}
void TileAnimatorEvent::boardPreModify(const TileCoords& oTileCoords) noexcept
{
	boardRemoveSelected(oTileCoords);
}
void TileAnimatorEvent::boardPostModify(const Coords& oCoords) noexcept
{
//std::cout << "TileAnimatorEvent::boardPostModify()" << '\n';
	boardAddSelected(oCoords);
}


void TileAnimatorEvent::blockAddSelectedBrick(LevelBlock& oLevelBlock, int32_t nBrickId, int32_t nBlockId) noexcept
{
//std::cout << "TileAnimatorEvent::blockAddSelectedBrick" << '\n';
	const Tile& oTile = oLevelBlock.blockBrickTile(nBrickId);
	assert(!oTile.isEmpty());
	const bool bIsSelected = ((!m_oInit.m_refSelect) || m_oInit.m_refSelect->select(oTile));
	if (bIsSelected) {
		if (oLevelBlock.blockBrickVisible(nBrickId)) {
			m_aNotAnisPos.push_back(NPoint{-1 - nBrickId,  nBlockId});
		}
	}
}
void TileAnimatorEvent::blockAddSelected(LevelBlock& oLevelBlock) noexcept
{
//std::cout << "TileAnimatorEvent::blockAddSelected" << '\n';

	const int32_t nLBId = oLevelBlock.blockGetId();
	auto& aBrickId = oLevelBlock.blockBrickIds();
	for (auto& nBrickId : aBrickId) {
		blockAddSelectedBrick(oLevelBlock, nBrickId, nLBId);
	}
}
void TileAnimatorEvent::blockRemoveSelected(LevelBlock& oLevelBlock, const std::vector<int32_t>& aRemoveBrickId) noexcept
{
	const bool bRemoveAll = aRemoveBrickId.empty();
	const int32_t nLBId = oLevelBlock.blockGetId();
	int32_t nIdx = 0;
	while (nIdx < static_cast<int32_t>(m_aNotAnisPos.size())) {
		const NPoint& oBrickBlockId = m_aNotAnisPos[nIdx];
		if ((oBrickBlockId.m_nX >= 0) || (nLBId != oBrickBlockId.m_nY)) {
			++nIdx;
			continue;
		}
		const int32_t nBrickId = -1 - oBrickBlockId.m_nX;
		if (bRemoveAll || Util::vectorContains(aRemoveBrickId, nBrickId)) {
			vectorRemoveIndex(m_aNotAnisPos, nIdx);
		} else {
			++nIdx;
		}
	}
	nIdx = 0;
	while (nIdx < static_cast<int32_t>(m_aWaitingAnisPos.size())) {
		const NPoint& oBrickBlockId = m_aWaitingAnisPos[nIdx];
		if ((oBrickBlockId.m_nX >= 0) || (nLBId != oBrickBlockId.m_nY)) {
			++nIdx;
			continue;
		}
		const int32_t nBrickId = -1 - oBrickBlockId.m_nX;
		if (bRemoveAll || Util::vectorContains(aRemoveBrickId, nBrickId)) {
			#ifndef NDEBUG
			shared_ptr<TileAni>& refTileAni = m_aWaitingAnis[nIdx];
			#endif //NDEBUG
			assert(oLevelBlock.blockGetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
			oLevelBlock.blockSetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx, nullptr, 0);
			//
			vectorRemoveIndex(m_aWaitingAnisPos, nIdx);
			vectorRemoveIndex(m_aWaitingAnisStart, nIdx);
			vectorRemoveIndex(m_aWaitingAnis, nIdx);
		} else {
			++nIdx;
		}
	}
	nIdx = 0;
	while (nIdx < static_cast<int32_t>(m_aRunningAnisPos.size())) {
		const NPoint& oBrickBlockId = m_aRunningAnisPos[nIdx];
		if ((oBrickBlockId.m_nX >= 0) || (nLBId != oBrickBlockId.m_nY)) {
			++nIdx;
			continue;
		}
		const int32_t nBrickId = -1 - oBrickBlockId.m_nX;
		if (bRemoveAll || Util::vectorContains(aRemoveBrickId, nBrickId)) {
			#ifndef NDEBUG
			shared_ptr<TileAni>& refTileAni = m_aRunningAnis[nIdx];
			#endif //NDEBUG
			assert(oLevelBlock.blockGetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
			oLevelBlock.blockSetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx, nullptr, 0);
			//
			vectorRemoveIndex(m_aRunningAnisPos, nIdx);
			vectorRemoveIndex(m_aRunningAnis, nIdx);
		} else {
			++nIdx;
		}
	}
}

void TileAnimatorEvent::blockPreAdd(const LevelBlock& /*oBlock*/) noexcept
{
}
void TileAnimatorEvent::blockPostAdd(LevelBlock& oLevelBlock) noexcept
{
//std::cout << "TileAnimatorEvent::blockPostAdd  id=" << oLevelBlock.blockGetId() << '\n';
	blockAddSelected(oLevelBlock);
}
void TileAnimatorEvent::blockPreRemove(LevelBlock& oLevelBlock) noexcept
{
//std::cout << "TileAnimatorEvent::blockPreRemove id=" << oLevelBlock.blockGetId() << '\n';
	blockRemoveSelected(oLevelBlock, s_aEmptyVector);
}
void TileAnimatorEvent::blockPostRemove(const LevelBlock& /*oLevelBlock*/) noexcept
{
}
void TileAnimatorEvent::blockPreDestroy(LevelBlock& oBlock) noexcept
{
//std::cout << "TileAnimatorEvent::blockPreDestroy" << '\n';
	blockRemoveSelected(oBlock, s_aEmptyVector);
}
void TileAnimatorEvent::blockPostDestroy(const LevelBlock& /*oBlock*/) noexcept
{
//std::cout << "TileAnimatorEvent::blockPostDestroy" << '\n';
}

void TileAnimatorEvent::blockPreFuse(LevelBlock& /*oFusedToBlock*/, LevelBlock& /*oFusedBlock*/) noexcept
{
}
void TileAnimatorEvent::blockPostFuse(LevelBlock& oResBlock, const LevelBlock& oFusedBlock
									, const std::unordered_map<int32_t, int32_t>& oFusedToBrickIds
									, const std::unordered_map<int32_t, int32_t>& oFusedBrickIds) noexcept
{
//std::cout << "TileAnimatorEvent::blockPostFuse" << '\n';

	const int32_t nResLBId = oResBlock.blockGetId();
	const int32_t nFusedLBId = oFusedBlock.blockGetId();
	for (NPoint& oBrickBlockId : m_aNotAnisPos) {
		if (oBrickBlockId.m_nX >= 0) {
			continue;
		}
		const int32_t nBrickId = -1 - oBrickBlockId.m_nX;
		const int32_t nLBId = oBrickBlockId.m_nY;
		if (nResLBId == nLBId) {
			const auto itFind = oFusedToBrickIds.find(nBrickId);
			assert(itFind != oFusedToBrickIds.end());
			const int32_t nNewBrickId = itFind->second;
			oBrickBlockId.m_nX = -1 - nNewBrickId;
		} else if (nFusedLBId == nLBId) {
			const auto itFind = oFusedBrickIds.find(nBrickId);
			assert(itFind != oFusedBrickIds.end());
			const int32_t nNewBrickId = itFind->second;
			oBrickBlockId.m_nX = -1 - nNewBrickId;
			oBrickBlockId.m_nY = nResLBId;
		}
	}
	for (int32_t nIdx = 0; nIdx < static_cast<int32_t>(m_aWaitingAnisPos.size()); ++nIdx) {
		NPoint& oBrickBlockId = m_aWaitingAnisPos[nIdx];
		if (oBrickBlockId.m_nX >= 0) {
			continue;
		}
		const int32_t nBrickId = -1 - oBrickBlockId.m_nX;
		const int32_t nLBId = oBrickBlockId.m_nY;
		if (nResLBId == nLBId) {
			const auto itFind = oFusedToBrickIds.find(nBrickId);
			assert(itFind != oFusedToBrickIds.end());
			const int32_t nNewBrickId = itFind->second;
			oBrickBlockId.m_nX = -1 - nNewBrickId;
			shared_ptr<TileAni>& refTileAni = m_aWaitingAnis[nIdx];
			#ifndef NDEBUG
			assert(refTileAni->m_p0LevelBlock == &oResBlock);
			assert(oResBlock.blockGetTileAnimator(nNewBrickId, m_oInit.m_nAniNameIdx) == nullptr);
			#endif //NDEBUG
			oResBlock.blockSetTileAnimator(nNewBrickId, m_oInit.m_nAniNameIdx, refTileAni.get(), 0);
		} else if (nFusedLBId == nLBId) {
			const auto itFind = oFusedBrickIds.find(nBrickId);
			assert(itFind != oFusedBrickIds.end());
			const int32_t nNewBrickId = itFind->second;
			oBrickBlockId.m_nX = -1 - nNewBrickId;
			oBrickBlockId.m_nY = nResLBId;
			shared_ptr<TileAni>& refTileAni = m_aWaitingAnis[nIdx];
			refTileAni->m_p0LevelBlock = &oResBlock;
			assert(oResBlock.blockGetTileAnimator(nNewBrickId, m_oInit.m_nAniNameIdx) == nullptr);
			oResBlock.blockSetTileAnimator(nNewBrickId, m_oInit.m_nAniNameIdx, refTileAni.get(), 0);
		}
	}
	for (int32_t nIdx = 0; nIdx < static_cast<int32_t>(m_aRunningAnisPos.size()); ++nIdx) {
		NPoint& oBrickBlockId = m_aRunningAnisPos[nIdx];
		if (oBrickBlockId.m_nX >= 0) {
			continue;
		}
		const int32_t nBrickId = -1 - oBrickBlockId.m_nX;
		const int32_t nLBId = oBrickBlockId.m_nY;
		if (nResLBId == nLBId) {
			const auto itFind = oFusedToBrickIds.find(nBrickId);
			assert(itFind != oFusedToBrickIds.end());
			const int32_t nNewBrickId = itFind->second;
			oBrickBlockId.m_nX = -1 - nNewBrickId;
			shared_ptr<TileAni>& refTileAni = m_aRunningAnis[nIdx];
			#ifndef NDEBUG
			assert(refTileAni->m_p0LevelBlock == &oResBlock);
			assert(oResBlock.blockGetTileAnimator(nNewBrickId, m_oInit.m_nAniNameIdx) == nullptr);
			#endif //NDEBUG
			oResBlock.blockSetTileAnimator(nNewBrickId, m_oInit.m_nAniNameIdx, refTileAni.get(), 0);
		} else if (nFusedLBId == nLBId) {
			const auto itFind = oFusedBrickIds.find(nBrickId);
			assert(itFind != oFusedBrickIds.end());
			const int32_t nNewBrickId = itFind->second;
			oBrickBlockId.m_nX = -1 - nNewBrickId;
			oBrickBlockId.m_nY = nResLBId;
			shared_ptr<TileAni>& refTileAni = m_aRunningAnis[nIdx];
			refTileAni->m_p0LevelBlock = &oResBlock;
			assert(oResBlock.blockGetTileAnimator(nNewBrickId, m_oInit.m_nAniNameIdx) == nullptr);
			oResBlock.blockSetTileAnimator(nNewBrickId, m_oInit.m_nAniNameIdx, refTileAni.get(), 0);
		}
	}
}
void TileAnimatorEvent::blockPreModify(LevelBlock& oBlock
										, const std::vector<int32_t>& aDeleteBrickId
										, const std::vector<int32_t>& /*aModifyPosBrickId*/
										, const std::vector<int32_t>& /*aModifyTileBrickId*/
										, bool /*bAddsBricks*/) noexcept
{
//std::cout << "TileAnimatorEvent::blockPreModify" << '\n';

	blockRemoveSelected(oBlock, aDeleteBrickId);
}
void TileAnimatorEvent::blockPostModify(LevelBlock& oLevelBlock
										, const std::vector<int32_t>& /*aDeletedBrickId*/
										, const std::vector<int32_t>& aModifiedPosBrickId
										, const std::vector<int32_t>& aModifiedTileBrickId
										, const std::vector<int32_t>& aAddedBrickId) noexcept
{
//std::cout << "TileAnimatorEvent::blockPostModify" << '\n';

	const int32_t nLBId = oLevelBlock.blockGetId();
	int32_t nIdx = 0;
	while (nIdx < static_cast<int32_t>(m_aNotAnisPos.size())) {
		const NPoint& oBrickBlockId = m_aNotAnisPos[nIdx];
		if ((oBrickBlockId.m_nX >= 0) || (nLBId != oBrickBlockId.m_nY)) {
			++nIdx;
			continue;
		}
		const int32_t nBrickId = -1 - oBrickBlockId.m_nX;
		if (Util::vectorContains(aModifiedPosBrickId, nBrickId)) {
			if (! oLevelBlock.blockBrickVisible(nBrickId)) {
				vectorRemoveIndex(m_aNotAnisPos, nIdx);
				continue;
			}
		}
		if (Util::vectorContains(aModifiedTileBrickId, nBrickId)) {
			const Tile& oTile = oLevelBlock.blockBrickTile(nBrickId);
			assert(!oTile.isEmpty());
			const bool bIsSelected = ((!m_oInit.m_refSelect) || m_oInit.m_refSelect->select(oTile));
			if ((! bIsSelected) || ! oLevelBlock.blockBrickVisible(nBrickId)) {
				vectorRemoveIndex(m_aNotAnisPos, nIdx);
				continue;
			}
		}
		++nIdx;
	}
	nIdx = 0;
	while (nIdx < static_cast<int32_t>(m_aWaitingAnisPos.size())) {
		const NPoint& oBrickBlockId = m_aWaitingAnisPos[nIdx];
		if ((oBrickBlockId.m_nX >= 0) || (nLBId != oBrickBlockId.m_nY)) {
			++nIdx;
			continue;
		}
		const int32_t nBrickId = -1 - oBrickBlockId.m_nX;
		if (Util::vectorContains(aModifiedPosBrickId, nBrickId)) {
			if (! oLevelBlock.blockBrickVisible(nBrickId)) {
				#ifndef NDEBUG
				shared_ptr<TileAni>& refTileAni = m_aWaitingAnis[nIdx];
				#endif //NDEBUG
				assert(oLevelBlock.blockGetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
				oLevelBlock.blockSetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx, nullptr, 0);
				//
				vectorRemoveIndex(m_aWaitingAnisPos, nIdx);
				vectorRemoveIndex(m_aWaitingAnisStart, nIdx);
				vectorRemoveIndex(m_aWaitingAnis, nIdx);
				continue;
			}
		}
		if (Util::vectorContains(aModifiedTileBrickId, nBrickId)) {
			const Tile& oTile = oLevelBlock.blockBrickTile(nBrickId);
			assert(!oTile.isEmpty());
			const bool bIsSelected = ((!m_oInit.m_refSelect) || m_oInit.m_refSelect->select(oTile));
			if ((! bIsSelected) || ! oLevelBlock.blockBrickVisible(nBrickId)) {
				#ifndef NDEBUG
				shared_ptr<TileAni>& refTileAni = m_aWaitingAnis[nIdx];
				#endif //NDEBUG
				assert(oLevelBlock.blockGetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
				oLevelBlock.blockSetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx, nullptr, 0);
				//
				vectorRemoveIndex(m_aWaitingAnisPos, nIdx);
				vectorRemoveIndex(m_aWaitingAnisStart, nIdx);
				vectorRemoveIndex(m_aWaitingAnis, nIdx);
				continue;
			}
		}
		++nIdx;
	}
	nIdx = 0;
	while (nIdx < static_cast<int32_t>(m_aRunningAnisPos.size())) {
		const NPoint& oBrickBlockId = m_aRunningAnisPos[nIdx];
		if ((oBrickBlockId.m_nX >= 0) || (nLBId != oBrickBlockId.m_nY)) {
			++nIdx;
			continue;
		}
		const int32_t nBrickId = -1 - oBrickBlockId.m_nX;
		if (Util::vectorContains(aModifiedPosBrickId, nBrickId)) {
			if (! oLevelBlock.blockBrickVisible(nBrickId)) {
				#ifndef NDEBUG
				shared_ptr<TileAni>& refTileAni = m_aRunningAnis[nIdx];
				#endif //NDEBUG
				assert(oLevelBlock.blockGetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
				oLevelBlock.blockSetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx, nullptr, 0);
				//
				vectorRemoveIndex(m_aRunningAnisPos, nIdx);
				vectorRemoveIndex(m_aRunningAnis, nIdx);
				continue;
			}
		}
		if (Util::vectorContains(aModifiedTileBrickId, nBrickId)) {
			const Tile& oTile = oLevelBlock.blockBrickTile(nBrickId);
			assert(!oTile.isEmpty());
			const bool bIsSelected = ((!m_oInit.m_refSelect) || m_oInit.m_refSelect->select(oTile));
			if ((! bIsSelected) || ! oLevelBlock.blockBrickVisible(nBrickId)) {
				#ifndef NDEBUG
				shared_ptr<TileAni>& refTileAni = m_aRunningAnis[nIdx];
				#endif //NDEBUG
				assert(oLevelBlock.blockGetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx) == refTileAni.operator->());
				oLevelBlock.blockSetTileAnimator(nBrickId, m_oInit.m_nAniNameIdx, nullptr, 0);
				//
				vectorRemoveIndex(m_aRunningAnisPos, nIdx);
				vectorRemoveIndex(m_aRunningAnis, nIdx);
				continue;
			}
		}
		++nIdx;
	}

	//
	for (auto& nBrickId : aAddedBrickId) {
		blockAddSelectedBrick(oLevelBlock, nBrickId, nLBId);
	}
}


} // namespace stmg
