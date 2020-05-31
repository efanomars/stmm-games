/*
 * File:   imagesequencethanifactory.cc
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

#include "animations/imagesequencethanifactory.h"

#include "stdthemecontext.h"

#include "gtkutil/dynanimation.h"
#include "gtkutil/image.h"

#include <stmm-games/animations/imagesequenceanimation.h>
#include <stmm-games/levelanimation.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <cassert>
//#include <iostream>
#include <algorithm>
#include <limits>
#include <vector>

namespace stmg { class StdTheme; }

namespace stmg
{

////////////////////////////////////////////////////////////////////////
int32_t ImageSequenceThAniFactory::ImageSeqThAni::calcBestPic(int32_t nViewTick, int32_t nTotViewTicks) const noexcept
{
//std::cout << "ImageSequenceThAniFactory::ImageSeqThAni(" << reinterpret_cast<int64_t>(this) << ")::calcBestPic(nVT=" << nViewTick << ",nTVT=" << nTotViewTicks << ")" << '\n';
	const double fElapsed = m_refModel->getElapsed(nViewTick, nTotViewTicks);
//std::cout << "          ::calcBestPic fElapsed=" << fElapsed << " m_fInverseDuration=" << m_fInverseDuration << '\n';

	const double fGameInterval = m_refModel->gameInterval();
	assert(fGameInterval > 0.0);
	const double fViewInterval = fGameInterval / nTotViewTicks;
	assert(fViewInterval > 0.0);
	const double fStartMillisec = fElapsed; // + nViewTick * fViewInterval;
	const double fStart01 = m_fInverseDuration * fStartMillisec;
	const double fStop01 = m_fInverseDuration * (fStartMillisec + fViewInterval);
//std::cout << "          ::calcBestPic fGameInterval=" << fGameInterval << " fViewInterval=" << fViewInterval << '\n';
//std::cout << "          ::calcBestPic fStartMillisec=" << fStartMillisec << " fStart01=" << fStart01 << " fStop01=" << fStop01 << '\n';

	int32_t nStopPic;
	if (fStop01 >= 1.0) {
		nStopPic = m_refAnimation->getTotImages() - 1;
	} else {
		nStopPic = m_refAnimation->getImageIdx(fStop01);
	}

	int32_t nBestPic = m_nLastPicDrawn;
	int32_t nBestPriority = std::numeric_limits<int32_t>::lowest();

	int32_t nCurPic = m_refAnimation->getImageIdx(fStart01);
	while (nCurPic <= nStopPic) {
		const int32_t nPriority = m_refAnimation->getImageByIdx(nCurPic).m_nPriority;
		if (nPriority >= nBestPriority) {
			nBestPriority = nPriority;
			nBestPic = nCurPic;
		}
		++nCurPic;
	}
	return nBestPic;
}
FRect ImageSequenceThAniFactory::ImageSeqThAni::calcPicSize(int32_t nBestPic) const noexcept
{
	const DynAnimation::DynImage& oDynImage = m_refAnimation->getImageByIdx(nBestPic);
	const bool bWidthDefined = (oDynImage.m_oRelRect.m_fW > 0.0);
	const bool bHeightDefined = (oDynImage.m_oRelRect.m_fH > 0.0);
	if (bWidthDefined && bHeightDefined) {
		return oDynImage.m_oRelRect; //-----------------------------------------
	}
	FRect oRelRect = oDynImage.m_oRelRect;
	const shared_ptr<Image>& refImg = oDynImage.m_refImage;
	const NSize oNatSize = refImg->getNaturalSize();
	if (bWidthDefined) {
		oRelRect.m_fH = (oRelRect.m_fW * oNatSize.m_nH) / oNatSize.m_nW;
	} else if (bHeightDefined) {
		oRelRect.m_fW = (oRelRect.m_fH * oNatSize.m_nW) / oNatSize.m_nH;
	} else if (oNatSize.m_nW > oNatSize.m_nH) {
		oRelRect.m_fW = 1.0;
		oRelRect.m_fH = 1.0 * oNatSize.m_nH / oNatSize.m_nW;
		oRelRect.m_fY += (1.0 - oRelRect.m_fH) / 2;
	} else {
		oRelRect.m_fH = 1.0;
		oRelRect.m_fW = 1.0 * oNatSize.m_nW / oNatSize.m_nH;
		oRelRect.m_fX += (1.0 - oRelRect.m_fW) / 2;
	}
	return oRelRect;
}
void ImageSequenceThAniFactory::ImageSeqThAni::getRectAndBestPic(int32_t nViewTick, int32_t nTotViewTicks
																, NRect& oPixRect, int32_t& nBestPic) noexcept
{
	nBestPic = calcBestPic(nViewTick, nTotViewTicks);
	if (nBestPic != m_nCachePic) {
		m_nCachePic = nBestPic;
		m_oCacheRelRect = calcPicSize(nBestPic);
	}
	const NSize oTileSize = m_refThemeContext->getTileSize();
	const int32_t& nTileW = oTileSize.m_nW;
	const int32_t& nTileH = oTileSize.m_nH;

	const FSize oSize = m_refModel->getSize();
	const double fRefW = oSize.m_fW;
	const double fRefH = oSize.m_fH;
	const FPoint oPos = m_refModel->getPos();
	const double fCenterPosX = oPos.m_fX + 0.5 * fRefW;
	const double fCenterPosY = oPos.m_fY + 0.5 * fRefH;
	const int32_t nPixRefW = fRefW * nTileW;
	const int32_t nPixRefH = fRefH * nTileH;
	oPixRect.m_nW = m_oCacheRelRect.m_fW * nPixRefW;
	oPixRect.m_nH = m_oCacheRelRect.m_fH * nPixRefH;
	oPixRect.m_nX = fCenterPosX * nTileW - 0.5 * m_oCacheRelRect.m_fW * nPixRefW + m_oCacheRelRect.m_fX * nPixRefW;
	oPixRect.m_nY = fCenterPosY * nTileH - 0.5 * m_oCacheRelRect.m_fH * nPixRefH + m_oCacheRelRect.m_fY * nPixRefH;
}
void ImageSequenceThAniFactory::ImageSeqThAni::draw(int32_t nViewTick, int32_t nTotViewTicks
													, const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
//std::cout << "ImageSequenceThAniFactory::ImageSeqThAni::draw" << '\n';
	assert(nTotViewTicks > 0);
	assert((nViewTick >= 0) && (nViewTick < nTotViewTicks));
	assert(m_refModel);
	ImageSequenceThAniFactory* p0Factory = m_p1Owner;
	if (p0Factory == nullptr) {
		return; //--------------------------------------------------------------
	}
	StdTheme* p0Theme = p0Factory->owner();
	if (p0Theme == nullptr) {
		return; //--------------------------------------------------------------
	}

	NRect oRect;
	int32_t nBestPic;
	getRectAndBestPic(nViewTick, nTotViewTicks, oRect, nBestPic);

	const shared_ptr<Image>& refImg = m_refAnimation->getImageByIdx(nBestPic).m_refImage;

	refCc->save();

	refImg->draw(refCc, oRect.m_nX, oRect.m_nY, oRect.m_nW, oRect.m_nH);
	refCc->restore();

	m_nLastPicDrawn = nBestPic;
}
int32_t ImageSequenceThAniFactory::ImageSeqThAni::getZ(int32_t /*nViewTick*/, int32_t /*nTotViewTicks*/) noexcept
{
	return m_nZ;
}
bool ImageSequenceThAniFactory::ImageSeqThAni::isStarted(int32_t nViewTick, int32_t nTotViewTicks) noexcept
{
	assert(m_refModel);
	return m_refModel->isStarted(nViewTick, nTotViewTicks);
}
bool ImageSequenceThAniFactory::ImageSeqThAni::isDone(int32_t nViewTick, int32_t nTotViewTicks) noexcept
{
	assert(m_refModel);
	const bool bIsDone = (m_refModel->getElapsed(nViewTick, nTotViewTicks) >= 1.0 / m_fInverseDuration);
	if (bIsDone) {
		m_refModel.reset();
	}
	return bIsDone;
}
void ImageSequenceThAniFactory::ImageSeqThAni::onRemoved() noexcept
{
	m_refModel.reset();
}

ImageSequenceThAniFactory::ImageSequenceThAniFactory(StdTheme* p1Owner, const shared_ptr<DynAnimation>& refDyn) noexcept
: StdThemeAnimationFactory(p1Owner)
, m_refDynAnimation(refDyn)
{
	assert(refDyn);
}
bool ImageSequenceThAniFactory::supports(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refLevelAnimation);
	return (nullptr != dynamic_cast<ImageSequenceAnimation*>(refLevelAnimation.get()));
}
shared_ptr<ThemeAnimation> ImageSequenceThAniFactory::create(const shared_ptr<StdThemeContext>& refThemeContext
															, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refThemeContext);
	assert(refLevelAnimation);

	auto refModel = std::dynamic_pointer_cast<ImageSequenceAnimation>(refLevelAnimation);
	if (!refModel) {
		return shared_ptr<ThemeAnimation>{};
	}
	return createAny(refThemeContext, refModel);
}
shared_ptr<ThemeAnimation> ImageSequenceThAniFactory::createAny(const shared_ptr<StdThemeContext>& refThemeContext
																, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refThemeContext);
	assert(refLevelAnimation);

	shared_ptr<ImageSeqThAni> refNew;
	m_oImageSeqThAnis.create(refNew);
	refNew->m_p1Owner = this;
	refNew->m_refModel = refLevelAnimation;

	refNew->m_refAnimation = m_refDynAnimation;
	refNew->m_nZ = refLevelAnimation->getZ();

	refNew->m_nLastPicDrawn = -1;
	refNew->m_nCachePic = -1;

	double fInverseDuration = 1.0;
	const double fDuration = refLevelAnimation->getDuration();
	if (fDuration <= 0.0) {
		const int32_t nNaturalDuration = m_refDynAnimation->getNaturalDuration();
		assert(nNaturalDuration > 0);
		fInverseDuration = 1.0 / static_cast<double>(nNaturalDuration);
	} else if (fDuration >= LevelAnimation::s_fDurationInfinity) {
		fInverseDuration = 1.0 / LevelAnimation::s_fDurationInfinity;
	} else {
		fInverseDuration = 1.0 / fDuration;
	}
	refNew->m_fInverseDuration = fInverseDuration;

	refNew->m_refThemeContext = refThemeContext;

	return refNew;
}

} // namespace stmg
