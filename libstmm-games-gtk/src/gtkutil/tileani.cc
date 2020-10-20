/*
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
/*
 * File:   tileani.cc
 */

#include "gtkutil/tileani.h"

#include <stmm-games/util/randomparts.h>

#include <cassert>
//#include <iostream>

namespace stmg { class Image; }

namespace stmg
{

TileAni::TileAni() noexcept
{
}
void TileAni::setDefaultImage(const shared_ptr<Image>& refImage) noexcept
{
	m_refDefaultImage = refImage;
}
void TileAni::addImage(int32_t nDuration, const shared_ptr<Image>& refImage) noexcept
{
	m_oDurationImages.addRandomPart(nDuration, refImage);
}
const shared_ptr<Image>& TileAni::getDefaultImage() noexcept
{
	return m_refDefaultImage;
}
const shared_ptr<Image>& TileAni::getImage(double fElapsed, int32_t& nIdx) noexcept
{
	assert((fElapsed >= 0.0) && (fElapsed <= 1.0));
	if (m_oDurationImages.getTotRandomParts() <= 0) {
		return getDefaultImage(); //--------------------------------------------
	}
	fElapsed = 1.0 - fElapsed;
	const int32_t nRange = m_oDurationImages.getRandomRange();
	int32_t nRandom = fElapsed * nRange;
	nRandom = nRange - 1 - nRandom;
	if (nRandom < 0) {
		nRandom = 0;
	}
	assert((nRandom >= 0) && (nRandom < nRange));
	return m_oDurationImages.getRandomPartProb(nRandom, nIdx);
}
const shared_ptr<Image>& TileAni::getImage(double fElapsed) noexcept
{
	int32_t nIdx;
	return getImage(fElapsed, nIdx);
}
int32_t TileAni::getTotImages() const noexcept
{
	return m_oDurationImages.getTotRandomParts();
}
const shared_ptr<Image>& TileAni::getImageByIdx(int32_t nIdx) noexcept
{
	return m_oDurationImages.getRandomPart(nIdx);
}

} // namespace stmg
