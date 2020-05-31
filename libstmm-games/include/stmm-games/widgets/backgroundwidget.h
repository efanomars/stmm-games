/*
 * File:  backgroundwidget.h
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

#ifndef STMG_BACKGROUND_WIDGET_H
#define STMG_BACKGROUND_WIDGET_H

#include "containerwidget.h"

#include <memory>
#include <cassert>
#include <utility>
#include <vector>

#include <stdint.h>

namespace stmg { class GameWidget; }

namespace stmg
{

using std::shared_ptr;

/** Background widget.
 * A container that holds a widget on top of a background image.
 */
class BackgroundWidget : public ContainerWidget
{
public:
	struct LocalInit
	{
		int32_t m_nImgId = -1; /**< The image id into Named.images(). Default: `-1`.*/
	};
	struct Init : public ContainerWidget::Init, public LocalInit
	{
	};
	/** The background image.
	 * @return The image. Is &gt;= 0.
	 */
	inline int32_t getImgId() const noexcept { return m_oData.m_nImgId; }

	/** The child widget.
	 * @return The widget. Cannot be null.
	 */
	inline const shared_ptr<GameWidget>& getChildWidget() const noexcept { return getChildren()[0]; }

	/** Constructor.
	 * The single child cannot be null and cannot be added to multiple containers.
	 * @param oInit The initialization data.
	 */
	explicit BackgroundWidget(Init&& oInit) noexcept
	: ContainerWidget(std::move(oInit))
	, m_oData(std::move(oInit))
	{
		assert(m_oData.m_nImgId >= 0);
		assert(getChildren().size() == 1);
	}
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	/** Reinitialization.
	 * The single child cannot be null and cannot be added to multiple containers.
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept
	{
		ContainerWidget::reInit(std::move(oInit));
		m_oData = std::move(oInit);
		assert(m_oData.m_nImgId >= 0);
		assert(getChildren().size() == 1);
	}
private:
	LocalInit m_oData;
};


} // namespace stmg

#endif	/* STMG_BACKGROUND_WIDGET_H */

