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
 * File:   selectcasemodifier.cc
 */

#include "modifiers/selectcasemodifier.h"
#include "stdtheme.h"

#include "stdthemedrawingcontext.h"

#include <cassert>
#include <cstdint>
#include <utility>

namespace stmg { class Image; }
namespace stmg { class Tile; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

SelectCaseModifier::SelectCaseModifier(StdTheme* p1Owner) noexcept
: ContainerModifier(p1Owner)
{
	// The else group is at position 0
	m_aAssignContainers.push_back(std::make_unique<ContainerModifier>(p1Owner));
}
void SelectCaseModifier::addCase(int32_t nIdAss, std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept
{
//std::cout << "SelectCaseModifier::addCase nIdAss=" << nIdAss << "  aModifiers.size()=" << aModifiers.size() << '\n';
	assert((nIdAss >= 0) && owner()->hasAssignId(nIdAss));
	m_aAssign.push_back(nIdAss);
	m_aAssignContainers.push_back(std::make_unique<ContainerModifier>(owner()));
	m_aAssignContainers.back()->addSubModifiers(std::move(aModifiers));
}
void SelectCaseModifier::setDefault(std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept
{
//std::cout << "SelectCaseModifier::setDefault  aModifiers.size()=" << aModifiers.size() << '\n';
	assert(m_aAssignContainers[0]->getSubModifiers().size() == 0); // Can only be set once
	m_aAssignContainers[0]->addSubModifiers(std::move(aModifiers));
}
StdThemeModifier::FLOW_CONTROL SelectCaseModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
													, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept
{
	const int32_t nTotCases = static_cast<int32_t>(m_aAssign.size());
	for (int32_t nCase = 0; nCase < nTotCases; ++nCase) {
//std::cout << "            SelectCaseModifier::drawTile   nCondition=" << nCondition << '\n';
		const int32_t nIdAss = m_aAssign[nCase];
		shared_ptr<Image> refImage = owner()->getAssignImage(nIdAss, oTile, nPlayer);
		if (refImage) {
//std::cout << "            SelectCaseModifier::drawTile   nIdAss=" << nIdAss << "  nPlayer=" << nPlayer << '\n';
			Image* p0SaveImage = oDc.getSelectedImage();
			oDc.setSelectedImage(refImage.get());
			const FLOW_CONTROL eCtl = m_aAssignContainers[nCase + 1]->drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
			oDc.setSelectedImage(p0SaveImage);
			return eCtl; //-----------------------------------------------------
		}
	}
//std::cout << "            SelectCaseModifier::drawTile   DEFAULT" << '\n';
	// Default
	return m_aAssignContainers[0]->drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
}

} // namespace stmg
