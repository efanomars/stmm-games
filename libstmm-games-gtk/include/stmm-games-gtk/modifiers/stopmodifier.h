/*
 * File:   stopmodifier.h
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

#ifndef STMG_STOP_MODIFIER_H
#define STMG_STOP_MODIFIER_H

#include "stdthememodifier.h"

#include <vector>

#include <stdint.h>

namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }
namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

class StdTheme;

/** Tells the theme to stop drawing.
 */
class StopModifier : public StdThemeModifier
{
public:
	/** Constructor.
	 * @param p1Owner The owner theme. Cannot be null.
	 */
	explicit StopModifier(StdTheme* p1Owner) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
private:
	StopModifier() = delete;
	StopModifier(const StopModifier& oSource) = delete;
	StopModifier& operator=(const StopModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_STOP_MODIFIER_H */

