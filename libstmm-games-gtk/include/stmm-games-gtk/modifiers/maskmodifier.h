/*
 * File:   maskmodifier.h
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

#ifndef STMG_MASK_MODIFIER_H
#define STMG_MASK_MODIFIER_H

#include "containermodifier.h"

#include <memory>

#include "stdthememodifier.h"

#include <vector>

#include <stdint.h>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace stmg { class Image; }
namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }
namespace stmg { class TileAni; }

namespace stmg
{

using std::shared_ptr;

class StdTheme;

/** Mask modifier.
 * Applies a mask to the sub modifiers. The mask can be an image or a TileAni.
 */
class MaskModifier : public ContainerModifier
{
public:
	struct Init
	{
		int32_t m_nElapsedTileAniIdx = -1; /**< The tileanimation that determines the mask to apply. Default -1 (undefined).*/
		double m_fDefaultElapsed = -1.0; /**< The default elapsed value. From 0.0 to 1.0 or -1.0 if not defined. */
		bool m_bInvert = false; /**< Whether the mask animation should be inverted. Default is false.*/
		shared_ptr<Image> m_refMask; /**< The default image. Can be null. */
		shared_ptr<TileAni> m_refTileAni; /**< The image tile ani. Can be null. */
	};
	/** Constructor.
	 * @param p1Owner The owner. Cannot be null.
	 * @param oInit The initialization data.
	 */
	MaskModifier(StdTheme* p1Owner, Init&& oInit) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;

	void registerTileSize(int32_t nW, int32_t nH) noexcept override;
	void unregisterTileSize(int32_t nW, int32_t nH) noexcept override;
private:
	Init m_oData;
private:
	MaskModifier() = delete;
	MaskModifier(const MaskModifier& oSource) = delete;
	MaskModifier& operator=(const MaskModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_MASK_MODIFIER_H */

