/*
 * File:   positionerevent.cc
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

#include "events/positionerevent.h"

#include "level.h"
#include "gameproxy.h"
#include "levelblock.h"
#include "levelshow.h"
#include "util/basictypes.h"

#include <vector>
#include <cassert>
#include <algorithm>
//#include <iostream>
#include <memory>
#include <utility>

#include <stdint.h>

namespace stmg
{

PositionerEvent::PositionerEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
{
	initCommon(std::move(oInit));
}
void PositionerEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	initCommon(std::move(oInit));
}
void PositionerEvent::initCommon(LocalInit&& oInit) noexcept
{
	Level& oLevel = level();
	m_oData = std::move(oInit);
	assert((m_oData.m_oTrackingRect.m_nW > 0) && (m_oData.m_oTrackingRect.m_nH > 0));
	assert(m_oData.m_nCheckEachTicks > 0);
	m_bSubshowMode = oLevel.subshowMode();
	m_nTotLevelPlayers = oLevel.getTotLevelPlayers();
	m_aShowPositions.resize(m_bSubshowMode ? m_nTotLevelPlayers : 1);
	if (m_bSubshowMode) {
		for (int32_t nLevelPlayer = 0; nLevelPlayer < m_nTotLevelPlayers; ++nLevelPlayer) {
			auto& oShowPosition = m_aShowPositions[nLevelPlayer];
			oShowPosition.m_p0LevelShow = &(oLevel.subshowGet(nLevelPlayer));
			oShowPosition.m_aLevelBlockIds.clear();
			oShowPosition.m_nGameTicksToTarget = -1;
			oShowPosition.m_oPos = FPoint{};
			oShowPosition.m_oTargetPos = FPoint{};
		}
		m_oParentSize.m_nW = oLevel.showGet().getW();
		m_oParentSize.m_nH = oLevel.showGet().getH();
	} else {
		auto& oShowPosition = m_aShowPositions[0];
		oShowPosition.m_p0LevelShow = &(oLevel.showGet());
		oShowPosition.m_aLevelBlockIds.clear();
		oShowPosition.m_nGameTicksToTarget = -1;
		oShowPosition.m_oPos = FPoint{};
		oShowPosition.m_oTargetPos = FPoint{};
		m_oParentSize.m_nW = oLevel.boardWidth();
		m_oParentSize.m_nH = oLevel.boardHeight();
	}
	#ifndef NDEBUG
	NRect oContextRect;
	if (m_bSubshowMode) {
		// The size of the subshow is the same for all the players
		oContextRect.m_nW = oLevel.subshowGet(0).getW();
		oContextRect.m_nH = oLevel.subshowGet(0).getH();
	} else {
		oContextRect.m_nW = oLevel.showGet().getW();
		oContextRect.m_nH = oLevel.showGet().getH();
	}
	assert(NRect::boundingRect(m_oData.m_oTrackingRect, oContextRect) == oContextRect);
	#endif //NDEBUG
	m_bPaused = false;
	m_eState = POSITIONER_STATE_ACTIVATE;
	m_nLastCheck = -1;
	m_nTotTransitioning = 0;
}

void PositionerEvent::trigger(int32_t nMsg, int32_t /*nValue*/, Event* p0TriggeringEvent) noexcept
{
	Level& oLevel = level();
	const int32_t nGameTick = oLevel.game().gameElapsed();
	switch (m_eState) {
	case POSITIONER_STATE_ACTIVATE:
	{
		m_eState = POSITIONER_STATE_INIT;
		if (p0TriggeringEvent != nullptr) {
			oLevel.activateEvent(this, nGameTick);
			return; //----------------------------------------------------------
		}
	} // fallthrough
	case POSITIONER_STATE_INIT:
	{
		if (p0TriggeringEvent != nullptr) {
			oLevel.activateEvent(this, nGameTick);
			return; //----------------------------------------------------------
		}
		m_nLastCheck = nGameTick - m_oData.m_nCheckEachTicks;
		initTracking();
		m_eState = POSITIONER_STATE_TRACKING;
	} // fallthrough
	case POSITIONER_STATE_TRACKING:
	{
		if (p0TriggeringEvent != nullptr) {
			if (m_bPaused) {
				if (nMsg == MESSAGE_RESUME_TRACKING) {
					oLevel.activateEvent(this, nGameTick);
				}
			} else {
				if (nMsg == MESSAGE_PAUSE_TRACKING) {
					m_bPaused = true;
				}
			}
			return; //----------------------------------------------------------
		}
		if (m_nTotTransitioning > 0) {
			handleTransitions();
		}
		if (nGameTick >= m_nLastCheck + m_oData.m_nCheckEachTicks) {
			checkNewPositions();
			m_nLastCheck = nGameTick;
		}
		if (m_nTotTransitioning > 0) {
			oLevel.activateEvent(this, nGameTick + 1);
		} else {
			oLevel.activateEvent(this, m_nLastCheck + m_oData.m_nCheckEachTicks);
		}
	}
	break;
	}
}
void PositionerEvent::initTracking() noexcept
{
//std::cout << "PositionerEvent::initTracking()" << '\n';
	auto aLBs = level().blocksGetAll();
	for (LevelBlock*& p0LevelBlock : aLBs) {
		const int32_t nLevelPlayer = p0LevelBlock->getPlayer();
//std::cout << "PositionerEvent::initTracking()  nLevelPlayer=" << nLevelPlayer << '\n';
		if (nLevelPlayer >= 0) {
			if (m_bSubshowMode) {
				assert(nLevelPlayer < static_cast<int32_t>(m_aShowPositions.size()));
				auto& aLBIds = m_aShowPositions[nLevelPlayer].m_aLevelBlockIds;
				assert(aLBIds.empty());
				const int32_t nLBId = p0LevelBlock->blockGetId();
				aLBIds.push_back(nLBId);
//std::cout << "PositionerEvent::initTracking()  add nLBId=" << nLBId << '\n';
			} else {
				auto& aLBIds = m_aShowPositions[0].m_aLevelBlockIds;
				const int32_t nLBId = p0LevelBlock->blockGetId();
				assert(std::find(aLBIds.begin(), aLBIds.end(), nLBId) == aLBIds.end());
				aLBIds.push_back(nLBId);
			}
		}
	}
	level().blocksAddPlayerChangeListener(this);
	for (auto& oShowPosition : m_aShowPositions) {
		auto& p0LS = oShowPosition.m_p0LevelShow;
		oShowPosition.m_oPos = p0LS->getPos();
		p0LS->setPositioner(&oShowPosition);
	}
}
void PositionerEvent::handleTransitions() noexcept
{
//std::cout << "PositionerEvent::handleTransitions()" << '\n';
	for (auto& oShowPosition : m_aShowPositions) {
		if (oShowPosition.m_nGameTicksToTarget > 0) {
			--oShowPosition.m_nGameTicksToTarget;
			if (oShowPosition.m_nGameTicksToTarget == 0) {
				// reached target
				oShowPosition.m_oPos = oShowPosition.m_oTargetPos;
				oShowPosition.m_nGameTicksToTarget = -1;
				--m_nTotTransitioning;
			} else {
				const double fPX = oShowPosition.m_oPos.m_fX;
				const double fTX = oShowPosition.m_oTargetPos.m_fX;
				const double fX = fPX + (fTX - fPX) / (oShowPosition.m_nGameTicksToTarget + 1);
				const double fPY = oShowPosition.m_oPos.m_fY;
				const double fTY = oShowPosition.m_oTargetPos.m_fY;
				const double fY = fPY + (fTY - fPY) / (oShowPosition.m_nGameTicksToTarget + 1);
				oShowPosition.m_oPos = FPoint{fX, fY};
			}
		} else {
			assert(oShowPosition.m_nGameTicksToTarget == -1);
		}
//std::cout << "PositionerEvent::handleTransitions() oShowPosition.m_oPos=(" << oShowPosition.m_oPos.m_fX << "," << oShowPosition.m_oPos.m_fY << ")" << '\n';
	}
}
void PositionerEvent::checkNewPositions() noexcept
{
//std::cout << "PositionerEvent::checkNewPositions()" << '\n';
	Level& oLevel = level();
	for (auto& oShowPosition : m_aShowPositions) {
		auto& aLBIds = oShowPosition.m_aLevelBlockIds;
		bool bRect = false;
		NRect oRect;
		for (int32_t nLBId : aLBIds) {
			LevelBlock* p0LB = oLevel.blocksGet(nLBId);
			assert(p0LB != nullptr);
//std::cout << "PositionerEvent::checkNewPositions()  nLBId=" << nLBId << '\n';
			if (p0LB->blockBricksTotVisible() > 0) {
				const NPoint oLBPos = p0LB->blockPos();
				const NPoint oLBBricksMinPos = p0LB->blockBricksMinPos();
				const NSize oLBSize = p0LB->blockSize();
				NRect oLBRect;
				oLBRect.m_nX = oLBPos.m_nX + oLBBricksMinPos.m_nX;
				oLBRect.m_nY = oLBPos.m_nY + oLBBricksMinPos.m_nY;
				oLBRect.m_nW = oLBSize.m_nW;
				oLBRect.m_nH = oLBSize.m_nH;
				if (bRect) {
					oRect = NRect::boundingRect(oRect, oLBRect);
				} else {
					oRect = oLBRect;
					bRect = true;
				}
			}
//std::cout << "PositionerEvent::checkNewPositions() bRect=" << bRect << " oRect=(" << oRect.m_nX << "," << oRect.m_nY << "," << oRect.m_nW << "," << oRect.m_nH << ")" << '\n';
		}
		if (!bRect) {
			if (oShowPosition.m_oLastRect.m_nW > 0) {
				oRect = oShowPosition.m_oLastRect;
				oShowPosition.m_oLastRect.m_nW = 0;
			} else {
				continue; // for(oShowPosition) ------
			}
		}
		const double fCorrX = (m_bSubshowMode ? oLevel.showGet().getPos().m_fX : 0.0);
		const double fCorrY = (m_bSubshowMode ? oLevel.showGet().getPos().m_fY : 0.0);
//std::cout << "PositionerEvent::checkNewPositions() fCorrX=" << fCorrX << " fCorrY=" << fCorrY << '\n';
		const double fShowBoardPosX = oShowPosition.m_oPos.m_fX + fCorrX;
		const double fShowBoardPosY = oShowPosition.m_oPos.m_fY + fCorrY;
//std::cout << "PositionerEvent::checkNewPositions() fShowBoardPosX=" << fShowBoardPosX << " fShowBoardPosY=" << fShowBoardPosY << '\n';
		const bool bLBsInsideX = ((oRect.m_nX >= fShowBoardPosX + m_oData.m_oTrackingRect.m_nX)
								&& (oRect.m_nX + oRect.m_nW <= fShowBoardPosX + m_oData.m_oTrackingRect.m_nX + m_oData.m_oTrackingRect.m_nW));
		const bool bLBsInsideY = ((oRect.m_nY >= fShowBoardPosY + m_oData.m_oTrackingRect.m_nY)
								&& (oRect.m_nY + oRect.m_nH <= fShowBoardPosY + m_oData.m_oTrackingRect.m_nY + m_oData.m_oTrackingRect.m_nH));
//std::cout << "PositionerEvent::checkNewPositions() bLBsInsideX=" << bLBsInsideX << " bLBsInsideY=" << bLBsInsideY << '\n';
		if (bLBsInsideX && bLBsInsideY) {
			continue; // for(oShowPosition)
		}
		auto& p0LS = oShowPosition.m_p0LevelShow;
		double fTargetX = oShowPosition.m_oPos.m_fX;
		if (!bLBsInsideX) {
			const double fDx = 0.5 * (m_oData.m_oTrackingRect.m_nW - oRect.m_nW);
			const double fNewTRx = 0.0 + oRect.m_nX - fDx;
			fTargetX = fNewTRx - m_oData.m_oTrackingRect.m_nX;
//std::cout << "PositionerEvent::checkNewPositions() fDx=" << fDx << " fNewTRx=" << fNewTRx << " fTargetX=" << fTargetX << '\n';
//std::cout << "PositionerEvent::checkNewPositions() m_oParentSize.m_nW=" << m_oParentSize.m_nW << " p0LS->getW()=" << p0LS->getW() << '\n';
			fTargetX = std::min(std::max(0.0, fTargetX), 0.0 + m_oParentSize.m_nW - p0LS->getW());
		}
		double fTargetY = oShowPosition.m_oPos.m_fY;
		if (!bLBsInsideY) {
			const double fDy = 0.5 * (m_oData.m_oTrackingRect.m_nH - oRect.m_nH);
			const double fNewTRy = 0.0 + oRect.m_nY - fDy;
			fTargetY = fNewTRy - m_oData.m_oTrackingRect.m_nY;
			fTargetY = std::min(std::max(0.0, fTargetY), 0.0 + m_oParentSize.m_nH - p0LS->getH());
		}
//std::cout << "PositionerEvent::checkNewPositions() fTargetX=" << fTargetX << " fTargetY=" << fTargetY << '\n';
		if ((oShowPosition.m_oPos.m_fX == fTargetX) && (oShowPosition.m_oPos.m_fY == fTargetY)) {
			continue; // for(oShowPosition)
		}
		oShowPosition.m_oTargetPos = FPoint{fTargetX, fTargetY};
		oShowPosition.m_nGameTicksToTarget = m_oData.m_nTransitionTicks;
		++m_nTotTransitioning;
//std::cout << "PositionerEvent::checkNewPositions() m_nTotTransitioning=" << m_nTotTransitioning << '\n';
	}	
}
FPoint PositionerEvent::ShowPosition::getPos(int32_t nViewTick, int32_t nTotViewTicks) noexcept
{
//std::cout << "PositionerEvent::ShowPosition::getPos() nViewTick=" << nViewTick << " nTotViewTicks=" << nTotViewTicks << '\n';
//std::cout << "PositionerEvent::ShowPosition::getPos() m_oTargetPos.m_fY=" << m_oTargetPos.m_fY << " m_oPos.m_fY=" << m_oPos.m_fY << '\n';
	assert(nTotViewTicks >= 1);
	assert((nViewTick >= 0) && (nViewTick < nTotViewTicks));
	const double fGameTickDeltaX = (m_oTargetPos.m_fX - m_oPos.m_fX) / m_nGameTicksToTarget;
	const double fGameTickDeltaY = (m_oTargetPos.m_fY - m_oPos.m_fY) / m_nGameTicksToTarget;
	const double fPosX = m_oPos.m_fX + (1.0 + nViewTick) * fGameTickDeltaX / nTotViewTicks;
	const double fPosY = m_oPos.m_fY + (1.0 + nViewTick) * fGameTickDeltaY / nTotViewTicks;
	return FPoint{fPosX, fPosY};
}

void PositionerEvent::blockChangedPlayer(LevelBlock& oLevelBlock, int32_t nOldPlayer) noexcept
{
//std::cout << "PositionerEvent::blockChangedPlayer()" << '\n';
	const int32_t nLBId = oLevelBlock.blockGetId();
	const int32_t nNewPlayer = oLevelBlock.getPlayer();
	assert(nNewPlayer != nOldPlayer);
	const bool bOldPlayerHadControl = (nOldPlayer >= 0);
	const bool bNewPlayerHasControl = (nNewPlayer >= 0);
	assert(bOldPlayerHadControl || (nOldPlayer == -1));
	assert(bNewPlayerHasControl || (nNewPlayer == -1));
	if (m_bSubshowMode) {
		if (bOldPlayerHadControl) {
			auto& aLBIds = m_aShowPositions[nOldPlayer].m_aLevelBlockIds;
			assert(! aLBIds.empty());
			assert(nLBId == aLBIds[0]);
			aLBIds.clear();
			if (! bNewPlayerHasControl) {
				// probably freezed
				const NPoint oLBPos = oLevelBlock.blockPos();
				const NPoint oLBBricksMinPos = oLevelBlock.blockBricksMinPos();
				const NSize oLBSize = oLevelBlock.blockSize();
				NRect& oLBRect = m_aShowPositions[nOldPlayer].m_oLastRect;
				oLBRect.m_nX = oLBPos.m_nX + oLBBricksMinPos.m_nX;
				oLBRect.m_nY = oLBPos.m_nY + oLBBricksMinPos.m_nY;
				oLBRect.m_nW = oLBSize.m_nW;
				oLBRect.m_nH = oLBSize.m_nH;
			}
		}
		if (bNewPlayerHasControl) {
			auto& aLBIds = m_aShowPositions[nNewPlayer].m_aLevelBlockIds;
			assert(aLBIds.empty()); // a player can control only one  level block at a time
			aLBIds.push_back(nLBId);
			auto& oShowPosition = m_aShowPositions[nNewPlayer];
			oShowPosition.m_oLastRect.m_nW = 0;
			oShowPosition.m_oLastRect.m_nH = 0;
		}
	} else {
		auto& aLBIds = m_aShowPositions[0].m_aLevelBlockIds;
		if (bNewPlayerHasControl && bOldPlayerHadControl) {
			// unchanged
		} else if (bOldPlayerHadControl) {
			// remove level block from tracked
			auto itFound = std::find(aLBIds.begin(), aLBIds.end(), nLBId);
			assert(itFound != aLBIds.end());
			aLBIds.erase(itFound);
		} else {
			assert(bNewPlayerHasControl);
			#ifndef NDEBUG
			auto itFound = std::find(aLBIds.begin(), aLBIds.end(), nLBId);
			assert(itFound == aLBIds.end());
			#endif //NDEBUG
			aLBIds.push_back(nLBId);
		}
	}
}

} // namespace stmg
