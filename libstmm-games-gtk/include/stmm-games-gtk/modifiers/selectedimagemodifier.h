/*
 * File:   selectedimagemodifiers.h
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

#ifndef STMG_SELECTED_IMAGE_MODIFIER_H
#define STMG_SELECTED_IMAGE_MODIFIER_H

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

/** Draws the selected image.
 * The image can be selected foe example by SelectCaseModifier or CaseModifier.
 */
class SelectedImageModifier : public StdThemeModifier
{
public:
	/** Constructor.
	 * @param p1Owner The owner theme. Cannot be null.
	 */
	explicit SelectedImageModifier(StdTheme* p1Owner) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
private:
	SelectedImageModifier() = delete;
	SelectedImageModifier(const SelectedImageModifier& oSource) = delete;
	SelectedImageModifier& operator=(const SelectedImageModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_SELECTED_IMAGE_MODIFIER_H */

