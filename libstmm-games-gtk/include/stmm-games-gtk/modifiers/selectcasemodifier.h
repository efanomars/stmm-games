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
 * File:   selectcasemodifier.h
 */

#ifndef STMG_SELECT_CASE_MODIFIER_H
#define STMG_SELECT_CASE_MODIFIER_H

#include "containermodifier.h"

#include "stdthememodifier.h"

#include <memory>
#include <vector>

#include <stdint.h>

namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

class StdTheme;

/** Tile to image assignment selection.
 */
class SelectCaseModifier : public ContainerModifier //StdThemeModifier, protected MultiContainerModifier
{
public:
	/** Constructor.
	 * @param p1Owner The owner theme. Cannot be null.
	 */
	explicit SelectCaseModifier(StdTheme* p1Owner) noexcept;
	/** Add a case to the select.
	 * @param nIdAss The assignment. Must be valid within the theme.
	 * @param aModifiers The modifiers to be painted if drawn tile selects an image of the assignment. Can be empty.
	 */
	void addCase(int32_t nIdAss, std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept;
	/** Set the default case.
	 * Can only be called once.
	 * @param aModifiers The modifiers to be painted if all cases are skipped. Can be empty.
	 */
	void setDefault(std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
private:
	std::vector< int32_t > m_aAssign;
	std::vector< unique_ptr<ContainerModifier> > m_aAssignContainers; // Size: m_aAssign.size() + 1
private:
	SelectCaseModifier() = delete;
	SelectCaseModifier(const SelectCaseModifier& oSource) = delete;
	SelectCaseModifier& operator=(const SelectCaseModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_SELECT_CASE_MODIFIER_H */

