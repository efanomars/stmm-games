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
 * File:   tileanimatorevent.h
 */

#ifndef STMG_TILE_ANIMATOR_EVENT_H
#define STMG_TILE_ANIMATOR_EVENT_H

#include "event.h"

#include "tileanimator.h"
#include "levellisteners.h"

#include "utile/tileselector.h"

#include "util/recycler.h"
#include "util/basictypes.h"
#include "util/direction.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <cassert>

#include <stdint.h>

namespace stmg { class Coords; }
namespace stmg { class LevelBlock; }
namespace stmg { class TileCoords; }
namespace stmg { class TileRect; }

namespace stmg
{

using std::unique_ptr;

class TileAnimatorEvent : public Event, public BoardListener, public BlocksBricksIdListener
{
public:
	struct LocalInit
	{
		int32_t m_nRepeat = -1; /**< Ticks the event should be active for. -1: forever. Default is -1. */
		unique_ptr<TileSelector> m_refSelect; /**< The tiles that should be animated. Empty tiles are never selected. If null means every non empty tile. */
		int32_t m_nAniNameIdx = -1; /**< The tile animation id. Must be valid in Named::tileAnis(). */
		bool m_bDoBoard = true; /**< Whether to animate board tiles. */
		NRect m_oArea; /**< The area within the board to be animated (in tiles). Must be within board. */
		bool m_bDoBlocks = true; /**< Whether to animate block tiles. Only currently visible tiles are considered. */
		NTimeRange m_oInitialWait; /**< The wait in game ticks and milliseconds before starting to animate a cell. All m_nFrom must be &gt;= 0 and &lt;= m_nTo. */
		NTimeRange m_oDuration; /**< The duration in game ticks and milliseconds of the cell animation. All m_nFrom must be &gt;= 0 and &lt;= m_nTo. */
		NTimeRange m_oPause; /**< The pause in game ticks and milliseconds between cell animations. All m_nFrom must be &gt;= 0 and &lt;= m_nTo. */
		NRange m_oTotCount = {1,1}; /**< The number of times the cell animation should be repeated. Default is 1. */
		int32_t m_nMaxParallel = -1; /**< How many single TileAnimation should be active at the same moment. -1: all (if possible). Default is -1. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * All the ranges must have m_nFrom &lt;= m_nTo.
	 * The values are chosen randomly from the ranges. The duration is always at least one tick.
	 * @param oInit The initialization data.
	 */
	explicit TileAnimatorEvent(Init&& oInit) noexcept;
protected:
	/** Reinitialization.
	 * See constructor.
	 */
	void reInit(Init&& oInit) noexcept;
public:
	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	void boabloPreFreeze(LevelBlock& oBlock) noexcept override;
	void boabloPostFreeze(const Coords& oCoords) noexcept override;
	void boabloPreUnfreeze(const Coords& oCoords) noexcept override;
	void boabloPostUnfreeze(LevelBlock& oBlock) noexcept override;

	void boardPreScroll(Direction::VALUE eDir, const shared_ptr<TileRect>& refTiles) noexcept override;
	void boardPostScroll(Direction::VALUE eDir) noexcept override;

	void boardPreInsert(Direction::VALUE eDir, NRect oArea, const shared_ptr<TileRect>& refTiles) noexcept override;
	void boardPostInsert(Direction::VALUE eDir, NRect oArea) noexcept override;
	void boardPreDestroy(const Coords& oCoords) noexcept override;
	void boardPostDestroy(const Coords& oCoords) noexcept override;
	void boardPreModify(const TileCoords& oTileCoords) noexcept override;
	void boardPostModify(const Coords& oCoords) noexcept override;

	void blockPreAdd(const LevelBlock& oBlock) noexcept override;
	void blockPostAdd(LevelBlock& oBlock) noexcept override;
	void blockPreRemove(LevelBlock& oBlock) noexcept override;
	void blockPostRemove(const LevelBlock& oBlock) noexcept override;
	void blockPreDestroy(LevelBlock& oBlock) noexcept override;
	void blockPostDestroy(const LevelBlock& oBlock) noexcept override;
	void blockPreFuse(LevelBlock& oFusedToBlock, LevelBlock& oFusedBlock) noexcept override;
	void blockPostFuse(LevelBlock& oResBlock, const LevelBlock& oFusedBlock
						, const std::unordered_map<int32_t, int32_t>& oFusedToBrickIds
						, const std::unordered_map<int32_t, int32_t>& oFusedBrickIds) noexcept override;
	void blockPreModify(LevelBlock& oBlock
						, const std::vector<int32_t>& aDeleteBrickId
						, const std::vector<int32_t>& aModifyPosBrickId
						, const std::vector<int32_t>& aModifyTileBrickId
						, bool bAddsBricks) noexcept override;
	void blockPostModify(LevelBlock& oBlock
						, const std::vector<int32_t>& aDeletedBrickId
						, const std::vector<int32_t>& aModifiedPosBrickId
						, const std::vector<int32_t>& aModifiedTileBrickId
						, const std::vector<int32_t>& aAddedBrickId) noexcept override;

	// Outputs
	enum {
		LISTENER_GROUP_TILEANI_STARTED = 10
	};

private:
	void commonInit() noexcept;
	void deInit() noexcept;

	void checkNewTileAnis(int32_t nGameTick, double fGameInterval) noexcept;
	void animate(int32_t nGameTick, double fGameInterval) noexcept;

	struct TileAni final : public TileAnimator
	{
		TileAni() = default;
		void reInit() noexcept {} // this is for the recycler
		friend class TileAnimatorEvent;
		double getCommonElapsed(int32_t nAni, int32_t nViewTick, int32_t nTotViewTicks) const noexcept;
		double getElapsed01(int32_t nHash, int32_t nX, int32_t nY, int32_t nAni, int32_t nViewTick, int32_t nTotTicks) const noexcept override;
		double getElapsed01(int32_t nHash, const LevelBlock& oLevelBlock, int32_t nBrickIdx, int32_t nAni, int32_t nViewTick, int32_t nTotTicks) const noexcept override;
		bool m_bWaiting;
		int32_t m_nCountdown; // animation ticks down counter
		int32_t m_nTotTicks; // remember the number of ticks (for elapsed calc))
		int32_t m_nCount; // number of animations completed so far
		int32_t m_nTotCount; // Tot number of animations to be completed
		int32_t m_nAniNameIdx; // this copied here because otherwise we would need a pointer to TileAnimatorEvent
		LevelBlock* m_p0LevelBlock; // Board if m_p0LevelBlock == null
	};

	shared_ptr<TileAnimatorEvent::TileAni> createTileAni(LevelBlock* p0LevelBlock) noexcept;
	bool createTileAniBoard(int32_t nNotAniIdx, int32_t nGameTick, double fGameInterval, int32_t nX, int32_t nY) noexcept;
	bool createTileAniBlock(int32_t nNotAniIdx, int32_t nGameTick, double fGameInterval, LevelBlock* p0LevelBlock, int32_t nBrick) noexcept;

	void blockAddSelectedBrick(LevelBlock& oLevelBlock, int32_t nBrickId, int32_t nBlockId) noexcept;
	void blockAddSelected(LevelBlock& oBlock) noexcept;
	// aDeleteBrickId is empty, remove all
	void blockRemoveSelected(LevelBlock& oLevelBlock, const std::vector<int32_t>& aRemoveBrickId) noexcept;

	void boardAddSelected(const NRect& oRect) noexcept;
	void boardAddSelected(const Coords& oCoords) noexcept;
	void boardRemoveSelected(const NRect& oRect) noexcept;
	void boardRemoveSelected(const Coords& oCoords) noexcept;
	void boardMoveSelected(const NRect& oRect, Direction::VALUE eDir) noexcept;

private:
	LocalInit m_oInit;

	int32_t m_nBoardWidth;
	int32_t m_nBoardHeight;

	enum TILE_ANIMATOR_STATE
	{
		  TILE_ANIMATOR_STATE_ACTIVATE = 0
		, TILE_ANIMATOR_STATE_INIT = 1
		, TILE_ANIMATOR_STATE_RUN = 2
		//, TILE_ANIMATOR_STATE_DEAD = 3
	};
	TILE_ANIMATOR_STATE m_eState;

	int32_t m_nCounter;

	// a board position in m_aNotAnisPos has no cell`s tile animator set
	// m_aWaitingAnisPos (despite not animating yet, sort of reserved) and m_aRunningAnisPos do

	std::vector< NPoint > m_aNotAnisPos; // if m_nX is negated it's the brick id and m_nY the block id, if positive position in board

	std::vector< NPoint > m_aWaitingAnisPos; // if m_nX is negated it's the brick id and m_nY the block id, if positive position in board
	std::vector< int32_t > m_aWaitingAnisStart; // Value: start game tick, Size: m_aWaitingAnisPos.size()
	std::vector< shared_ptr<TileAni> > m_aWaitingAnis; // Size: m_aWaitingAnisPos.size()

	std::vector< NPoint > m_aRunningAnisPos; // if m_nX is negated it's the brick id and m_nY the block id, if positive position in board
	std::vector< shared_ptr<TileAni> > m_aRunningAnis; // Size: m_aPosRunningAnis.size()

	//int32_t m_nAnis; // == m_oRunningAnis.size() + m_oWaitingAnis.size()

	Recycler<TileAni> m_oTileAniRecycler;

	// utility var to informListeners
	int32_t m_nStartedRunning;
	static const int32_t s_nMaxTry;
private:
	TileAnimatorEvent() = delete;
	TileAnimatorEvent(const TileAnimatorEvent& oSource) = delete;
	TileAnimatorEvent& operator=(const TileAnimatorEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_TILE_ANIMATOR_EVENT_H */

