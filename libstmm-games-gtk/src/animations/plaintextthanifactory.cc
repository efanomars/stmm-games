/*
 * File:   plaintextthanifactory.cc
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

#include "animations/plaintextthanifactory.h"

#include "stdthemecontext.h"
#include "stdtheme.h"

#include <stmm-games/animations/textanimation.h>
#include <stmm-games/levelanimation.h>
#include <stmm-games/tile.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

//#include <pangomm/context.h>
#include <cairomm/context.h>
#include <cairomm/refptr.h>

#include <cassert>
#include <algorithm>
#include <limits>
#include <string>

#include <stdint.h>

namespace Pango { class Context; }

namespace stmg
{

static const double s_fInterlineSpaceToLineFactor = 0.1;

PlainTextThAniFactory::PlainTextThAni::PlainTextThAni() noexcept
{
	reInitCommon();
}
void PlainTextThAniFactory::PlainTextThAni::reInit() noexcept
{
	reInitCommon();
}
void PlainTextThAniFactory::PlainTextThAni::reInitCommon() noexcept
{
	
}
int32_t PlainTextThAniFactory::PlainTextThAni::getZ(int32_t /*nViewTick*/, int32_t /*nTotViewTicks*/) noexcept
{
	return m_nZ;
}
bool PlainTextThAniFactory::PlainTextThAni::isStarted(int32_t nViewTick, int32_t nTotViewTicks) noexcept
{
	assert(m_refModel);
	const bool bIsStarted = m_refModel->isStarted(nViewTick, nTotViewTicks);
//std::cout << "PlainTextThAniFactory::ShowText::isStarted   nViewTick=" << nViewTick << "  nTotViewTicks=" << nTotViewTicks << "  bIsStarted=" << bIsStarted << '\n';
//std::cout << "                          ::isStarted   getElapsed=" << m_refModel->getElapsed(nViewTick, nTotViewTicks) << '\n';
	return bIsStarted;
}
bool PlainTextThAniFactory::PlainTextThAni::isDone(int32_t nViewTick, int32_t nTotViewTicks) noexcept
{
	assert(m_refModel);
	const bool bIsDone = m_refModel->isDone(nViewTick, nTotViewTicks);
	if (bIsDone) {
		m_refModel.reset();
	}
	return bIsDone;
}
void PlainTextThAniFactory::PlainTextThAni::onRemoved() noexcept
{
	m_refModel.reset();
}

void PlainTextThAniFactory::PlainTextThAni::getRectAndScale(FRect& oRect, double& fScale, double& fPixHLine) noexcept
{
	assert(m_refModel);
	const int32_t nTotStrs = static_cast<int32_t>(m_refModel->getText().size());

	const NSize oTileSize = m_refThemeContext->getTileSize();
	const int32_t& nTileW = oTileSize.m_nW;
	const int32_t& nTileH = oTileSize.m_nH;

	const double fFontHeight = m_refModel->getFontHeight();
	const double fPixHeight = ((fFontHeight > 0.0) ? fFontHeight : 1.0) * nTileH;

	const double fScaleTxt = fPixHeight / m_fHighest;
	assert(fScaleTxt > 0.0);

	const double fPixHLineTxt = fPixHeight * (1.0 + s_fInterlineSpaceToLineFactor);
	const double fPixHTxt = fPixHLineTxt * nTotStrs;
	const double fPixWTxt = m_fWidest * fScaleTxt;

	const FSize oSize = m_refModel->getSize();
	const double fMaxPixW = oSize.m_fW * nTileW;
	const double fMaxPixH = oSize.m_fH * nTileH;

	double fSX = fScaleTxt;
	double fSY = fScaleTxt;
	if ((fMaxPixW > 0.0) && (fPixWTxt > 0.0) && (fPixWTxt > fMaxPixW)) {
		fSX = fSX * fMaxPixW / fPixWTxt;
	}
	if ((fMaxPixH > 0.0) && (fPixHTxt > 0.0) && (fPixHTxt > fMaxPixH)) {
		fSY = fSY * fMaxPixH / fPixHTxt;
	}
	fScale = std::min(fSX, fSY);
	fPixHLine = fPixHLineTxt / fScaleTxt * fScale;
	const double fPixW = fPixWTxt / fScaleTxt * fScale;
	const double fPixH = fPixHTxt / fScaleTxt * fScale;

	const FPoint oPos = m_refModel->getPos();
	oRect.m_fX = oPos.m_fX * nTileW + fMaxPixW / 2 - fPixW / 2;
	oRect.m_fY = oPos.m_fY * nTileH + fMaxPixH / 2 - fPixH / 2;
	oRect.m_fW = fPixW;
	oRect.m_fH = fPixH;
}
void PlainTextThAniFactory::PlainTextThAni::draw(int32_t nViewTick, int32_t nTotViewTicks, const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
//std::cout << "PlainTextThAniFactory::ShowText::draw   nViewTick=" << nViewTick << "  nTotViewTicks=" << nTotViewTicks << '\n';
//std::cout << "                          ::draw   m_nFadeIn=" << m_nFadeIn << "  m_nFadeOut=" << m_nFadeOut << '\n';
	assert(nTotViewTicks > 0);
	assert((nViewTick >= 0) && (nViewTick < nTotViewTicks));
	assert(m_refModel);
	if ((m_fWidest <= 0.0) || (m_fHighest <= 0.0)) {
		// nothing to draw 
		return; //--------------------------------------------------------------
	}
	PlainTextThAniFactory* p0Factory = m_p1Owner;
	assert(p0Factory != nullptr);

	const double fElapsed = m_refModel->getElapsed(nViewTick, nTotViewTicks);
	const double fDuration = m_refModel->getDuration();
	const std::vector<std::string>& aLines = m_refModel->getText();

	// Calc Alpha
	double fA1 = p0Factory->m_fA1;
	const double fFadeIn = m_fFadeIn;
	const double fFadeOut = fDuration - m_fFadeOut;
	if ((fFadeIn > 0) && (fElapsed < fFadeIn)) {
		fA1 = fA1 * fElapsed / fFadeIn;
	} else if (fDuration > 0) {
		if ((fFadeOut > 0.0) && (fFadeOut < fDuration) && (fElapsed >= fFadeOut)) {
			fA1 = std::max<double>(0, fA1 * (fDuration - fElapsed) / (fDuration - fFadeOut));
		}
	}

//if (m_fLastAlpha1Drawn != fA1) {
//std::cout << "PlainTextThAniFactory::ShowText::draw   nViewTick=" << nViewTick << "  nTotViewTicks=" << nTotViewTicks << '\n';
//std::cout << "                          ::draw   m_nFadeIn=" << m_nFadeIn << "  m_nFadeOut=" << m_nFadeOut << "  fA1=" << fA1 << '\n';
//}
	FRect oFRect;
	double fScale;
	double fPixHLine;
	getRectAndScale(oFRect, fScale, fPixHLine);

	const bool bCenter = p0Factory->m_bCenter;

	refCc->save();
	refCc->set_source_rgba(p0Factory->m_fR1, p0Factory->m_fG1, p0Factory->m_fB1, fA1);

	refCc->translate(oFRect.m_fX, oFRect.m_fY);
	refCc->scale(fScale, fScale);

//if (nViewTick == 0) {
//std::cout << "-----------------------" << '\n';
//std::cout << "     nTileH = " << nTileH << '\n';
//std::cout << "     m_fWidest = " << m_fWidest << '\n';
//std::cout << "     m_fHighest = " << m_fHighest << '\n';
//std::cout << "     fPixHeight = " << fPixHeight << '\n';
//std::cout << "     fScaleTxt = " << fScaleTxt << '\n';
//std::cout << "     fPixWTxt = " << fPixWTxt << '\n';
//std::cout << "     fPixHTxt = " << fPixHTxt << '\n';
//std::cout << "     fScale = " << fScale << '\n';
//std::cout << "     fMaxPixW = " << fMaxPixW << '\n';
//std::cout << "     fMaxPixH = " << fMaxPixH << '\n';
//std::cout << "     fPixW = " << oFRect.m_fW << '\n';
//std::cout << "     fPixH = " << oFRect.m_fH << '\n';
//std::cout << "     fPixX = " << oFRect.m_fX << '\n';
//std::cout << "     fPixY = " << oFRect.m_fY << '\n';
//std::cout << "     fPixHLine = " << fPixHLine << '\n';
//std::cout << "     fPixHLine / fScale = " << fPixHLine / fScale << '\n';
//}

	double fDisplY = 0.0;
	int32_t nIdx = 0;
	for (const auto& sStr : aLines) {
		if (!sStr.empty()) {
			const NSize& oTextSize = m_aTextSize[nIdx];
			const int32_t nTextW = oTextSize.m_nW;

			m_refFontLayout->set_text(sStr);
			refCc->save();
			if (bCenter) {
				refCc->translate((m_fWidest - nTextW) / 2, fDisplY);
			} else {
				refCc->translate(0, fDisplY);
			}
			m_refFontLayout->show_in_cairo_context(refCc);
			refCc->restore();
		}
		fDisplY += fPixHLine / fScale;
		++nIdx;
	}
	refCc->restore();
}

PlainTextThAniFactory::PlainTextThAniFactory(StdTheme* p1Owner, bool bCenter
					, const TileColor& oColor, const TileAlpha& oAlpha, const TileFont& oFont
					, bool bFadeInIsFactor, double fFadeIn, bool bFadeOutIsFactor, double fFadeOut) noexcept
: StdThemeAnimationFactory(p1Owner)
, m_bCenter(bCenter)
, m_bFadeInIsFactor(bFadeInIsFactor)
, m_fFadeIn(fFadeIn)
, m_bFadeOutIsFactor(bFadeOutIsFactor)
, m_fFadeOut(fFadeOut)
{
	assert(fFadeIn >= 0);
	assert(fFadeOut >= 0);
	uint8_t nR, nG, nB;
	p1Owner->getColorRgb(oColor, nR, nG, nB);
	m_fR1 = TileColor::colorUint8ToDouble1(nR);
	m_fG1 = TileColor::colorUint8ToDouble1(nG);
	m_fB1 = TileColor::colorUint8ToDouble1(nB);
	m_fA1 = oAlpha.getAlpha1();

	const int32_t nFontIdx = oFont.getFontIndex();
	const std::string& sFontDesc = p1Owner->getFontDesc(nFontIdx);
	m_refFont = std::make_unique<Pango::FontDescription>(sFontDesc);
}

bool PlainTextThAniFactory::supports(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refLevelAnimation);
	return (nullptr != dynamic_cast<TextAnimation*>(refLevelAnimation.get()));
}

shared_ptr<ThemeAnimation> PlainTextThAniFactory::create(const shared_ptr<StdThemeContext>& refThemeContext, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refThemeContext);
	assert(refLevelAnimation);

	auto refModel = std::dynamic_pointer_cast<TextAnimation>(refLevelAnimation);
	if (!refModel) {
		return shared_ptr<ThemeAnimation>{}; //---------------------------------
	}
//std::cout << "PlainTextThAniFactory::create   m_bFadeInIsFactor=" << m_bFadeInIsFactor << "  m_fFadeIn=" << m_fFadeIn << '\n';
//std::cout << "                ::create   m_bFadeOutIsFactor=" << m_bFadeOutIsFactor << "  m_fFadeOut=" << m_fFadeOut << '\n';
//std::cout << "                ::create   refModel->getDuration()     = " << refModel->getDuration() << '\n';
//std::cout << "                ::create   refModel->getText().size()  = " << refModel->getText().size() << '\n';
//std::cout << "                ::create   refModel->getText()[0]      = " << refModel->getText()[0] << '\n';

	shared_ptr<PlainTextThAni> refNew;
	m_oPlainTextThAnis.create(refNew);
	auto* p0NewThAni = refNew.get();
	p0NewThAni->m_p1Owner = this;
	p0NewThAni->m_refModel = refModel;
	p0NewThAni->m_nZ = refModel->getZ();

	double fDuration = refModel->getDuration();
	if ((fDuration == LevelAnimation::s_fDurationInfinity) || (fDuration == LevelAnimation::s_fDurationUndefined)) {
		fDuration = std::numeric_limits<int32_t>::max() / 2;
	}
	if (fDuration >= std::numeric_limits<int32_t>::max() / 2) {
		// considered infinity
		p0NewThAni->m_fFadeOut = 0;
		if (m_bFadeInIsFactor) {
			p0NewThAni->m_fFadeIn = 0;
		} else {
			p0NewThAni->m_fFadeIn = m_fFadeIn;
		}
	} else {
		if (m_bFadeInIsFactor) {
			p0NewThAni->m_fFadeIn = fDuration * m_fFadeIn;
		} else {
			p0NewThAni->m_fFadeIn = m_fFadeIn;
		}
		if (m_bFadeOutIsFactor) {
			p0NewThAni->m_fFadeOut = fDuration * m_fFadeOut;
		} else {
			p0NewThAni->m_fFadeOut = m_fFadeOut;
		}
		if (p0NewThAni->m_fFadeIn > fDuration) {
			p0NewThAni->m_fFadeIn = fDuration;
		}
		if (p0NewThAni->m_fFadeOut > fDuration - p0NewThAni->m_fFadeIn) {
			p0NewThAni->m_fFadeOut = fDuration - p0NewThAni->m_fFadeIn;
		}
	}
//std::cout << "                ::create   p0NewThAni->m_fFadeIn =" << p0NewThAni->m_fFadeIn << '\n';
//std::cout << "                ::create   p0NewThAni->m_fFadeOut=" << p0NewThAni->m_fFadeOut << '\n';
	const Glib::RefPtr<Pango::Context>& refFontContext = refThemeContext->getFontContext();
	assert(refFontContext);
	p0NewThAni->m_refFontLayout = Pango::Layout::create(refFontContext);
	p0NewThAni->m_refFontLayout->set_font_description(*m_refFont);

	const std::vector<std::string>& aLines = refModel->getText();

	//const int32_t nTotStrs = aLines.size();
	double fWidest = -1.0;
	double fHighest = -1.0;
	p0NewThAni->m_aTextSize.reserve(aLines.size());
	p0NewThAni->m_aTextSize.clear();
	for (const auto& sStr : aLines) {
//std::cout << "     m_aLines[" << nIdx<< "] = '" << sStr << "'" << '\n';
		if (!sStr.empty()) {
			p0NewThAni->m_refFontLayout->set_text(sStr);
			int32_t nTextW;
			int32_t nTextH;
			//get the text dimensions
			p0NewThAni->m_refFontLayout->get_pixel_size(nTextW, nTextH);
			p0NewThAni->m_aTextSize.push_back(NSize{nTextW, nTextH});
			fWidest = std::max<double>(fWidest, nTextW);
			fHighest = std::max<double>(fHighest, nTextH);
		}
	}
	p0NewThAni->m_fWidest = fWidest;
	p0NewThAni->m_fHighest = fHighest;

	p0NewThAni->m_refThemeContext = refThemeContext;
	return refNew;
}

} // namespace stmg
