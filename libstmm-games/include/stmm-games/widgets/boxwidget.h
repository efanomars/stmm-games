/*
 * File:  boxwidget.h
 *
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
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

#ifndef STMG_BOX_WIDGET_H
#define STMG_BOX_WIDGET_H

#include "containerwidget.h"

#include "gamewidget.h"

#include <memory>
#include <cassert>
#include <array>
#include <utility>
#include <vector>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

class BoxWidget : public ContainerWidget
{
public:
	struct LocalInit
	{
		std::array<bool, g_nTotLayoutConfigs> m_aVertical = {{false, false}}; /**< Whether the children are stacked vertically or horizontally. Default: false. */
		std::array<bool, g_nTotLayoutConfigs> m_aInvertChildren = {{false, false}}; /**< Whether the order of the children should be inverted. Default: false. */
	};
	struct Init : public ContainerWidget::Init, public LocalInit
	{
		void setVertical(bool bVertical) noexcept
		{
			for (int32_t nLC = 0; nLC < g_nTotLayoutConfigs; ++nLC) {
				m_aVertical[nLC] = bVertical;
			}
		}
		void setInvertChildren(bool bInvertChildren) noexcept
		{
			for (int32_t nLC = 0; nLC < g_nTotLayoutConfigs; ++nLC) {
				m_aInvertChildren[nLC] = bInvertChildren;
			}
		}
		void setSwitchable(bool bVertical, bool bInvertIfSwitched, bool bFill) noexcept
		{
			static_assert(g_nTotLayoutConfigs >= 2, "");
			m_aVertical[0] = bVertical;
			m_aVertical[1] = !bVertical;
			m_aInvertChildren[0] = false;
			m_aInvertChildren[1] = bInvertIfSwitched;
			for (int32_t nLC = 1; nLC < g_nTotLayoutConfigs; ++nLC) {
				m_aHorizAxisFit[nLC].m_bExpand = bFill;
				m_aHorizAxisFit[nLC].m_eAlign = (bFill ? WidgetAxisFit::ALIGN_FILL : WidgetAxisFit::ALIGN_CENTER);
				m_aVertAxisFit[nLC].m_bExpand = bFill;
				m_aVertAxisFit[nLC].m_eAlign = (bFill ? WidgetAxisFit::ALIGN_FILL : WidgetAxisFit::ALIGN_START);
			}
		}
	};
	/** Whether the children are stacked vertically.
	 * @param nLayoutConfig The layout config.
	 * @return Whether the box is vertical.
	 */
	inline bool isVertical(int32_t nLayoutConfig) const noexcept
	{
		assert((nLayoutConfig >= 0) && (nLayoutConfig < g_nTotLayoutConfigs));
		return m_oData.m_aVertical[nLayoutConfig];
	}
	/** Whether the children should be boxed in reverse order for the layout config.
	 * @param nLayoutConfig The layout config.
	 * @return Whether the children passed with m_aChildWidgets are inverted.
	 */
	inline bool hasInvertedChildren(int32_t nLayoutConfig) const noexcept
	{
		assert((nLayoutConfig >= 0) && (nLayoutConfig < g_nTotLayoutConfigs));
		return m_oData.m_aInvertChildren[nLayoutConfig];
	}

	/** Constructor.
	 * There must be at least one non null child.
	 * Children cannot be repeated, added twice or added to multiple containers.
	 * @param oInit The initialization data.
	 */
	explicit BoxWidget(Init&& oInit) noexcept
	: ContainerWidget(std::move(oInit))
	, m_oData(std::move(oInit))
	{
		assert(getChildren().size() > 0);
	}

	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	/** Reinitialization.
	 * There must be at least one non null child.
	 * Children cannot be repeated, added twice or added to multiple containers.
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept
	{
		ContainerWidget::reInit(std::move(oInit));
		m_oData = std::move(oInit);
		assert(getChildren().size() > 0);
	}
private:
	LocalInit m_oData;
private:
	BoxWidget() = delete;
};

} // namespace stmg

#endif	/* STMG_BOX_WIDGET_H */

