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
 * File:   stopmodifier.cc
 */

#include "modifiers/stopmodifier.h"

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }


namespace stmg
{

StopModifier::StopModifier(StdTheme* p1Owner) noexcept
: StdThemeModifier(p1Owner)
{
}
StdThemeModifier::FLOW_CONTROL StopModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& /*refCc*/, StdThemeDrawingContext& /*oDc*/
														, const Tile& /*oTile*/, int32_t /*nPlayer*/, const std::vector<double>& /*aAniElapsed*/) noexcept
{
	return StdThemeModifier::FLOW_CONTROL_STOP;
}

} // namespace stmg
