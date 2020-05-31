/*
 * File:   varthwidgetfactory.cc
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

#include "widgets/varthwidgetfactory.h"

#include "stdtheme.h"
#include "gtkutil/ccsave.h"
#include "gtkutil/image.h"

#include <stmm-games/variable.h>
#include <stmm-games/util/util.h>
#include <stmm-games/gamewidget.h>
#include <stmm-games/tile.h>
#include <stmm-games/widgets/varwidget.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <cairomm/context.h>
#include <cairomm/refptr.h>

#include <cassert>
#include <algorithm>
#include <iostream>
#include <vector>

#include <stdint.h>

namespace Pango { class Context; }
namespace stmg { class SegmentedFunction; }

namespace stmg
{

VarThWidgetFactory::VarThWidgetFactory(StdTheme* p1Owner, bool bTitlePreValue
									, const TileColor& oTitleColor, const TileAlpha& oTitleAlpha, const TileFont& oTitleFont
									, const shared_ptr<Image>& refTitleBgImg
									, const TileColor& oValueColor, const TileAlpha& oValueAlpha, const TileFont& oValueFont
									, const shared_ptr<Image>& refValueBgImg
									, const Frame& oFrame) noexcept
: StdThemeWidgetFactory(p1Owner)
, m_bTitlePreValue(bTitlePreValue)
, m_refTitleBgImg(refTitleBgImg)
, m_refValueBgImg(refValueBgImg)
, m_oFrame(oFrame)
{
	assert(p1Owner != nullptr);
	{
	uint8_t nR, nG, nB;
	p1Owner->getColorRgb(oTitleColor, nR, nG, nB);
	m_fTitleR1 = TileColor::colorUint8ToDouble1(nR);
	m_fTitleG1 = TileColor::colorUint8ToDouble1(nG);
	m_fTitleB1 = TileColor::colorUint8ToDouble1(nB);
	m_fTitleA1 = oTitleAlpha.getAlpha1();

	const int32_t nFontIdx = oTitleFont.getFontIndex();
	const std::string& sFontDesc = p1Owner->getFontDesc(nFontIdx);
	m_refTitleFont = std::make_unique<Pango::FontDescription>(sFontDesc);
	}
	{
	uint8_t nR, nG, nB;
	p1Owner->getColorRgb(oValueColor, nR, nG, nB);
	m_fValueR1 = TileColor::colorUint8ToDouble1(nR);
	m_fValueG1 = TileColor::colorUint8ToDouble1(nG);
	m_fValueB1 = TileColor::colorUint8ToDouble1(nB);
	m_fValueA1 = oValueAlpha.getAlpha1();

	const int32_t nFontIdx = oValueFont.getFontIndex();
	const std::string& sFontDesc = p1Owner->getFontDesc(nFontIdx);
	m_refValueFont = std::make_unique<Pango::FontDescription>(sFontDesc);
	}
}
shared_ptr<ThemeWidget> VarThWidgetFactory::create(const shared_ptr<GameWidget>& refGameWidget
												, double fTileWHRatio
												, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept
{
//std::cout << "VarThWidgetFactory::create" << '\n';
	assert(refGameWidget);
	assert(fTileWHRatio > 0);
	assert(refFontContext);
	GameWidget* p0GameWidget = refGameWidget.get();
	assert(p0GameWidget != nullptr);
	VarWidget* p0VarWidget = dynamic_cast<VarWidget*>(p0GameWidget);
	if (p0VarWidget == nullptr) {
 		return shared_ptr<ThemeWidget>{}; //------------------------------------
 	}
	shared_ptr<VarTWidget> refWidget;
	m_oVarTWidgets.create(refWidget, this, refGameWidget, p0VarWidget, refFontContext, fTileWHRatio);
	return refWidget;
}
VarThWidgetFactory::VarTWidget::VarTWidget(VarThWidgetFactory* p1Owner
											, const shared_ptr<GameWidget>& refGameWidget
											, VarWidget* p0VarWidget
											, const Glib::RefPtr<Pango::Context>& refFontContext
											, double fTileWHRatio) noexcept
: ThemeWidget(refGameWidget)
, m_p1Owner(p1Owner)
, m_p0VarWidget(p0VarWidget)
, m_oMutaTW(this)
, m_oSizedTW(this)
{
	reInitCommon(refFontContext, fTileWHRatio);
}
void VarThWidgetFactory::VarTWidget::reInit(VarThWidgetFactory* p1Owner
											, const shared_ptr<GameWidget>& refGameWidget, VarWidget* p0VarWidget
											, const Glib::RefPtr<Pango::Context>& refFontContext
											, double fTileWHRatio) noexcept
{
//std::cout << "  VarThWidgetFactory::VarTWidget::reInit()  p0VarWidget=" << reinterpret_cast<int64_t>(p0VarWidget) << '\n';
	ThemeWidget::reInit(refGameWidget);
	m_p1Owner = p1Owner;
	m_p0VarWidget = p0VarWidget;
	//
	reInitCommon(refFontContext, fTileWHRatio);
}
void VarThWidgetFactory::VarTWidget::reInitCommon(const Glib::RefPtr<Pango::Context>& refFontContext, double fTileWHRatio) noexcept
{
	assert(m_p1Owner != nullptr);
	assert(m_p0VarWidget != nullptr);

	m_nConstTitleFactorPerc = m_p0VarWidget->getTitleFactorPerc();
	assert((m_nConstTitleFactorPerc >= 0) && (m_nConstTitleFactorPerc <= 100));
	m_bConstTitleBesideValue = m_p0VarWidget->getTitleBesideValue();
	m_sConstTitle = m_p0VarWidget->getTitle();

	m_refTitleFontLayout = Pango::Layout::create(refFontContext);
	m_refValueFontLayout = Pango::Layout::create(refFontContext);
	m_refTitleFontLayout->set_font_description(*(m_p1Owner->m_refTitleFont));
	m_refValueFontLayout->set_font_description(*(m_p1Owner->m_refValueFont));

	m_refTitleFontLayout->set_text(m_sConstTitle);
	m_refTitleFontLayout->get_pixel_size(m_nTitleFontPixW, m_nTitleFontPixH);

	const int32_t nValueDigits = m_p0VarWidget->getValueDigits();
	m_fMaxValueWHRatio = getTextMaxValueWHRatio(nValueDigits);

	m_nPixCanvasX = 0;
	m_nPixCanvasY = 0;
	m_nPixCanvasW = 0;
	m_nPixCanvasH = 0;

	//
	const int32_t nMinTitlePixWidth = 5 * Util::strUTF8SizeInCodePoints(m_sConstTitle);
	const int32_t nMinTitlePixHeight = 6;
	const int32_t nMinValuePixWidth = 5 * 7; // 7 == "9999999".size()
	const int32_t nMinValuePixHeight = 6;
	if (m_bConstTitleBesideValue) {
		m_nPixMinCanvasWidth = nMinTitlePixWidth + nMinValuePixWidth;
		m_nPixMinCanvasHeight = std::max(nMinTitlePixHeight, nMinValuePixHeight);
	} else {
		m_nPixMinCanvasWidth = std::max(nMinTitlePixWidth, nMinValuePixWidth);
		m_nPixMinCanvasHeight = nMinTitlePixHeight + nMinValuePixHeight;
	}
	m_oMutaTW.reInit(fTileWHRatio);
	m_oSizedTW.reInit(m_p0VarWidget);
}
static char const * s_sDigits = "000000000000000";

double VarThWidgetFactory::VarTWidget::getTextMaxValueWHRatio(int32_t nDigits) noexcept
{
	if (nDigits > 15) {
		nDigits = 15;
	} else if (nDigits <= 0) {
		nDigits = 1;
	}
	m_refValueFontLayout->set_text(&(s_sDigits[15 - nDigits]));
	int32_t nPixTextW;
	int32_t nPixTextH;
	m_refValueFontLayout->get_pixel_size(nPixTextW, nPixTextH);
	assert(nPixTextH > 0);
	const double fWHRatio = (1.0 * nPixTextW) / nPixTextH;
	return fWHRatio;
}

void VarThWidgetFactory::VarTWidget::dump(int32_t
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
		std::cout << sIndent << "VarThWidgetFactory::VarTWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	ThemeWidget::dump(nIndentSpaces, false);
	if (m_nPixCanvasW > 0) {
		std::cout << sIndent << "  " << "m_nPixCanvas: (" << m_nPixCanvasX << ", " << m_nPixCanvasY << ", " << m_nPixCanvasW << ", " << m_nPixCanvasH << ")" << '\n';
		//std::cout << sIndent << "  " << "m_fTitleAlign: " << m_fTitleAlign << '\n';
	}
	#endif //NDEBUG
}
NSize VarThWidgetFactory::VarTWidget::getMinSize(int32_t /*nLayoutConfig*/) const noexcept
{
	VarThWidgetFactory* const & p0Factory = m_p1Owner;
	if (p0Factory == nullptr) {
		return NSize{0,0};
	}
	const Frame& oFrame = p0Factory->m_oFrame;
	const int32_t nPixW = m_nPixMinCanvasWidth + oFrame.m_nPixMinLeft + oFrame.m_nPixMinRight;
	const int32_t nPixH = m_nPixMinCanvasHeight + oFrame.m_nPixMinTop + oFrame.m_nPixMinBottom;
	return NSize{nPixW, nPixH};
}
void VarThWidgetFactory::VarTWidget::calcCanvas() noexcept
{
	VarThWidgetFactory*& p0Factory = m_p1Owner;
	if (p0Factory == nullptr) {
		return;
	}
	const Frame& oFrame = p0Factory->m_oFrame;
	m_nPixCanvasX = oFrame.m_nPixMinLeft;
	m_nPixCanvasY = oFrame.m_nPixMinTop;
	m_nPixCanvasW = std::max(0, m_oMutaTW.getPixW() - oFrame.m_nPixMinLeft - oFrame.m_nPixMinRight);
	m_nPixCanvasH = std::max(0, m_oMutaTW.getPixH() - oFrame.m_nPixMinTop - oFrame.m_nPixMinBottom);
//std::cout << "VarThWidgetFactory::VarTWidget::calcCanvas()  m_nPixCanvasW=" << m_nPixCanvasW << "  m_nPixCanvasH=" << m_nPixCanvasH << '\n';
	const bool bVarTitlePreValue = p0Factory->m_bTitlePreValue;
	const int32_t nLayoutConfig = m_oMutaTW.getLayoutConfig();
	const WidgetAxisFit::ALIGN eTitleAlign = m_p0VarWidget->getTitleAlign(nLayoutConfig);
	const WidgetAxisFit::ALIGN eValueAlign = m_p0VarWidget->getValueAlign(nLayoutConfig);
	if (m_sConstTitle.empty()) {
		m_nPixTitleX = 0;
		m_nPixTitleY = 0;
		m_nPixTitleW = 0;
		m_nPixTitleH = 0;
		m_nPixValueX = 0;
		m_nPixValueY = 0;
		m_nPixValueW = m_nPixCanvasW;
		m_nPixValueH = m_nPixCanvasH;
		m_fTitleAlign = 0.0;
		m_fValueAlign = ((eValueAlign == WidgetAxisFit::ALIGN_FILL) ? 0.0 : WidgetAxisFit::getAlign(eValueAlign));
	} else {
		if (m_bConstTitleBesideValue) {
			m_nPixTitleW = m_nPixCanvasW * m_nConstTitleFactorPerc / 100;
			m_nPixValueW = m_nPixCanvasW * (100 - m_nConstTitleFactorPerc) / 100;
			if (bVarTitlePreValue) {
				m_nPixTitleX = 0;
				m_nPixValueX = 0 + m_nPixTitleW;
				m_fTitleAlign = ((eTitleAlign == WidgetAxisFit::ALIGN_FILL) ? 0.0 : WidgetAxisFit::getAlign(eTitleAlign));
				m_fValueAlign = ((eValueAlign == WidgetAxisFit::ALIGN_FILL) ? 1.0 : WidgetAxisFit::getAlign(eValueAlign));
			} else {
				m_nPixValueX = 0;
				m_nPixTitleX = 0 + m_nPixValueW;
				m_fTitleAlign = ((eTitleAlign == WidgetAxisFit::ALIGN_FILL) ? 1.0 : WidgetAxisFit::getAlign(eTitleAlign));
				m_fValueAlign = ((eValueAlign == WidgetAxisFit::ALIGN_FILL) ? 0.0 : WidgetAxisFit::getAlign(eValueAlign));
			}
			m_nPixTitleY = 0;
			m_nPixTitleH = m_nPixCanvasH;
			m_nPixValueY = 0;
			m_nPixValueH = m_nPixCanvasH;
		} else {
			m_nPixTitleH = m_nPixCanvasH * m_nConstTitleFactorPerc / 100;
			m_nPixValueH = m_nPixCanvasH * (100 - m_nConstTitleFactorPerc) / 100;
			if (bVarTitlePreValue) {
				m_nPixTitleY = 0;
				m_nPixValueY = 0 + m_nPixTitleH;
			} else {
				m_nPixValueY = 0;
				m_nPixTitleY = 0 + m_nPixValueH;
			}
			m_nPixTitleX = 0;
			m_nPixTitleW = m_nPixCanvasW;
			m_nPixValueX = 0;
			m_nPixValueW = m_nPixCanvasW;
			m_fTitleAlign = ((eTitleAlign == WidgetAxisFit::ALIGN_FILL) ? 0.5 : WidgetAxisFit::getAlign(eTitleAlign));
			m_fValueAlign = ((eValueAlign == WidgetAxisFit::ALIGN_FILL) ? 0.5 : WidgetAxisFit::getAlign(eValueAlign));
		}
	}
//std::cout << "VarThWidgetFactory::VarTWidget::calcCanvas()  " << reinterpret_cast<int64_t>(this) << "  m_fTitleAlign=" << m_fTitleAlign << "  m_fValueAlign=" << m_fValueAlign << '\n';
//std::cout << "                                              layout config:" << m_oMutaTW.getLayoutConfig() << '\n';
	assert(m_fMaxValueWHRatio != 0.0);
	if ((1.0 * m_nPixValueW / m_fMaxValueWHRatio) > (1.0 * m_nPixValueH)) {
		m_nPixValueTextX = m_nPixValueX;
		m_nPixValueTextY = m_nPixValueY;
		m_nPixValueTextW = m_nPixValueW;
		m_nPixValueTextH = m_nPixValueH;
	} else {
		m_nPixValueTextX = m_nPixValueX;
		m_nPixValueTextW = m_nPixValueW;
		m_nPixValueTextH = (1.0 * m_nPixValueW / m_fMaxValueWHRatio);
		m_nPixValueTextY = m_nPixValueY + (m_nPixValueH - m_nPixValueTextH) / 2;
	}

}
void VarThWidgetFactory::VarTWidget::sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept
{
	m_oMutaTW.sizeAndConfig(nTileW, nLayoutConfig);
	calcCanvas();
}
void VarThWidgetFactory::VarTWidget::placeAndMaybeResizeIn(const NRect& oRect) noexcept
{
//std::cout << "VarThWidgetFactory::VarTWidget::placeAndMaybeResizeIn()  nX=" << nX << "  nY=" << nY << "   nW=" << nW << "  nH=" << nH << '\n';
	m_oMutaTW.placeAndMaybeResizeIn(oRect);
	calcCanvas();
//std::cout << "VarThWidgetFactory::VarTWidget::placeAndMaybeResizeIn()  nResX=" << m_oSizedTW.getPixX() << "  nResY=" << m_oSizedTW.getPixY() 
//<< "   nResW=" << m_oSizedTW.getPixW() << "  nResH=" << m_oSizedTW.getPixH() << '\n';
}
const std::pair<SegmentedFunction, SegmentedFunction>& VarThWidgetFactory::VarTWidget::getSizeFunctions(int32_t nLayoutConfig) const noexcept
{
//std::cout << "VarThWidgetFactory::VarTWidget::getSizeFunctions this=" << reinterpret_cast<int64_t>(this) << '\n';
	return m_oSizedTW.getSizeFunctions(nLayoutConfig);
}
void VarThWidgetFactory::VarTWidget::drawBase(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
//std::cout << "VarThWidgetFactory::VarTWidget::drawBase w=" << m_oMutaTW.getPixW() << "  h=" << m_oMutaTW.getPixH() << '\n';
	VarThWidgetFactory*& p0Factory = m_p1Owner;
	assert(p0Factory != nullptr);
	if ((m_nPixCanvasW <= 0) || (m_nPixCanvasH <= 0)) {
//std::cout << "VarThWidgetFactory::VarTWidget::drawBase no canvas" << '\n';
		return; //--------------------------------------------------------------
	}
	const Frame& oFrame = p0Factory->m_oFrame;
	oFrame.draw(refCc, 0, 0, m_oMutaTW.getPixW(), m_oMutaTW.getPixH());

	const shared_ptr<Image>& refTitleImg = p0Factory->m_refTitleBgImg;
	if (refTitleImg) {
		refTitleImg->draw(refCc, m_nPixTitleX, m_nPixTitleY, m_nPixTitleW, m_nPixTitleH);
	}
	const shared_ptr<Image>& refValueImg = p0Factory->m_refValueBgImg;
	if (refValueImg) {
		refValueImg->draw(refCc, m_nPixValueX, m_nPixValueY, m_nPixValueW, m_nPixValueH);
	}

	if (m_sConstTitle.empty()) {
		return; //--------------------------------------------------------------
	}

	CCSave oCCSave(refCc);
	double fTransX = m_nPixTitleX + m_fTitleAlign * m_nPixTitleW;
	double fTransY = m_nPixTitleY + 0.5 * m_nPixTitleH;
	refCc->translate(fTransX, fTransY);

	refCc->set_source_rgba(m_p1Owner->m_fTitleR1, m_p1Owner->m_fTitleG1, m_p1Owner->m_fTitleB1, m_p1Owner->m_fTitleA1);

	const double fRatioX = (1.0 * m_nPixTitleW) / m_nTitleFontPixW;
	const double fRatioY = (1.0 * m_nPixTitleH) / m_nTitleFontPixH;
	const double fRatioM = std::min<double>(fRatioX, fRatioY);
	const double fRatio = fRatioM * 1.0; //TODO

	refCc->scale(fRatio, fRatio);

	fTransX = - m_fTitleAlign * m_nTitleFontPixW;
	fTransY = - 0.5 * m_nTitleFontPixH;
	refCc->translate(fTransX, fTransY);

//std::cout << "VarThWidgetFactory::VarTWidget::drawBase m_nPixTitleW=" << m_nPixTitleW << "  m_nPixTitleH=" << m_nPixTitleH << '\n';

	m_refTitleFontLayout->show_in_cairo_context(refCc);
}
void VarThWidgetFactory::VarTWidget::drawVariable(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	const Variable& oVar = m_p0VarWidget->variable();
	const std::string sValue = oVar.toFormattedString();

	if (sValue.empty()) {
		return; //--------------------------------------------------------------
	}

	m_refValueFontLayout->set_text(sValue);
	//
	int32_t nValueFontPixW;
	int32_t nValueFontPixH;
	m_refValueFontLayout->get_pixel_size(nValueFontPixW, nValueFontPixH);

	CCSave oCCSave(refCc);
	double fTransX = m_nPixValueTextX + m_fValueAlign * m_nPixValueTextW;
	double fTransY = m_nPixValueTextY + 0.5 * m_nPixValueTextH;
	refCc->translate(fTransX, fTransY);

	refCc->set_source_rgba(m_p1Owner->m_fValueR1, m_p1Owner->m_fValueG1, m_p1Owner->m_fValueB1, m_p1Owner->m_fValueA1);

	//TODO should be precalculated in calcCanvas
	const double fRatioX = (1.0 * m_nPixValueTextW) / nValueFontPixW;
	const double fRatioY = (1.0 * m_nPixValueTextH) / nValueFontPixH;
	const double fRatioM = std::min<double>(fRatioX, fRatioY);
	const double fRatio = fRatioM * 1.0; //TODO

	refCc->scale(fRatio, fRatio);

	fTransX = - m_fValueAlign * nValueFontPixW;
	fTransY = - 0.5 * nValueFontPixH;
	refCc->translate(fTransX, fTransY);

	m_refValueFontLayout->show_in_cairo_context(refCc);
}

} // namespace stmg
