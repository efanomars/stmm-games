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
 * File:   frame.cc
 */

#include "gtkutil/frame.h"

#include "gtkutil/image.h"

#include <cassert>
//#include <iostream>
#include <algorithm>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

void Frame::draw(const Cairo::RefPtr<Cairo::Context>& refCc
			, int32_t nPixX, int32_t nPixY, int32_t nPixW, int32_t nPixH) const noexcept
{
	draw(refCc, nPixX, nPixY, nPixW, nPixH, 0, 0, 0, 0);
}
void Frame::draw(const Cairo::RefPtr<Cairo::Context>& refCc
			, int32_t nPixX, int32_t nPixY, int32_t nPixW, int32_t nPixH
			, int32_t nPixFrameTop, int32_t nPixFrameBottom, int32_t nPixFrameLeft, int32_t nPixFrameRight) const noexcept
{
	assert(nPixFrameTop >= 0);
	assert(nPixFrameBottom >= 0);
	assert(nPixFrameLeft >= 0);
	assert(nPixFrameRight >= 0);
	if (m_refBaseImg) {
		m_refBaseImg->draw(refCc, nPixX, nPixY, nPixW, nPixH);
	}
	const int32_t nLeft = std::max(m_nPixMinLeft, nPixFrameLeft);
	const int32_t nRight = std::max(m_nPixMinRight, nPixFrameRight);
	const int32_t nTop = std::max(m_nPixMinTop, nPixFrameTop);
	const int32_t nBottom = std::max(m_nPixMinBottom, nPixFrameBottom);
	if (m_refTopLeftImg) {
		m_refTopLeftImg->draw(refCc, nPixX, nPixY, nLeft, nTop);
	}
	if (m_refTopRightImg) {
		m_refTopRightImg->draw(refCc, nPixX + nPixW - nRight, nPixY, nRight, nTop);
	}
	if (m_refTopImg) {
		m_refTopImg->draw(refCc, nPixX + nLeft, nPixY, nPixW - nLeft - nRight, nTop);
	}
	if (m_refBottomLeftImg) {
		m_refBottomLeftImg->draw(refCc, nPixX, nPixY + nPixH - nBottom, nLeft, nBottom);
	}
	if (m_refBottomRightImg) {
		m_refBottomRightImg->draw(refCc, nPixX + nPixW - nRight, nPixY + nPixH - nBottom, nRight, nBottom);
	}
	if (m_refBottomImg) {
		m_refBottomImg->draw(refCc, nPixX + nLeft, nPixY + nPixH - nBottom, nPixW - nLeft - nRight, nBottom);
	}
	if (m_refLeftImg) {
		m_refLeftImg->draw(refCc, nPixX, nPixY + nTop, nLeft, nPixH - nTop - nBottom);
	}
	if (m_refRightImg) {
		m_refRightImg->draw(refCc, nPixX + nPixW - nRight, nPixY + nTop, nRight, nPixH - nTop - nBottom);
	}
	if (m_refCenterImg) {
		m_refCenterImg->draw(refCc, nPixX + nLeft, nPixY + nTop, nPixW - nLeft - nRight, nPixH - nTop - nBottom);
	}
}

} // namespace stmg
