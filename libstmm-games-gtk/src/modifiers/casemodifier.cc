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
 * File:   casemodifier.cc
 */

#include "modifiers/casemodifier.h"
#include "stdtheme.h"
#include "stdthemedrawingcontext.h"

#include <cassert>
#include <memory>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg { class Image; }
namespace stmg { class Tile; }

namespace stmg
{

CaseModifier::CaseModifier(StdTheme* p1Owner, int32_t nIdAss) noexcept
: ContainerModifier(p1Owner)
, m_nAssId(nIdAss)
{
//std::cout << "CaseModifier::CaseModifier   m_nAssId=" << m_nAssId << "  =" << owner()->getAssignId(m_nAssId) << '\n';
	assert((nIdAss >= 0) && owner()->hasAssignId(nIdAss));
}
StdThemeModifier::FLOW_CONTROL CaseModifier::drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
												, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept
{
	FLOW_CONTROL eCtl = FLOW_CONTROL_CONTINUE;

//std::cout << "            CaseModifier::drawTile   m_nAssId=" << m_nAssId << "  nPlayer=" << nPlayer << '\n';
	shared_ptr<Image> refImage = owner()->getAssignImage(m_nAssId, oTile, nPlayer);
	if (refImage) {
//std::cout << "            CaseModifier::drawTile   refImage found" << '\n';
		Image* p0SaveImage = oDc.getSelectedImage();
		oDc.setSelectedImage(refImage.get());
		eCtl = ContainerModifier::drawTile(refCc, oDc, oTile, nPlayer, aAniElapsed);
		oDc.setSelectedImage(p0SaveImage);
	}
	return eCtl;
}

} // namespace stmg
