/*
 * File:   selectanimodifier.cc
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

#include "modifiers/selectanimodifier.h"
#include "stdtheme.h"

#include <stmm-games/named.h>
#include <stmm-games/util/namedindex.h>

#include <cassert>
#include <cstdint>
#include <utility>

namespace stmg { class Tile; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

SelectAniModifier::SelectAniModifier(StdTheme* p1Owner, int32_t nTileAniId) noexcept
: ContainerModifier(p1Owner)
, m_nTileAniId(nTileAniId)
{
	assert((nTileAniId >= 0) && (nTileAniId < p1Owner->getNamed().tileAnis().size()));
	// The else group is at position 0
	m_aRangeContainers.push_back(std::make_unique<ContainerModifier>(p1Owner));
}
void SelectAniModifier::addCase(double fFrom, bool bFromExcl, double fTo, bool bToExcl
								, std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept
{
//std::cout << "SelectAniModifier::addCase nIdAss=" << nIdAss << "  aModifiers.size()=" << aModifiers.size() << '\n';
	assert((fFrom < fTo) || ((fFrom == fTo) && ((! bFromExcl) && ! bToExcl)));
	CaseFromTo oCaseFromTo;
	oCaseFromTo.m_fFrom = fFrom;
	oCaseFromTo.m_bFromExcl = bFromExcl;
	oCaseFromTo.m_fTo = fTo;
	oCaseFromTo.m_bToExcl = bToExcl;
	m_aRanges.push_back(std::move(oCaseFromTo));
	m_aRangeContainers.push_back(std::make_unique<ContainerModifier>(owner()));
	m_aRangeContainers.back()->addSubModifiers(std::move(aModifiers));
}
void SelectAniModifier::setDefault(std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept
{
//std::cout << "SelectAniModifier::setDefault  aModifiers.size()=" << aModifiers.size() << '\n';
	assert(m_aRangeContainers[0]->getSubModifiers().size() == 0); // Can only be set once
	m_aRangeContainers[0]->addSubModifiers(std::move(aModifiers));
}
StdThemeModifier::FLOW_CONTROL SelectAniModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
													, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept
{
	const double fAniElapsed = aAniElapsed[m_nTileAniId];
	const int32_t nTotCases = static_cast<int32_t>(m_aRanges.size());
	for (int32_t nCase = 0; nCase < nTotCases; ++nCase) {
//std::cout << "            SelectAniModifier::drawTile   nCondition=" << nCondition << '\n';
		const CaseFromTo& oCaseFromTo = m_aRanges[nCase];
		if (oCaseFromTo.m_bFromExcl) {
			if (fAniElapsed <= oCaseFromTo.m_fFrom) {
				continue;
			}
		} else {
			if (fAniElapsed < oCaseFromTo.m_fFrom) {
				continue;
			}
		}
		if (oCaseFromTo.m_bToExcl) {
			if (fAniElapsed >= oCaseFromTo.m_fTo) {
				continue;
			}
		} else {
			if (fAniElapsed > oCaseFromTo.m_fTo) {
				continue;
			}
		}
		const FLOW_CONTROL eCtl = m_aRangeContainers[nCase + 1]->drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
		return eCtl; //-----------------------------------------------------
	}
//std::cout << "            SelectAniModifier::drawTile   DEFAULT" << '\n';
	// Default
	return m_aRangeContainers[0]->drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
}

} // namespace stmg
