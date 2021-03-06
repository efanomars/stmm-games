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
 * File:   casemodifier.h
 */

#ifndef STMG_CASE_MODIFIER_H
#define STMG_CASE_MODIFIER_H

#include "containermodifier.h"

#include "stdthememodifier.h"

#include <vector>

#include <stdint.h>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }

namespace stmg
{

class StdTheme;

/** Single case shortcut of SelectCaseModifier.
 */
class CaseModifier : public ContainerModifier
{
public:
	/** Constructor.
	 * @param p1Owner The owner theme. Cannot be null.
	 * @param nIdAss The assignment. Must be valid within the theme.
	 */
	CaseModifier(StdTheme* p1Owner, int32_t nIdAss) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
private:
	int32_t m_nAssId;
private:
	CaseModifier() = delete;
	CaseModifier(const CaseModifier& oSource) = delete;
	CaseModifier& operator=(const CaseModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_CASE_MODIFIER_H */
