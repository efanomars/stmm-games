/*
 * File:   boxthemewidgetfactory.h
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

#ifndef STMG_BOX_THEME_WIDGET_FACTORY_H
#define STMG_BOX_THEME_WIDGET_FACTORY_H

#include "stdthemewidgetfactory.h"
#include "stdtheme.h"
#include "themecontainerwidget.h"

#include "gtkutil/segmentedfunction.h"

#include <stmm-games/gamewidget.h>
#include <stmm-games/widgets/boxwidget.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <glibmm/refptr.h>

#include <array>
#include <utility>
#include <memory>
#include <cassert>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include <stdint.h>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace Pango { class Context; }
namespace stmg { class ThemeWidget; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

/** Template class for Box theme widgets.
 * Params:
 *     TOwnerFactory The class creating instances.
 *     TBoxThemeWidget The actual ThemeWidget subclassing this template.
 *     TBoxWidget The model box widget.
 */
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
class BoxThemeWidget : public ThemeContainerWidget
{
public:
	/** Constructor.
	 * @param p1Owner The factory class creating instance. Cannot be null.
	 * @param refGameWidget The model. Cannot be null.
	 * @param p0BoxWidget The model. Must point to the same object as refGameWidget.
	 * @param aChildTW The children.
	 * @param fTileWHRatio The tile WH ratio.
	 */
	BoxThemeWidget(TOwnerFactory* p1Owner, const shared_ptr<GameWidget>& refGameWidget, TBoxWidget* p0BoxWidget
				, const std::vector< shared_ptr<ThemeWidget> >& aChildTW, double fTileWHRatio) noexcept;
	/** Reinitialization.
	 * See constructor.
	 */
	void reInit(TOwnerFactory* p1Owner, const shared_ptr<GameWidget>& refGameWidget, TBoxWidget* p0BoxWidget
				, const std::vector< shared_ptr<ThemeWidget> >& aChildTW, double fTileWHRatio) noexcept;

	bool canChange() const noexcept override;

	NSize getSize() const noexcept override;
	NPoint getPos() const noexcept override;
	bool drawIfChanged(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override;
	void draw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override;
	const std::pair<SegmentedFunction, SegmentedFunction>& getSizeFunctions(int32_t nLayoutConfig) const noexcept override;
	void placeAndMaybeResizeIn(const NRect& oRect) noexcept override;
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	void onAssignedToLayout() noexcept override;
	void onRecalcSizeFunctions(ThemeWidget* p0ReferenceThemeWidget) noexcept override;
	void sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept override;
	/** The pointer to the model.
	 * @return The model. Is not null.
	 */
	inline TBoxWidget* getBoxWidgetModel() noexcept { return m_p0BoxWidget; }
private:
	std::pair<SegmentedFunction, SegmentedFunction> calcSizeFunctions(int32_t nLayoutConfig) noexcept;
	void setPos(int32_t nPixX, int32_t nPixY) noexcept;
	void setSize(int32_t nPixW, int32_t nPixH) noexcept;
private:
	friend TOwnerFactory;
	TOwnerFactory* m_p1Owner;
	TBoxWidget* m_p0BoxWidget;
	double m_fTileWHRatio;
	int32_t m_nTileW;
	int32_t m_nLayoutConfig;
	int32_t m_nPixX;
	int32_t m_nPixY;
	int32_t m_nPixW;
	int32_t m_nPixH;
	std::array< std::pair<SegmentedFunction, SegmentedFunction>, g_nTotLayoutConfigs> m_aSizeFunctions;
};

template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>
::BoxThemeWidget(TOwnerFactory* p1Owner
				, const shared_ptr<GameWidget>& refGameWidget, TBoxWidget* p0BoxWidget
				, const std::vector< shared_ptr<ThemeWidget> >& aChildTW, double fTileWHRatio) noexcept
: ThemeContainerWidget(refGameWidget)
, m_p1Owner(p1Owner)
, m_p0BoxWidget(p0BoxWidget)
, m_fTileWHRatio(fTileWHRatio)
, m_nTileW(0)
, m_nLayoutConfig(0)
, m_nPixX(0)
, m_nPixY(0)
, m_nPixW(0)
, m_nPixH(0)
{
	setChildren(aChildTW);
}

template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
void BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>
::reInit(TOwnerFactory* p1Owner
		, const shared_ptr<GameWidget>& refGameWidget, TBoxWidget* p0BoxWidget
		, const std::vector< shared_ptr<ThemeWidget> >& aChildTW, double fTileWHRatio) noexcept
{
	ThemeContainerWidget::reInit(refGameWidget);
	m_p1Owner = p1Owner;
	m_p0BoxWidget = p0BoxWidget;
	m_fTileWHRatio = fTileWHRatio;
	m_nTileW = 0;
	m_nLayoutConfig = 0;
	m_nPixX = 0;
	m_nPixY = 0;
	m_nPixW = 0;
	m_nPixH = 0;
	setChildren(aChildTW);
}
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
void BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>::dump(int32_t
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
		std::cout << sIndent << "BoxThWidgetFactory::BoxTWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	ThemeContainerWidget::dump(nIndentSpaces, false);
	#endif //NDEBUG
}
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
const std::pair<SegmentedFunction, SegmentedFunction>& BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>
::getSizeFunctions(int32_t nLayoutConfig) const noexcept
{
//std::cout << "BoxThWidgetFactory::BoxTWidget::getSizeFunctions" << '\n';
	assert((nLayoutConfig >= 0) && (nLayoutConfig < g_nTotLayoutConfigs));
	return m_aSizeFunctions[nLayoutConfig];
}
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
void BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>::sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept
{
	m_nTileW = nTileW;
	m_nLayoutConfig = nLayoutConfig;
	const auto& oPair = getSizeFunctions(nLayoutConfig);
	const SegmentedFunction& oWFun = oPair.first;
	const SegmentedFunction& oHFun = oPair.second;
	const int32_t nPixW = oWFun.eval(m_nTileW);
	const int32_t nPixH = oHFun.eval(1.0 * m_nTileW / m_fTileWHRatio);
	setSize(nPixW, nPixH);
}
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
void BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>::setSize(int32_t nPixW, int32_t nPixH) noexcept
{
	m_nPixW = nPixW;
	m_nPixH = nPixH;
}
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
void BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>
::placeAndMaybeResizeIn(const NRect& oRect) noexcept
{
//std::cout << "BoxThemeWidget<>::placeAndMaybeResizeIn adr=" << reinterpret_cast<int64_t>(this);
//std::cout << "  (" << oRect.m_nX << "," << oRect.m_nY << "," << oRect.m_nW << "," << oRect.m_nH << ")" << '\n';
	assert(m_p0BoxWidget != nullptr);
	NRect oWidgetRect;
	oWidgetRect.m_nW = m_nPixW;
	oWidgetRect.m_nH = m_nPixH;
	const WidgetAxisFit oHoriz = m_p0BoxWidget->getWidgetHorizAxisFit(m_nLayoutConfig);
	const WidgetAxisFit oVert = m_p0BoxWidget->getWidgetVertAxisFit(m_nLayoutConfig);
	if (oHoriz.getAlign() < 0) {
		oWidgetRect.m_nX = oRect.m_nX;
		oWidgetRect.m_nW = oRect.m_nW;
	} else {
		const int32_t nFreeW = oRect.m_nW - oWidgetRect.m_nW;
		oWidgetRect.m_nX = oRect.m_nX + oHoriz.getAlign() * nFreeW;
	}
	if (oVert.getAlign() < 0) {
		oWidgetRect.m_nY = oRect.m_nY;
		oWidgetRect.m_nH = oRect.m_nH;
	} else {
		const int32_t nFreeH = oRect.m_nH - oWidgetRect.m_nH;
		oWidgetRect.m_nY = oRect.m_nY + oVert.getAlign() * nFreeH;
	}
	setPos(oWidgetRect.m_nX, oWidgetRect.m_nY);
	setSize(oWidgetRect.m_nW, oWidgetRect.m_nH);
//std::cout << "BoxThemeWidget<>::placeAndMaybeResizeIn oWidgetRect.m_nX=" << oWidgetRect.m_nX << "  oWidgetRect.m_nY=" << oWidgetRect.m_nY << '\n';
//std::cout << "                                        oWidgetRect.m_nW=" << oWidgetRect.m_nW << "  oWidgetRect.m_nH=" << oWidgetRect.m_nH << '\n';
	// This is a rule for containers:
	// they have to set their size and position before calling
	// placeAndMaybeResizeIn() of their children. Why?

	bool bIsVertical = m_p0BoxWidget->isVertical(m_nLayoutConfig);
	bool bInvertChildren = m_p0BoxWidget->hasInvertedChildren(m_nLayoutConfig);
//std::cout << "BoxThemeWidget<>::placeAndMaybeResizeIn m_nLayoutConfig=" << m_nLayoutConfig << " bIsVertical=" << bIsVertical << " bInvertChildren=" << bInvertChildren << '\n';
	int32_t nChildSumW = 0;
	//int32_t nChildMaxW = 0;
	int32_t nChildSumH = 0;
	//int32_t nChildMaxH = 0;
	int32_t nTotExpandHoriz = 0;
	int32_t nTotExpandVert = 0;
	const auto& aChildren = getChildren();
	const int32_t nTotChildren = static_cast<int32_t>(aChildren.size());
	const int32_t nIdxInc = (bInvertChildren ? -1 : +1);
	int32_t nIdx = (bInvertChildren ? nTotChildren - 1 : 0);
	for (; (nIdx >= 0) && (nIdx < nTotChildren); nIdx += nIdxInc) {
		auto& refChildTW = aChildren[nIdx];
		auto& refChildTWModel = refChildTW->getModel();
		const NSize oChildSize = refChildTW->getSize();
//std::cout << "BoxThemeWidget<>::placeAndMaybeResizeIn child adr=" << reinterpret_cast<int64_t>(refChildTW.get()) << '\n';
//std::cout << "                                        nIdx=" << nIdx << "  oChildSize.m_nW=" << oChildSize.m_nW << "  oChildSize.m_nH=" << oChildSize.m_nH << '\n';
		nChildSumW += oChildSize.m_nW;
		//nChildMaxW += std::max(nChildW, nChildMaxW);
		nChildSumH += oChildSize.m_nH;
		//nChildMaxH += std::max(nChildH, nChildMaxH);
		if (refChildTWModel->getWidgetHorizAxisFit(m_nLayoutConfig).m_bExpand) {
			++nTotExpandHoriz;
		}
		if (refChildTWModel->getWidgetVertAxisFit(m_nLayoutConfig).m_bExpand) {
			++nTotExpandVert;
		}
	}
	const int32_t nRestW = (bIsVertical ? 0 : (oWidgetRect.m_nW - nChildSumW));
	const int32_t nRestH = (bIsVertical ? (oWidgetRect.m_nH - nChildSumH) : 0);
	int32_t nAddW = 0;
	if ((nTotExpandHoriz > 0) && !bIsVertical) {
		nAddW = 1.0 * nRestW / nTotExpandHoriz;
	}
	int32_t nAddH = 0;
	if ((nTotExpandVert > 0) && bIsVertical) {
		nAddH = 1.0 * nRestH / nTotExpandVert;
	}
//std::cout << "BoxThemeWidget<>::placeAndMaybeResizeIn nChildSumW=" << nChildSumW << "  nChildSumH=" << nChildSumH << '\n';
//std::cout << "                                        nTotExpandHoriz=" << nTotExpandHoriz << "  nTotExpandVert=" << nTotExpandVert << '\n';
//std::cout << "                                        nAddW=" << nAddW << "  nAddH=" << nAddH << '\n';

	int32_t nCurPosX = oWidgetRect.m_nX;
	int32_t nCurPosY = oWidgetRect.m_nY;
	nIdx = (bInvertChildren ? nTotChildren - 1 : 0);
	for (; (nIdx >= 0) && (nIdx < nTotChildren); nIdx += nIdxInc) {
		auto& refChildTW = aChildren[nIdx];
//std::cout << "BoxThemeWidget<>::placeAndMaybeResizeIn CHILD adr=" << reinterpret_cast<int64_t>(refChildTW.get()) << '\n';
//std::cout << "                                        nIdx=" << nIdx << "  nCurPosX=" << nCurPosX << "  nCurPosY=" << nCurPosY << '\n';
		auto& refChildTWModel = refChildTW->getModel();
		NSize oChildSize = refChildTW->getSize();
		if (bIsVertical) {
			if ((nAddH > 0) && refChildTWModel->getWidgetVertAxisFit(m_nLayoutConfig).m_bExpand) {
				oChildSize.m_nH += nAddH;
			}
			if (nCurPosY + oChildSize.m_nH > oWidgetRect.m_nY + oWidgetRect.m_nH) {
				oChildSize.m_nH = oWidgetRect.m_nY + oWidgetRect.m_nH - nCurPosY;
			}
			NRect oChildRect;
			oChildRect.m_nX = nCurPosX;
			oChildRect.m_nY = nCurPosY;
			oChildRect.m_nW = oWidgetRect.m_nW;
			oChildRect.m_nH = oChildSize.m_nH;
//std::cout << "                                        oChildRect.m_nW=" << oChildRect.m_nW << "  oChildRect.m_nH=" << oChildRect.m_nH << '\n';
			refChildTW->placeAndMaybeResizeIn(oChildRect);
			nCurPosY += oChildSize.m_nH;
		} else {
			if ((nAddW > 0) && refChildTWModel->getWidgetHorizAxisFit(m_nLayoutConfig).m_bExpand) {
				oChildSize.m_nW += nAddW;
			}
			if (nCurPosX + oChildSize.m_nW > oWidgetRect.m_nX + oWidgetRect.m_nW) {
				oChildSize.m_nW = oWidgetRect.m_nX + oWidgetRect.m_nW - nCurPosX;
			}
			NRect oChildRect;
			oChildRect.m_nX = nCurPosX;
			oChildRect.m_nY = nCurPosY;
			oChildRect.m_nW = oChildSize.m_nW;
			oChildRect.m_nH = oWidgetRect.m_nH;
//std::cout << "                                        oChildRect.m_nW=" << oChildRect.m_nW << "  oChildRect.m_nH=" << oChildRect.m_nH << '\n';
			refChildTW->placeAndMaybeResizeIn(oChildRect);
			nCurPosX += oChildSize.m_nW;
		}
	}
}
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
void BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>::onAssignedToLayout() noexcept
{
}
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
std::pair<SegmentedFunction, SegmentedFunction> BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>
::calcSizeFunctions(int32_t nLayoutConfig) noexcept
{
	const bool bIsVertical = m_p0BoxWidget->isVertical(nLayoutConfig);
	SegmentedFunction oFunW;
	SegmentedFunction oFunH;
	auto& aChildTW = getChildren();
//std::cout << "BoxTWidget::calcSizeFunctions bIsVertical=" << bIsVertical << "  aChildTW.size()=" << aChildTW.size() << '\n';
	for (auto& refChildTW : aChildTW) {
		const auto& oPair = refChildTW->getSizeFunctions(nLayoutConfig);
		const SegmentedFunction& oChildFunW = oPair.first;
		const SegmentedFunction& oChildFunH = oPair.second;
		if (bIsVertical) {
			oFunW = SegmentedFunction::max(oFunW, oChildFunW);
			oFunH = SegmentedFunction::add(oFunH, oChildFunH);
		} else {
			oFunW = SegmentedFunction::add(oFunW, oChildFunW);
			oFunH = SegmentedFunction::max(oFunH, oChildFunH);
		}
	}
	return std::make_pair(std::move(oFunW), std::move(oFunH));
}
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
void BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>::onRecalcSizeFunctions(ThemeWidget* /*p0ReferenceThemeWidget*/) noexcept
{
//std::cout << "BoxThWidgetFactory::BoxTWidget::onRecalcSizeFunctions this=" << reinterpret_cast<int64_t>(this) << '\n';
	for (int32_t nLC = 0; nLC < g_nTotLayoutConfigs; ++nLC) {
		m_aSizeFunctions[nLC] = calcSizeFunctions(nLC);
	}
}
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
bool BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>::canChange() const noexcept
{
	return false;
}
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
NSize BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>::getSize() const noexcept
{
	return {m_nPixW, m_nPixH};
}
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
NPoint BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>::getPos() const noexcept
{
	return {m_nPixX, m_nPixY};
}
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
void BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>::setPos(int32_t nPixX, int32_t nPixY) noexcept
{
	m_nPixX = nPixX;
	m_nPixY = nPixY;
}
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
bool BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>::drawIfChanged(const Cairo::RefPtr<Cairo::Context>& /*refCc*/) noexcept
{
	return false;
}
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
void BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>::draw(const Cairo::RefPtr<Cairo::Context>& /*refCc*/) noexcept
{
//std::cout << "BoxThemeWidget<>::draw m_nPixX=" << m_nPixX << " m_nPixY=" << m_nPixY << " m_nPixW=" << m_nPixW << " m_nPixH=" << m_nPixH << '\n';
}

////////////////////////////////////////////////////////////////////////////////
template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
class BoxThemeWidgetFactory : public StdThemeWidgetFactory
{
public:
	BoxThemeWidgetFactory(StdTheme* p1Owner) noexcept;
	shared_ptr<ThemeWidget> create(const shared_ptr<GameWidget>& refGameWidget
									, double fTileWHRatio, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept override;
private:
	Recycler<TBoxThemeWidget> m_oBoxThemeWidgets;
};

template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
BoxThemeWidgetFactory<TOwnerFactory, TBoxThemeWidget, TBoxWidget>::BoxThemeWidgetFactory(StdTheme* p1Owner) noexcept
: StdThemeWidgetFactory(p1Owner)
{
	static_assert(std::is_base_of<BoxThemeWidgetFactory<TOwnerFactory, TBoxThemeWidget, TBoxWidget>, TOwnerFactory>::value, "");
	static_assert(std::is_base_of<BoxThemeWidget<TOwnerFactory, TBoxThemeWidget, TBoxWidget>, TBoxThemeWidget>::value, "");
	static_assert(std::is_base_of<BoxWidget, TBoxWidget>::value, "");
}

template<class TOwnerFactory, class TBoxThemeWidget, class TBoxWidget>
shared_ptr<ThemeWidget> BoxThemeWidgetFactory<TOwnerFactory, TBoxThemeWidget, TBoxWidget>::create(
													const shared_ptr<GameWidget>& refGameWidget
													, double fTileWHRatio
													, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept
{
//std::cout << "BoxThWidgetFactory::create FACTORY:" << reinterpret_cast<int64_t>(this) << '\n';
	assert(refGameWidget);
	assert(fTileWHRatio > 0);
	assert(refFontContext);
	StdTheme* p0StdTheme = owner();
	if (p0StdTheme == nullptr) {
		return shared_ptr<ThemeWidget>{}; //------------------------------------
	}
	GameWidget* p0GameWidget = refGameWidget.get();
	assert(p0GameWidget != nullptr);
	TBoxWidget* p0BoxWidget = dynamic_cast<TBoxWidget*>(p0GameWidget);
	if (p0BoxWidget == nullptr) {
 		return shared_ptr<ThemeWidget>{}; //------------------------------------
 	}
	std::vector< shared_ptr<ThemeWidget> > aChildTW;
	auto& aChildWidget = p0BoxWidget->getChildren();
	for (auto& refChildGW : aChildWidget) {
		auto refChildTW = p0StdTheme->createWidget(refChildGW, fTileWHRatio, refFontContext);
		if (refChildTW) {
			aChildTW.push_back(refChildTW);
		}
	}
	if (aChildTW.empty()) {
		// no children, don't even create the widget.
		return shared_ptr<ThemeWidget>{}; //------------------------------------
	}
	shared_ptr<TBoxThemeWidget> refWidget;
	m_oBoxThemeWidgets.create(refWidget, static_cast<TOwnerFactory*>(this)
							, refGameWidget, p0BoxWidget, aChildTW, fTileWHRatio);
	return refWidget;
}

} // namespace stmg

#endif	/* STMG_BOX_THEME_WIDGET_FACTORY_H */

