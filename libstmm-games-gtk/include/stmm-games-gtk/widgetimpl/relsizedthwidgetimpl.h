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
 * File:   relsizedthwidgetimpl.h
 */

#ifndef STMG_REL_SIZED_TH_WIDGET_IMPL_H
#define STMG_REL_SIZED_TH_WIDGET_IMPL_H

#include "themewidget.h"

#include "gtkutil/segmentedfunction.h"

#include <stmm-games/relsizedgamewidget.h>
#include <stmm-games/gamewidget.h>
#include <stmm-games/util/basictypes.h>

#include <memory>
//#include <iostream>
#include <array>

#include <cassert>
#include <type_traits>
#include <utility>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

////////////////////////////////////////////////////////////////////////////////
// class TW must define
//   // draws to 0,0 of the context rather than the widget position
//   void drawBase(const Cairo::RefPtr<Cairo::Context>& refCc)
//   // draws to 0,0 of the context rather than the widget position
//   void drawVariable(const Cairo::RefPtr<Cairo::Context>& refCc)
//   // whether the variable part of the widget has changed
//   bool isChanged() const
//   // the minimum widget size in pixel
//   NSize getMinSize(int32_t nLayoutConfig) const
template< class TW >
class RelSizedThemeWidgetImpl
{
public:
	explicit RelSizedThemeWidgetImpl(TW* p0TW) noexcept;

	void reInit(RelSizedGameWidget* p0RelSizedGameWidget) noexcept;

	const std::pair<SegmentedFunction, SegmentedFunction>& getSizeFunctions(int32_t nLayoutConfig) const noexcept;

	void onRecalcSizeFunctions(ThemeWidget* p0ReferenceThemeWidget) noexcept;
private:
	RelSizedGameWidget* m_p0RelSizedGameWidget;
	TW* m_p0TW;
	std::array<std::pair<SegmentedFunction, SegmentedFunction>, g_nTotLayoutConfigs> m_aSizeFunctions;
};

////////////////////////////////////////////////////////////////////////////////
template< class TW >
RelSizedThemeWidgetImpl<TW>::RelSizedThemeWidgetImpl(TW* p0TW) noexcept
: m_p0RelSizedGameWidget(nullptr)
, m_p0TW(p0TW)
{
	static_assert(std::is_base_of<ThemeWidget, TW>::value, "Must derive from ThemeWidget");
	assert(p0TW != nullptr);
}
template< class TW >
void RelSizedThemeWidgetImpl<TW>::reInit(RelSizedGameWidget* p0RelSizedGameWidget) noexcept
{
	assert(p0RelSizedGameWidget != nullptr);
	m_p0RelSizedGameWidget = p0RelSizedGameWidget;
}
template< class TW >
void RelSizedThemeWidgetImpl<TW>::onRecalcSizeFunctions(ThemeWidget* p0ReferenceThemeWidget) noexcept
{
//std::cout << "RelSizedThemeWidgetImpl<TW>::onRecalcSizeFunctions" << '\n';
	assert(p0ReferenceThemeWidget != nullptr);

	const double fSWFactor = m_p0RelSizedGameWidget->getReferenceWFactor();
	const double fSHFactor = m_p0RelSizedGameWidget->getReferenceHFactor();
	const double fWHRatio = m_p0RelSizedGameWidget->getWHRatio();
	//
	for (int32_t nLC = 0; nLC < g_nTotLayoutConfigs; ++nLC) {
		auto& oPair = p0ReferenceThemeWidget->getSizeFunctions(nLC);
		SegmentedFunction oRefWInTilesW = oPair.first;
		SegmentedFunction oRefHInTilesW = oPair.second;
		if (fSWFactor > 0.0) {
			//fWInTilesW = fSWFactor * fTotalRefW;
			oRefWInTilesW.mul(fSWFactor);
		} else {
			assert(fWHRatio > 0.0);
			assert(fSHFactor > 0.0);
			oRefWInTilesW.mul(fSHFactor * fWHRatio);
		}
		if (fSHFactor > 0.0) {
			oRefHInTilesW.mul(fSHFactor);
		} else {
			assert(fWHRatio > 0.0);
			assert(fSWFactor > 0.0);
			//fHInTilesW = fSWFactor * fTotalRefW / fWHRatio;
			oRefHInTilesW.mul(fSWFactor / fWHRatio);
		}
		const NSize oMinSize = m_p0TW->getMinSize(nLC);
		SegmentedFunction oWInTilesW(SegmentedFunction::max(oRefWInTilesW, SegmentedFunction(0, oMinSize.m_nW)));
		SegmentedFunction oHInTilesW(SegmentedFunction::max(oRefHInTilesW, SegmentedFunction(0, oMinSize.m_nH)));
		m_aSizeFunctions[nLC] = std::make_pair(oWInTilesW, oHInTilesW);
	}
}
template< class TW >
const std::pair<SegmentedFunction, SegmentedFunction>& RelSizedThemeWidgetImpl<TW>::getSizeFunctions(int32_t nLayoutConfig) const noexcept
{
	assert((nLayoutConfig >= 0) && (nLayoutConfig < g_nTotLayoutConfigs));
	return m_aSizeFunctions[nLayoutConfig];
}

} // namespace stmg

#endif	/* STMG_REL_SIZED_TH_WIDGET_IMPL_H */

