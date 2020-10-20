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
 * File:   tileanimodifier.cc
 */

#include "modifiers/tileanimodifier.h"
#include "stdtheme.h"

#include "stdthemedrawingcontext.h"
#include "gtkutil/image.h"
#include "gtkutil/tileani.h"

#include <stmm-games/named.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/namedindex.h>

#include <cassert>
#include <utility>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace stmg { class Tile; }

namespace stmg
{

TileAniModifier::TileAniModifier(StdTheme* p1Owner, Init&& oInit) noexcept
: StdThemeModifier(p1Owner)
, m_oData(std::move(oInit))
{
	assert(m_oData.m_refTileAni || m_oData.m_refImg);
	assert((m_oData.m_nElapsedTileAniIdx >= -1) && (m_oData.m_nElapsedTileAniIdx < p1Owner->getNamed().tileAnis().size()));
	assert(((m_oData.m_fDefaultElapsed >= 0.0) && (m_oData.m_fDefaultElapsed <= 1.0)) || (m_oData.m_fDefaultElapsed == -1.0));
	if (! m_oData.m_refTileAni) {
		assert((!m_oData.m_bInvert) && (m_oData.m_fDefaultElapsed < 0.0) && (m_oData.m_nElapsedTileAniIdx < 0));
	} else if (!m_oData.m_refImg) {
		assert((m_oData.m_fDefaultElapsed >= 0.0) || (m_oData.m_nElapsedTileAniIdx >= 0) || m_oData.m_refTileAni->getDefaultImage());
		m_oData.m_refImg = m_oData.m_refTileAni->getDefaultImage();
	} else {
		assert((m_oData.m_fDefaultElapsed < 0.0) && (m_oData.m_nElapsedTileAniIdx >= 0));
	}
}

StdThemeModifier::FLOW_CONTROL TileAniModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
												, const Tile& /*oTile*/, int32_t /*nPlayer*/, const std::vector<double>& aAniElapsed) noexcept
{
	const auto oDrawImg = [&](const shared_ptr<Image>& refImage) -> FLOW_CONTROL
	{
		if (refImage) {
			const NSize oSize = oDc.getTileSize();
			const int32_t& nW = oSize.m_nW;
			const int32_t& nH = oSize.m_nH;
			refImage->draw(refCc, 0, 0, nW, nH);
		}
		return FLOW_CONTROL_CONTINUE;
	};

	double fElapsed = -1.0;
	if (m_oData.m_nElapsedTileAniIdx >= 0) {
		assert(m_oData.m_nElapsedTileAniIdx < static_cast<int32_t>(aAniElapsed.size()));
		fElapsed = aAniElapsed[m_oData.m_nElapsedTileAniIdx];
	}
	if (fElapsed < 0.0) {
		fElapsed = m_oData.m_fDefaultElapsed;
		if (fElapsed < 0.0) {
			const auto& refImg = m_oData.m_refImg;
			return oDrawImg(refImg); //-----------------------------------------
		}
	}
	if (m_oData.m_bInvert) {
		fElapsed = 1.0 - fElapsed;
	}
	return oDrawImg(m_oData.m_refTileAni->getImage(fElapsed));
}
void TileAniModifier::registerTileSize(int32_t nW, int32_t nH) noexcept
{
	if (m_oData.m_refImg) {
		m_oData.m_refImg->addCachedSize(nW, nH);
	}
	if (m_oData.m_refTileAni) {
		const int32_t nTotImgs = m_oData.m_refTileAni->getTotImages();
		for (int32_t nIdx = 0; nIdx < nTotImgs; ++nIdx) {
			m_oData.m_refTileAni->getImageByIdx(nIdx)->addCachedSize(nW, nH);
		}
	}
}
void TileAniModifier::unregisterTileSize(int32_t nW, int32_t nH) noexcept
{
	if (m_oData.m_refImg) {
		m_oData.m_refImg->releaseCachedSize(nW, nH);
	}
	if (m_oData.m_refTileAni) {
		const int32_t nTotImgs = m_oData.m_refTileAni->getTotImages();
		for (int32_t nIdx = 0; nIdx < nTotImgs; ++nIdx) {
			m_oData.m_refTileAni->getImageByIdx(nIdx)->releaseCachedSize(nW, nH);
		}
	}
}

} // namespace stmg
