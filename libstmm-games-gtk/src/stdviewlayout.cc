/*
 * File:   stdviewlayout.cc
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

#include "stdviewlayout.h"
#include "themecontainerwidget.h"

#include "layout.h"
#include "levelshowthemewidget.h"
#include "themewidget.h"
#include "themewidgetinteractive.h"
#include "gtkutil/segmentedfunction.h"
#include "widgets/levelshowwidget.h"

#include <stmm-games/apppreferences.h>
#include <stmm-games/gamewidget.h>
#include <stmm-games/util/basictypes.h>

#include <limits>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <utility>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

StdViewLayout::StdViewLayout(const shared_ptr<Layout>& refGameLayout, const shared_ptr<ThemeWidget>& refRootTW
							, const AppPreferences& oAppPreferences) noexcept
: ThemeLayout(refRootTW)
, m_refGameLayout(refGameLayout)
, m_bIsSubshow(false)
, m_bValid(false)
, m_sErrorString()
{
	assert(refGameLayout);
	assert(refRootTW);
	checkAndGet(oAppPreferences);
	ThemeLayout::recalcSizeFunctions(m_refReferenceWidget.get());
}
void StdViewLayout::reInit(const shared_ptr<Layout>& refGameLayout, const shared_ptr<ThemeWidget>& refRootTW
							, const AppPreferences& oAppPreferences) noexcept
{
	ThemeLayout::reInit(refRootTW);
	//
	assert(refGameLayout);
	m_refGameLayout = refGameLayout;
	m_bIsSubshow = false;
	m_bValid = false;
	m_sErrorString.clear();
	//
	checkAndGet(oAppPreferences);
	ThemeLayout::recalcSizeFunctions(m_refReferenceWidget.get());
}
void StdViewLayout::checkAndGet(const AppPreferences& oAppPreferences) noexcept
{
	m_bIsSubshow = m_refGameLayout->isSubshows();
	int32_t nTotLS;
	if (m_bIsSubshow) {
		nTotLS = oAppPreferences.getTotPlayers();
	} else {
		nTotLS = (m_refGameLayout->isAllTeamsInOneLevel() ? 1 : oAppPreferences.getTotTeams());
	}
	m_refReferenceWidget.reset();
	m_aLevelShow.clear();
	m_aCanChange.clear();
	m_aInteractive.clear();
	//
	std::vector<int32_t> aCanChangeDangling;
	traverse(getRoot(), aCanChangeDangling);
	//
	if (static_cast<int32_t>(m_aLevelShow.size()) != nTotLS) {
		m_sErrorString = "Couldn't generate some of the LevelShowThemeWidget";
		return; //----------------------------------------------------------
	}
	assert(m_refReferenceWidget); // The previous condition should ensure this
	m_bValid = true;
}
void StdViewLayout::traverse(const shared_ptr<ThemeWidget>& refTW, std::vector<int32_t>& aCanChangeDangling) noexcept
{
	assert(refTW);
	auto p0TW = refTW.get();
	int32_t nCanChangeIdx = -1;
	if (p0TW->canChange()) {
//std::cout << "StdViewLayout::traverse canChange adr; " << reinterpret_cast<int64_t>(p0TW) << '\n';
		nCanChangeIdx = static_cast<int32_t>(m_aCanChange.size());
		for (int32_t nCurIdxCanChange : aCanChangeDangling) {
			m_aIfChanged[nCurIdxCanChange] = nCanChangeIdx;
		}
		aCanChangeDangling.clear();
		m_aCanChange.emplace_back(refTW);
		m_aIfChanged.push_back(-1);
	}
	const bool bIsConatiner = p0TW->isContainer();
	if (bIsConatiner) {
		const std::vector< shared_ptr<ThemeWidget> >& aSubTW = static_cast<ThemeContainerWidget*>(p0TW)->getChildren();
		for (auto& refSubTW : aSubTW) {
			traverse(refSubTW, aCanChangeDangling);
		}
	}
	if (nCanChangeIdx != -1) {
		aCanChangeDangling.push_back(nCanChangeIdx);
	}
	if (! bIsConatiner) {
		auto p0LSTW = dynamic_cast<LevelShowThemeWidget*>(p0TW);
		if (p0LSTW != nullptr) {
			auto p0Model = p0TW->getModel().get();
			auto p0LSW = dynamic_cast<LevelShowWidget*>(p0Model);
			if (p0LSW == nullptr) {
				// Only a LevelShowWidget can generate a LevelShowThemeWidget
				assert(false);
				return; //------------------------------------------------------
			}
			m_aLevelShow.emplace_back(std::static_pointer_cast<LevelShowThemeWidget>(refTW));
			if (m_refGameLayout->getReferenceShow().get() == p0LSW) {
				m_refReferenceWidget = std::static_pointer_cast<LevelShowThemeWidget>(refTW);
			}
			if (p0LSW->isInteractive()) {
				m_aInteractive.emplace_back(std::dynamic_pointer_cast<ThemeWidgetInteractive>(refTW));
			}
			return; //----------------------------------------------------------
		}
	}
	// depth first ordering
	auto p0ITW = dynamic_cast<ThemeWidgetInteractive*>(p0TW);
	if ((p0ITW != nullptr) && (p0ITW->isInteractive())) {
		m_aInteractive.emplace_back(std::dynamic_pointer_cast<ThemeWidgetInteractive>(refTW));
	}
}
void StdViewLayout::setSizes(int32_t nLayoutConfig, int32_t nTileW) noexcept
{
	assert(nTileW > 0);
	calcSize(getRoot(), nLayoutConfig, nTileW);
}
void StdViewLayout::calcSize(const shared_ptr<ThemeWidget>& refTW, int32_t nLayoutConfig, int32_t nTileW) noexcept
{
	const auto p0TW = refTW.get();
	assert(p0TW != nullptr);
//std::cout << "StdViewLayout::calcSize refTW=" << reinterpret_cast<int64_t>(refTW.get()) << '\n';
	ThemeLayout::sizeAndConfig(p0TW, nTileW, nLayoutConfig);
	const bool bIsContainer = p0TW->isContainer();
	if (bIsContainer) {
		const std::vector< shared_ptr<ThemeWidget> >& aSubTW = static_cast<ThemeContainerWidget*>(p0TW)->getChildren();
		for (auto& refSubTW : aSubTW) {
			calcSize(refSubTW, nLayoutConfig, nTileW);
		}
	}
}
const std::vector< shared_ptr<LevelShowThemeWidget> >& StdViewLayout::getLevelShowThemeWidgets() const noexcept
{
	return m_aLevelShow;
}
const std::vector< shared_ptr<ThemeWidget> >& StdViewLayout::getCanChangeThemeWidgets() const noexcept
{
	return m_aCanChange;
}
const std::vector< shared_ptr<ThemeWidgetInteractive> >& StdViewLayout::getInteractiveWidgets() const noexcept
{
	return m_aInteractive;
}
int32_t StdViewLayout::getLevelShowWidgetByPos(int32_t nX, int32_t nY) const noexcept
{
	auto itFindLSTW = std::find_if(m_aLevelShow.begin(), m_aLevelShow.end(), [&](const shared_ptr<LevelShowThemeWidget>& refLSTW)
			{
				const NPoint oPixPos = refLSTW->getPos();
				const NSize oPixSize = refLSTW->getSize();
				return (nX >= oPixPos.m_nX) && (nX < oPixPos.m_nX + oPixSize.m_nW) && (nY >= oPixPos.m_nY) && (nY < oPixPos.m_nY + oPixSize.m_nH);
			});
	if (itFindLSTW == m_aLevelShow.end()) {
		return -1; //-----------------------------------------------------------
	}
	return std::distance(m_aLevelShow.begin(), itFindLSTW);
}
int32_t StdViewLayout::getInteractiveWidget(int32_t nX, int32_t nY) const noexcept
{
	return getInteractiveWidget(nX, nY, -1, -1);
}
int32_t StdViewLayout::getInteractiveWidget(int32_t nX, int32_t nY, int32_t nTeam, int32_t nMate) const noexcept
{
	assert((nTeam >= -1) && (nMate >= -1));
	assert(!((nTeam < 0) && (nMate >= 0)));
//std::cout << "StdViewLayout::getInteractiveWidget at pos nX: " << nX << " nY:" << nY << '\n';
	auto itFindITW = std::find_if(m_aInteractive.begin(), m_aInteractive.end(), [&](const shared_ptr<ThemeWidgetInteractive>& refITW)
			{
				auto p0TW = refITW->getThemeWidget();
				const NPoint oTWPos = p0TW->getPos();
				const NSize oTWSize = p0TW->getSize();
				const auto& p0Model = p0TW->getModel().get();
				const int32_t nITWTeam = p0Model->getTeam();
				const int32_t nITWMate = p0Model->getMate();
				if (nTeam >= 0) {
					if (nTeam != nITWTeam) {
						return false; //------------------------------------++++
					}
					if ((nMate >= 0) && (nMate != nITWMate)) {
						return false; //------------------------------------++++
					}
				}
//std::cout << "StdViewLayout::getInteractiveWidget checking TW: " << reinterpret_cast<int64_t>(p0TW) << " nPix:" << nPixX << "," << nPixY << "," << nPixW << "," << nPixH << '\n';
				return (nX >= oTWPos.m_nX) && (nX < oTWPos.m_nX + oTWSize.m_nW) && (nY >= oTWPos.m_nY) && (nY < oTWPos.m_nY + oTWSize.m_nH);
			});
	if (itFindITW == m_aInteractive.end()) {
		return -1; //-----------------------------------------------------------
	}
	return std::distance(m_aInteractive.begin(), itFindITW);
}
void StdViewLayout::calcLayout(int32_t nDestW, int32_t nDestH, int32_t nTileMinW, double fWHTileRatio
								, int32_t& nTileW, int32_t& nTileH, int32_t& nLayoutConfig) noexcept
{
//std::cout << "StdViewLayout::calcLayout" << '\n';
	assert(nDestW > 0);
	assert(nDestH > 0);
	assert(nTileMinW > 0);
	assert(fWHTileRatio > 0.0);
	auto& refRootTW = getRoot();
	assert(refRootTW);
//std::cout << "StdViewLayout::calcLayout  nDestW=" << nDestW << " nDestH=" << nDestH << " fWHTileRatio=" << fWHTileRatio << " nTileMinW=" << nTileMinW << '\n';
	int32_t nBestLayoutConfig = -1;
	int32_t nBestMaxTileW = -1;
	for (int32_t nCurLayoutConfig = 0; nCurLayoutConfig < g_nTotLayoutConfigs; ++nCurLayoutConfig) {
		const std::pair<SegmentedFunction, SegmentedFunction>& oFuns = refRootTW->getSizeFunctions(nCurLayoutConfig);
		const SegmentedFunction& oWFun = oFuns.first;
		const SegmentedFunction& oHFun = oFuns.second;
//#ifndef NDEBUG
//std::cout << "StdViewLayout::calcLayout nLayoutConfig =" << nLayoutConfig << "  oWFun="; oWFun.dump();
//#endif //NDEBUG
//#ifndef NDEBUG
//std::cout << "StdViewLayout::calcLayout nLayoutConfig =" << nLayoutConfig << "  oHFun="; oHFun.dump();
//#endif //NDEBUG
		
		int32_t nPixW = nDestW;
		int32_t nPixH = nDestH;
		{
			const int32_t nMinPixW = oWFun.eval(nTileMinW);
			const int32_t nMinPixH = oHFun.eval(nTileMinW);
			if (nPixW < nMinPixW) {
				nPixW = nMinPixW;
			}
			if (nPixH < nMinPixH) {
				nPixH = nMinPixH;
			}
		}
		int32_t nMaxTileWw = oWFun.evalInverse(nPixW);
		if (nMaxTileWw < 0) {
			nMaxTileWw = std::numeric_limits<int32_t>::max();
		}
		int32_t nMaxTileWh = oHFun.evalInverse(nPixH);
		if (nMaxTileWh < 0) {
			nMaxTileWh = std::numeric_limits<int32_t>::max();
		}
		const int32_t nMaxTileW = std::min(nMaxTileWw, nMaxTileWh);
		if (nMaxTileW < std::numeric_limits<int32_t>::max()) {
			if ((nBestLayoutConfig < 0) || (nMaxTileW >= nBestMaxTileW)) {
				nBestLayoutConfig = nCurLayoutConfig;
				nBestMaxTileW = nMaxTileW;
			}
		}
	}
	if (nBestLayoutConfig < 0) {
		// TODO choose the layout config with biggest area among those
		//      with smallest area outside of {nPixW, nPixH}
		nLayoutConfig = 0;
		nTileW = nTileMinW;
	} else {
		nTileW = nBestMaxTileW;
		nLayoutConfig = nBestLayoutConfig;
	}
	const double fTileH = 1.0 * nTileW / fWHTileRatio;
	nTileH = std::round(fTileH);

	// set the sizes of widgets without position and expansion
	setSizes(nLayoutConfig, nTileW);
//std::cout << "StdViewLayout::calcLayout 4" << '\n';

	NSize oTWSize = refRootTW->getSize();
	const bool bExpandHoriz = refRootTW->getModel()->getWidgetHorizAxisFit(nLayoutConfig).m_bExpand;
	const bool bExpandVert = refRootTW->getModel()->getWidgetVertAxisFit(nLayoutConfig).m_bExpand;
	if (bExpandHoriz) {
		oTWSize.m_nW = nDestW;
	}
	if (bExpandVert) {
		oTWSize.m_nH = nDestH;
	}
//std::cout << "StdViewLayout::calcLayout 5" << '\n';
	const int32_t nPixX = (nDestW - oTWSize.m_nW) / 2;
	const int32_t nPixY = (nDestH - oTWSize.m_nH) / 2;
	NRect oRootRect;
	oRootRect.m_nX = nPixX;
	oRootRect.m_nY = nPixY;
	oRootRect.m_nW = oTWSize.m_nW;
	oRootRect.m_nH = oTWSize.m_nH;
	refRootTW->placeAndMaybeResizeIn(oRootRect);
//std::cout << "StdViewLayout::calcLayout FIN" << '\n';
#ifndef NDEBUG
//std::cout << ""; dump(0);
#endif //NDEBUG
}
void StdViewLayout::draw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	auto& refTW = getRoot();
	traverseDraw(refCc, *refTW, false);
}
void StdViewLayout::traverseDraw(const Cairo::RefPtr<Cairo::Context>& refCc, ThemeWidget& oTW, bool bSkipWidget) noexcept
{
	// containers are drawn before their children
	if (! bSkipWidget) {
//std::cout << "StdViewLayout::traverseDraw TW adr: " << reinterpret_cast<int64_t>(&oTW) << '\n';
		oTW.draw(refCc);
	}
	if (oTW.isContainer()) {
		assert(dynamic_cast<ThemeContainerWidget*>(&oTW) != nullptr);
		ThemeContainerWidget* p0TCW = static_cast<ThemeContainerWidget*>(&oTW);
		auto& aChildren = p0TCW->getChildren();
		for (auto& refTW : aChildren) {
			traverseDraw(refCc, *refTW, false);
		}
	}
}
void StdViewLayout::drawIfChanged(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	const int32_t nTotCanChange = static_cast<int32_t>(m_aCanChange.size());
	int32_t nCurCanChangeIdx = 0;
	while (nCurCanChangeIdx < nTotCanChange) {
		auto& refTW = m_aCanChange[nCurCanChangeIdx];
		const bool bChanged = refTW->drawIfChanged(refCc);
		if (bChanged) {
			traverseDraw(refCc, *refTW, true);
			nCurCanChangeIdx = m_aIfChanged[nCurCanChangeIdx];
			if (nCurCanChangeIdx == -1) {
				break; // while ----
			}
		} else {
			++nCurCanChangeIdx;
		}
	}
}
void StdViewLayout::dump(int32_t
#ifndef NDEBUG
nIndentSpaces
#endif //NDEBUG
) const noexcept
{
	#ifndef NDEBUG
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent << "StdViewLayout adr:" << reinterpret_cast<int64_t>(this) << '\n';
	ThemeLayout::dump(nIndentSpaces);
	#endif //NDEBUG
}

} // namespace stmg
