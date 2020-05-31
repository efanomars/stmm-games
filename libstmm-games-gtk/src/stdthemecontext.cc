/*
 * File:   stdthemecontext.cc
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

#include "stdthemecontext.h"
#include "stdtheme.h"

#include <stmm-games/util/basictypes.h>

#include <cassert>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace stmg { class ThemeSound; }
namespace stmi { class PlaybackCapability; }

namespace stmg { class LevelAnimation; }
namespace stmg { class ThemeAnimation; }
namespace stmg { class Tile; }

namespace stmg
{

StdThemeContext::StdThemeContext() noexcept
: m_p1Owner(nullptr)
, m_nTileW(-1)
, m_nTileH(-1)
, m_bRegistered(false)
{
	m_oDrawingContext.m_p1Owner = this;
}
void StdThemeContext::reInit() noexcept
{
	if (m_bRegistered) {
		assert(m_p1Owner != nullptr);
		m_p1Owner->unregisterTileSize(m_nTileW, m_nTileH);
	}
	m_p1Owner = nullptr;
	m_nTileW = -1;
	m_nTileH = -1;
	m_bRegistered = false;
	m_oDrawingContext.reInit();
	m_oDrawingContext.m_p1Owner = this;
}

void StdThemeContext::setTileSize(NSize oSize) noexcept
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return;
	}
	#endif //NDEBUG
	assert(oSize.m_nW > 0);
	assert(oSize.m_nH > 0);
	if ((oSize.m_nW == m_nTileW) && (oSize.m_nH == m_nTileH)) {
		return; //--------------------------------------------------------------
	}
	if (m_bRegistered) {
		m_p1Owner->unregisterTileSize(m_nTileW, m_nTileH);
	}
	m_nTileW = oSize.m_nW;
	m_nTileH = oSize.m_nH;
	if (m_bRegistered) {
		m_p1Owner->registerTileSize(m_nTileW, m_nTileH);
	}
}
const Glib::RefPtr<Pango::Context>& StdThemeContext::getFontContext() noexcept
{
	return m_refFontContext;
}
void StdThemeContext::drawBoardTile(const Cairo::RefPtr<Cairo::Context>& refCc, const Tile& oTile
									, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept
{
//std::cout << "StdThemeContext::drawTile()" << '\n';
	assert(m_p1Owner != nullptr);
	m_bDrawingBoardTile = true;
	m_p1Owner->drawBoardTile(refCc, *this, oTile, nPlayer, aAniElapsed);
}
void StdThemeContext::drawBoardTile(const Cairo::RefPtr<Cairo::Context>& refCc, const Tile& oTile
									, int32_t nPlayer) noexcept
{
//std::cout << "StdThemeContext::drawTile()" << '\n';
	assert(m_p1Owner != nullptr);
	m_bDrawingBoardTile = true;
	m_p1Owner->drawBoardTile(refCc, *this, oTile, nPlayer, m_p1Owner->m_aNoAniElapsed);
}
void StdThemeContext::drawBlockTile(const Cairo::RefPtr<Cairo::Context>& refCc, const Tile& oTile
									, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept
{
//std::cout << "StdThemeContext::drawTile()" << '\n';
	assert(m_p1Owner != nullptr);
	m_bDrawingBoardTile = false;
	m_p1Owner->drawBlockTile(refCc, *this, oTile, nPlayer, aAniElapsed);
}
void StdThemeContext::drawBlockTile(const Cairo::RefPtr<Cairo::Context>& refCc, const Tile& oTile
									, int32_t nPlayer) noexcept
{
//std::cout << "StdThemeContext::drawTile()" << '\n';
	assert(m_p1Owner != nullptr);
	m_bDrawingBoardTile = false;
	m_p1Owner->drawBlockTile(refCc, *this, oTile, nPlayer, m_p1Owner->m_aNoAniElapsed);
}
shared_ptr<ThemeAnimation> StdThemeContext::createAnimation(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(m_p1Owner != nullptr);
	shared_ptr<StdThemeContext> refSelf = shared_from_this();
	assert(! (refSelf.use_count() == 1));

	return m_p1Owner->createAnimation(refSelf, refLevelAnimation);
}

void StdThemeContext::preloadSound(int32_t nSoundIdx, const std::vector<shared_ptr<stmi::PlaybackCapability>>& aPlaybacks) noexcept
{
	assert(m_p1Owner != nullptr);

	m_p1Owner->preloadSound(nSoundIdx, aPlaybacks);
}
shared_ptr<ThemeSound> StdThemeContext::createSound(int32_t nSoundIdx, const std::vector<shared_ptr<stmi::PlaybackCapability>>& aPlaybacks
													, FPoint oXYPosition, double fZPosition, bool bRelative
													, double fVolume, bool bLoop) noexcept
{
	assert(m_p1Owner != nullptr);

	return m_p1Owner->createSound(this, nSoundIdx, aPlaybacks
								, oXYPosition, fZPosition, bRelative
								, fVolume, bLoop);
}

} // namespace stmg
