/*
 * File:   tileani.h
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

#ifndef STMG_TILE_ANI_H
#define STMG_TILE_ANI_H

#include <stmm-games/util/randomparts.h>

#include <memory>

#include <stdint.h>

namespace stmg { class Image; }

namespace stmg
{

using std::shared_ptr;

/** Tile animation made of images.
 * Each image in the animation has a duration weight.
 *
 * The elapsed time is not in milliseconds but a number from 0.0 to 1.0. Elapsed
 * 0.0 is the start of the animation and 1.0 is the end.
 */
class TileAni
{
public:
	/** Constructor.
	 */
	TileAni() noexcept;
	/** Set the default image.
	 * The default image can only be set once.
	 * @param refImage The default image. Can be null.
	 */
	void setDefaultImage(const shared_ptr<Image>& refImage) noexcept;
	/** The default image.
	 * @return The default image or null.
	 */
	const shared_ptr<Image>& getDefaultImage() noexcept;

	/** Adds an image to the tile animation.
	 * @param nDuration The duration weight. Must be a positive number.
	 * @param refImage The image. Can be null.
	 */
	void addImage(int32_t nDuration, const shared_ptr<Image>& refImage) noexcept;
	/** The image at a certain point in the animation.
	 * If no image was added the default image is returned.
	 * @param fElapsed The elapsed "time". Must be &gt;= `0.0` and &lt;= `1.0`.
	 * @return The image or null.
	 */
	const shared_ptr<Image>& getImage(double fElapsed) noexcept;
	/** The image and the index at a certain point in the animation.
	 * If no image was added index -1 and the default image is returned.
	 * @param fElapsed The elapsed "time". Must be &gt;= `0.0` and &lt;= `1.0`.
	 * @param nIdx The index of the image or -1 if none was added. (output).
	 * @return The image or null.
	 */
	const shared_ptr<Image>& getImage(double fElapsed, int32_t& nIdx) noexcept;

	/** The nummber of images added.
	 * @return The total number of images.
	 */
	int32_t getTotImages() const noexcept;
	/** The image by index.
	 * @param nIdx The index. Must be &gt;= 0 and &lt; getTotImages().
	 * @return The image or null.
	 */
	const shared_ptr<Image>& getImageByIdx(int32_t nIdx) noexcept;
private:
	shared_ptr<Image> m_refDefaultImage;
	RandomParts< shared_ptr<Image> > m_oDurationImages;

private:
	TileAni(const TileAni& oSource) = delete;
	TileAni& operator=(const TileAni& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_TILE_ANI_H */

