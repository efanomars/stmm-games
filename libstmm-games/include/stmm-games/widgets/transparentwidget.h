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
 * File:   transparentwidget.h
 */

#ifndef STMG_TRANSPARENT_WIDGET_H
#define STMG_TRANSPARENT_WIDGET_H

#include "relsizedgamewidget.h"

#include <utility>

#include <stdint.h>

namespace stmg
{

/** Transparent widget. Used for points or time values.
 */
class TransparentWidget : public RelSizedGameWidget
{
public:
	struct LocalInit
	{
	};
	struct Init : public RelSizedGameWidget::Init, public LocalInit
	{
	};

	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit TransparentWidget(Init&& oInit) noexcept
	: RelSizedGameWidget(std::move(oInit))
	{
	}
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	/** Reinitialization.
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept
	{
		RelSizedGameWidget::reInit(std::move(oInit));
	}
};

} // namespace stmg

#endif	/* STMG_TRANSPARENT_WIDGET_H */

