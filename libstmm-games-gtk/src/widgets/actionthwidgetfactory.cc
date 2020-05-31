/*
 * File:   actionthwidgetfactory.cc
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

#include "widgets/actionthwidgetfactory.h"

#include "stdtheme.h"
#include "gtkutil/ccsave.h"
#include "gtkutil/image.h"

#include <stmm-games/gamewidget.h>
#include <stmm-games/tile.h>
#include <stmm-games/widgets/actionwidget.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <cairomm/context.h>
#include <cairomm/refptr.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include <stdint.h>

namespace stmg { class SegmentedFunction; }
namespace stmi { class Event; }

namespace Pango { class Context; }

namespace stmg
{

static constexpr const int32_t s_nPixCanvasMinSizeW = 5;
static constexpr const int32_t s_nPixCanvasMinSizeH = 5;
static constexpr const int32_t s_nMarginPix = 2;
static constexpr const double s_fMarginFactor = 0.1;

ActionThWidgetFactory::ActionThWidgetFactory(StdTheme* p1Owner
											, const TileColor& oTextColor, const TileAlpha& oTextAlpha, const TileFont& oTextFont
											, const Frame& oFrame) noexcept
: StdThemeWidgetFactory(p1Owner)
, m_oFrame(oFrame)
{
	assert(p1Owner != nullptr);
	uint8_t nR, nG, nB;
	p1Owner->getColorRgb(oTextColor, nR, nG, nB);
	m_fR1 = TileColor::colorUint8ToDouble1(nR);
	m_fG1 = TileColor::colorUint8ToDouble1(nG);
	m_fB1 = TileColor::colorUint8ToDouble1(nB);
	m_fA1 = oTextAlpha.getAlpha1();

	const int32_t nFontIdx = oTextFont.getFontIndex();
	const std::string& sFontDesc = p1Owner->getFontDesc(nFontIdx);
	m_refFont = std::make_unique<Pango::FontDescription>(sFontDesc);
}
shared_ptr<ThemeWidget> ActionThWidgetFactory::create(const shared_ptr<GameWidget>& refGameWidget
													, double fTileWHRatio, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept
{
	assert(refGameWidget);
	assert(fTileWHRatio > 0);
	assert(refFontContext);
	GameWidget* p0GameWidget = refGameWidget.get();
	assert(p0GameWidget != nullptr);
	ActionWidget* p0ActionWidget = dynamic_cast<ActionWidget*>(p0GameWidget);
	if (p0ActionWidget == nullptr) {
 		return shared_ptr<ThemeWidget>{}; //------------------------------------
 	}
	shared_ptr<KeyActionTWidget> refWidget;
	m_oKeyActionTWidgets.create(refWidget, this, refGameWidget, p0ActionWidget, refFontContext, fTileWHRatio);
	return refWidget;
}

ActionThWidgetFactory::KeyActionTWidget::KeyActionTWidget(ActionThWidgetFactory* p1Owner
														, const shared_ptr<GameWidget>& refGameWidget, ActionWidget* p0ActionWidget
														, const Glib::RefPtr<Pango::Context>& refFontContext, double fTileWHRatio) noexcept
: ThemeWidget(refGameWidget)
, ThemeWidgetInteractive(this, true)
, m_p1Owner(p1Owner)
, m_p0ActionWidget(p0ActionWidget)
, m_oImmuTW(this)
, m_oSizedTW(this)
{
	reInitCommon(refFontContext, fTileWHRatio);
}
void ActionThWidgetFactory::KeyActionTWidget::reInit(ActionThWidgetFactory* p1Owner
							, const shared_ptr<GameWidget>& refGameWidget, ActionWidget* p0ActionWidget
							, const Glib::RefPtr<Pango::Context>& refFontContext, double fTileWHRatio) noexcept
{
	ThemeWidget::reInit(refGameWidget);
	ThemeWidgetInteractive::reInit(this, true);

	assert(p1Owner != nullptr);
	m_p1Owner = p1Owner;
	m_p0ActionWidget = p0ActionWidget;
	reInitCommon(refFontContext, fTileWHRatio);
}
void ActionThWidgetFactory::KeyActionTWidget::reInitCommon(const Glib::RefPtr<Pango::Context>& refFontContext, double fTileWHRatio) noexcept
{
	assert(m_p1Owner != nullptr);
	assert(m_p0ActionWidget != nullptr);

	m_nPixCanvasX = 0;
	m_nPixCanvasY = 0;
	m_nPixCanvasW = 0;
	m_nPixCanvasH = 0;

	m_oImmuTW.reInit(fTileWHRatio);
	m_oSizedTW.reInit(m_p0ActionWidget);
	m_sConstText = m_p0ActionWidget->getText();
	const auto& nImgId = m_p0ActionWidget->getImageId();
	if (nImgId < 0) {
		m_refConstImg.reset();
	} else {
		m_refConstImg = m_p1Owner->owner()->getImageById(nImgId);
	}
	if (! m_sConstText.empty()) {
		m_refFontLayout = Pango::Layout::create(refFontContext);
		m_refFontLayout->set_font_description(*(m_p1Owner->m_refFont));
		//
		m_refFontLayout->set_text(m_sConstText);
		//
		m_refFontLayout->get_pixel_size(m_nTextFontPixW, m_nTextFontPixH);
	}

}
void ActionThWidgetFactory::KeyActionTWidget::dump(int32_t
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
		std::cout << sIndent << "ActionThWidgetFactory::KeyActionTWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	ThemeWidget::dump(nIndentSpaces, false);
	if (m_nPixCanvasW > 0) {
		std::cout << sIndent << "  " << "m_nPixCanvas: (" << m_nPixCanvasX << ", " << m_nPixCanvasY << ", " << m_nPixCanvasW << ", " << m_nPixCanvasH << ")" << '\n';
	}
	#endif //NDEBUG
}
NSize ActionThWidgetFactory::KeyActionTWidget::getMinSize(int32_t /*nLayoutConfig*/) const noexcept
{
	ActionThWidgetFactory* const & p0Factory = m_p1Owner;
	if (p0Factory == nullptr) {
		return NSize{0,0};
	}
	const Frame& oFrame = p0Factory->m_oFrame;
	const int32_t nPixW = oFrame.m_nPixMinLeft + s_nPixCanvasMinSizeW + oFrame.m_nPixMinRight;
	const int32_t nPixH = oFrame.m_nPixMinTop + s_nPixCanvasMinSizeH + oFrame.m_nPixMinBottom;
	return NSize{nPixW, nPixH};
}
void ActionThWidgetFactory::KeyActionTWidget::calcCanvas() noexcept
{
	ActionThWidgetFactory*& p0Factory = m_p1Owner;
	assert(p0Factory != nullptr);
	const Frame& oFrame = p0Factory->m_oFrame;
	m_nPixCanvasX = oFrame.m_nPixMinLeft;
	m_nPixCanvasY = oFrame.m_nPixMinTop;
	m_nPixCanvasW = std::max(0, m_oImmuTW.getPixW() - oFrame.m_nPixMinLeft - oFrame.m_nPixMinRight);
	m_nPixCanvasH = std::max(0, m_oImmuTW.getPixH() - oFrame.m_nPixMinTop - oFrame.m_nPixMinBottom);
}
void ActionThWidgetFactory::KeyActionTWidget::sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept
{
	m_oImmuTW.sizeAndConfig(nTileW, nLayoutConfig);
	calcCanvas();
}
void ActionThWidgetFactory::KeyActionTWidget::placeAndMaybeResizeIn(const NRect& oRect) noexcept
{
	m_oImmuTW.placeAndMaybeResizeIn(oRect);
	calcCanvas();
}
const std::pair<SegmentedFunction, SegmentedFunction>& ActionThWidgetFactory::KeyActionTWidget::getSizeFunctions(int32_t nLayoutConfig) const noexcept
{
	return m_oSizedTW.getSizeFunctions(nLayoutConfig);
}
double ActionThWidgetFactory::KeyActionTWidget::getTextScale(int32_t nInCanvasW, int32_t nInCanvasH) noexcept
{
//std::cout << "KeyActionTWidget::getTextScale  nInCanvasW=" << nInCanvasW << "  nInCanvasH=" << nInCanvasH << '\n';
//std::cout << "KeyActionTWidget::getTextScale  m_nTextFontPixW=" << m_nTextFontPixW << "  m_nTextFontPixH=" << m_nTextFontPixH << '\n';
	const int32_t& nTileW = m_oImmuTW.getTileW();
	const int32_t& nTileH = (1.0 * nTileW) / m_oImmuTW.getTileWHRatio();

	const double fMaxPixW = nInCanvasW;
	const double fMaxPixH = nInCanvasH;

	const double fFontHeight = m_p0ActionWidget->getTextSize();
	const double fPixHeight = ((fFontHeight > 0.0) ? (fFontHeight * nTileH) : fMaxPixH);
	assert(fPixHeight >= 1.0);

	const double fScaleTxt = fPixHeight / m_nTextFontPixH; // The requested text height in pixels to reference font height ratio
	assert(fScaleTxt > 0.0);

	const double fPixHTxt = fPixHeight;
	const double fPixWTxt = m_nTextFontPixW * fScaleTxt; // The requested text width in pixels
//std::cout << "KeyActionTWidget::getTextScale  fPixWTxt=" << fPixWTxt << "  fPixHTxt=" << fPixHTxt << '\n';
	double fSX = fScaleTxt;
	double fSY = fScaleTxt;
	if ((fMaxPixW > 0.0) && (fPixWTxt > 0.0) && (fPixWTxt > fMaxPixW)) {
		// The text doesn't fit, need to shrink it
		fSX = fSX * fMaxPixW / fPixWTxt;
	}
	if ((fMaxPixH > 0.0) && (fPixHTxt > 0.0) && (fPixHTxt > fMaxPixH)) {
		// The text doesn't fit, need to shrink it
		fSY = fSY * fMaxPixH / fPixHTxt;
	}
	// The text size to reference (font) size ratio that fits
	const double fScale = std::min(fSX, fSY);
//std::cout << "KeyActionTWidget::getTextScale  fSX=" << fSX << "  fSY=" << fSY << '\n';
	return fScale;
}

void ActionThWidgetFactory::KeyActionTWidget::draw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	const int32_t nMarginW  = std::max(static_cast<int32_t>(s_fMarginFactor * m_nPixCanvasW), s_nMarginPix);
	const int32_t nMarginH  = std::max(static_cast<int32_t>(s_fMarginFactor * m_nPixCanvasH), s_nMarginPix);
	const int32_t nInCanvasW = m_nPixCanvasW - nMarginW * 2;
	const int32_t nInCanvasH = m_nPixCanvasH - nMarginH * 2;
	//
	if ((nInCanvasW <= 0) || (nInCanvasH <= 0)) {
		return; //--------------------------------------------------------------
	}
	ActionThWidgetFactory*& p0Factory = m_p1Owner;
	assert(p0Factory != nullptr);
	const Frame& oFrame = p0Factory->m_oFrame;

	CCSave oCCSave(refCc);
	refCc->translate(m_oImmuTW.getPixX(), m_oImmuTW.getPixY());
	oFrame.draw(refCc, 0, 0, m_oImmuTW.getPixW(), m_oImmuTW.getPixH());
	//
	const int32_t nInCanvasX = m_nPixCanvasX + nMarginW;
	const int32_t nInCanvasY = m_nPixCanvasY + nMarginH;
	if (m_refConstImg) {
		int32_t nResW, nResH;
		const NSize oNatSize = m_refConstImg->getNaturalSize();
		if (oNatSize.m_nH > 0) {
			// Let's keep the natural wh-ratio of the image (since it's probably an icon)
			const double fWHRatio = (1.0 * oNatSize.m_nW) / (1.0 * oNatSize.m_nH);
			if (((1.0 * nInCanvasW) / (1.0 * nInCanvasH)) > fWHRatio) {
				nResH = nInCanvasH;
				nResW = (1.0 * nResH) * fWHRatio;
			} else {
				nResW = nInCanvasW;
				nResH = (1.0 * nResW) / fWHRatio;
			}
		} else {
			nResW = nInCanvasW;
			nResH = nInCanvasH;
		}
		const int32_t nResX = nInCanvasX + (nInCanvasW - nResW) / 2;
		const int32_t nResY = nInCanvasY + (nInCanvasH - nResH) / 2;
		m_refConstImg->draw(refCc, nResX, nResY, nResW, nResH);
	} else if (!m_sConstText.empty()) {

		// !! Currently text only drawn if no image !!
		refCc->set_source_rgba(p0Factory->m_fR1, p0Factory->m_fG1, p0Factory->m_fB1, p0Factory->m_fA1);

//std::cout << "KeyActionTWidget::draw  nInCanvasX=" << nInCanvasX << "  nInCanvasY=" << nInCanvasY << '\n';
		const double fScale = getTextScale(nInCanvasW, nInCanvasH);
//std::cout << "KeyActionTWidget::getRectAndScale  oRect.m_fX=" << oRect.m_fX << "  oRect.m_fY=" << oRect.m_fY << '\n';
		//
		refCc->translate(nInCanvasX + nInCanvasW / 2, nInCanvasY + nInCanvasH / 2);
		refCc->scale(fScale, fScale);
		refCc->translate(- 1.0 * m_nTextFontPixW / 2, - 1.0 * m_nTextFontPixH / 2);
		m_refFontLayout->show_in_cairo_context(refCc);

//#ifndef NDEBUG
//refCc->restore();
//refCc->set_source_rgb(1.0, 0.0, 0.0);
//refCc->set_line_width(1.5);
//refCc->rectangle(nInCanvasX, nInCanvasY, nInCanvasW, nInCanvasH);
//refCc->stroke();
//#endif //NDEBUG
	}
}
bool ActionThWidgetFactory::KeyActionTWidget::handleXYInput(const shared_ptr<stmi::Event>& refXYEvent
															, int32_t nTeam, int32_t nMate) noexcept
{
	m_p0ActionWidget->handleXYInput(refXYEvent, nTeam, nMate);
	return true;
}

} // namespace stmg
