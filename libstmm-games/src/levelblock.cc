/*
 * File:   levelblock.cc
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

#include "levelblock.h"

#include "level.h"
#include "tileanimator.h"
#include "block.h"
#include "tile.h"
#include "util/basictypes.h"
#include "util/coords.h"
#include "util/direction.h"

#include <cassert>
#include <limits>
#include <algorithm>
//#include <iostream>
#include <cstdint>
#include <list>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <stdint.h>

namespace stmg { class KeyActionEvent; }
namespace stmi { class Event; }


namespace stmg
{

int32_t LevelBlock::s_nId = 0;

LevelBlock::LevelBlock(bool bRemoveEmptyShapes) noexcept
: m_nId(++s_nId)
, m_p0Level(nullptr)
, m_bNestedModificationLock(false)
, m_eMgmtType(MGMT_TYPE_INVALID)
, m_nShapeId(-1)
, m_nPosX(0)
, m_nPosY(0)
, m_nTeam(-1)
, m_nTeammate(-1)
, m_nPlayer(-1)
, m_bControllable(false)
, m_nControllerTeam(-1)
, m_nLastTimerCall(-1)
, m_nLastFallCall(-1)
, m_nScrolledUnique(0)
, m_bRemoveEmptyShapes(bRemoveEmptyShapes)
{
	// ensure there is at least one shape defined
	m_oBlock.shapeInsert(-1);
	m_nShapeId = m_oBlock.shapeFirst();
}
void LevelBlock::handleInput(const shared_ptr<stmi::Event>& /*refEvent*/) noexcept
{
}
void LevelBlock::handleKeyActionInput(const shared_ptr<KeyActionEvent>& /*refEvent*/) noexcept
{
}
const std::vector<int32_t>& LevelBlock::blockBrickIds() const noexcept
{
	if (m_aCachedBrickId.empty()) {
		const_cast<LevelBlock*>(this)->m_aCachedBrickId = m_oBlock.brickIds();
	}
	return m_aCachedBrickId;
}
const std::vector< Block::Contact >& LevelBlock::blockContacts(Direction::VALUE eDir) noexcept
{
	assert((static_cast<int32_t>(eDir) >= 0) && (static_cast<int32_t>(eDir) < 4));
	if (m_aCachedShapeContacts[eDir].empty()) {
		m_aCachedShapeContacts[eDir].resize(m_nShapeId + 1);
	}
	if (m_aCachedShapeContacts[eDir][m_nShapeId].empty()) {
		m_aCachedShapeContacts[eDir][m_nShapeId] = m_oBlock.shapeContacts(m_nShapeId, eDir);
	}
	return m_aCachedShapeContacts[eDir][m_nShapeId];
}
void LevelBlock::resetBrickPosCaches() noexcept
{
	for (int32_t nIdx = 0; nIdx < 4; ++nIdx) {
		m_aCachedShapeContacts[nIdx].resize(0); // This shouldn't free the memory
	}
}
void LevelBlock::resetCaches() noexcept
{
	m_aCachedBrickId.resize(0); // This shouldn't free the memory
	resetBrickPosCaches();
}
bool LevelBlock::remove() noexcept
{
	assert(m_p0Level != nullptr);
	m_p0Level->blockRemove(this);
	return true;
}
bool LevelBlock::destroy() noexcept
{
	assert(m_p0Level != nullptr);
	m_p0Level->blockDestroy(this);
	return true;
}
bool LevelBlock::freeze() noexcept
{
	assert(m_p0Level != nullptr);
	m_p0Level->blockFreeze(this);
	return true;
}
bool LevelBlock::fuseTo(LevelBlock& oLevelBlock) noexcept
{
	if (!oLevelBlock.canFuseWith(*this)) {
		return false;
	}
	assert(m_p0Level != nullptr);
	m_p0Level->blockFuse(&oLevelBlock, this);
	return true;
}
bool LevelBlock::removeBrick(int32_t nBrickId) noexcept
{
	m_p0Level->blockModify(this, {nBrickId}, {}, {}, {}, false
							, &LevelBlock::callbackDeleteBrick, nBrickId);
	return true;
}
bool LevelBlock::destroyBrick(int32_t nBrickId) noexcept
{
	m_p0Level->blockModify(this, {}, {nBrickId}, {}, {}, false
							, &LevelBlock::callbackDeleteBrick, nBrickId);
	return true;
}
void LevelBlock::callbackDeleteBrick(std::vector<int32_t>& /*aAddedBrickId*/, int32_t nBrickId) noexcept
{
	const bool bLevel = (m_p0Level != nullptr);
	const bool bAutoOwner = bLevel && ((m_eMgmtType == LevelBlock::MGMT_TYPE_AUTO_OWNER) || (m_eMgmtType == LevelBlock::MGMT_TYPE_AUTO_STRICT_OWNER));

	if (bAutoOwner) {
		m_p0Level->boabloOwnerBlockClear(*this);
	}

	m_oBlock.brickRemove(nBrickId);
	resetCaches();
	if (m_bRemoveEmptyShapes) {
		m_oBlock.shapeRemoveAllInvisible();
	}

	if (bAutoOwner) {
		m_p0Level->boabloOwnerBlockSet(*this);
	}
}
void LevelBlock::blockInitialSet(const Block& oBlock, int32_t nShapeId, NPoint oPos, bool bControllable, int32_t nControllerTeam) noexcept
{
	if (m_p0Level != nullptr) {
		// not allowed
		assert(false);
		return;
	}
	m_oBlock = oBlock;
	m_nPosX = oPos.m_nX;
	m_nPosY = oPos.m_nY;
	if (m_oBlock.totShapes() == 0) {
		m_oBlock.shapeInsert(-1);
	}
	if (nShapeId < 0) {
		m_nShapeId = m_oBlock.shapeFirst();
	} else {
		assert(m_oBlock.isShapeId(nShapeId));
		m_nShapeId = nShapeId;
	}
	m_bControllable = bControllable;
	m_nControllerTeam = nControllerTeam;
	resetCaches();
}
void LevelBlock::blockMove(int32_t nDeltaX, int32_t nDeltaY) noexcept
{
	blockMoveRotate(m_nShapeId, nDeltaX, nDeltaY);
}
void LevelBlock::blockMoveRotate(int32_t nShapeId, int32_t nDeltaX, int32_t nDeltaY) noexcept
{
	static const std::vector<int32_t> s_aEmptyVector;

	assert(m_oBlock.isShapeId(nShapeId));
	if (m_p0Level == nullptr) {
		std::vector<int32_t> aAddedBrickId;
		callbackMoveRotate(aAddedBrickId, nShapeId, nDeltaX, nDeltaY);
		return;
	}
	const bool bShapeChanged = (m_nShapeId != nShapeId);
	if (bShapeChanged && m_aCachedBrickId.empty()) {
		m_aCachedBrickId = m_oBlock.brickIds();
	}
	m_p0Level->blockModify(this, {}, {}, (bShapeChanged ? m_aCachedBrickId : s_aEmptyVector), {}, false
							, &LevelBlock::callbackMoveRotate, nShapeId, nDeltaX, nDeltaY);
}
void LevelBlock::callbackMoveRotate(std::vector<int32_t>& /*aAddedBrickId*/, int32_t nShapeId, int32_t nDeltaX, int32_t nDeltaY) noexcept
{
	const bool bLevel = (m_p0Level != nullptr);
	const bool bAutoOwner = bLevel && ((m_eMgmtType == LevelBlock::MGMT_TYPE_AUTO_OWNER) || (m_eMgmtType == LevelBlock::MGMT_TYPE_AUTO_STRICT_OWNER));

	if (bAutoOwner) {
		m_p0Level->boabloOwnerBlockClear(*this);
	}
	const bool bShapeChanged = (m_nShapeId != nShapeId);

	m_nShapeId = nShapeId;
	m_nPosX += nDeltaX;
	m_nPosY += nDeltaY;

	if (bAutoOwner) {
		m_p0Level->boabloOwnerBlockSet(*this);
	}

	if (bShapeChanged) {
		resetBrickPosCaches();
	}
}
void LevelBlock::blockModify(int32_t nDeltaX, int32_t nDeltaY
							, const std::vector<int32_t>& aRemoveBrickId, const std::vector<int32_t>& aDestroyBrickId
							, const std::vector< std::tuple<int32_t, int32_t, int32_t, bool> >& aModifyPosBrick
							, const std::vector< std::pair<int32_t, Tile> >& aModifyTileBrick
							, const std::vector< std::tuple<Tile, int32_t, int32_t, bool> >& aAddBrick) noexcept
{
	if (m_p0Level == nullptr) {
		std::vector<int32_t> aAddedBrickId;
		callbackModify(aAddedBrickId, nDeltaX, nDeltaY, aRemoveBrickId, aDestroyBrickId, aModifyPosBrick, aModifyTileBrick, aAddBrick);
		return;
	}
	std::vector<int32_t> aModifyPosBrickId;
	for (auto& oPosBrick : aModifyPosBrick) {
		const int32_t nBrickId = std::get<0>(oPosBrick);
		assert(m_oBlock.isBrickId(nBrickId));
		aModifyPosBrickId.push_back(nBrickId);
	}
	std::vector<int32_t> aModifyTileBrickId;
	for (auto& oTileBrick : aModifyTileBrick) {
		const int32_t nBrickId = std::get<0>(oTileBrick);
		assert(m_oBlock.isBrickId(nBrickId));
		aModifyTileBrickId.push_back(nBrickId);
	}
	m_p0Level->blockModify(this, aRemoveBrickId, aDestroyBrickId, aModifyPosBrickId, aModifyTileBrickId, !aAddBrick.empty()
							, &LevelBlock::callbackModify
							, nDeltaX, nDeltaY, aRemoveBrickId, aDestroyBrickId, aModifyPosBrick, aModifyTileBrick, aAddBrick);
}
void LevelBlock::callbackModify(std::vector<int32_t>& aAddedBrickId, int32_t nDeltaX, int32_t nDeltaY
								, const std::vector<int32_t>& aRemoveBrickId, const std::vector<int32_t>& aDestroyBrickId
								, const std::vector< std::tuple<int32_t, int32_t, int32_t, bool> >& aModifyPosBrick
								, const std::vector< std::pair<int32_t, Tile> >& aModifyTileBrick
								, const std::vector< std::tuple<Tile, int32_t, int32_t, bool> >& aAddBrick) noexcept
{
	const bool bLevel = (m_p0Level != nullptr);
	const bool bAutoOwner = bLevel && ((m_eMgmtType == LevelBlock::MGMT_TYPE_AUTO_OWNER) || (m_eMgmtType == LevelBlock::MGMT_TYPE_AUTO_STRICT_OWNER));
	const bool bAutoStrictOwner = bLevel && (m_eMgmtType == LevelBlock::MGMT_TYPE_AUTO_STRICT_OWNER);

	for (auto& nBrickId : aRemoveBrickId) {
		assert(m_oBlock.isBrickId(nBrickId));
		m_oBlock.brickRemove(nBrickId);
		if (bAutoOwner) {
			const bool bVisible = m_oBlock.shapeBrickVisible(m_nShapeId, nBrickId);
			if (bVisible) {
				const int32_t nX = m_nPosX + m_oBlock.shapeBrickPosX(m_nShapeId, nBrickId);
				const int32_t nY = m_nPosY + m_oBlock.shapeBrickPosY(m_nShapeId, nBrickId);
				assert(m_p0Level->boardGetOwner(nX, nY) == this);
				m_p0Level->boardSetOwner(nX, nY, nullptr);
			}
		}
	}
	for (auto& nBrickId : aDestroyBrickId) {
		assert(m_oBlock.isBrickId(nBrickId));
		m_oBlock.brickRemove(nBrickId);
		if (bAutoOwner) {
			const bool bVisible = m_oBlock.shapeBrickVisible(m_nShapeId, nBrickId);
			if (bVisible) {
				const int32_t nX = m_nPosX + m_oBlock.shapeBrickPosX(m_nShapeId, nBrickId);
				const int32_t nY = m_nPosY + m_oBlock.shapeBrickPosY(m_nShapeId, nBrickId);
				assert(m_p0Level->boardGetOwner(nX, nY) == this);
				m_p0Level->boardSetOwner(nX, nY, nullptr);
			}
		}
	}
	for (auto& oTileBrick : aModifyTileBrick) {
		const int32_t nBrickId = oTileBrick.first;
		const Tile& oTile = oTileBrick.second;
		assert(!oTile.isEmpty());
		m_oBlock.brickModify(nBrickId, oTile);
	}

	if (bAutoOwner) {
		// first remove old positions
		for (auto& oPosBrick : aModifyPosBrick) {
			const int32_t nBrickId = std::get<0>(oPosBrick);
			assert(m_oBlock.isBrickId(nBrickId));
			// old
			const int32_t nX = m_nPosX + m_oBlock.shapeBrickPosX(m_nShapeId, nBrickId);
			const int32_t nY = m_nPosY + m_oBlock.shapeBrickPosY(m_nShapeId, nBrickId);
			const bool bVisible = m_oBlock.shapeBrickVisible(m_nShapeId, nBrickId);
			if (bVisible) {
				assert(m_p0Level->boardGetOwner(nX, nY) == this);
				if (bAutoStrictOwner) {
					assert(m_p0Level->boardGetTile(nX, nY).isEmpty());
				}
				m_p0Level->boardSetOwner(nX, nY, nullptr);
			}
		}
	}

	m_nPosX += nDeltaX;
	m_nPosY += nDeltaY;

	if (bAutoOwner) {
		// first set new x,y with visible false
		for (auto& oPosBrick : aModifyPosBrick) {
			const int32_t nBrickId = std::get<0>(oPosBrick);
			const int32_t nNewRelX = std::get<1>(oPosBrick);
			const int32_t nNewRelY = std::get<2>(oPosBrick);
			const bool bNewVisible = std::get<3>(oPosBrick);
			const int32_t nNewX = m_nPosX + nNewRelX;
			const int32_t nNewY = m_nPosY + nNewRelY;
			assert(m_oBlock.isBrickId(nBrickId));
			// set new
			m_oBlock.shapeBrickSetPosVisible(m_nShapeId, nBrickId, nNewRelX, nNewRelY, false);
			if (bNewVisible) {
				assert(m_p0Level->boardGetOwner(nNewX, nNewY) == nullptr);
				if (bAutoStrictOwner) {
					assert(m_p0Level->boardGetTile(nNewX, nNewY).isEmpty());
				}
				m_p0Level->boardSetOwner(nNewX, nNewY, this);
			}
		}
		// set correct visible
		for (auto& oPosBrick : aModifyPosBrick) {
			const int32_t nBrickId = std::get<0>(oPosBrick);
			const bool bNewVisible = std::get<3>(oPosBrick);
			// set new
			#ifndef NDEBUG
			const bool bOk =
			#endif //NDEBUG
			m_oBlock.shapeBrickSetVisible(m_nShapeId, nBrickId, bNewVisible);
			assert(bOk);
		}
	} else {
		#ifndef NDEBUG
		const bool bAllOk =
		#endif //NDEBUG
		m_oBlock.shapeModifyBricks(m_nShapeId, aModifyPosBrick);
		assert(bAllOk);
	}
	for (auto& oAddBrick : aAddBrick) {
		const Tile& oTile = std::get<0>(oAddBrick);
		const int32_t nRelX = std::get<1>(oAddBrick);
		const int32_t nRelY = std::get<2>(oAddBrick);
		const bool bVisible = std::get<3>(oAddBrick);
		const int32_t nBrickId = m_oBlock.brickAdd(oTile, nRelX, nRelY, bVisible);
		aAddedBrickId.push_back(nBrickId);
		if (bVisible && bAutoOwner) {
			const int32_t nX = m_nPosX + nRelX;
			const int32_t nY = m_nPosY + nRelY;
			// In the current shape no brick should already be visible at the position
			assert(m_p0Level->boardGetOwner(nX, nY) == nullptr);
			if (bAutoStrictOwner) {
				assert(m_p0Level->boardGetTile(nX, nY).isEmpty());
			}
			m_p0Level->boardSetOwner(nX, nY, this);
		}
	}
	resetCaches();
	if (bLevel) {
		resetPrivateJustTileAnis(m_p0Level->getNrTileAniAttrs());
	}
}
void LevelBlock::onAdded() noexcept
{
}
void LevelBlock::onUnfreezed() noexcept
{
}
void LevelBlock::onFusedWith(const LevelBlock& /*oLevelBlock*/) noexcept
{
}
void LevelBlock::onScrolled(Direction::VALUE /*eDir*/) noexcept
{
}
void LevelBlock::onPlayerChanged() noexcept
{
}
void LevelBlock::onControllabilityChanged() noexcept
{
}

void LevelBlock::resetPrivateJustTileAnis(int32_t nTileAnis) noexcept
{
	m_oLevelBricks.clear();
	const Block& oBlock = m_oBlock;
	const std::vector<int32_t> aBrickId = oBlock.brickIds();
	for (auto& nBrickId : aBrickId) {
		auto oRetPair = m_oLevelBricks.emplace(std::make_pair(nBrickId, LevelBrick()));
		auto itMap = oRetPair.first;
		itMap->second.clear(nTileAnis);
	}
}
void LevelBlock::resetPrivate(int32_t nTileAnis) noexcept
{
	m_nTeam = -1;
	m_nTeammate = -1;
	m_nPlayer = -1;

	m_nLastTimerCall = std::numeric_limits<int32_t>::lowest();
	m_nLastFallCall = std::numeric_limits<int32_t>::lowest();
	resetPrivateJustTileAnis(nTileAnis);
}
const std::vector<int32_t>& LevelBlock::blockVTBrickIds(int32_t /*nViewTick*/, int32_t /*nTotViewTicks*/) const noexcept
{
	return blockBrickIds();
}
FPoint LevelBlock::blockVTPos(int32_t /*nViewTick*/, int32_t /*nTotViewTicks*/) const noexcept
{
	const NPoint oPos = blockPos();
	return FPoint{static_cast<double>(oPos.m_nX), static_cast<double>(oPos.m_nY)};
}
const Tile& LevelBlock::blockVTBrickTile(int32_t /*nViewTick*/, int32_t /*nTotViewTicks*/, int32_t nBrickId) const noexcept
{
	return blockBrickTile(nBrickId);
}
NPoint LevelBlock::blockVTBrickPos(int32_t /*nViewTick*/, int32_t /*nTotViewTicks*/, int32_t nBrickId) const noexcept
{
	return blockBrickPos(nBrickId);
}
bool LevelBlock::blockVTBrickVisible(int32_t /*nViewTick*/, int32_t /*nTotViewTicks*/, int32_t nBrickId) const noexcept
{
	return blockBrickVisible(nBrickId);
}
int32_t LevelBlock::blockVTPosZ(int32_t /*nViewTick*/, int32_t /*nTotViewTicks*/) const noexcept
{
	return blockPosZ();
}

double LevelBlock::blockGetTileAniElapsed(int32_t nBrickId, int32_t nIdxTileAni, int32_t nViewTick, int32_t nTotTicks) const noexcept
{
	assert(m_p0Level != nullptr);
	assert(m_oBlock.isBrickId(nBrickId));
	assert(m_oLevelBricks.find(nBrickId) != m_oLevelBricks.end());
	assert((nIdxTileAni >= 0) && (nIdxTileAni < m_p0Level->getNrTileAniAttrs()));
	const LevelBrick& oBrick = m_oLevelBricks.at(nBrickId);//[nBrickId];
	const std::vector<BrickAni>& aBrickAni = oBrick.m_aBrickAni;
	assert(static_cast<int32_t>(aBrickAni.size()) == m_p0Level->getNrTileAniAttrs());
	const BrickAni& oBrickAni = aBrickAni[nIdxTileAni];
	const TileAnimator* p0TileAnimator = oBrickAni.m_p0TileAnimator;
	if (p0TileAnimator == nullptr)
	{
		return TileAnimator::s_fInactiveElapsed;
	}
	return p0TileAnimator->getElapsed01(oBrickAni.m_nHash, *this, nBrickId, nIdxTileAni, nViewTick, nTotTicks);
}
void LevelBlock::blockSetTileAnimator(int32_t nBrickId, int32_t nIdxTileAni, TileAnimator* p0TileAnimator, int32_t nHash) noexcept
{
	assert(m_p0Level != nullptr);
	assert(m_oBlock.isBrickId(nBrickId));
	assert(m_oLevelBricks.find(nBrickId) != m_oLevelBricks.end());
	assert((nIdxTileAni >= 0) && (nIdxTileAni < m_p0Level->getNrTileAniAttrs()));
	LevelBrick& oBrick = m_oLevelBricks[nBrickId];
	std::vector<BrickAni>& aBrickAni = oBrick.m_aBrickAni;
	assert(static_cast<int32_t>(aBrickAni.size()) == m_p0Level->getNrTileAniAttrs());
	BrickAni& oBrickAni = aBrickAni[nIdxTileAni];
	oBrickAni.m_p0TileAnimator = p0TileAnimator;
	oBrickAni.m_nHash = nHash;
}
const TileAnimator* LevelBlock::blockGetTileAnimator(int32_t nBrickId, int32_t nIdxTileAni) const noexcept
{
	assert(m_p0Level != nullptr);
	assert(m_oBlock.isBrickId(nBrickId));
	assert(m_oLevelBricks.find(nBrickId) != m_oLevelBricks.end());
	assert((nIdxTileAni >= 0) && (nIdxTileAni < m_p0Level->getNrTileAniAttrs()));
	const LevelBrick& oBrick = m_oLevelBricks.at(nBrickId);//[nBrickId];
	const std::vector<BrickAni>& aBrickAni = oBrick.m_aBrickAni;
	assert(static_cast<int32_t>(aBrickAni.size()) == m_p0Level->getNrTileAniAttrs());
	const BrickAni& oBrickAni = aBrickAni[nIdxTileAni];
	return oBrickAni.m_p0TileAnimator;
}
bool LevelBlock::canFuseWith(LevelBlock& /*oLevelBlock*/) const noexcept
{
	return false;
}
void LevelBlock::LevelBrick::clear(int32_t nTileAnis) noexcept
{
	assert(nTileAnis >= 0);
	m_aBrickAni.resize(nTileAnis);
	for (int32_t nIdx = 0; nIdx < nTileAnis; ++nIdx) {
		m_aBrickAni[nIdx].m_p0TileAnimator = nullptr;
		m_aBrickAni[nIdx].m_nHash = -1;
	}
}

Coords LevelBlock::getCoords(const LevelBlock& oLevelBlock) noexcept
{
	Coords oCoords;
	const int32_t nPosX = oLevelBlock.m_nPosX;
	const int32_t nPosY = oLevelBlock.m_nPosY;
	const int32_t nShape = oLevelBlock.m_nShapeId;
	const Block& oBlock = oLevelBlock.m_oBlock;
	auto aBrickId = oBlock.brickIds();
	for (auto& nBrickId : aBrickId) {
		if (oBlock.shapeBrickVisible(nShape, nBrickId)) {
			const int32_t nX = nPosX + oBlock.shapeBrickPosX(nShape, nBrickId);
			const int32_t nY = nPosY + oBlock.shapeBrickPosY(nShape, nBrickId);
			oCoords.add(nX, nY);
		}
	}
	return oCoords;
}

} // namespace stmg
