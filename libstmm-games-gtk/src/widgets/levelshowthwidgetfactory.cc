/*
 * File:   levelshowthwidgetfactory.cc
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

#include "widgets/levelshowthwidgetfactory.h"

#include "themewidget.h"

#include <stmm-games/gamewidget.h>
#include <stmm-games/widgets/levelshowwidget.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <stmm-input/xyevent.h>
#include <stmm-input/event.h>

#include <cassert>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace Glib { template <class T_CppObject> class RefPtr; }
namespace Pango { class Context; }
namespace stmg { class StdTheme; }

namespace stmg
{

LevelShowThWidgetFactory::LevelShowThWidgetFactory(StdTheme* p1Owner, const Frame& oFrame
												, double fMinTop, double fMinBottom, double fMinLeft, double fMinRight) noexcept
: StdThemeWidgetFactory(p1Owner)
, m_oFrame(oFrame)
, m_fMinTop(fMinTop)
, m_fMinBottom(fMinBottom)
, m_fMinLeft(fMinLeft)
, m_fMinRight(fMinRight)
{
	assert(fMinTop >= 0.0);
	assert(fMinBottom >= 0.0);
	assert(fMinLeft >= 0.0);
	assert(fMinRight >= 0.0);
}
shared_ptr<ThemeWidget> LevelShowThWidgetFactory::create(const shared_ptr<GameWidget>& refGameWidget
														, double fTileWHRatio
														, const Glib::RefPtr<Pango::Context>& /*refFontContext*/) noexcept
{
	assert(refGameWidget);
	assert(fTileWHRatio > 0);
	GameWidget* p0GameWidget = refGameWidget.get();
	assert(p0GameWidget != nullptr);
	LevelShowWidget* p0LevelShowWidget = dynamic_cast<LevelShowWidget*>(p0GameWidget);
	if (p0LevelShowWidget == nullptr) {
 		return shared_ptr<ThemeWidget>{};
 	}
	shared_ptr<LevelShowTWidget> refWidget;
	m_oLevelShowTWidgets.create(refWidget, this, refGameWidget, p0LevelShowWidget, fTileWHRatio);
	return std::static_pointer_cast<ThemeWidget>(refWidget);
}
LevelShowThWidgetFactory::LevelShowTWidget::LevelShowTWidget(LevelShowThWidgetFactory* p1Owner
															, const shared_ptr<GameWidget>& refGameWidget
															, LevelShowWidget* p0LevelShowWidget
															, double fTileWHRatio) noexcept
: LevelShowThemeWidget(refGameWidget)
, m_p1Owner(p1Owner)
, m_oLeafTW(this)
, m_nTileW(0)
, m_nTileH(0)
, m_nCanvasPixX(std::numeric_limits<decltype(m_nCanvasPixX)>::min())
, m_nCanvasPixY(std::numeric_limits<decltype(m_nCanvasPixY)>::min())
, m_nCanvasPixW(0)
, m_nCanvasPixH(0)
{
	reInitCommon(p0LevelShowWidget, fTileWHRatio);
}
void LevelShowThWidgetFactory::LevelShowTWidget::reInit(LevelShowThWidgetFactory* p1Owner
														, const shared_ptr<GameWidget>& refGameWidget
														, LevelShowWidget* p0LevelShowWidget
														, double fTileWHRatio) noexcept
{
	LevelShowThemeWidget::reInit(refGameWidget);
	m_p1Owner = p1Owner;
	m_nTileW = 0;
	m_nTileH = 0;
	m_nCanvasPixX = std::numeric_limits<decltype(m_nCanvasPixX)>::min();
	m_nCanvasPixY = std::numeric_limits<decltype(m_nCanvasPixY)>::min();
	m_nCanvasPixW = 0;
	m_nCanvasPixH = 0;
	reInitCommon(p0LevelShowWidget, fTileWHRatio);
}
void LevelShowThWidgetFactory::LevelShowTWidget::reInitCommon(LevelShowWidget* p0LevelShowWidget
															, double fTileWHRatio) noexcept
{
	assert(m_p1Owner != nullptr);
	assert(p0LevelShowWidget != nullptr);

	m_fTop = std::max(m_p1Owner->m_fMinTop, p0LevelShowWidget->getTopMarginInTiles());
	m_fBottom = std::max(m_p1Owner->m_fMinBottom, p0LevelShowWidget->getBottomMarginInTiles());
	m_fLeft = std::max(m_p1Owner->m_fMinLeft, p0LevelShowWidget->getLeftMarginInTiles());
	m_fRight = std::max(m_p1Owner->m_fMinRight, p0LevelShowWidget->getRightMarginInTiles());

	m_oLeafTW.reInit(fTileWHRatio);
}
void LevelShowThWidgetFactory::LevelShowTWidget::dump(int32_t
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
		std::cout << sIndent << "LevelShowThWidgetFactory::LevelShowTWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	if (m_nCanvasPixW > 0) {
		std::cout << sIndent << "  " << "m_nCanvasPix X: " << m_nCanvasPixX << " Y: " << m_nCanvasPixY << " W: " << m_nCanvasPixW << " H: " << m_nCanvasPixH << '\n';
	}
	std::cout << sIndent << "  Margins (tiles): ";
	std::cout << "(Top: " << m_fTop << ", Bottom: " << m_fBottom << ", Left: " << m_fLeft << ", Right: " << m_fRight << ")" << '\n';
	const bool bSizeAndConfigCalled = (m_oLeafTW.getTileW() > 0);
	if (bSizeAndConfigCalled) {
		std::cout << sIndent << "  " << "nTileW       : " << m_oLeafTW.getTileW() << '\n';
		std::cout << sIndent << "  " << "nLayoutConfig: " << m_oLeafTW.getLayoutConfig() << '\n';
	} else {
		std::cout << sIndent << "  " << "nTileW       : Not calculated" << '\n';
		std::cout << sIndent << "  " << "nLayoutConfig: Not calculated" << '\n';
	}
	LevelShowThemeWidget::dump(nIndentSpaces, false);
	#endif //NDEBUG
}
const std::pair<SegmentedFunction, SegmentedFunction>& LevelShowThWidgetFactory::LevelShowTWidget::getSizeFunctions(int32_t /*nLayoutConfig*/) const noexcept
{
//std::cout << "LevelShowThWidgetFactory::LevelShowTWidget::getSizeFunctions" << '\n';
	return m_oSizeFunctions;
}
void LevelShowThWidgetFactory::LevelShowTWidget::onRecalcSizeFunctions(ThemeWidget*
																		#ifndef NDEBUG
																		p0ReferenceThemeWidget
																		#endif //NDEBUG
																		) noexcept
{
//std::cout << "LevelShowThWidgetFactory::LevelShowTWidget::onRecalcSizeFunctions" << '\n';
	#ifndef NDEBUG
	auto p0RefLSW = dynamic_cast<LevelShowWidget*>(p0ReferenceThemeWidget->getModel().get());
	if (p0RefLSW != nullptr) {
		// All LevelShows must have same width and height
		// but might have different borders
		assert(getLevelShowWidget()->getWidth() == p0RefLSW->getWidth());
		assert(getLevelShowWidget()->getHeight() == p0RefLSW->getHeight());
	}
	#endif //NDEBUG
	LevelShowThWidgetFactory*& p0Factory = m_p1Owner;
	if (p0Factory == nullptr) {
		return; //--------------------------------------------------------------
	}
	using SF = SegmentedFunction;
	{
		SF oLeft(SF::max(SF(m_fLeft, 0), SF(0, p0Factory->m_oFrame.m_nPixMinLeft)));
		SF oRight(SF::max(SF(m_fRight, 0), SF(0, p0Factory->m_oFrame.m_nPixMinRight)));
		SF oTop(SF::max(SF(m_fTop, 0), SF(0, p0Factory->m_oFrame.m_nPixMinTop)));
		SF oBottom(SF::max(SF(m_fBottom, 0), SF(0, p0Factory->m_oFrame.m_nPixMinBottom)));
		SF oWInTilesW(SF::add(SF::add(oLeft, oRight), SF(getLevelShowWidget()->getWidth(), 0)));
		SF oHInTilesW(SF::add(SF::add(oTop, oBottom), SF(getLevelShowWidget()->getHeight(), 0)));
		m_oSizeFunctions = std::make_pair(std::move(oWInTilesW), std::move(oHInTilesW));
	}
}
void LevelShowThWidgetFactory::LevelShowTWidget::sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept
{
	assert(nTileW > 0);
	m_oLeafTW.sizeAndConfig(nTileW, nLayoutConfig);
	m_nTileW = nTileW;
	double fTileWHRatio = m_oLeafTW.getTileWHRatio();
	assert(fTileWHRatio > 0);
	m_nTileH = std::round(1.0 * m_nTileW / fTileWHRatio);
	calcCanvas();
}
void LevelShowThWidgetFactory::LevelShowTWidget::calcCanvas() noexcept
{
	LevelShowThWidgetFactory*& p0Factory = m_p1Owner;
	if (p0Factory == nullptr) {
		return;
	}
	const int32_t nPixLeft = std::max(1.0 * p0Factory->m_oFrame.m_nPixMinLeft, m_nTileW * m_fLeft);
	const int32_t nPixRight = std::max(1.0 * p0Factory->m_oFrame.m_nPixMinRight, m_nTileW * m_fRight);
	const int32_t nPixTop = std::max(1.0 * p0Factory->m_oFrame.m_nPixMinTop, m_nTileH * m_fTop);
	const int32_t nPixBottom = std::max(1.0 * p0Factory->m_oFrame.m_nPixMinBottom, m_nTileH * m_fBottom);
//std::cout << "LevelShowTWidget::calcCanvas   nPixLeft=" << nPixLeft << " nPixRight=" << nPixRight << " nPixTop=" << nPixTop << " nPixBottom=" << nPixBottom << '\n';
//std::cout << "LevelShowTWidget::calcCanvas   m_oFrame.m_nPixMinLeft=" << p0Factory->m_oFrame.m_nPixMinLeft << " m_oFrame.m_nPixMinRight=" << p0Factory->m_oFrame.m_nPixMinRight << " m_fLeft=" << m_fLeft << " m_fRight=" << m_fRight << '\n';

	m_nCanvasPixW = m_nTileW * getLevelShowWidget()->getWidth();
	m_nCanvasPixH = m_nTileH * getLevelShowWidget()->getHeight();
	const int32_t nExtraPixW = m_oLeafTW.getPixW() - (nPixLeft + m_nCanvasPixW + nPixRight);
	const int32_t nExtraPixH = m_oLeafTW.getPixH() - (nPixTop + m_nCanvasPixH + nPixBottom);
	m_nCanvasPixX = (nExtraPixW / 2) + nPixLeft;
	m_nCanvasPixY = (nExtraPixH / 2) + nPixTop;
//std::cout << "LevelShowTWidget::calcCanvas   m_nCanvasPixX=" << m_nCanvasPixX << " m_nCanvasPixY=" << m_nCanvasPixY << " m_nCanvasPixW=" << m_nCanvasPixW << " m_nCanvasPixH=" << m_nCanvasPixH << '\n';
}
void LevelShowThWidgetFactory::LevelShowTWidget::placeAndMaybeResizeIn(const NRect& oRect) noexcept
{
	m_oLeafTW.placeAndMaybeResizeIn(oRect);
	calcCanvas();
}
NPoint LevelShowThWidgetFactory::LevelShowTWidget::getCanvasPixPos() noexcept
{
	return {m_nCanvasPixX, m_nCanvasPixY};
}
void LevelShowThWidgetFactory::LevelShowTWidget::drawBase(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
//std::cout << "LevelShowTWidget::drawBase" << '\n';
	LevelShowThWidgetFactory*& p0Factory = m_p1Owner;
	assert(p0Factory != nullptr);
	if ((m_nCanvasPixW <= 0) || (m_nCanvasPixH <= 0)) {
		return; //--------------------------------------------------------------
	}
	// draw background images if any
	const int32_t nPixW = m_oLeafTW.getPixW();
	const int32_t nPixH = m_oLeafTW.getPixH();
	const int32_t nTopPix = m_nCanvasPixY;
	const int32_t nBottomPix = nPixH - m_nCanvasPixH - nTopPix;
	const int32_t nLeftPix = m_nCanvasPixX;
	const int32_t nRightPix = nPixW - m_nCanvasPixW - nLeftPix;
//std::cout << "LevelShowThWidgetFactory::LevelShowTWidget::drawBase()" << '\n';
//std::cout << "              nTopPix=" << nTopPix << " nBottomPix=" << nBottomPix << " nLeftPix=" << nLeftPix << " nRightPix=" << nRightPix << '\n';
//std::cout << "              nPixW=" << nPixW << " nPixH=" << nPixH << '\n';
	const Frame& oFrame = p0Factory->m_oFrame;
//std::cout << "              BaseImg=" << (oFrame.m_refBaseImg ? "T" : "F") << "  CenterImg=" << (oFrame.m_refCenterImg ? "T" : "F") << '\n';
//std::cout << "              LeftImg=" << (oFrame.m_refLeftImg ? "T" : "F") << "  RightImg=" << (oFrame.m_refRightImg ? "T" : "F") << '\n';
	oFrame.draw(refCc, 0, 0, nPixW, nPixH, nTopPix, nBottomPix, nLeftPix, nRightPix);
}
void LevelShowThWidgetFactory::LevelShowTWidget::drawVariable(const Cairo::RefPtr<Cairo::Context>& /*refCc*/) noexcept
{
}
bool LevelShowThWidgetFactory::LevelShowTWidget::handleXYInput(const shared_ptr<stmi::Event>& refXYEvent, int32_t nTeam, int32_t nMate) noexcept
{
	if (nMate < 0) {
		// no target player
		return false; //--------------------------------------------------------
	}
	const NPoint oPixPos = m_oLeafTW.getPos();

	auto p0Event = refXYEvent.get();
	assert(dynamic_cast<stmi::XYEvent*>(p0Event) != nullptr);
	auto p0XYEvent = static_cast<stmi::XYEvent*>(p0Event);

	p0XYEvent->translateXY(- (oPixPos.m_nX + m_nCanvasPixX), - (oPixPos.m_nY + m_nCanvasPixY));
	p0XYEvent->scaleXY(1.0 / m_nTileW, 1.0 / m_nTileH);
	//
	getLevelShowWidget()->handleXYInput(refXYEvent, nTeam, nMate);
	return true;
}

} // namespace stmg
