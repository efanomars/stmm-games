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
 * File:   ifelsemodifier.h
 */

#ifndef STMG_IF_ELSE_MODIFIER_H
#define STMG_IF_ELSE_MODIFIER_H

#include "containermodifier.h"

#include "stdthememodifier.h"

#include <stmm-games/utile/tileselector.h>

#include <vector>
#include <memory>

#include <stdint.h>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace stmg { class StdThemeDrawingContext; }
namespace stmg { class Tile; }

namespace stmg
{

class StdTheme;

/** If ElseIf Else tile selection conditions.
 */
class IfElseModifier : public ContainerModifier
{
public:
	/** Constructor.
	 * @param p1Owner The owner theme. Cannot be null.
	 */
	explicit IfElseModifier(StdTheme* p1Owner) noexcept;
	/** Add a tile selection condition.
	 * If the tile selector matches the current tile and player aModifiers a
	 * @param refSelect The tile selector. Cannot be null.
	 * @param aModifiers The sub modifiers to be drawn if the selection matches.
	 */
	void addCondition(unique_ptr<TileSelector> refSelect, std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept;
	/** The modifiers to be drawn if none of the conditions applies.
	 * @param aModifiers The sub modifiers.
	 */
	void setElse(std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
private:
	std::vector< unique_ptr<TileSelector> > m_aCondition;
	std::vector< unique_ptr<ContainerModifier> > m_aConditionContainers; // Size: m_aCondition.size() + 1
private:
	IfElseModifier() = delete;
	IfElseModifier(const IfElseModifier& oSource) = delete;
	IfElseModifier& operator=(const IfElseModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_IF_ELSE_MODIFIER_H */

