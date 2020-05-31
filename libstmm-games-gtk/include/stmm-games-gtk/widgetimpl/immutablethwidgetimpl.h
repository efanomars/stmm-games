/*
 * File:   immutablethwidgetimpl.h
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

#ifndef STMG_IMMUTABLE_TH_WIDGET_IMPL_H
#define STMG_IMMUTABLE_TH_WIDGET_IMPL_H

#include "gtkutil/segmentedfunction.h"

#include <stmm-games/gamewidget.h>
#include <stmm-games/util/basictypes.h>

#include <memory>
//#include <iostream>
#include <cassert>
#include <limits>
#include <type_traits>

#include <stdint.h>

namespace stmg { class ThemeWidget; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

using std::shared_ptr;

////////////////////////////////////////////////////////////////////////////////
// class TW must define
//   // draws to 0,0 of the context rather than the widget position
//   void drawImmutable(const Cairo::RefPtr<Cairo::Context>& refCc)
template< class TW >
class ImmutableThWidgetImpl
{
public:
	explicit ImmutableThWidgetImpl(TW* p0TW) noexcept;
	void reInit(double fTileWHRatio) noexcept;

	NSize getSize() const noexcept;
	NPoint getPos() const noexcept;
	void setPos(int32_t nPixX, int32_t nPixY) noexcept;
	void draw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;

	void onAssignedToLayout() noexcept;

	void sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept;
	void placeAndMaybeResizeIn(const NRect& oRect) noexcept;

	inline double getTileWHRatio() const noexcept { return m_fTileWHRatio; }

	inline int32_t getTileW() const noexcept { return m_nTileW; }
	inline int32_t getLayoutConfig() const noexcept { return m_nLayoutConfig; }

	inline int32_t getPixX() const noexcept { return m_nPixX; }
	inline int32_t getPixY() const noexcept { return m_nPixY; }
	inline int32_t getPixW() const noexcept { return m_nPixW; }
	inline int32_t getPixH() const noexcept { return m_nPixH; }

protected:
	void setSize(int32_t nPixW, int32_t nPixH) noexcept;
	inline TW* getTW() noexcept { return m_p0TW; }
private:
	TW* m_p0TW;
	double m_fTileWHRatio;
	int32_t m_nLayoutConfig;
	int32_t m_nTileW;
	int32_t m_nPixX;
	int32_t m_nPixY;
	int32_t m_nPixW;
	int32_t m_nPixH;
};

////////////////////////////////////////////////////////////////////////////////
template< class TW >
ImmutableThWidgetImpl<TW>::ImmutableThWidgetImpl(TW* p0TW) noexcept
: m_p0TW(p0TW)
, m_fTileWHRatio(1.0)
, m_nLayoutConfig(0)
, m_nTileW(0)
, m_nPixX(0)
, m_nPixY(0)
, m_nPixW(0)
, m_nPixH(0)
{
	static_assert(std::is_base_of<ThemeWidget, TW>::value, "Must derive from ThemeWidget");
	assert(p0TW != nullptr);
}
template< class TW >
void ImmutableThWidgetImpl<TW>::reInit(double fTileWHRatio) noexcept
{
	m_fTileWHRatio = fTileWHRatio;
	m_nLayoutConfig = 0;
	m_nTileW = 0;
	m_nPixX = std::numeric_limits<int32_t>::min();
	m_nPixY = std::numeric_limits<int32_t>::min();
	m_nPixW = 0;
	m_nPixH = 0;
}

template< class TW >
void ImmutableThWidgetImpl<TW>::sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept
{
	assert(m_p0TW != nullptr);
//std::cout << "ImmutableThWidgetImpl<>::placeAndMaybeResizeIn adr=" << reinterpret_cast<int64_t>(m_p0TW);
//std::cout << "  nTileW=" << nTileW << "  nLayoutConfig=" << nLayoutConfig << '\n';
	m_nTileW = nTileW;
	m_nLayoutConfig = nLayoutConfig;
	const auto& oPair = m_p0TW->getSizeFunctions(nLayoutConfig);
	const SegmentedFunction& oWFun = oPair.first;
	const SegmentedFunction& oHFun = oPair.second;
	const int32_t nPixW = oWFun.eval(m_nTileW);
	const int32_t nPixH = oHFun.eval(m_nTileW / m_fTileWHRatio);
	setSize(nPixW, nPixH);
//std::cout << "                                      nPixW=" << nPixW << "  nPixH=" << nPixH << '\n';
}
template< class TW >
void ImmutableThWidgetImpl<TW>::placeAndMaybeResizeIn(const NRect& oRect) noexcept
{
//std::cout << "ImmutableThWidgetImpl<>::placeAndMaybeResizeIn adr=" << reinterpret_cast<int64_t>(this);
//std::cout << " " << typeid(TW).name();
//std::cout << "  (" << oRect.m_nX << "," << oRect.m_nY << "," << oRect.m_nW << "," << oRect.m_nH << ")" << '\n';
	assert(m_p0TW != nullptr);
	const NSize oOldWidgetSize = getSize();
	int32_t nWidgetX, nWidgetY;
	int32_t nWidgetW, nWidgetH;
	const WidgetAxisFit oHoriz = m_p0TW->getModel()->getWidgetHorizAxisFit(m_nLayoutConfig);
	const WidgetAxisFit oVert = m_p0TW->getModel()->getWidgetVertAxisFit(m_nLayoutConfig);
	if (oHoriz.m_eAlign == WidgetAxisFit::ALIGN_FILL) {
		nWidgetX = oRect.m_nX;
		nWidgetW = oRect.m_nW;
	} else {
		const int32_t nFreeW = oRect.m_nW - oOldWidgetSize.m_nW;
		nWidgetX = oRect.m_nX + oHoriz.getAlign() * nFreeW;
		nWidgetW = oOldWidgetSize.m_nW;
	}
	if (oVert.m_eAlign == WidgetAxisFit::ALIGN_FILL) {
		nWidgetY = oRect.m_nY;
		nWidgetH = oRect.m_nH;
	} else {
		const int32_t nFreeH = oRect.m_nH - oOldWidgetSize.m_nH;
		nWidgetY = oRect.m_nY + oVert.getAlign() * nFreeH;
		nWidgetH = oOldWidgetSize.m_nH;
	}
	setPos(nWidgetX, nWidgetY);
	setSize(nWidgetW, nWidgetH);
}
template< class TW >
void ImmutableThWidgetImpl<TW>::onAssignedToLayout() noexcept
{
}
template< class TW >
NSize ImmutableThWidgetImpl<TW>::getSize() const noexcept
{
	return {m_nPixW, m_nPixH};
}
template< class TW >
NPoint ImmutableThWidgetImpl<TW>::getPos() const noexcept
{
	return {m_nPixX, m_nPixY};
}
template< class TW >
void ImmutableThWidgetImpl<TW>::setPos(int32_t nPixX, int32_t nPixY) noexcept
{
	m_nPixX = nPixX;
	m_nPixY = nPixY;
}
template< class TW >
void ImmutableThWidgetImpl<TW>::setSize(int32_t nPixW, int32_t nPixH) noexcept
{
	m_nPixW = nPixW;
	m_nPixH = nPixH;
}

} // namespace stmg

#endif	/* STMG_IMMUTABLE_TH_WIDGET_IMPL_H */

