/*
 * File:   stdthememodifier.h
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

#ifndef STMG_STD_THEME_MODIFIER_H
#define STMG_STD_THEME_MODIFIER_H

#include <cairomm/context.h>
#include <gtkmm.h>

#include <vector>

#include <stdint.h>

namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }

namespace stmg
{

class StdTheme;

/** Base class for tile drawing.
 */
class StdThemeModifier
{
public:
	virtual ~StdThemeModifier() noexcept = default;

	enum FLOW_CONTROL
	{
		FLOW_CONTROL_CONTINUE = 0 /**< Continue painting. */
		, FLOW_CONTROL_STOP = 1 /**< Stop painting. */
	};
	/** Draw tile (with player skin).
	 * @param refCc The cairo context. Cannot be null.
	 * @param oDc The theme drawing context. Cannot be null.
	 * @param oTile The tile to draw.
	 * @param nPlayer The player the tile "belongs" to. -1 means no player. Must be &gt;= -1.
	 * @param aAniElapsed The tile animations current value.
	 * @return Whether the caller should continue or stop painting.
	 */
	virtual FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc
						, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept = 0;
	/** Register tile size.
	 * Make sure to unregisterTileSize() when done with this size.
	 *
	 * This method should be implemented to cache images for frequently used sizes.
	 *
	 * Subclasses of ContainerModifier should not propagate this to contained Modifier
	 * instances since StdTheme does it automatically.
	 * @param nW The tile width. Must be positive.
	 * @param nH The tile height. Must be positive.
	 */
	virtual void registerTileSize(int32_t nW, int32_t nH) noexcept;
	/** Unregister tile size.
	 * Called when a tile size no longer needed. See registerTileSize().
	 * @param nW The tile width. Must be positive.
	 * @param nH The tile height. Must be positive.
	 */
	virtual void unregisterTileSize(int32_t nW, int32_t nH) noexcept;
protected:
	/** Constructor.
	 * @param p1Owner The theme. Cannot be null.
	 */
	explicit StdThemeModifier(StdTheme* p1Owner) noexcept;
	/** The theme.
	 * @return The theme. Is not null.
	 */
	inline StdTheme* owner() noexcept { return m_p1Owner; }
	/** The theme.
	 * @return The theme. Is not null.
	 */
	inline const StdTheme* owner() const noexcept { return m_p1Owner; }
private:
	friend class StdTheme;
	StdTheme* m_p1Owner;
private:
	StdThemeModifier() = delete;
	StdThemeModifier(const StdThemeModifier& oSource) = delete;
	StdThemeModifier& operator=(const StdThemeModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_STD_THEME_MODIFIER_H */

