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
 * File:   stdlevelview.h
 */

#ifndef STMG_STD_LEVEL_VIEW_H
#define STMG_STD_LEVEL_VIEW_H

#include "themeanimation.h"

#include <stmm-games/levelanimation.h>
#include <stmm-games/levelblock.h>
#include <stmm-games/levelview.h>
#include <stmm-games/util/direction.h>

#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <cairomm/refptr.h>

#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <vector>

#include <stdint.h>

namespace stmg { struct FPoint; }
namespace stmg { class Coords; }
namespace stmg { class Game; }
namespace stmg { class Level; }
namespace stmg { class LevelShowThemeWidget; }
namespace stmg { class Theme; }
namespace stmg { class ThemeContext; }
namespace stmg { class TileCoords; }
namespace stmg { class TileRect; }
namespace stmg { struct NPoint; }
namespace stmg { struct NRect; }

namespace stmg
{

class StdView;

class StdLevelView : public LevelView
{
public:
	StdLevelView() noexcept;

	void reInit(bool bSubshows, const std::vector< shared_ptr<LevelShowThemeWidget> >& aLevelShows
				, const shared_ptr<Game>& refGame, int32_t nLevel, const shared_ptr<Theme>& refTheme
				, const shared_ptr<ThemeContext>& refThemeCtx, StdView* p0StdView) noexcept;

	void onSizeChanged(int32_t nTileW, int32_t nTileH) noexcept;

	void beforeGameTick() noexcept; // TODO rename to gameBeforeTick
	void drawStepToBuffers(int32_t nViewTick, int32_t nTotViewTicks, bool bRedraw) noexcept;
	void drawBuffers(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;

	//std::pair<double, double> getXY(int32_t nViewPixX, int32_t nViewPixY);

	// Level callbacks
	void boabloPreFreeze(LevelBlock& oBlock) noexcept override;
	void boabloPostFreeze(const Coords& oCoords) noexcept override;
	void boabloPreUnfreeze(const Coords& oCoords) noexcept override;
	void boabloPostUnfreeze(LevelBlock& oBlock) noexcept override;

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

	void boardPreScroll(Direction::VALUE eDir, const shared_ptr<TileRect>& refTiles) noexcept override;
	void boardPostScroll(Direction::VALUE eDir) noexcept override;

	void boardPreInsert(Direction::VALUE eDir, NRect oArea, const shared_ptr<TileRect>& refTiles) noexcept override;
	void boardPostInsert(Direction::VALUE eDir, NRect oArea) noexcept override;
	void boardPreModify(const TileCoords& oTileCoords) noexcept override;
	void boardPostModify(const Coords& oCoords) noexcept override;
	void boardPreDestroy(const Coords& oCoords) noexcept override;
	void boardPostDestroy(const Coords& oCoords) noexcept override;

	void boardAnimateTiles(NRect oArea) noexcept override;
	void boardAnimateTile(NPoint oXY) noexcept override;

	bool animationCreate(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;
	bool animationRemove(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;

	void setSoundListenersToShowCenter() noexcept;
	void setSoundListenersToSubshowCenter() noexcept;

#ifndef NDEBUG
	void dump(bool bTickTileAnis) const noexcept;
#endif //NDEBUG

private:

	void setSoundListenersToShowCenter(const FPoint& oShowPos) noexcept;
	void setSoundListenerToSubshowCenter(int32_t nLevelPlayer, const FPoint& oShowPos, const FPoint& oSubshowPos) noexcept;

	void drawStep(const Cairo::RefPtr<Cairo::Context>& cr) noexcept;
	struct AniData;

	void clearAniDatas(std::vector< std::unique_ptr<AniData> >& aAniData) noexcept;
	void clearSubshowData() noexcept;

	void recreateSurfs() noexcept;

	struct AniData
	{
		LevelBlock* m_p0LevelBlock; // (when active) if null implies m_refAnimation not null
		shared_ptr<ThemeAnimation> m_refAnimation; // animation view: (when active) if null implies m_p0LevelBlock not null
		shared_ptr<LevelAnimation> m_refLevelAnimation; // animation model
		LevelAnimation::REFSYS m_eRefSys; // if = LevelAnimation::REFSYS_SUBSHOW its value  is the level player

		inline int32_t getZ(int32_t nViewTick, int32_t nTotViewTicks) const noexcept
		{
			return ((m_p0LevelBlock == nullptr)
					? m_refAnimation->getZ(nViewTick, nTotViewTicks)
					: m_p0LevelBlock->blockVTPosZ(nViewTick, nTotViewTicks));
		}
	};
	// return true if to be removed
	bool drawAniData(AniData& oAniData, const Cairo::RefPtr<Cairo::Context>& refCc, double fShowPixX, double fShowPixY
					, int32_t nViewTick, int32_t nTotViewTicks) noexcept;

	void drawBoard(const Cairo::RefPtr<Cairo::Context>& cr) noexcept;
	void drawBoard(const Cairo::RefPtr<Cairo::Context>& cr, int32_t nX, int32_t nY, int32_t nW, int32_t nH
					, int32_t nViewTick, int32_t nTotViewTicks) noexcept;
	void drawLevelBlock(const Cairo::RefPtr<Cairo::Context>& cr, LevelBlock& oLevelBlock, int32_t nViewTick, int32_t nTotViewTicks) noexcept;
	// if position is tile-animated does nothing
	void redrawBoardPos(int32_t nX, int32_t nY) noexcept;

	void reMoveTickAnimatedTiles(int32_t nRemoveX, int32_t nRemoveY, int32_t nRemoveW, int32_t nRemoveH
									, int32_t nAreaX, int32_t nAreaY, int32_t nAreaW, int32_t nAreaH
									, int32_t nDx, int32_t nDy) noexcept;
	void boardInsertKeep(const Cairo::RefPtr<Cairo::Context>& refCc
							, int32_t nX, int32_t nY, int32_t nW, int32_t nH) noexcept;
	void boardInsertMove(const Cairo::RefPtr<Cairo::Context>& refCc
							, int32_t nX, int32_t nY, int32_t nW, int32_t nH, int32_t nDx, int32_t nDy) noexcept;
	void boardInsertRedrawAndRemove(const Cairo::RefPtr<Cairo::Context>& refCc
									, int32_t nX, int32_t nY, int32_t nW, int32_t nH, int32_t nDx, int32_t nDy
									, int32_t nX1, int32_t nY1, int32_t nW1, int32_t nH1
									, int32_t nX2, int32_t nY2, int32_t nW2, int32_t nH2
									, bool bRedraw1) noexcept;

	std::unique_ptr<AniData> anidataRecycle() noexcept;
	std::vector< std::unique_ptr<AniData> >::iterator anidataRecycleKeepOrder(std::vector< std::unique_ptr<AniData> >& aAniData
																	, const std::vector< std::unique_ptr<AniData> >::iterator& itAniData) noexcept;
	void anidataRecycle(std::vector< std::unique_ptr<AniData> >& aAniData
						, const std::vector< std::unique_ptr<AniData> >::iterator& itAniData) noexcept;
private:
	StdView* m_p0StdView;
	bool m_bSoundEnabled;
	bool m_bPerPlayerSound;
	bool m_bSubshows;
	bool m_bAllTeamsInOneLevel;
	int32_t m_nLevel;
	int32_t m_nBasePrefPlayer;
	shared_ptr<Game> m_refGame;
	shared_ptr<Level> m_refLevel;
	shared_ptr<Theme> m_refTheme;
	shared_ptr<ThemeContext> m_refThemeCtx;

	int32_t m_nUniqueHumanPrefPlayer;

	int32_t m_nThemeTotTileAnis;

	// Ani data can be in
	// - m_aAniDataNotStarted
	// - m_aAniDataNonSubshow
	// - m_aSubshowData[nLevelPlayer].m_aAniDataSubshow
	// - m_aAniDataRecycle
	std::vector< std::unique_ptr<AniData> > m_aAniDataNotStarted;
	// TODO LevelAnimation should have a canChangeZPos, LevelBlock too
	// TODO If all AniData can't change z it would be faster to have a
	// TODO vector of (z,id) to sort by z and a unordered_map id to AniData
	std::vector< std::unique_ptr<AniData> > m_aAniDataNonSubshow; // Ordered by z
	std::vector< std::unique_ptr<AniData> > m_aAniDataRecycle;

	int32_t m_nTileW;
	int32_t m_nTileH;

	// in tiles
	int32_t m_nBoardW;
	int32_t m_nBoardH;
	int32_t m_nShowW;
	int32_t m_nShowH;
	int32_t m_nSubshowW;
	int32_t m_nSubshowH;

	int32_t m_nBoardSurfPixW;
	int32_t m_nBoardSurfPixH;
	int32_t m_nShowSurfPixW;
	int32_t m_nShowSurfPixH;
	int32_t m_nSubshowSurfPixW; // If not in subshow mode this is -1
	int32_t m_nSubshowSurfPixH; // If not in subshow mode this is -1
	struct SubshowData
	{
		shared_ptr<LevelShowThemeWidget> m_refLSTW; // The widget to paint to
		std::vector< std::unique_ptr<AniData> > m_aAniDataSubshow; // Ordered by z
		// This surface is where m_refLSTW is drawn followed by
		// subshow animations with z < 0, followed by the clipped contents of
		// m_refShowSurf, followed by subshow animations with z >= 0.
		// Size: m_nSubshowSurfPixW x m_nSubshowSurfPixH
		Cairo::RefPtr<Cairo::ImageSurface> m_refSubshowSurf;
		Cairo::RefPtr<Cairo::Context> m_refSubshowCc;
	};
	// If m_bSubshow is false this vector is empty,
	// if m_bSubshow is true this vector contains the Show data for each player of the level
	std::vector< std::unique_ptr<SubshowData> > m_aSubshowData;
	std::vector< std::unique_ptr<SubshowData> > m_aSubshowDataRecycle;
	// This only is set if not in subshow mode, to paint the background of m_refShowSurf
	// If m_aSubshowData.size() > 0 then m_refLevelShowTW is null
	shared_ptr<LevelShowThemeWidget> m_refLevelShowTW;

	// This surface only contains the tiles (also their tileani)
	// on a (0,0,0,0) surface
	// Size: m_nBoardSurfPixW x m_nBoardSurfPixH
	Cairo::RefPtr<Cairo::ImageSurface> m_refBoardSurf; // >= (m_nBoardSurfPixW x m_nBoardSurfPixH)
	Cairo::RefPtr<Cairo::Context> m_refBoardCc;
	// This is a helper surface used to perform operations such as
	// insert tiles, delete tiles, etc.
	Cairo::RefPtr<Cairo::ImageSurface> m_refBoard2Surf; // >= (m_nBoardSurfPixW x m_nBoardSurfPixH)
	Cairo::RefPtr<Cairo::Context> m_refBoard2Cc;
	// This is where the non-subshow animations, level blocks and board are painted.
	// In Subshow mode this is painted to SubshowData.m_refSubshowSurf, otherwise
	// it is directly copied to the window.
	// If not in subshow mode the background (drawn by m_refLevelShowTW)  TODO ???
	// is included.
	// Size: m_nShowSurfPixW x m_nShowSurfPixH
	Cairo::RefPtr<Cairo::ImageSurface> m_refShowSurf; // Show size
	Cairo::RefPtr<Cairo::Context> m_refShowCc;

	std::vector<double> m_aTileAniElapsed; // utility array to pass params

	std::unordered_set<int64_t> m_oTickTileAnis;
	std::unordered_set<int64_t> m_oTickTileAnisWork;

private:
	StdLevelView(const StdLevelView& oSource) = delete;
	StdLevelView& operator=(const StdLevelView& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_STD_LEVEL_VIEW_H */

