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
 * File:   themecontainerwidget.h
 */

#ifndef STMG_THEME_CONTAINER_WIDGET_H
#define STMG_THEME_CONTAINER_WIDGET_H

#include "themewidget.h"

#include <cassert>
#include <memory>
#include <vector>

#include <stdint.h>

namespace stmg { class GameWidget; }

namespace stmg
{

using std::shared_ptr;

/** ThemeWidget container class.
 * Implementations of this class should ensure that the children do not
 * overlap each other in the xy space because ThemeWidget::drawDeep() and
 * ThemeWidget::drawIfChanged() are only required to check and draw their direct
 * ancestors.
 */
class ThemeContainerWidget : public ThemeWidget
{
public:
	/** The child ThemeWidget vector of the container.
	 * The container may contain null values if the ThemeWidget couldn't be created
	 * for the corresponding GameWidget of the container's model.
	 * @return The children of the container.
	 */
	inline const std::vector< shared_ptr<ThemeWidget> >& getChildren() const noexcept
	{
		return m_aChlidren;
	}
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	/** Sets the children of the container.
	 * @param aChildren Non null children of the container.
	 */
	void setChildren(const std::vector< shared_ptr<ThemeWidget> >& aChildren) noexcept;
	/** Constructor.
	 * @param refModel The model. Cannot be null.
	 */
	explicit ThemeContainerWidget(const shared_ptr<GameWidget>& refModel) noexcept
	: ThemeWidget(refModel)
	{
		ThemeWidget::m_bIsContainer = true;
	}
	/** Reinitialization.
	 * See constructor.
	 */
	void reInit(const shared_ptr<GameWidget>& refModel) noexcept
	{
		ThemeWidget::reInit(refModel);
		ThemeWidget::m_bIsContainer = true;
	}
	virtual ~ThemeContainerWidget() noexcept
	{
		for (auto& refTW : m_aChlidren) {
			assert(refTW);
			refTW->m_p0Parent = nullptr;
		}
	}
private:
	friend class ThemeLayout;
	std::vector< shared_ptr<ThemeWidget> > m_aChlidren;
};


} // namespace stmg

#endif	/* STMG_THEME_CONTAINER_WIDGET_H */

