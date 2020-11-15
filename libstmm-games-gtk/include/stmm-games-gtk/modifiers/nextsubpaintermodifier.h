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
 * File:   nextsubpaintermodifier.h
 */

#ifndef STMG_NEXT_SUB_PAINTER_MODIFIER_H
#define STMG_NEXT_SUB_PAINTER_MODIFIER_H

#include "stdthememodifier.h"

#include <vector>

#include <stdint.h>
#include <stddef.h>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }

namespace stmg
{

class StdTheme;

/** Continue drawing jumping to the first modifier of the next sub-painter.
 * 
 * Sort of a "gosub" call to the sub painter following the current one.
 * Has no effect if there isn't a following sub painter.
 */
class NextSubPainterModifier : public StdThemeModifier
{
public:
	/** Constructor.
	 * @param p1Owner The owner theme. Cannot be null.
	 */
	explicit NextSubPainterModifier(StdTheme* p1Owner) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
private:
	friend class StdTheme;
	size_t m_nPPOfNextSubPainter; // set by StdTheme
private:
	NextSubPainterModifier() = delete;
	NextSubPainterModifier(const NextSubPainterModifier& oSource) = delete;
	NextSubPainterModifier& operator=(const NextSubPainterModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_NEXT_SUB_PAINTER_MODIFIER_H */

