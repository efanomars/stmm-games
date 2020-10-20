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
 * File:   fakelevelview.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef _ZIMTRIS_FAKE_LEVEL_VIEW_H_
#define _ZIMTRIS_FAKE_LEVEL_VIEW_H_

#include <stmm-games/levelview.h>
#include <stmm-games/utile/tilecoords.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/coords.h>
#include <stmm-games/util/direction.h>

#include <tuple>
#include <memory>
#include <unordered_map>
#include <vector>

#include <stdint.h>

namespace stmg { class Game; }
namespace stmg { class Level; }
namespace stmg { class LevelAnimation; }
namespace stmg { class LevelBlock; }
namespace stmg { class TileRect; }

namespace stmg
{

using std::unique_ptr;
using std::shared_ptr;
using std::tuple;

/** Fake LevelView for testing.
 * Lets you collect some of the callbacks from the Level.
 */
class FakeLevelView : public LevelView
{
public:
	FakeLevelView(Game* p0Game, Level* p0Level) noexcept;

	class Called
	{
	public:
		virtual ~Called() noexcept = default;
	};

	struct BoardAnimateTiles : public Called
	{
		NRect m_oArea;
	};
	void boardAnimateTiles(NRect oArea) noexcept override;
	void boardAnimateTile(NPoint oXY) noexcept override;

	struct AnimationCreate : public Called
	{
		shared_ptr<LevelAnimation> m_refLevelAnimation;
	};
	bool animationCreate(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;

	struct AnimationRemove : public Called
	{
		shared_ptr<LevelAnimation> m_refLevelAnimation;
	};
	bool animationRemove(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;

	struct BoardPreScroll : public Called
	{
		Direction::VALUE m_eDir;
		shared_ptr<TileRect> m_refTiles;
	};
	void boardPreScroll(Direction::VALUE eDir, const shared_ptr<TileRect>& refTiles) noexcept override;

	struct BoardPostScroll : public Called
	{
		Direction::VALUE m_eDir;
	};
	void boardPostScroll(Direction::VALUE eDir) noexcept override;

	struct BoabloPreFreeze : public Called
	{
		LevelBlock* m_p0Block;
	};
	void boabloPreFreeze(LevelBlock& oBlock) noexcept override;

	struct BoabloPostFreeze : public Called
	{
		Coords m_oCoords;
	};
	void boabloPostFreeze(const Coords& oCoords) noexcept override;

	struct BoabloPreUnfreeze : public Called
	{
		Coords m_oCoords;
	};
	void boabloPreUnfreeze(const Coords& oCoords) noexcept override;

	struct BoabloPostUnfreeze : public Called
	{
		LevelBlock* m_p0Block;
	};
	void boabloPostUnfreeze(LevelBlock& oBlock) noexcept override;

	struct BoardPreInsert : public Called
	{
		Direction::VALUE m_eDir;
		NRect m_oArea;
		shared_ptr<TileRect> m_refTiles;
	};
	void boardPreInsert(Direction::VALUE eDir, NRect oArea, const shared_ptr<TileRect>& refTiles) noexcept override;

	struct BoardPostInsert : public Called
	{
		Direction::VALUE m_eDir;
		NRect m_oArea;
	};
	void boardPostInsert(Direction::VALUE eDir, NRect oArea) noexcept override;

	struct BoardPreDestroy : public Called
	{
		Coords m_oCoords;
	};
	void boardPreDestroy(const Coords& oCoords) noexcept override;

	struct BoardPostDestroy : public Called
	{
		Coords m_oCoords;
	};
	void boardPostDestroy(const Coords& oCoords) noexcept override;

	struct BoardPreModify : public Called
	{
		TileCoords m_oTileCoords;
	};
	void boardPreModify(const TileCoords& oTileCoords) noexcept override;

	struct BoardPostModify : public Called
	{
		Coords m_oCoords;
	};
	void boardPostModify(const Coords& oCoords) noexcept override;

	struct BlockPreAdd : public Called
	{
		LevelBlock const * m_p0Block;
	};
	void blockPreAdd(const LevelBlock& oBlock) noexcept override;

	struct BlockPostAdd : public Called
	{
		LevelBlock const * m_p0Block;
	};
	void blockPostAdd(LevelBlock& oBlock) noexcept override;

	struct BlockPreRemove : public Called
	{
		LevelBlock const * m_p0Block;
	};
	void blockPreRemove(LevelBlock& oBlock) noexcept override;

	struct BlockPostRemove : public Called
	{
		LevelBlock const * m_p0Block;
	};
	void blockPostRemove(const LevelBlock& oBlock) noexcept override;

	struct BlockPreDestroy : public Called
	{
		LevelBlock const * m_p0Block;
	};
	void blockPreDestroy(LevelBlock& oBlock) noexcept override;

	struct BlockPostDestroy : public Called
	{
		LevelBlock const * m_p0Block;
	};
	void blockPostDestroy(const LevelBlock& oBlock) noexcept override;

	struct BlockPreFuse : public Called
	{
		LevelBlock* m_p0FusedToBlock;
		LevelBlock* m_p0FusedBlock;
	};
	void blockPreFuse(LevelBlock& oFusedToBlock, LevelBlock& oFusedBlock) noexcept override;

	struct BlockPostFuse : public Called
	{
		LevelBlock* m_p0ResBlock;
		LevelBlock const * m_p0FusedBlock;
		std::unordered_map<int32_t, int32_t> m_oFusedToBrickIds;
		std::unordered_map<int32_t, int32_t> m_oFusedBrickIds;
	};
	void blockPostFuse(LevelBlock& oResBlock, const LevelBlock& oFusedBlock
						, const std::unordered_map<int32_t, int32_t>& oFusedToBrickIds
						, const std::unordered_map<int32_t, int32_t>& oFusedBrickIds) noexcept override;

	const std::vector<unique_ptr<Called>>& getAllCalled() noexcept;
	// Use the following like this: getCalled<BlockPreDestroy>()
	template<typename T>
	const std::vector<T*> getCalled() noexcept
	{
		std::vector<T*> aRes;
		for (const auto& refCalled : m_oAllCalls) {
			Called* p0Called = refCalled.operator->();
			if (T* p0T = dynamic_cast<T*>(p0Called)) {
				aRes.push_back(p0T);
			}
		}
		return aRes;
	}
private:
	int32_t totCalled(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept;
private:
	std::vector<unique_ptr<Called>> m_oAllCalls;

private:
	FakeLevelView() = delete;
	FakeLevelView(const FakeLevelView& oSource) = delete;
	FakeLevelView& operator=(const FakeLevelView& oSource) = delete;
};

} // namespace stmg

#endif	/* _ZIMTRIS_FAKE_LEVEL_VIEW_H_ */

