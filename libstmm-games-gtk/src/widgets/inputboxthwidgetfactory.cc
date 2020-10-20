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
 * File:   inputboxthwidgetfactory.cc
 */

#include "widgets/inputboxthwidgetfactory.h"

#include "themelayout.h"
#include "boxthemewidgetfactory.h"
#include "themewidgetinteractive.h"

#include <stmm-games/widgets/inputboxwidget.h>

#include <memory>
#include <string>
#include <iostream>
#include <utility>
#include <vector>

#include <stdint.h>

namespace stmi { class Event; }

namespace stmg
{

void InputBoxThWidget::dump(int32_t
#ifndef NDEBUG
nIndentSpaces
#endif //NDEBUG
, bool
#ifndef NDEBUG
bHeader
#endif //NDEBUG
) const noexcept
{
	#ifndef NDEBUG
	auto sIndent = std::string(nIndentSpaces, ' ');
	if (bHeader) {
		std::cout << sIndent << "InputBoxThWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	BoxThemeWidget<InputBoxThWidgetFactory, InputBoxThWidget, InputBoxWidget>::dump(nIndentSpaces, false);
	#endif //NDEBUG
}

void InputBoxThWidget::onAssignedToLayout() noexcept
{
	InputBoxWidget* p0InputBoxWidget = getBoxWidgetModel();
	const auto& sTargetName = p0InputBoxWidget->getTargetWidgetName();
	const int32_t nTargetTeam = p0InputBoxWidget->getTeam();
	const int32_t nTargetMate = p0InputBoxWidget->getMate();
	auto refTargetWidget = themeLayout().getWidgetNamed(sTargetName, nTargetTeam, nTargetMate);
	auto refITW = std::dynamic_pointer_cast<ThemeWidgetInteractive>(refTargetWidget);
	m_refTargetWidget = refITW;
	m_p0TargetWidget = refITW.get();
}

bool InputBoxThWidget::handleXYInput(const shared_ptr<stmi::Event>& refXYEvent, int32_t nTeam, int32_t nMate) noexcept
{
	if (m_refTargetWidget.expired()) {
		m_p0TargetWidget = nullptr;
		return false;
	}
	return m_p0TargetWidget->handleXYInput(refXYEvent, nTeam, nMate);
}

} // namespace stmg
