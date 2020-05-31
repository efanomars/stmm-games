/*
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
/*
 * File:   levelshowthemewidget.cc
 */

#include "levelshowthemewidget.h"

#include "gamewidget.h"
#include "widgets/levelshowwidget.h"

//#include <iostream>
#include <cassert>


namespace stmg
{

LevelShowThemeWidget::LevelShowThemeWidget(const shared_ptr<GameWidget>& refModel) noexcept
: ThemeWidget(refModel)
, ThemeWidgetInteractive(this, (assert(dynamic_cast<LevelShowWidget*>(refModel.get()) != nullptr)
								, static_cast<LevelShowWidget*>(refModel.get())->isInteractive()))
, m_p0LevelShowWidget(static_cast<LevelShowWidget*>(refModel.get()))
{
//std::cout << "LevelShowThemeWidget::LevelShowThemeWidget " << m_p0LevelShowWidget->isInteractive() << '\n';
}
void LevelShowThemeWidget::reInit(const shared_ptr<GameWidget>& refModel) noexcept
{
	ThemeWidget::reInit(refModel);
	auto p0Model = refModel.get();
	assert(dynamic_cast<LevelShowWidget*>(p0Model) != nullptr);
	m_p0LevelShowWidget = static_cast<LevelShowWidget*>(p0Model);
	ThemeWidgetInteractive::reInit(this, m_p0LevelShowWidget->isInteractive());
}

void LevelShowThemeWidget::dump(int32_t
#ifndef NDEBUG
nIndentSpaces
#endif //NDEBUG
, bool /*bHeader*/) const noexcept
{
	#ifndef NDEBUG
	ThemeWidget::dump(nIndentSpaces, false);
	#endif //NDEBUG
}

} // namespace stmg
