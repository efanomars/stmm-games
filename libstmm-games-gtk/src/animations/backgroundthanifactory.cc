/*
 * File:   backgroundthanifactory.cc
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

#include "animations/backgroundthanifactory.h"

#include "stdtheme.h"
#include "stdthemecontext.h"

#include "animations/backgroundanimation.h"
#include "gtkutil/image.h"

#include <stmm-games/util/recycler.h>

#include <cairomm/context.h>
#include <cairomm/refptr.h>

#include <cassert>
//#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

namespace stmg { class LevelAnimation; }


namespace stmg
{

void BackgroundThAniFactory::BackgroundThAni::draw(int32_t nViewTick, int32_t nTotViewTicks, const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
//std::cout << "BackgroundThAniFactory::BackgroundThAni::draw nViewTick=" << nViewTick << "   nTotViewTicks=" << nTotViewTicks << '\n';
	BackgroundThAniFactory* p0Factory = m_p1Owner;
	if (p0Factory == nullptr) {
		return; // -------------------------------------------------------------
	}
	StdTheme* p0Theme = p0Factory->owner();
	if (p0Theme == nullptr) {
		return; // -------------------------------------------------------------
	}

	const int32_t nImageIdx = m_refModel->getImageId();
	if (nImageIdx < 0) {
//TODO need Log class for this kind of errors!
//std::cout << "                                      ::draw nImageIdx < 0" << '\n';
		return; // -------------------------------------------------------------
	}
	FPoint oAniPos = m_refModel->getPos();
	FSize oAniSize = m_refModel->getSize();
	FPoint oImgRelPos = m_refModel->getImageRelPos(nViewTick, nTotViewTicks);
	FSize oImgSize = m_refModel->getImageSize();
	if (nImageIdx != m_nCachedImgId) {
		shared_ptr<Image> refImage = p0Theme->getImageById(nImageIdx);
		if (!refImage) {
			// This image is not defined by the theme, draw nothing
//std::cout << "                                      ::draw nImageIdx empty" << '\n';
			return; // ---------------------------------------------------------
		}
		if (m_refCachedImage) {
			if (m_oCachedSize.m_nW != 0) {
				m_refCachedImage->releaseCachedSize(m_oCachedSize);
				m_oCachedSize = NSize{0,0};
			}
		}
		m_refCachedImage = refImage;
		m_nCachedImgId = nImageIdx;
	}

	const NSize oSize = m_refThemeContext->getTileSize();
	const int32_t& nTileW = oSize.m_nW;
	const int32_t& nTileH = oSize.m_nH;
//std::cout << "      ::draw nTileW=" << nTileW << "  nTileH=" << nTileH << '\n';

	const int32_t nAniPixX = oAniPos.m_fX * nTileW;
	const int32_t nAniPixY = oAniPos.m_fY * nTileH;
	const int32_t nAniPixW = oAniSize.m_fW * nTileW;
	const int32_t nAniPixH = oAniSize.m_fH * nTileH;

//std::cout << "BackgroundThAni::draw fRelPosX=" << oImgRelPos.m_fX << "  fRelPosY=" << oImgRelPos.m_fY << '\n';
	const int32_t nImgRelPixX = oImgRelPos.m_fX * nTileW;
	const int32_t nImgRelPixY = oImgRelPos.m_fY * nTileH;
	const int32_t nImgPixX = nAniPixX + nImgRelPixX;
	const int32_t nImgPixY = nAniPixY + nImgRelPixY;

	int32_t nImgPixW;
	int32_t nImgPixH;
	double fImgW = oImgSize.m_fW;
	double fImgH = oImgSize.m_fH;
//std::cout << "BackgroundThAni::draw fImgW=" << fImgW << "  fImgH=" << fImgH << '\n';
	const bool bWUndefined = (fImgW <= 0.0);
	const bool bHUndefined = (fImgH <= 0.0);
	if (bWUndefined || bHUndefined) {
		NSize oNatSize = m_refCachedImage->getNaturalSize();
		if (oNatSize.m_nW <= 0) {
			oNatSize.m_nW = 100;
		}
		if (oNatSize.m_nH <= 0) {
			oNatSize.m_nH = 100;
		}
		if (bWUndefined && bHUndefined) {
			nImgPixW = oNatSize.m_nW;
			nImgPixH = oNatSize.m_nH;
		} else if (bWUndefined) {
			nImgPixH = fImgH * nTileH;
			nImgPixW = nImgPixH * oNatSize.m_nW / oNatSize.m_nH;
		} else {
			assert(bHUndefined);
			nImgPixW = fImgW * nTileW;
			nImgPixH = nImgPixW * oNatSize.m_nH / oNatSize.m_nW;
		}
//std::cout << "      ::draw nNatPixW=" << nNatPixW << "  nNatPixH=" << nNatPixH << "  nImgPixW=" << nImgPixW << "  nImgPixH=" << nImgPixH << '\n';
	} else {
		nImgPixW = fImgW * nTileW;
		nImgPixH = fImgH * nTileH;
	}
	if ((nImgPixW <= 0) || (nImgPixH <= 0)) {
		return; // -------------------------------------------------------------
	}
	const int32_t nTLx = std::floor(1.0 * (nAniPixX - nImgPixX) / nImgPixW);
	const int32_t nTLy = std::floor(1.0 * (nAniPixY - nImgPixY) / nImgPixH);
	const int32_t nBRx = std::floor(1.0 * (nAniPixX + nAniPixW - 1 - nImgPixX) / nImgPixW);
	const int32_t nBRy = std::floor(1.0 * (nAniPixY + nAniPixH - 1 - nImgPixY) / nImgPixH);
//std::cout << "BackgroundThAni::draw nAniPixX=" << nAniPixX << "  nAniPixY=" << nAniPixY << "  nAniPixW=" << nAniPixW << "  nAniPixH=" << nAniPixH << '\n';
//std::cout << "BackgroundThAni::draw nImgPixX=" << nImgPixX << "  nImgPixY=" << nImgPixY << "  nImgPixW=" << nImgPixW << "  nImgPixH=" << nImgPixH << '\n';
//std::cout << "BackgroundThAni::draw nTLx=" << nTLx << "  nTLy=" << nTLy << "  nBRx=" << nBRx << "  nBRy=" << nBRy << '\n';

	if (m_oCachedSize.m_nW == 0) {
		m_oCachedSize = NSize{nImgPixW, nImgPixH};
		m_refCachedImage->addCachedSize(m_oCachedSize.m_nW, m_oCachedSize.m_nH);
	}
	refCc->save();
	refCc->rectangle(nAniPixX, nAniPixY, nAniPixW, nAniPixH);
	refCc->clip();
	for (int32_t nIx = nTLx; nIx <= nBRx; ++nIx) {
		const int32_t nDestPixX = nImgPixX + nIx * nImgPixW;
		for (int32_t nIy = nTLy; nIy <= nBRy; ++nIy) {
			const int32_t nDestPixY = nImgPixY + nIy * nImgPixH;
//std::cout << "      ::draw nDestPixX=" << nDestPixX << "  nDestPixY=" << nDestPixY << "  nImgPixW=" << nImgPixW << "  nImgPixH=" << nImgPixH << '\n';
			m_refCachedImage->draw(refCc, nDestPixX, nDestPixY, nImgPixW, nImgPixH);
		}
	}
	refCc->restore();
}
int32_t BackgroundThAniFactory::BackgroundThAni::getZ(int32_t /*nViewTick*/, int32_t /*nTotViewTicks*/) noexcept
{
	return m_nZ;
}
bool BackgroundThAniFactory::BackgroundThAni::isStarted(int32_t nViewTick, int32_t nTotViewTicks) noexcept
{
	assert(m_refModel);
	return m_refModel->isStarted(nViewTick, nTotViewTicks);
}
bool BackgroundThAniFactory::BackgroundThAni::isDone(int32_t nViewTick, int32_t nTotViewTicks) noexcept
{
	assert(m_refModel);
	const bool bIsDone = m_refModel->isDone(nViewTick, nTotViewTicks);
	if (bIsDone) {
		m_refModel.reset();
	}
	return bIsDone;
}
void BackgroundThAniFactory::BackgroundThAni::onRemoved() noexcept
{
	m_refModel.reset();
}

BackgroundThAniFactory::BackgroundThAniFactory(StdTheme* p1Owner) noexcept
: StdThemeAnimationFactory(p1Owner)
{
}
bool BackgroundThAniFactory::supports(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refLevelAnimation);
	return (nullptr != dynamic_cast<BackgroundAnimation*>(refLevelAnimation.get()));
}
shared_ptr<ThemeAnimation> BackgroundThAniFactory::create(const shared_ptr<StdThemeContext>& refThemeContext, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
//std::cout << "BackgroundThAniFactory::create()" << '\n';
	assert(refThemeContext);
	assert(refLevelAnimation);

	auto refModel = std::dynamic_pointer_cast<BackgroundAnimation>(refLevelAnimation);
	if (!refModel) {
		return shared_ptr<ThemeAnimation>{};
	}

	shared_ptr<BackgroundThAni> refNew;
	m_oBackgroundThAnis.create(refNew);
	refNew->m_p1Owner = this;
	refNew->m_refModel = refModel;
	refNew->m_nZ = refModel->getZ();

	refNew->m_nCachedImgId = -1;
	if (refNew->m_refCachedImage) {
		if (refNew->m_oCachedSize.m_nW != 0) {
			refNew->m_refCachedImage->releaseCachedSize(refNew->m_oCachedSize);
		}
	}
	refNew->m_refCachedImage.reset();
	refNew->m_oCachedSize = NSize{0,0};

	refNew->m_refThemeContext = refThemeContext;

	return refNew;
}

} // namespace stmg
