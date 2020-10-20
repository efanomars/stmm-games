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
 * File:   relsizedgamewidget.h
 */

#ifndef STMG_REL_SIZED_GAME_WIDGET_H
#define STMG_REL_SIZED_GAME_WIDGET_H

#include "gamewidget.h"

//#include <iostream>
#include <utility>

#include <stdint.h>

namespace stmg
{

/** Widget sized relative to a reference widget of the same layout.
 */
class RelSizedGameWidget : public GameWidget
{
public:
	/** The data to initialize a RelSizedGameWidget.
	 * Of the values m_fReferenceWFactor, m_fReferenceHFactor, m_fWHRatio two have
	 * to be defined (&lt; 0), one should have a negative value (which means not defined).
	 */
	struct LocalInit
	{
		double m_fReferenceWFactor = -1.0; /**< The width compared to the reference widget's width. Default: `-1.0` (not set).*/
		double m_fReferenceHFactor = -1.0; /**< The height compared to the reference widget's height. Default: `-1.0` (not set). */
		double m_fWHRatio = -1.0; /**< The width to height ratio. Default: `-1.0` (not set).*/
	};
	struct Init : public GameWidget::Init, public LocalInit
	{
	};

	/** The width size factor to the reference widget.
	 * Example: if it is `0.5` this widget will have (if not expanded) about half
	 * the width of the reference widget.
	 * @return The reference width factor or `-1.0` if not defined.
	 */
	inline double getReferenceWFactor() const noexcept { return m_oData.m_fReferenceWFactor; }
	/** The height size factor to the reference widget.
	 * @return The reference height factor or `-1.0` if not defined.
	 */
	inline double getReferenceHFactor() const noexcept { return m_oData.m_fReferenceHFactor; }
	/** The width to height ratio.
	 * If this value is `&gt; 0.0` then either width or height factor are undefined.
	 * @return The width to height ratio or `-1.0` if not defined.
	 */
	inline double getWHRatio() const noexcept { return m_oData.m_fWHRatio; }

	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit RelSizedGameWidget(Init&& oInit) noexcept
	: GameWidget(std::move(oInit))
	, m_oData(std::move(oInit))
	{
	}
	/** Reinitialization.
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept
	{
		GameWidget::reInit(std::move(oInit));
		m_oData = std::move(oInit);
	}

private:
	LocalInit m_oData;
private:
	RelSizedGameWidget() = delete;
};

} // namespace stmg

#endif	/* STMG_REL_SIZED_GAME_WIDGET_H */

