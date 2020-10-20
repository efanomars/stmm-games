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
 * File:   level.h
 */

#ifndef STMG_LEVEL_H
#define STMG_LEVEL_H

#include "levelblock.h"
#include "levellisteners.h"
#include "gameproxy.h"
#include "levelview.h"
#include "levelshow.h"
#include "apppreferences.h"

#include "variable.h"

#include "private-listenerstk.h"

#include "animations/textanimation.h"
#include "event.h"
#include "levelanimation.h"
#include "tile.h"
#include "util/recycler.h"
#include "util/basictypes.h"
#include "util/direction.h"
#include "util/namedobjindex.h"

#include <vector>
#include <list>
#include <unordered_map>
#include <string>
#include <memory>
#include <cassert>
#include <algorithm>
#include <utility>

#include <stdint.h>

namespace stmg { class Coords; }
namespace stmg { class KeyActionEvent; }
namespace stmg { class Named; }
namespace stmg { class TileCoords; }
namespace stmg { class TileRect; }
namespace stmi { class Event; }

namespace stmg
{

using std::unique_ptr;
using std::shared_ptr;

class Game;
class GameWidget;
class TileAnimator;

class Level
{
public:
	struct Init
	{
		int32_t m_nBoardW = 0; /**< The board width. Must be &gt; 0. */
		int32_t m_nBoardH = 0; /**< The board height. Must be &gt; 0. */
		std::vector< Tile > m_aBoard; /**< The initial board tiles.
										 * m_aBoard[nX + nY * nBoardW]: nX from 0 to nBoardW-1, nY from 0 to nBoardH-1. */
		int32_t m_nShowW = 0; /**< The show width. Must be &gt; 0 and &lt;= m_nBoardW. */
		int32_t m_nShowH = 0; /**< The show height. Must be &gt; 0 and &lt;= m_nBoardH. */
		int32_t m_nShowInitX = 0; /**< The show initial x position within the board. Must be &gt;= 0 and &lt;= m_nBoardW - m_nShowW. */
		int32_t m_nShowInitY = 0; /**< The show initial y position within the board. Must be &gt;= 0 and &lt;= m_nBoardH - m_nShowH. */
		int32_t m_nSubshowW = 0; /**< The player subshow width. If &lt;=0 no subshow mode, otherwise must be &lt;= m_nShowW. */
		int32_t m_nSubshowH = 0; /**< The player subshow height. If &lt;=0 no subshow mode, otherwise must be &lt;= m_nShowH. */
		int32_t m_nInitialFallEachTicks = 8; /**< The initial fall rate of level blocks. Default is 8. */
	public:
		/** Gets the index within m_aBoard from coords.
		 * The correct values for m_nBoardW and m_nBoardH must be set before
		 * calling this function.
		 * @param oXY The coords.
		 * @return The index.
		 */
		inline int32_t getBoardIndex(NPoint oXY) const noexcept
		{
			return oXY.m_nX + oXY.m_nY * m_nBoardW;
		}
		static int32_t getBoardIndex(NPoint oXY, NSize oBoardSize) noexcept
		{
			return oXY.m_nX + oXY.m_nY * oBoardSize.m_nW;
		}
	};

	/** Construct level instance.
	 * @param p0Game The game. Cannot be null.
	 * @param nLevel The number to identify the level within the game.
	 * @param refPreferences The preferences. Cannot be null.
	 * @param oInit Initialization data.
	 */
	Level(Game* p0Game, int32_t nLevel, const shared_ptr<AppPreferences>& refPreferences, const Init& oInit) noexcept;
protected:
	/** Reinitializes the level instance.
	 * The level view is set to null.
	 * @param p0Game The game. Cannot be null.
	 * @param nLevel The number to identify the level within the game.
	 * @param refPreferences The preferences. Cannot be null.
	 * @param oInit Initialization data.
	 */
	void reInit(Game* p0Game, int32_t nLevel, const shared_ptr<AppPreferences>& refPreferences, const Init& oInit) noexcept;

	friend class Game;
	/** Signals the start of the game.
	 * This function is called by Game when a new game is started. */
	void gameStart() noexcept;

	/** Extracts all events from the level.
	 * This can be used for recycling.
	 * @return The owning vector of event
	 */
	std::vector< unique_ptr<Event> > extractAllEvents() noexcept;
	/** Set the level view.
	 * @param p0View The view. Can be null.
	 */
	void setLevelView(LevelView* p0View) noexcept;
public:
	/** Adds an event to the level (deactivated).
	 * @param refEvent The event. Cannot be null.
	 */
	void addEvent(unique_ptr<Event> refEvent) noexcept;
	/** Adds an event (identifiable by a string id) to the level (deactivated).
	 * @param sId The event id. If empty or name is already present the event is unnamed.
	 * @param refEvent The event. Cannot be null.
	 */
	void addEvent(const std::string& sId, unique_ptr<Event> refEvent) noexcept;
	/** Tells whether the event was added to the level.
	 * @param p0Event The event. Cannot be null.
	 * @return Whether event part of level.
	 */
	bool hasEvent(Event const * p0Event) const noexcept;
	/** Get an event by id.
	 * @param sId The string id.
	 * @return The event or null if not found.
	 */
	Event* getEventById(const std::string& sId) noexcept;

	/** The preferences.
	 * @return The const preferences.
	 */
	inline const AppPreferences& prefs() const noexcept { return *m_refPreferences; }
	/** The game proxy.
	 * @return The game.
	 */
	GameProxy& game() noexcept;
	/** The game proxy.
	 * @return The const game.
	 */
	const GameProxy& game() const noexcept;

	/** The level identifier within the game.
	 * @return The level number.
	 */
	int32_t getLevel() const noexcept;
	/** The number of players in the level.
	 * @return How many players in the level.
	 */
	int32_t getTotLevelPlayers() const noexcept;
	/** The number of teams in the level.
	 * @return How many teams in the level.
	 */
	int32_t getTotLevelTeams() const noexcept;
	/** The currently active human players.
	 * @return Vector of &lt;nLevelTeam, nMate&gt; pairs.
	 */
	// This is not const because it is calculated lazily TODO make it mutable
	const std::vector< std::pair<int32_t, int32_t> >& getActiveHumanPlayers() noexcept;

	/** The level show of the level.
	 * @return The level show.
	 */
	LevelShow& showGet() noexcept;
	/** Whether each level player has a subshow.
	 * @return Whether in subshow mode.
	 */
	inline bool subshowMode() const noexcept { return m_bSubshowMode; }
	/** The subshow of a level player.
	 * Call this function only if subshowMode() is true.
	 * @param nLevelTeam The level team.
	 * @param nMate The mate of the team.
	 * @return The player's subshow.
	 */
	LevelShow& subshowGet(int32_t nLevelTeam, int32_t nMate) noexcept;
	/** The subshow of a level player.
	 * Call this function only if subshowMode() is true.
	 * @param nLevelPlayer The level player.
	 * @return The player's subshow.
	 */
	LevelShow& subshowGet(int32_t nLevelPlayer) noexcept;

	/** The number of tile animations for both brick and board cell tiles.
	 * This number is determined when the game is started and corresponds to the
	 * size of game().getNamed().tileAnis() at that time.
	 * @return The number of tile animations.
	 */
	int32_t getNrTileAniAttrs() const noexcept;

	/** Factory interface for level blocks.
	 */
	class LevelBlockCreator
	{
	public:
		/** The factory function.
		 * @return The level block. Must not be null.
		 */
		virtual LevelBlock* create() noexcept = 0;
	};
	/** Creates a block from the board.
	 * The board cells cannot be empty when the block is created.
	 * The board cells used to build the block will be empty after the call.
	 * @param oCoords The coords of the cells on the board used to make the block. Cannot be empty.
	 * @param oCreator The level block creator factory.
	 * @param eMgmtType The level block type.
	 * @return Whether the block could be created.
	 */
	bool boabloUnfreeze(const Coords& oCoords
						, LevelBlockCreator& oCreator
						, LevelBlock::MGMT_TYPE eMgmtType) noexcept;
	/** Adds a level block to the level.
	 * Whether the block could be added to the level depends on the eMgmtType
	 * and the current state of the board.
	 * @param p0LevelBlock The level block. Cannot be null.
	 * @param eMgmtType How the level block is handled.
	 * @return Whether the block could be added.
	 */
	bool blockAdd(LevelBlock* p0LevelBlock, LevelBlock::MGMT_TYPE eMgmtType) noexcept;
	/** @see blockCycleControl(LevelBlock* p0LevelBlock, LevelBlock* p0Preferred).
	 */
	void blockCycleControl(int32_t nPlayer) noexcept;
	/** @see blockCycleControl(LevelBlock* p0LevelBlock, LevelBlock* p0Preferred).
	 */
	void blockCycleControl(int32_t nPlayer, LevelBlock* p0Preferred) noexcept;
	/** @see blockCycleControl(LevelBlock* p0LevelBlock, LevelBlock* p0Preferred).
	 */
	void blockCycleControl(LevelBlock* p0LevelBlock) noexcept;
	/** Gives the player control of another level block.
	 * If the level block is not controlled by a player or
	 * if there is no other level block the player can control this function does nothing.
	 *
	 * If the preferred player is null or not available the next level block is
	 * usually the next in a chain of level blocks ordered by their x position.
	 * @param p0LevelBlock The level block the player currently controls. Cannot be null.
	 * @param p0Preferred The preferred level block the player should control next. Can be null.
	 */
	void blockCycleControl(LevelBlock* p0LevelBlock, LevelBlock* p0Preferred) noexcept;
	/** Set the controllability of a level block.
	 * @param p0LevelBlock The level block. Cannot be null.
	 * @param bPlayerControllable Whether players can control the level block.
	 * @param nControllerTeam The level team allowed to control the block or -1 if any.
	 */
	void blockSetControllable(LevelBlock* p0LevelBlock, bool bPlayerControllable, int32_t nControllerTeam) noexcept;

	/** Tells whether block can be placed on board.
	 * @param oLevelBlock The level block.
	 * @param bStrict Whether to ensure that the corresponding board cells are empty.
	 * @return Whether the block could be placed.
	 */
	bool blockCanPlaceOnBoard(const LevelBlock& oLevelBlock, bool bStrict) const noexcept;
	/** Sets the owner of the cells occupied  by the block's visible bricks of its current shape.
	 * The owner is the block itself. If a cell already has an owner it is overwritten.
	 * @param oLevelBlock The level block.
	 */
	void boabloOwnerBlockSet(LevelBlock& oLevelBlock) noexcept;
	/** Clears the owner of the cells occupied by the block's visible bricks of its current shape.
	 * If a cell is owned, it has to be owned by oLevelBlock.
	 * @param oLevelBlock The level block.
	 */
	void boabloOwnerBlockClear(LevelBlock& oLevelBlock) noexcept;
	/** Whether any of the block's visible bricks of its current shape are within a rectangle.
	 * @param oLevelBlock The level block.
	 * @param nX The x of the rectangle (in tiles).
	 * @param nY The y of the rectangle (in tiles).
	 * @param nW The width of the rectangle (in tiles).
	 * @param nH The height of the rectangle (in tiles).
	 * @return Whether block intersects with rectangle.
	 */
	bool blockIntersectsArea(const LevelBlock& oLevelBlock, int32_t nX, int32_t nY, int32_t nW, int32_t nH) const noexcept;
	/** Whether all of the the block's visible bricks of its current shape are within a rectangle after a move.
	 * @param oLevelBlock The level block.
	 * @param nMoveX The move in the x axis (in tiles).
	 * @param nMoveY The move in the y axis (in tiles).
	 * @param nX The x of the rectangle (in tiles).
	 * @param nY The y of the rectangle (in tiles).
	 * @param nW The width of the rectangle (in tiles).
	 * @param nH The height of the rectangle (in tiles).
	 * @return Whether block within rectangle after move.
	 */
	bool blockMoveIsWithinArea(const LevelBlock& oLevelBlock, int32_t nMoveX, int32_t nMoveY
								, int32_t nX, int32_t nY, int32_t nW, int32_t nH) const noexcept;

	/** Adds a a blocks listener.
	 * @param p0Listener The listener. Cannot be null.
	 */
	void blocksAddListener(BlocksListener* p0Listener) noexcept;
	/** Removes a a blocks listener.
	 * @param p0Listener The listener. Cannot be null.
	 */
	void blocksRemoveListener(BlocksListener* p0Listener) noexcept;
	/** Adds a block's bricks change listener.
	 * @param p0Listener The listener. Cannot be null.
	 */
	void blocksAddBricksIdListener(BlocksBricksIdListener* p0Listener) noexcept;
	/** Removes a block's bricks change listener.
	 * @param p0Listener The listener. Cannot be null.
	 */
	void blocksRemoveBricksIdListener(BlocksBricksIdListener* p0Listener) noexcept;
	/** Adds a block's player change listener.
	 * @param p0Listener The listener. Cannot be null.
	 */
	void blocksAddPlayerChangeListener(BlocksPlayerChangeListener* p0Listener) noexcept;
	/** Removes a block's player change listener.
	 * @param p0Listener The listener. Cannot be null. Must exist.
	 */
	void blocksRemovePlayerChangeListener(BlocksPlayerChangeListener* p0Listener) noexcept;

	/** Get the level block by id.
	 * @param nId The id.
	 * @return The level block or null if not active (was removed).
	 */
	LevelBlock* blocksGet(int32_t nId) noexcept;
	/** All the ids if the active level blocks.
	 * @return The ids.
	 */
	std::vector<int32_t> blocksGetAllIds() noexcept;
	/** All the active level blocks.
	 * This is supposed to be called once and then changes tracked by installing a
	 * BlocksListener.
	 * @return The level blocks. Are all non null.
	 */
	std::vector<LevelBlock*> blocksGetAll() noexcept;

	/** Add a listener to events from other levels.
	 * If the listener already is added this function does nothing.
	 * @param p0OthersListener The listener. Cannot be null.
	 * @return False if listener already added.
	 */
	bool othersAddListener(Event* p0OthersListener) noexcept;
	/** Removes a listener to events from other levels.
	 * If the listener is not present this function does nothing.
	 * @param p0OthersListener The listener. Cannot be null.
	 * @return False if listener not found.
	 */
	bool othersRemoveListener(Event* p0OthersListener) noexcept;

	/** The width of the board in tiles.
	 * @return The width.
	 */
	int32_t boardWidth() const noexcept;
	/** The height of the board in tiles.
	 * @return The height.
	 */
	int32_t boardHeight() const noexcept;

	/** Sets the tile of a board cell.
	 * @param nX The x (in tiles). Must be &gt;= 0 and &lt; boardWidth().
	 * @param nY The y (in tiles). Must be &gt;= 0 and &lt; boardHeight().
	 * @param oTile The tile.
	 */
	void boardSetTile(int32_t nX, int32_t nY, const Tile& oTile) noexcept;
	/** Returns the tile of a board cell.
	 * @param nX The x (in tiles). Must be &gt;= 0 and &lt; boardWidth().
	 * @param nY The y (in tiles). Must be &gt;= 0 and &lt; boardHeight().
	 * @return The tile.
	 */
	const Tile& boardGetTile(int32_t nX, int32_t nY) const noexcept;
	/** Sets the level block owner of a board's cell.
	 * @param nX The x (in tiles). Must be &gt;= 0 and &lt; boardWidth().
	 * @param nY The y (in tiles). Must be &gt;= 0 and &lt; boardHeight().
	 * @param p0LevelBlock The level block. Can be null.
	 */
	void boardSetOwner(int32_t nX, int32_t nY, LevelBlock* p0LevelBlock) noexcept;
	/** Returns the level block owner of a board cell.
	 * @param nX The x (in tiles). Must be &gt;= 0 and &lt; boardWidth().
	 * @param nY The y (in tiles). Must be &gt;= 0 and &lt; boardHeight().
	 * @return The level block. Can be null.
	 */
	LevelBlock* boardGetOwner(int32_t nX, int32_t nY) const noexcept;

	/** Get the elapsed unary time of a certain tile animation of a cell.
	 * This function calls the tile animator of the cell.
	 * @param nX The x (in tiles). Must be &gt;= 0 and &lt; boardWidth().
	 * @param nY The y (in tiles). Must be &gt;= 0 and &lt; boardHeight().
	 * @param nIdxTileAni The tile animation. Must be &gt;= 0 and &lt; getNrTileAniAttrs().
	 * @param nViewTick The view tick. Must be &gt;= 0 and &lt; nTotTicks.
	 * @param nTotViewTicks The number of view ticks. Must be &gt;= 1.
	 * @return A number from 0.0 to 1.0 or -1.0 if no tile animator is defined or tile animation not active.
	 */
	double boardGetTileAniElapsed(int32_t nX, int32_t nY, int32_t nIdxTileAni, int32_t nViewTick = 0, int32_t nTotViewTicks = 1) const noexcept;
	/** Set the tile animator of a cell for a certain tile animation.
	 * @param nX The x (in tiles). Must be &gt;= 0 and &lt; boardWidth().
	 * @param nY The y (in tiles). Must be &gt;= 0 and &lt; boardHeight().
	 * @param nIdxTileAni The tile animation. Must be &gt;= 0 and &lt; getNrTileAniAttrs().
	 * @param p0TileAnimator The tile animator. Can be null.
	 * @param nHash A number that is passed to the TileAnimator when boardGetTileAniElapsed() calls it.
	 */
	void boardSetTileAnimator(int32_t nX, int32_t nY, int32_t nIdxTileAni, TileAnimator* p0TileAnimator, int32_t nHash) noexcept;
	/** Returns the TileAnimator for a certain tile animation.
	 * @param nX The x (in tiles). Must be &gt;= 0 and &lt; boardWidth().
	 * @param nY The y (in tiles). Must be &gt;= 0 and &lt; boardHeight().
	 * @param nIdxTileAni The tile animation. Must be &gt;= 0 and &lt; getNrTileAniAttrs().
	 * @return The tile animator. Can be null.
	 */
	const TileAnimator* boardGetTileAnimator(int32_t nX, int32_t nY, int32_t nIdxTileAni) const noexcept;

	/** Adds a listener to board scrolls.
	 * @param p0Listener The listener. Cannot be null.
	 */
	void boardScrollAddListener(BoardScrollListener* p0Listener) noexcept;
	/** Removes a listener to board scrolls.
	 * @param p0Listener The listener. Cannot be null.
	 */
	void boardScrollRemoveListener(BoardScrollListener* p0Listener) noexcept;
	/** Adds a listener to board modifications.
	 * @param p0Listener The listener. Cannot be null.
	 */
	void boardAddListener(BoardListener* p0Listener) noexcept;
	/** Removes a listener to board modifications.
	 * @param p0Listener The listener. Cannot be null.
	 */
	void boardRemoveListener(BoardListener* p0Listener) noexcept;

	/** Scrolls the board contents in a certain direction.
	 * Same as boardInsert with params nX=0, nY=0, nW=boardWidth(), nH=boardHeight()
	 * but with a different meaning. It moves the auto scrolled level blocks
	 * in the direction of the scroll.
	 * In the same way it also moves animations tied to the board.
	 * It calls BoardScrollListener listeners.
	 *
	 * If a LevelBlock that owns a cell that
	 * is about to be scrolled out of the board, it should do something in its
	 * BoardScrollListener::boardPreScroll function (provided it is a listener),
	 * otherwise it will either be frozen, destroyed or removed.
	 *
	 * The caller of this function should expect listeners might keep
	 * a reference to the buffer.
	 *
	 * A null buffer means all new tiles are empty.
	 * @param eDir The direction of the scroll.
	 * @param refTiles The tiles to be inserted. Can be null.
	 */
	void boardScroll(Direction::VALUE eDir, const shared_ptr<TileRect>& refTiles) noexcept;
	/** Inserts a row or column in a rectangle area.
	 * Calls BoardListener::boardPreInsert and BoardListener::boardPostInsert.
	 *
	 * If refTiles is null empty tiles are inserted. If it isn't it's size must
	 * be equal or bigger than (oArea.m_nW, oArea.m_nH).
	 *
	 * Doesn't change the rectangle's cells' owner (level block). The tile
	 * animations move (and get removed) with the cell tile.
	 *
	 * Examples:
	 *
	 *        0123456789    0123456789
	 *      0 |--------|    |--------|
	 *      1 |        |    |        |
	 *      2 |  AaAA  |    |  XxXX  |   boardInsert(DOWN, {3,2,4,5}, {XxXX})
	 *      3 |  BbBB  |    |  AaAA  |
	 *      4 |  CcCC  |    |  BbBB  |     TileAnis of EeEE are removed,
	 *      5 |  DdDD  |    |  CcCC  |     the others moved down
	 *      6 |  EeEE  |    |  DdDD  |
	 *      7 |        |    |        |
	 *      8 |        |    |        |
	 *      9 |--------|    |--------|
	 *
	 *        0123456789
	 *      0 |--------|
	 *      1 |        |
	 *      2 |  BbBB  |   boardInsert(UP, {3,2,4,5}, {XxXX})
	 *      3 |  CcCC  |
	 *      4 |  DdDD  |     TileAnis of AaAA are removed,
	 *      5 |  EeEE  |     the others moved up
	 *      6 |  XxXX  |
	 *      7 |        |
	 *      8 |        |
	 *      9 |--------|
	 *
	 *        0123456789
	 *      0 |--------|
	 *      1 |        |
	 *      2 |  XAaA  |   boardInsert(RIGHT, {3,2,4,5}, {XxXXX})
	 *      3 |  xBbB  |
	 *      4 |  XCcC  |     TileAnis of ABCDE (row x=6) are removed,
	 *      5 |  XDdD  |     the others moved right
	 *      6 |  XEeE  |
	 *      7 |        |
	 *      8 |        |
	 *      9 |--------|
	 *
	 *        0123456789
	 *      0 |--------|
	 *      1 |        |
	 *      2 |  aAAX  |   boardInsert(LEFT, {3,2,4,5}, {XxXXX})
	 *      3 |  bBBx  |
	 *      4 |  cCCX  |     TileAnis of ABCDE (row x=3) are removed,
	 *      5 |  dDDX  |     the others moved left
	 *      6 |  eEEX  |
	 *      7 |        |
	 *      8 |        |
	 *      9 |--------|
	 * @param eDir The direction of the insertion.
	 * @param oArea The rectangle. Must be completely within the board and have positive width and height.
	 * @param refTiles The tiles to be inserted. Can be null.
	 */
	void boardInsert(Direction::VALUE eDir, NRect oArea, const shared_ptr<TileRect>& refTiles) noexcept;
	/** Modify the cells of given area.
	 * Calls BoardListener::boardPreModify and BoardListener::boardPostModify.
	 *
	 * The tile animations and ownership are left unchanged.
	 * @param oTileCoords The modified tiles and their position. Must be all within the board.
	 */
	void boardModify(const TileCoords& oTileCoords) noexcept;
	/** Destroy single tiles of the board.
	 * The tiles are set to empty.
	 * Calls BoardListener::boardPreDestroy and BoardListener::boardPostDestroy.
	 *
	 * The tile animations and block ownership are left unchanged.
	 *
	 * The difference with modifying the tiles to empty tiles is that the
	 * view can automatically create explosions.
	 * @param oCoords The coords of the board cells. Must be within the board.
	 */
	void boardDestroy(const Coords& oCoords) noexcept;

	/** Get existing variable.
	 * The variable must exist! Use game().hasVariableId() to find out.
	 * @param nId The variable id.
	 * @param nLevelTeam The level team or -1 if game variable.
	 * @param nMate The mate or -1 if team or game variable.
	 * @return The const variable. Its address is stable throughout a game.
	 */
	const Variable& variable(int32_t nId, int32_t nLevelTeam, int32_t nMate) const noexcept;
	/** Get existing variable.
	 * The variable must exist! Use game().hasVariableId() to find out.
	 * @param nId The variable id.
	 * @param nLevelTeam The level team or -1 if game variable.
	 * @param nMate The mate or -1 if team or game variable.
	 * @return The variable. Its address is stable throughout a game.
	 */
	Variable& variable(int32_t nId, int32_t nLevelTeam, int32_t nMate) noexcept;

	/** Get a named widget.
	 * @param sWidgetName The name of the widget. Cannot be empty.
	 * @param nLevelTeam The level team or -1 if a game widget.
	 * @param nMate The mate or .1 if a game or team widget.
	 * @return The widget or null if not found.
	 */
	shared_ptr<GameWidget> widgetsGet(const std::string& sWidgetName, int32_t nLevelTeam, int32_t nMate) noexcept;

	/** Adds a level animation.
	 * If present this function calls the view's animationCreate.
	 * @param refLevelAnimation The animation. Cannot be null.
	 * @param eRefSys The reference system.
	 * @param fDelayMillisec The delay in milliseconds. Must be &gt;= 0.
	 */
	void animationAdd(const shared_ptr<LevelAnimation>& refLevelAnimation, LevelAnimation::REFSYS eRefSys, double fDelayMillisec) noexcept;
	/** Shortcut for animationAdd(refLevelAnimation, eRefSys, 0.0).
	 */
	inline void animationAdd(const shared_ptr<LevelAnimation>& refLevelAnimation, LevelAnimation::REFSYS eRefSys) noexcept
	{
		animationAdd(refLevelAnimation, eRefSys, 0.0);
	}
	/** Adds a scrolled level animation.
	 * If present this function calls the view's animationCreate.
	 * @param refLevelAnimation The animation. Cannot be null.
	 * @param fDelayMillisec The delay in milliseconds. Must be &gt;= 0.
	 */
	void animationAddScrolled(const shared_ptr<LevelAnimation>& refLevelAnimation, double fDelayMillisec) noexcept;
	/** Shortcut for animationAddScrolled(refLevelAnimation, 0.0).
	 */
	inline void animationAddScrolled(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
	{
		animationAddScrolled(refLevelAnimation, 0.0);
	}
	/** Remove a level animation.
	 * If the animation (scrolled or not) was already removed this function does nothing.
	 * @param refLevelAnimation The animation. Cannot be null.
	 */
	void animationRemove(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept;
	/** Tells the view it should animate the tiles of an area.
	 * See boardAnimateTile(NPoint oXY).
	 * @param oArea The area to animate.
	 */
	inline void boardAnimateTiles(NRect oArea) noexcept
	{
		if (m_p0View != nullptr) {
			m_p0View->boardAnimateTiles(oArea);
		}
	}
	/** Tells the view it should animate a tile.
	 * The tile will be animated during the view ticks following the current game tick
	 * by calling the tile animator of the tile if present or just redrawing it.
	 * @param oXY The tile to animate. Must be within board.
	 */
	inline void boardAnimateTile(NPoint oXY) noexcept
	{
		if (m_p0View != nullptr) {
			m_p0View->boardAnimateTile(oXY);
		}
	}

	/** Activate event.
	 * If nStart is smaller than the current game tick (see Game::gameElapsed() function),
	 * the event is started in the current game tick. This is a way to trigger an event
	 * before events with higher priority.
	 * @param p0Event The event. Cannot be null.
	 * @param nStart The triggering time in game ticks. Must be &gt;= 0.
	 */
	void activateEvent(Event* p0Event, int32_t nStart) noexcept;
	/** Deactivate event.
	 * @param p0Event The event. Cannot be null.
	 */
	void deactivateEvent(Event* p0Event) noexcept;

	/** Calls the trigger function of an event.
	 * The triggering event must be owned by the same level of this event.
	 * This function deactivates the event without setting its trigger time to -1,
	 * so that the event's trigger function can find out whether it was supposed
	 * to trigger later.
	 * @param p0Event The event. Cannot be null.
	 * @param nMsg The message. Must be &gt;= -1.
	 * @param nValue The value.
	 * @param p0TriggeringEvent The triggering event or null.
	 */
	void triggerEvent(Event* p0Event, int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept;

	/** Gets the fall rate of level blocks for this level.
	 * @return The interval in game ticks between level block fall.
	 */
	int32_t getFallEachTicks() noexcept;
	/** Sets the fall rate of level blocks for this level.
	 * The LevelBlock::fall() function is called each nFallEachTicks game ticks.
	 *
	 * The new value has only effect in the next game tick.
	 * @param nFallEachTicks The new interval in game ticks between level block fall.
	 */
	void setFallEachTicks(int32_t nFallEachTicks) noexcept;
	/** Get the requested interval for the level.
	 * This function is called by the game to calculate the next game interval.
	 * @return The (next) interval im milliseconds.
	 */
	double getInterval() noexcept;
	/** Set the requested game interval for the level.
	 * @param fMillisec The new interval im milliseconds.
	 */
	void setInterval(double fMillisec) noexcept;

	/** Declare player dead.
	 * If it was the last mate of the team gameStatusFailed() for the team is called.
	 * @param nLevelTeam The team of the player.
	 * @param nMate The mate number of the player.
	 * @param bForceOtherCompleted If last of a failed team declare the only other team completed the game.
	 * @param bCreateTexts Whether to create standard text animations.
	 * @return Whether it succeeded.
	 */
	bool gameStatusPlayerOut(int32_t nLevelTeam, int32_t nMate, bool bForceOtherCompleted, bool bCreateTexts) noexcept;
	/** Declare team completed the game.
	 * @param nLevelTeam The level team that completed the level.
	 * @param bForceOthersFailed Whether other teams should fail immediately or go on playing.
	 * @param bCreateTexts Whether to create standard text animations.
	 * @return Whether it succeeded.
	 */
	bool gameStatusCompleted(int32_t nLevelTeam, bool bForceOthersFailed, bool bCreateTexts) noexcept;
	/** Declare team failed the game.
	 * @param nLevelTeam The team that failed.
	 * @param bForceOtherCompleted If only one other team still playing declare it completed the game.
	 * @param bCreateTexts Whether to create standard text animations.
	 * @return Whether it succeeded.
	 */
	bool gameStatusFailed(int32_t nLevelTeam, bool bForceOtherCompleted, bool bCreateTexts) noexcept;
	/** Technical game over.
	 * @param aIssue Problem description text.
	 */
	void gameStatusTechnical(const std::vector<std::string>& aIssue) noexcept;

	inline Named& getNamed() noexcept { return game().getNamed(); }
	inline const Named& getNamed() const noexcept { return game().getNamed(); }
	/** The elapsed time since the game start in intervals.
	 * This a shortcut of game().gameElapsed().
	 * @return The elapsed time (game ticks).
	 */
	inline int32_t gameElapsed() const noexcept { return game().gameElapsed(); }

#ifndef NDEBUG
	void dump(bool bDumpBoard, bool bDumpLevelBlocks, bool bDumpTileAni, bool bDumpCharNames, bool bDumpColorNames, bool bDumpFontNames) const noexcept;
#endif

private:
	struct TeamData;
	bool gameStatusTeamIsPlaying(int32_t nLevelTeam) const noexcept;
	void gameStatusSetTeamStatus(int32_t nLevelTeam, bool bCompleted) noexcept;
	void gameStatusSetTeamRank(int32_t nLevelTeam, int32_t nRank) noexcept;
	void gameStatusWinnerTexts(TeamData& oTeamData) noexcept;
	void gameStatusLoserTexts(TeamData& oTeamData) noexcept;

	void eventsQueueSetDirty(bool bDirty) noexcept;
	bool eventsQueueIsDirty() const noexcept;
	void othersReceive(int32_t nSenderLevel, int32_t nMsg, int32_t nValue) noexcept;

	LevelBlock* getControlled(int32_t nLevelTeam, int32_t nMate) noexcept;
	//TODO Game dispatches inputs for all levels after all handlePreTimer (and before handleTimer) in the order they came in
	//TODO Make additional specialization handleXYInput()
	void handleInput(int32_t nLevelTeam, int32_t nMate, const shared_ptr<stmi::Event>& refEvent) noexcept;
	void handleKeyActionInput(int32_t nLevelTeam, int32_t nMate, const shared_ptr<KeyActionEvent>& refEvent) noexcept;

	void handlePreTimer() noexcept;
	void handleTimer() noexcept;
	void handleTimerEvents() noexcept;
	void handlePostTimer() noexcept;

	void deactivateEvent(Event* p0Event, bool bPreserveTriggerTime) noexcept;

	void deInit() noexcept;

	void variablesInit(const NamedObjIndex<Variable::VariableType>& oTeamVariableTypes
						, const NamedObjIndex<Variable::VariableType>& oMateVariableTypes) noexcept;
	/* Inhibits variables for all level, specific team or specific mate. */
	void variablesInhibit(int32_t nLevelTeam, int32_t nMate) noexcept;


	void animationStartDelayed() noexcept;
	void animationPrivAdd(int32_t nLevAniId, const shared_ptr<LevelAnimation>& refLevelAnimation, double fDelay) noexcept;
	void animationPrivAddScrolled(int32_t nLevAniId, const shared_ptr<LevelAnimation>& refLevelAnimation, double fDelay) noexcept;
	void animationPrivAddCommon(const shared_ptr<LevelAnimation>& refLevelAnimation, LevelAnimation::REFSYS eRefSys
								, double fDelayMillisec) noexcept;
	void animationCreateShowTextCommon(LevelAnimation::REFSYS eRefSys, int32_t nNamedIdx
										, std::vector<std::string>&& aLines, double fTextSize
										, double fX, double fY, double fW, double fH, int32_t nZ, int32_t nDuration) noexcept;
	struct CellAni
	{
		TileAnimator* m_p0TileAnimator;
		int32_t m_nHash;
	};
	struct Cell
	{
		Tile m_oTile;
		std::vector<CellAni> m_aCellAni;
		void clear(int32_t nTileAnis) noexcept;
		void clearOnlyTileAnis(int32_t nTileAnis) noexcept;
	};

	inline int32_t calcIndex(int32_t nPosX, int32_t nPosY) const noexcept { return nPosX + nPosY * m_nW; }

	static bool orderLevelBlocks(LevelBlock* p0Lhs, LevelBlock* p0Rhs) noexcept
	{
		return p0Lhs->blockPos().m_nX < p0Rhs->blockPos().m_nX;
	};

	friend class LevelBlock;

	void blockRemove(LevelBlock* p0LevelBlock) noexcept;
	void blockDestroy(LevelBlock* p0LevelBlock) noexcept;
	void blockFreeze(LevelBlock* p0LevelBlock) noexcept;
	void blockFuse(LevelBlock* p0Master, LevelBlock* p0Victim) noexcept;
	template<class CallbackFunction, typename...CallbackParams>
	void blockModify(LevelBlock* p0LevelBlock
					, const std::vector<int32_t>& aRemoveBrickId
					, const std::vector<int32_t>& aDestroyBrickId
					, const std::vector<int32_t>& aModifyPosBrickId
					, const std::vector<int32_t>& aModifyTileBrickId
					, bool bAddsBricks
					, const CallbackFunction& oFun
					, CallbackParams&... oParams) noexcept;

	bool coordsCanPlaceOnBoard(const Coords& oCoords, bool bStrict) const noexcept;
	void repositionLevelBlock(LevelBlock* p0LevelBlock) noexcept;

	void blockAddCommon(LevelBlock* p0LevelBlock) noexcept;
	void blockRemoveCommon(LevelBlock* p0LevelBlock) noexcept;

	void blockAddToControllable(LevelBlock* p0LevelBlock, int32_t nControllerTeam, int32_t nNotTeam) noexcept;
	void blockAssignControlToLongestWaitingMate(LevelBlock* p0LevelBlock, int32_t nControllerTeam, int32_t nNotTeam) noexcept;
	// returns whether unassigned and possibly another level block for the mate
	std::pair<bool, LevelBlock*> blockUnassignControlFromMate(LevelBlock* p0LevelBlock, int32_t nExceptIfTeam) noexcept;
	void blockRemoveFromControllable(LevelBlock* p0LevelBlock, int32_t nNotTeam) noexcept;
	void blockAssignToMate(LevelBlock* p0LevelBlock, int32_t nTeam, int32_t nMate, bool bWasntAssigned) noexcept;

	void informBlockChangePlayer(LevelBlock* p0Block, int32_t nOldPlayer) noexcept;

	template<class T>
	void boardMoveVector(std::vector<T>& aVec, Direction::VALUE eDir, NRect oArea
						, int32_t& nInsertX, int32_t& nInsertY) noexcept;
	void boardSetInserted(NRect oArea
						, int32_t nInsertX, int32_t nInsertY, const shared_ptr<TileRect>& refTiles) noexcept;

private:
	Game* m_p0Game;
	GameProxy m_oOwner;
	int32_t m_nLevelInGame;
	LevelView* m_p0View;

	shared_ptr<AppPreferences> m_refPreferences;
	bool m_bAllTeamsInOneLevel;
	// Shortcuts
	int32_t m_nTotLevelTeams;
	int32_t m_nTotLevelPlayers;

	int32_t m_nW;
	int32_t m_nH;
	int32_t m_nTotTileAnis;
	std::vector<Cell> m_aBoard; // size: m_nW * m_nH
	std::vector<LevelBlock*> m_aOwner; // size: m_nW * m_nH

	Private::ListenerStk<BoaBloListener> m_oBoaBloListenerStk;
	Private::ListenerStk<BoardListener> m_oBoardListenerStk;
	Private::ListenerStk<BoardScrollListener> m_oBoardScrollListenerStk;
	bool m_bBoardAllowOnlyModify; //TODO should it be just Freeze? (or Fusion for blocks!)

	Private::ListenerStk<BlocksListener> m_oBlocksListenerStk;
	Private::ListenerStk<BlocksBricksIdListener> m_oBlocksBricksIdListenerStk;

	std::vector<BlocksPlayerChangeListener*> m_aBlocksPlayerChangeListener;
	bool m_bBlockDisallowNestedPlayerChanges;

	//TODO instead of list use std::multimap<StartTick,Event*>: naturally ordered and Ins+Del is o(logN)+o(logN) instead of o(N)+o(1)
	std::list<Event*> m_oActiveEvents; // the Events are ordered by Event::getTriggerTime() and priority
	std::list<Event*> m_oInactiveEvents;

	std::vector< unique_ptr<Event> > m_aEvents;
	NamedObjIndex<Event*> m_oEventIds;

	LevelShow m_oShow;
	bool m_bSubshowMode;

	std::unordered_map<int32_t, LevelBlock*> m_oAllLevelBlocks; // key: LevelBlock::getId()
	struct MateData {
		LevelBlock* m_p0Controlled; // LevelBlock* is element of TeamData::m_oOrderedControllable
		int32_t m_nLastTimeInControl;
		LevelShow m_oSubshow;
		Variable::Variables m_oVariables;
		NamedObjIndex< shared_ptr<GameWidget> > m_oWidgets;
		shared_ptr<AppPreferences::PrefPlayer> m_refPrefPlayer;
		int32_t m_nLevelPlayer;
	};
	struct TeamData {
		// m_oOrderedControllable: contains all LevelBlocks that have team constraint (index in m_aTeamData)
		// or can be controlled by any team (-1)
		//TODO use vector!
		std::list<LevelBlock*> m_oOrderedControllable; // ordered according to orderLevelBlocks()
		int32_t m_nTotTeammates; // Shortcut (cached) value of `TeamData::m_refPrefTeam->getTotMates()`, constant during game
		int32_t m_nTotInControl; // number of teammates currently controlling a LevelBlock
		std::vector<MateData> m_aTeammate; // Size: m_nTotTeammates
		bool m_bGameEnded;
		Variable::Variables m_oVariables;
		NamedObjIndex< shared_ptr<GameWidget> > m_oWidgets;
		shared_ptr<AppPreferences::PrefTeam> m_refPrefTeam;
	};
	std::vector<TeamData> m_aTeamData; // size: m_nTotLevelTeams
	std::vector<MateData*> m_aLevelPlayer; // size: m_nTotLevelPlayers
	std::vector< std::pair<int32_t, int32_t> > m_aActiveHumanPlayers; // size: <= m_nTotLevelPlayers

	// helper arrays to choose a team and mate for a newly added LevelBlock*
	std::vector<int32_t> m_aHelperMinTeam; // Size: m_nTotLevelPlayers
	std::vector<int32_t> m_aHelperMinTeammate; // Size: m_nTotLevelPlayers

	int32_t m_nOthersNestedCalls;
	bool m_bDirtyEvents;
	std::list<Event*> m_oOthersListeners; // Value: can be nullptr temporarily until next others

	std::vector<int32_t> m_aDelayedLevAniIds;
	std::vector< shared_ptr<LevelAnimation> > m_aDelayedLevAnis; // When delay is smaller than gameTick, they become active
	// m_oDelayedScrAnis intersected with m_oDelayedLevAnis = empty set
	std::vector<int32_t> m_aDelayedScrAniIds;
	std::vector< shared_ptr<LevelAnimation> > m_aDelayedScrAnis; // When delay is smaller than gameTick, they become active
	// The active animations
	std::unordered_map< int32_t, shared_ptr<LevelAnimation> > m_oActiveLevAnis; // key: LevelAnimation::getId()
	// m_oActiveScrolledAnis is a subset of m_oActiveLevAnis
	std::unordered_map< int32_t, shared_ptr<LevelAnimation> > m_oActiveScrolledAnis; // key: LevelAnimation::getId()

	double m_fInterval; // The level interval
	int32_t m_nFallEachTicks;

	uint32_t m_nScrolledUnique;

	class PrivateTextAnimation : public TextAnimation
	{
	public:
		using TextAnimation::TextAnimation;
		void reInit(TextAnimation::Init&& oInit)
		{
			TextAnimation::reInit(std::move(oInit));
		}
	};
	static Recycler<PrivateTextAnimation, TextAnimation> s_oTextAnimationRecycler;
	static Recycler<TileCoords> s_oTileCoordsRecycler;

	int32_t m_nTotGameEndedTeams;
	static const std::string s_sGameOverPlayerOut;
	static const std::string s_sGameOverWinOne;
	static const std::string s_sGameOverLoseOne;
	static const std::string s_sGameOverOneLevelWinOne;
	static const std::string s_sGameOverOneLevelWinMany;
	static const std::string s_sGameOverManyLevelsWin;
	static const std::string s_sGameOverLoseMany;
	static const std::string s_sGameOverTechnical;

	static const int32_t s_nZObjectZShowText;
	static const int32_t s_nZObjectZGameOver;
private:
	Level(const Level& oSource) = delete;
	Level& operator=(const Level& oSource) = delete;
};

// Private implementation!
template<class CallbackFunction, typename...CallbackParams>
void Level::blockModify(LevelBlock* p0LevelBlock
						, const std::vector<int32_t>& aRemoveBrickId
						, const std::vector<int32_t>& aDestroyBrickId
						, const std::vector<int32_t>& aModifyPosBrickId
						, const std::vector<int32_t>& aModifyTileBrickId
						, bool bAddsBricks
						, const CallbackFunction& oFun
						, CallbackParams&... oParams) noexcept
{
	const bool bBricksIdHasListeners = !m_oBlocksBricksIdListenerStk.isEmpty();
//std::cout << "Level(" << m_nLevelInGame << ")::blockModify()  bBricksIdHasListeners=" << bBricksIdHasListeners << '\n';

	if (p0LevelBlock->m_bNestedModificationLock) {
		assert(false);
		return;
	}

	// Note: view() doesn't listen to this iface

	p0LevelBlock->m_bNestedModificationLock = true;

	std::vector<int32_t> aDeleteBrickId; // = aRemoveBrickId + aDestroyBrickId

	std::list< shared_ptr<Private::ListenerStk<BlocksBricksIdListener>::PreCalled> >::iterator itPreCalled;
	if (bBricksIdHasListeners) {
		aDeleteBrickId.insert(aDeleteBrickId.end(), aRemoveBrickId.begin(), aRemoveBrickId.end());
		aDeleteBrickId.insert(aDeleteBrickId.end(), aDestroyBrickId.begin(), aDestroyBrickId.end());
		itPreCalled = m_oBlocksBricksIdListenerStk.grabPreCalled();
		m_oBlocksBricksIdListenerStk.callPre(itPreCalled, &BlocksBricksIdListener::blockPreModify, *p0LevelBlock
											, aDeleteBrickId, aModifyPosBrickId, aModifyTileBrickId, bAddsBricks);
	}

	//NO View().blockPreModify call ! TODO If Z will be modifiable view will need to know!

	std::vector<int32_t> aAddedBlockId;
	(p0LevelBlock->* oFun)(aAddedBlockId, oParams...);
	assert(bAddsBricks == !aAddedBlockId.empty());

	//NO View().blockPostModify call !

	if (bBricksIdHasListeners) {
		m_oBlocksBricksIdListenerStk.callPost(itPreCalled, &BlocksBricksIdListener::blockPostModify, *p0LevelBlock
											, aDeleteBrickId, aModifyPosBrickId, aModifyTileBrickId, aAddedBlockId);
		m_oBlocksBricksIdListenerStk.freePreCalled(itPreCalled);
	}

	p0LevelBlock->m_bNestedModificationLock = false;
}

} // namespace stmg

#endif	/* STMG_LEVEL_H */

