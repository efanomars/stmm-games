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
 * File:   cachedsurfaces.cc
 */

#include "gtkutil/cachedsurfaces.h"

#include <stmm-games/util/basictypes.h>

#include <cairomm/enums.h>

#include <cassert>
//#include <iostream>
#include <algorithm>
#include <type_traits>


namespace stmg
{

const Cairo::RefPtr<Cairo::Surface> s_refEmptySurf{};

std::vector<std::pair<NSize, CachedSurfaces::CachedSize>>::iterator CachedSurfaces::findSize(int32_t nW, int32_t nH) noexcept
{
	auto it = std::find_if(m_aCashedSizes.begin(), m_aCashedSizes.end(), [&](const std::pair<NSize, CachedSize>& oT)
		{
			const NSize& oSize = oT.first;
			return (oSize.m_nW == nW) && (oSize.m_nH == nH);
		});
	return it;
}
void CachedSurfaces::addSize(int32_t nW, int32_t nH) noexcept
{
	auto it = findSize(nW, nH);
	if (it == m_aCashedSizes.end()) {
		CachedSize oData;
		oData.m_nRefCount = 1;
		m_aCashedSizes.push_back(std::make_pair(NSize{nW, nH}, std::move(oData)));
	} else {
		++(it->second.m_nRefCount);
	}
}
void CachedSurfaces::subSize(int32_t nW, int32_t nH) noexcept
{
	auto it = findSize(nW, nH);
	assert(it != m_aCashedSizes.end());
	const int32_t nRefCount = it->second.m_nRefCount;
	if (nRefCount == 1) {
		m_aCashedSizes.erase(it);
	} else {
		--(it->second.m_nRefCount);
	}
}
const Cairo::RefPtr<Cairo::Surface>& CachedSurfaces::getCachedRgba(int32_t nW, int32_t nH, bool& bCreated) noexcept
{
	assert(nW > 0);
	assert(nH > 0);
	bCreated = false;
	auto it = findSize(nW, nH);
	if (it == m_aCashedSizes.end()) {
		return s_refEmptySurf; //-----------------------------------------------
	}
	CachedSize& oData = it->second;
	if (!oData.m_refRgba) {
		oData.m_refRgba = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, nW, nH);
		bCreated = true;
	}
	return oData.m_refRgba;
}
Cairo::RefPtr<Cairo::Surface> CachedSurfaces::getRgba(int32_t nW, int32_t nH, bool& bCreated) noexcept
{
	const auto& refSurf = getCachedRgba(nW, nH, bCreated);
	if (!refSurf) {
		bCreated = true;
		return Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, nW, nH); //----
	}
	return refSurf;
}
const Cairo::RefPtr<Cairo::Surface>& CachedSurfaces::getCachedA(int32_t nW, int32_t nH, bool& bCreated) noexcept
{
	assert(nW > 0);
	assert(nH > 0);
	bCreated = false;
	auto it = findSize(nW, nH);
	if (it == m_aCashedSizes.end()) {
		return s_refEmptySurf; //-----------------------------------------------
	}
	CachedSize& oData = it->second;
	if (!oData.m_refA) {
		oData.m_refA = Cairo::ImageSurface::create(Cairo::FORMAT_A8, nW, nH);
		bCreated = true;
	}
	return oData.m_refA;
}
Cairo::RefPtr<Cairo::Surface> CachedSurfaces::getA(int32_t nW, int32_t nH, bool& bCreated) noexcept
{
	const auto& refSurf = getCachedA(nW, nH, bCreated);
	if (!refSurf) {
		bCreated = true;
		return Cairo::ImageSurface::create(Cairo::FORMAT_A8, nW, nH); //--------
	}
	return refSurf;
}
void CachedSurfaces::clear() noexcept
{
	m_aCashedSizes.clear();
}

} // namespace stmg
