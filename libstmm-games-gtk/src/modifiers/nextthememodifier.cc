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
 * File:   nextthememodifier.cc
 */

#include "modifiers/nextthememodifier.h"
#include "stdtheme.h"

#include <cassert>
//#include <iostream>

namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }


namespace stmg
{

NextThemeModifier::NextThemeModifier(StdTheme* p1Owner, int32_t nThemeNr) noexcept
: StdThemeModifier(p1Owner)
, m_nThemeNr(nThemeNr)
, m_nPPOfNextTheme(0)
{
	assert(nThemeNr > 0);
}
StdThemeModifier::FLOW_CONTROL NextThemeModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
														, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept
{
	StdTheme* p0Theme = owner();
	assert(p0Theme != nullptr);
	if (m_nPPOfNextTheme == 0) {
		m_nPPOfNextTheme = p0Theme->getThemePP(m_nThemeNr, oDc);
	}
	if (m_nPPOfNextTheme > 0) {
		assert(owner() != nullptr);
		p0Theme->drawTileFromPP(m_nPPOfNextTheme, refCc, oDc, oTile, nPlayer, aAniElapsed);
	}
	return StdThemeModifier::FLOW_CONTROL_CONTINUE;
}

} // namespace stmg
