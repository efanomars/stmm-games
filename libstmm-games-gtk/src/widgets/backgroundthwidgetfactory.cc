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
 * File:   backgroundthwidgetfactory.cc
 */

#include "widgets/backgroundthwidgetfactory.h"

#include "themewidget.h"
#include "stdtheme.h"
#include "gtkutil/ccsave.h"
#include "gtkutil/image.h"
#include "gtkutil/segmentedfunction.h"

#include <stmm-games/gamewidget.h>
#include <stmm-games/named.h>
#include <stmm-games/widgets/backgroundwidget.h>
#include <stmm-games/util/namedindex.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <cairomm/context.h>
#include <cairomm/enums.h>
#include <cairomm/refptr.h>
#include <glibmm/refptr.h>

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace Pango { class Context; }

namespace stmg
{

BackgroundThWidgetFactory::BackgroundThWidgetFactory(StdTheme* p1Owner) noexcept
: StdThemeWidgetFactory(p1Owner)
{
}
shared_ptr<ThemeWidget> BackgroundThWidgetFactory::create(const shared_ptr<GameWidget>& refGameWidget
														, double fTileWHRatio, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept
{
	assert(refGameWidget);
	assert(fTileWHRatio > 0);
	assert(refFontContext);
	StdTheme* p0StdTheme = owner();
	if (p0StdTheme == nullptr) {
		return shared_ptr<ThemeWidget>{}; //------------------------------------
	}
	GameWidget* p0GameWidget = refGameWidget.get();
	assert(p0GameWidget != nullptr);
	BackgroundWidget* p0BackgroundWidget = dynamic_cast<BackgroundWidget*>(p0GameWidget);
	if (p0BackgroundWidget == nullptr) {
		return shared_ptr<ThemeWidget>{}; //------------------------------------
	}
	auto& refChildGW = p0BackgroundWidget->getChildWidget();
	shared_ptr<ThemeWidget> refChildTW = p0StdTheme->createWidget(refChildGW, fTileWHRatio, refFontContext);
	if (!refChildTW) {
		// without the child the background widget doesn't know its size!
		return shared_ptr<ThemeWidget>{}; //------------------------------------
	}

	shared_ptr<BackgroundTWidget> refWidget;
	m_oBackgroundTWidgets.create(refWidget, this, refGameWidget, p0BackgroundWidget, refChildTW, fTileWHRatio);
	return refWidget;
}
BackgroundThWidgetFactory::BackgroundTWidget::BackgroundTWidget(BackgroundThWidgetFactory* p1Owner
																, const shared_ptr<GameWidget>& refGameWidget
																, BackgroundWidget* p0BackgroundWidget
																, const shared_ptr<ThemeWidget>& refChildTW, double fTileWHRatio) noexcept
: ThemeContainerWidget(refGameWidget)
, m_p1Owner(p1Owner)
, m_p0BackgroundWidget(p0BackgroundWidget)
, m_fTileWHRatio(fTileWHRatio)
, m_nTileW(0)
, m_nLayoutConfig(0)
, m_nPixX(0)
, m_nPixY(0)
, m_nPixW(0)
, m_nPixH(0)
{
	reInitCommon(refChildTW);
}
void BackgroundThWidgetFactory::BackgroundTWidget::reInit(BackgroundThWidgetFactory* p1Owner
														, const shared_ptr<GameWidget>& refGameWidget
														, BackgroundWidget* p0BackgroundWidget
														, const shared_ptr<ThemeWidget>& refChildTW, double fTileWHRatio) noexcept
{
	ThemeContainerWidget::reInit(refGameWidget);
	m_p1Owner = p1Owner;
	m_p0BackgroundWidget = p0BackgroundWidget;
	m_fTileWHRatio = fTileWHRatio;
	m_nTileW = 0;
	m_nLayoutConfig = 0;
	m_nPixX = 0;
	m_nPixY = 0;
	m_nPixW = 0;
	m_nPixH = 0;
	reInitCommon(refChildTW);
}
void BackgroundThWidgetFactory::BackgroundTWidget::reInitCommon(const shared_ptr<ThemeWidget>& refChildTW) noexcept
{
	assert(m_p0BackgroundWidget != nullptr);
	assert(m_p1Owner != nullptr);
	m_refBackgImg.reset();
	const int32_t nImgId = m_p0BackgroundWidget->getImgId();
	assert(nImgId >= 0);
	StdTheme* p0StdTheme = m_p1Owner->owner();
	auto& oNamedImages = p0StdTheme->getNamed().images();
	if (oNamedImages.isIndex(nImgId)) {
		m_refBackgImg = p0StdTheme->getImageById(nImgId);
		#ifndef NDEBUG
		if (!m_refBackgImg) {
			std::cout << "Warning! Theme doesn't provide image " << oNamedImages.getName(nImgId) << '\n';
		}
		#endif //NDEBUG
	} else {
		#ifndef NDEBUG
		std::cout << "Warning! Theme doesn't provide image " << oNamedImages.getName(nImgId) << '\n';
		#endif //NDEBUG
	}
	setChildren({refChildTW});
}

void BackgroundThWidgetFactory::BackgroundTWidget::dump(int32_t
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
		std::cout << sIndent << "BackgroundThWidgetFactory::BackgroundTWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	ThemeContainerWidget::dump(nIndentSpaces, false);
	#endif //NDEBUG
}
const std::pair<SegmentedFunction, SegmentedFunction>& BackgroundThWidgetFactory::BackgroundTWidget::getSizeFunctions(int32_t nLayoutConfig) const noexcept
{
	assert((nLayoutConfig >= 0) && (nLayoutConfig < g_nTotLayoutConfigs));
	return m_aSizeFunctions[nLayoutConfig];
}
void BackgroundThWidgetFactory::BackgroundTWidget::sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept
{
	m_nTileW = nTileW;
	m_nLayoutConfig = nLayoutConfig;
	const auto& oPair = getSizeFunctions(nLayoutConfig);
	const SegmentedFunction& oWFun = oPair.first;
	const SegmentedFunction& oHFun = oPair.second;
	const int32_t nPixW = oWFun.eval(m_nTileW);
	const int32_t nPixH = oHFun.eval(m_nTileW / m_fTileWHRatio);
	setSize(nPixW, nPixH);
}
void BackgroundThWidgetFactory::BackgroundTWidget::placeAndMaybeResizeIn(const NRect& oRect) noexcept
{
//std::cout << "BackgroundTWidget::placeAndMaybeResizeIn nX=" << nX << " nY=" << nY << " nW=" << nW << " nH=" << nH << '\n';
	assert(m_p0BackgroundWidget != nullptr);
	NRect oWidget;
	oWidget.m_nW = m_nPixW;
	oWidget.m_nH = m_nPixH;
	const WidgetAxisFit oHoriz = m_p0BackgroundWidget->getWidgetHorizAxisFit(m_nLayoutConfig);
	const WidgetAxisFit oVert = m_p0BackgroundWidget->getWidgetVertAxisFit(m_nLayoutConfig);
	if (oHoriz.getAlign() < 0) {
		oWidget.m_nX = oRect.m_nX;
		oWidget.m_nW = oRect.m_nW;
	} else {
		const int32_t nFreeW = oRect.m_nW - oWidget.m_nW;
		oWidget.m_nX = oRect.m_nX + oHoriz.getAlign() * nFreeW;
	}
	if (oVert.getAlign() < 0) {
		oWidget.m_nY = oRect.m_nY;
		oWidget.m_nH = oRect.m_nH;
	} else {
		const int32_t nFreeH = oRect.m_nH - oWidget.m_nH;
		oWidget.m_nY = oRect.m_nY + oVert.getAlign() * nFreeH;
	}
//std::cout << "                                         nWidgetX=" << nWidgetX << " nWidgetY=" << nWidgetY << " nWidgetW=" << nWidgetW << " nWidgetH=" << nWidgetH << '\n';
	setPos(oWidget.m_nX, oWidget.m_nY);
	setSize(oWidget.m_nW, oWidget.m_nH);
	//
	auto& aChildTW = getChildren();
	assert(aChildTW.size() == 1);
	auto& refChildTW = aChildTW[0];
	refChildTW->placeAndMaybeResizeIn(oWidget);
}
std::pair<SegmentedFunction, SegmentedFunction> BackgroundThWidgetFactory::BackgroundTWidget::calcSizeFunctions(int32_t nLC) noexcept
{
	auto& aChildTW = getChildren();
	assert(aChildTW.size() == 1);
	auto& refChildTW = aChildTW[0];
	const auto& oPair = refChildTW->getSizeFunctions(nLC);
	const SegmentedFunction& oChildFunW = oPair.first;
	const SegmentedFunction& oChildFunH = oPair.second;
	return std::make_pair(oChildFunW, oChildFunH);
}
void BackgroundThWidgetFactory::BackgroundTWidget::onRecalcSizeFunctions(ThemeWidget* /*p0ReferenceThemeWidget*/) noexcept
{
	for (int32_t nLC = 0; nLC < g_nTotLayoutConfigs; ++nLC) {
		m_aSizeFunctions[nLC] = calcSizeFunctions(nLC);
	}
}
bool BackgroundThWidgetFactory::BackgroundTWidget::canChange() const noexcept
{
	return false;
}
NSize BackgroundThWidgetFactory::BackgroundTWidget::getSize() const noexcept
{
	return {m_nPixW, m_nPixH};
}
NPoint BackgroundThWidgetFactory::BackgroundTWidget::getPos() const noexcept
{
	return {m_nPixX, m_nPixY};
}
void BackgroundThWidgetFactory::BackgroundTWidget::setPos(int32_t nPixX, int32_t nPixY) noexcept
{
	m_nPixX = nPixX;
	m_nPixY = nPixY;
}
void BackgroundThWidgetFactory::BackgroundTWidget::setSize(int32_t nPixW, int32_t nPixH) noexcept
{
	m_nPixW = nPixW;
	m_nPixH = nPixH;
}
bool BackgroundThWidgetFactory::BackgroundTWidget::drawIfChanged(const Cairo::RefPtr<Cairo::Context>& /*refCc*/) noexcept
{
	return false;
}
void BackgroundThWidgetFactory::BackgroundTWidget::draw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
//std::cout << "BackgroundTWidget::draw  m_nPixX=" << m_nPixX << " m_nPixY=" << m_nPixY << " m_nPixW=" << m_nPixW << " m_nPixH=" << m_nPixH << '\n';
//std::cout << "BackgroundTWidget::draw  in_clip=" << refCc->in_clip(m_nPixX + m_nPixW - 1, m_nPixY + m_nPixH - 1) << '\n';
	ThemeContainerWidget* p0Container = getParent();
	const bool bHasContainer = (p0Container != nullptr);
	refCc->save();
	if (bHasContainer) {
		refCc->rectangle(m_nPixX, m_nPixY, m_nPixW, m_nPixH);
		refCc->clip();
		p0Container->draw(refCc);
		refCc->set_operator(Cairo::OPERATOR_OVER);
	} else {
		CCSave oCCSave(refCc);
		refCc->set_source_rgba(0.0, 0.0, 0.0, 1.0);
		refCc->set_operator(Cairo::OPERATOR_SOURCE);
		refCc->rectangle(m_nPixX, m_nPixY, m_nPixW, m_nPixH);
		refCc->fill();
	}
	if (m_refBackgImg) {
		m_refBackgImg->draw(refCc, m_nPixX, m_nPixY, m_nPixW, m_nPixH);
	}
	refCc->restore();
}

} // namespace stmg
