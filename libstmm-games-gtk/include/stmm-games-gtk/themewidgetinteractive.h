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
 * File:   themewidgetinteractive.h
 */

#ifndef STMG_THEME_WIDGET_INTERACTIVE_H
#define STMG_THEME_WIDGET_INTERACTIVE_H

#include <memory>

#include <stdint.h>

namespace stmg { class ThemeWidget; }
namespace stmi { class Event; }

namespace stmg
{

using std::shared_ptr;

/** Optional interface for ThemeWidget.
 * This interface can only be acquired from a ThemeWidget instance
 * (usually by the theme layout) through
 * dynamic_cast or std::dynamic_pointer_cast.
 */
class ThemeWidgetInteractive
{
public:
	virtual ~ThemeWidgetInteractive() noexcept = default;
	/** Handles the XYEvent input.
	 * It's safe to statically cast `refXYEvent->get()` to `stmi::XYEvent*`.
	 *
	 * The team and mate parameters are not necessarily those of the widget but can
	 * also be determined by the stmi::Capability of the stmi::XYEvent, which
	 * is done by the caller of this function.
	 *
	 * The interactive widget might not handle an event. For example
	 * when a new grab (see stmi::XYEvent::XY_GRAB) is initiated, the widget
	 * might return false because it already has an open grab. Another example:
	 * a key action widget needs the player to be defined (nMate &gt;= 0).
	 * @param refXYEvent Reference to a stmi::XYEvent object. Cannot be null.
	 * @param nTeam The team or `-1` if not set.
	 * @param nMate The mate of `-1` if not set.
	 * @return Whether the widget has handled the event.
	 */
	virtual bool handleXYInput(const shared_ptr<stmi::Event>& refXYEvent, int32_t nTeam, int32_t nMate) noexcept = 0;
	/** Tells whether the widget should really be interactive.
	 * When the widget's model refers to an AI player or a team, level, game
	 * that doesn't contain human players this may return false.
	 * Even widgets assigned to a human player might want to disable interactivity
	 * because the game only uses the keyboard as input.
	 * @return Whether the widget is really interactive.
	 */
	inline bool isInteractive() const noexcept
	{
		return m_bInteractive; // || m_p0TW->getModel()->isAssignedToHuman()
	}
	/** The pointer to the interface's theme widget.
	 * @return The widget implementing this interface. Can't be null.
	 */
	inline ThemeWidget* getThemeWidget() noexcept { return m_p0TW; }

	#ifndef NDEBUG
	void dump(int32_t nIndentSpaces) const noexcept;
	#endif //NDEBUG
protected:
	/** Constructor.
	 * @param p0TW Pointer to the theme widget implementing this interface. Cannot be null.
	 * @param bInteractive Whether the widget is really interactive.
	 */
	ThemeWidgetInteractive(ThemeWidget* p0TW, bool bInteractive) noexcept;
	/** Reinitialization.
	 * @param p0TW Pointer to the theme widget implementing this interface. Cannot be null.
	 * @param bInteractive Whether the widget is really interactive.
	 */
	void reInit(ThemeWidget* p0TW, bool bInteractive) noexcept;
private:
	ThemeWidget* m_p0TW;
	bool m_bInteractive;
};


} // namespace stmg

#endif	/* STMG_THEME_WIDGET_INTERACTIVE_H */

