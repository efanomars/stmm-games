/*
 * File:   block.h
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

#ifndef STMG_BLOCK_H
#define STMG_BLOCK_H

#include "tile.h"

#include "util/direction.h"
#include "util/basictypes.h"

#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <cstdint>
#include <tuple>

#include <stdint.h>

namespace stmg
{

class Block
{
public:
	/** Empty constructor.
	 * Creates a block with no shapes and no bricks.
	 */
	Block() noexcept;

	/** Constructor.
	 *     `aBrick[0 .. nTotBricks-1]: Tile`
	 *     `aShapeBrickPos[0 .. nTotShapes-1][0.. nTotBricks-1]: tuple(bVisible, nPosX, nPosY)`
	 *
	 * Empty Tiles are not allowed and are replaced with some unusual non-empty tile.
	 * Missing tiles (aBrick parameter) also are filled with some non-empty tile.
	 * Undefined positions within a shape make bricks invisible.
	 *
	 * For each shape a position can only have a visible brick,
	 * those in excess are set to not visible.
	 *
	 * The created bricks start from id 0 and increment.
	 * The created shapes start from id 0 and increment.
	 * @param nTotBricks The number of bricks. Must be &gt; 0.
	 * @param aBrick The bricks (tiles).
	 * @param nTotShapes The number of shapes. Must be &gt; 0.
	 * @param aShapeBrickPos The visibility and position of each brick and shape.
	 */
	Block(int32_t nTotBricks, const std::vector<Tile>& aBrick
			, int32_t nTotShapes
			, const std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > >& aShapeBrickPos) noexcept;

	/** Automatically rotated four shapes constructor.
	 * Block is generated automatically from 1 shape, adding 3 shapes rotated 90, 180, 270 degrees.
	 *
	 *     aBrick[0 .. nTotBricks-1]: Tile
	 *     aBrickPos[0 .. nTotBricks-1]: tuple(bVisible, nPosX, nPosY)
	 *
	 * Empty Tiles are not allowed and are replaced with some unusual non-empty tile.
	 * Missing tiles (aBrick parameter) also are filled with some non-empty tile.
	 * Undefined positions within a shape make bricks invisible.
	 *
	 * For each shape a position can only have a visible brick,
	 * those in excess are set to not visible.
	 *
	 * The created bricks start from id 0 and increment.
	 * The created shapes start from id 0 and increment.
	 * @param nTotBricks The number of bricks. Must be &gt; 0.
	 * @param aBrick The bricks (tiles).
	 * @param aBrickPos The visibility and position of each brick within the first shape.
	 * @param nWH The value that defines the center of rotation (nWH / 2, nWH / 2). Example: 3 -> (1.5, 1.5).
	 */
	Block(int32_t nTotBricks, const std::vector<Tile>& aBrick
			, const std::vector< std::tuple<bool, int32_t, int32_t> >& aBrickPos
			, int32_t nWH) noexcept;
	// The first shape of this instance becomes the concatenation of the 
	// visible bricks of the shape of two blocks.
	// Three further shapes are created by rotating the first shape counterclockwise
	// by 90, 180, 270 degrees around a "geometric" center.
	// oFirstBrickIds is the mapping from the first block's brick ids to the result's brick ids
	// oSecondBrickIds is the mapping from the second block's brick ids to the result's brick ids
	// The created bricks start from id 0 and increment.
	// The created shapes start from id 0 and increment.
	/** Automatically rotated four shapes constructor fusing two blocks.
	 * Only the visible bricks of the given shapes are used to build the result block.
	 *
	 * Add (nFirstPosXDelta, nFirstPosYDelta) to the position
	 * of the first block to get the position of the result block to make it coincide
	 * with the fused blocks.
	 *
	 * The result Block is generated automatically from the fused shapes, adding 3 
	 * additional shapes rotated counterclockwise 90, 180 and 270 degrees around
	 * the "geometric" center.
	 *
	 * The created bricks start from id 0 and increment.
	 * The created shapes start from id 0 and increment.
	 * @param nFirstShapeId The shape of the first block to be used.
	 * @param oFirst The first block.
	 * @param nSecondShapeId The shape of the second block to be used.
	 * @param oSecond The second block.
	 * @param nRelX The x position of the second block relative to the first.
	 * @param nRelY The y position of the second block relative to the first.
	 * @param nFirstPosXDelta The delta x position of the first block to the result block.
	 * @param nFirstPosYDelta The delta y position of the first block to the result block.
	 * @param oFirstBrickIds The mapping from the first block's brick ids to the result's brick ids.
	 * @param oSecondBrickIds The mapping from the second block's brick ids to the result's brick ids.
	 */
	Block(int32_t nFirstShapeId, const Block& oFirst, int32_t nSecondShapeId, const Block& oSecond
			, int32_t nRelX, int32_t nRelY
			, int32_t& nFirstPosXDelta, int32_t& nFirstPosYDelta
			, std::unordered_map<int32_t, int32_t>& oFirstBrickIds, std::unordered_map<int32_t, int32_t>& oSecondBrickIds) noexcept;
	/** Swap two blocks.
	 * @param oBlock The other block.
	 */
	void swap(Block& oBlock) noexcept;
	/** Copy constructor.
	 * @param oSource The source block.
	 */
	Block(const Block& oSource) noexcept;
	/** Assignment.
	 * @param oSource The source block.
	 * @return This block.
	 */
	Block& operator=(const Block& oSource) noexcept;

	/** Whether the block has any bricks.
	 * Note: the block with bricks but no shapes is considered non empty.
	 * @return Whether empty.
	 */
	inline bool isEmpty() const noexcept { return (m_nTotBricks == 0); }
	/** The total number of bricks in the block.
	 * @return The number of bricks. Can be 0 if block empty.
	 */
	inline int32_t totBricks() const noexcept { return m_nTotBricks; }
	/** The total number of shapes in the block.
	 * @return The number of shapes. Can be 0 if block empty.
	 */
	inline int32_t totShapes() const noexcept { return m_nTotShapes; }
	/** The brick ids of the block.
	 * @return The brick ids.
	 */
	inline const std::vector<int32_t>& brickIds() const noexcept { return m_aBrickId; }
	// Returns the shape ids (in order)
	/** The shape ids (in ascending order).
	 * @return The shape ids.
	 */
	inline std::vector<int32_t> shapeIds() const noexcept { return shapeIdsAsVector(); }
	/** Whether the brick id is valid.
	 * @param nBrickId The brick id.
	 * @return Whether brick exists in block.
	 */
	bool isBrickId(int32_t nBrickId) const noexcept;
	/** Whether the shape id is valid.
	 * @param nShapeId The shape id.
	 * @return Whether shape exists in block.
	 */
	bool isShapeId(int32_t nShapeId) const noexcept;

	/** The tile of the block's brick.
	 * @param nBrickId The brick id. Must be valid.
	 * @return The tile.
	 */
	const Tile& brick(int32_t nBrickId) const noexcept;
	/** The x position of a brick in a shape.
	 * @param nShapeId The shape id. Must be valid.
	 * @param nBrickId The brick id. Must be valid.
	 * @return The x position.
	 */
	int32_t shapeBrickPosX(int32_t nShapeId, int32_t nBrickId) const noexcept;
	/** The y position of a brick in a shape.
	 * @param nShapeId The shape id. Must be valid.
	 * @param nBrickId The brick id. Must be valid.
	 * @return The y position.
	 */
	int32_t shapeBrickPosY(int32_t nShapeId, int32_t nBrickId) const noexcept;
	/** The position of a brick in a shape.
	 * @param nShapeId The shape id. Must be valid.
	 * @param nBrickId The brick id. Must be valid.
	 * @return The position.
	 */
	NPoint shapeBrickPos(int32_t nShapeId, int32_t nBrickId) const noexcept;
	/** Tells whether a brick is visible in the given shape.
	 * @param nShapeId The shape id. Must be valid.
	 * @param nBrickId The brick id. Must be valid.
	 * @return Whether visible.
	 */
	bool shapeBrickVisible(int32_t nShapeId, int32_t nBrickId) const noexcept;
	/** Total number of visible bricks of a shape.
	 * @param nShapeId The shape id. Must be valid.
	 * @return The number of visible bricks.
	 */
	int32_t shapeTotVisibleBricks(int32_t nShapeId) const noexcept;
	/** Tells whether at the given position in the shape there is a visible brick.
	 * @param nShapeId The shape id. Must be valid.
	 * @param nX The x position.
	 * @param nY The y position.
	 * @return The visible brick id at the position or -q if nothing there.
	 */
	int32_t shapeBrickVisiblePosition(int32_t nShapeId, int32_t nX, int32_t nY) const noexcept;
	/** Sets the visibility of a brick in a shape.
	 * @param nShapeId The shape id. Must be valid.
	 * @param nBrickId The brick id. Must be valid.
	 * @param bVisible The visibility.
	 * @return Returns false if couldn't make visible because position already occupied by another brick.
	 */
	bool shapeBrickSetVisible(int32_t nShapeId, int32_t nBrickId, bool bVisible) noexcept;
	/** Sets the position and visibility of a brick in a shape.
	 * The position is set even if couldn't set the visibility.
	 * @param nShapeId The shape id. Must be valid.
	 * @param nBrickId The brick id. Must be valid.
	 * @param nX The x position.
	 * @param nY The y position.
	 * @param bVisible The visibility.
	 * @return Returns false if couldn't make visible because position already occupied by another brick.
	 */
	bool shapeBrickSetPosVisible(int32_t nShapeId, int32_t nBrickId, int32_t nX, int32_t nY, bool bVisible) noexcept;
	/**  Modifies the given bricks: tuple<nBrickId, nX, nY, bVisible>
	 * The positions are set even if couldn't set the visibility of one or more bricks.
	 * @param nShapeId The shape id. Must be valid.
	 * @param aBrickPosition The modified bricks as tuple<nBrickId, nX, nY, bVisible>.
	 * @return Returns false if any of the modified bricks couldn't be turned visible.
	 */
	bool shapeModifyBricks(int32_t nShapeId, const std::vector< std::tuple<int32_t, int32_t, int32_t, bool> >& aBrickPosition) noexcept;

	/** A contact.
	 * The position adjacent a certain brick that is not occupied by another brick.
	 */
	struct Contact
	{
		int32_t m_nRelX; /**< The x coordinate of the contact relative to the block's origin (not the brick's position). */
		int32_t m_nRelY; /**< The y coordinate of the contact relative to the block's origin (not the brick's position). */
		int32_t m_nBrickId;  /**< The brick the contact refers to. */
		constexpr bool operator==(const Contact& oOther) const noexcept
		{
			return (oOther.m_nRelX == m_nRelX) && (oOther.m_nRelY == m_nRelY) && (oOther.m_nBrickId == m_nBrickId);
		}
		constexpr bool operator<(const Block::Contact& oOther) const noexcept
		{
			return std::make_tuple(m_nRelX, m_nRelY, m_nBrickId) < std::make_tuple(oOther.m_nRelX, oOther.m_nRelY, oOther.m_nBrickId);
		}
	};
	/** The contacts for a certain shape and direction.
	 * @param nShapeId The shape id. Must be valid.
	 * @param eDir The direction.
	 * @return  The vector containing all the contacts.
	 */
	const std::vector< Contact > shapeContacts(int32_t nShapeId, Direction::VALUE eDir) const noexcept;

	/** Adds a brick at the given position in each shape.
	 * If bVisible is true the added brick might still not be visible in a shape because
	 * its position is already occupied by another visible brick.
	 *
	 * If the block has no shapes, one is created.
	 *
	 * If the tile is empty, some non empty tile is used.
	 * @param oTile The tile of the brick.
	 * @param nX The x coord of the brick.
	 * @param nY The y coord of the brick.
	 * @param bVisible Whether the brick is visible.
	 * @return The brick id.
	 */
	int32_t brickAdd(const Tile& oTile, int32_t nX, int32_t nY, bool bVisible) noexcept;
	/** Modify the tile of a brick.
	 * If the tile is empty, some non empty tile is used.
	 * @param nBrickId The brick id.
	 * @param oTile The new tile.
	 */
	void brickModify(int32_t nBrickId, const Tile& oTile) noexcept;
	/** Removes a brick.
	 * @param nBrickId The brick id.
	 */
	void brickRemove(int32_t nBrickId) noexcept;

	/** Inserts a shape into the block.
	 * If nBeforeShapeId is -1 added as last shape.
	 *
	 * All bricks will be set at position (0,0) and be not visible.
	 * @param nBeforeShapeId The shape id before which the shape must be inserted or -1.
	 * @return The newly inserted shape id.
	 */
	int32_t shapeInsert(int32_t nBeforeShapeId) noexcept;
	/** Inserts a copy of another shape into the block.
	 * If nBeforeShapeId is -1 added as last shape.
	 * @param nBeforeShapeId The shape id before which the shape must be inserted or -1.
	 * @param nCopyOfShapeId The shape the inserted shouuld be copied from.
	 * @return The newly inserted shape id.
	 */
	int32_t shapeInsert(int32_t nBeforeShapeId, int32_t nCopyOfShapeId) noexcept;
	/** Removes a shape.
	 * @param nShapeId The shape to remove.
	 */
	void shapeRemove(int32_t nShapeId) noexcept;
	/** Remove all shapes that have no visible bricks.
	 * @return The number of removed shapes.
	 */
	int32_t shapeRemoveAllInvisible() noexcept;

	/** The first shape id.
	 * Is (sort of) same as shapeIds()[0], but more efficient.
	 * @return The id of the first shape or -1 if block has no shapes.
	 */
	int32_t shapeFirst() const noexcept;
	/** The last shape id.
	 * @return The id of the last shape or -1 if block has no shapes.
	 */
	int32_t shapeLast() const noexcept;
	/** The shape id of the shape preceding another.
	 * @param nShapeId The shape id the preceding of which is returned.
	 * @return The preceding shape id or -1 if nShapeId is the first.
	 */
	int32_t shapePrec(int32_t nShapeId) const noexcept;
	/** The shape id of the shape following another.
	 * @param nShapeId The shape id the following of which is returned.
	 * @return The following shape id or -1 if nShapeId is the last.
	 */
	int32_t shapeNext(int32_t nShapeId) const noexcept;

	/** The minimum x position of the visible bricks in a shape.
	 * If the shape has no visible bricks the return value is std::numeric_limits<int32_t>::max().
	 * The value is cached (not calculated for each call).
	 * @param nShapeId The shape id.
	 * @return The minimum x position (relative to block's origin).
	 */
	int32_t shapeMinX(int32_t nShapeId) const noexcept;
	/** The minimum y position of the visible bricks in a shape.
	 * If the shape has no visible bricks the return value is std::numeric_limits<int32_t>::max().
	 * The value is cached (not calculated for each call).
	 * @param nShapeId The shape id.
	 * @return The minimum y position (relative to block's origin).
	 */
	int32_t shapeMinY(int32_t nShapeId) const noexcept;
	/** The minimum position of the visible bricks in a shape.
	 * If the shape has no visible bricks the return value is {std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max()}.
	 * The value is cached (not calculated for each call).
	 * @param nShapeId The shape id.
	 * @return The minimum position (relative to block's origin).
	 */
	NPoint shapeMinPos(int32_t nShapeId) const noexcept;
	/** The maximum x position of the visible bricks in a shape.
	 * If the shape has no visible bricks the return value is std::numeric_limits<int32_t>::lowest().
	 * The value is cached (not calculated for each call).
	 * @param nShapeId The shape id.
	 * @return The maximum x position (relative to block's origin).
	 */
	int32_t shapeMaxX(int32_t nShapeId) const noexcept;
	/** The maximum y position of the visible bricks in a shape.
	 * If the shape has no visible bricks the return value is std::numeric_limits<int32_t>::lowest().
	 * The value is cached (not calculated for each call).
	 * @param nShapeId The shape id.
	 * @return The maximum y position (relative to block's origin).
	 */
	int32_t shapeMaxY(int32_t nShapeId) const noexcept;
	/** The maximum position of the visible bricks in a shape.
	 * If the shape has no visible bricks the return value is {std::numeric_limits<int32_t>::lowest(), std::numeric_limits<int32_t>::lowest()}.
	 * The value is cached (not calculated for each call).
	 * @param nShapeId The shape id.
	 * @return The maximum position (relative to block's origin).
	 */
	NPoint shapeMaxPos(int32_t nShapeId) const noexcept;

	/** The maximum width of all shapes.
	 * Returns 0 if no visible bricks or no shapes.
	 * @return The maximum width.
	 */
	int32_t maxWidth() const noexcept;
	/** The maximum height of all shapes.
	 * Returns 0 if no visible bricks or no shapes.
	 * @return The maximum height.
	 */
	int32_t maxHeight() const noexcept;
	/** The width of a given shape.
	 * @param nShapeId The shape id.
	 * @return The shape's width or 0 if no visible bricks.
	 */
	int32_t shapeWidth(int32_t nShapeId) const noexcept;
	/** The height of a given shape.
	 * @param nShapeId The shape id.
	 * @return The shape's height or 0 if no visible bricks.
	 */
	int32_t shapeHeight(int32_t nShapeId) const noexcept;
	/** The size of a given shape.
	 * @param nShapeId The shape id.
	 * @return The shape's size or (0, 0) if no visible bricks.
	 */
	NSize shapeSize(int32_t nShapeId) const noexcept;
	/** The widest shape of the block.
	 * @return The widest shape id or -1 if block has no visible bricks or has no shapes.
	 */
	int32_t widestShapeId() const noexcept;
	/** The highest shape of the block.
	 * @return The highest shape id or -1 if block has no visible bricks or has no shapes.
	 */
	int32_t highestShapeId() const noexcept;

#ifndef NDEBUG
	void dump() const noexcept;
	void dump(int32_t nIndent) const noexcept;
#endif //NDEBUG
private:
	const std::vector<int32_t> shapeIdsAsVector() const noexcept;
	void shapeRemove(int32_t nShapeId, std::list<int32_t>::iterator& itShapeId) noexcept;

	void assignBricks(const std::vector<Tile>& aBrick) noexcept;
	struct Shape;
	void assignShape(Shape& oShape, const std::vector< std::tuple<bool, int32_t, int32_t> >& aBrickPos) noexcept;

	void calcMinMaxVisible(int32_t nShape) noexcept;
	void calcContacts(int32_t nShapeId) noexcept;
	void calcContactsCardinal(int32_t nShapeId, int32_t nDx, int32_t nDy) noexcept;
	int32_t calcWidestShape(int32_t& nMaxW) const noexcept;
	int32_t calcHighestShape(int32_t& nMaxH) const noexcept;

	void calcExtraInfo(int32_t nShapeId) noexcept;

	void init3Shapes(int32_t nWH) noexcept;

#ifndef NDEBUG
	bool isIteratorOfListShapeIds(const std::list<int32_t>::const_iterator& itShapeId) const noexcept;
	bool isIteratorOfListShapeIds(const std::list<int32_t>::iterator& itShapeId) const noexcept;
#endif
private:
	int32_t m_nTotBricks;
	int32_t m_nTotShapes;

	std::vector<int32_t> m_aBrickId; // Size: m_nTotBricks, Value: BrickId
	std::vector<int32_t> m_aBrickIdx; // Size: Highest BrickId + 1, Index: BrickId, Value: Index in m_aBrickId or -1 if free
	std::vector<int32_t> m_aFreeBrickIds; // Size: m_aBrickIdx.size() - m_nTotBricks
	std::vector<Tile> m_aTile; // Size: m_aBrickIdx.size(), Index: BrickId

	struct Shape
	{
		std::vector<int32_t> m_aBrickPosX; // Size: m_aBrickIdx.size(), Index: BrickId
		std::vector<int32_t> m_aBrickPosY; // Size: m_aBrickIdx.size(), Index: BrickId
		std::vector<bool> m_aBrickVisible; // Size: m_aBrickIdx.size(), Index: BrickId
		// This helps to detect visible bricks defined on same position (which is not permitted)
		std::unordered_map<int64_t, int32_t> m_oPosOfVisibleBrick; // Key: Coords::packXY(nPosX, nPosY), Value: (visible) BrickId
		int32_t m_nMinVisiblePosX; // min(visible m_aBrickPosX)
		int32_t m_nMinVisiblePosY; // min(visible m_aBrickPosY)
		int32_t m_nMaxVisiblePosX; // max(visible m_aBrickPosX)
		int32_t m_nMaxVisiblePosY; // max(visible m_aBrickPosY)
		// ex.:   u                       where X: tile   u: up
		//        X    X   lX    Xr                       d: down
		//        Xu   X   lX    Xr                       l: left
		//        XX   XX  lXX   XXr                      r: right
		//             dd
		std::unordered_map<int64_t, int32_t> m_aDirContactPositions[4]; // Index: Direction::VALUE, Value: contact position of visible bricks (Key: Coords::packXY(nPosX, nPosY), Value: BrickId)
	};
	std::list<int32_t> m_oShapeIds; // Size: m_nTotShapes, Value: ShapeId, Keep order after shape removals
	std::vector< std::list<int32_t>::iterator > m_aShapeIdIterator; // Size: m_aIsShapeId.size(), Index: ShapeId, Value: iterator of m_oShapeIds
	std::vector<bool> m_aIsShapeId; // Size: Highest ShapeId + 1, Index: ShapeId, Value: validity of ShapeId 
	std::vector<int32_t> m_aFreeShapeId; // Size: m_aIsShapeId.size() - m_nTotShapes
	std::vector<Shape> m_aShape; // Size: m_aIsShapeId.size(), Index: ShapeId

	int32_t m_nMaxWidth; // = max(for all Shapes: Shape.m_nMaxVisiblePosX - Shape.m_nMinVisiblePosX + 1)
	int32_t m_nMaxHeight; // = max(for all Shapes: Shape.m_nMaxVisiblePosY - Shape.m_nMinVisiblePosY + 1)
	int32_t m_nWidestShapeId;
	int32_t m_nHighestShapeId;

	static const Tile& getNonEmptyTile() noexcept
	{
		static Tile oTile;
		oTile.getTileChar().setChar(TileChar::CHAR_UCS4_MAX);
		return oTile;
	}
	static const Tile s_oNonEmptyTile;
};

} // namespace stmg

#endif	/* STMG_BLOCK_H */

