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
 * File:   growmodifier.cc
 */

#include "modifiers/growmodifier.h"

#include "stdthemedrawingcontext.h"

#include <stmm-games/util/basictypes.h>

#include <cairomm/context.h>
#include <cairomm/refptr.h>

#include <cassert>
#include <algorithm>
#include <utility>

namespace stmg { class Tile; }


namespace stmg
{

GrowModifier::GrowModifier(StdTheme* p1Owner, Init&& oInit) noexcept
: ContainerModifier(p1Owner)
, m_oData(std::move(oInit))
{
	assert(((m_oData.m_fDefaultElapsed >= 0.0) && (m_oData.m_fDefaultElapsed <= 1.0)) || (m_oData.m_fDefaultElapsed == -1.0));
	assert(m_oData.m_nElapsedTileAniIdx >= -1);
	assert((m_oData.m_nElapsedTileAniIdx >= 0) || (m_oData.m_fDefaultElapsed != -1.0));
}
void GrowModifier::growImageInRect(const Cairo::RefPtr<Cairo::Context>& refCc
						, int32_t nImgW, int32_t nImgH, int32_t nRectW, int32_t nRectH
						, double fFactor) noexcept
{
	const double fShrink = fFactor;
	const int32_t nMaxWH = std::max(nImgW, nImgH);
	{
		double fTransX = 0.5 * nRectW * (1 - fShrink);
		double fTransY = 0.5 * nRectH * (1 - fShrink);
		refCc->translate(fTransX, fTransY);
	}
	{
		const double fWScale = 1.0 * nRectW * fShrink / nMaxWH;
		const double fHScale = 1.0 * nRectH * fShrink / nMaxWH;
		refCc->scale(fWScale, fHScale);
	}
	{
		const double fTransX = 0.5 * nMaxWH;
		const double fTransY = 0.5 * nMaxWH;
		refCc->translate(fTransX, fTransY);
	}
	{
		const double fWScale = 1.0 * nMaxWH / nImgW;
		const double fHScale = 1.0 * nMaxWH / nImgH;
		refCc->scale(fWScale, fHScale);
	}
	{
		const double fTransX = - 0.5 * nImgW;
		const double fTransY = - 0.5 * nImgH;
		refCc->translate(fTransX, fTransY);
	}
}

StdThemeModifier::FLOW_CONTROL GrowModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
													, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept
{
	double fElapsed = -1.0;
	if (m_oData.m_nElapsedTileAniIdx < 0) {
		fElapsed = m_oData.m_fDefaultElapsed;
	} else {
		assert(m_oData.m_nElapsedTileAniIdx < static_cast<int32_t>(aAniElapsed.size()));
		fElapsed = aAniElapsed[m_oData.m_nElapsedTileAniIdx];
		if (fElapsed < 0.0) {
			fElapsed = m_oData.m_fDefaultElapsed;
		}
	}
	fElapsed = m_oData.m_oMapper.map(fElapsed);
	if (fElapsed < 0.0) {
		// draw normally
		const FLOW_CONTROL eCtl = ContainerModifier::drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
		return eCtl; //---------------------------------------------------------
	}
	if (m_oData.m_bInvert) {
		fElapsed = 1.0 - fElapsed;
	}
	if (fElapsed == 0.0) {
		// an invisible point, draw nothing
		return FLOW_CONTROL_CONTINUE; //----------------------------------------
	} else if (fElapsed == 1.0) {
		// draw normally
		const FLOW_CONTROL eCtl = ContainerModifier::drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
		return eCtl; //---------------------------------------------------------
	}
//std::cout << "GrowModifier::drawTile fElapsed=" << fElapsed << '\n';
	const NSize oSize = oDc.getTileSize();
	const int32_t& nW = oSize.m_nW;
	const int32_t& nH = oSize.m_nH;

	refCc->save();
	refCc->rectangle(0, 0, nW, nH);
	refCc->clip();

	growImageInRect(refCc, nW, nH, nW, nH, fElapsed);

	refCc->rectangle(0, 0, nW, nH);
	refCc->clip();

	const FLOW_CONTROL eCtl = ContainerModifier::drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);

	refCc->restore();

	return eCtl;
}

} // namespace stmg
