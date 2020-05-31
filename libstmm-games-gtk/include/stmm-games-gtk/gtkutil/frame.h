/*
 * File:   frame.h
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

#ifndef STMG_FRAME_H
#define STMG_FRAME_H

#include <memory>

#include <stdint.h>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace stmg { class Image; }

namespace stmg
{

using std::shared_ptr;

//TODO also repeat img in top,bottom,left,right, center

/** Frame of images.
 * Example: given a rectangle of size 14 x 7 pixels
 *     nPixFrameTop = 1
 *     nPixFrameBottom = 2
 *     nPixFrameLeft = 2
 *     nPixFrameRight = 3
 *
 *     AABBBBBBBBBCCC
 *     DDeeeeeeeeeFFF
 *     DDeeeeeeeeeFFF
 *     DDeeeeeeeeeFFF
 *     DDeeeeeeeeeFFF
 *     GGHHHHHHHHHIII
 *     GGHHHHHHHHHIII
 *
 *     A: pixels painted by m_refTopLeftImg
 *     B: pixels painted by m_refTopImg
 *     C: pixels painted by m_refTopRightImg
 *     D: pixels painted by m_refLeftImg
 *     e: pixels painted by m_refCenterImg
 *     F: pixels painted by m_refRightImg
 *     G: pixels painted by m_refBottomLeftImg
 *     H: pixels painted by m_refBottomImg
 *     I: pixels painted by m_refBottomRightImg
 */
class Frame {
public:
	int32_t m_nPixMinTop = 0; /**< Top margin in pixels. Default: 0. */
	int32_t m_nPixMinBottom = 0; /**< Bottom margin in pixels. Default: 0. */
	int32_t m_nPixMinLeft = 0; /**< Left margin in pixels. Default: 0. */
	int32_t m_nPixMinRight = 0; /**< Right margin in pixels. Default: 0. */
	shared_ptr<Image> m_refBaseImg; /**< The "bakground" image, all other images are on top of it. Covers the whole rectangle. Can be null. */
	shared_ptr<Image> m_refTopLeftImg; /**< Can be null. */
	shared_ptr<Image> m_refTopImg; /**< Can be null. */
	shared_ptr<Image> m_refTopRightImg /**< Can be null. */;
	shared_ptr<Image> m_refLeftImg; /**< Can be null. */
	shared_ptr<Image> m_refCenterImg; /**< Can be null. */
	shared_ptr<Image> m_refRightImg; /**< Can be null. */
	shared_ptr<Image> m_refBottomLeftImg; /**< Can be null. */
	shared_ptr<Image> m_refBottomImg; /**< Can be null. */
	shared_ptr<Image> m_refBottomRightImg; /**< Can be null. */
public:
	/** Draw frame in a given area using the minimum frame sizes.
	 * @param refCc The context. Cannot be null.
	 * @param nPixX The area x in pixels.
	 * @param nPixY The area y in pixels.
	 * @param nPixW The area width in pixels. Must be positive.
	 * @param nPixH The area height in pixels. Must be positive.
	 */
	void draw(const Cairo::RefPtr<Cairo::Context>& refCc, int32_t nPixX, int32_t nPixY, int32_t nPixW, int32_t nPixH) const noexcept;
	/** Draw frame in a given area with minimal margin sizes.
	 * @param refCc The context. Cannot be null.
	 * @param nPixX The area x in pixels.
	 * @param nPixY The area y in pixels.
	 * @param nPixW The area width in pixels. Must be positive.
	 * @param nPixH The area height in pixels. Must be positive.
	 * @param nPixFrameTop Top margin size in pixels. Cannnot be negative. If &lt; m_nPixMinTop, m_nPixMinTop is used.
	 * @param nPixFrameBottom Bottom margin size in pixels. Cannnot be negative. If &lt; m_nPixMinBottom, m_nPixMinBottom is used.
	 * @param nPixFrameLeft Left margin size in pixels. Cannnot be negative. If &lt; m_nPixMinLeft, m_nPixMinLeft is used.
	 * @param nPixFrameRight Right margin size in pixels. Cannnot be negative. If &lt; m_nPixMinRight, m_nPixMinRight is used.
	 */
	void draw(const Cairo::RefPtr<Cairo::Context>& refCc
				, int32_t nPixX, int32_t nPixY, int32_t nPixW, int32_t nPixH
				, int32_t nPixFrameTop, int32_t nPixFrameBottom, int32_t nPixFrameLeft, int32_t nPixFrameRight) const noexcept;
};


} // namespace stmg

#endif	/* STMG_FRAME_H */

