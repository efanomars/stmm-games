/*
 * File:   staticgridanimation.cc
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

#include "animations/staticgridanimation.h"

#include <cassert>
#include <utility>

namespace stmg
{

StaticGridAnimation::StaticGridAnimation(Init&& oInit) noexcept
: LevelAnimation(oInit)
, m_oLocalInit(std::move(oInit))
{
	commonInit();
}
void StaticGridAnimation::reInit(Init&& oInit) noexcept
{
	LevelAnimation::reInit(oInit);
	m_oLocalInit = std::move(oInit);
	commonInit();
}

void StaticGridAnimation::commonInit() noexcept
{
	if (m_oLocalInit.m_oCellsSize.m_nW <= 0) {
		m_oLocalInit.m_oCellsSize.m_nW = 1;
	}
	if (m_oLocalInit.m_oCellsSize.m_nH <= 0) {
		m_oLocalInit.m_oCellsSize.m_nH = 1;
	}
	assert(m_oLocalInit.m_aSpans.size() > 0);
	for (ImageSpan& oSpan : m_oLocalInit.m_aSpans) {
		assert(oSpan.m_nImgId >= 0);
		if (oSpan.m_oSpanSize.m_nW <= 0) {
			oSpan.m_oSpanSize.m_nW = 1;
		}
		if (oSpan.m_oSpanSize.m_nH <= 0) {
			oSpan.m_oSpanSize.m_nH = 1;
		}
		if (oSpan.m_oRepeatSize.m_nW <= 0) {
			oSpan.m_oRepeatSize.m_nW = 1;
		}
		if (oSpan.m_oRepeatSize.m_nH <= 0) {
			oSpan.m_oRepeatSize.m_nH = 1;
		}
	}
	m_bIsStopped = false;
}

void StaticGridAnimation::stop() noexcept
{
	m_bIsStopped = true;
}

} // namespace stmg
