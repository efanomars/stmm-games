/*
 * File:   dynanimation.cc
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

#include "gtkutil/dynanimation.h"

#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/randomparts.h>

#include <cassert>
//#include <iostream>
#include <algorithm>
#include <utility>

namespace stmg { class Image; }

namespace stmg
{

DynAnimation::DynAnimation() noexcept
{
}
void DynAnimation::addImage(int32_t nNaturalDuration, shared_ptr<Image>& refImage
							, double fRelX, double fRelY, double fRelW, double fRelH
							, int32_t nPriority) noexcept
{
	DynImage oNew;
	oNew.m_refImage = refImage;
	oNew.m_oRelRect.m_fX = fRelX;
	oNew.m_oRelRect.m_fY = fRelY;
	oNew.m_oRelRect.m_fW = fRelW;
	oNew.m_oRelRect.m_fH = fRelH;
	oNew.m_nPriority = nPriority;
	addImage(nNaturalDuration, std::move(oNew));
}
void DynAnimation::addImage(int32_t nNaturalDuration, shared_ptr<Image>& refImage
							, FRect oRelRect, int32_t nPriority) noexcept
{
	DynImage oNew;
	oNew.m_refImage = refImage;
	oNew.m_oRelRect = oRelRect;
	oNew.m_nPriority = nPriority;
	addImage(nNaturalDuration, std::move(oNew));
}
void DynAnimation::addImage(int32_t nNaturalDuration, DynImage&& oDynImage) noexcept
{
	assert(oDynImage.m_refImage);
	m_oDynImages.addRandomPart(nNaturalDuration, std::move(oDynImage));
}

const DynAnimation::DynImage& DynAnimation::getImage(double fElapsed) noexcept
{
	int32_t nIdx;
	return getImage(fElapsed, nIdx);
}
const DynAnimation::DynImage& DynAnimation::getImage(double fElapsed, int32_t& nIdx) noexcept
{
	assert((fElapsed >= 0.0) && (fElapsed <= 1.0));
	assert(m_oDynImages.getTotRandomParts() > 0);
	fElapsed = 1.0 - fElapsed;
	const int32_t nRange = m_oDynImages.getRandomRange();
	int32_t nRandom = fElapsed * nRange;
	nRandom = nRange - 1 - nRandom;
	if (nRandom < 0) {
		nRandom = 0;
	}
	assert((nRandom >= 0) && (nRandom < nRange));
	return m_oDynImages.getRandomPartProb(nRandom, nIdx);
}
int32_t DynAnimation::getImageIdx(double fElapsed) noexcept
{
	assert((fElapsed >= 0.0) && (fElapsed <= 1.0));
	assert(m_oDynImages.getTotRandomParts() > 0);
	fElapsed = 1.0 - fElapsed;
	const int32_t nRange = m_oDynImages.getRandomRange();
	int32_t nRandom = fElapsed * nRange;
	nRandom = nRange - 1 - nRandom;
	if (nRandom < 0) {
		nRandom = 0;
	}
	assert((nRandom >= 0) && (nRandom < nRange));
	int32_t nIdx;
	m_oDynImages.getRandomPartProb(nRandom, nIdx);
	return nIdx;
}
int32_t DynAnimation::getTotImages() const noexcept
{
	return m_oDynImages.getTotRandomParts();
}
const DynAnimation::DynImage& DynAnimation::getImageByIdx(int32_t nIdx) noexcept
{
	return m_oDynImages.getRandomPart(nIdx);
}
int32_t DynAnimation::getNaturalDuration() const noexcept
{
	return m_oDynImages.getRandomRange();
}

} // namespace stmg
