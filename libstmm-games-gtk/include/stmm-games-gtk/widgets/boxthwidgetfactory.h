/*
 * File:   boxthwidgetfactory.h
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

#ifndef STMG_BOX_TH_WIDGET_FACTORY_H
#define STMG_BOX_TH_WIDGET_FACTORY_H

#include "boxthemewidgetfactory.h"

#include <memory>
#include <utility>

namespace stmg { class BoxWidget; }

namespace stmg
{

////////////////////////////////////////////////////////////////////////////////
class BoxThWidgetFactory;

class BoxThWidget : public BoxThemeWidget<BoxThWidgetFactory, BoxThWidget, BoxWidget>
{
public:
	using BoxThemeWidget<BoxThWidgetFactory, BoxThWidget, BoxWidget>::BoxThemeWidget;
	using BoxThemeWidget<BoxThWidgetFactory, BoxThWidget, BoxWidget>::reInit;
};

////////////////////////////////////////////////////////////////////////////////
class BoxThWidgetFactory : public BoxThemeWidgetFactory<BoxThWidgetFactory, BoxThWidget, BoxWidget>
{
public:
	using BoxThemeWidgetFactory<BoxThWidgetFactory, BoxThWidget, BoxWidget>::BoxThemeWidgetFactory;
};

} // namespace stmg

#endif	/* STMG_BOX_TH_WIDGET_FACTORY_H */

