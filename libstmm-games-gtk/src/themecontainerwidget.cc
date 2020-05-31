/*
 * File:   themecontainerwidget.cc
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

#include "themecontainerwidget.h"

#include <iostream>
#include <string>

namespace stmg
{

void ThemeContainerWidget::setChildren(const std::vector< shared_ptr<ThemeWidget> >& aChildren) noexcept
{
//std::cout << "ThemeContainerWidget::setChildren aChildren.size()=" << aChildren.size() << '\n';
	for (auto& refTW : m_aChlidren) {
		assert(refTW);
		refTW->m_p0Parent = nullptr;
	}
	m_aChlidren = aChildren;
	for (auto& refTW : m_aChlidren) {
		assert(refTW);
		// A child can only be in one container
		assert(refTW->m_p0Parent == nullptr);
		refTW->m_p0Parent = this;
	}
}

void ThemeContainerWidget::dump(int32_t
#ifndef NDEBUG
nIndentSpaces
#endif //NDEBUG
, bool /*bHeader*/) const noexcept
{
	#ifndef NDEBUG
	ThemeWidget::dump(nIndentSpaces, false);
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent << "  Children:" << '\n';
	for (auto& refTW : m_aChlidren) {
		refTW->dump(nIndentSpaces + 4);
	}
	#endif //NDEBUG
}

} // namespace stmg
