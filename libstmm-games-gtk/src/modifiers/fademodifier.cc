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
 * File:   fademodifier.cc
 */

#include "modifiers/fademodifier.h"

#include <cairomm/context.h>
#include <cairomm/enums.h>
#include <cairomm/refptr.h>
#include <cairomm/surface.h>

#include <cassert>
//#include <iostream>
#include <utility>

namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }

namespace stmg
{

FadeModifier::FadeModifier(StdTheme* p1Owner, Init&& oInit) noexcept
: ContainerModifier(p1Owner)
, m_oData(std::move(oInit))
{
	assert(((m_oData.m_fDefaultElapsed >= 0.0) && (m_oData.m_fDefaultElapsed <= 1.0)) || (m_oData.m_fDefaultElapsed == -1.0));
	assert(m_oData.m_nElapsedTileAniIdx >= -1);
	assert((m_oData.m_nElapsedTileAniIdx >= 0) || (m_oData.m_fDefaultElapsed != -1.0));
}
StdThemeModifier::FLOW_CONTROL FadeModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
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
		// draw opaque
		const FLOW_CONTROL eCtl = ContainerModifier::drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
		return eCtl; //---------------------------------------------------------
	}
	const double fAlpha1 = (m_oData.m_bInvert ? fElapsed : (1.0 - fElapsed));
	if (fAlpha1 == 0.0) {
		// transparent, draw nothing
		return FLOW_CONTROL_CONTINUE; //----------------------------------------
	} else if (fAlpha1 == 1.0) {
		// draw opaque
		const FLOW_CONTROL eCtl = ContainerModifier::drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
		return eCtl; //---------------------------------------------------------
	}
	Cairo::RefPtr<Cairo::Surface> refWork;
	const FLOW_CONTROL eCtl = drawContainedToWorkSurface(oDc, oTile, nPlayer, aAniElapsed, refWork);

	refCc->save();
	refCc->set_operator(Cairo::OPERATOR_OVER);
	refCc->set_source(refWork,0,0);

//std::cout << "FadeModifier::drawTile paint_with_alpha=" << fAlpha1 << '\n';
	refCc->paint_with_alpha(fAlpha1);

	refCc->restore();

	return eCtl;
}

} // namespace stmg
