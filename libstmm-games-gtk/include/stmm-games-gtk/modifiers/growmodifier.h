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
 * File:   growmodifier.h
 */

#ifndef STMG_GROW_MODIFIER_H
#define STMG_GROW_MODIFIER_H

#include "containermodifier.h"

#include "stdthememodifier.h"

#include "gtkutil/elapsedmapper.h"

#include <vector>

#include <stdint.h>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }

namespace stmg
{

class StdTheme;

/** Growing effect of sub-modifiers.
 * This modifier grows the sub-modifiers: from [elapsed == 0.0] a point
 * to [elapsed == 1.0] the full sized tile.
 * If elapsed is -1 the sub-modifiers are drawn opaque even if inverted.
 * The elapsed mapping is applied first.
 */
class GrowModifier : public ContainerModifier
{
public:
	struct Init
	{
		int32_t m_nElapsedTileAniIdx = -1; /**< The tile animation that determines the grow value. Default -1 (undefined).*/
		double m_fDefaultElapsed = -1.0; /**< The default elapsed value. From 0.0 to 1.0 or -1.0 if not defined. */
		ElapsedMapper m_oMapper; /**< The elapsed mapper. */
		bool m_bInvert = false; /**< Whether the growing should be inverted (becomes shrinking). Default is false.*/
	};
	/** Constructor.
	 * The two params m_nElapsedTileAniIdx and m_fDefaultElapsed cannot be both undefined.
	 * @param p1Owner The owner theme. Cannot be null.
	 * @param oInit The initialization data.
	 */
	GrowModifier(StdTheme* p1Owner, Init&& oInit) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
private:
	void growImageInRect(const Cairo::RefPtr<Cairo::Context>& refCc
							, int32_t nImgW, int32_t nImgH, int32_t nRectW, int32_t nRectH
							, double fFactor) noexcept;

	Init m_oData;
private:
	GrowModifier() = delete;
	GrowModifier(const GrowModifier& oSource) = delete;
	GrowModifier& operator=(const GrowModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_GROW_MODIFIER_H */

