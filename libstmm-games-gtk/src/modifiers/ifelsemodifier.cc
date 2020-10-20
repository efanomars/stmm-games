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
 * File:   ifelsemodifier.cc
 */

#include "modifiers/ifelsemodifier.h"

#include <stmm-games/utile/tileselector.h>

#include <cassert>
#include <utility>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }

namespace stmg
{

IfElseModifier::IfElseModifier(StdTheme* p1Owner) noexcept
: ContainerModifier(p1Owner)
{
	// The else group is at position 0
	m_aConditionContainers.push_back(std::make_unique<ContainerModifier>(p1Owner));
}
void IfElseModifier::addCondition(unique_ptr<TileSelector> refSelect, std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept
{
//std::cout << "IfElseModifier::addCondition aModifiers.size()=" << aModifiers.size() << '\n';
	m_aCondition.push_back(std::move(refSelect));
	m_aConditionContainers.push_back(std::make_unique<ContainerModifier>(owner()));
	m_aConditionContainers.back()->addSubModifiers(std::move(aModifiers));
}
void IfElseModifier::setElse(std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept
{
//std::cout << "IfElseModifier::setElse aModifiers.size()=" << aModifiers.size() << '\n';
	assert(m_aConditionContainers[0]->getSubModifiers().size() == 0); // Can only be set once
	m_aConditionContainers[0]->addSubModifiers(std::move(aModifiers));
}
StdThemeModifier::FLOW_CONTROL IfElseModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
												, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept
{
	int32_t nCondition = 0;
	const int32_t nTotConditions = m_aCondition.size();
	for ( ; nCondition < nTotConditions; ++nCondition) {
//std::cout << "            IfElseModifier::drawTile   nCondition=" << nCondition << '\n';
		const auto& refSelect = m_aCondition[nCondition];
		if (refSelect->select(oTile, nPlayer)) {
			return m_aConditionContainers[nCondition + 1]->drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
		}
	}
//std::cout << "            IfElseModifier::drawTile   ELSE" << '\n';
	// Else
	return m_aConditionContainers[0]->drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
}

} // namespace stmg
