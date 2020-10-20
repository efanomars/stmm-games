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
 * File:   mutablethwidgetimpl.h
 */

#ifndef STMG_LEAF_MUTABLE_TH_WIDGET_H
#define STMG_LEAF_MUTABLE_TH_WIDGET_H

#include "themecontainerwidget.h"

#include "gtkutil/ccsave.h"
#include "gtkutil/segmentedfunction.h"

#include <stmm-games/gamewidget.h>
#include <stmm-games/util/basictypes.h>

#include <cairomm/context.h>
#include <cairomm/enums.h>
#include <cairomm/refptr.h>
#include <cairomm/surface.h>

#include <memory>
//#include <iostream>
#include <cassert>
#include <algorithm>
#include <limits>
#include <type_traits>

#include <stdint.h>

namespace stmg { class ThemeWidget; }

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
template< class TW >
class MutableThWidgetImpl
{
public:
	explicit MutableThWidgetImpl(TW* p0TW) noexcept;
	void reInit(double fTileWHRatio) noexcept;

	NSize getSize() const noexcept;
	NPoint getPos() const noexcept;
	void setPos(int32_t nPixX, int32_t nPixY) noexcept;
	bool drawIfChanged(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
	void drawDeep(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
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
	// Draws to the buffer: therefore to 0,0 rather than m_nPixX,m_nPixY
	inline void drawBase(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept { m_p0TW->drawBase(refCc); }
	// Draws to the buffer: therefore to 0,0 rather than m_nPixX,m_nPixY
	inline void drawVariable(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept { m_p0TW->drawVariable(refCc); }
	inline bool isChanged() const noexcept { return m_p0TW->isChanged(); }
	void cacheBackgroundAndBase() noexcept;
	void drawCacheAndVariable(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
private:
	TW* m_p0TW;
	double m_fTileWHRatio;
	int32_t m_nLayoutConfig;
	int32_t m_nTileW;
	int32_t m_nPixX;
	int32_t m_nPixY;
	int32_t m_nPixW;
	int32_t m_nPixH;

	bool m_bCacheInvalid;
	Cairo::RefPtr<Cairo::ImageSurface> m_refSurf;
	Cairo::RefPtr<Cairo::Context> m_refCc;
};

////////////////////////////////////////////////////////////////////////////////
template< class TW >
MutableThWidgetImpl<TW>::MutableThWidgetImpl(TW* p0TW) noexcept
: m_p0TW(p0TW)
, m_fTileWHRatio(1.0)
, m_nLayoutConfig(0)
, m_nTileW(0)
, m_nPixX(0)
, m_nPixY(0)
, m_nPixW(0)
, m_nPixH(0)
, m_bCacheInvalid(true)
{
	static_assert(std::is_base_of<ThemeWidget, TW>::value, "Must derive from ThemeWidget");
	assert(p0TW != nullptr);
}
template< class TW >
void MutableThWidgetImpl<TW>::reInit(double fTileWHRatio) noexcept
{
	m_fTileWHRatio = fTileWHRatio;
	m_nLayoutConfig = 0;
	m_nTileW = 0;
	m_nPixX = std::numeric_limits<int32_t>::min();
	m_nPixY = std::numeric_limits<int32_t>::min();
	m_nPixW = 0;
	m_nPixH = 0;
	m_bCacheInvalid = true;
}

template< class TW >
void MutableThWidgetImpl<TW>::sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept
{
	assert(m_p0TW != nullptr);
//std::cout << "MutableThWidgetImpl<>::placeAndMaybeResizeIn adr=" << reinterpret_cast<int64_t>(m_p0TW);
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
void MutableThWidgetImpl<TW>::placeAndMaybeResizeIn(const NRect& oRect) noexcept
{
//std::cout << "MutableThWidgetImpl<>::placeAndMaybeResizeIn adr=" << reinterpret_cast<int64_t>(this);
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
void MutableThWidgetImpl<TW>::onAssignedToLayout() noexcept
{
}
template< class TW >
NSize MutableThWidgetImpl<TW>::getSize() const noexcept
{
	return {m_nPixW, m_nPixH};
}
template< class TW >
NPoint MutableThWidgetImpl<TW>::getPos() const noexcept
{
	return {m_nPixX, m_nPixY};
}
template< class TW >
void MutableThWidgetImpl<TW>::setPos(int32_t nPixX, int32_t nPixY) noexcept
{
	// not only size change,
	// also pos change within parent makes cache invalid
	// since drawing might have transparencies
	m_bCacheInvalid = true; // (m_nPixX != nPixX) || (m_nPixY != nPixY);
	m_nPixX = nPixX;
	m_nPixY = nPixY;
}
template< class TW >
void MutableThWidgetImpl<TW>::setSize(int32_t nPixW, int32_t nPixH) noexcept
{
//std::string sClass = typeid(TW).name();
//if (sClass.find("LevelShowTWidget") != std::string::npos) {
//std::cout << "MutableThWidgetImpl<TW>::setSize " << typeid(TW).name() << " adr:" << reinterpret_cast<int64_t>(m_p0TW) << '\n';
//std::cout << "                         Cur:    m_nPixW=" << m_nPixW << " m_nPixH=" << m_nPixH << '\n';
//std::cout << "                         New:      nPixW=" << nPixW << "   nPixH=" << nPixH << '\n';
//}
	const bool bNewSize = ((m_nPixW != nPixW) || (m_nPixH != nPixH));
	bool bSetClip = false;
	const bool bSurfUndefined = (!m_refSurf);
	int32_t nSurfW = (bSurfUndefined ? 0 : m_refSurf->get_width());
	int32_t nSurfH = (bSurfUndefined ? 0 : m_refSurf->get_height());
	if (bSurfUndefined || (nSurfW < nPixW) || (nSurfH < nPixH)) {
		if (!bSurfUndefined) {
			// cache surface can only increase
			nSurfW = std::max(nSurfW, nPixW);
			nSurfH = std::max(nSurfH, nPixH);
		} else {
			nSurfW = nPixW;
			nSurfH = nPixH;
		}
		m_refSurf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, nSurfW, nSurfH);
		m_refCc = Cairo::Context::create(m_refSurf);
//std::cout << "MutableThWidgetImpl<TW>::setSize create buffer  adr:" << reinterpret_cast<int64_t>(m_p0TW) << "  buffer w=" << nPixW << " h=" << nPixH << '\n';
		bSetClip = true;
	} else {
		if (bNewSize) {
			// The new size fits into the current cache
			m_refCc->reset_clip();
			m_refCc->begin_new_path();
			bSetClip = true;
		}
	}
	if (bSetClip) {
//std::cout << "MutableThWidgetImpl<TW>::setSize set clip adr:" << reinterpret_cast<int64_t>(m_p0TW) << "  buffer w=" << nPixW << " h=" << nPixH << '\n';
		m_refCc->rectangle(0, 0, nPixW, nPixH);
		m_refCc->clip();
	}
	m_bCacheInvalid = true; //m_bCacheInvalid || bSetClip;
	//
	m_nPixW = nPixW;
	m_nPixH = nPixH;
}
template< class TW >
void MutableThWidgetImpl<TW>::cacheBackgroundAndBase() noexcept
{
	if (! m_bCacheInvalid) {
		return;
	}
//std::string sClass = typeid(TW).name();
//const bool bLevelShow = (sClass.find("LevelShowTWidget") != std::string::npos);
//if (bLevelShow) {
//std::cout << "MutableThWidgetImpl<TW>::cacheBackgroundAndBase " << typeid(TW).name() << " adr:" << reinterpret_cast<int64_t>(m_p0TW) << '\n';
//std::cout << "                         Cur:    m_nPixW=" << m_nPixW << " m_nPixH=" << m_nPixH << '\n';
//}
	ThemeContainerWidget* p0Container = m_p0TW->getParent();
//std::cout << "MutableThWidgetImpl<TW>::drawIfChanged   m_bCacheInvalid x=" << m_nPixX << " y=" << m_nPixY << "  w=" << m_nPixW << " h=" << m_nPixH << '\n';
	if (p0Container != nullptr) {
		// parent(s) redraw the cache
		// since parent always bigger and root must be opaque
		// no need to clear the cache
		m_refCc->save();
		m_refCc->translate(-m_nPixX, -m_nPixY);
		p0Container->drawDeep(m_refCc);
		m_refCc->restore();
	}
	// draw stable part to buffer
//std::cout << " drawIfChanged() draw base to buffer " << typeid(TW).name() << '\n';
	m_refCc->set_operator(Cairo::OPERATOR_OVER);
	drawBase(m_refCc);
	//
	m_bCacheInvalid = false;
}
template< class TW >
void MutableThWidgetImpl<TW>::drawCacheAndVariable(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	refCc->save();
	refCc->rectangle(m_nPixX, m_nPixY, m_nPixW, m_nPixH);
	refCc->clip();
	refCc->set_source(m_refSurf, m_nPixX, m_nPixY);
	refCc->set_operator(Cairo::OPERATOR_SOURCE);
	refCc->paint();
	//refCc->restore();
	//
//std::cout << "                 draw variable " << typeid(TW).name() << '\n';
	//refCc->save();
	// draw variable
	refCc->translate(+m_nPixX, +m_nPixY);
	refCc->set_operator(Cairo::OPERATOR_OVER);
	drawVariable(refCc);
//#ifndef NDEBUG
//refCc->translate(-m_nPixX, -m_nPixY);
//refCc->set_source_rgb(1.0, 0.0, 0.0);
//refCc->set_line_width(1.5);
//refCc->rectangle(m_nPixX + 1, m_nPixY + 1, m_nPixW - 2, m_nPixH - 2);
//refCc->stroke();
//#endif //NDEBUG
	refCc->restore();
}
template< class TW >
bool MutableThWidgetImpl<TW>::drawIfChanged(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
//std::cout << "MutableThWidgetImpl<TW>::drawIfChanged " << typeid(TW).name() << " adr:" << reinterpret_cast<int64_t>(m_p0TW) << '\n';
	cacheBackgroundAndBase();
	const bool bChanged = isChanged();
	if (! bChanged) {
		return false; //--------------------------------------------------------
	}
	drawCacheAndVariable(refCc);
	//
	return true;
}
template< class TW >
void MutableThWidgetImpl<TW>::drawDeep(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	cacheBackgroundAndBase();
	drawCacheAndVariable(refCc);
}
template< class TW >
void MutableThWidgetImpl<TW>::draw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
//if (std::string(typeid(TW).name()).find("LevelShow") == std::string::npos) {
//std::cout << "MutableThWidgetImpl<TW>::draw " << typeid(TW).name() << " adr:" << reinterpret_cast<int64_t>(m_p0TW) << '\n';
//}
	m_bCacheInvalid = true;
	CCSave oCCSave{refCc};
	refCc->set_operator(Cairo::OPERATOR_OVER);
	refCc->translate(+m_nPixX, +m_nPixY);
	drawBase(refCc);
	drawVariable(refCc);
}

} // namespace stmg

#endif	/* STMG_LEAF_MUTABLE_TH_WIDGET_H */

