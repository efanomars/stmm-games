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
 * File:   rotatemodifier.cc
 */

#include "modifiers/rotatemodifier.h"

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

static constexpr double s_fShrinkFactorWhileRotating = 0.8;

RotateModifier::RotateModifier(StdTheme* p1Owner, Init&& oInit) noexcept
: ContainerModifier(p1Owner)
, m_oData(std::move(oInit))
{
	assert(((m_oData.m_fDefaultElapsed >= 0.0) && (m_oData.m_fDefaultElapsed <= 1.0)) || (m_oData.m_fDefaultElapsed == -1.0));
	assert(m_oData.m_nElapsedTileAniIdx >= -1);
	assert((m_oData.m_nElapsedTileAniIdx >= 0) || (m_oData.m_fDefaultElapsed != -1.0));
}
void RotateModifier::rotateImageInRect(const Cairo::RefPtr<Cairo::Context>& refCc
						, int32_t nImgW, int32_t nImgH, int32_t nRectW, int32_t nRectH
						, double fShrink, double fAngleDeg) noexcept
{
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
		const double fRotate = fAngleDeg;
		refCc->rotate_degrees(fRotate);
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

StdThemeModifier::FLOW_CONTROL RotateModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
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
	if ((fElapsed <= 0.0) || (fElapsed == 1.0)) {
		// no rotation
		const FLOW_CONTROL eCtl = ContainerModifier::drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
		return eCtl; //---------------------------------------------------------
	}
	if (m_oData.m_bInvert) {
		fElapsed = 1.0 - fElapsed;
	}

	const NSize oSize = oDc.getTileSize();
	const int32_t& nW = oSize.m_nW;
	const int32_t& nH = oSize.m_nH;

	refCc->save();
	refCc->rectangle(0, 0, nW, nH);
	refCc->clip();

	rotateImageInRect(refCc, nW, nH, nW, nH, s_fShrinkFactorWhileRotating, fElapsed * 360);

	refCc->rectangle(0, 0, nW, nH);
	refCc->clip();

	const FLOW_CONTROL eCtl = ContainerModifier::drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);

	refCc->restore();

	return eCtl;
}

} // namespace stmg
