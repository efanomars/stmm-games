/*
 * File:   rotatemodifier.h
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

#ifndef STMG_ROTATE_MODIFIER_H
#define STMG_ROTATE_MODIFIER_H

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

/** Rotates the sub modifier according to a tile animator.
 */
class RotateModifier : public ContainerModifier
{
public:
	struct Init
	{
		int32_t m_nElapsedTileAniIdx = -1; /**< The tileanimation that determines the rotate value. Default -1 (undefined).*/
		double m_fDefaultElapsed = -1.0; /**< The default elapsed value. From 0.0 to 1.0 or -1.0 if not defined. */
		bool m_bInvert = false; /**< Whether the rotation should be inverted. Default is false.*/
	};
	/** Constructor.
	 * The two params m_nElapsedTileAniIdx and m_fDefaultElapsed cannot be both undefined.
	 * @param p1Owner The owner theme. Cannot be null.
	 * @param oInit The initialization data.
	 */
	RotateModifier(StdTheme* p1Owner, Init&& oInit) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
private:
	void rotateImageInRect(const Cairo::RefPtr<Cairo::Context>& refCc
							, int32_t nImgW, int32_t nImgH, int32_t nRectW, int32_t nRectH
							, double fShrink, double fAngleDeg) noexcept;

	Init m_oData;
private:
	RotateModifier() = delete;
	RotateModifier(const RotateModifier& oSource) = delete;
	RotateModifier& operator=(const RotateModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_ROTATE_MODIFIER_H */

