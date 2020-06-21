/*
 * File:   levelblock.h
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

#ifndef STMG_LEVEL_BLOCK_H
#define STMG_LEVEL_BLOCK_H

#include "block.h"

#include "util/coords.h"
#include "util/direction.h"
#include "util/basictypes.h"

#include <vector>
#include <memory>
#include <list>
#include <tuple>
#include <unordered_map>
#include <utility>

#include <stdint.h>

namespace stmg { class KeyActionEvent; }
namespace stmg { class Tile; }
namespace stmi { class Event; }

namespace stmg
{

using std::unique_ptr;
using std::shared_ptr;

class Level;
class TileAnimator;

/** Base class for run-time Blocks.
 */
class LevelBlock
{
public:
	/** The origin position of the block in the board.
	 * @return The position.
	 */
	inline NPoint blockPos() const noexcept { return NPoint{m_nPosX, m_nPosY}; }
	/** The current brick ids of the block.
	 * The returned vector might be invalidated by operations such as removeBrick(),
	 * fuseTo(), remove(), etc.
	 * @return The brick ids.
	 */
	const std::vector<int32_t>& blockBrickIds() const noexcept;
	/** The tile of a brick.
	 * The returned tile might be invalidated by operations such as removeBrick(),
	 * fuseTo(), remove(), etc.
	 * @param nBrickId The brick id. Must be valid.
	 * @return The tile.
	 */
	inline const Tile& blockBrickTile(int32_t nBrickId) const noexcept { return m_oBlock.brick(nBrickId); }
	/** The position of the brick relative to the block's origin in the current shape.
	 * `blockPos() + blockBrickPos(nBrickId)` is the position within the board.
	 * @param nBrickId The brick id. Must be valid.
	 * @return The y position.
	 */
	inline NPoint blockBrickPos(int32_t nBrickId) const noexcept { return m_oBlock.shapeBrickPos(m_nShapeId, nBrickId); }
	/** The visibility of the brick in the current shape.
	 * @param nBrickId The brick id. Must be valid.
	 * @return Whether visible.
	 */
	inline bool blockBrickVisible(int32_t nBrickId) const noexcept { return m_oBlock.shapeBrickVisible(m_nShapeId, nBrickId); }
	/** The total number of bricks in the block.
	 * @return The number of bricks.
	 */
	inline int32_t blockBricksTot() const noexcept { return m_oBlock.totBricks(); }
	/** The total number of visible bricks in the current shape.
	 * @return Number of visible bricks.
	 */
	inline int32_t blockBricksTotVisible() const noexcept { return m_oBlock.shapeTotVisibleBricks(m_nShapeId); }
	/** The contact bricks in the current shape in a given direction.
	 * The position of the contacts is relative to the block's origin.
	 *
	 * Example: O is a brick, x is a contact
	 *
	 *      xx
	 *     xOO
	 *     OO
	 *
	 * There are only three contacts in the UP direction for the four bricks block shape.
	 * @param eDir The direction.
	 * @return The contacts.
	 */
	const std::vector< Block::Contact >& blockContacts(Direction::VALUE eDir) noexcept;
	/** The size of the current shape.
	 * If at least one brick is visible returns `{blockBricksMaxX() - blockBricksMinX() + 1, blockBricksMaxY() - blockBricksMinY() + 1}`.
	 * @return The size or {0,0} if no brick is visible.
	 */
	inline NSize blockSize() const noexcept { return m_oBlock.shapeSize(m_nShapeId); }
	/** The minimum position (relative to blockPos) of the visible bricks of the current shape.
	 * @return The min position or undefined if no bricks visible.
	 */
	inline NPoint blockBricksMinPos() const noexcept { return m_oBlock.shapeMinPos(m_nShapeId); }
	/** The maximum position (relative to blockPos) of the visible bricks of the current shape.
	 * @return The max position or undefined if no bricks visible.
	 */
	inline NPoint blockBricksMaxPos() const noexcept { return m_oBlock.shapeMaxPos(m_nShapeId); }

	/** The z position.
	 * @return The z (drawing priority).
	 */
	virtual int32_t blockPosZ() const noexcept = 0;
	/** The brick ids at a certain view tick.
	 * The default calls blockBrickIds().
	 * @param nViewTick The view tick. Must be &gt;= 0 and &lt; nTotViewTicks.
	 * @param nTotViewTicks The total number of view ticks in the game interval. Must be &gt; 0.
	 * @return The brick ids.
	 */
	virtual const std::vector<int32_t>& blockVTBrickIds(int32_t nViewTick, int32_t nTotViewTicks) const noexcept;
	/** The origin position of the block in the board at a certain view tick.
	 * The default calls blockPos().
	 * @param nViewTick The view tick. Must be &gt;= 0 and &lt; nTotViewTicks.
	 * @param nTotViewTicks The total number of view ticks in the game interval. Must be &gt; 0.
	 * @return The position.
	 */
	virtual FPoint blockVTPos(int32_t nViewTick, int32_t nTotViewTicks) const noexcept;
	/** The tile of a brick at a certain view tick.
	 * The default calls blockBrickTile().
	 *
	 * The returned tile might be invalidated by operations such as removeBrick(),
	 * fuseTo(), remove(), etc.
	 * @param nViewTick The view tick. Must be &gt;= 0 and &lt; nTotViewTicks.
	 * @param nTotViewTicks The total number of view ticks in the game interval. Must be &gt; 0.
	 * @param nBrickId The brick id. Must be valid.
	 * @return The tile.
	 */
	virtual const Tile& blockVTBrickTile(int32_t nViewTick, int32_t nTotViewTicks, int32_t nBrickId) const noexcept;
	/** The position of the brick relative to the block's origin in the current shape at a certain view tick.
	 * @param nViewTick The view tick. Must be &gt;= 0 and &lt; nTotViewTicks.
	 * @param nTotViewTicks The total number of view ticks in the game interval. Must be &gt; 0.
	 * @param nBrickId The brick id. Must be valid.
	 * @return The position.
	 */
	virtual NPoint blockVTBrickPos(int32_t nViewTick, int32_t nTotViewTicks, int32_t nBrickId) const noexcept;
	/** The visibility of the brick in the current shape at a certain view tick.
	 * @param nViewTick The view tick. Must be &gt;= 0 and &lt; nTotViewTicks.
	 * @param nTotViewTicks The total number of view ticks in the game interval. Must be &gt; 0.
	 * @param nBrickId The brick id. Must be valid.
	 * @return Whether visible.
	 */
	virtual bool blockVTBrickVisible(int32_t nViewTick, int32_t nTotViewTicks, int32_t nBrickId) const noexcept;
	/** The z position at a certain view tick.
	 * @param nViewTick The view tick. Must be &gt;= 0 and &lt; nTotViewTicks.
	 * @param nTotViewTicks The total number of view ticks in the game interval. Must be &gt; 0.
	 * @return The z (drawing priority).
	 */
	virtual int32_t blockVTPosZ(int32_t nViewTick, int32_t nTotViewTicks) const noexcept;

	/** The tile animation unary elapsed time.
	 * @param nBrickId The brick id. Must be valid.
	 * @param nIdxTileAni The tile ani index into Named::tileAnis().
	 * @param nViewTick The view tick. Must be &gt;= 0 and &lt; nTotViewTicks.
	 * @param nTotViewTicks The total number of view ticks in the game interval. Must be &gt; 0.
	 * @return The elapsed &gt;= 0.0 and &lt;=1.0 or -1.0 iftile animation not active.
	 */
	double blockGetTileAniElapsed(int32_t nBrickId, int32_t nIdxTileAni, int32_t nViewTick, int32_t nTotViewTicks) const noexcept;
	/** Set the tile animator for a certain tile animation and brick.
	 * The callback function is getElapsed01().
	 *
	 * The caller must ensure that the tile animator doesn't get deleted while
	 * active.
	 *
	 * If a tile animator is already installed it is overwritten.
	 * @param nBrickId The brick id. Must be valid.
	 * @param nIdxTileAni The tile ani index into Named::tileAnis().
	 * @param p0TileAnimator The tile animator. Cannot be null.
	 * @param nHash A custom value that will be passed to the callback.
	 */
	void blockSetTileAnimator(int32_t nBrickId, int32_t nIdxTileAni, TileAnimator* p0TileAnimator, int32_t nHash) noexcept;
	/** Get the current tile animator for the given tile animation and brick.
	 *
	 * @param nBrickId The brick id. Must be valid.
	 * @param nIdxTileAni The tile ani index into Named::tileAnis().
	 * @return The tile animator or null if none is defined.
	 */
	const TileAnimator* blockGetTileAnimator(int32_t nBrickId, int32_t nIdxTileAni) const noexcept;
	/** Removes this block from the level.
	 * @return Whether it succeeded.
	 */
	virtual bool remove() noexcept;
	/** Destroy this block.
	 * @return Whether it succeeded.
	 */
	virtual bool destroy() noexcept;
	/** Freeze this block.
	 * @return Whether it succeeded.
	 */
	virtual bool freeze() noexcept;
	/** Fuse to another block and get removed.
	 * @param oLevelBlock The master block.
	 * @return Whether it succeeded.
	 */
	virtual bool fuseTo(LevelBlock& oLevelBlock) noexcept;
	/** Whether it can fuse with another block.
	 * @param oLevelBlock The other (slave) block.
	 * @return Whether it can fuse.
	 */
	virtual bool canFuseWith(LevelBlock& oLevelBlock) const noexcept;
	/** Try to remove a brick from the block.
	 * @param nBrickId The brick id.
	 * @return Whether the brick could be removed.
	 */
	virtual bool removeBrick(int32_t nBrickId) noexcept;
	/** Try to destroy a brick from the block.
	 * @param nBrickId The brick id.
	 * @return Whether the brick could be destroyed.
	 */
	virtual bool destroyBrick(int32_t nBrickId) noexcept;

	enum MGMT_TYPE
	{
		MGMT_TYPE_INVALID = -1
		, MGMT_TYPE_NORMAL = 0 /**< LevelBlock is not automatically scrolled. */
		, MGMT_TYPE_AUTO_SCROLL = 1 /**< LevelBlock is scrolled automatically (ex. SwapperEvent). */
		, MGMT_TYPE_AUTO_OWNER = 2 /**< MGMT_TYPE_AUTO_SCROLL + Board-cell owner is set/reset for each visible brick. */
		, MGMT_TYPE_AUTO_STRICT_OWNER = 3 /**< MGMT_TYPE_AUTO_OWNER + owned Board-cell's tile is enforced to be empty (ex. BlockEvent) */
	};
	// Returns what was passed to Level::blockAdd or Level::blockUnfreeze
	// When not managed by the Level returns MGMT_TYPE_INVALID
	inline MGMT_TYPE blockGetMgmtType() noexcept { return m_eMgmtType; }

	/** The LevelBlock unique identifier.
	 * @return The id. Is non negative.
	 */
	inline int32_t blockGetId() const noexcept
	{
		return m_nId;
	};

	/** Whether players can control this level block.
	 * @return Whether instance controllable.
	 */
	bool isPlayerControllable() const noexcept { return m_bControllable; }
	/** Potential team allowed to control this instance.
	 * Only call this function if isPlayerControllable() returns true.
	 * @return The level team whose players are allowed to control this block or -1 for any.
	 */
	int32_t getControllerTeam() const noexcept { return m_nControllerTeam; }

	/** The level team of the player that currently controls this instance.
	 * @return The level team controlling the LevelBlock or -1 if not controlled.
	 */
	int32_t getTeam() const noexcept { return m_nTeam; }
	/** The teammate of the player that currently controls this instance.
	 * @return The level team mate controlling the LevelBlock or -1 if not controlled.
	 */
	int32_t getTeammate() const noexcept { return m_nTeammate; }
	/** The level player that currently controls this instance.
	 * @return The level player controlling the LevelBlock or -1 if not controlled.
	 */
	int32_t getPlayer() const noexcept { return m_nPlayer; }


	enum QUERY_ATTACK_TYPE
	{
		QUERY_ATTACK_TYPE_NOTHING = 0 /**< Nothing happens. */
		, QUERY_ATTACK_TYPE_FUSE_TO_ATTACKER = 1 /**< The aatacked fuses to the attacker. */
		, QUERY_ATTACK_TYPE_FREEZE_ATTACKED = 2 /**< The attacked freezes. */
		, QUERY_ATTACK_TYPE_ATTACK_FREES_POSITION = 3 /**< The attacked frees the attack position on the board. */
		, QUERY_ATTACK_TYPE_OTHER = 99 /**< Who knows. */
	};
	/** Whether another block can attack this block and the outcome.
	 * @param oAttacker The attacker block.
	 * @param nBoardX The board x position where the attack takes place.
	 * @param nBoardY The board y position where the attack takes place.
	 * @param oTile The attacker's tile. TODO not sure.
	 * @return The theoretical outcome of the attack.
	 */
	virtual QUERY_ATTACK_TYPE queryAttack(LevelBlock& oAttacker, int32_t nBoardX, int32_t nBoardY, const Tile& oTile) const noexcept = 0;
	/** Another block attacks this block.
	 * Returns true if the position was freed by either:
	 *      - removing or freezing the attacked LB's brick,
	 *      - removing or freezing the attacker LB's brick,
	 *      - fusing attacked to attacker LBs
	 * @param oAttacker The attacker block.
	 * @param nBoardX The board x position where the attack takes place.
	 * @param nBoardY The board y position where the attack takes place.
	 * @param oTile The attacker's tile. TODO not sure.
	 * @return Whether it succeeded.
	 */
	virtual bool attack(LevelBlock& oAttacker, int32_t nBoardX, int32_t nBoardY, const Tile& oTile) noexcept = 0;

	/** Returns coords of visible bricks (of current shape).
	 * @param oLevelBlock Level block.
	 * @return The coords of visible bricks.
	 */
	static Coords getCoords(const LevelBlock& oLevelBlock) noexcept;
protected:
	/** Constructor.
	 * @param bRemoveEmptyShapes Whether empty shapes are automatically removed.
	 */
	explicit LevelBlock(bool bRemoveEmptyShapes) noexcept;

	/** Initialization function.
	 * Use this function before LevelBlock instance is added to Level.
	 * @param oBlock The block.
	 * @param nShapeId The current shape.
	 * @param oPos The position on the level's board.
	 * @param bControllable Whether the level block is controllable by players.
	 * @param nControllerTeam The level team who's players are allowed to control the block or -1 if all teams.
	 */
	void blockInitialSet(const Block& oBlock, int32_t nShapeId, NPoint oPos, bool bControllable, int32_t nControllerTeam) noexcept;

	/** Whether the block is automatically scrolled by the level.
	 * @return Whether automatically scrolled.
	 */
	inline bool blockIsAutoScrolled() noexcept { return (m_eMgmtType >= MGMT_TYPE_AUTO_SCROLL); }
	/** Whether the cell owner is automatically set by the level.
	 * @return Whether owner automatically set.
	 */
	inline bool blockIsAutoOwner() noexcept { return (m_eMgmtType >= MGMT_TYPE_AUTO_OWNER); }

	//TODO Also provide XYEvent
	/** Input event handler.
	 * Function handleKeyActionInput for events of type KeyActionEvent.
	 * The base implementation does nothing.
	 * @param refEvent The received event. Cannot be null.
	 */
	virtual void handleInput(const shared_ptr<stmi::Event>& refEvent) noexcept;
	/** KeyActionevent handler.
	 * The base implementation does nothing.
	 * @param refEvent The key action event. Cannot be null.
	 */
	virtual void handleKeyActionInput(const shared_ptr<KeyActionEvent>& refEvent) noexcept;

	/** Function called each game tick.
	 * This function is called before LevelBlock::fall() if the block is falling on a
	 * specific game tick.
	 */
	virtual void handleTimer() noexcept = 0; //TODO Maybe provide empty implementation
	/** Function called when block supposed to fall.
	 * Called each Level::getFallEachTicks() game ticks. It is called after
	 * LevelBlock::handleTimer().
	 */
	virtual void fall() noexcept = 0; //TODO Maybe provide empty implementation

	/** The underlying block.
	 * @return The block.
	 */
	inline const Block& blockGet() const noexcept { return m_oBlock; }
	/** The current shape of the underlying block.
	 * @return The shape.
	 */
	inline int32_t blockGetShapeId() const noexcept { return m_nShapeId; }

	/** Move the block.
	 * @param nDeltaX The number of cells the block should move in the x direction.
	 * @param nDeltaY The number of cells the block should move in the y direction.
	 */
	void blockMove(int32_t nDeltaX, int32_t nDeltaY) noexcept;
	/** Set shape and move level block.
	 * @param nShapeId The (possibly new) current shape id.
	 * @param nDeltaX The number of cells the block should move in the x direction.
	 * @param nDeltaY The number of cells the block should move in the y direction.
	 */
	void blockMoveRotate(int32_t nShapeId, int32_t nDeltaX, int32_t nDeltaY) noexcept;

	/** Modifies the level block.
	 * @param nDeltaX The number of cells the block should move in the x direction.
	 * @param nDeltaY The number of cells the block should move in the y direction.
	 * @param aRemoveBrickId The bricks that should be removed. Value: nBrickId.
	 * @param aDestroyBrickId The bricks that should be destroyed. Value: nBrickId.
	 * @param aModifyPosBrick The position and visibility change of bricks. Value: tuple<nBrickId, nRelX, nRelY, bVisible>.
	 * @param aModifyTileBrick The brick tiles to be modified. Value: tuple<nBrickId, oTile>.
	 * @param aAddBrick The bricks to be added. Value: tuple<oTile, nRelX, nRelY, bVisible>.
	 */
	void blockModify(int32_t nDeltaX, int32_t nDeltaY
						, const std::vector<int32_t>& aRemoveBrickId, const std::vector<int32_t>& aDestroyBrickId
						, const std::vector< std::tuple<int32_t, int32_t, int32_t, bool> >& aModifyPosBrick
						, const std::vector< std::pair<int32_t, Tile> >& aModifyTileBrick
						, const std::vector< std::tuple<Tile, int32_t, int32_t, bool> >& aAddBrick) noexcept;
	//TODO detailed modification iface for Block (non virtual!), especially for changing other shapes than the current

	/** Called when block added to level.
	 */
	virtual void onAdded() noexcept;
	/** Called when block is added to level by unfreezing.
	 */
	virtual void onUnfreezed() noexcept;
	/** Called when block has fused with another (by now removed) block.
	 */
	virtual void onFusedWith(const LevelBlock& oLevelBlock) noexcept;
	/** Called when the block was scrolled.
	 * @param eDir The direction of the scroll.
	 */
	virtual void onScrolled(Direction::VALUE eDir) noexcept;
	/** Called when controlling player was changed by level.
	 */
	virtual void onPlayerChanged() noexcept;
	/** Called when controllability has changed.
	 */
	virtual void onControllabilityChanged() noexcept;
private:
	friend class Level;

	void callbackDeleteBrick(std::vector<int32_t>& aAddedBrickId, int32_t nBrickId) noexcept;
	void callbackMoveRotate(std::vector<int32_t>& aAddedBrickId, int32_t nShapeId, int32_t nDeltaX, int32_t nDeltaY) noexcept;
	void callbackModify(std::vector<int32_t>& aAddedBrickId, int32_t nDeltaX, int32_t nDeltaY
						, const std::vector<int32_t>& aRemoveBrickId, const std::vector<int32_t>& aDestroyBrickId
						, const std::vector< std::tuple<int32_t, int32_t, int32_t, bool> >& aModifyPosBrick
						, const std::vector< std::pair<int32_t, Tile> >& aModifyTileBrick
						, const std::vector< std::tuple<Tile, int32_t, int32_t, bool> >& aAddBrick
						) noexcept;
	void resetBrickPosCaches() noexcept;
	void resetCaches() noexcept;

	void resetPrivate(int32_t nTileAnis) noexcept;
	void resetPrivateJustTileAnis(int32_t nTileAnis) noexcept;
private:
	const int32_t m_nId;
	static int32_t s_nId;

	Level* m_p0Level;

	bool m_bNestedModificationLock;

	LevelBlock::MGMT_TYPE m_eMgmtType;

	Block m_oBlock;
	int32_t m_nShapeId;
	int32_t m_nPosX, m_nPosY;
	std::vector<int32_t> m_aCachedBrickId;
	// Index: DIRECTION, Value: Shapes
	//    Shapes:   Index: shape id, Size: highest cached shape id, Value: Shape
	//       Shape:   Size: totBricks, Value: Contact
	//          Contact:   tuple< nX, nY, nBrickd >
	std::vector< std::vector< Block::Contact > > m_aCachedShapeContacts[4];

	std::vector< std::list<LevelBlock*>::iterator > m_aItOrderedList; // Size: Level::m_nTotTeams, Value: iterator to Level::TeamData::m_oOrderedControllable
	// shortcuts
	int32_t m_nTeam;
	int32_t m_nTeammate;
	int32_t m_nPlayer;
	// save status on Level::blockAdd (so that Level::blockRemove works)
	bool m_bControllable; // Accessed by Level
	int32_t m_nControllerTeam; // Accessed by Level

	int32_t m_nLastTimerCall;
	int32_t m_nLastFallCall;

	struct BrickAni {
		TileAnimator* m_p0TileAnimator;
		int32_t m_nHash;
	};
	struct LevelBrick {
		std::vector<BrickAni> m_aBrickAni; // Index: Level::m_oNamed.tileAnis.getIndex(...)
		void clear(int32_t nTileAnis) noexcept;
	};
	std::unordered_map<int32_t, LevelBrick> m_oLevelBricks; // Key: BrickId

	uint32_t m_nScrolledUnique;
	bool m_bRemoveEmptyShapes;
};

} // namespace stmg

#endif	/* STMG_LEVEL_BLOCK_H */

