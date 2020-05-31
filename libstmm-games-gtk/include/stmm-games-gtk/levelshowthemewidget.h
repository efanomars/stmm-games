/*
 * File:   levelshowthemewidget.h
 *
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

#ifndef STMG_LEVEL_SHOW_THEME_WIDGET_H
#define STMG_LEVEL_SHOW_THEME_WIDGET_H

#include "themewidget.h"
#include "themewidgetinteractive.h"

#include <stmm-games/util/basictypes.h>

#include <memory>

#include <stdint.h>

namespace stmg { class GameWidget; }
namespace stmg { class LevelShowWidget; }

namespace stmg
{

using std::shared_ptr;

/** Base class for implementations of LevelShowWidget view.
 */
class LevelShowThemeWidget : public ThemeWidget, public ThemeWidgetInteractive
{
public:
	/** The origin of the canvas relative to the upper left corner of the widget.
	 * The canvas is where the widget draws the level's board tiles. The widget's
	 * frame is not considered part of the canvas.
	 * @return The position in pixels.
	 */
	virtual NPoint getCanvasPixPos() noexcept = 0;
	/** Returns a pointer to the model.
	 * @return The pointer to the model or null if not set yet.
	 */
	LevelShowWidget* getLevelShowWidget() noexcept
	{
		return m_p0LevelShowWidget;
	}
	/** Returns a pointer to the model.
	 * @return The pointer to the model or null if not set yet.
	 */
	const LevelShowWidget* getLevelShowWidget() const noexcept
	{
		return m_p0LevelShowWidget;
	}
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	/** Constructor.
	 * @param refModel The model. Cannot be null.
	 */
	LevelShowThemeWidget(const shared_ptr<GameWidget>& refModel) noexcept;
	/** See constructor.
	 */
	void reInit(const shared_ptr<GameWidget>& refModel) noexcept;
private:
	LevelShowWidget* m_p0LevelShowWidget;
};

} // namespace stmg

#endif	/* STMG_LEVEL_SHOW_THEME_WIDGET_H */

