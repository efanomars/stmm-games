/*
 * File:   tileanimatorevent.h
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

#ifndef STMG_TILE_ANIMATOR_EVENT_H
#define STMG_TILE_ANIMATOR_EVENT_H

#include "event.h"

#include "tileanimator.h"
#include "utile/tileselector.h"
#include "util/recycler.h"
#include "levellisteners.h"
#include "util/basictypes.h"
#include "util/direction.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <list>
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
		int32_t m_nRepeat = -1; /**< Ticks the event should be active for. -1: forever */
		unique_ptr<TileSelector> m_refSelect; /**< The tiles that should be animated. If null means every tile. */
		int32_t m_nAniNameIdx = -1; /**< The tile animation id. Must be valid in Named::tileAnis(). */
		bool m_bDoBoard = true; /**< Whether to animate board tiles. */
		NRect m_oArea; /**< The area within the board to be animated (in tiles). */
		bool m_bDoBlocks = true; /**< Whether to animate block tiles. */
		NTimeRange m_oInitialWait; /**< The wait in game ticks and milliseconds before starting to animate a cell. All m_nFrom must be &gt;= 0 and &lt;= m_nTo. */
		NTimeRange m_oDuration; /**< The duration in game ticks and milliseconds of the cell animation. All m_nFrom must be &gt;= 0 and &lt;= m_nTo. */
		NTimeRange m_oPause; /**< The pause in game ticks and milliseconds between cell animations. All m_nFrom must be &gt;= 0 and &lt;= m_nTo. */
		NRange m_oTotCount = {1,1}; /**< The number of times the cell animation should be repeated. Default is 1. */
		int32_t m_nMaxParallel = -1; /**< How many single TileAnimation should be active at the same moment. -1: all (if possible). */
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
	void boardPostDeleteDown(int32_t nDelY, int32_t nX, int32_t nW) noexcept;
	void boardPostInsertUp(int32_t nInsY, int32_t nX, int32_t nW) noexcept;
	void blockPreDeleteCommon(LevelBlock& oLevelBlock) noexcept;
	void blockPostDeleteCommon(const LevelBlock& oLevelBlock) noexcept;
	void blockPostInsertCommon(LevelBlock& oLevelBlock) noexcept;
	void boardCoordsRemoveSelected(const Coords& oCoords) noexcept;

	inline int32_t boardIndex(int32_t nPosX, int32_t nPosY) const noexcept { return nPosX + nPosY * m_nBoardWidth; }

	void checkNewTileAnis(int32_t nGameTick, double fGameInterval) noexcept;
	void animate(int32_t nGameTick, double fGameInterval) noexcept;

	struct SelectedTile;
	struct TileAni final : public TileAnimator
	{
		friend class TileAnimatorEvent;
		TileAni() noexcept : m_p0SelectedTile(nullptr) {}
		void reInit() noexcept
		{
			m_p0SelectedTile = nullptr;
		}
		double getCommonElapsed(int32_t nAni, int32_t nViewTick, int32_t nTotViewTicks) const noexcept;
		double getElapsed01(int32_t nHash, int32_t nX, int32_t nY, int32_t nAni, int32_t nViewTick, int32_t nTotTicks) const noexcept override;
		double getElapsed01(int32_t nHash, const LevelBlock& oLevelBlock, int32_t nBrickIdx, int32_t nAni, int32_t nViewTick, int32_t nTotTicks) const noexcept override;
		bool m_bWaiting;
		int32_t m_nNextStartTick; // if m_bWaiting == true
		int32_t m_nCountdown; // animation ticks down counter
		int32_t m_nTotTicks; // remember the number of ticks (for elapsed calc))
		int32_t m_nCount; // number of animations completed so far
		int32_t m_nTotCount; // Tot number of animations to be completed
		SelectedTile* m_p0SelectedTile;
		int32_t m_nAniNameIdx; // this copied here because otherwise we would need a pointer to TileAnimatorEvent
	};
	struct BlockData;
	struct SelectedTile
	{
		SelectedTile() noexcept : m_p0Container(nullptr) {}
		// if (m_refTileAni.isNull()) this is the index in m_aAllNotAnis
		int32_t m_nIdxInGlobalNotAnis;
		// if (m_refTileAni not null) iterator (pointer) in 
		//    m_oRunningAnis (if !m_refTileAni->m_bWaiting) or 
		//    m_oWaitingAnis (if m_refTileAni->m_bWaiting)
		std::list< shared_ptr<SelectedTile> >::iterator m_itGlobalAniContainer;
		// pointer to the container (either row or block)
		BlockData* m_p0Container;
		int32_t m_nXOrBrick; // nX if (m_p0Container->m_p0LevelBlock == null), nBrick otherwise
		shared_ptr<TileAni> m_refTileAni;
	};
	struct BlockData
	{
		explicit BlockData(TileAnimatorEvent* p0TileAnimatorEvent) noexcept
		: m_p0TileAnimatorEvent(p0TileAnimatorEvent)
		, m_p0LevelBlock(nullptr)
		{
			assert(p0TileAnimatorEvent != nullptr);
		}
		const TileAnimatorEvent* m_p0TileAnimatorEvent;
		LevelBlock* m_p0LevelBlock; // Board if m_p0LevelBlock == null
		int32_t m_nRow; // valid only if m_p0LevelBlock == null, Value: 0 .. m_oInit.m_nAreaH - 1
		// The map contains only selected bricks
		// If a brick is not visible (although selected) then m_oSelected[nBrickId] is null
		std::unordered_map<int32_t, shared_ptr<SelectedTile> > m_oSelected; // Key: nX (Board) or brick (m_p0LevelBlock != null) Value: null if not visible (m_p0LevelBlock != null)
	};

	bool createTileAniBoard(int32_t nNotAniIdx, int32_t nGameTick, double fGameInterval, int32_t nX, int32_t nY
							, shared_ptr<SelectedTile>& refSelectedTile) noexcept;
	bool createTileAniBlock(int32_t nNotAniIdx, int32_t nGameTick, double fGameInterval, LevelBlock* p0LevelBlock, int32_t nBrick
							, shared_ptr<SelectedTile>& refSelectedTile) noexcept;

	bool blockAddSelectedBrick(LevelBlock& oLevelBlock, int32_t nBrickId, shared_ptr<BlockData>& refBlockData) noexcept;
	void blockAddSelected(LevelBlock& oBlock) noexcept;
	void boardSegmentRemoveSelected(int32_t nRemoveY, int32_t nFromX, int32_t nToNX) noexcept;
	void boardSegmentAddSelected(int32_t nY, int32_t nFromX, int32_t nToNX) noexcept;

	std::list< shared_ptr<SelectedTile> >::iterator insertInWaitingAnis(const shared_ptr<SelectedTile>& refSelectedTile) noexcept;
	void removeFromNotAnis(int32_t nToRemoveIdx) noexcept;
	void addToNotAnis(const shared_ptr<SelectedTile>& refSelectedTile) noexcept;

	shared_ptr<TileAni> createTileAni(SelectedTile* p0SelectedTile, int32_t nGameTick, double fGameInterval) noexcept;
	void recyclePushBackSelectedTile(const shared_ptr<SelectedTile>& refSelectedTile) noexcept;
	shared_ptr<SelectedTile> recyclePopSelectedTile() noexcept;
	void recyclePushBackBlockData(const shared_ptr<BlockData>& refBlockData) noexcept;
	shared_ptr<BlockData> recyclePopBlockData() noexcept;

private:
	LocalInit m_oInit;

	int32_t m_nBoardWidth;
	int32_t m_nBoardHeight;

	enum TILE_ANIMATOR_STATE
	{
		TILE_ANIMATOR_STATE_ACTIVATE = 0,
		TILE_ANIMATOR_STATE_INIT = 1,
		TILE_ANIMATOR_STATE_RUN = 2,
		TILE_ANIMATOR_STATE_DEAD = 3
	};
	TILE_ANIMATOR_STATE m_eState;

	int32_t m_nCounter;

	std::vector< shared_ptr<SelectedTile> > m_aAllNotAnis; // shared_ptr<SelectedTile> is in here if m_refTileAni is null
	std::list< shared_ptr<SelectedTile> > m_oRunningAnis; // shared_ptr<SelectedTile> is in here if (m_refTileAni is not null) && (!m_bWainting)
	//TODO instead of list use std::multimap<NextStartTick,SelectedTile>: naturally ordered and Ins+Del is o(log2N)+o(log2N) instead of o(N)+o(1)
	std::list< shared_ptr<SelectedTile> > m_oWaitingAnis; // shared_ptr<SelectedTile> is in here if (m_refTileAni is not null) && m_bWainting
	int32_t m_nAnis; // == m_oRunningAnis.size() + m_oWaitingAnis.size()

	std::vector< shared_ptr<BlockData> > m_aRows; // Size: m_oInit.m_nAreaH, m_aRows[i].m_nRow = i
	std::unordered_map<int32_t, shared_ptr<BlockData> > m_oBlocks; // Key: LevelBlock::blockGetId

	std::list< shared_ptr<BlockData> > m_oInactiveBlockData;
	std::list< shared_ptr<SelectedTile> > m_oInactiveSelectedTile;
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

