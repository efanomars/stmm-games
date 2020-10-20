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
 * File:   imagemodifier.cc
 */

#include "modifiers/imagemodifier.h"
#include "gtkutil/image.h"

#include "stdthemedrawingcontext.h"

#include <stmm-games/util/basictypes.h>

#include <cassert>
//#include <iostream>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace stmg { class Tile; }


namespace stmg
{

ImageModifier::ImageModifier(StdTheme* p1Owner, const shared_ptr<Image>& refImage) noexcept
: StdThemeModifier(p1Owner)
, m_refImage(refImage)
{
	assert(m_refImage);
}
StdThemeModifier::FLOW_CONTROL ImageModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
														, const Tile& /*oTile*/, int32_t /*nPlayer*/, const std::vector<double>& /*aAniElapsed*/) noexcept
{
	const NSize oSize = oDc.getTileSize();
	const int32_t& nTileW = oSize.m_nW;
	const int32_t& nTileH = oSize.m_nH;
	m_refImage->draw(refCc, 0, 0, nTileW, nTileH);
	return FLOW_CONTROL_CONTINUE;
}
void ImageModifier::registerTileSize(int32_t nW, int32_t nH) noexcept
{
	m_refImage->addCachedSize(nW, nH);
}
void ImageModifier::unregisterTileSize(int32_t nW, int32_t nH) noexcept
{
	m_refImage->releaseCachedSize(nW, nH);
}

} // namespace stmg
