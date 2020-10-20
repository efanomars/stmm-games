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
 * File:   tileanimodifier.h
 */

#ifndef STMG_TILE_ANI_MODIFIER_H
#define STMG_TILE_ANI_MODIFIER_H

#include "stdthememodifier.h"

#include <vector>
#include <memory>

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

/** Draws the image selected by a tile animation value.
 */
class TileAniModifier : public StdThemeModifier
{
public:
	struct Init
	{
		int32_t m_nElapsedTileAniIdx = -1; /**< The tile animation that determines the image to show. Default -1 (undefined).*/
		double m_fDefaultElapsed = -1.0; /**< The default elapsed value. From 0.0 to 1.0 or -1.0 if not defined. */
		bool m_bInvert = false; /**< Whether the tile animation should be inverted. Default is false.*/
		shared_ptr<Image> m_refImg; /**< The default image. Can be null. */
		shared_ptr<TileAni> m_refTileAni; /**< The theme's image tile ani. Can be null. */
	};
	/** Constructor.
	 * @param p1Owner The owner. Cannot be null.
	 * @param oInit The initialization data.
	 */
	TileAniModifier(StdTheme* p1Owner, Init&& oInit) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
	void registerTileSize(int32_t nW, int32_t nH) noexcept override;
	void unregisterTileSize(int32_t nW, int32_t nH) noexcept override;
private:
	Init m_oData;
private:
	TileAniModifier() = delete;
	TileAniModifier(const TileAniModifier& oSource) = delete;
	TileAniModifier& operator=(const TileAniModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_TILE_ANI_MODIFIER_H */

