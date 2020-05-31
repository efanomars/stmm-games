/*
 * File:   nextthememodifier.h
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

#ifndef STMG_NEXT_THEME_MODIFIER_H
#define STMG_NEXT_THEME_MODIFIER_H

#include "stdthememodifier.h"

#include <stddef.h>
#include <vector>

#include <stdint.h>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }

namespace stmg
{

class StdTheme;

/** Draw the modifiers after the closest next sub theme separator.
 * Sort of a function call to another set of modifiers following the current one.
 * Has no effect if there isn't a following "sub theme".
 */
class NextThemeModifier : public StdThemeModifier
{
public:
	/** Constructor.
	 * @param p1Owner The owner. Cannot be null.
	 * @param nThemeNr The index in the sequence of sub theme modifiers. Must be &gt; 0.
	 */
	NextThemeModifier(StdTheme* p1Owner, int32_t nThemeNr) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
private:
	friend class StdTheme;
private:
	int32_t m_nThemeNr;
	size_t m_nPPOfNextTheme;
private:
	NextThemeModifier() = delete;
	NextThemeModifier(const NextThemeModifier& oSource) = delete;
	NextThemeModifier& operator=(const NextThemeModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_NEXT_THEME_MODIFIER_H */

