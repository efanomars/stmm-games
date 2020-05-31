/*
 * File:   imagemodifier.h
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

#ifndef STMG_IMAGE_MODIFIER_H
#define STMG_IMAGE_MODIFIER_H

#include "stdthememodifier.h"

#include <memory>
#include <vector>

#include <stdint.h>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace stmg { class Image; }
namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }

namespace stmg
{

using std::shared_ptr;

class StdTheme;

/** Modifier drawing an image.
 */
class ImageModifier : public StdThemeModifier
{
public:
	/** Constructor.
	 * @param p1Owner The owner theme. Cannot be null.
	 * @param refImage The image to draw. Cannot be null.
	 */
	ImageModifier(StdTheme* p1Owner, const shared_ptr<Image>& refImage) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
protected:
	void registerTileSize(int32_t nW, int32_t nH) noexcept override;
	void unregisterTileSize(int32_t nW, int32_t nH) noexcept override;
private:
	shared_ptr<Image> m_refImage;
private:
	ImageModifier() = delete;
	ImageModifier(const ImageModifier& oSource) = delete;
	ImageModifier& operator=(const ImageModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_IMAGE_MODIFIER_H */

