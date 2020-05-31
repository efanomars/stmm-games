/*
 * File:   progressthwidgetfactory.cc
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

#include "widgets/progressthwidgetfactory.h"

#include "stdtheme.h"

#include <stmm-games/tile.h>
#include <stmm-games/variable.h>
#include <stmm-games/gamewidget.h>
#include <stmm-games/util/intset.h>
#include <stmm-games/widgets/progresswidget.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <cairomm/context.h>
#include <cairomm/refptr.h>

#include <cassert>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <stdint.h>

namespace Glib { template <class T_CppObject> class RefPtr; }
namespace Pango { class Context; }
namespace stmg { class SegmentedFunction; }

namespace stmg
{

ProgressThWidgetFactory::ProgressThWidgetFactory(StdTheme* p1Owner
												, const TileColor& oNormalColor, const TileColor& oDangerColor
												, const Frame& oFrame) noexcept
: StdThemeWidgetFactory(p1Owner)
, m_oNormalColor(oNormalColor)
, m_oDangerColor(oDangerColor)
, m_oFrame(oFrame)
{
	assert(p1Owner != nullptr);
}
shared_ptr<ThemeWidget> ProgressThWidgetFactory::create(const shared_ptr<GameWidget>& refGameWidget
												, double fTileWHRatio
												, const Glib::RefPtr<Pango::Context>& /*refFontContext*/) noexcept
{
//std::cout << "ProgressThWidgetFactory::create" << '\n';
	assert(refGameWidget);
	assert(fTileWHRatio > 0);
	GameWidget* p0GameWidget = refGameWidget.get();
	assert(p0GameWidget != nullptr);
	ProgressWidget* p0ProgressWidget = dynamic_cast<ProgressWidget*>(p0GameWidget);
	if (p0ProgressWidget == nullptr) {
 		return shared_ptr<ThemeWidget>{};
 	}
	shared_ptr<ProgressTWidget> refWidget;
	m_oProgressTWidgets.create(refWidget, this, refGameWidget, p0ProgressWidget, fTileWHRatio);
	return refWidget;
}
ProgressThWidgetFactory::ProgressTWidget::ProgressTWidget(ProgressThWidgetFactory* p1Owner
														, const shared_ptr<GameWidget>& refGameWidget, ProgressWidget* p0ProgressWidget
														, double fTileWHRatio) noexcept
: ThemeWidget(refGameWidget)
, m_p1Owner(p1Owner)
, m_p0ProgressWidget(p0ProgressWidget)
, m_oMutaTW(this)
, m_oSizedTW(this)
, m_nPixCanvasX(0)
, m_nPixCanvasY(0)
, m_nPixCanvasW(0)
, m_nPixCanvasH(0)
{
	reInitCommon(fTileWHRatio);
}
void ProgressThWidgetFactory::ProgressTWidget::reInit(ProgressThWidgetFactory* p1Owner
													, const shared_ptr<GameWidget>& refGameWidget, ProgressWidget* p0ProgressWidget
													, double fTileWHRatio) noexcept
{
//std::cout << "  ProgressThWidgetFactory::ProgressTWidget::reInit()  p0ProgressWidget=" << reinterpret_cast<int64_t>(p0ProgressWidget) << '\n';
	ThemeWidget::reInit(refGameWidget);
	m_p1Owner = p1Owner;
	m_p0ProgressWidget = p0ProgressWidget;
	m_nPixCanvasX = 0;
	m_nPixCanvasY = 0;
	m_nPixCanvasW = 0;
	m_nPixCanvasH = 0;
	//
	reInitCommon(fTileWHRatio);
}
void ProgressThWidgetFactory::ProgressTWidget::reInitCommon(double fTileWHRatio) noexcept
{
	assert(m_p1Owner != nullptr);
	assert(m_p0ProgressWidget != nullptr);
	//
	m_nPixMinCanvasWidth = 20;
	m_nPixMinCanvasHeight = 20;
	m_oMutaTW.reInit(fTileWHRatio);
	m_oSizedTW.reInit(m_p0ProgressWidget);
	m_nMinValue = m_p0ProgressWidget->getMinValue();
	m_nMaxValue = m_p0ProgressWidget->getMaxValue();
	m_nThresholdValue = m_p0ProgressWidget->getThresholdValue();
	m_bThresholdValid = ((m_nThresholdValue >= m_nMinValue) && (m_nThresholdValue <= m_nMaxValue));
	m_bDangerBelow = m_p0ProgressWidget->getDangerBelow();
}

void ProgressThWidgetFactory::ProgressTWidget::dump(int32_t
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
		std::cout << sIndent << "ProgressThWidgetFactory::ProgressTWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	ThemeWidget::dump(nIndentSpaces, false);
	if (m_nPixCanvasW > 0) {
		std::cout << sIndent << "  " << "m_nPixCanvas: (" << m_nPixCanvasX << ", " << m_nPixCanvasY << ", " << m_nPixCanvasW << ", " << m_nPixCanvasH << ")" << '\n';
		//std::cout << sIndent << "  " << "m_fTitleAlign: " << m_fTitleAlign << '\n';
	}
	#endif //NDEBUG
}
NSize ProgressThWidgetFactory::ProgressTWidget::getMinSize(int32_t /*nLayoutConfig*/) const noexcept
{
	ProgressThWidgetFactory* const & p0Factory = m_p1Owner;
	if (p0Factory == nullptr) {
		return NSize{0,0};
	}
	const Frame& oFrame = p0Factory->m_oFrame;
	const int32_t nPixW = m_nPixMinCanvasWidth + oFrame.m_nPixMinLeft + oFrame.m_nPixMinRight;
	const int32_t nPixH = m_nPixMinCanvasHeight + oFrame.m_nPixMinTop + oFrame.m_nPixMinBottom;
	return NSize{nPixW, nPixH};
}
void ProgressThWidgetFactory::ProgressTWidget::calcCanvas() noexcept
{
	ProgressThWidgetFactory*& p0Factory = m_p1Owner;
	if (p0Factory == nullptr) {
		return;
	}
	const Frame& oFrame = p0Factory->m_oFrame;
	m_nPixCanvasX = oFrame.m_nPixMinLeft;
	m_nPixCanvasY = oFrame.m_nPixMinTop;
	m_nPixCanvasW = std::max(0, m_oMutaTW.getPixW() - oFrame.m_nPixMinLeft - oFrame.m_nPixMinRight);
	m_nPixCanvasH = std::max(0, m_oMutaTW.getPixH() - oFrame.m_nPixMinTop - oFrame.m_nPixMinBottom);
//std::cout << "ProgressThWidgetFactory::ProgressTWidget::calcCanvas()  m_nPixCanvasW=" << m_nPixCanvasW << "  m_nPixCanvasH=" << m_nPixCanvasH << '\n';
}
void ProgressThWidgetFactory::ProgressTWidget::sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept
{
	m_oMutaTW.sizeAndConfig(nTileW, nLayoutConfig);
	calcCanvas();
}
void ProgressThWidgetFactory::ProgressTWidget::placeAndMaybeResizeIn(const NRect& oRect) noexcept
{
//std::cout << "ProgressThWidgetFactory::ProgressTWidget::placeAndMaybeResizeIn()  nX=" << nX << "  nY=" << nY << "   nW=" << nW << "  nH=" << nH << '\n';
	m_oMutaTW.placeAndMaybeResizeIn(oRect);
	calcCanvas();
//std::cout << "ProgressThWidgetFactory::ProgressTWidget::placeAndMaybeResizeIn()  nResX=" << m_oSizedTW.getPixX() << "  nResY=" << m_oSizedTW.getPixY() 
//<< "   nResW=" << m_oSizedTW.getPixW() << "  nResH=" << m_oSizedTW.getPixH() << '\n';
}
const std::pair<SegmentedFunction, SegmentedFunction>& ProgressThWidgetFactory::ProgressTWidget::getSizeFunctions(int32_t nLayoutConfig) const noexcept
{
//std::cout << "ProgressThWidgetFactory::ProgressTWidget::getSizeFunctions this=" << reinterpret_cast<int64_t>(this) << '\n';
	return m_oSizedTW.getSizeFunctions(nLayoutConfig);
}
void ProgressThWidgetFactory::ProgressTWidget::drawBase(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	ProgressThWidgetFactory*& p0Factory = m_p1Owner;
	assert(p0Factory != nullptr);
	if ((m_nPixCanvasW <= 0) || (m_nPixCanvasH <= 0)) {
		return; //--------------------------------------------------------------
	}
	const Frame& oFrame = p0Factory->m_oFrame;
	oFrame.draw(refCc, 0, 0, m_oMutaTW.getPixW(), m_oMutaTW.getPixH());
}
void ProgressThWidgetFactory::ProgressTWidget::drawVariable(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	ProgressThWidgetFactory*& p0Factory = m_p1Owner;
	assert(p0Factory != nullptr);
	StdTheme* p0StdTheme = p0Factory->owner();
	assert(p0StdTheme != nullptr);

	const Variable& oVar = m_p0ProgressWidget->variable();
	const int32_t nValue = [&](int32_t nVal)
	{
		if (nVal < m_nMinValue) {
			return m_nMinValue;
		} else if (nVal > m_nMaxValue) {
			return m_nMaxValue;
		}
		return nVal;
	}(oVar.get());

	bool bDanger = false;
	if (m_bThresholdValid) {
		if (m_bDangerBelow) {
			bDanger = (nValue <= m_nThresholdValue);
		} else {
			bDanger = (nValue >= m_nThresholdValue);
		}
	}
	const TileColor& oColor = (bDanger ? p0Factory->m_oDangerColor : p0Factory->m_oNormalColor);

	double fR1, fG1, fB1;
	uint8_t nR, nG, nB;
	p0StdTheme->getColorRgb(oColor, nR, nG, nB);
	fR1 = TileColor::colorUint8ToDouble1(nR);
	fG1 = TileColor::colorUint8ToDouble1(nG);
	fB1 = TileColor::colorUint8ToDouble1(nB);

	int32_t nX = m_nPixCanvasX;
	int32_t nY = m_nPixCanvasY;
	int32_t nW;
	int32_t nH;
	double fPixPerUnit;
	const int32_t nDiff = m_nMaxValue - m_nMinValue + 1;
	const bool bHoriz = (m_nPixCanvasW > m_nPixCanvasH);
	if (bHoriz) {
		fPixPerUnit = (1.0 * m_nPixCanvasW / nDiff);
		nW = fPixPerUnit * (nValue - m_nMinValue);
		nH = m_nPixCanvasH;
	} else {
		fPixPerUnit = (1.0 * m_nPixCanvasH / nDiff);
		nW = m_nPixCanvasW;
		nH = fPixPerUnit * (nValue - m_nMinValue);
		nY += m_nPixCanvasH - nH;
	}
	refCc->save();
	refCc->set_source_rgb(fR1,fG1,fB1);
	refCc->rectangle(nX, nY, nW, nH);
	refCc->fill();

	const int32_t s_nMarkPixSize = 2;
	const IntSet& oMarks = m_p0ProgressWidget->getMarks();
	const int32_t nTotValues = oMarks.size();
	if (nTotValues > 0) {
		refCc->set_source_rgb(0.0,0.0,0.0);
		if (bHoriz) {
			for (int32_t nIdx = 0; nIdx < nTotValues; ++nIdx) {
				const int32_t nMarkValue = oMarks.getValueByIndex(nIdx);
				const int32_t nMarkPixX = nX + fPixPerUnit * (nMarkValue - m_nMinValue);
				const int32_t nMarkPixY = nY;
				const int32_t nMarkPixW = s_nMarkPixSize;
				const int32_t nMarkPixH = nH;
				refCc->rectangle(nMarkPixX, nMarkPixY, nMarkPixW, nMarkPixH);
				refCc->fill();
			}
		} else {
			for (int32_t nIdx = 0; nIdx < nTotValues; ++nIdx) {
				const int32_t nMarkValue = oMarks.getValueByIndex(nIdx);
				const int32_t nMarkPixX = nX;
				const int32_t nMarkPixY = (nY + nH) - fPixPerUnit * (nMarkValue - m_nMinValue);
				const int32_t nMarkPixW = nW;
				const int32_t nMarkPixH = s_nMarkPixSize;
				refCc->rectangle(nMarkPixX, nMarkPixY, nMarkPixW, nMarkPixH);
				refCc->fill();
			}
		}
	}
	refCc->restore();
}

} // namespace stmg
