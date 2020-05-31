/*
 * File:   inputboxthwidgetfactory.h
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

#ifndef STMG_INPUT_BOX_TH_WIDGET_FACTORY_H
#define STMG_INPUT_BOX_TH_WIDGET_FACTORY_H

#include "boxthemewidgetfactory.h"
#include "themewidgetinteractive.h"

#include <stmm-games/widgets/inputboxwidget.h>

#include <memory>
#include <vector>
#include <utility>

#include <stdint.h>

namespace stmg { class GameWidget; }
namespace stmg { class ThemeWidget; }
namespace stmi { class Event; }

namespace stmg
{

using std::weak_ptr;
using std::shared_ptr;

class InputBoxThWidgetFactory;

class InputBoxThWidget : public BoxThemeWidget<InputBoxThWidgetFactory, InputBoxThWidget, InputBoxWidget>, public ThemeWidgetInteractive
{
public:
	InputBoxThWidget(InputBoxThWidgetFactory* p1Owner
					, const shared_ptr<GameWidget>& refGameWidget, InputBoxWidget* p0BoxWidget
					, const std::vector< shared_ptr<ThemeWidget> >& aChildTW, double fTileWHRatio) noexcept
	: BoxThemeWidget<InputBoxThWidgetFactory, InputBoxThWidget, InputBoxWidget>(p1Owner
																				, refGameWidget, p0BoxWidget, aChildTW, fTileWHRatio)
	, ThemeWidgetInteractive(this, true)
	, m_p0TargetWidget(nullptr)
	{
	}
	void reInit(InputBoxThWidgetFactory* p1Owner
				, const shared_ptr<GameWidget>& refGameWidget, InputBoxWidget* p0BoxWidget
				, const std::vector< shared_ptr<ThemeWidget> >& aChildTW, double fTileWHRatio) noexcept
	{
		BoxThemeWidget<InputBoxThWidgetFactory, InputBoxThWidget, InputBoxWidget>::reInit(p1Owner
																				, refGameWidget, p0BoxWidget, aChildTW, fTileWHRatio);
		ThemeWidgetInteractive::reInit(this, true);
		m_p0TargetWidget = nullptr;
	}

	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;

	bool handleXYInput(const shared_ptr<stmi::Event>& refXYEvent, int32_t nTeam, int32_t nMate) noexcept override;

protected:
	void onAssignedToLayout() noexcept override;
private:
	weak_ptr<ThemeWidgetInteractive> m_refTargetWidget;
	// To avoid creating a shared_ptr each time an event is handled
	ThemeWidgetInteractive* m_p0TargetWidget;
};

////////////////////////////////////////////////////////////////////////////////
class InputBoxThWidgetFactory : public BoxThemeWidgetFactory<InputBoxThWidgetFactory, InputBoxThWidget, InputBoxWidget>
{
public:
	using BoxThemeWidgetFactory<InputBoxThWidgetFactory, InputBoxThWidget, InputBoxWidget>::BoxThemeWidgetFactory;
	using BoxThemeWidgetFactory<InputBoxThWidgetFactory, InputBoxThWidget, InputBoxWidget>::create;
private:
};

} // namespace stmg

#endif	/* STMG_INPUT_BOX_TH_WIDGET_FACTORY_H */

