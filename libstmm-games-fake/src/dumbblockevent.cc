/*
 * File:   dumbblockevent.cc
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

#include "dumbblockevent.h"

#include <stmm-games/utile/tilerect.h>
#include <stmm-games/block.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/direction.h>
#include <stmm-games/event.h>
#include <stmm-games/gameproxy.h>
#include <stmm-games/level.h>
#include <stmm-games/levelblock.h>
#include <stmm-games/tile.h>

#include <cassert>
//#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace stmg { class Coords; }
namespace stmg { class TileBuffer; }
namespace stmg { class TileCoords; }


namespace stmg
{

const int32_t DumbBlockEvent::s_nZObjectZDumbBlockEvent = 10;

DumbBlockEvent::DumbBlockEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, LevelBlock(true)
, m_oData(std::move(oInit))
, m_nBoardWidth(level().boardWidth())
, m_nBoardHeight(level().boardHeight())
, m_eState(DUMB_BLK_EVENT_STATE_ACTIVATE)
{
	blockInitialSet(m_oData.m_oBlock, 0, m_oData.m_oInitPos, false, -1);
}

void DumbBlockEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	m_oData = std::move(oInit);
	m_nBoardWidth = level().boardWidth();
	m_nBoardHeight = level().boardHeight();
	m_eState = DUMB_BLK_EVENT_STATE_ACTIVATE;
	blockInitialSet(m_oData.m_oBlock, 0, m_oData.m_oInitPos, false, -1);
}
bool DumbBlockEvent::validBlock() const noexcept
{
	return !m_oData.m_oBlock.isEmpty();
}
void DumbBlockEvent::trigger(int32_t /*nMsg*/, int32_t /*nValue*/, Event* p0TriggeringEvent) noexcept
{
	// ACTIVATE activate event
	// INIT     calc initial position, reset attempts to place it
	// PLACE    try to place block on board, game over if fail
	//          if succeeded deactivate and add to level's falling blocks
	// FALL     expect and do nothing while falling
	//            (zombie state, cannot fall more than once without being reinitialized)
	Level& oLevel = level();
	const int32_t nGameTick = oLevel.game().gameElapsed();
//std::cout << "DumbBlockEvent(" << blockGetId() << ")::trigger() nGameTick=" << nGameTick << '\n';
	switch (m_eState)
	{
		case DUMB_BLK_EVENT_STATE_ACTIVATE:
		{
			if (!validBlock()) {
				m_eState = DUMB_BLK_EVENT_STATE_ZOMBIE;
				break;
			}
			m_eState = DUMB_BLK_EVENT_STATE_INIT;
			if (p0TriggeringEvent != nullptr) {
				oLevel.activateEvent(this, nGameTick);
				break;
			}
		} // fallthrough
		case DUMB_BLK_EVENT_STATE_INIT:
		{
			if (p0TriggeringEvent != nullptr) {
				return; //------------------------------------------------------
			}
			m_eState = DUMB_BLK_EVENT_STATE_PLACE;
			//calcInitialPos();
		} // fallthrough
		case DUMB_BLK_EVENT_STATE_PLACE:
		{
			if (p0TriggeringEvent != nullptr) {
				return; //------------------------------------------------------
			}
			if (!oLevel.blockAdd(this, LevelBlock::MGMT_TYPE_AUTO_STRICT_OWNER)) {
//std::cout << "DumbBlockEvent(" << blockGetId() << ")::trigger   blockAdd failed" << '\n';
				m_eState = DUMB_BLK_EVENT_STATE_ZOMBIE;
				informListeners(LISTENER_GROUP_CANNOT_PLACE, 0);
				informListeners(LISTENER_GROUP_FINISHED, 0);
			} else {
				m_eState = DUMB_BLK_EVENT_STATE_FALL;
				oLevel.boardAddListener(this);
				oLevel.deactivateEvent(this);
				informListeners(LISTENER_GROUP_COULD_PLACE, 0);
			}
		}
		break;
		case DUMB_BLK_EVENT_STATE_FALL:
		{
			if (p0TriggeringEvent != nullptr) {
				return; //------------------------------------------------------
			}
		}
		break;
		case DUMB_BLK_EVENT_STATE_ZOMBIE:
		{
		}
		break;
	}
}
void DumbBlockEvent::onAdded() noexcept
{
//std::cout << "DumbBlockEvent(" << blockGetId() << ")::onAdded" << '\n';
}
void DumbBlockEvent::handleTimer() noexcept
{
}
bool DumbBlockEvent::canMove(Direction::VALUE eDir) noexcept
{
	return move(eDir, false, 0,0,0,0);
}
bool DumbBlockEvent::canMove(Direction::VALUE eDir, int32_t nClipX, int32_t nClipY, int32_t nClipW, int32_t nClipH) noexcept
{
	return move(eDir, false, nClipX, nClipY, nClipW, nClipH);
}
bool DumbBlockEvent::move(Direction::VALUE eDir) noexcept
{
	return move(eDir, true, 0,0,0,0);
}
bool DumbBlockEvent::move(Direction::VALUE eDir, bool bDoMove
		, int32_t nClipX, int32_t nClipY, int32_t nClipW, int32_t nClipH) noexcept
{
	Level& oLevel = level();
	assert((static_cast<int32_t>(eDir) >= 0) && (static_cast<int32_t>(eDir) < 4));
	const NPoint oPos = LevelBlock::blockPos();
	const int32_t nPosX = oPos.m_nX;
	const int32_t nPosY = oPos.m_nY;
	const std::vector< Block::Contact >& aContacts = blockContacts(eDir);
	for (auto& oContactBrickPos : aContacts) {
		const int32_t nRelX = oContactBrickPos.m_nRelX;
		const int32_t nRelY = oContactBrickPos.m_nRelY;
		//const int32_t nContactBrickId = oContactBrickPos.m_nBrickId;
		const int32_t nBoardX = nPosX + nRelX;
		const int32_t nBoardY = nPosY + nRelY;
		if ((nClipW == 0) || (nClipH == 0)
				|| ((nBoardX >= nClipX) && (nBoardX < nClipX + nClipW)
					&& (nBoardY >= nClipY) && (nBoardY < nClipY + nClipH)))
		{
			if ((nBoardX < 0) || (nBoardX >= m_nBoardWidth)
					|| (nBoardY >= m_nBoardHeight) || (nBoardY < 0)) {
				// cannot move
				return false; //------------------------------------------------
			}
			const Tile& oTile = oLevel.boardGetTile(nBoardX, nBoardY);
			if (!oTile.isEmpty()) {
				// solid tile: cannot move
				return false; //------------------------------------------------
			} else {
				LevelBlock* p0LevelBlock = oLevel.boardGetOwner(nBoardX, nBoardY);
				if (p0LevelBlock != nullptr) {
					assert(p0LevelBlock != this);
					return false; //--------------------------------------------
				}
			}
		}
	}
	if (bDoMove) {
		const int32_t nDx = Direction::deltaX(eDir);
		const int32_t nDy = Direction::deltaY(eDir);
		blockMove(nDx, nDy);
	}
	return true;
}

void DumbBlockEvent::fall() noexcept
{
}

void DumbBlockEvent::privateOnFreeze() noexcept
{
	level().boardRemoveListener(this);
	//if (m_p0Parent != nullptr) {
	//	m_p0Parent->trigger(m_nTriggerFinishedMsg, m_nId, m_p0Parent);
	//}
}
bool DumbBlockEvent::remove() noexcept
{
	return freeze();
}
bool DumbBlockEvent::destroy() noexcept
{
	return freeze();
}
bool DumbBlockEvent::freeze() noexcept
{
	#ifndef NDEBUG
	const bool bFroze =
	#endif //NDEBUG
	LevelBlock::freeze();
	assert(bFroze);
	privateOnFreeze();
	return true;
}
bool DumbBlockEvent::fuseTo(LevelBlock& /*oLevelBlock*/) noexcept
{
	return false;
}
bool DumbBlockEvent::removeBrick(int32_t /*nBrickId*/) noexcept
{
	return false;
}
bool DumbBlockEvent::destroyBrick(int32_t /*nBrickId*/) noexcept
{
	return false;
}

LevelBlock::QUERY_ATTACK_TYPE DumbBlockEvent::queryAttack(LevelBlock& /*oAttacker*/, int32_t /*nBoardX*/, int32_t /*nBoardY*/, const Tile& /*oTile*/) const noexcept
{
	return LevelBlock::QUERY_ATTACK_TYPE_FREEZE_ATTACKED;
}
bool DumbBlockEvent::attack(LevelBlock& /*oAttacker*/, int32_t /*nBoardX*/, int32_t /*nBoardY*/, const Tile& /*oTile*/) noexcept
{
	#ifndef NDEBUG
	const bool bFroze =
	#endif //NDEBUG
	freeze();
	assert(bFroze);
	return true;
}

void DumbBlockEvent::boabloPreFreeze(LevelBlock& /*oBlock*/) noexcept
{
}
void DumbBlockEvent::boabloPostFreeze(const Coords& /*oCoords*/) noexcept
{
}
void DumbBlockEvent::boabloPreUnfreeze(const Coords& /*oCoords*/) noexcept
{
}
void DumbBlockEvent::boabloPostUnfreeze(LevelBlock& /*oBlock*/) noexcept
{
}
void DumbBlockEvent::boardPreScroll(Direction::VALUE eDir, const shared_ptr<TileRect>& /*refTiles*/) noexcept
{
	assert(blockIsAutoOwner());
	const int32_t nDx = Direction::deltaX(eDir);
	const int32_t nDy = Direction::deltaY(eDir);
	if (!level().blockMoveIsWithinArea(*this, nDx, nDy, 0, 0, m_nBoardWidth, m_nBoardHeight)) {
		// block is scrolled outside board, freeze first
		#ifndef NDEBUG
		const bool bFroze =
		#endif //NDEBUG
		freeze();
		assert(bFroze);
	}
}
void DumbBlockEvent::boardPostScroll(Direction::VALUE /*eDir*/) noexcept
{
}
void DumbBlockEvent::boardPreInsert(Direction::VALUE eDir, NRect oRect, const shared_ptr<TileRect>& refTiles) noexcept
{
	if (eDir == Direction::DOWN) {
		if ((refTiles) && !TileRect::isAllEmptyTiles(*refTiles)) {
			level().gameStatusTechnical(std::vector<std::string>{"DumbBlockEvent::boardPreInsert()","DOWN only supports empty tiles insertion"});
			return;
		}
	}
	if (!((eDir == Direction::UP) || (eDir == Direction::DOWN))) {
		level().gameStatusTechnical(std::vector<std::string>{"DumbBlockEvent::boardPreInsert()","Only DOWN and UP supported"});
		return;
	}
	const auto& nX = oRect.m_nX;
	const auto& nY = oRect.m_nY;
	const auto& nW = oRect.m_nW;
	const auto& nH = oRect.m_nH;
	if (nY != 0) {
		level().gameStatusTechnical(std::vector<std::string>{"DumbBlockEvent::boardPreInsert()","Only nY=0 supported"});
		return;
	}
	if (eDir == Direction::DOWN) {
		boardPreDeleteDown(nY + nH - 1, nX, nW);
	} else{
		boardPreInsertUp(nY + nH - 1, nX, nW, shared_ptr<TileBuffer>());
	}
}
void DumbBlockEvent::boardPostInsert(Direction::VALUE /*eDir*/
									, NRect /*oRect*/) noexcept
{
}
void DumbBlockEvent::boardPreDeleteDown(int32_t nDelY, int32_t nX, int32_t nW) noexcept
{
//std::cout << "DumbBlockEvent(" << getId() << ")::boardPreDeleteDown" << '\n';
	if ((nDelY > 0) && !canMove(Direction::UP, nX, 0, nW, nDelY)) {
		// it is pushed down by some board cell
		#ifndef NDEBUG
		const bool bFroze =
		#endif //NDEBUG
		freeze();
		assert(bFroze);
	}
}
void DumbBlockEvent::boardPreInsertUp(int32_t nY, int32_t nX, int32_t nW, const shared_ptr<TileBuffer>& /*refBuffer*/) noexcept
{
//std::cout << "DumbBlockEvent(" << getId() << ")::boardPreInsertUp" << '\n';
	if ((nY > 0) && !canMove(Direction::DOWN, nX, 1, nW, nY)) {
		// pushed up by some board cell
		#ifndef NDEBUG
		const bool bFroze =
		#endif //NDEBUG
		freeze();
		assert(bFroze);
		return;
	}
	if (level().blockIntersectsArea(*this, nX, nY, nW, 1)) {
		#ifndef NDEBUG
		const bool bFroze =
		#endif //NDEBUG
		freeze();
		assert(bFroze);
		return;
	}
}
void DumbBlockEvent::boardPreDestroy(const Coords& /*oCoords*/) noexcept
{
}
void DumbBlockEvent::boardPostDestroy(const Coords& /*oCoords*/) noexcept
{
}
void DumbBlockEvent::boardPreModify(const TileCoords& /*oTileCoords*/) noexcept
{
}
void DumbBlockEvent::boardPostModify(const Coords& /*oCoords*/) noexcept
{
}

} // namespace stmg
