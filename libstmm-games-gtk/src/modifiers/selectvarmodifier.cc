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
 * File:   selectvarmodifier.cc
 */

#include "modifiers/selectvarmodifier.h"
#include "stdtheme.h"

#include <cassert>
#include <cstdint>
#include <utility>

namespace stmg { class Tile; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

SelectVarModifier::SelectVarModifier(StdTheme* p1Owner, const std::string& sVariableName) noexcept
: ContainerModifier(p1Owner)
, m_nVariableId(p1Owner->getVariableIndex(sVariableName))
{
	assert(! sVariableName.empty());
	// The else group is at position 0
	m_aRangeContainers.push_back(std::make_unique<ContainerModifier>(p1Owner));
}
void SelectVarModifier::addCase(int32_t nFrom, int32_t nTo
								, std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept
{
//std::cout << "SelectVarModifier::addCase nIdAss=" << nIdAss << "  aModifiers.size()=" << aModifiers.size() << '\n';
	CaseFromTo oCaseFromTo;
	oCaseFromTo.m_nFrom = nFrom;
	oCaseFromTo.m_nTo = nTo;
	m_aRanges.push_back(std::move(oCaseFromTo));
	m_aRangeContainers.push_back(std::make_unique<ContainerModifier>(owner()));
	m_aRangeContainers.back()->addSubModifiers(std::move(aModifiers));
}
void SelectVarModifier::setDefault(std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept
{
//std::cout << "SelectVarModifier::setDefault  aModifiers.size()=" << aModifiers.size() << '\n';
	assert(m_aRangeContainers[0]->getSubModifiers().size() == 0); // Can only be set once
	m_aRangeContainers[0]->addSubModifiers(std::move(aModifiers));
}
StdThemeModifier::FLOW_CONTROL SelectVarModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
													, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept
{
//std::cout << "            SelectVarModifier::drawTile()" << '\n';
	const auto oPair = oDc.getVariableValue(m_nVariableId);
	const bool bVarUndefined = ! oPair.first;
	const int32_t nVarValue = oPair.second;

	const int32_t nTotCases = static_cast<int32_t>(m_aRanges.size());
	for (int32_t nCase = 0; nCase < nTotCases; ++nCase) {
//std::cout << "                             ::drawTile   nVarValue=" << nVarValue << "  bVarUndefined=" << bVarUndefined << '\n';
		const CaseFromTo& oCaseFromTo = m_aRanges[nCase];
//std::cout << "                                          oCaseFromTo.m_nFrom=" << oCaseFromTo.m_nFrom << "  oCaseFromTo.m_nTo=" << oCaseFromTo.m_nTo << '\n';
		if (oCaseFromTo.m_nFrom > oCaseFromTo.m_nTo) {
			if (! bVarUndefined) {
				continue;
			}
		} else {
			if ((nVarValue < oCaseFromTo.m_nFrom) || (nVarValue > oCaseFromTo.m_nTo)) {
				continue;
			}
		}
		const FLOW_CONTROL eCtl = m_aRangeContainers[nCase + 1]->drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
		return eCtl; //-----------------------------------------------------
	}
//std::cout << "            SelectVarModifier::drawTile   DEFAULT" << '\n';
	// Default
	return m_aRangeContainers[0]->drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
}

} // namespace stmg
