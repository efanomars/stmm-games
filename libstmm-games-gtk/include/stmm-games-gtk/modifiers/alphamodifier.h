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
 * File:   alphamodifier.h
 */

#ifndef STMG_ALPHA_MODIFIER_H
#define STMG_ALPHA_MODIFIER_H

#include "containermodifier.h"

#include "stdthememodifier.h"

#include <vector>

#include <stdint.h>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }

namespace stmg
{

class StdTheme;

/** Alpha tile modifier.
 * Draws the sub modifiers with the alpha (transparency) defined by the
 * to be drawn tile.
 */
class AlphaModifier : public ContainerModifier
{
public:
	/** Constructor.
	 * @param p1Owner The owner. Cannot be null.
	 */
	explicit AlphaModifier(StdTheme* p1Owner) noexcept;
	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
private:
	AlphaModifier() = delete;
	AlphaModifier(const AlphaModifier& oSource) = delete;
	AlphaModifier& operator=(const AlphaModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_ALPHA_MODIFIER_H */

