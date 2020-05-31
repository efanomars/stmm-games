/*
 * File:   dynanimation.h
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

#ifndef STMG_DYN_ANIMATION_H
#define STMG_DYN_ANIMATION_H

#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/randomparts.h>

#include <memory>

#include <stdint.h>

namespace stmg { class Image; }

namespace stmg
{

using std::shared_ptr;

class DynAnimation
{
public:
	struct DynImage {
		FRect m_oRelRect; /**< The position and size of the image relative to the animations rectangle. */
		shared_ptr<Image> m_refImage; /**< The image to display. Cannot be empty. */
		int32_t m_nPriority; /**< The priority of the image. */
	};

	/** Constructor.
	 */
	DynAnimation() noexcept;

	/** Add an image.
	 * See addImage(int32_t nNaturalDuration, DynImage&& oDynImage).
	 */
	void addImage(int32_t nNaturalDuration, shared_ptr<Image>& refImage
				, double fRelX, double fRelY, double fRelW, double fRelH
				, int32_t nPriority) noexcept;
	/** Add an image.
	 * See addImage(int32_t nNaturalDuration, DynImage&& oDynImage).
	 */
	void addImage(int32_t nNaturalDuration, shared_ptr<Image>& refImage
				, FRect oRelRect, int32_t nPriority) noexcept;
	/** Add an image.
	 * The natural duration can also e interpreted as weight if the duration
	 * of the animation is determined by the model.
	 *
	 * When an animation can't paint all the images because the frame rate is too low,
	 * it chooses the one with the highest priority.
	 * @param nNaturalDuration The duration in milliseconds in the animation. Must be positive.
	 * @param oDynImage The image data.
	 */
	void addImage(int32_t nNaturalDuration, DynImage&& oDynImage) noexcept;

	/** Get the image data at a certain unary time of the animation.
	 * Elapsed value 0.0 is start, 1.0 is end of animation.
	 * @param fElapsed The unary time. Must be &gt;= 0.0 and &lt;= 1.0.
	 * @return The image data.
	 */
	const DynImage& getImage(double fElapsed) noexcept;

	/** The image index at a certain unary time of the animation.
	 * Elapsed value 0.0 is start, 1.0 is end of animation.
	 * @param fElapsed The unary time. Must be &gt;= 0.0 and &lt;= 1.0.
	 * @return The index starting from 0 (the first added image).
	 */
	int32_t getImageIdx(double fElapsed) noexcept;

	/** The natural duration of the whole animation in milliseconds.
	 * @return The sum of all natural durations of the added images.
	 */
	int32_t getNaturalDuration() const noexcept;

	/** The number of images.
	 * @return The total number of images.
	 */
	int32_t getTotImages() const noexcept;
	/** Get image data by index.
	 * @param nIdx The index. Must be &gt;= 0 and &lt; getTotImages().
	 * @return The image data.
	 */
	const DynImage& getImageByIdx(int32_t nIdx) noexcept;
private:
	// fElapsed  0.0 is start, 1.0 is end of animation
	const DynImage& getImage(double fElapsed, int32_t& nIdx) noexcept;
private:
	RandomParts< DynImage > m_oDynImages;

private:
	DynAnimation(const DynAnimation& oSource) = delete;
	DynAnimation& operator=(const DynAnimation& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_DYN_ANIMATION_H */

