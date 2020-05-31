/*
 * File:   image.h
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

#ifndef STMG_IMAGE_H
#define STMG_IMAGE_H

#include "cachedsurfaces.h"

#include <stmm-games-file/file.h>

#include <stmm-games/util/basictypes.h>

#include <cairomm/refptr.h>
#include <glibmm/refptr.h>
#include <librsvg/rsvg.h>

#include <memory>
#include <limits>

#include <stdint.h>

namespace Cairo { class Context; }
namespace Cairo { class Surface; }
namespace Gdk { class Pixbuf; }

namespace stmg
{

using std::shared_ptr;

/** Image class.
 * Can explicitely cache specific sizes of the image.
 */
class Image
{
public:
	/** Image from a file constructor.
	 * @param oImageFile The file. Must be defined.
	 */
	explicit Image(const File& oImageFile) noexcept;
	/** Sub-image of another image constructor.
	 * @param refImage The master image. Cannot be null.
	 * @param oSubRect The rectangle within the master image in pixels. Width and height must be positive numbers.
	 */
	Image(const shared_ptr<Image>& refImage, const NRect& oSubRect) noexcept;
	/** Sub-image of another image constructor.
	 * @param refImage The master image. Cannot be null.
	 * @param nSubX The rectangle's x within the master image in pixels.
	 * @param nSubY The rectangle's y within the master image in pixels.
	 * @param nSubW The rectangle's w within the master image in pixels. Must be positive.
	 * @param nSubH The rectangle's h within the master image in pixels. Must be positive.
	 */
	Image(const shared_ptr<Image>& refImage, int32_t nSubX, int32_t nSubY, int32_t nSubW, int32_t nSubH) noexcept;
	/** Non virtual destructor.
	 */
	~Image() noexcept;
	/** Draws the image within a rectangle.
	 * The image is loaded if necessary.
	 * The image is stretched over the rectangle.
	 * If the image cannot be loaded draws nothing.
	 * @param refCc The context. Cannot be null.
	 * @param nX The rectangle's x in pixels.
	 * @param nY The rectangle's y in pixels.
	 * @param nW The rectangle's w in pixels.
	 * @param nH The rectangle's h in pixels.
	 */
	void draw(const Cairo::RefPtr<Cairo::Context>& refCc, int32_t nX, int32_t nY, int32_t nW, int32_t nH) noexcept;
	/** Whether the image is already loaded into memory.
	 * @return Whether already loaded.
	 */
	bool isLoaded() const noexcept;
	/** Loads the image into memory.
	 * @return Whether succeeded.
	 */
	bool load() noexcept;
	/** The natural size of the image.
	 * @return The natural size.
	 */
	NSize getNaturalSize() noexcept;
	/** Add a cached size.
	 * Cached sizes are reference counted.
	 * Remember to call releaseCachedSize() when no longer needed.
	 * @param nW The width in pixels.
	 * @param nH The height in pixels.
	 */
	void addCachedSize(int32_t nW, int32_t nH) noexcept;
	/** Add a cached size.
	 * Cached sizes are reference counted.
	 * Remember to call releaseCachedSize() when no longer needed.
	 * @param oSize The size in pixel.
	 */
	void addCachedSize(NSize oSize) noexcept;
	/** Release a cached size.
	 * If the reference count reaches 0 the cached image is removed.
	 * @param nW The width in pixels.
	 * @param nH The height in pixels.
	 */
	void releaseCachedSize(int32_t nW, int32_t nH) noexcept;
	/** Release a cached size.
	 * If the reference count reaches 0 the cached image is removed.
	 * @param oSize The size in pixel.
	 */
	void releaseCachedSize(NSize oSize) noexcept;
	/** Gets a surface of a given size with the image painted on it.
	 * If the requested size is cached the cached surface is returned.
	 * @param nW The width in pixels.
	 * @param nH The height in pixels.
	 * @return The surface. Is not null.
	 */
	Cairo::RefPtr<Cairo::Surface> getAsSurface(int32_t nW, int32_t nH) noexcept;
	/** Gets a cached surface with the image painted on it.
	 * The addCachedSize() function must have been called for the size.
	 * @param nW The width. Must be &gt; 0.
	 * @param nH The height. Must be &gt; 0.
	 * @return The surface or null if not cached.
	 */
	const Cairo::RefPtr<Cairo::Surface>& getAsCachedSurface(int32_t nW, int32_t nH) noexcept;
	/** Gets a mask surface of a given size with the image painted on it.
	 * If the requested size is cached the cached surface is returned.
	 * @param nW The width in pixels.
	 * @param nH The height in pixels.
	 * @return The mask (8 bit) surface. Is not null.
	 */
	Cairo::RefPtr<Cairo::Surface> getAsMaskSurface(int32_t nW, int32_t nH) noexcept;
	/** Gets a cached mask surface with the image painted on it.
	 * The addCachedSize() function must have been called for the size.
	 * @param nW The width. Must be &gt; 0.
	 * @param nH The height. Must be &gt; 0.
	 * @return The mask (8 bit) surface or null if not cached.
	 */
	const Cairo::RefPtr<Cairo::Surface>& getAsCachedMaskSurface(int32_t nW, int32_t nH) noexcept;
private:
	File m_oImageFile;
	int32_t m_nW, m_nH;
	CachedSurfaces m_oCached;
	Glib::RefPtr<Gdk::Pixbuf> m_refPixbuf;
	RsvgHandle* m_pHandle;
	constexpr static int32_t s_nNotSubX = std::numeric_limits<int32_t>::max();
	const int32_t m_nSubX, m_nSubY;
	const shared_ptr<Image> m_refMasterImage;
private:
	Image(const Image& oSource) = delete;
	Image& operator=(const Image& oSource) = delete;
};


} // namespace stmg

#endif	/* STMG_IMAGE_H */

