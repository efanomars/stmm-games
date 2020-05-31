/*
 * File:   alphamodifier.cc
 *
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
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

#include "modifiers/alphamodifier.h"

#include <cairomm/context.h>
#include <cairomm/enums.h>
#include <cairomm/refptr.h>
#include <cairomm/surface.h>

#include <stmm-games/tile.h>

namespace stmg { class StdThemeDrawingContext; }


namespace stmg
{

AlphaModifier::AlphaModifier(StdTheme* p1Owner) noexcept
: ContainerModifier(p1Owner)
{
}
StdThemeModifier::FLOW_CONTROL AlphaModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
												, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept
{
	const double fAlpha1 = oTile.getTileAlpha().getAlpha1();
//if (fAlpha1 < 1.0) {
//std::cout << "AlphaModifier::drawTile   fAlpha1=" << fAlpha1 << '\n';
//}
	Cairo::RefPtr<Cairo::Surface> refWork;
	const FLOW_CONTROL eCtl = drawContainedToWorkSurface(oDc, oTile, nPlayer, aAniElapsed, refWork);

	refCc->save();
	refCc->set_operator(Cairo::OPERATOR_OVER);
	refCc->set_source(refWork,0,0);

	refCc->paint_with_alpha(fAlpha1);

	refCc->restore();

	return eCtl;
}

} // namespace stmg
