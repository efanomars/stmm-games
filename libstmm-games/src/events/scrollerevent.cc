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
 * File:   scrollerevent.cc
 */

#include "events/scrollerevent.h"

#include "gameproxy.h"
#include "level.h"
#include "levelshow.h"
#include "named.h"
#include "tile.h"
#include "util/basictypes.h"
#include "util/direction.h"
#include "util/namedindex.h"
#include "utile/querytileremoval.h"
#include "utile/tilebuffer.h"

#include <cassert>
#include <iostream>
#include <algorithm>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace stmg { class TileAnimator; }

namespace stmg
{

static std::string s_sTileAniRemoving = "TILEANI:REMOVING";

ScrollerEvent::ScrollerEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
{
	commonInit(std::move(oInit));
}

void ScrollerEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	commonInit(std::move(oInit));
}
void ScrollerEvent::commonInit(LocalInit&& oInit) noexcept
{
	Level& oLevel = level();
	m_nBoardW = oLevel.boardWidth();
	m_nBoardH = oLevel.boardHeight();

	m_nRepeat = oInit.m_nRepeat;
	m_nStep = oInit.m_nStep;
	m_nSlices = oInit.m_nSlices;
	m_bKeepTopVisible = oInit.m_bKeepTopVisible;
	m_nTopNotEmptyWaitTicks = oInit.m_nTopNotEmptyWaitTicks;
	m_nTopNotEmptyWaitMillisec = oInit.m_nTopNotEmptyWaitMillisec;
	m_refNewRows = std::move(oInit.m_refNewRows);
	m_p0TileRemover = oInit.m_p0TileRemover;

	m_oTileBufferRecycler.create(m_refCurTileBuf, NSize{m_nBoardW, 1});
	m_nCheckNewRowTries = oInit.m_nCheckNewRowTries;

	// checks
	assert((m_nRepeat > 0) || (m_nRepeat == -1));
	assert(m_nStep > 0);

	assert(m_nSlices > 0);
	assert(!(m_bKeepTopVisible && ((m_nTopNotEmptyWaitTicks < 0) || (m_nTopNotEmptyWaitMillisec < 0))));
	assert(m_nCheckNewRowTries > 0);
	assert(m_refNewRows.get() != nullptr);
	assert(m_refNewRows->getTotNewRowGens() > 0);
	//
	m_eState = SCROLLER_STATE_ACTIVATE;
	m_nTileAniRemovingIndex = oLevel.getNamed().tileAnis().getIndex(s_sTileAniRemoving);
	if (m_nTileAniRemovingIndex < 0) {
		#ifndef NDEBUG
		std::cout << "Warning! ScrollerEvent: tile animation '" << s_sTileAniRemoving << "' not defined!" << '\n';
		#endif //NDEBUG
		m_nTileAniRemovingIndex = oLevel.getNamed().tileAnis().addName(s_sTileAniRemoving);
	}

	resetRuntime();
}
void ScrollerEvent::resetRuntime() noexcept
{
	m_nCurRandomGen = 0;
	m_nCurSlices = m_nSlices;
	m_nNextSlices = m_nSlices;
	m_nCounter = 0;
	m_nSliceCounter = 0;
	m_nWaitTicks = 0;
	m_nToPushUp = 0;
	m_nLastPushUpTime = -1;
	m_bWaitingBecauseNotEmptyTop = false;
	m_oCoords.reInit();
}

void ScrollerEvent::trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept
{
//std::cout << "ScrollerEvent::trigger" << '\n';
	//TODO
	// ACTIVATE activate event
	// INIT init
	// RUN  scroll up if possible
	// last RUN deactivate, state to INIT
	//          send messages to finished-group listeners
	Level& oLevel = level();
	const int32_t nTimer = oLevel.game().gameElapsed();
	//TODO every time a push row is executed also a scroll is (which is not good)
	switch (m_eState)
	{
		case SCROLLER_STATE_ACTIVATE:
		{
			m_eState = SCROLLER_STATE_INIT;
			if (p0TriggeringEvent != nullptr) {
				// never scroll when triggered by another event
				oLevel.activateEvent(this, nTimer);
				break;
			}
		} //fallthrough
		case SCROLLER_STATE_INIT:
		{
		} //fallthrough
		case SCROLLER_STATE_RUN:
		{
			if (p0TriggeringEvent != nullptr) {
				switch (nMsg)
				{
					case MESSAGE_PAUSE_TICKS:
					{
						m_nWaitTicks += std::max<int32_t>(nValue, 0);
//std::cout << "ScrollerEvent::trigger MESSAGE_PAUSE_TICKS nValue=" << nValue << "  m_nWaitTicks=" << m_nWaitTicks << '\n';
						if (m_bWaitingBecauseNotEmptyTop) {
							informListeners(LISTENER_GROUP_TOP_NOT_EMPTY_TICK, m_nWaitTicks);
						}
					}
					break;
					case MESSAGE_PAUSE_MILLISEC:
					{
						if (nValue > 0) {
							nValue = nValue / oLevel.game().gameInterval();
							m_nWaitTicks += std::max<int32_t>(nValue, 1);
							if (m_bWaitingBecauseNotEmptyTop) {
								informListeners(LISTENER_GROUP_TOP_NOT_EMPTY_TICK, m_nWaitTicks);
							}
						}
//std::cout << "ScrollerEvent::trigger MESSAGE_PAUSE_MILLISEC nValue=" << nValue << "  m_nWaitTicks=" << m_nWaitTicks << '\n';
					}
					break;
					case MESSAGE_PUSH_ROW:
					{
						m_nToPushUp += std::max(1, nValue);
						if (! m_bWaitingBecauseNotEmptyTop) {
							m_nWaitTicks = 1;
						}
					}
					break;
					case MESSAGE_SET_SLICES:
					{
						m_nNextSlices = std::max(1, nValue);
					}
					break;
					case MESSAGE_SET_NEW_ROW_GEN:
					{
						m_nCurRandomGen = std::min(std::max(0, nValue), m_refNewRows->getTotNewRowGens() - 1);
					}
					break;
					case MESSAGE_NEXT_NEW_ROW_GEN:
					{
						if (m_nCurRandomGen < m_refNewRows->getTotNewRowGens() - 1) {
							++m_nCurRandomGen;
						}
					}
					break;
					case MESSAGE_PREV_NEW_ROW_GEN:
					{
						if (m_nCurRandomGen > 0) {
							--m_nCurRandomGen;
						}
					}
					break;
				}
				oLevel.activateEvent(this, nTimer);
				break;
			}
			const bool bTerminate = (m_nRepeat != -1) && (m_nCounter >= m_nRepeat);
			if (bTerminate) {
//std::cout << "ScrollerEvent::trigger FINISHED" << '\n';
				m_eState = SCROLLER_STATE_INIT;
				resetRuntime();
				informListeners(LISTENER_GROUP_FINISHED, 0);
				break;
			}
			if (m_nSliceCounter == 0) {
				if (checkTopLineNotBusy()) {
					if (m_bKeepTopVisible && !checkLineEmpty(0)) {
						// top row not empty
						if (m_nWaitTicks <= 0) {
							if (m_bWaitingBecauseNotEmptyTop || ((m_nTopNotEmptyWaitTicks <= 0) && (m_nTopNotEmptyWaitMillisec <= 0))) {
								if (m_bWaitingBecauseNotEmptyTop) {
//std::cout << "ScrollerEvent::trigger Send LISTENER_GROUP_TOP_NOT_EMPTY_END" << '\n';
									m_bWaitingBecauseNotEmptyTop = false;
									informListeners(LISTENER_GROUP_TOP_NOT_EMPTY_END, 0);
								}
//std::cout << "ScrollerEvent::trigger Destroy Top Line" << '\n';
								informNotEmptyTopLineColumns();
								m_oCoords.reInit();
								m_oCoords.addRect(0, 0, m_nBoardW, 1);
								oLevel.boardDestroy(m_oCoords);
								assert(checkLineEmpty(0));
								informListeners(LISTENER_GROUP_TOP_DESTROYED, 0);
							} else {
								m_bWaitingBecauseNotEmptyTop = true;
								const int32_t nTopNotEmptyWait = m_nTopNotEmptyWaitTicks + m_nTopNotEmptyWaitMillisec / oLevel.game().gameInterval();
								m_nWaitTicks += nTopNotEmptyWait;
								informListeners(LISTENER_GROUP_TOP_NOT_EMPTY_START, m_nWaitTicks);
							}
						}
					} else {
						if (m_bWaitingBecauseNotEmptyTop) {
//std::cout << "ScrollerEvent::trigger Send LISTENER_GROUP_TOP_NOT_EMPTY_END" << '\n';
							m_bWaitingBecauseNotEmptyTop = false;
							informListeners(LISTENER_GROUP_TOP_NOT_EMPTY_END, 0);
						}
					}
				} else {
					// retry later
//std::cout << "ScrollerEvent::trigger RETRY LATER" << '\n';
					oLevel.activateEvent(this, nTimer + 1);
					break;
				}
			}
			const bool bCanPushRow = (m_nLastPushUpTime < nTimer);
			if (bCanPushRow && (m_nToPushUp > 0)) {
				if (checkTopLineNotBusy()) {
//std::cout << "             ::trigger could push row" << '\n';
					// decrement rows to push even though the actual pushing can't take place!
					--m_nToPushUp;
					if ((!m_bKeepTopVisible)  ||  ((m_nSliceCounter == 0) && checkLineEmpty(0))  ||  (checkLineEmpty(1))) {
//std::cout << "             ::trigger    push row" << '\n';
						++m_nCounter;
						pushRow();
						m_nLastPushUpTime = nTimer;
						oLevel.activateEvent(this, nTimer + 1);
						break;
					} // else { top entirely visible line is not empty }
				}
			}
			if (m_nWaitTicks > 0) {
				--m_nWaitTicks;
				if (m_bWaitingBecauseNotEmptyTop) {
					informListeners(LISTENER_GROUP_TOP_NOT_EMPTY_TICK, m_nWaitTicks);
				}
				oLevel.activateEvent(this, nTimer + 1);
				break;
			}
			if (! bCanPushRow) {
				oLevel.activateEvent(this, nTimer + 1);
				break;
			}
			if (m_bWaitingBecauseNotEmptyTop) {
				m_bWaitingBecauseNotEmptyTop = false;
//std::cout << "ScrollerEvent::trigger Send LISTENER_GROUP_TOP_NOT_EMPTY_END 2" << '\n';
				informListeners(LISTENER_GROUP_TOP_NOT_EMPTY_END, 0);
			}
			//
			++m_nSliceCounter;
			if (m_nSliceCounter >= m_nCurSlices) {
				m_nSliceCounter = 0;
				m_nCurSlices = m_nNextSlices;
			}
//std::cout << "ScrollerEvent::trigger m_nSliceCounter=" << m_nSliceCounter << " m_nCurSlices=" << m_nCurSlices << '\n';
			LevelShow& oLevelShow = oLevel.showGet();
			FPoint oShowPos = oLevelShow.getPos();
			oShowPos.m_fY = 1.0 * m_nSliceCounter / m_nCurSlices + static_cast<int32_t>(oShowPos.m_fY);
			oLevelShow.setPos(oShowPos);
			if (m_nSliceCounter == 0) {
				++m_nCounter;
				// add
				pushRow();
				m_nLastPushUpTime = nTimer;
			}
			if (m_nToPushUp > 0) {
				oLevel.activateEvent(this, nTimer + 1);
			} else {
				oLevel.activateEvent(this, nTimer + m_nStep);
			}
		}
		break;
	}
}

bool ScrollerEvent::checkTopLineNotBusy() noexcept
{
	Level& oLevel = level();
	for (int32_t nCurX = 0; nCurX < m_nBoardW; nCurX++) {
		if (oLevel.boardGetTileAnimator(nCurX, 0, m_nTileAniRemovingIndex) != nullptr) {
			return false;
		}
	}
	return true;
}
bool ScrollerEvent::checkLineEmpty(int32_t nY) noexcept
{
	Level& oLevel = level();
	for (int32_t nCurX = 0; nCurX < m_nBoardW; nCurX++) {
		if (!oLevel.boardGetTile(nCurX, nY).isEmpty()) {
			return false;
		}
	}
	return true;
}
void ScrollerEvent::informNotEmptyTopLineColumns() noexcept
{
	Level& oLevel = level();
	for (int32_t nCurX = 0; nCurX < m_nBoardW; nCurX++) {
		if (!oLevel.boardGetTile(nCurX, 0).isEmpty()) {
			informListeners(LISTENER_GROUP_TOP_COLUMN, nCurX);
		}
	}
}

void ScrollerEvent::pushRow() noexcept
{
//std::cout << "ScrollerEvent::pushRow" << '\n';
	Level& oLevel = level();

	int32_t nBestToRemove = std::numeric_limits<int32_t>::max();

	NRect oExtRect;
	oExtRect.m_nX = 0;
	oExtRect.m_nY = m_nBoardH;
	oExtRect.m_nW = m_nBoardW;
	oExtRect.m_nH = 1;
	// Some listener might have kept a reference to the current m_refBestTileBuffer
	// during last boardScroll call
	m_oTileBufferRecycler.create(m_refBestTileBuffer, NSize{m_nBoardW, 1});
	//
	const bool bHasTileRemover = (m_p0TileRemover != nullptr);
	const int32_t nTotTries = m_nCheckNewRowTries;
	for (int32_t nTry = 0; nTry < nTotTries; ++nTry) {
//std::cout << "ScrollerEvent::pushRow nTry=" << nTry << '\n';
		m_refNewRows->createNewRow(m_nCurRandomGen, *m_refCurTileBuf, 0);
		if (! bHasTileRemover) {
			m_refBestTileBuffer.swap(m_refCurTileBuf);
			break; // for nTry ---
		}
		const int32_t nToRemove = m_p0TileRemover->wouldRemoveTiles(*this, oExtRect);
		if (nToRemove < nBestToRemove) {
			m_refBestTileBuffer.swap(m_refCurTileBuf);
			if (nToRemove == 0) {
				// can't get better than this
				break; // for nTry ---
			}
			nBestToRemove = nToRemove;
		}
	}
//std::cout << "ScrollerEvent::pushRow 2" << '\n';
	oLevel.boardScroll(Direction::UP, m_refBestTileBuffer);
	informListeners(LISTENER_GROUP_PUSHED, m_nCurRandomGen);
}

const Tile& ScrollerEvent::boardGetTile(int32_t nX, int32_t nY) const noexcept
{
	assert((nX >= 0) && (nX < m_nBoardW));
	assert((nY >= 0) && (nY < m_nBoardH + 1));
	if (nY >= m_nBoardH) {
		return m_refCurTileBuf->get({nX, 0});
	}
	return level().boardGetTile(nX, nY);
}
const TileAnimator* ScrollerEvent::boardGetTileAnimator(int32_t nX, int32_t nY, int32_t nIdxTileAni) const noexcept
{
	assert((nX >= 0) && (nX < m_nBoardW));
	assert((nY >= 0) && (nY < m_nBoardH + 1));
	if (nY >= m_nBoardH) {
		return nullptr;
	}
	return level().boardGetTileAnimator(nX, nY, nIdxTileAni);

}

} // namespace stmg
