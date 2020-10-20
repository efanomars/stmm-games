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
 * File:   inputboxwidget.cc
 */

#include "widgets/inputboxwidget.h"

#include "layout.h"
#include "containerwidget.h"
#include "widgets/boxwidget.h"

#include <iostream>
#include <memory>

namespace stmg
{

void InputBoxWidget::dump(int32_t
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
		std::cout << sIndent << "InputBoxWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	BoxWidget::dump(nIndentSpaces, false);
	std::cout << sIndent << "  " << "m_sTargetWidgetName:  " << m_oData.m_sTargetWidgetName << '\n';
	#endif //NDEBUG
}
void InputBoxWidget::onAddedToLayout() noexcept
{
	BoxWidget::onAddedToLayout();
	#ifndef NDEBUG
	auto refWidget = layout().getWidgetNamed(m_oData.m_sTargetWidgetName, getTeam(), getMate());
	assert(refWidget);
	const auto* p0Container = dynamic_cast<ContainerWidget*>(refWidget.get());
	assert(p0Container == nullptr);
	#endif //NDEBUG
}

} // namespace stmg
