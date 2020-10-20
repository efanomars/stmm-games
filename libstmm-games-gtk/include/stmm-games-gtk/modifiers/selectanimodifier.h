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
 * File:   selectanimodifier.h
 */

#ifndef STMG_SELECT_ANI_MODIFIER_H
#define STMG_SELECT_ANI_MODIFIER_H

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

/** Tile animation value selector.
 * Selects which sub modifiers to paint depending on the value of a tile animation.
 * The value is a floating point number from 0.0 to 1.0 or -1.0 if not defined (not animated).
 */
class SelectAniModifier : public ContainerModifier
{
public:
	/** Constructor.
	 * @param p1Owner The owner theme. Cannot be null.
	 * @param nTileAniId The tile ani idx. Must be valid in p1Owner->getNamed().tileAnis().
	 */
	explicit SelectAniModifier(StdTheme* p1Owner, int32_t nTileAniId) noexcept;
	/** Add a case to the tile animation selection.
	 * The interval cannot be impossible.
	 * @param fFrom The min value. Should be a value between -1.0 and 1.0.
	 * @param bFromExcl Whether the tile animation value must be &gt;= (false) or &gt; (true) fFrom.
	 * @param fTo The max value. Should be a value between fFrom and 1.0.
	 * @param bToExcl Whether the tile animation value must be &lt;= (false) or &lt; (true) fTo.
	 * @param aModifiers The modifiers painted if the condition is satisfied.
	 */
	void addCase(double fFrom, bool bFromExcl, double fTo, bool bToExcl
				, std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept;
	/** Set the default case.
	 * Can only be called once.
	 * @param aModifiers The modifiers to be painted if all cases are skipped. Can be empty.
	 */
	void setDefault(std::vector< unique_ptr<StdThemeModifier> >&& aModifiers) noexcept;

	FLOW_CONTROL drawTile(const Cairo::RefPtr<Cairo::Context>& refCc, StdThemeDrawingContext& oDc
						, const Tile& oTile, int32_t nPlayer, const std::vector<double>& aAniElapsed) noexcept override;
private:
	int32_t m_nTileAniId;
	struct CaseFromTo
	{
		double m_fFrom;
		bool m_bFromExcl;
		double m_fTo;
		bool m_bToExcl;
	};
	std::vector< CaseFromTo > m_aRanges;
	std::vector< unique_ptr<ContainerModifier> > m_aRangeContainers; // Size: m_aRanges.size() + 1
private:
	SelectAniModifier() = delete;
	SelectAniModifier(const SelectAniModifier& oSource) = delete;
	SelectAniModifier& operator=(const SelectAniModifier& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_SELECT_ANI_MODIFIER_H */

