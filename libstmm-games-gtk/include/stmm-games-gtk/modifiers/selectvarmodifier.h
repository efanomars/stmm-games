/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   selectvarmodifier.h
 */

#ifndef STMG_SELECT_VAR_MODIFIER_H
#define STMG_SELECT_VAR_MODIFIER_H

#include "containermodifier.h"

#include "stdthememodifier.h"

#include <memory>
#include <vector>
#include <string>

#include <stdint.h>

namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

class StdTheme;

/** Game variable value selector.
 * Selects which sub modifiers to paint depending on the value of a game variable set at runtime.
 * The value is an integer. If the variable is not defined (by the game) that can be selected too.
 */
class SelectVarModifier : public ContainerModifier
{
public:
	/** Constructor.
	 * @param p1Owner The owner theme. Cannot be null.
	 * @param sVariableName The variable name.Cannot be empty.
	 */
	explicit SelectVarModifier(StdTheme* p1Owner, const std::string& sVariableName) noexcept;
	/** Add a case to the value selection.
	 * The interval cannot be impossible.
	 * @param nFrom The min value. If not &lt;= nTo, the case selects the undefined variable.
	 * @param nTo The max value.
	 * @param aModifiers The modifiers to be painted for the case. Can be empty.
	 */
	void addCase(int32_t nFrom, int32_t nTo, std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept;
	/** Set the default case.
	 * Can only be called once.
	 * @param aModifiers The modifiers to be painted if all cases are skipped. Can be empty.
	 */
	void setDefault(std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
private:
	int32_t m_nVariableId;
	struct CaseFromTo
	{
		int32_t m_nFrom;
		int32_t m_nTo;
	};
	std::vector< CaseFromTo > m_aRanges;
	std::vector< unique_ptr<ContainerModifier> > m_aRangeContainers; // Size: m_aRanges.size() + 1
private:
	SelectVarModifier() = delete;
	SelectVarModifier(const SelectVarModifier& oSource) = delete;
	SelectVarModifier& operator=(const SelectVarModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_SELECT_VAR_MODIFIER_H */

