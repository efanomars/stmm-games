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
 * File:   selectedimagemodifier.cc
 */

#include "modifiers/selectedimagemodifier.h"

#include "stdthemedrawingcontext.h"
#include "gtkutil/image.h"

#include <stmm-games/util/basictypes.h>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace stmg { class Tile; }

namespace stmg
{

SelectedImageModifier::SelectedImageModifier(StdTheme* p1Owner) noexcept
: StdThemeModifier(p1Owner)
{
//std::cout << "            SelectedImageModifier::SelectedImageModifier" << '\n';
}
StdThemeModifier::FLOW_CONTROL SelectedImageModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
														, const Tile& /*oTile*/, int32_t /*nPlayer*/, const std::vector<double>& /*aAniElapsed*/) noexcept
{
	Image* p0Image = oDc.getSelectedImage();
	if (p0Image != nullptr) {
		const NSize oSize = oDc.getTileSize();
		const int32_t& nW = oSize.m_nW;
		const int32_t& nH = oSize.m_nH;
//std::cout << "            SelectedImageModifier::drawTile   nW=" << nW << "  nH=" << nH << '\n';
		p0Image->draw(refCc, 0,0, nW, nH);
	}
	return FLOW_CONTROL_CONTINUE;
}

} // namespace stmg
