/*
 * File:   dumbblockevent.h
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
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef _ZIMTRIS_DUMB_BLOCK_EVENT_H_
#define _ZIMTRIS_DUMB_BLOCK_EVENT_H_

#ifdef STMG_TESTING_COPY_FROM_FAKE
#include "levelblock.h"
#include "event.h"
#include "block.h"
#include "levellisteners.h"
#include "util/basictypes.h"
#include "util/direction.h"
#else
#include <stmm-games/levelblock.h>
#include <stmm-games/event.h>
#include <stmm-games/block.h>
#include <stmm-games/levellisteners.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/direction.h>
#endif

#include <memory>

namespace stmg { class Coords; }
namespace stmg { class Tile; }
namespace stmg { class TileBuffer; }
namespace stmg { class TileCoords; }
namespace stmg { class TileRect; }
namespace stmi { class Event; }

namespace stmg
{

using std::unique_ptr;
using std::shared_ptr;

/** Block that does nothing.
 * Does not fall. But you can freeze it.
 */
class DumbBlockEvent : public Event, public LevelBlock, public BoardListener
{
public:
	struct LocalInit
	{
		Block m_oBlock;
		NPoint m_oInitPos;
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * @param oInit The initialization data.
	 */
	DumbBlockEvent(Init&& oInit) noexcept;

protected:
	/** Reinitialization.
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept;

	void onAdded() noexcept override;
public:

	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	int32_t blockPosZ() const noexcept override { return s_nZObjectZDumbBlockEvent; }

	//	void handleKeyActionInput(const shared_ptr<stmi::Event>& refKeyActionEvent) {}
	//	void handleXYInput(const shared_ptr<stmi::Event>& refXYEvent) {}
	void handleInput(const shared_ptr<stmi::Event>& /*refEvent*/) noexcept override {}
	void handleTimer() noexcept override;
	void fall() noexcept override;

	bool remove() noexcept override;
	bool destroy() noexcept override;
	bool freeze() noexcept override;
	bool fuseTo(LevelBlock& oLevelBlock) noexcept override;
	bool removeBrick(int32_t nBrickId) noexcept override;
	bool destroyBrick(int32_t nBrickId) noexcept override;

	LevelBlock::QUERY_ATTACK_TYPE queryAttack(LevelBlock& oAttacker, int32_t nBoardX, int32_t nBoardY, const Tile& oTile) const noexcept override;
	bool attack(LevelBlock& oAttacker, int32_t nBoardX, int32_t nBoardY, const Tile& oTile) noexcept override;
	// BoardListener
	void boabloPreFreeze(LevelBlock& oBlock) noexcept override;
	void boabloPostFreeze(const Coords& oCoords) noexcept override;
	void boabloPreUnfreeze(const Coords& oCoords) noexcept override;
	void boabloPostUnfreeze(LevelBlock& oBlock) noexcept override;
	void boardPreScroll(Direction::VALUE eDir, const shared_ptr<TileRect>& refTiles) noexcept override;
	void boardPostScroll(Direction::VALUE eDir) noexcept override;
	void boardPreInsert(Direction::VALUE eDir, NRect oRect, const shared_ptr<TileRect>& refTiles) noexcept override;
	void boardPostInsert(Direction::VALUE eDir, NRect oRect) noexcept override;
	void boardPreDestroy(const Coords& oCoords) noexcept override;
	void boardPostDestroy(const Coords& oCoords) noexcept override;
	void boardPreModify(const TileCoords& oTileCoords) noexcept override;
	void boardPostModify(const Coords& oCoords) noexcept override;

	enum { 
		LISTENER_GROUP_CANNOT_PLACE = 10
		, LISTENER_GROUP_COULD_PLACE = 11
		//, LISTENER_GROUP_FUSED_WITH = 20
		, LISTENER_GROUP_REMOVED = 90
		, LISTENER_GROUP_DESTROYED = 91
		, LISTENER_GROUP_FREEZED = 92
		//, LISTENER_GROUP_FUSED_TO = 93
	};

private:
	enum DUMB_BLK_EVENT_STATE
	{
		DUMB_BLK_EVENT_STATE_ACTIVATE = 0,
		DUMB_BLK_EVENT_STATE_INIT = 1,
		DUMB_BLK_EVENT_STATE_PLACE = 2,
		DUMB_BLK_EVENT_STATE_FALL = 3,
		DUMB_BLK_EVENT_STATE_ZOMBIE = 4
	};

	bool validBlock() const noexcept;

	void privateOnFreeze() noexcept;
	void boardPreDeleteDown(int32_t nY, int32_t nX, int32_t nW) noexcept;
	void boardPreInsertUp(int32_t nY, int32_t nX, int32_t nW, const shared_ptr<TileBuffer>& refBuffer) noexcept;
	bool canMove(Direction::VALUE eDir, int32_t nClipX, int32_t nClipY, int32_t nClipW, int32_t nClipH) noexcept;
	bool canMove(Direction::VALUE eDir) noexcept;
	bool move(Direction::VALUE eDir) noexcept;
	bool move(Direction::VALUE eDir, bool bDoMove
					, int32_t nClipX, int32_t nClipY, int32_t nClipW, int32_t nClipH) noexcept;

private:
	LocalInit m_oData;

	int32_t m_nBoardWidth;
	int32_t m_nBoardHeight;

	DUMB_BLK_EVENT_STATE m_eState;

	static const int32_t s_nZObjectZDumbBlockEvent;
private:
	DumbBlockEvent() = delete;
	DumbBlockEvent(const DumbBlockEvent& oSource) = delete;
	DumbBlockEvent& operator=(const DumbBlockEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* _ZIMTRIS_DUMB_BLOCK_EVENT_H_ */

