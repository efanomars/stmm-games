/*
 * File:   transparentthwidgetfactory.cc
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

#include "widgets/transparentthwidgetfactory.h"

#include <glibmm/refptr.h>

#include <cassert>
#include <iostream>                     // for operator<<, basic_ostream
#include <string>                       // for operator<<, char_traits, string
#include <vector>                       // for vector

#include <stmm-games/gamewidget.h>
#include <stmm-games/widgets/transparentwidget.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

namespace stmg { class SegmentedFunction; }
namespace stmg { class StdTheme; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace Pango { class Context; }

namespace stmg
{

TransparentThWidgetFactory::TransparentThWidgetFactory(StdTheme* p1Owner) noexcept
: StdThemeWidgetFactory(p1Owner)
{
	assert(p1Owner != nullptr);
}
shared_ptr<ThemeWidget> TransparentThWidgetFactory::create(const shared_ptr<GameWidget>& refGameWidget
															, double /*fTileWHRatio*/
															, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept
{
	static_cast<void>(refFontContext.operator->());
//std::cout << "TransparentThWidgetFactory::create" << '\n';
	assert(refGameWidget);
	assert(refFontContext);
	GameWidget* p0GameWidget = refGameWidget.get();
	assert(p0GameWidget != nullptr);
	TransparentWidget* p0TransparentWidget = dynamic_cast<TransparentWidget*>(p0GameWidget);
	if (p0TransparentWidget == nullptr) {
 		return shared_ptr<ThemeWidget>{};
 	}
	shared_ptr<TransparentTWidget> refWidget;
	m_oTransparentTWidgets.create(refWidget, this, refGameWidget, p0TransparentWidget);
	return refWidget;
}
TransparentThWidgetFactory::TransparentTWidget::TransparentTWidget(TransparentThWidgetFactory* p1Owner
																	, const shared_ptr<GameWidget>& refGameWidget
																	, TransparentWidget* p0TransparentWidget) noexcept
: ThemeWidget(refGameWidget)
, m_p1Owner(p1Owner)
, m_p0TransparentWidget(p0TransparentWidget)
, m_oImmuTW(this)
, m_oSizedTW(this)
{
	reInitCommon();
}
void TransparentThWidgetFactory::TransparentTWidget::reInit(TransparentThWidgetFactory* p1Owner
															, const shared_ptr<GameWidget>& refGameWidget
															, TransparentWidget* p0TransparentWidget) noexcept
{
//std::cout << "  TransparentThWidgetFactory::TransparentTWidget::reInit()  p0TransparentWidget=" << reinterpret_cast<int64_t>(p0TransparentWidget) << '\n';
	ThemeWidget::reInit(refGameWidget);
	m_p1Owner = p1Owner;
	m_p0TransparentWidget = p0TransparentWidget;
	//
	reInitCommon();
}
void TransparentThWidgetFactory::TransparentTWidget::reInitCommon() noexcept
{
	assert(m_p1Owner != nullptr);
	assert(m_p0TransparentWidget != nullptr);
	const double fTileWHRatio = 1.0;
	m_oImmuTW.reInit(fTileWHRatio);
	m_oSizedTW.reInit(m_p0TransparentWidget);
}

void TransparentThWidgetFactory::TransparentTWidget::dump(int32_t
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
		std::cout << sIndent << "TransparentThWidgetFactory::TransparentTWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	ThemeWidget::dump(nIndentSpaces, false);
	#endif //NDEBUG
}
NSize TransparentThWidgetFactory::TransparentTWidget::getMinSize(int32_t /*nLayoutConfig*/) const noexcept
{
	TransparentThWidgetFactory* const & p0Factory = m_p1Owner;
	if (p0Factory == nullptr) {
		return NSize{0,0};
	}
	return NSize{1,1};
}

void TransparentThWidgetFactory::TransparentTWidget::sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept
{
	m_oImmuTW.sizeAndConfig(nTileW, nLayoutConfig);
}
void TransparentThWidgetFactory::TransparentTWidget::placeAndMaybeResizeIn(const NRect& oRect) noexcept
{
	m_oImmuTW.placeAndMaybeResizeIn(oRect);
}
const std::pair<SegmentedFunction, SegmentedFunction>& TransparentThWidgetFactory::TransparentTWidget::getSizeFunctions(int32_t nLayoutConfig) const noexcept
{
//std::cout << "TransparentThWidgetFactory::TransparentTWidget::getSizeFunctions this=" << reinterpret_cast<int64_t>(this) << '\n';
	return m_oSizedTW.getSizeFunctions(nLayoutConfig);
}
void TransparentThWidgetFactory::TransparentTWidget::draw(const Cairo::RefPtr<Cairo::Context>& /*refCc*/) noexcept
{
}

} // namespace stmg
