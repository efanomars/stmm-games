/*
 * File:   staticgridthanifactory.cc
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

#include "animations/staticgridthanifactory.h"

#include "stdthemecontext.h"
#include "stdtheme.h"

#include "animations/staticgridanimation.h"
#include "gtkutil/image.h"

#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <cairomm/enums.h>

#include <vector>
#include <cassert>
//#include <iostream>

namespace stmg { class LevelAnimation; }

namespace stmg
{

void StaticGridThAniFactory::StaticGridThAni::draw(int32_t /*nViewTick*/, int32_t /*nTotViewTicks*/, const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
	StaticGridThAniFactory* p0Factory = m_p1Owner;
	if (p0Factory == nullptr) {
		return;
	}
	StdTheme* p0Theme = p0Factory->owner();
	if (p0Theme == nullptr) {
		return;
	}

	const bool bNewSize = getRectInternal();
	if (bNewSize) {
//std::cout << "StaticGridThAniFactory::StaticGridThAni::draw new size  m_oPixRect.m_nW=" << m_oPixRect.m_nW << "  m_oPixRect.m_nH=" << m_oPixRect.m_nH << '\n';
		int32_t nSurfPixW = m_oPixRect.m_nW;
		int32_t nSurfPixH = m_oPixRect.m_nH;
		bool bEnlarged = false;
		if (m_refSurf) {
			const auto nCurWidth = m_refSurf->get_width();
			if (nCurWidth < nSurfPixW) {
				bEnlarged = true;
			} else {
				nSurfPixW = nCurWidth;
			}
			const auto nCurHeigth = m_refSurf->get_height();
			if (nCurHeigth < nSurfPixH) {
				bEnlarged = true;
			} else {
				nSurfPixH = nCurHeigth;
			}
		} else {
			bEnlarged = true;
		}
		if (bEnlarged) {
//std::cout << "StaticGridThAniFactory::StaticGridThAni::draw enlarged  nSurfPixW=" << nSurfPixW << "  nSurfPixH=" << nSurfPixH << '\n';
			m_refSurf = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, nSurfPixW, nSurfPixH);
			m_refCc = Cairo::Context::create(m_refSurf);
		}

		m_refCc->save();
		m_refCc->set_source_rgba(0, 0, 0, 0); // transparency
		m_refCc->set_operator(Cairo::OPERATOR_SOURCE);
		m_refCc->rectangle(0, 0, m_oPixRect.m_nW, m_oPixRect.m_nH);
		m_refCc->fill();

		m_refCc->set_operator(Cairo::OPERATOR_OVER);

		const NSize oGridCellsSize = m_refModel->getGridCellsSize();
		const auto& aSpans = m_refModel->getSpans();
		const int32_t nCellPixW = m_oPixRect.m_nW / oGridCellsSize.m_nW;
		const int32_t nCellPixH = m_oPixRect.m_nH / oGridCellsSize.m_nH;
//std::cout << "StaticGridThAniFactory::StaticGridThAni::draw  nCellPixW=" << nCellPixW << "  nCellPixH=" << nCellPixH << '\n';

		if ((nCellPixW > 0) && (nCellPixH > 0)) {

			for (const StaticGridAnimation::ImageSpan& oSpan : aSpans) {
//std::cout << "StaticGridThAniFactory::StaticGridThAni::draw  image oSpan.m_nImgId=" << oSpan.m_nImgId << '\n';
				const shared_ptr<Image>& refImage = p0Theme->getImageById(oSpan.m_nImgId);
				if (!refImage) {
					continue; //for ---
				}
				const int32_t nBaseImgPixX = oSpan.m_oPos.m_nX * nCellPixW;
				const int32_t nBaseImgPixY = oSpan.m_oPos.m_nY * nCellPixH;
				const int32_t nImgPixW = oSpan.m_oSpanSize.m_nW * nCellPixW;
				const int32_t nImgPixH = oSpan.m_oSpanSize.m_nH * nCellPixH;
				for (int32_t nIdxX = 0; nIdxX < oSpan.m_oRepeatSize.m_nW; ++nIdxX) {
					const int32_t nImgPixX = nBaseImgPixX + nIdxX * nImgPixW;
					for (int32_t nIdxY = 0; nIdxY < oSpan.m_oRepeatSize.m_nH; ++nIdxY) {
						const int32_t nImgPixY = nBaseImgPixY + nIdxY * nImgPixH;
						refImage->draw(m_refCc, nImgPixX, nImgPixY, nImgPixW, nImgPixH);
//std::cout << "StaticGridThAniFactory::StaticGridThAni::draw nImgPixX=" << nImgPixX << " nImgPixY=" << nImgPixY << " nImgPixW=" << nImgPixW << " nImgPixH=" << nImgPixH << '\n';
					}
				}
			}
		}
		m_refCc->restore();
	}
	refCc->save();
	//m_refCc->set_operator(Cairo::OPERATOR_OVER);
	refCc->set_source(m_refSurf, m_oPixRect.m_nX, m_oPixRect.m_nY);
	refCc->paint_with_alpha(p0Factory->m_fOpacity01);
	refCc->restore();
}
bool StaticGridThAniFactory::StaticGridThAni::getRectInternal() noexcept
{
	const NSize oTileSize = m_refThemeContext->getTileSize();
	const int32_t& nTileW = oTileSize.m_nW;
	const int32_t& nTileH = oTileSize.m_nH;

	const FPoint oGridPos = m_refModel->getPos();
	m_oPixRect.m_nX = oGridPos.m_fX * nTileW;
	m_oPixRect.m_nY = oGridPos.m_fY * nTileH;

	if ((m_nPixRectTileW == nTileW) && (m_nPixRectTileH == nTileH)) {
		return false; //--------------------------------------------------------
	}

	m_nPixRectTileW = nTileW;
	m_nPixRectTileH = nTileH;

	const FSize oGridSize = m_refModel->getSize();
	m_oPixRect.m_nW = oGridSize.m_fW * nTileW;
	m_oPixRect.m_nH = oGridSize.m_fH * nTileH;
	return true;
}
int32_t StaticGridThAniFactory::StaticGridThAni::getZ(int32_t /*nViewTick*/, int32_t /*nTotViewTicks*/) noexcept
{
	return m_nZ;
}
bool StaticGridThAniFactory::StaticGridThAni::isStarted(int32_t nViewTick, int32_t nTotViewTicks) noexcept
{
	return m_refModel->isStarted(nViewTick, nTotViewTicks);
}
bool StaticGridThAniFactory::StaticGridThAni::isDone(int32_t nViewTick, int32_t nTotViewTicks) noexcept
{
	const bool bIsDone = m_refModel->isDone(nViewTick, nTotViewTicks) || m_refModel->isStopped();
	if (bIsDone) {
		m_refModel.reset();
	}
	return bIsDone;
}
void StaticGridThAniFactory::StaticGridThAni::onRemoved() noexcept
{
	m_refModel.reset();
}

StaticGridThAniFactory::StaticGridThAniFactory(StdTheme* p1Owner, double fOpacity01) noexcept
: StdThemeAnimationFactory(p1Owner)
, m_fOpacity01(fOpacity01)
{
	assert(fOpacity01 >= 0.0);
	assert(fOpacity01 <= 1.0);
}
bool StaticGridThAniFactory::supports(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refLevelAnimation);
	return (nullptr != dynamic_cast<StaticGridAnimation*>(refLevelAnimation.get()));
}
shared_ptr<ThemeAnimation> StaticGridThAniFactory::create(const shared_ptr<StdThemeContext>& refThemeContext
														, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refThemeContext);
	assert(refLevelAnimation);

	auto refModel = std::dynamic_pointer_cast<StaticGridAnimation>(refLevelAnimation);
	if (!refModel) {
		return shared_ptr<ThemeAnimation>{};
	}
	
	shared_ptr<StaticGridThAniFactory::StaticGridThAni> refNew;
	m_oStaticGridThAnis.create(refNew);
	refNew->m_p1Owner = this;
	refNew->m_refModel = refModel;
	refNew->m_nZ = refModel->getZ();
//std::cout << "StaticGridThAniFactory::create refNew->m_nZ=" << refNew->m_nZ << '\n';

	refNew->m_refThemeContext = refThemeContext;

	return refNew;
}

} // namespace stmg
