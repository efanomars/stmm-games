/*
 * File:   stdthemewidgetfactory.h
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

#ifndef STMG_STD_THEME_WIDGET_FACTORY_H
#define STMG_STD_THEME_WIDGET_FACTORY_H

#include <memory>
//#include <iostream>

namespace stmg { class GameWidget; }
namespace stmg { class ThemeWidget; }

namespace Glib { template <class T_CppObject> class RefPtr; }
namespace Pango { class Context; }

namespace stmg
{

using std::shared_ptr;

class StdTheme;

/** Base class for theme widget factories.
 */
class StdThemeWidgetFactory
{
public:
	virtual ~StdThemeWidgetFactory() noexcept = default;
	/** Constructor.
	 * @param p1Owner The owner. Cannot be null.
	 */
	explicit StdThemeWidgetFactory(StdTheme* p1Owner) noexcept;
	/** Creates a theme widget for a game widget.
	 * @param refGameWidget The model. Cannot be null.
	 * @param fTileWHRatio The tile weight to height ratio.
	 * @param refFontContext The pango font context. Cannot be null.
	 * @return The created theme widget instance or null if can't create.
	 */
	virtual shared_ptr<ThemeWidget> create(const shared_ptr<GameWidget>& refGameWidget
											, double fTileWHRatio, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept = 0;

	/** Clears the owner passed in the constructor.
	 * Makes the instance unusable.
	 */
	virtual void clearOwner() noexcept;
protected:
	/** The owner.
	 * @return The owner StdTheme.
	 */
	inline StdTheme* owner() noexcept { return m_p1Owner; }
	/** The owner.
	 * @return The owner StdTheme.
	 */
	inline const StdTheme* owner() const noexcept { return m_p1Owner; }
private:
	StdTheme* m_p1Owner;
private:
	StdThemeWidgetFactory() = delete;
	StdThemeWidgetFactory(const StdThemeWidgetFactory& oSource) = delete;
	StdThemeWidgetFactory& operator=(const StdThemeWidgetFactory& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_STD_THEME_WIDGET_FACTORY_H */

