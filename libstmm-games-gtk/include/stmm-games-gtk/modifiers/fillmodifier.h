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
 * File:   fillmodifier.h
 */

#ifndef STMG_FILL_MODIFIER_H
#define STMG_FILL_MODIFIER_H

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

/** Fills the tile rectangle with the color of the to be drawn tile.
 */
class FillModifier : public StdThemeModifier
{
public:
	struct Init
	{
		int32_t m_nAlpha = 255; /**< The transparency. Must be &gt;= 0 and  &lt;= 255, Opaque is 255. Default is 255. */
		//TODO could let rect size in perc of tile (0..1) and define stroke color and width (of rect border)
	};
	/** Constructor.
	 * @param p1Owner The owner theme. Cannot be null.
	 * @param oInit The initialization data.
	 */
	FillModifier(StdTheme* p1Owner, Init&& oInit) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
private:
	double m_fAlpha;
private:
	FillModifier() = delete;
	FillModifier(const FillModifier& oSource) = delete;
	FillModifier& operator=(const FillModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_FILL_MODIFIER_H */

