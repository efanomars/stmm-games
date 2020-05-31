/*
 * File:   themewidget.cc
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

#include "themewidget.h"

#include "themecontainerwidget.h"

#include "gtkutil/segmentedfunction.h"
#include "gtkutil/ccsave.h"

#include <stmm-games/util/basictypes.h>

#include <cairomm/context.h>
#include <cairomm/refptr.h>

#include <cassert>
#include <iostream>
#include <string>

namespace stmg
{

void ThemeWidget::onRecalcSizeFunctions(ThemeWidget* /*p0ReferenceThemeWidget*/) noexcept
{
}

ThemeLayout& ThemeWidget::themeLayout() noexcept
{
	assert(m_p0ThemeLayout != nullptr);
	return *m_p0ThemeLayout;
}
const ThemeLayout& ThemeWidget::themeLayout() const noexcept
{
	assert(m_p0ThemeLayout != nullptr);
	return *m_p0ThemeLayout;
}
void ThemeWidget::drawDeep(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
//std::cout << "ThemeContainerWidget::drawDeep" << '\n';
	if (m_p0Parent != nullptr) {
		CCSave oCCSave(refCc);
		NPoint oPos = getPos();
		NSize oSize = getSize();
		refCc->rectangle(oPos.m_nX, oPos.m_nY, oSize.m_nW, oSize.m_nH);
		refCc->clip();
		m_p0Parent->drawDeep(refCc);
	}
	draw(refCc);
}

void ThemeWidget::dump(int32_t
#ifndef NDEBUG
nIndentSpaces
#endif //NDEBUG
, bool /*bHeader*/
) const noexcept
{
#ifndef NDEBUG
	auto sIndent = std::string(nIndentSpaces + 2, ' ');
	std::cout << sIndent << "Model adr:" << reinterpret_cast<int64_t>(m_refModel.get()) << '\n';
	const auto& oPairOff = getSizeFunctions(false);
	const SegmentedFunction& oSFOffW = oPairOff.first;
	const SegmentedFunction& oSFOffH = oPairOff.second;
	auto& oPairOn = getSizeFunctions(true);
	const SegmentedFunction& oSFOnW = oPairOn.first;
	const SegmentedFunction& oSFOnH = oPairOn.second;
	const bool bOffEqOnW = (oSFOffW == oSFOnW);
	const bool bOffEqOnH = (oSFOffH == oSFOnH);
	std::cout << sIndent << (bOffEqOnW ? "Width:" : "Width  (Off):") << '\n';
	oSFOffW.dump(nIndentSpaces + 4);
	std::cout << sIndent << (bOffEqOnW ? "Height:" : "Height (Off):") << '\n';
	oSFOffH.dump(nIndentSpaces + 4);
	if (!bOffEqOnW) {
		std::cout << sIndent << "Width  (On):" << '\n';
		oSFOnW.dump(nIndentSpaces + 4);
	}
	if (!bOffEqOnH) {
		std::cout << sIndent << "Height (On):" << '\n';
		oSFOnH.dump(nIndentSpaces + 4);
	}
	std::cout << sIndent << "canChange(): " << canChange() << '\n';
	const NPoint oTWPos = getPos();
	const NSize oTWSize = getSize();
	std::cout << sIndent << "Pos X: " << oTWPos.m_nX << "  Y: " << oTWPos.m_nY << "  W: " << oTWSize.m_nW << "  H: " << oTWSize.m_nH << '\n';
#endif //NDEBUG
}

} // namespace stmg
