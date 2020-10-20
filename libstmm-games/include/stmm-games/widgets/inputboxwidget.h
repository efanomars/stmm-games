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
 * File:   inputboxwidget.h
 */

#ifndef STMG_INPUT_BOX_WIDGET_H
#define STMG_INPUT_BOX_WIDGET_H

#include "widgets/boxwidget.h"

#include <string>
#include <cassert>
#include <utility>

#include <stdint.h>

namespace stmg
{

class InputBoxWidget : public BoxWidget
{
public:
	struct LocalInit
	{
		std::string m_sTargetWidgetName; /**< The name of the target widget. Cannot be empty. */
	};
	struct Init : public BoxWidget::Init, public LocalInit
	{
	};
	/** Constructor.
	 * The target widget with given name must exist in the layout when the layout is created
	 * and must not be a ContainerWidget (subclass).
	 *
	 * The team and mate of the target widget must be the same as this widget.
	 * @param oInit The initialization data.
	 */
	explicit InputBoxWidget(Init&& oInit) noexcept
	: BoxWidget(std::move(oInit))
	, m_oData(std::move(oInit))
	{
		assert(!m_oData.m_sTargetWidgetName.empty());
	}

	/** The name of the target widget.
	 * @return The name of the target widget. Cannot be empty.
	 */
	inline const std::string& getTargetWidgetName() const noexcept { return m_oData.m_sTargetWidgetName; }

	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	/** Reinitialization.
	 * See constructor.
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept
	{
		BoxWidget::reInit(std::move(oInit));
		m_oData = std::move(oInit);
		assert(!m_oData.m_sTargetWidgetName.empty());
	}

	void onAddedToLayout() noexcept override;
private:
	LocalInit m_oData;
private:
	InputBoxWidget() = delete;
};

} // namespace stmg

#endif	/* STMG_INPUT_BOX_WIDGET_H */

