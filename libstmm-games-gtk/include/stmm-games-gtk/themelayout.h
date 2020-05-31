/*
 * File:   themelayout.h
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

#ifndef STMG_THEME_LAYOUT_H
#define STMG_THEME_LAYOUT_H

#include "themewidget.h"

#include <cassert>
#include <memory>
#include <string>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

class ThemeLayout
{
public:
	/** Get the root theme widget.
	 * @return The root widget as passed to the constructor.
	 */
	inline const shared_ptr<ThemeWidget>& getRoot() const noexcept { return m_refRootTW; }
	virtual void dump(int32_t nIndentSpaces) const noexcept;
	void dump() const noexcept
	{
		dump(0);
	}

	/** Find theme widget with a model with a certain name.
	 * @param sName The name of the widget.
	 * @param nTeam The team of the widget. Is -1 if game widget.
	 * @param nMate The mate of the widget. Is -1 if game or team widget.
	 * @return The widget. Null if none defined with given name.
	 */
	const shared_ptr<ThemeWidget>& getWidgetNamed(const std::string& sName, int32_t nTeam, int32_t nMate) const noexcept;
protected:
	/** Creates a layout.
	 * Calls onAssignedToLayout() of each theme widget reachable from the root
	 * widget.
	 * @param refRootTW The root widget. Cannot be null.
	 */
	ThemeLayout(const shared_ptr<ThemeWidget>& refRootTW) noexcept
	: m_refRootTW(refRootTW)
	{
		markAllThemeWidgets(true);
	}
	virtual ~ThemeLayout() noexcept
	{
		markAllThemeWidgets(false);
	}
	/** Reinitializes a layout.
	 * Calls onAssignedToLayout() of each theme widget reachable from the root
	 * widget. The old root is dropped.
	 * @param refRootTW The root widget. Cannot be null.
	 */
	void reInit(const shared_ptr<ThemeWidget>& refRootTW) noexcept
	{
		markAllThemeWidgets(false);
		m_refRootTW = refRootTW;
		markAllThemeWidgets(true);
	}
	/** Signals widgets in the layout to recalculate their size functions.
	 * The reference widget, who's size functions are already valid, can be used
	 * if the widget's size is relative to it.
	 * Calls onRecalcSizeFunctions() of each theme widget reachable from the root.
	 * ThemeWidget::getSizeFunctions() of the layout widgets might be invalid
	 * before this function is called by subclasses of ThemeLayout.
	 * @param p0ReferenceThemeWidget The reference theme widget. Cannot be null.
	 */
	void recalcSizeFunctions(ThemeWidget* p0ReferenceThemeWidget) noexcept;
	/** Sets the tile size of the widget and its configuration.
	 * @param p0TW The widget. Cannot be null.
	 * @param nTileW The tile size. Must be &gt;0.
	 * @param nLayoutConfig The layout config.
	 */
	void sizeAndConfig(ThemeWidget* p0TW, int32_t nTileW, int32_t nLayoutConfig) noexcept
	{
		assert(p0TW != nullptr);
		p0TW->sizeAndConfig(nTileW, nLayoutConfig);
	}
private:
	// calls onAssignedToLayout() for each widget)
	void markAllThemeWidgets(bool bAssigned) noexcept;
	void traverseMarkAllThemeWidgets(ThemeWidget* p0TW, bool bAssigned) noexcept;
	void traverseInformAllThemeWidgets(ThemeWidget* p0TW) noexcept;
	const shared_ptr<ThemeWidget>& traverseFindNamedThemeWidgets(const shared_ptr<ThemeWidget>& refTW
																, const std::string& sName, int32_t nTeam, int32_t nMate) const noexcept;
	void recalcSizeFunctions(ThemeWidget* p0TW, ThemeWidget* p0ReferenceThemeWidget) noexcept;
private:
	shared_ptr<ThemeWidget> m_refRootTW;
};

} // namespace stmg

#endif	/* STMG_THEME_LAYOUT_H */

