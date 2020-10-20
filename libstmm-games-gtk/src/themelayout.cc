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
 * File:   themelayout.cc
 */

#include "themelayout.h"

#include "themecontainerwidget.h"

#include <stmm-games/gamewidget.h>

#include <vector>

namespace stmg
{

void ThemeLayout::markAllThemeWidgets(bool bAssigned) noexcept
{
	traverseMarkAllThemeWidgets(m_refRootTW.get(), bAssigned);
	if (bAssigned) {
		traverseInformAllThemeWidgets(m_refRootTW.get());
	}
}
void ThemeLayout::traverseMarkAllThemeWidgets(ThemeWidget* p0TW, bool bAssigned) noexcept
{
	assert(p0TW != nullptr);
	if (bAssigned) {
		// Can't be assigned to more than one layouts
		assert(p0TW->m_p0ThemeLayout == nullptr);
	}
	//
	p0TW->setThemeLayout(bAssigned ? this : nullptr);
	//
	auto p0ThemeContainerWidget = dynamic_cast<ThemeContainerWidget*>(p0TW);
	if (p0ThemeContainerWidget != nullptr) {
		for (auto& refChildTW : p0ThemeContainerWidget->m_aChlidren) {
			traverseMarkAllThemeWidgets(refChildTW.get(), bAssigned);
		}
	}
}
void ThemeLayout::traverseInformAllThemeWidgets(ThemeWidget* p0TW) noexcept
{
	p0TW->onAssignedToLayout();
	//
	auto p0ThemeContainerWidget = dynamic_cast<ThemeContainerWidget*>(p0TW);
	if (p0ThemeContainerWidget != nullptr) {
		for (auto& refChildTW : p0ThemeContainerWidget->m_aChlidren) {
			traverseInformAllThemeWidgets(refChildTW.get());
		}
	}
}
void ThemeLayout::recalcSizeFunctions(ThemeWidget* p0ReferenceThemeWidget) noexcept
{
	// fist calculate the size functions of the reference itself
	p0ReferenceThemeWidget->onRecalcSizeFunctions(p0ReferenceThemeWidget);
	recalcSizeFunctions(m_refRootTW.get(), p0ReferenceThemeWidget);
}
void ThemeLayout::recalcSizeFunctions(ThemeWidget* p0TW, ThemeWidget* p0ReferenceThemeWidget) noexcept
{
	assert(p0TW != nullptr);
	assert(p0ReferenceThemeWidget != nullptr);
	if (p0ReferenceThemeWidget == p0TW) {
		// Already calculated
		return;
	}
	// depth first:
	// if it is a container, first make sure functions of children are calculated
	auto p0ThemeContainerWidget = dynamic_cast<ThemeContainerWidget*>(p0TW);
	if (p0ThemeContainerWidget != nullptr) {
		for (auto& refChildTW : p0ThemeContainerWidget->m_aChlidren) {
			recalcSizeFunctions(refChildTW.get(), p0ReferenceThemeWidget);
		}
	}
//std::cout << "ThemeLayout::recalcSizeFunctions  p0TW=" << reinterpret_cast<int64_t>(p0TW) << '\n';
	p0TW->onRecalcSizeFunctions(p0ReferenceThemeWidget);
}
const shared_ptr<ThemeWidget>& ThemeLayout::getWidgetNamed(const std::string& sName, int32_t nTeam, int32_t nMate) const noexcept
{
	return traverseFindNamedThemeWidgets(m_refRootTW, sName, nTeam, nMate);
}
const shared_ptr<ThemeWidget>& ThemeLayout::traverseFindNamedThemeWidgets(const shared_ptr<ThemeWidget>& refTW
																		, const std::string& sName, int32_t nTeam, int32_t nMate) const noexcept
{
	auto& refGW = refTW->getModel();
	if ((refGW->getName() == sName) && (refGW->getTeam() == nTeam) && (refGW->getMate() == nMate)) {
		return refTW;
	}
	//
	auto p0ThemeContainerWidget = dynamic_cast<ThemeContainerWidget*>(refTW.get());
	if (p0ThemeContainerWidget != nullptr) {
		for (auto& refChildTW : p0ThemeContainerWidget->m_aChlidren) {
			auto& refFound = traverseFindNamedThemeWidgets(refChildTW, sName, nTeam, nMate);
			if (refFound) {
				return refFound; //---------------------------------------------
			}
		}
	}
	static const shared_ptr<ThemeWidget> s_refEmptyTW{};
	return s_refEmptyTW;
}
void ThemeLayout::dump(int32_t
#ifndef NDEBUG
nIndentSpaces
#endif //NDEBUG
) const noexcept
{
	#ifndef NDEBUG
	m_refRootTW->dump(nIndentSpaces);
	#endif //NDEBUG
}

} // namespace stmg
