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
 * File:   cachedsurfaces.h
 */

#ifndef STMG_CACHED_SURFACES_H
#define STMG_CACHED_SURFACES_H

#include <cairomm/refptr.h>
#include <cairomm/surface.h>

#include <vector>
#include <utility>

#include <stdint.h>

namespace stmg { struct NSize; }

namespace stmg
{

/** Cairo cached surfaces for an image.
 * Keeps sized surfaces to avoid redrawing the same image.
 * The creation is done lazily.
 */
class CachedSurfaces
{
public:
	/** Tells to keep a ref counted cache with a certain size.
	 * The surface isn't created right away but only when first
	 * requested by getRgba() or getA().
	 * @param nW The width of the cached surface(s).
	 * @param nH The height of the cached surface(s).
	 */
	void addSize(int32_t nW, int32_t nH) noexcept;
	/** Decrement the ref count for a cached surface.
	 * If the ref count reaches 0 the cached surface is removed.
	 * @param nW The width of the cached surface(s).
	 * @param nH The height of the cached surface(s).
	 */
	void subSize(int32_t nW, int32_t nH) noexcept;
	/** Returns a cached rgba surface.
	 * @param nW The width in pixels.
	 * @param nH The height in pixels.
	 * @param bCreated Whether the surface had to be created.
	 * @return The surface or null if the size is not cached.
	 */
	const Cairo::RefPtr<Cairo::Surface>& getCachedRgba(int32_t nW, int32_t nH, bool& bCreated) noexcept;
	/** Return a rgba (possibly cached) surface of a certain size.
	 * @param nW The width of the cached surface.
	 * @param nH The height of the cached surface.
	 * @param bCreated Whether the surface had to be created.
	 * @return The rgba surface.
	 */
	Cairo::RefPtr<Cairo::Surface> getRgba(int32_t nW, int32_t nH, bool& bCreated) noexcept;
	/** Returns a cached alpha channel surface.
	 * @param nW The width in pixels.
	 * @param nH The height in pixels.
	 * @param bCreated Whether the surface had to be created.
	 * @return The alpha surface or null if the size is not cached.
	 */
	const Cairo::RefPtr<Cairo::Surface>& getCachedA(int32_t nW, int32_t nH, bool& bCreated) noexcept;
	/** Return a possibly cached alpha channel surface of a certain size.
	 * @param nW The width of the surface in pixel.
	 * @param nH The height of the surface in pixel.
	 * @param bCreated Whether the surface had to be created.
	 * @return The alpha channel surface.
	 */
	Cairo::RefPtr<Cairo::Surface> getA(int32_t nW, int32_t nH, bool& bCreated) noexcept;
	/** Clears all ref counted sized surfaces.
	 * Also resets all the ref counts.
	 */
	void clear() noexcept;
private:
	struct CachedSize {
		int32_t m_nRefCount;
		Cairo::RefPtr<Cairo::Surface> m_refRgba; //lazy creation
		Cairo::RefPtr<Cairo::Surface> m_refA; //lazy creation
	};
private:
	std::vector<std::pair<NSize, CachedSize>>::iterator findSize(int32_t nW, int32_t nH) noexcept;
private:
	std::vector<std::pair<NSize, CachedSize>> m_aCashedSizes;
};

} // namespace stmg

#endif	/* STMG_CACHED_SURFACES_H */

