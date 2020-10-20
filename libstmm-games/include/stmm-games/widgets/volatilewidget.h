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
 * File:   volatilewidget.h
 */

#ifndef STMG_VOLATILE_WIDGET_H
#define STMG_VOLATILE_WIDGET_H

#include "containerwidget.h"
#include "util/intset.h"
#include "ownertype.h"

#include <memory>
#include <vector>
#include <array>
#include <utility>

#include <stdint.h>

namespace stmg { class GameWidget; }
namespace stmg { class Variable; }

namespace stmg
{

using std::shared_ptr;

/** Volatile widget.
 * A container that changes appearance depending on a variable. It holds a child widget.
 */
class VolatileWidget : public ContainerWidget
{
public:
	struct ValueImg
	{
		IntSet m_oValues; /**< The variable values for which m_nImgId is drawn. Must not be empty. */
		int32_t m_nImgId = -1; /**< The image to be drawn as background or -1 if nothing should be drawn.
								 * Index into Named::images(). Default is -1. */
	};
	struct LocalInit
	{
		int32_t m_nVarId = -1; /**< The variable id. Must be `&gt;= 0` and exist for the widget's m_eVarOwnerType. Default: `-1`. */
		OwnerType m_eVarOwnerType = OwnerType::GAME; /**< The variable owner type. Team and mate must be defined accordingly.  Default is OwnerType::GAME.*/
		int32_t m_nDefaultImgId = -1; /**< The default image to be drawn as background or -1 if nothing should be drawn.
										 * Only applies if none of the images in m_aValueImgs apply.
										 * Index into Named::images(). Default is -1. */
		std::vector< ValueImg > m_aValueImgs; /**< The value images. Cannot be empty. */
	};
	struct Init : public ContainerWidget::Init, public LocalInit
	{
	};
	/** The current image id according to the variable's value.
	 * @return The image id or -1 if no image is selected by variable.
	 */
	int32_t getCurrentImgId() const noexcept;
	/** The child widget.
	 * @return The widget. Is not null.
	 */
	inline const shared_ptr<GameWidget>& getChildWidget() const noexcept { return getChildren()[0]; }

	/** Whether the variable's value has changed.
	 * If reInit() is called the change is not detected.
	 */
	bool isChanged() const noexcept;

	/** Constructor.
	 * The single child cannot be null and cannot be added to multiple containers.
	 * @param oInit The initialization data.
	 */
	explicit VolatileWidget(Init&& oInit) noexcept
	: ContainerWidget(std::move(oInit))
	, m_oData(std::move(oInit))
	{
		reInitCommon();
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
		reInitCommon();
	}
	void onAddedToGame() noexcept override;
private:
	void reInitCommon() noexcept;
	const Variable& variable() const noexcept;
	bool isChanged(int32_t& nNewCachedValue) const noexcept;
	int32_t calcImgIdxFromValue(int32_t nValue) const noexcept;
	int32_t calcImgIdFromIdx(int32_t nImgIdx) const noexcept;
private:
	LocalInit m_oData;
	Variable* m_p0Variable;
	struct CachedValue
	{
		int32_t m_nGameTick = -1;
		int32_t m_nValue;
		int32_t m_nIdx;
		int32_t m_nImgId;
	};
	mutable std::array<CachedValue, 2> m_aCachedValues;
	mutable int32_t m_nOlderCachedValue;
};

} // namespace stmg

#endif	/* STMG_VOLATILE_WIDGET_H */

