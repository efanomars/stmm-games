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
 * File:   fillmodifier.cc
 */

#include "modifiers/fillmodifier.h"
#include "stdtheme.h"
#include "stdthemedrawingcontext.h"

#include <stmm-games/tile.h>
#include <stmm-games/util/basictypes.h>

#include <cairomm/context.h>
#include <cairomm/refptr.h>

#include <cassert>
//#include <iostream>

#include <stdint.h>


namespace stmg
{

FillModifier::FillModifier(StdTheme* p1Owner, Init&& oInit) noexcept
: StdThemeModifier(p1Owner)
, m_fAlpha(TileAlpha::alphaUint8ToDouble1(static_cast<uint8_t>(oInit.m_nAlpha)))
{
	assert((m_fAlpha >= 0.0) && (m_fAlpha <= 1.0));
}
StdThemeModifier::FLOW_CONTROL FillModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
													, const Tile& oTile, int32_t /*nPlayer*/, const std::vector<double>& /*aAniElapsed*/) noexcept
{
	if (owner() == nullptr) {
		return FLOW_CONTROL_STOP;
	}
	const NSize oSize = oDc.getTileSize();
	const int32_t& nTileW = oSize.m_nW;
	const int32_t& nTileH = oSize.m_nH;
	double fR1, fG1, fB1;
	uint8_t nR, nG, nB;
	owner()->getColorRgb(oTile.getTileColor(), nR, nG, nB);
	fR1 = TileColor::colorUint8ToDouble1(nR);
	fG1 = TileColor::colorUint8ToDouble1(nG);
	fB1 = TileColor::colorUint8ToDouble1(nB);

	refCc->save();
	refCc->set_source_rgba(fR1,fG1,fB1, m_fAlpha);
	refCc->rectangle(0.0, 0.0, nTileW, nTileH);
	refCc->fill();
	refCc->restore();
	return FLOW_CONTROL_CONTINUE;
}

} // namespace stmg
