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
 * File:   fakelevelview.cc
 */

#include "stmm-games-fake/fakelevelview.h"

#include <stmm-games/game.h>
#include <stmm-games/level.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/coords.h>
#include <stmm-games/util/direction.h>
#include <stmm-games/utile/tilecoords.h>

#include <cassert>
//#include <iostream>
#include <utility>

namespace stmg { class LevelAnimation; }
namespace stmg { class LevelBlock; }
namespace stmg { class TileRect; }


namespace stmg
{

using std::make_unique;

FakeLevelView::FakeLevelView(Game* p0Game, Level* p0Level) noexcept
{
	assert(p0Game != nullptr);
	assert(p0Level != nullptr);
	const int32_t nLevel = p0Level->getLevel();
	assert(p0Game->level(nLevel).operator->() == p0Level);
	p0Game->setLevelView(nLevel, this);
}
void FakeLevelView::clear() noexcept
{
	m_oAllCalls.clear();
}
void FakeLevelView::boardAnimateTiles(NRect oArea) noexcept
{
	auto refCalled = make_unique<BoardAnimateTiles>();
	refCalled->m_oArea = std::move(oArea);
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::boardAnimateTile(NPoint oXY) noexcept
{
	auto refCalled = make_unique<BoardAnimateTiles>();
	NRect oArea;
	oArea.m_nX = oXY.m_nX;
	oArea.m_nY = oXY.m_nY;
	oArea.m_nW = 1;
	oArea.m_nH = 1;
	refCalled->m_oArea = std::move(oArea);
	m_oAllCalls.push_back(std::move(refCalled));
}
bool FakeLevelView::BoardAnimateTiles::pointWasAnimated(const std::vector<BoardAnimateTiles*>& aCalled
														, const NPoint& oXY) noexcept
{
	for (auto& p0Called : aCalled) {
		assert(p0Called != nullptr);
		if (p0Called->m_oArea.containsPoint(oXY)) {
			return true;
		}
	}
	return false;
}

bool FakeLevelView::animationCreate(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refLevelAnimation);
	if (totCalled(refLevelAnimation) == 1) {
		return false;
	}
	auto refCalled = make_unique<AnimationCreate>();
	refCalled->m_refLevelAnimation = refLevelAnimation;
	m_oAllCalls.push_back(std::move(refCalled));
	return true;
}
bool FakeLevelView::animationRemove(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refLevelAnimation);
	if (totCalled(refLevelAnimation) == 0) {
		return false;
	}
	auto refCalled = make_unique<AnimationRemove>();
	refCalled->m_refLevelAnimation = refLevelAnimation;
	m_oAllCalls.push_back(std::move(refCalled));
	return true;
}
void FakeLevelView::boardPreScroll(Direction::VALUE eDir, const shared_ptr<TileRect>& refTiles) noexcept
{
	auto refCalled = make_unique<BoardPreScroll>();
	refCalled->m_eDir = eDir;
	refCalled->m_refTiles = refTiles;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::boardPostScroll(Direction::VALUE eDir) noexcept
{
	auto refCalled = make_unique<BoardPostScroll>();
	refCalled->m_eDir = eDir;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::boabloPreFreeze(LevelBlock& oBlock) noexcept
{
	auto refCalled = make_unique<BoabloPreFreeze>();
	refCalled->m_p0Block = &oBlock;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::boabloPostFreeze(const Coords& oCoords) noexcept
{
	auto refCalled = make_unique<BoabloPostFreeze>();
	refCalled->m_oCoords = oCoords;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::boabloPreUnfreeze(const Coords& oCoords) noexcept
{
	auto refCalled = make_unique<BoabloPreUnfreeze>();
	refCalled->m_oCoords = oCoords;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::boabloPostUnfreeze(LevelBlock& oBlock) noexcept
{
	auto refCalled = make_unique<BoabloPostUnfreeze>();
	refCalled->m_p0Block = &oBlock;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::boardPreInsert(Direction::VALUE eDir, NRect oArea, const shared_ptr<TileRect>& refTiles) noexcept
{
	auto refCalled = make_unique<BoardPreInsert>();
	refCalled->m_eDir = eDir;
	refCalled->m_oArea = oArea;
	refCalled->m_refTiles = refTiles;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::boardPostInsert(Direction::VALUE eDir, NRect oArea) noexcept
{
	auto refCalled = make_unique<BoardPostInsert>();
	refCalled->m_eDir = eDir;
	refCalled->m_oArea = oArea;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::boardPreDestroy(const Coords& oCoords) noexcept
{
	auto refCalled = make_unique<BoardPreDestroy>();
	refCalled->m_oCoords = oCoords;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::boardPostDestroy(const Coords& oCoords) noexcept
{
	auto refCalled = make_unique<BoardPostDestroy>();
	refCalled->m_oCoords = oCoords;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::boardPreModify(const TileCoords& oTileCoords) noexcept
{
	auto refCalled = make_unique<BoardPreModify>();
	refCalled->m_oTileCoords = oTileCoords;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::boardPostModify(const Coords& oCoords) noexcept
{
	auto refCalled = make_unique<BoardPostModify>();
	refCalled->m_oCoords = oCoords;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::blockPreAdd(const LevelBlock& oBlock) noexcept
{
	auto refCalled = make_unique<BlockPreAdd>();
	refCalled->m_p0Block = &oBlock;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::blockPostAdd(LevelBlock& oBlock) noexcept
{
	auto refCalled = make_unique<BlockPostAdd>();
	refCalled->m_p0Block = &oBlock;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::blockPreRemove(LevelBlock& oBlock) noexcept
{
	auto refCalled = make_unique<BlockPreRemove>();
	refCalled->m_p0Block = &oBlock;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::blockPostRemove(const LevelBlock& oBlock) noexcept
{
	auto refCalled = make_unique<BlockPostRemove>();
	refCalled->m_p0Block = &oBlock;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::blockPreDestroy(LevelBlock& oBlock) noexcept
{
	auto refCalled = make_unique<BlockPreDestroy>();
	refCalled->m_p0Block = &oBlock;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::blockPostDestroy(const LevelBlock& oBlock) noexcept
{
	auto refCalled = make_unique<BlockPostDestroy>();
	refCalled->m_p0Block = &oBlock;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::blockPreFuse(LevelBlock& oFusedToBlock, LevelBlock& oFusedBlock) noexcept
{
	auto refCalled = make_unique<BlockPreFuse>();
	refCalled->m_p0FusedToBlock = &oFusedToBlock;
	refCalled->m_p0FusedBlock = &oFusedBlock;
	m_oAllCalls.push_back(std::move(refCalled));
}
void FakeLevelView::blockPostFuse(LevelBlock& oResBlock, const LevelBlock& oFusedBlock
						, const std::unordered_map<int32_t, int32_t>& oFusedToBrickIds
						, const std::unordered_map<int32_t, int32_t>& oFusedBrickIds) noexcept
{
	auto refCalled = make_unique<BlockPostFuse>();
	refCalled->m_p0ResBlock = &oResBlock;
	refCalled->m_p0FusedBlock = &oFusedBlock;
	refCalled->m_oFusedToBrickIds = oFusedToBrickIds;
	refCalled->m_oFusedBrickIds = oFusedBrickIds;
	m_oAllCalls.push_back(std::move(refCalled));
}

int32_t FakeLevelView::totCalled(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refLevelAnimation);
	int32_t nTot = 0;
	for (const auto& refCalled : m_oAllCalls) {
		Called* p0Called = refCalled.operator->();
		if (AnimationCreate* p0AnimationCreate = dynamic_cast<AnimationCreate*>(p0Called)) {
			if (p0AnimationCreate->m_refLevelAnimation == refLevelAnimation) {
				++nTot;
			}
		} else if (AnimationRemove* p0AnimationRemove = dynamic_cast<AnimationRemove*>(p0Called)) {
			if (p0AnimationRemove->m_refLevelAnimation == refLevelAnimation) {
				--nTot;
			}
		}
	}
	return nTot;
}

} // namespace stmg
