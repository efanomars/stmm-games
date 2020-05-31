/*
 * File:   volatilethwidgetfactory.cc
 *
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

#include "widgets/volatilethwidgetfactory.h"

#include "stdtheme.h"
#include "gtkutil/image.h"
#include "gtkutil/ccsave.h"
#include "gtkutil/segmentedfunction.h"

#include <stmm-games/widgets/volatilewidget.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>
#include <stmm-games/gamewidget.h>

#include <cairomm/context.h>
#include <cairomm/enums.h>
#include <cairomm/refptr.h>
#include <glibmm/refptr.h>

#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <string>

namespace Pango { class Context; }

namespace stmg
{

VolatileThWidgetFactory::VolatileThWidgetFactory(StdTheme* p1Owner) noexcept
: StdThemeWidgetFactory(p1Owner)
{
}
shared_ptr<ThemeWidget> VolatileThWidgetFactory::create(const shared_ptr<GameWidget>& refGameWidget
														, double fTileWHRatio, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept
{
//std::cout << "VolatileThWidgetFactory::create A" << '\n';
	assert(refGameWidget);
	assert(fTileWHRatio > 0);
	assert(refFontContext);
	StdTheme* p0StdTheme = owner();
	if (p0StdTheme == nullptr) {
		return shared_ptr<ThemeWidget>{}; //------------------------------------
	}
	GameWidget* p0GameWidget = refGameWidget.get();
	assert(p0GameWidget != nullptr);
	VolatileWidget* p0VolatileWidget = dynamic_cast<VolatileWidget*>(p0GameWidget);
	if (p0VolatileWidget == nullptr) {
		return shared_ptr<ThemeWidget>{}; //------------------------------------
	}
	auto& refChildGW = p0VolatileWidget->getChildWidget();
	shared_ptr<ThemeWidget> refChildTW = p0StdTheme->createWidget(refChildGW, fTileWHRatio, refFontContext);
	if (!refChildTW) {
		// without the child the volatile widget doesn't know its size!
		return shared_ptr<ThemeWidget>{}; //------------------------------------
	}

	shared_ptr<VolatileTWidget> refWidget;
	m_oVolatileTWidgets.create(refWidget, this, refGameWidget, p0VolatileWidget, refChildTW, fTileWHRatio);
	return refWidget;
}
VolatileThWidgetFactory::VolatileTWidget::VolatileTWidget(VolatileThWidgetFactory* p1Owner
																, const shared_ptr<GameWidget>& refGameWidget
																, VolatileWidget* p0VolatileWidget
																, const shared_ptr<ThemeWidget>& refChildTW, double fTileWHRatio) noexcept
: ThemeContainerWidget(refGameWidget)
, m_p1Owner(p1Owner)
, m_p0VolatileWidget(p0VolatileWidget)
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
void VolatileThWidgetFactory::VolatileTWidget::reInit(VolatileThWidgetFactory* p1Owner
														, const shared_ptr<GameWidget>& refGameWidget
														, VolatileWidget* p0VolatileWidget
														, const shared_ptr<ThemeWidget>& refChildTW, double fTileWHRatio) noexcept
{
	ThemeContainerWidget::reInit(refGameWidget);
	m_p1Owner = p1Owner;
	m_p0VolatileWidget = p0VolatileWidget;
	m_fTileWHRatio = fTileWHRatio;
	m_nTileW = 0;
	m_nLayoutConfig = 0;
	m_nPixX = 0;
	m_nPixY = 0;
	m_nPixW = 0;
	m_nPixH = 0;
	//
	m_aCachedImgs.clear();
	
	reInitCommon(refChildTW);
}
void VolatileThWidgetFactory::VolatileTWidget::reInitCommon(const shared_ptr<ThemeWidget>& refChildTW) noexcept
{
	assert(m_p0VolatileWidget != nullptr);
	assert(m_p1Owner != nullptr);
	setChildren({refChildTW});
}

void VolatileThWidgetFactory::VolatileTWidget::dump(int32_t
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
		std::cout << sIndent << "VolatileThWidgetFactory::VolatileTWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	ThemeContainerWidget::dump(nIndentSpaces, false);
	#endif //NDEBUG
}
std::pair<SegmentedFunction, SegmentedFunction> VolatileThWidgetFactory::VolatileTWidget::calcSizeFunctions(int32_t nLC) noexcept
{
	auto& aChildTW = getChildren();
	assert(aChildTW.size() == 1);
	auto& refChildTW = aChildTW[0];
	const auto& oPair = refChildTW->getSizeFunctions(nLC);
	const SegmentedFunction& oChildFunW = oPair.first;
	const SegmentedFunction& oChildFunH = oPair.second;
	return std::make_pair(oChildFunW, oChildFunH);
}
void VolatileThWidgetFactory::VolatileTWidget::onRecalcSizeFunctions(ThemeWidget* /*p0ReferenceThemeWidget*/) noexcept
{
	for (int32_t nLC = 0; nLC < g_nTotLayoutConfigs; ++nLC) {
		m_aSizeFunctions[nLC] = calcSizeFunctions(nLC);
	}
}

const std::pair<SegmentedFunction, SegmentedFunction>& VolatileThWidgetFactory::VolatileTWidget::getSizeFunctions(int32_t nLayoutConfig) const noexcept
{
	assert((nLayoutConfig >= 0) && (nLayoutConfig < g_nTotLayoutConfigs));
	return m_aSizeFunctions[nLayoutConfig];
}
void VolatileThWidgetFactory::VolatileTWidget::sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept
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
void VolatileThWidgetFactory::VolatileTWidget::placeAndMaybeResizeIn(const NRect& oRect) noexcept
{
//std::cout << "VolatileTWidget::placeAndMaybeResizeIn nX=" << nX << " nY=" << nY << " nW=" << nW << " nH=" << nH << '\n';
	assert(m_p0VolatileWidget != nullptr);
	NRect oWidget;
	oWidget.m_nW = m_nPixW;
	oWidget.m_nH = m_nPixH;
	const WidgetAxisFit oHoriz = m_p0VolatileWidget->getWidgetHorizAxisFit(m_nLayoutConfig);
	const WidgetAxisFit oVert = m_p0VolatileWidget->getWidgetVertAxisFit(m_nLayoutConfig);
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
NSize VolatileThWidgetFactory::VolatileTWidget::getSize() const noexcept
{
	return {m_nPixW, m_nPixH};
}
NPoint VolatileThWidgetFactory::VolatileTWidget::getPos() const noexcept
{
	return {m_nPixX, m_nPixY};
}
void VolatileThWidgetFactory::VolatileTWidget::setPos(int32_t nPixX, int32_t nPixY) noexcept
{
	m_nPixX = nPixX;
	m_nPixY = nPixY;
}
void VolatileThWidgetFactory::VolatileTWidget::setSize(int32_t nPixW, int32_t nPixH) noexcept
{
	m_nPixW = nPixW;
	m_nPixH = nPixH;
}
void VolatileThWidgetFactory::VolatileTWidget::loadAndDrawImg(const Cairo::RefPtr<Cairo::Context>& refCc, int32_t nImgId) noexcept
{
//std::cout << "VolatileThWidgetFactory::VolatileTWidget::loadAndDrawImg  nImgId=" << nImgId << '\n';
	auto itCacheImg = std::find_if(m_aCachedImgs.begin(), m_aCachedImgs.end(), [&](const ImgInfo& oImgInfo)
	{
		return (oImgInfo.m_nImgId == nImgId);
	});
	if (itCacheImg == m_aCachedImgs.end()) {
		// image not loaded yet
		m_aCachedImgs.emplace_back();
		ImgInfo& oImgInfo = m_aCachedImgs.back();
		oImgInfo.m_nImgId = nImgId;
		StdTheme* p0StdTheme = m_p1Owner->owner();
		oImgInfo.m_refImg = p0StdTheme->getImageById(nImgId);
		itCacheImg = m_aCachedImgs.begin() + (m_aCachedImgs.size() - 1);
	}
	shared_ptr<Image>& refImg = itCacheImg->m_refImg;
	assert(refImg);
	refImg->draw(refCc, m_nPixX, m_nPixY, m_nPixW, m_nPixH);
}
bool VolatileThWidgetFactory::VolatileTWidget::drawIfChanged(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	const bool bWidgetChanged = m_p0VolatileWidget->isChanged();
	if (!bWidgetChanged) {
		return false; //--------------------------------------------------------
	}
	CCSave oCCSave(refCc);
	{
		ThemeContainerWidget* p0Container = getParent();
		if (p0Container != nullptr) {
			// parent needs to be redrawn
			refCc->rectangle(m_nPixX, m_nPixY, m_nPixW, m_nPixH);
			refCc->clip();
			p0Container->drawDeep(refCc);
			refCc->set_operator(Cairo::OPERATOR_OVER);
		} else {
			CCSave oCCSaveParent(refCc);
			refCc->set_source_rgba(0.0, 0.0, 0.0, 0.0);
			refCc->set_operator(Cairo::OPERATOR_SOURCE);
			refCc->rectangle(m_nPixX, m_nPixY, m_nPixW, m_nPixH);
			refCc->fill();
		}
	}
	// draw image (if any)
	const int32_t nImgId = m_p0VolatileWidget->getCurrentImgId();
	if (nImgId < 0) {
		// there's no image
		return true; //---------------------------------------------------------
	}
	loadAndDrawImg(refCc, nImgId);
	return true;
}
void VolatileThWidgetFactory::VolatileTWidget::draw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	CCSave oCCSave(refCc);
	ThemeContainerWidget* p0Container = getParent();
	if (p0Container != nullptr) {
		refCc->rectangle(m_nPixX, m_nPixY, m_nPixW, m_nPixH);
		refCc->clip();
		refCc->set_operator(Cairo::OPERATOR_OVER);
	} else {
		CCSave oCCSaveParent(refCc);
		refCc->set_source_rgba(0.0, 0.0, 0.0, 0.0);
		refCc->set_operator(Cairo::OPERATOR_SOURCE);
		refCc->rectangle(m_nPixX, m_nPixY, m_nPixW, m_nPixH);
		refCc->fill();
	}
	// draw image (if any)
	const int32_t nImgId = m_p0VolatileWidget->getCurrentImgId();
	if (nImgId < 0) {
		// there's no image
		return; //--------------------------------------------------------------
	}
	loadAndDrawImg(refCc, nImgId);
}

} // namespace stmg
