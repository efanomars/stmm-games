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
 * File:   actionsboxthwidgetfactory.h
 */

#ifndef STMG_ACTIONS_BOX_TH_WIDGET_FACTORY_H
#define STMG_ACTIONS_BOX_TH_WIDGET_FACTORY_H

#include "boxthemewidgetfactory.h"
#include "themewidgetinteractive.h"

#include <stmm-games/widgets/actionsboxwidget.h>

#include <memory>
#include <utility>
#include <vector>

#include <stdint.h>

namespace stmg { class GameWidget; }
namespace stmg { class ThemeWidget; }
namespace stmi { class Event; }

namespace stmg
{

class ActionsBoxThWidgetFactory;

class ActionsBoxThWidget : public BoxThemeWidget<ActionsBoxThWidgetFactory, ActionsBoxThWidget, ActionsBoxWidget>
							, public ThemeWidgetInteractive
{
public:
	ActionsBoxThWidget(ActionsBoxThWidgetFactory* p1Owner
						, const shared_ptr<GameWidget>& refGameWidget, ActionsBoxWidget* p0BoxWidget
						, const std::vector< shared_ptr<ThemeWidget> >& aChildTW, double fTileWHRatio) noexcept
	: BoxThemeWidget<ActionsBoxThWidgetFactory, ActionsBoxThWidget, ActionsBoxWidget>
					::BoxThemeWidget(p1Owner, refGameWidget, p0BoxWidget, aChildTW, fTileWHRatio)
	, ThemeWidgetInteractive(this, true)
	{
	}
	void reInit(ActionsBoxThWidgetFactory* p1Owner
				, const shared_ptr<GameWidget>& refGameWidget, ActionsBoxWidget* p0BoxWidget
				, const std::vector< shared_ptr<ThemeWidget> >& aChildTW, double fTileWHRatio) noexcept
	{
		BoxThemeWidget<ActionsBoxThWidgetFactory, ActionsBoxThWidget, ActionsBoxWidget>::reInit(p1Owner
																				, refGameWidget, p0BoxWidget, aChildTW, fTileWHRatio);
		ThemeWidgetInteractive::reInit(this, true);
	}

	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;

	bool handleXYInput(const shared_ptr<stmi::Event>& refXYEvent, int32_t nTeam, int32_t nMate) noexcept override;
protected:
	void sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept override;
	void calcGrid() noexcept;
private:
	//
	// Example: height of 600 pixel partitioned in 3 rows
	//
	//              --------------------
	// part[0] = 10 |       |          |
	//              |-------|----------| 100 = m_aRowY[0]
	//              |       |          |
	// part[1] = 30 |       |          |
	//              |       |          |
	//              |-------|----------| 400 = m_aRowY[1]
	// part[2] = 20 |       |          |
	//              |       |          |
	//              --------------------
	// ^
	// Where part[nRow] = m_p0ActionsBoxWidget->getRowPart(nRow)
	//
	std::vector<int32_t> m_aColX; // Size: m_p0ActionsBoxWidget->getTotColumns() - 1
	std::vector<int32_t> m_aRowY; // Size: m_p0ActionsBoxWidget->getTotRows() - 1
};



class ActionsBoxThWidgetFactory : public BoxThemeWidgetFactory<ActionsBoxThWidgetFactory, ActionsBoxThWidget, ActionsBoxWidget>
{
public:
	using BoxThemeWidgetFactory<ActionsBoxThWidgetFactory, ActionsBoxThWidget, ActionsBoxWidget>::BoxThemeWidgetFactory;
	using BoxThemeWidgetFactory<ActionsBoxThWidgetFactory, ActionsBoxThWidget, ActionsBoxWidget>::create;
};

} // namespace stmg

#endif	/* STMG_ACTIONS_BOX_TH_WIDGET_FACTORY_H */

