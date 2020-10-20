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
 * File:   containermodifier.h
 */

#ifndef STMG_CONTAINER_MODIFIER_H
#define STMG_CONTAINER_MODIFIER_H

#include "stdthememodifier.h"

#include <cairomm/refptr.h>
#include <cairomm/surface.h>

#include <memory>
#include <vector>

#include <stdint.h>

namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }

namespace Cairo { class Context; }

namespace stmg
{

using std::unique_ptr;

class StdTheme;

class ContainerModifier : public StdThemeModifier
{
public:
	/** Constructor.
	 * @param p1Owner The theme. Cannot be null.
	 */
	explicit ContainerModifier(StdTheme* p1Owner) noexcept;
	/** Add sub modifiers to this container.
	 * @param aModifier The modifiers. Cannot be null.
	 */
	void addSubModifiers(std::vector< unique_ptr<StdThemeModifier> >&& aModifier) noexcept;
	const std::vector< unique_ptr<StdThemeModifier> >& getSubModifiers() noexcept;
	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc
						, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
	void registerTileSize(int32_t nW, int32_t nH) noexcept override;
	void unregisterTileSize(int32_t nW, int32_t nH) noexcept override;
protected:
	FLOW_CONTROL drawContainedToWorkSurface(StdThemeDrawingContext& oDc
											, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed
											, Cairo::RefPtr<Cairo::Surface>& refSurface) noexcept;
private:
	Cairo::RefPtr<Cairo::Context> getWorkContext(int32_t nW, int32_t nH) noexcept;
private:
	std::vector< unique_ptr<StdThemeModifier> > m_aSubModifiers;
	Cairo::RefPtr<Cairo::ImageSurface> m_refWork;
private:
	ContainerModifier() = delete;
	ContainerModifier(const ContainerModifier& oSource) = delete;
	ContainerModifier& operator=(const ContainerModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_CONTAINER_MODIFIER_H */

