/*
 * File:   containermodifier.cc
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

#include "containermodifier.h"

#include "stdthemedrawingcontext.h"

#include <stmm-games/util/basictypes.h>

#include <cairomm/context.h>
#include <cairomm/enums.h>

#include <cassert>
//#include <iostream>
#include <limits>
#include <algorithm>
#include <utility>

#include <stddef.h>

namespace stmg { class Tile; }


namespace stmg
{

ContainerModifier::ContainerModifier(StdTheme* p1Owner) noexcept
: StdThemeModifier(p1Owner)
{
}
void ContainerModifier::addSubModifiers(std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept
{
	for (auto& refModifier : aModifiers) {
		assert(refModifier);
		m_aSubModifiers.push_back(std::move(refModifier));
	}
}
const std::vector< unique_ptr<StdThemeModifier> >& ContainerModifier::getSubModifiers() noexcept
{
	return m_aSubModifiers;
}
void ContainerModifier::registerTileSize(int32_t nW, int32_t nH) noexcept
{
	for (const auto& refModifier : m_aSubModifiers) {
		refModifier->registerTileSize(nW, nH);
	}
}
void ContainerModifier::unregisterTileSize(int32_t nW, int32_t nH) noexcept
{
	for (const auto& refModifier : m_aSubModifiers) {
		refModifier->unregisterTileSize(nW, nH);
	}
}
StdThemeModifier::FLOW_CONTROL ContainerModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc
										, StdThemeDrawingContext& oDc
										, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept
{
	const auto nTotModifiers = m_aSubModifiers.size();
	for (size_t nPP = 0; nPP < nTotModifiers; ++nPP) {
		const auto& refModifier = m_aSubModifiers[nPP];
//std::cout << "                             " << typeid(*refModifier).name() << '\n';
		const FLOW_CONTROL eCtl = refModifier->drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
		if (eCtl == FLOW_CONTROL_STOP) {
			return FLOW_CONTROL_STOP; //----------------------------------------
		}
	}
	return FLOW_CONTROL_CONTINUE;
}
StdThemeModifier::FLOW_CONTROL ContainerModifier::drawContainedToWorkSurface(StdThemeDrawingContext& oDc
										, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed
										, Cairo::RefPtr<Cairo::Surface>& refSurface) noexcept
{
	const NSize oSize = oDc.getTileSize();
	const int32_t& nTileW = oSize.m_nW;
	const int32_t& nTileH = oSize.m_nH;
	Cairo::RefPtr<Cairo::Context> refSubCc = getWorkContext(nTileW, nTileH);
	refSurface = m_refWork;

	return ContainerModifier::drawTile(refSubCc, oDc, oTile, nPlayer, aAniElapsed);
}
Cairo::RefPtr<Cairo::Context> ContainerModifier::getWorkContext(int32_t nW, int32_t nH) noexcept
{
	assert(nW < std::numeric_limits<int32_t>::max() / 2);
	assert(nH < std::numeric_limits<int32_t>::max() / 2);
	int32_t nCurW, nCurH;
	if (!m_refWork) {
		nCurW = std::max<int32_t>(8, nW);
		nCurH = std::max<int32_t>(8, nH);
		m_refWork = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, nCurW, nCurH);
	} else {
		bool bBigger = false;
		nCurW = m_refWork->get_width();
		nCurH = m_refWork->get_height();
		while (nW > nCurW) {
			nCurW = nCurW * 2;
			bBigger = true;
		}
		while (nH > nCurH) {
			nCurH = nCurH * 2;
			bBigger = true;
		}
		if (bBigger) {
			m_refWork = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, nCurW, nCurH);
		} else {
			Cairo::RefPtr<Cairo::Context> refSubCc = Cairo::Context::create(m_refWork);
			refSubCc->set_operator(Cairo::OPERATOR_SOURCE);
			refSubCc->set_source_rgba(0,0,0,0);
			refSubCc->rectangle(0,0, m_refWork->get_width(), m_refWork->get_height());
			refSubCc->fill();
			return refSubCc;
		}
	}
	Cairo::RefPtr<Cairo::Context> refSubCc = Cairo::Context::create(m_refWork);
	return refSubCc;
}


} // namespace stmg
