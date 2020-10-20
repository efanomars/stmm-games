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
 * File:   stdlevelview.cc
 */

#include "stdlevelview.h"
#include "stdview.h"

#include "theme.h"
#include "themecontext.h"
#include "levelshowthemewidget.h"

#include <stmm-games/game.h>
#include <stmm-games/level.h>
#include <stmm-games/levelshow.h>
#include <stmm-games/levelanimation.h>
#include <stmm-games/levelblock.h>
#include <stmm-games/named.h>
#include <stmm-games/tile.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/namedindex.h>
#include <stmm-games/animations/explosionanimation.h>
#include <stmm-games/util/recycler.h>
#include <stmm-games/util/coords.h>
#include <stmm-games/util/util.h>
#include <stmm-games/util/direction.h>

#include <stmm-input-au/playbackcapability.h>

#include <gtkmm.h>

#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <cstdint>
#include <utility>

namespace stmg { class TileCoords; }
namespace stmg { class TileRect; }

namespace stmg
{

static const int32_t s_nZObjectZBoardTileDestruct = 2;

class PrivateExplosionAnimation : public ExplosionAnimation
{
public:
	using ExplosionAnimation::ExplosionAnimation;
	using ExplosionAnimation::reInit;
};
static Recycler<PrivateExplosionAnimation, ExplosionAnimation> s_oBoardDestructRecycler;

StdLevelView::StdLevelView() noexcept
: m_p0StdView(nullptr)
, m_bSoundEnabled(false)
, m_bPerPlayerSound(false)
, m_bSubshows(false)
, m_bAllTeamsInOneLevel(false)
, m_nLevel(-1)
, m_nBasePrefPlayer(-1)
, m_nThemeTotTileAnis(-1)
, m_nTileW(-1)
, m_nTileH(-1)
, m_nBoardW(-1)
, m_nBoardH(-1)
, m_nShowW(-1)
, m_nShowH(-1)
, m_nSubshowW(-1)
, m_nSubshowH(-1)
, m_nBoardSurfPixW(-1)
, m_nBoardSurfPixH(-1)
, m_nShowSurfPixW(-1)
, m_nShowSurfPixH(-1)
, m_nSubshowSurfPixW(-1)
, m_nSubshowSurfPixH(-1)
{
}

void StdLevelView::reInit(bool bSubshows, const std::vector< shared_ptr<LevelShowThemeWidget> >& aLevelShows
						, const shared_ptr<Game>& refGame, int32_t nLevel, const shared_ptr<Theme>& refTheme, const shared_ptr<ThemeContext>& refThemeCtx
						, StdView* p0StdView) noexcept
{
//std::cout << "StdLevelView[" << reinterpret_cast<int64_t>(this) <<  "]::reInit  bSubshows=" << bSubshows << '\n';
	assert(refThemeCtx);
	assert(refTheme);
	assert(p0StdView != nullptr);
	assert(refGame);
	m_p0StdView = p0StdView;
	m_bSoundEnabled = m_p0StdView->m_bSoundEnabled;
	m_bPerPlayerSound = m_p0StdView->m_bPerPlayerSound;
	m_nUniqueHumanPrefPlayer = m_p0StdView->m_nUniqueHumanPrefPlayer;
	m_refGame = refGame;
	m_bSubshows = bSubshows;
	m_bAllTeamsInOneLevel = refGame->isAllTeamsInOneLevel();
	clearSubshowData();
	if (bSubshows) {
//std::cout << "StdLevelView::reInit aLevelShows.size()=" << aLevelShows.size() << '\n';
		for (auto& refLSTW : aLevelShows) {
			if (m_aSubshowDataRecycle.empty()) {
				m_aSubshowData.push_back(std::make_unique<SubshowData>());
			} else {
				m_aSubshowData.push_back({});
				std::swap(m_aSubshowDataRecycle.back(), m_aSubshowData.back());
				m_aSubshowDataRecycle.pop_back();
			}
			assert(refLSTW);
			assert(m_aSubshowData.back());
			m_aSubshowData.back()->m_refLSTW = refLSTW;
		}
		m_refLevelShowTW.reset();
	} else {
		assert(aLevelShows.size() == 1);
		m_refLevelShowTW = aLevelShows[0];
		assert(m_refLevelShowTW);
	}
	m_nLevel = nLevel;
	m_refLevel = m_refGame->level(m_nLevel);
	m_refTheme = refTheme;
	m_refThemeCtx = refThemeCtx;
	m_nBasePrefPlayer = m_refGame->getPlayer(nLevel, 0);

	clearAniDatas(m_aAniDataNotStarted);
	clearAniDatas(m_aAniDataNonSubshow);

	m_nThemeTotTileAnis = m_refTheme->getNamed().tileAnis().size();

	m_nBoardW = m_refLevel->boardWidth();
	m_nBoardH = m_refLevel->boardHeight();
	assert(m_nBoardW > 0);
	assert(m_nBoardH > 0);
	m_nShowW = m_refLevel->showGet().getW();
	m_nShowH = m_refLevel->showGet().getH();
	assert((m_nShowW > 0) && (m_nShowW <= m_nBoardW));
	assert((m_nShowH > 0) && (m_nShowH <= m_nBoardH));
	m_nSubshowW = (m_bSubshows ? m_refLevel->subshowGet(0, 0).getW() : -1);
	m_nSubshowH = (m_bSubshows ? m_refLevel->subshowGet(0, 0).getH() : -1);
	if (m_bSubshows) {
		assert((m_nSubshowW > 0) && (m_nSubshowW <= m_nShowW));
		assert((m_nSubshowH > 0) && (m_nSubshowH <= m_nShowH));
	}
	m_oTickTileAnis.clear();

	// This is to make clear that onSizeChanged() has to be called
	m_nTileW = -1;
	m_nTileH = -1;

	m_refGame->setLevelView(m_nLevel, this);
}
void StdLevelView::clearAniDatas(std::vector< std::unique_ptr<AniData> >& aAniData) noexcept
{
	for (auto& refAniData : aAniData) {
		if (refAniData) {
			refAniData->m_refAnimation.reset();
			refAniData->m_refLevelAnimation.reset();
			refAniData->m_p0LevelBlock = nullptr;
			m_aAniDataRecycle.push_back({});
			std::swap(refAniData, m_aAniDataRecycle.back());
			assert(!refAniData);
			assert(m_aAniDataRecycle.back().operator bool());
		}
	}
	aAniData.clear();
}
void StdLevelView::clearSubshowData() noexcept
{
	for (auto& refSubshowData : m_aSubshowData) {
		if (refSubshowData) {
			clearAniDatas(refSubshowData->m_aAniDataSubshow);
			refSubshowData->m_refLSTW.reset();
			m_aSubshowDataRecycle.push_back({});
			std::swap(refSubshowData, m_aSubshowDataRecycle.back());
			assert(!refSubshowData);
			assert(m_aSubshowDataRecycle.back().operator bool());
		}
	}
	m_aSubshowData.clear();
}
void StdLevelView::onSizeChanged(int32_t nTileW, int32_t nTileH) noexcept
{
	assert(nTileW > 0);
	assert(nTileH > 0);
	m_nTileW = nTileW;
	m_nTileH = nTileH;
	recreateSurfs();
}
void StdLevelView::recreateSurfs() noexcept
{
//std::cout << "StdLevelView::recreateSurfs  m_bSubshows=" << m_bSubshows << '\n';
	const int32_t nTileW = m_nTileW;
	const int32_t nTileH = m_nTileH;
	const int32_t nBoardPixW = nTileW * m_nBoardW;
	const int32_t nBoardPixH = nTileH * m_nBoardH;
	const int32_t nShowPixW = nTileW * m_nShowW;
	const int32_t nShowPixH = nTileH * m_nShowH;
	const int32_t nSubshowPixW = nTileW * m_nSubshowW;
	const int32_t nSubshowPixH = nTileH * m_nSubshowH;

	if ((m_nBoardSurfPixW != nBoardPixW) || (m_nBoardSurfPixH != nBoardPixH)) {
		m_nBoardSurfPixW = nBoardPixW;
		m_nBoardSurfPixH = nBoardPixH;
		int32_t nSurfPixW = nBoardPixW;
		int32_t nSurfPixH = nBoardPixH;
		bool bEnlarged = false;
		if (m_refBoardSurf) {
			if (m_refBoardSurf->get_width() < nSurfPixW) {
				bEnlarged = true;
			} else {
				nSurfPixW = m_refBoardSurf->get_width();
			}
			if (m_refBoardSurf->get_height() < nSurfPixH) {
				bEnlarged = true;
			} else {
				nSurfPixH = m_refBoardSurf->get_height();
			}
		} else {
			bEnlarged = true;
		}
		if (bEnlarged) {
			m_refBoardSurf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, nSurfPixW, nSurfPixH);
			m_refBoardCc = Cairo::Context::create(m_refBoardSurf);
			m_refBoard2Surf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, nSurfPixW, nSurfPixH);
			m_refBoard2Cc = Cairo::Context::create(m_refBoard2Surf);
		}
		m_refBoard2Cc->save();
		m_refBoard2Cc->set_source_rgba(0, 0, 0, 0); // transparency
		m_refBoard2Cc->set_operator(Cairo::OPERATOR_SOURCE);
		m_refBoard2Cc->rectangle(0, 0, nSurfPixW, nSurfPixH);
		m_refBoard2Cc->fill();
		m_refBoard2Cc->restore();
	}
	if ((m_nShowSurfPixW != nShowPixW) || (m_nShowSurfPixH != nShowPixH)) {
		m_nShowSurfPixW = nShowPixW;
		m_nShowSurfPixH = nShowPixH;
		// The surface can only enlarge
		int32_t nSurfPixW = nShowPixW;
		int32_t nSurfPixH = nShowPixH;
		bool bEnlarged = false;
		if (m_refShowSurf) {
			if (m_refShowSurf->get_width() < nSurfPixW) {
				bEnlarged = true;
			} else {
				nSurfPixW = m_refShowSurf->get_width();
			}
			if (m_refShowSurf->get_height() < nSurfPixH) {
				bEnlarged = true;
			} else {
				nSurfPixH = m_refShowSurf->get_height();
			}
		} else {
			bEnlarged = true;
		}
		if (bEnlarged) {
			m_refShowSurf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, nSurfPixW, nSurfPixH);
			m_refShowCc = Cairo::Context::create(m_refShowSurf);
		}
		m_refShowCc->save();
		m_refShowCc->set_source_rgba(0, 0, 0, 0); // transparency
		m_refShowCc->set_operator(Cairo::OPERATOR_SOURCE);
		m_refShowCc->rectangle(0, 0, nSurfPixW, nSurfPixH);
		m_refShowCc->fill();
		m_refShowCc->restore();
	}
//std::cout << "StdLevelView::recreateSurfs  nSubshowPixW=" << nSubshowPixW << "  nSubshowPixH=" << nSubshowPixH << '\n';
//std::cout << "                             m_nSubshowSurfPixW=" << m_nSubshowSurfPixW << "  m_nSubshowSurfPixH=" << m_nSubshowSurfPixH << '\n';
	if (m_bSubshows && ((m_nSubshowSurfPixW != nSubshowPixW) || (m_nSubshowSurfPixH != nSubshowPixH))) {
		m_nSubshowSurfPixW = nSubshowPixW;
		m_nSubshowSurfPixH = nSubshowPixH;
		for (auto& refSubshow : m_aSubshowData) {
			int32_t nSurfPixW = nSubshowPixW;
			int32_t nSurfPixH = nSubshowPixH;
			bool bEnlarged = false;
			if (refSubshow->m_refSubshowSurf) {
				const int32_t nCairoSurfW = refSubshow->m_refSubshowSurf->get_width();
				if (nCairoSurfW < nSurfPixW) {
					bEnlarged = true;
				} else {
					nSurfPixW = nCairoSurfW;
				}
				const int32_t nCairoSurfH = refSubshow->m_refSubshowSurf->get_height();
				if (nCairoSurfH < nSurfPixH) {
					bEnlarged = true;
				} else {
					nSurfPixH = nCairoSurfH;
				}
			} else {
				bEnlarged = true;
			}
			if (bEnlarged) {
				refSubshow->m_refSubshowSurf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, nSurfPixW, nSurfPixH);
				refSubshow->m_refSubshowCc = Cairo::Context::create(refSubshow->m_refSubshowSurf);
			}
			refSubshow->m_refSubshowCc->save();
			refSubshow->m_refSubshowCc->set_source_rgba(0, 0, 0, 0); // transparency
			refSubshow->m_refSubshowCc->set_operator(Cairo::OPERATOR_SOURCE);
			refSubshow->m_refSubshowCc->rectangle(0, 0, nSurfPixW, nSurfPixH);
			refSubshow->m_refSubshowCc->fill();
			refSubshow->m_refSubshowCc->restore();
		}
	}

	//
	m_refBoardCc->save();
	m_refBoardCc->set_source_rgba(0, 0, 0, 0); // transparency
	m_refBoardCc->set_operator(Cairo::OPERATOR_SOURCE);
	m_refBoardCc->rectangle(0, 0, nBoardPixW, nBoardPixH);
	m_refBoardCc->fill();
	m_refBoardCc->set_operator(Cairo::OPERATOR_OVER);
	drawBoard(m_refBoardCc);
	m_refBoardCc->restore();

}
		//std::pair<double, double> StdLevelView::getXY(int32_t nViewPixX, int32_t nViewPixY)
		//{
		//	const int32_t nShowRelPixX = nViewPixX - m_nShowPixX;
		//	const int32_t nShowRelPixY = nViewPixY - m_nShowPixY;
		//	assert((m_nTileW > 0) && (m_nTileH > 0));
		//	const double fShowRelX = 1.0 * nShowRelPixX / m_nTileW;
		//	const double fShowRelY = 1.0 * nShowRelPixY / m_nTileH;
		//
		//	double fShowX, fShowY;
		//	m_refLevel->showGetPos(fShowX, fShowY);
		//
		//	const double fBoardTilesX = fShowRelX + fShowX;
		//	const double fBoardTilesY = fShowRelY + fShowY;
		//	return std::make_pair(fBoardTilesX, fBoardTilesY);
		//}

void StdLevelView::beforeGameTick() noexcept
{
	m_oTickTileAnis.clear();
}
void StdLevelView::setSoundListenersToShowCenter() noexcept
{
	const auto oShowPos = m_refLevel->showGet().getPos();
	setSoundListenersToShowCenter(oShowPos);
}
void StdLevelView::setSoundListenersToShowCenter(const FPoint& oShowPos) noexcept
{
	const double fShowCenterX = oShowPos.m_fX + 0.5 * m_nShowW;
	const double fShowCenterY = oShowPos.m_fY + 0.5 * m_nShowH;
	auto& aPlaybacks = m_p0StdView->m_aPlayerPlaybacks;
	auto& aPlaybackDatas = m_p0StdView->m_aPlayerPlaybackDatas;
	if (m_bPerPlayerSound) {
		#ifndef NDEBUG
		const int32_t nTotPlaybacks = static_cast<int32_t>(aPlaybacks.size());
		#endif //NDEBUG
		const int32_t nTotLevelPlayers = m_refLevel->getTotLevelPlayers();
		const int32_t nEndPrefPlayer = m_nBasePrefPlayer + nTotLevelPlayers;
		assert(nEndPrefPlayer <= nTotPlaybacks);
		for (int32_t nCurPlayback = m_nBasePrefPlayer; nCurPlayback < nEndPrefPlayer; ++nCurPlayback) {
			auto& refPlayback = aPlaybacks[nCurPlayback];
			if (! refPlayback) {
				continue; // for nCurPlayback
			}
			StdView::PlaybackData& oPlaybackData = aPlaybackDatas[nCurPlayback];
			if (! (oShowPos == oPlaybackData.m_oCurrentListenerPos)) {
				oPlaybackData.m_oCurrentListenerPos = oShowPos;
				// set listener at center of shown area
				refPlayback->setListenerPos(fShowCenterX, fShowCenterY, 0.0);
			}
		}
	} else {
		auto& refPlayback = aPlaybacks[0];
		if (refPlayback) {
			StdView::PlaybackData& oPlaybackData = aPlaybackDatas[0];
//std::cout << "StdLevelView::setSoundListenersToShowCenter oPlaybackData.m_oCurrentListenerPos = (" << oPlaybackData.m_oCurrentListenerPos.m_fX << ", " << oPlaybackData.m_oCurrentListenerPos.m_fY << ")" << '\n';
//std::cout << "StdLevelView::setSoundListenersToShowCenter                            oShowPos = (" << oShowPos.m_fX << ", " << oShowPos.m_fY << ")" << '\n';
			if (! (oShowPos == oPlaybackData.m_oCurrentListenerPos)) {
				oPlaybackData.m_oCurrentListenerPos = oShowPos;
				// set listener at center of shown area
				refPlayback->setListenerPos(fShowCenterX, fShowCenterY, 0.0);
			}
		}
	}
}
void StdLevelView::setSoundListenersToSubshowCenter() noexcept
{
	const auto oShowPos = m_refLevel->showGet().getPos();
	const int32_t nTotLevelPlayers = m_refLevel->getTotLevelPlayers();
	for (int32_t nLevelPlayer = 0; nLevelPlayer < nTotLevelPlayers; ++nLevelPlayer) {
		const auto oSubshowPos = m_refLevel->subshowGet(nLevelPlayer).getPos(0, 1);
		setSoundListenerToSubshowCenter(nLevelPlayer, oShowPos, oSubshowPos);
	}
}
void StdLevelView::setSoundListenerToSubshowCenter(int32_t nLevelPlayer, const FPoint& oShowPos, const FPoint& oSubshowPos) noexcept
{
//std::cout << "StdLevelView::setSoundListenerToSubshowCenter m_nBasePrefPlayer = " << m_nBasePrefPlayer << "  nLevelPlayer = " << nLevelPlayer << '\n';
	const int32_t nPrefPlayer = m_nBasePrefPlayer + nLevelPlayer;
	auto& aPlaybacks = m_p0StdView->m_aPlayerPlaybacks;
	auto& refPlayback = aPlaybacks[nPrefPlayer];
	if (refPlayback) {
		auto& aPlaybackDatas = m_p0StdView->m_aPlayerPlaybackDatas;
		StdView::PlaybackData& oPlaybackData = aPlaybackDatas[nPrefPlayer];
		const bool bSetListenerAtSubshowCenter = (m_bPerPlayerSound || (m_nUniqueHumanPrefPlayer == nPrefPlayer));
		const double fSubshowCenterX = (bSetListenerAtSubshowCenter ? oShowPos.m_fX + oSubshowPos.m_fX + 0.5 * m_nSubshowW : 0.0);
		const double fSubshowCenterY = (bSetListenerAtSubshowCenter ? oShowPos.m_fY + oSubshowPos.m_fY + 0.5 * m_nSubshowH : 0.0);
		const FPoint oSubshowCenter{fSubshowCenterX, fSubshowCenterY};
		if (! (oSubshowCenter == oPlaybackData.m_oCurrentListenerPos)) {
			oPlaybackData.m_oCurrentListenerPos = oSubshowCenter;
			// set listener at center of shown area
			refPlayback->setListenerPos(fSubshowCenterX, fSubshowCenterY, 0.0);
//std::cout << "StdLevelView::setSoundListenerToSubshowCenter fSubshowCenterX = " << fSubshowCenterX << "  fSubshowCenterY = " << fSubshowCenterY << '\n';
		}
	}
}
void StdLevelView::drawStepToBuffers(int32_t nViewTick, int32_t nTotViewTicks, bool /*bRedraw*/) noexcept
{
	assert((nViewTick >= 0) && (nViewTick < nTotViewTicks));

	// Draw board`s animated tiles if any
	if (!m_oTickTileAnis.empty())	{
//std::cout << "StdLevelView::drawStep redraw animated tiles! gameTick=" << m_refLevel->game().gameElapsed() << " m_nViewTick=" << m_nViewTick << '\n';
		for (auto it = m_oTickTileAnis.cbegin(); it != m_oTickTileAnis.end(); ++it) {
			const int64_t nXY = *it;
			const NPoint oXY = Util::unpackPointFromInt64(nXY);
			const int32_t nX = oXY.m_nX;
			const int32_t nY = oXY.m_nY;
//std::cout << "StdLevelView::drawStep    m_oTickTileAnis                    nX=" << nX << " nY=" << nY << '\n';
			m_refBoardCc->save();
			m_refBoardCc->set_operator(Cairo::OPERATOR_SOURCE);
			m_refBoardCc->set_source_rgba(0, 0, 0, 0);
			m_refBoardCc->rectangle(nX * m_nTileW, nY * m_nTileH, m_nTileW, m_nTileH);
			m_refBoardCc->fill();
			drawBoard(m_refBoardCc, nX, nY, 1, 1, nViewTick, nTotViewTicks);
			m_refBoardCc->restore();
		}
	}
	// Start ready animations
	auto it = m_aAniDataNotStarted.begin();
	while (it != m_aAniDataNotStarted.end()) {
		AniData* p0AniData = it->get();
		assert(p0AniData != nullptr);
		bool bStartNow = true;
		//int32_t nZ;
		if (p0AniData->m_p0LevelBlock == nullptr) {
			shared_ptr<ThemeAnimation>& refAni = p0AniData->m_refAnimation;
			bStartNow = refAni->isStarted(nViewTick, nTotViewTicks);
//std::cout << "StdLevelView::drawStepToBuffers bStartNow=" << bStartNow << '\n';
			//if (bStartNow) {
			//	nZ = refAni->getZ(nViewTick, nTotViewTicks);
			//}
		//} else {
		//	nZ = p0AniData->m_p0LevelBlock->blockPosZ(nViewTick, nTotViewTicks);
		}
		if (bStartNow) {
//std::cout << "StdLevelView::drawStepToBuffers starting animation/levelblock andData=" << reinterpret_cast<int64_t>(p0AniData) << '\n';
			//p0AniData->m_nZ = nZ;
			//addAniDataToZObjects(*it, nZ);
			if (p0AniData->m_eRefSys >= LevelAnimation::REFSYS_SUBSHOW) {
				const int32_t nLevelPlayer = static_cast<int32_t>(p0AniData->m_eRefSys);
				assert((nLevelPlayer >= 0) && (nLevelPlayer < static_cast<int32_t>(m_aSubshowData.size())));
				m_aSubshowData[nLevelPlayer]->m_aAniDataSubshow.push_back({});
				std::swap(m_aSubshowData[nLevelPlayer]->m_aAniDataSubshow.back(), *it);
			} else {
				m_aAniDataNonSubshow.push_back({});
				std::swap(m_aAniDataNonSubshow.back(), *it);
			}
			assert(it->get() == nullptr);
			// swap last to current (they can be the same!)
			std::swap(*it, m_aAniDataNotStarted.back());
			m_aAniDataNotStarted.pop_back();
		} else {
			++it;
		}
	}
	// Draw Show area
	// sort show and board relative animations by z
	std::sort(m_aAniDataNonSubshow.begin(), m_aAniDataNonSubshow.end()
			, [&](const std::unique_ptr<AniData>& refL, const std::unique_ptr<AniData>& refR)
			{
				const int32_t nZL = refL->getZ(nViewTick, nTotViewTicks);
				const int32_t nZR = refR->getZ(nViewTick, nTotViewTicks);
				return (nZL < nZR);
			});
	if (m_bSubshows) {
		// clear the show area
		m_refShowCc->save();
		m_refShowCc->set_operator(Cairo::OPERATOR_SOURCE);
		m_refShowCc->rectangle(0, 0, m_nShowSurfPixW, m_nShowSurfPixH);
		m_refShowCc->set_source_rgba(0, 0, 0, 0);
		m_refShowCc->fill();
		m_refShowCc->restore();
	} else {
		// draw the widget background
		assert(m_refLevelShowTW);
		const NPoint oPos = m_refLevelShowTW->getPos();
		const NPoint oCanvasPos = m_refLevelShowTW->getCanvasPixPos();
		m_refShowCc->save();
		m_refShowCc->translate(- (oPos.m_nX + oCanvasPos.m_nX), - (oPos.m_nY + oCanvasPos.m_nY));
		m_refLevelShowTW->drawDeep(m_refShowCc);
		m_refShowCc->restore();
	}
	const bool bCheckSounds = (nViewTick == 0) && m_bSoundEnabled;
	const auto oShowPos = m_refLevel->showGet().getPos(nViewTick, nTotViewTicks);
	{
	//
	if (bCheckSounds && (! m_bSubshows) && (m_p0StdView->m_nTotAbsActiveSounds > 0)) {
		setSoundListenersToShowCenter(oShowPos);
	}
	const double fShowPosX = oShowPos.m_fX * m_nTileW;
	const double fShowPosY = oShowPos.m_fY * m_nTileH;
//std::cout << "StdLevelView::drawStepToBuffers  oShowPos.m_fX=" << oShowPos.m_fX << "  oShowPos.m_fY=" << oShowPos.m_fY << '\n';
	bool bBoardDrawn = false;
	auto itAniData = m_aAniDataNonSubshow.begin();
	while (true) {
		const bool bEnd = (itAniData == m_aAniDataNonSubshow.end());
		if ((!bBoardDrawn) && (bEnd || ((*itAniData)->getZ(nViewTick, nTotViewTicks) >= 0))) {
			// draw Board
			m_refShowCc->save();
			m_refShowCc->set_operator(Cairo::OPERATOR_OVER);
			m_refShowCc->set_source(m_refBoardSurf, - fShowPosX, - fShowPosY);
			m_refShowCc->paint();
			m_refShowCc->restore();
			bBoardDrawn = true;
		}
		if (bEnd) {
			break; //--------------
		}
		auto& refAniData = *itAniData;
		auto& oAniData = *refAniData;
		const bool bRemove = drawAniData(oAniData, m_refShowCc, fShowPosX, fShowPosY, nViewTick, nTotViewTicks);
		if (bRemove) {
//std::cout << "StdLevelView::drawStepToBuffers  drawAniData() remove" << '\n';
			itAniData = anidataRecycleKeepOrder(m_aAniDataNonSubshow, itAniData);
		} else {
			++itAniData;
		}
	}
	}
	if (m_bSubshows) {
		const int32_t nTotLevelPlayers = static_cast<int32_t>(m_aSubshowData.size());
		assert(nTotLevelPlayers > 0);
		const bool bSetSoundListenerPos = (bCheckSounds && (m_p0StdView->m_nTotAbsActiveSounds > 0));
		for (int32_t nLevelPlayer = 0; nLevelPlayer < nTotLevelPlayers; ++nLevelPlayer) {
			auto& oSubshowData = *(m_aSubshowData[nLevelPlayer]);
			auto& refSubshowCc = oSubshowData.m_refSubshowCc;
			const NPoint oPos = oSubshowData.m_refLSTW->getPos();
			const NPoint oCanvasPos = oSubshowData.m_refLSTW->getCanvasPixPos();
			refSubshowCc->save();
			//refSubshowCc->set_operator(Cairo::OPERATOR_SOURCE);
//std::cout << "drawStepToBuffers translate x:" << - (oPos.m_nX + oCanvasPos.m_nX) << "  y:" << - (oPos.m_nY + oCanvasPos.m_nY) << '\n';
			refSubshowCc->translate(- (oPos.m_nX + oCanvasPos.m_nX), - (oPos.m_nY + oCanvasPos.m_nY));
			oSubshowData.m_refLSTW->drawDeep(refSubshowCc);
			//refSubshowCc->translate(+ (oPos.m_nX + nCanvasPixX), + (oPos.m_nY + nCanvasPixY));
			refSubshowCc->restore();
			//
			const auto oSubshowPos = m_refLevel->subshowGet(nLevelPlayer).getPos(nViewTick, nTotViewTicks);
			if (bSetSoundListenerPos) {
				setSoundListenerToSubshowCenter(nLevelPlayer, oShowPos, oSubshowPos);
			}
			const double fSubshowPosX = oSubshowPos.m_fX * m_nTileW;
			const double fSubshowPosY = oSubshowPos.m_fY * m_nTileH;
//std::cout << "drawStepToBuffers (subshow) nLevelPlayer=" << nLevelPlayer << " getPos.x:" << oSubshowPos.m_fX << "  getPos.y:" << - oSubshowPos.m_fY << '\n';
			refSubshowCc->save();
			refSubshowCc->set_operator(Cairo::OPERATOR_OVER);
			refSubshowCc->set_source(m_refShowSurf, - fSubshowPosX, - fSubshowPosY);
			refSubshowCc->paint();
			refSubshowCc->restore();

			auto& aAniData = oSubshowData.m_aAniDataSubshow;
			std::sort(aAniData.begin(), aAniData.end()
					, [&](const std::unique_ptr<AniData>& refL, const std::unique_ptr<AniData>& refR)
					{
						const int32_t nZL = refL->getZ(nViewTick, nTotViewTicks);
						const int32_t nZR = refR->getZ(nViewTick, nTotViewTicks);
						return (nZL < nZR);
					});
			auto itAniData = aAniData.begin();
			while (itAniData != aAniData.end()) {
				auto& refAniData = *itAniData;
				auto& oAniData = *refAniData;
				const bool bRemove = drawAniData(oAniData, refSubshowCc, fSubshowPosX, fSubshowPosY, nViewTick, nTotViewTicks);
				if (bRemove) {
					itAniData = anidataRecycleKeepOrder(aAniData, itAniData);
					//m_aAniDataRecycle.emplace_back(refAniData);
					//assert(!refAniData);
					////std::unique_ptr<AniData> refSave = std::move(*itAniData);
					//itAniData = aAniData.erase(itAniData);
					////TODO inform ThemeAnimation of removal refAniCall->onRemove() or ThemeCtx->remove(refAniCall)
					//m_refLevel->animationRemove(m_aAniDataRecycle.back()->m_refLevelAnimation);
				} else {
					++itAniData;
				}
			}
		}
	}
}
void StdLevelView::drawBuffers(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	if (!m_bSubshows) {
		const NPoint oShowPos = m_refLevelShowTW->getPos();
		const NPoint oCanvasPos = m_refLevelShowTW->getCanvasPixPos();
//std::cout << "StdLevelView::drawBuffers nShowPixX=" << nShowPixX << " nShowPixY=" << nShowPixY << " m_nShowSurfPixW=" << m_nShowSurfPixW << " m_nShowSurfPixH=" << m_nShowSurfPixH << '\n';
//std::cout << "                          m_refShowSurf->get_width()=" << m_refShowSurf->get_width() << " m_refShowSurf->get_height()=" << m_refShowSurf->get_height() << '\n';
		refCc->save();
		refCc->translate(oShowPos.m_nX + oCanvasPos.m_nX, oShowPos.m_nY + oCanvasPos.m_nY);
		refCc->rectangle(0, 0, m_nShowSurfPixW, m_nShowSurfPixH); // needed because surf size could be bigger
		refCc->clip();
		refCc->set_source(m_refShowSurf, 0, 0);
		refCc->set_operator(Cairo::OPERATOR_SOURCE);
		refCc->paint();
		refCc->restore();
	} else {
		const int32_t nTotLevelPlayers = static_cast<int32_t>(m_aSubshowData.size());
		assert(nTotLevelPlayers > 0);
		for (int32_t nLevelPlayer = 0; nLevelPlayer < nTotLevelPlayers; ++nLevelPlayer) {
			auto& oSubshowData = *(m_aSubshowData[nLevelPlayer]);
			const NPoint oSubshowPos = oSubshowData.m_refLSTW->getPos();
			const NPoint oCanvasPos = oSubshowData.m_refLSTW->getCanvasPixPos();
			refCc->save();
			refCc->translate(oSubshowPos.m_nX + oCanvasPos.m_nX, oSubshowPos.m_nY + oCanvasPos.m_nY);
			refCc->rectangle(0, 0, m_nSubshowSurfPixW, m_nSubshowSurfPixH); // needed because surf size could be bigger
			refCc->clip();
			refCc->set_source(oSubshowData.m_refSubshowSurf, 0, 0);
			refCc->set_operator(Cairo::OPERATOR_SOURCE);
			refCc->paint();
			refCc->restore();
		}
	}
}
bool StdLevelView::drawAniData(AniData& oAniData, const Cairo::RefPtr<Cairo::Context>& refCc
								, double fShowPixX, double fShowPixY
								, int32_t nViewTick, int32_t nTotViewTicks) noexcept
{
	if (oAniData.m_p0LevelBlock != nullptr) {
		refCc->translate(- fShowPixX, - fShowPixY);
		drawLevelBlock(refCc, *(oAniData.m_p0LevelBlock), nViewTick, nTotViewTicks);
		refCc->translate(+ fShowPixX, + fShowPixY);
		return false; //--------------------------------------------------------
	}
	shared_ptr<ThemeAnimation>& refAni = oAniData.m_refAnimation;
	assert(refAni);
	const bool bDone = refAni->isDone(nViewTick, nTotViewTicks);
	if (bDone) {
		return true; //--------------------------------------------------------
	}
	const auto eRefSys = oAniData.m_eRefSys;
	if (eRefSys == LevelAnimation::REFSYS_BOARD) {
		refCc->translate(- fShowPixX, - fShowPixY);
	}
//if (nViewTick == 0) {
//std::cout << "StdLevelView::drawAniData  refAni=" << typeid(refAni.get()).name() << '\n';
//}
	refAni->draw(nViewTick, nTotViewTicks, refCc);
	if (eRefSys == LevelAnimation::REFSYS_BOARD) {
		refCc->translate(+ fShowPixX, + fShowPixY);
	}
	return false;
}
std::unique_ptr<StdLevelView::AniData> StdLevelView::anidataRecycle() noexcept
{
	const bool bRecycleEmpty = m_aAniDataRecycle.empty();
	std::unique_ptr<AniData> refAniData = (bRecycleEmpty
			? std::make_unique<AniData>()
			: std::unique_ptr<AniData>(m_aAniDataRecycle.back().release()));
	if (bRecycleEmpty) {
		refAniData->m_p0LevelBlock = nullptr;
	} else {
		assert(!m_aAniDataRecycle.back());
		m_aAniDataRecycle.pop_back();
	}
	assert(refAniData);
	return refAniData;
}
std::vector< std::unique_ptr<StdLevelView::AniData> >::iterator
		StdLevelView::anidataRecycleKeepOrder(std::vector< std::unique_ptr<AniData> >& aAniData
											, const std::vector< std::unique_ptr<AniData> >::iterator& itAniData) noexcept
{
	auto& refAniData = *itAniData;
	m_aAniDataRecycle.push_back({});
	std::swap(m_aAniDataRecycle.back(), refAniData);
	assert(!refAniData);
	//
	auto itNextAniData = aAniData.erase(itAniData);
	//
	auto& refAniDataR = m_aAniDataRecycle.back();
	assert(refAniDataR);
	if (refAniDataR->m_p0LevelBlock == nullptr) {
		//TODO inform ThemeAnimation of removal refAniCall->onRemove() or ThemeCtx->remove(refAniCall)
		m_refLevel->animationRemove(refAniDataR->m_refLevelAnimation);
		refAniDataR->m_refLevelAnimation.reset();
		refAniDataR->m_refAnimation.reset();
	} else { // it's a level block
		assert(!refAniDataR->m_refLevelAnimation);
		assert(!refAniDataR->m_refAnimation);
		refAniDataR->m_p0LevelBlock = nullptr; //
	}
	return itNextAniData;
}
void StdLevelView::anidataRecycle(std::vector< std::unique_ptr<AniData> >& aAniData
								, const std::vector< std::unique_ptr<AniData> >::iterator& itAniData) noexcept
{
	auto& refAniData = *itAniData;
	m_aAniDataRecycle.push_back({});
	std::swap(m_aAniDataRecycle.back(), refAniData);

	assert(!refAniData);
	//
	if (aAniData.size() > 1) {
		std::swap(aAniData.back(), refAniData);
	}
	aAniData.pop_back();
	//
	auto& refAniDataR = m_aAniDataRecycle.back();
	assert(refAniDataR);
	if (refAniDataR->m_p0LevelBlock == nullptr) {
		refAniDataR->m_refLevelAnimation.reset();
		refAniDataR->m_refAnimation.reset();
	} else {
		assert(!refAniDataR->m_refLevelAnimation);
		assert(!refAniDataR->m_refAnimation);
		refAniDataR->m_p0LevelBlock = nullptr;
	}
}
void StdLevelView::drawBoard(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	drawBoard(refCc, 0, 0, m_nBoardW, m_nBoardH, 0, 1);
}
void StdLevelView::drawBoard(const Cairo::RefPtr<Cairo::Context>& refCc, int32_t nX, int32_t nY, int32_t nW, int32_t nH
						, int32_t nViewTick, int32_t nTotViewTicks) noexcept
{
	assert((nViewTick >= 0) && (nViewTick < nTotViewTicks));

	const int32_t nTotTileAni = m_nThemeTotTileAnis;
	m_aTileAniElapsed.resize(nTotTileAni);

	const int32_t nTileW = m_nTileW;
	const int32_t nTileH = m_nTileH;
	const int32_t nAreaPixH = nTileH * nH;
	refCc->translate(nX * nTileW, nY * nTileH);
	for (int32_t nCurX = nX; nCurX < nX+nW; ++nCurX) {
		for (int32_t nCurY = nY; nCurY < nY+nH; ++nCurY) {
			const Tile& oTile = m_refLevel->boardGetTile(nCurX, nCurY);
//std::cout << "StdLevelView::drawBoard nX=" << nCurX << " nY=" << nCurY << "   "; m_refLevel->dumpTile(oTile);
			if (!oTile.isEmpty()) {
				for (int32_t nIdxAni = 0; nIdxAni < nTotTileAni; ++nIdxAni) {
					const double fElapsed = m_refLevel->boardGetTileAniElapsed(nCurX, nCurY, nIdxAni, nViewTick, nTotViewTicks);
					m_aTileAniElapsed[nIdxAni] = fElapsed;
				}
				m_refThemeCtx->drawBoardTile(refCc, oTile, -1, m_aTileAniElapsed);
			}
			refCc->translate(0, nTileH);
		}
		refCc->translate(nTileW, -nAreaPixH);
	}
}
void StdLevelView::drawLevelBlock(const Cairo::RefPtr<Cairo::Context>& refCc, LevelBlock& oLevelBlock
								, int32_t nViewTick, int32_t nTotViewTicks) noexcept
{
//std::cout << "StdLevelView::drawLevelBlock" << '\n';
	const int32_t nTotTileAni = m_nThemeTotTileAnis;
	m_aTileAniElapsed.resize(nTotTileAni);

	const int32_t nPlayer = oLevelBlock.getPlayer();
	assert(nPlayer >= -1);

	const int32_t nTileW = m_nTileW;
	const int32_t nTileH = m_nTileH;
	const FPoint oPos = oLevelBlock.blockVTPos(nViewTick, nTotViewTicks);
	const std::vector<int32_t>& aBrickId = oLevelBlock.blockVTBrickIds(nViewTick, nTotViewTicks);
	for (auto& nBrickId : aBrickId) {
		if (oLevelBlock.blockVTBrickVisible(nViewTick, nTotViewTicks, nBrickId)) {
			for (int32_t nIdxAni = 0; nIdxAni < nTotTileAni; ++nIdxAni) {
				const double fElapsed = oLevelBlock.blockGetTileAniElapsed(nBrickId, nIdxAni, nViewTick, nTotViewTicks);
				m_aTileAniElapsed[nIdxAni] = fElapsed;
			}
			const NPoint oBrickRelPos = oLevelBlock.blockVTBrickPos(nViewTick, nTotViewTicks, nBrickId);
			const double fBrickPosX = oPos.m_fX + oBrickRelPos.m_nX;
			const double fBrickPosY = oPos.m_fY + oBrickRelPos.m_nY;
			const int32_t nPixX = fBrickPosX * nTileW;
			const int32_t nPixY = fBrickPosY * nTileH;
			const Tile& oTile =  oLevelBlock.blockVTBrickTile(nViewTick, nTotViewTicks, nBrickId);
//std::cout << "StdLevelView::drawLevelBlock nPixX=" << nPixX << " nPixY=" << nPixY << '\n';
			refCc->translate(nPixX, nPixY);
			m_refThemeCtx->drawBlockTile(refCc, oTile, nPlayer, m_aTileAniElapsed);
			refCc->translate(-nPixX, -nPixY);
		}
	}
}
void StdLevelView::boabloPreFreeze(LevelBlock& oLevelBlock) noexcept
{
	blockPostRemove(oLevelBlock);
}
void StdLevelView::boabloPostFreeze(const Coords& oCoords) noexcept
{
	for (Coords::const_iterator it = oCoords.begin(); it != oCoords.end(); it.next()) {
		const int32_t nX = it.x();
		const int32_t nY = it.y();
		redrawBoardPos(nX, nY);
	}
}
void StdLevelView::boabloPreUnfreeze(const Coords& /*oCoords*/) noexcept
{
}
void StdLevelView::boabloPostUnfreeze(LevelBlock& oLevelBlock) noexcept
{
	const NPoint oPos = oLevelBlock.blockPos();
	const int32_t nPosX = oPos.m_nX;
	const int32_t nPosY = oPos.m_nY;
	const std::vector<int32_t>& aBrickId = oLevelBlock.blockBrickIds();
	for (auto& nBrickId : aBrickId) {
		if (oLevelBlock.blockBrickVisible(nBrickId)) {
			const NPoint oBrickPos = oLevelBlock.blockBrickPos(nBrickId);
			const int32_t nX = nPosX + oBrickPos.m_nX;
			const int32_t nY = nPosY + oBrickPos.m_nY;
			redrawBoardPos(nX, nY);
		}
	}
	blockPostAdd(oLevelBlock);
}
void StdLevelView::blockPreAdd(const LevelBlock& /*oBlock*/) noexcept
{
}
void StdLevelView::blockPostAdd(LevelBlock& oBlock) noexcept
{
//std::cout << "StdLevelView::blockPostAdd" << '\n';
	auto refAniData = anidataRecycle();
	//const bool bRecycleEmpty = m_aAniDataRecycle.empty();
	//std::unique_ptr<AniData> refAniData = (bRecycleEmpty
	//		? std::make_unique<AniData>()
	//		: std::make_unique<AniData>(m_aAniDataRecycle.back().release()));
	//if (!bRecycleEmpty) {
	//	assert(!m_aAniDataRecycle.back());
	//	m_aAniDataRecycle.pop_back();
	//}
	//assert(refAniData);
	assert(!(refAniData->m_refAnimation));
	assert(!(refAniData->m_refLevelAnimation));
	refAniData->m_eRefSys = LevelAnimation::REFSYS_BOARD;
	//refAniData->m_nZ = std::numeric_limits<int32_t>::max();
	refAniData->m_p0LevelBlock = &oBlock;
	m_aAniDataNonSubshow.push_back({});
	std::swap(m_aAniDataNonSubshow.back(), refAniData);
}
void StdLevelView::blockPreRemove(LevelBlock& /*oBlock*/) noexcept
{
}
void StdLevelView::blockPostRemove(const LevelBlock& oBlock) noexcept
{
//std::cout << "StdLevelView::blockPostRemove" << '\n';
	auto itFind = std::find_if(m_aAniDataNonSubshow.begin(), m_aAniDataNonSubshow.end()
			, [&](const std::unique_ptr<AniData>& refAniData)
			{
				return (refAniData->m_p0LevelBlock == &oBlock);
			});
	if (itFind == m_aAniDataNonSubshow.end()) {
		auto itFindNS = std::find_if(m_aAniDataNotStarted.begin(), m_aAniDataNotStarted.end()
					, [&](const std::unique_ptr<AniData>& refAniData)
					{
						return (refAniData->m_p0LevelBlock == &oBlock);
					});
		if (itFindNS == m_aAniDataNotStarted.end()) {
			assert(false);
			return; //----------------------------------------------------------
		}
		anidataRecycle(m_aAniDataNotStarted, itFindNS);
		//auto& refAniData = *itFindNS;
		//m_aAniDataRecycle.emplace_back(refAniData);
		//m_aAniDataNotStarted.erase(itFindNS);
		//assert(!refAniData->m_refLevelAnimation);
		//assert(!refAniData->m_refAnimation);
		//refAniData->m_p0LevelBlock = nullptr;
		return; //--------------------------------------------------------------
	} else {
		anidataRecycle(m_aAniDataNonSubshow, itFind);
	}
	//auto& refAniData = *itFind;
	//m_aAniDataRecycle.emplace_back(refAniData);
	//m_aAniDataNonSubshow.erase(itFind);
	//assert(!refAniData->m_refLevelAnimation);
	//assert(!refAniData->m_refAnimation);
	//refAniData->m_p0LevelBlock = nullptr;
}
void StdLevelView::blockPreDestroy(LevelBlock& /*oBlock*/) noexcept
{
}
void StdLevelView::blockPostDestroy(const LevelBlock& oBlock) noexcept
{
	blockPostRemove(oBlock);
}
void StdLevelView::blockPreFuse(LevelBlock& /*oFusedToBlock*/, LevelBlock& /*oFusedBlock*/) noexcept
{
}
void StdLevelView::blockPostFuse(LevelBlock& /*oResBlock*/, const LevelBlock& oFusedBlock
					, const std::unordered_map<int32_t, int32_t>& /*oFusedToBrickIds*/
					, const std::unordered_map<int32_t, int32_t>& /*oFusedBrickIds*/) noexcept
{
	blockPostRemove(oFusedBlock);
}

void StdLevelView::boardAnimateTiles(NRect oArea) noexcept
{
//std::cout << "StdLevelView::boardAnimateTiles  nX=" << nX << "  nY=" << nY << "   nW=" << nW << "  nH=" << nH << '\n';
	const int32_t nToNX = oArea.m_nX + oArea.m_nW;
	const int32_t nToNY = oArea.m_nY + oArea.m_nH;
	for (int32_t nCurX = oArea.m_nX; nCurX < nToNX; ++nCurX) {
		for (int32_t nCurY = oArea.m_nY; nCurY < nToNY; ++nCurY) {
			const int64_t nCurXY = Util::packPointToInt64(NPoint{nCurX, nCurY});
			m_oTickTileAnis.insert(nCurXY);
		}
	}
}
void StdLevelView::boardAnimateTile(NPoint oXY) noexcept
{
	const int64_t nXY = Util::packPointToInt64(NPoint{oXY.m_nX, oXY.m_nY});
	m_oTickTileAnis.insert(nXY);
}
void StdLevelView::reMoveTickAnimatedTiles(int32_t nRemoveX, int32_t nRemoveY, int32_t nRemoveW, int32_t nRemoveH
										, int32_t nAreaX, int32_t nAreaY, int32_t nAreaW, int32_t nAreaH
										, int32_t nDx, int32_t nDy) noexcept
{
	const int32_t nRemoveNToX = nRemoveX + nRemoveW;
	const int32_t nRemoveNToY = nRemoveY + nRemoveH;
	const int32_t nAreaNToX = nAreaX + nAreaW;
	const int32_t nAreaNToY = nAreaY + nAreaH;
	m_oTickTileAnisWork.clear();
	for (auto& nXY : m_oTickTileAnis) {
		const NPoint oXY = Util::unpackPointFromInt64(nXY);
		const int32_t nX = oXY.m_nX;
		const int32_t nY = oXY.m_nY;
		if ((nX >= nRemoveX) && (nX < nRemoveNToX) && (nY >= nRemoveY) && (nY < nRemoveNToY)) {
			continue; // for
		}
		if ((nX >= nAreaX) && (nX < nAreaNToX) && (nY >= nAreaY) && (nY < nAreaNToY)) {
			const int32_t nNewX = nX + nDx;
			const int32_t nNewY = nY + nDy;
			assert((nNewX >= 0) && (nNewX < m_nBoardW));
			assert((nNewY >= 0) && (nNewY < m_nBoardH));
			const int64_t nNewXY = Util::packPointToInt64(NPoint{nNewX, nNewY});
			m_oTickTileAnisWork.insert(nNewXY);
		} else {
			m_oTickTileAnisWork.insert(nXY);
		}
	}
	m_oTickTileAnisWork.swap(m_oTickTileAnis);
}

void StdLevelView::boardPreScroll(Direction::VALUE /*eDir*/, const shared_ptr<TileRect>& /*refTiles*/) noexcept
{
}
void StdLevelView::boardPostScroll(Direction::VALUE eDir) noexcept
{
	NRect oArea;
	oArea.m_nW = m_nBoardW;
	oArea.m_nH = m_nBoardH;
	boardPostInsert(eDir, oArea);
}
void StdLevelView::boardPreInsert(Direction::VALUE /*eDir*/, NRect /*oArea*/, const shared_ptr<TileRect>& /*refTiles*/) noexcept
{
}
void StdLevelView::boardPostInsert(Direction::VALUE eDir, NRect oArea) noexcept
{
	const int32_t nX = oArea.m_nX;
	const int32_t nY = oArea.m_nY;
	const int32_t nW = oArea.m_nW;
	const int32_t nH = oArea.m_nH;
	const int32_t nDx = Direction::deltaX(eDir);
	const int32_t nDy = Direction::deltaY(eDir);
//std::cout << "StdLevelView::boardPostInsert(" << nX << "," << nY << "," << nW << "," << nH << ")   nDx=" << nDx << " nDy=" << nDy << '\n';

	Cairo::RefPtr<Cairo::Context>& refCc = m_refBoard2Cc;

	boardInsertKeep(refCc, nX, nY, nW, nH);

	if ((eDir == Direction::DOWN) || (eDir == Direction::UP)) {
		if (eDir == Direction::DOWN) {
			boardInsertMove(refCc, nX, nY, nW, nH - 1, nDx, nDy);
		} else {
			boardInsertMove(refCc, nX, nY + 1, nW, nH - 1, nDx, nDy);
		}
		boardInsertRedrawAndRemove(refCc, nX, nY, nW, nH, nDx, nDy
									, nX, nY, nW, 1
									, nX, nY + nH - 1, nW, 1
									, (eDir == Direction::DOWN));
	} else if ((eDir == Direction::RIGHT) || (eDir == Direction::LEFT)) {
		if (eDir == Direction::RIGHT) {
			boardInsertMove(refCc, nX, nY, nW - 1, nH, nDx, nDy);
		} else {
			boardInsertMove(refCc, nX + 1, nY, nW - 1, nH, nDx, nDy);
		}
		boardInsertRedrawAndRemove(refCc, nX, nY, nW, nH, nDx, nDy
									, nX, nY, 1, nH
									, nX + nW + 1, nY, 1, nH
									, (eDir == Direction::RIGHT));
	} else {
		assert(false);
	}

	m_refBoardSurf.swap(m_refBoard2Surf);
	m_refBoardCc.swap(m_refBoard2Cc);
//m_refLevel->dump(true, true, false, false, false);
}

void StdLevelView::boardInsertKeep(const Cairo::RefPtr<Cairo::Context>& refCc
									, int32_t nX, int32_t nY, int32_t nW, int32_t nH) noexcept
{
	const int32_t nPixX = nX * m_nTileW;
	const int32_t nPixY = nY * m_nTileH;
	const int32_t nPixW = nW * m_nTileW;
	const int32_t nPixH = nH * m_nTileH;

	const int32_t nBoardPixW = m_nBoardW * m_nTileW;
	const int32_t nBoardPixH = m_nBoardH * m_nTileH;

	refCc->save();
	refCc->set_operator(Cairo::OPERATOR_SOURCE);
	if (nX > 0) { // keep left
//std::cout << "          boardInsertKeep  Pixel:(X,Y,W,H)=(" << 0 << "," << 0 << "," << nPixX << "," << nBoardPixH << ")" << '\n';
		refCc->save();
		refCc->rectangle(0, 0, nPixX, nBoardPixH);
		refCc->clip();
		refCc->set_source(m_refBoardSurf, 0, 0);
		refCc->paint();
		refCc->restore();
	}
	const int32_t nKeepRightPixW = nBoardPixW - (nPixX + nPixW);
	if (nKeepRightPixW > 0) { // keep right
//std::cout << "          boardInsertKeep  Pixel:(X,Y,W,H)=(" << nPixX + nPixW << "," << 0 << "," << nKeepRightPixW << "," << nBoardPixH << ")" << '\n';
		refCc->save();
		refCc->rectangle(nPixX + nPixW, 0, nKeepRightPixW, nBoardPixH);
		refCc->clip();
		refCc->set_source(m_refBoardSurf, 0, 0);
		refCc->paint();
		refCc->restore();
	}
	if (nY > 0) { // keep top
//std::cout << "          boardInsertKeep  Pixel:(X,Y,W,H)=(" << nPixX << "," << 0 << "," << nPixW << "," << nPixY << ")" << '\n';
		refCc->save();
		refCc->rectangle(nPixX, 0, nPixW, nPixY);
		refCc->clip();
		refCc->set_source(m_refBoardSurf, 0, 0);
		refCc->paint();
		refCc->restore();
	}
	const int32_t nKeepBottomPixH = nBoardPixH - (nPixY + nPixH);
	if (nKeepBottomPixH > 0) { // keep bottom
//std::cout << "          boardInsertKeep  Pixel:(X,Y,W,H)=(" << nPixX << "," << nPixY + nPixH << "," << nPixW << "," << nKeepBottomPixH << ")" << '\n';
		refCc->save();
		refCc->rectangle(nPixX, nPixY + nPixH, nPixW, nKeepBottomPixH);
		refCc->clip();
		refCc->set_source(m_refBoardSurf, 0, 0);
		refCc->paint();
		refCc->restore();
	}
	refCc->restore();
}
void StdLevelView::boardInsertMove(const Cairo::RefPtr<Cairo::Context>& refCc
						, int32_t nX, int32_t nY, int32_t nW, int32_t nH, int32_t nDx, int32_t nDy) noexcept
{
//std::cout << "StdLevelView::boardInsertMove" << '\n';
	const int32_t nPixX = nX * m_nTileW;
	const int32_t nPixY = nY * m_nTileH;
	const int32_t nPixW = nW * m_nTileW;
	const int32_t nPixH = nH * m_nTileH;

	const int32_t nPixDx = nDx * m_nTileW;
	const int32_t nPixDy = nDy * m_nTileH;

	refCc->save();
	refCc->set_operator(Cairo::OPERATOR_SOURCE);

//std::cout << "          boardInsertMove  Pixel:(X,Y,W,H) + (dX,dY)=(" << nPixX + nPixDx << "," <<  nPixY + nPixDy << "," << nPixW << "," << nPixH << ") + (" << nPixDx << "," << nPixDy << ")" << '\n';
	refCc->rectangle(nPixX + nPixDx, nPixY + nPixDy, nPixW, nPixH); // destination rectangle
	refCc->clip();
	refCc->set_source(m_refBoardSurf, +nPixDx, +nPixDy);
	refCc->paint();

	refCc->restore();
}
void StdLevelView::boardInsertRedrawAndRemove(const Cairo::RefPtr<Cairo::Context>& refCc
										, int32_t nX, int32_t nY, int32_t nW, int32_t nH, int32_t nDx, int32_t nDy
										, int32_t nX1, int32_t nY1, int32_t nW1, int32_t nH1
										, int32_t nX2, int32_t nY2, int32_t nW2, int32_t nH2
										, bool bRedraw1) noexcept
{
	const int32_t nPixX1 = nX1 * m_nTileW;
	const int32_t nPixY1 = nY1 * m_nTileH;
	const int32_t nPixW1 = nW1 * m_nTileW;
	const int32_t nPixH1 = nH1 * m_nTileH;

	const int32_t nPixX2 = nX2 * m_nTileW;
	const int32_t nPixY2 = nY2 * m_nTileH;
	const int32_t nPixW2 = nW2 * m_nTileW;
	const int32_t nPixH2 = nH2 * m_nTileH;

	refCc->save();
	refCc->set_operator(Cairo::OPERATOR_SOURCE);

	if (bRedraw1) {
		refCc->rectangle(nPixX1, nPixY1, nPixW1, nPixH1);
//std::cout << "          boardInsertRedrawAndRemove  Clear Pixel:(X,Y,W,H)=(" << nPixX1 << "," <<  nPixY1 << "," << nPixW1 << "," << nPixH1 << ")" << '\n';
	} else {
		refCc->rectangle(nPixX2, nPixY2, nPixW2, nPixH2);
//std::cout << "          boardInsertRedrawAndRemove  Clear Pixel:(X,Y,W,H)=(" << nPixX2 << "," <<  nPixY2 << "," << nPixW2 << "," << nPixH2 << ")" << '\n';
	}
	refCc->set_source_rgba(0, 0, 0, 0);
	refCc->fill();

	if (bRedraw1) {
		drawBoard(refCc, nX1, nY1, nW1, nH1, 0, 1);
	} else {
		drawBoard(refCc, nX2, nY2, nW2, nH2, 0, 1);
	}
	refCc->restore();

//#ifndef NDEBUG
//std::cout << "          boardInsertRedrawAndRemove before: "; dump(true);
//#endif //NDEBUG
	if (bRedraw1) {
		reMoveTickAnimatedTiles(nX2, nY2, nW2, nH2,  nX, nY, nW, nH,  nDx, nDy);
	} else {
		reMoveTickAnimatedTiles(nX1, nY1, nW1, nH1,  nX, nY, nW, nH,  nDx, nDy);
	}
//#ifndef NDEBUG
//std::cout << "          boardInsertRedrawAndRemove after: "; dump(true);
//#endif //NDEBUG
}

void StdLevelView::boardPreModify(const TileCoords& /*oTileCoords*/) noexcept
{
}
void StdLevelView::boardPostModify(const Coords& oCoords) noexcept
{
//std::cout << "StdLevelView::boardPostModify(" << oArea.m_nX << "," << oArea.m_nY << "," << oArea.m_nW << "," << oArea.m_nH << ")" << '\n';

	const auto itEnd = oCoords.end();
	for (auto it = oCoords.begin(); it != itEnd; it.next()) {
		redrawBoardPos(it.x(), it.y());
	}
}
void StdLevelView::redrawBoardPos(int32_t nX, int32_t nY) noexcept
{
	const int64_t nXY = Util::packPointToInt64(NPoint{nX, nY});
	if (m_oTickTileAnis.find(nXY) == m_oTickTileAnis.end()) {
		// only draw if it's not redrawn during view ticks
		const int32_t nPixX = nX * m_nTileW;
		const int32_t nPixY = nY * m_nTileH;
		const int32_t nPixW = 1 * m_nTileW;
		const int32_t nPixH = 1 * m_nTileH;

		Cairo::RefPtr<Cairo::Context>& refCc = m_refBoardCc;
		refCc->save();
		refCc->set_operator(Cairo::OPERATOR_SOURCE);

		refCc->rectangle(nPixX, nPixY, nPixW, nPixH);
		refCc->set_source_rgba(0, 0, 0, 0);
		refCc->fill();

		drawBoard(refCc, nX, nY, 1, 1, 0, 1);
		refCc->restore();
	}
}
void StdLevelView::boardPreDestroy(const Coords& oCoords) noexcept
{
	//TODO: An empty tile, whatever its tileAnis, is not drawn!?? Is this right?

	Cairo::RefPtr<Cairo::Context>& refCc = m_refBoardCc;
	refCc->save();
	refCc->set_operator(Cairo::OPERATOR_SOURCE);

	const int32_t nDestructAnimationIdx = m_p0StdView->m_nDestructBoardTileAnimation;
	for (Coords::const_iterator it = oCoords.begin(); it != oCoords.end(); it.next()) {
		const int32_t nX = it.x();
		const int32_t nY = it.y();
		const int32_t nPixX = nX * m_nTileW;
		const int32_t nPixY = nY * m_nTileH;
		const int32_t nPixW = 1 * m_nTileW;
		const int32_t nPixH = 1 * m_nTileH;
		const Tile& oTile = m_refLevel->boardGetTile(nX, nY);

		if (nDestructAnimationIdx >= 0) {
			shared_ptr<ExplosionAnimation> refBoardExplosion;
			ExplosionAnimation::Init oExplosionInit;
			oExplosionInit.m_nAnimationNamedIdx = nDestructAnimationIdx;
			//oExplosionInit.m_fDuration = 1000;
			oExplosionInit.m_oPos = {0.0 + nX, 0.0 + nY};
			oExplosionInit.m_oSize = {1.0, 1.0};
			oExplosionInit.m_nZ = s_nZObjectZBoardTileDestruct;
			oExplosionInit.m_oTile = oTile;
			oExplosionInit.m_nLevelPlayer = -1;
			s_oBoardDestructRecycler.create(refBoardExplosion, std::move(oExplosionInit));
			m_refLevel->animationAddScrolled(refBoardExplosion, 0.0);
//std::cout << "StdLevelView::boardPreDestroy nX=" << nX << "  nY=" << nY << '\n';
		}

		refCc->save();
		refCc->rectangle(nPixX, nPixY, nPixW, nPixH);
		refCc->set_source_rgba(0, 0, 0, 0);
		refCc->fill();
		refCc->restore();
	}
	refCc->restore();
}
void StdLevelView::boardPostDestroy(const Coords& /*oCoords*/) noexcept
{
}

bool StdLevelView::animationCreate(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refLevelAnimation);
	shared_ptr<ThemeAnimation> refAnimation = m_refThemeCtx->createAnimation(refLevelAnimation);
	if (!refAnimation) {
		return false; //--------------------------------------------------------
	}
	auto refAniData = anidataRecycle();
	assert(refAniData->m_p0LevelBlock == nullptr);

//std::cout << "StdLevelView::animationAdd  id=" << refLevelAnimation->getId() << " &(*refAnimation)=" << reinterpret_cast<int64_t>(refAnimation.get()) << '\n';
	refAniData->m_eRefSys = refLevelAnimation->getRefSys();
	refAniData->m_refAnimation = refAnimation;
	refAniData->m_refLevelAnimation = refLevelAnimation;
	m_aAniDataNotStarted.push_back({});
	std::swap(m_aAniDataNotStarted.back(), refAniData);
	return true;
}
bool StdLevelView::animationRemove(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
//std::cout << "StdLevelView::animationRemove  id=" << refLevelAnimation->getId() << '\n';
	assert(refLevelAnimation);
	const int32_t nLevelPlayer = static_cast<int32_t>(refLevelAnimation->getRefSys());
	const bool bSubshowAni = (nLevelPlayer >= 0);
	auto& aAniData = (bSubshowAni ? m_aSubshowData[nLevelPlayer]->m_aAniDataSubshow : m_aAniDataNonSubshow);
	auto itFind = std::find_if(aAniData.begin(), aAniData.end()
			, [&](const std::unique_ptr<AniData>& refAniData)
			{
				return (refAniData->m_refLevelAnimation == refLevelAnimation);
			});
	if (itFind == aAniData.end()) {
		auto itFindNS = std::find_if(m_aAniDataNotStarted.begin(), m_aAniDataNotStarted.end()
				, [&](const std::unique_ptr<AniData>& refAniData)
				{
					return (refAniData->m_refLevelAnimation == refLevelAnimation);
				});
		if (itFindNS == m_aAniDataNotStarted.end()) {
			// The animation already ended
			return false; //----------------------------------------------------
		}
		anidataRecycle(m_aAniDataNotStarted, itFindNS);
	} else {
		(*itFind)->m_refAnimation->onRemoved();
		anidataRecycle(aAniData, itFind);
	}
	return true;
}

#ifndef NDEBUG
void StdLevelView::dump(bool bTickTileAnis) const noexcept
{
	if (bTickTileAnis) {
		std::cout << "StdLevelView::dump()  TickTileAnis (nX,nY)" << '\n';
		for (auto& nXY : m_oTickTileAnis) {
			const NPoint oXY = Util::unpackPointFromInt64(nXY);
			const int32_t nX = oXY.m_nX;
			const int32_t nY = oXY.m_nY;
			std::cout << "    (" << nX << "," << nY << ")";
			std::cout << '\n';
		}
	}
}

#endif

} // namespace stmg
