/*
 * File:  containerwidget.h
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

#ifndef STMG_CONTAINER_WIDGET_H
#define STMG_CONTAINER_WIDGET_H

#include "gamewidget.h"

#include <memory>
#include <vector>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

/** The base class for container widgets.
 */
class ContainerWidget : public GameWidget
{
public:
	/** The children widgets of the container.
	 * @return The children widgets. Cannot be null.
	 */
	const std::vector< shared_ptr<GameWidget> >& getChildren() const noexcept { return m_oData.m_aChildWidgets; }

	struct LocalInit
	{
		std::vector< shared_ptr<GameWidget> > m_aChildWidgets; /**< The child widgets. Can be empty. Cannot have repeated or null widgets. */
	};
	struct Init : public GameWidget::Init, public LocalInit
	{
	};

	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;

	virtual ~ContainerWidget() noexcept;
	/** Tells whether any of a vector of game widgets expands.
	 * @param aChildWidgets The widgets. Can be empty.
	 * @param nLayoutConfig The layout config.
	 * @param bHoriz Whether horizontal axis (or vertical).
	 * @return Whether any expands.
	 */
	static bool getChildExpand(const std::vector< shared_ptr<GameWidget> >& aChildWidgets, int32_t nLayoutConfig, bool bHoriz) noexcept;
	/** Tells whether any of a vector of game widgets fills.
	 * @param aChildWidgets The widgets. Can be empty.
	 * @param nLayoutConfig The layout config.
	 * @param bHoriz Whether horizontal axis (or vertical).
	 * @return Whether any fills.
	 */
	static bool getChildFill(const std::vector< shared_ptr<GameWidget> >& aChildWidgets, int32_t nLayoutConfig, bool bHoriz) noexcept;
protected:
	/** Constructor.
	 * @param oData Initialization data.
	 */
	explicit ContainerWidget(Init&& oData) noexcept;
	/** Reinitialization.
	 * @param oData The initialization data.
	 */
	void reInit(Init&& oData) noexcept;
private:
	void disownChildren() noexcept;
	void commonInit() noexcept;
private:
	LocalInit m_oData;
};

} // namespace stmg

#endif	/* STMG_CONTAINER_WIDGET_H */

