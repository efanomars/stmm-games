/*
 * File:   previewthwidgetfactory.cc
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

#include "widgets/previewthwidgetfactory.h"

#include "stdtheme.h"
#include "themecontext.h"
#include "gtkutil/ccsave.h"

#include <stmm-games/block.h>
#include <stmm-games/gamewidget.h>
#include <stmm-games/tile.h>
#include <stmm-games/widgets/previewwidget.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <cairomm/context.h>
#include <cairomm/enums.h>
#include <cairomm/refptr.h>

#include <cassert>
#include <algorithm>
#include <iostream>
#include <vector>

#include <stdint.h>

namespace stmg { class SegmentedFunction; }

namespace stmg
{

const int32_t PreviewThWidgetFactory::PreviewTWidget::s_nPixCanvasMinSizeW = 15;
const int32_t PreviewThWidgetFactory::PreviewTWidget::s_nPixCanvasMinSizeH = 10;
const int32_t PreviewThWidgetFactory::PreviewTWidget::s_nPixMargin = 2;

PreviewThWidgetFactory::PreviewThWidgetFactory(StdTheme* p1Owner
											, const TileColor& oTextColor, const TileAlpha& oTextAlpha, const TileFont& oTextFont
											, const Frame& oFrame, const TileSizing& oTileSizing) noexcept
: StdThemeWidgetFactory(p1Owner)
//, m_oTextColor(oTextColor)
//, m_oTextAlpha(oTextAlpha)
//, m_oTextFont(oTextFont)
, m_oFrame(oFrame)
, m_oTileSizing(oTileSizing)
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
shared_ptr<ThemeWidget> PreviewThWidgetFactory::create(const shared_ptr<GameWidget>& refGameWidget
														, double fTileWHRatio
														, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept
{
	assert(refGameWidget);
	assert(fTileWHRatio > 0);
	assert(refFontContext);
	GameWidget* p0GameWidget = refGameWidget.get();
	assert(p0GameWidget != nullptr);
	PreviewWidget* p0PreviewWidget = dynamic_cast<PreviewWidget*>(p0GameWidget);
	if (p0PreviewWidget == nullptr) {
 		return shared_ptr<ThemeWidget>{};
 	}
	shared_ptr<PreviewTWidget> refWidget;
	m_oPreviewTWidgets.create(refWidget, this, refGameWidget, p0PreviewWidget, refFontContext, fTileWHRatio);
	return refWidget;
}
PreviewThWidgetFactory::PreviewTWidget::PreviewTWidget(PreviewThWidgetFactory* p1Owner
														, const shared_ptr<GameWidget>& refGameWidget
														, PreviewWidget* p0PreviewWidget
														, const Glib::RefPtr<Pango::Context>& refFontContext
														, double fTileWHRatio) noexcept
: ThemeWidget(refGameWidget)
, m_p1Owner(p1Owner)
, m_p0PreviewWidget(p0PreviewWidget)
, m_oMutaTW(this)
, m_oSizedTW(this)
{
	reInitCommon(refFontContext, fTileWHRatio);
}
void PreviewThWidgetFactory::PreviewTWidget::reInit(PreviewThWidgetFactory* p1Owner
													, const shared_ptr<GameWidget>& refGameWidget
													, PreviewWidget* p0PreviewWidget
													, const Glib::RefPtr<Pango::Context>& refFontContext
													, double fTileWHRatio) noexcept
{
	ThemeWidget::reInit(refGameWidget);
	m_p1Owner = p1Owner;
	m_p0PreviewWidget = p0PreviewWidget;
	reInitCommon(refFontContext, fTileWHRatio);
}
void PreviewThWidgetFactory::PreviewTWidget::reInitCommon(const Glib::RefPtr<Pango::Context>& refFontContext, double fTileWHRatio) noexcept
{
	assert(m_p1Owner != nullptr);
	assert(m_p0PreviewWidget != nullptr);

	m_oMutaTW.reInit(fTileWHRatio);
	m_oSizedTW.reInit(m_p0PreviewWidget);

	m_nPixCanvasX = 0;
	m_nPixCanvasY = 0;
	m_nPixCanvasW = 0;
	m_nPixCanvasH = 0;
	// set the initial size in tiles
	m_nTotTilesW = m_p0PreviewWidget->getMinTilesW();
	m_nTotTilesH = m_p0PreviewWidget->getMinTilesH();

	m_refFontContext = refFontContext;
	m_refFontLayout = Pango::Layout::create(refFontContext);
	m_refFontLayout->set_font_description(*(m_p1Owner->m_refFont));

	m_refPreviewTc.reset();
}
void PreviewThWidgetFactory::PreviewTWidget::dump(int32_t
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
		std::cout << sIndent << "PreviewThWidgetFactory::PreviewTWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	ThemeWidget::dump(nIndentSpaces, false);
	if (m_nPixCanvasW > 0) {
		std::cout << sIndent << "  " << "m_nPixCanvas: (" << m_nPixCanvasX << ", " << m_nPixCanvasY << ", " << m_nPixCanvasW << ", " << m_nPixCanvasH << ")" << '\n';
	}
	#endif //NDEBUG
}

NSize PreviewThWidgetFactory::PreviewTWidget::getMinSize(int32_t /*nLayoutConfig*/) const noexcept
{
	PreviewThWidgetFactory* const & p0Factory = m_p1Owner;
	if (p0Factory == nullptr) {
		return NSize{0,0};
	}
	const Frame& oFrame = p0Factory->m_oFrame;
	const int32_t nPixW = oFrame.m_nPixMinLeft + s_nPixCanvasMinSizeW + oFrame.m_nPixMinRight;
	const int32_t nPixH = oFrame.m_nPixMinTop + s_nPixCanvasMinSizeH + oFrame.m_nPixMinBottom;
	return NSize{nPixW, nPixH};
}
void PreviewThWidgetFactory::PreviewTWidget::calcCanvas() noexcept
{
	PreviewThWidgetFactory*& p0Factory = m_p1Owner;
	if (p0Factory == nullptr) {
		return;
	}
	const Frame& oFrame = p0Factory->m_oFrame;
	m_nPixCanvasX = oFrame.m_nPixMinLeft;
	m_nPixCanvasY = oFrame.m_nPixMinTop;
	m_nPixCanvasW = std::max(0, m_oMutaTW.getPixW() - oFrame.m_nPixMinLeft - oFrame.m_nPixMinRight);
	m_nPixCanvasH = std::max(0, m_oMutaTW.getPixH() - oFrame.m_nPixMinTop - oFrame.m_nPixMinBottom);
}
void PreviewThWidgetFactory::PreviewTWidget::sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept
{
	m_oMutaTW.sizeAndConfig(nTileW, nLayoutConfig);
	calcCanvas();
}
void PreviewThWidgetFactory::PreviewTWidget::placeAndMaybeResizeIn(const NRect& oRect) noexcept
{
	m_oMutaTW.placeAndMaybeResizeIn(oRect);
	calcCanvas();
}
const std::pair<SegmentedFunction, SegmentedFunction>& PreviewThWidgetFactory::PreviewTWidget::getSizeFunctions(int32_t nLayoutConfig) const noexcept
{
	return m_oSizedTW.getSizeFunctions(nLayoutConfig);
}
void PreviewThWidgetFactory::PreviewTWidget::drawBase(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	PreviewThWidgetFactory*& p0Factory = m_p1Owner;
	assert(p0Factory != nullptr);
	const Frame& oFrame = p0Factory->m_oFrame;
	refCc->save();
	refCc->set_operator(Cairo::OPERATOR_OVER);
	oFrame.draw(refCc, 0, 0, m_oMutaTW.getPixW(), m_oMutaTW.getPixH());
	refCc->restore();
}
void PreviewThWidgetFactory::PreviewTWidget::drawVariable(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	PreviewThWidgetFactory*& p0Factory = m_p1Owner;
	assert(p0Factory != nullptr);
	StdTheme* p0StdTheme = p0Factory->owner();
	assert(p0StdTheme != nullptr);
	if ((m_nPixCanvasW <= 0) || (m_nPixCanvasH <= 0)) {
		return; //--------------------------------------------------------------
	}
	const std::vector<Block>& aBlock = m_p0PreviewWidget->getBlocks();
	const std::string& sText = m_p0PreviewWidget->getText();
	// Calc the total width and height of the blocks when disposed
	// horizontally with one tile spacing
	// .x.yy.z
	// .x.yy.z
	// xx....z
	// ......z
	int32_t nTilesWidth = 0-1;
	int32_t nTilesHeight = 0;
	const int32_t nTotBlocks = static_cast<int32_t>(aBlock.size());
	constexpr int32_t nShape = 0; // just use first shape, not the best
	for (int32_t nCurBlock = 0; nCurBlock < nTotBlocks; ++nCurBlock) {
		const int32_t nShapeW = aBlock[nCurBlock].shapeWidth(nShape);
		const int32_t nShapeH = aBlock[nCurBlock].shapeHeight(nShape);
		nTilesWidth += 1 + nShapeW;
		nTilesHeight = std::max<int32_t>(nTilesHeight, nShapeH);
	}
	if (!((nTilesWidth > 0) && (nTilesHeight > 0))) {
		return; //--------------------------------------------------------------
	}
	if (nTilesWidth > m_nTotTilesW) {
		m_nTotTilesW = nTilesWidth;
	}
	if (nTilesHeight > m_nTotTilesH) {
		m_nTotTilesH = nTilesHeight;
	}

	refCc->save();
	refCc->set_operator(Cairo::OPERATOR_OVER);
	refCc->rectangle(m_nPixCanvasX, m_nPixCanvasY, m_nPixCanvasW, m_nPixCanvasH);
	refCc->clip();
	if (!sText.empty()) {
		drawText(refCc, m_nPixCanvasX, m_nPixCanvasY, m_nPixCanvasW, m_nPixCanvasH, sText);
	}
	const int32_t nBlocksCanvasW = m_nPixCanvasW - s_nPixMargin * 2;
	const int32_t nBlocksCanvasH = m_nPixCanvasH - s_nPixMargin * 2;
	const double fWHRatio = m_oMutaTW.getTileWHRatio();
	assert(fWHRatio > 0.0);
	int32_t nPreviewTileW = nBlocksCanvasW / m_nTotTilesW;
	int32_t nPreviewTileH = nBlocksCanvasH / m_nTotTilesH;
	if ((1.0 * nPreviewTileW / fWHRatio) < nPreviewTileH) {
		nPreviewTileH = (1.0 * nPreviewTileW / fWHRatio);
	} else {
		nPreviewTileW = 1.0 * nPreviewTileH * fWHRatio;
	}
	if ((nPreviewTileW > 0) && (nPreviewTileH > 0)) {
		const int32_t nDisplX = (nBlocksCanvasW - nTilesWidth * nPreviewTileW) / 2;
		const int32_t nDisplY = (nBlocksCanvasH - nTilesHeight * nPreviewTileH) / 2;
		const int32_t nBaseX = m_nPixCanvasX + s_nPixMargin + nDisplX;
		const int32_t nBaseY = m_nPixCanvasY + s_nPixMargin + nDisplY;
		refCc->translate(nBaseX, nBaseY);
		int32_t nCurX = 0;
		for (int32_t nCurBlock = 0; nCurBlock < nTotBlocks; ++nCurBlock) {
			const Block& oBlock = aBlock[nCurBlock];
			const int32_t nShapeW = oBlock.shapeWidth(nShape);
			drawPreviewBlock(refCc, p0StdTheme, oBlock, nShape, nPreviewTileW, nPreviewTileH, nCurX, 0);
			nCurX += nShapeW + 1;
		}
		refCc->translate(-nBaseX, -nBaseY);
	}
	refCc->restore();
}
void PreviewThWidgetFactory::PreviewTWidget::drawText(const Cairo::RefPtr<Cairo::Context>& refCc
													, int32_t nX, int32_t nY, int32_t nW, int32_t nH
													, const std::string& sText) noexcept
{
//	const TileColor& oColor = m_p1Owner->m_oTextColor;
//	const TileAlpha oAlpha = m_p1Owner->m_oTextAlpha;
	//const TileFont oFont = m_p1Owner->m_oTextFont;
//std::cout << "PreviewThWidgetFactory::PreviewTWidget::drawText(" << (int64_t)this << ")  nX=" << nX << "  nY=" << nY << " nW=" << nW << "  nH=" << nH << '\n';
//	p1Owner->drawText(refCc, m_refFontLayout, oColor, oAlpha, oFont, 0.5, nX, nY, nW, nH, sText);

	//
	m_refFontLayout->set_text(sText);
	//
	int32_t nTextFontPixW;
	int32_t nTextFontPixH;
	m_refFontLayout->get_pixel_size(nTextFontPixW, nTextFontPixH);

	CCSave oCCSave(refCc);
	double fTransX = nX + 0.5 * nW;
	double fTransY = nY + 0.5 * nH;
	refCc->translate(fTransX, fTransY);

	// !! Currently text only drawn if no image !!
	refCc->set_source_rgba(m_p1Owner->m_fR1, m_p1Owner->m_fG1, m_p1Owner->m_fB1, m_p1Owner->m_fA1);

	const double fRatioX = (1.0 * nW) / nTextFontPixW;
	const double fRatioY = (1.0 * nH) / nTextFontPixH;
	const double fRatioM = std::min<double>(fRatioX, fRatioY);
	const double fRatio = fRatioM * 1.0; //TODO
//if ((s.size() > 1) && (s[0] > '9')) {
//std::cout << "StdTheme::drawText()  nPixX=" << nPixX << "  nPixY=" << nPixY << '\n';
//std::cout << "StdTheme::drawText()  fTransX=" << fTransX << "  fTransY=" << fTransY << '\n';
//std::cout << "StdTheme::drawText() s=" << s << "  nTextW=" << nTextW << "  nTextH=" << nTextH << '\n';
//std::cout << "StdTheme::drawText() fRatio=" << fRatio << "  fRatioX=" << fRatioX << "  fRatioY=" << fRatioY << '\n';
//}

	refCc->scale(fRatio, fRatio);

	fTransY = - 0.5 * nTextFontPixH;
	fTransX = - 0.5 * nTextFontPixW;
	refCc->translate(fTransX, fTransY);

	m_refFontLayout->show_in_cairo_context(refCc);
}
void PreviewThWidgetFactory::PreviewTWidget::drawPreviewBlock(const Cairo::RefPtr<Cairo::Context>& refCc, StdTheme* p0StdTheme
															, const Block& oBlock, int32_t nShape
															, int32_t nTileW, int32_t nTileH, int32_t nPosX, int32_t nPosY) noexcept
{
//std::cout << "PreviewThWidgetFactory::PreviewTWidget::drawPreviewBlock" << '\n';
	assert(nShape >= 0);

	if (!m_refPreviewTc) {
		m_refPreviewTc = p0StdTheme->createContext(NSize{nTileW, nTileH}, false, 1.0, 1.0, 1.0, m_refFontContext);
	} else {
		const NSize oSize = m_refPreviewTc->getTileSize();
		const int32_t& nOldTileW = oSize.m_nW;
		const int32_t& nOldTileH = oSize.m_nH;
		if ((nOldTileW != nTileW) || (nOldTileH != nTileH)) {
			m_refPreviewTc->setTileSize(NSize{nTileW, nTileH});
		}
	}
	const int32_t nMinX = oBlock.shapeMinX(nShape);
	const int32_t nMinY = oBlock.shapeMinY(nShape);
	auto aBrickId = oBlock.brickIds();
	for (auto& nBrickId : aBrickId) {
		if (oBlock.shapeBrickVisible(nShape, nBrickId)) {
			const int32_t nBrickPosX = oBlock.shapeBrickPosX(nShape, nBrickId);
			const int32_t nBrickPosY = oBlock.shapeBrickPosY(nShape, nBrickId);
			const int32_t nPixX = (nPosX - nMinX + nBrickPosX) * nTileW;
			const int32_t nPixY = (nPosY - nMinY + nBrickPosY) * nTileH;
//std::cout << "PreviewThWidgetFactory::PreviewTWidget::drawPreviewBlock nPixX=" << nPixX << " nPixY=" << nPixY << '\n';
			constexpr int32_t nSkin = 0; // TODO Maybe it should be -1?
			refCc->translate(nPixX, nPixY);
			m_refPreviewTc->drawBlockTile(refCc, oBlock.brick(nBrickId), nSkin);
			refCc->translate(-nPixX, -nPixY);
		}
	}
}

} // namespace stmg
