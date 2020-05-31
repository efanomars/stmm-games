/*
 * File:   containerwidget.cc
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

#include "containerwidget.h"

#include <cassert>
#include <iostream>
#include <string>
#include <utility>

namespace stmg
{

ContainerWidget::~ContainerWidget() noexcept
{
	disownChildren();
}
bool ContainerWidget::getChildExpand(const std::vector< shared_ptr<GameWidget> >& aChildWidgets, int32_t nLayoutConfig, bool bHoriz) noexcept
{
	for (auto& refGW : aChildWidgets) {
		if (bHoriz ? refGW->getWidgetHorizAxisFit(nLayoutConfig).m_bExpand : refGW->getWidgetVertAxisFit(nLayoutConfig).m_bExpand)
		{
			return true;
		}
	}
	return false;

}
bool ContainerWidget::getChildFill(const std::vector< shared_ptr<GameWidget> >& aChildWidgets, int32_t nLayoutConfig, bool bHoriz) noexcept
{
	for (auto& refGW : aChildWidgets) {
		if ((bHoriz ? refGW->getWidgetHorizAxisFit(nLayoutConfig).m_eAlign : refGW->getWidgetVertAxisFit(nLayoutConfig).m_eAlign) == WidgetAxisFit::ALIGN_FILL) {
			return true;
		}
	}
	return false;

}
ContainerWidget::ContainerWidget(Init&& oData) noexcept
: GameWidget(std::move(oData))
, m_oData(std::move(oData))
{
	commonInit();
}
void ContainerWidget::reInit(Init&& oData) noexcept
{
	// disown the old widgets
	disownChildren();
	GameWidget::reInit(std::move(oData));
	m_oData = std::move(oData);
	commonInit();
}
void ContainerWidget::disownChildren() noexcept
{
	for (auto& refGW : m_oData.m_aChildWidgets) {
		refGW->m_p1Owner = nullptr;
	}
}
void ContainerWidget::commonInit() noexcept
{
	for (auto& refGW : m_oData.m_aChildWidgets) {
		assert(refGW);
		assert(refGW->m_p1Owner == nullptr);
		refGW->m_p1Owner = this;
	}
}

void ContainerWidget::dump(int32_t
#ifndef NDEBUG
nIndentSpaces
#endif //NDEBUG
, bool /*bHeader*/) const noexcept
{
	#ifndef NDEBUG
	GameWidget::dump(nIndentSpaces, false);
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent << "Children:" << '\n';
	for (auto& refChildGW : m_oData.m_aChildWidgets) {
		refChildGW->dump(nIndentSpaces + 2);
	}
	#endif //NDEBUG
}

} // namespace stmg
