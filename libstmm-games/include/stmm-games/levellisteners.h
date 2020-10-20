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
 * File:   levellisteners.h
 */

#ifndef STMG_LEVEL_LISTENERS_H
#define STMG_LEVEL_LISTENERS_H

#include "util/direction.h"

#include <vector>
#include <memory>
#include <unordered_map>

#include <stdint.h>

namespace stmg { class Coords; }
namespace stmg { class LevelBlock; }
namespace stmg { class TileCoords; }
namespace stmg { class TileRect; }
namespace stmg { struct NRect; }

namespace stmg
{

using std::unique_ptr;
using std::shared_ptr;

////////////////////////////////////////////////////////////////////////////////
/** Board scroll listener.
 */
class BoardScrollListener
{
public:
	virtual ~BoardScrollListener() noexcept = default;
	/** Pre scroll callback.
	 * Called by Level::boardScroll().
	 * @param eDir The direction of the scroll.
	 * @param refTiles The tiles that will be placed on the freed cells. If null all empty tiles.
	 */
	virtual void boardPreScroll(Direction::VALUE eDir, const shared_ptr<TileRect>& refTiles) noexcept = 0;
	/** Post scroll callback.
	 * Called by Level::boardScroll().
	 * @param eDir The direction of the scroll.
	 */
	virtual void boardPostScroll(Direction::VALUE eDir) noexcept = 0;
};
/** The common board and block listener.
 * When block becomes board and  board becomes a block.
 */
class BoaBloListener
{
public:
	virtual ~BoaBloListener() noexcept = default;
	/** Pre freeze callback.
	 * Called by LevelBlock::freeze().
	 * @param oBlock The block that will freeze on the board.
	 */
	virtual void boabloPreFreeze(LevelBlock& oBlock) noexcept = 0;
	/** Post freeze callback.
	 * The block has been removed and the board modified.
	 * Called by LevelBlock::freeze().
	 * @param oCoords The coords of the modified cells.
	 */
	virtual void boabloPostFreeze(const Coords& oCoords) noexcept = 0;
	/** Pre unfreeze callback.
	 * Called by Level::boabloUnfreeze().
	 * @param oCoords The coords of the cells that will be used for the block. Is not empty.
	 */
	virtual void boabloPreUnfreeze(const Coords& oCoords) noexcept = 0;
	/** Post unfreeze callback.
	 * The block has been created and the board cells emptied.
	 * Called by Level::boabloUnfreeze().
	 * @param oBlock
	 */
	virtual void boabloPostUnfreeze(LevelBlock& oBlock) noexcept = 0;
};
/** Board listener.
 */
class BoardListener : public BoardScrollListener, public virtual BoaBloListener
{
public:
	/** Pre insert callback.
	 * Called by Level::boardInsert().
	 * @param eDir The direction.
	 * @param oArea The area. Is within board.
	 * @param refTiles The tiles to insert.
	 */
	virtual void boardPreInsert(Direction::VALUE eDir, NRect oArea, const shared_ptr<TileRect>& refTiles) noexcept = 0;
	/** Post insert callback.
	 * Called by Level::boardInsert().
	 * @param eDir The direction.
	 * @param oArea The modified area.
	 */
	virtual void boardPostInsert(Direction::VALUE eDir, NRect oArea) noexcept = 0;
	/** Pre destroy callback.
	 * Called by Level::boardDestroy().
	 * @param oCoords The destroyed tiles' position.
	 */
	virtual void boardPreDestroy(const Coords& oCoords) noexcept = 0;
	/** Post destroy callback.
	 * Called by Level::boardDestroy().
	 * @param oCoords The destroyed tiles (now empty) positions.
	 */
	virtual void boardPostDestroy(const Coords& oCoords) noexcept = 0;
	/** Pre modify callback.
	 * Called by Level::boardModify().
	 * @param oTileCoords The new tiles and their position.
	 */
	virtual void boardPreModify(const TileCoords& oTileCoords) noexcept = 0;
	/** Post modify callback.
	 * Called by Level::boardModify().
	 * @param oCoords The positions of the modified tiles.
	 */
	virtual void boardPostModify(const Coords& oCoords) noexcept = 0;
};
/** Block listener.
 */
class BlocksListener : public virtual BoaBloListener
{
public:
	/** Pre block added callback.
	 * Called by Level::blockAdd().
	 * @param oBlock The level block
	 */
	virtual void blockPreAdd(const LevelBlock& oBlock) noexcept = 0;
	/** Post block added callback.
	 * Called by Level::blockAdd().
	 * @param oBlock The level block
	 */
	virtual void blockPostAdd(LevelBlock& oBlock) noexcept = 0;
	/** Pre block removed callback.
	 * Called by LevelBlock::remove().
	 * @param oBlock The level block
	 */
	virtual void blockPreRemove(LevelBlock& oBlock) noexcept = 0;
	/** Post block removed callback.
	 * Called by LevelBlock::remove().
	 * @param oBlock The level block
	 */
	virtual void blockPostRemove(const LevelBlock& oBlock) noexcept = 0;
	/** Pre block destroyed callback.
	 * Called by LevelBlock::destroy().
	 * @param oBlock The level block
	 */
	virtual void blockPreDestroy(LevelBlock& oBlock) noexcept = 0;
	/** Post block destroyed callback.
	 * Called by LevelBlock::destroy().
	 * @param oBlock The level block
	 */
	virtual void blockPostDestroy(const LevelBlock& oBlock) noexcept = 0;
	/** Pre block fusion callback.
	 * Called by LevelBlock::fuseTo().
	 * @param oFusedToBlock The block to which oFusedBlock will be attached.
	 * @param oFusedBlock The block that will be removed.
	 */
	virtual void blockPreFuse(LevelBlock& oFusedToBlock, LevelBlock& oFusedBlock) noexcept = 0;
	/** Post block fusion callback.
	 * Called by LevelBlock::fuseTo().
	 *
	 * The TileAnimator references of oResBlock are reset.
	 * The TileAnimator references of oFusedBlock are reset.
	 * @param oResBlock is the result of the fusion (what was oFusedToBlock in blockPreFuse).
	 * @param oFusedBlock is what was added to oResBlock and removed from the level.
	 * @param oFusedToBrickIds is a map of the brickIds of the original oFusedToBlock to those in oResBlock.
	 * @param oFusedBrickIds is a map of the brickIds of oFusedBlock to those in oResBlock.
	 */
	virtual void blockPostFuse(LevelBlock& oResBlock, const LevelBlock& oFusedBlock
								, const std::unordered_map<int32_t, int32_t>& oFusedToBrickIds
								, const std::unordered_map<int32_t, int32_t>& oFusedBrickIds) noexcept = 0;

};
/** Block modification listener.
 */
class BlocksBricksIdListener : public BlocksListener
{
public:
	/** Pre block modification callback.
	 * Called by LevelBlock::blockModify().
	 *
	 * When the visibility of a brick is changed its id is in aModifyPosBrickId.
	 *
	 * Note that the implementation might pass all the bricks of a rotated block to aModifyPosBrickId
	 * without really checking whether the bricks have really changed position or
	 * their visibility. So don't rely on it.
	 * @param oBlock The block that will be modified.
	 * @param aDeleteBrickId The to be removed bricks.
	 * @param aModifyPosBrickId The bricks the position of which will be modified.
	 * @param aModifyTileBrickId The bricks the tile of which will be modified.
	 * @param bAddsBricks Whether bricks will be added.
	 */
	virtual void blockPreModify(LevelBlock& oBlock
								, const std::vector<int32_t>& aDeleteBrickId
								, const std::vector<int32_t>& aModifyPosBrickId
								, const std::vector<int32_t>& aModifyTileBrickId
								, bool bAddsBricks) noexcept = 0;
	/** Post block modification callback.
	 * Called by LevelBlock::blockModify().
	 *
	 * When the visibility of a brick is changed its id is in aModifyPosBrickId.
	 *
	 * Note that the implementation might pass all the bricks of a rotated block to aModifyPosBrickId
	 * without really checking whether the bricks have really changed position or
	 * their visibility. So don't rely on it.
	 * @param oBlock The modified block.
	 * @param aDeletedBrickId The removed bricks.
	 * @param aModifiedPosBrickId The bricks with modified position.
	 * @param aModifiedTileBrickId The bricks with modified tile.
	 * @param aAddedBrickId The bricks added.
	 */
	virtual void blockPostModify(LevelBlock& oBlock
								, const std::vector<int32_t>& aDeletedBrickId
								, const std::vector<int32_t>& aModifiedPosBrickId
								, const std::vector<int32_t>& aModifiedTileBrickId
								, const std::vector<int32_t>& aAddedBrickId) noexcept = 0;
};
	//class BlocksBricksDestroyListener : public BlocksListener
	//{
	//public:
	//	virtual void blockPreModify(LevelBlock& oBlock, const std::vector<int32_t>& aDestroyBrickId) noexcept = 0;
	//	virtual void blockPostModify(LevelBlock& oBlock) noexcept = 0;
	//};
/** Block's player change listener.
 */
class BlocksPlayerChangeListener //TODO implement adding and removing and maybe LevelBlock self informing
{
public:
	virtual ~BlocksPlayerChangeListener() noexcept = default;
	// When receiving this callback implementors should not take any action
	// that may lead to another player change! // TODO there probably should be a guard that asserts it
	/** Player change callback.
	 * The new player can be retrieved by LevelBlock::getPlayer().
	 * @param oBlock The block that changed controlling player.
	 * @param nOldPlayer The old level player.
	 */
	virtual void blockChangedPlayer(LevelBlock& oBlock, int32_t nOldPlayer) noexcept = 0;
};

//TODO BlocksPosListener X,Y and maybe Z changes, would be subclassed by view
// This interface is useful to AI Players (Events) that want to know when they get control of a block
// or for a positioner to know which block it should track on a player's subshow

} // namespace stmg

#endif	/* STMG_LEVEL_LISTENERS_H */

