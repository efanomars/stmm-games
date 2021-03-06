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
 * File:   scrollerevent.h
 */

#ifndef STMG_SCROLLER_EVENT_H_
#define STMG_SCROLLER_EVENT_H_

#include "event.h"

#include "util/coords.h"
#include "utile/extendedboard.h"
#include "utile/newrows.h"
#include "util/recycler.h"

#include <memory>

#include <stdint.h>

namespace stmg { class QueryTileRemoval; }
namespace stmg { class Tile; }
namespace stmg { class TileAnimator; }
namespace stmg { class TileBuffer; }
namespace stmg { class TileSelector; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

/** Event that scrolls the board.
 * The scrolling is done by moving the Show area (see Level::showGet())
 * fractions of a tile (see LocalInit::m_nSlices) over the board.
 * When the next tile is reached a new row is inserted at the bottom of the board.
 *
 * How a new row is generated can be controlled by input messages.
 *
 * If new rows are generated randomly, it is possible to discard a
 * row if certain tiles appear in it by setting an inhibitor, which can be
 * activated or deactivated with input messages.
 *
 * It is also possible to try to minimize free tile removals
 * (the meaning of which is implementation specific) when new rows are added.
 *
 * This class can currently only scroll down (new rows appear at the bottom of
 * the board).
 */
class ScrollerEvent : public Event, public ExtendedBoard
{
public:
	struct NewRowCheckRemover
	{
		QueryTileRemoval* m_p0TileRemover = nullptr; /**< The tile remover that checks new rows. Must be persistent while the game
														 * is running. This interface is usually implemented by an event added to the same Level.
														 * Non owning. Cannot be null. */
		int32_t m_nFrom = 0; /**< The start position of the new row segment that m_p0TileRemover should check.
								 * Must be within the board. Default is 0. */
		int32_t m_nTo = -1; /**< The end position of the new row segment that m_p0TileRemover should check.
								 * Must be either -1 or within the board and not smaller than m_nFrom.
								 * If -1 means last position in the board. Default is -1.  */
	};
	struct LocalInit
	{
		int32_t m_nRepeat = -1; /**< The number of scrolls. The default is -1 (means infinite). */
		int32_t m_nStep = 1; /**< The interval between scrolls in game ticks. Must be &gt;= 1. The default is 1. */
		int32_t m_nSlices = 6; /**< How many partial row scroll positions (within a tile) should be shown. Default is 6. */
		bool m_bKeepTopVisible = true; /**< Whether the first non empty top line should always be completly visible. Default is true. */
		int32_t m_nTopNotEmptyWaitTicks = 0; /**< Number of game ticks scrolling is suspended when top row not empty. Default is 0. */
		int32_t m_nTopNotEmptyWaitMillisec = 0; /**< Number of millisec scrolling is suspended when top row not empty.
												 * Adds to m_nTopNotEmptyWaitTicks. Default is 0. */
		unique_ptr< NewRows > m_refNewRows; /**< The new rows generator. Cannot be null. */
		std::vector< NewRowCheckRemover > m_aRemovers; /**< The removal checkers. Can be empty. */
		std::vector< unique_ptr<TileSelector> > m_aInhibitors; /**< Array of inhibitors. When active, the more tiles selected by an inhibitor
																 * appear in a new row the more likely a new row is discarded.
																 * See m_nCheckNewRowTries.
																 * The size of the array shouldn't exceed 100. */
		int32_t m_nCheckNewRowTries = 5; /**< The maximum number of random rows to be generated to minimize inhibited tiles and "free" removals.
										 * Only used if m_p0TileRemover is set or m_aInhibitors is not empty.
										 * If set to 1 inhibitors and tile remover are ignored.
										 * Must be positive. Default is 5. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit ScrollerEvent(Init&& oInit) noexcept;

protected:
	/** Reinitialization.
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept;
public:

	//Event
	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	// input
	enum {
		MESSAGE_PUSH_ROW = 100 /**< Beware! currently pushed rows are not counted (don't influence LocalInit::m_nRepeat). */
		, MESSAGE_PAUSE_TICKS = 101 /**< Number of game ticks to pause (passed through nValue). */
		, MESSAGE_PAUSE_MILLISEC = 102 /**< Number of milliseconds to pause (passed through nValue). */
		, MESSAGE_SET_SLICES = 110 /**< Change nr of slices (passed through nValue). */
		, MESSAGE_SET_NEW_ROW_GEN = 120 /**< nValue is index LocalInit::m_aNewRowTileGens. If negative sets to 0.
										 * If bigger than vector size sets last. */
		, MESSAGE_NEXT_NEW_ROW_GEN = 121 /**< Switches to next row generator. If last stays on last. */
		, MESSAGE_PREV_NEW_ROW_GEN = 122 /**< Switches to previous row generator. If first stays on first. */
		, MESSAGE_INHIBIT_START_INDEX_BASE = 200 /**< Starts applying an inhibitor to new rows. The inhibitor index into Init::m_aInhibitors
												 * is determined by subtracting MESSAGE_INHIBIT_START_INDEX_BASE from the message number.
												 * There can be at most 100 inhibitors. */
		, MESSAGE_INHIBIT_STOP_INDEX_BASE = 300 /**< Stops applying an ibhibitor to new rows. Like MESSAGE_INHIBIT_START_INDEX_BASE. */
	};
	// output
	enum {
		LISTENER_GROUP_PUSHED = 10 /**< Sent if a row was pushed with nValue containing the row generator. */
		, LISTENER_GROUP_TOP_COLUMN = 11 /**< Sent for each column (nValue) with non empty tile in the to be destroyed top row. */
		, LISTENER_GROUP_TOP_DESTROYED = 12 /**< Sent when the top line is destroyed (after the LISTENER_GROUP_TOP_COLUMNs). */
		, LISTENER_GROUP_TOP_NOT_EMPTY_START = 20 /**< Sent when the top line not empty wait starts.
													 * nValue contains the initial number of wait game ticks. */
		, LISTENER_GROUP_TOP_NOT_EMPTY_END = 21 /**< Sent when the top line was cleared or destroyed (before LISTENER_GROUP_TOP_COLUMN).
												 * nValue is always 0. */
		, LISTENER_GROUP_TOP_NOT_EMPTY_TICK = 22 /**< Sent when the top line not empty wait changes.
													 * nValue contains the number of wait game ticks. */
	};

	const Tile& boardGetTile(int32_t nX, int32_t nY) const noexcept override;
	const TileAnimator* boardGetTileAnimator(int32_t nX, int32_t nY, int32_t nIdxTileAni) const noexcept override;

private:
	void commonInit(LocalInit&& oInit) noexcept;
	void resetRuntime() noexcept;

	void checkCanBePushed() noexcept;
	bool checkTopLineNotBusy() noexcept;
	bool checkLineEmpty(int32_t nY) noexcept;

	void pushRow() noexcept;

	int32_t getInhibited(const TileBuffer& oTiles) const noexcept;
	int32_t getWillRemove() const noexcept;

	void informNotEmptyTopLineColumns() noexcept;

private:
	int32_t m_nTileAniRemovingIndex;

	enum SCROLLER_STATE
	{
		SCROLLER_STATE_ACTIVATE = 0,
		SCROLLER_STATE_INIT = 1,
		SCROLLER_STATE_RUN = 2
	};
	SCROLLER_STATE m_eState;

	unique_ptr< NewRows > m_refNewRows;

	std::vector< unique_ptr<TileSelector> > m_aInhibitors;
	std::vector< bool > m_aInhibitorActive; // Size: m_aInhibitors.size()

	std::vector< NewRowCheckRemover > m_aRemovers;

	int32_t m_nBoardW;
	int32_t m_nBoardH;

	int32_t m_nRepeat;
	int32_t m_nStep;
	int32_t m_nSlices;
	int32_t m_nCheckNewRowTries;
//	QueryTileRemoval* m_p0TileRemover;

	bool m_bKeepTopVisible;
	int32_t m_nTopNotEmptyWaitMillisec;
	int32_t m_nTopNotEmptyWaitTicks;

	int32_t m_nCurRandomGen; // index into m_aRandomRows
	int32_t m_nCounter;
	int32_t m_nSliceCounter;
	int32_t m_nWaitTicks;
	int32_t m_nToPushUp;
	int32_t m_nLastPushUpTime;
	int32_t m_nCurSlices;
	int32_t m_nNextSlices;

	bool m_bWaitingBecauseNotEmptyTop;

	Coords m_oCoords;
	shared_ptr<TileBuffer> m_refCurTileBuf;
	shared_ptr<TileBuffer> m_refBestTileBuffer;
	Recycler<TileBuffer> m_oTileBufferRecycler;
private:
	ScrollerEvent();
	ScrollerEvent(const ScrollerEvent& oSource);
	ScrollerEvent& operator=(const ScrollerEvent& oSource);
};

} // namespace stmg

#endif	/* STMG_SCROLLER_EVENT_H_ */

