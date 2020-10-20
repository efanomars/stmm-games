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
 * File:   varwidget.h
 */

#ifndef STMG_VAR_WIDGET_H
#define STMG_VAR_WIDGET_H

#include "relsizedgamewidget.h"

#include "gamewidget.h"
#include "ownertype.h"

#include <array>
#include <string>
#include <utility>

#include <stdint.h>

namespace stmg { class Variable; }

namespace stmg
{

/** Variable widget. Used for representing a variable such as points or time.
 */
class VarWidget : public RelSizedGameWidget
{
public:
	struct LocalInit
	{
		std::string m_sTitle; /**< The title that describes the variable. If empty no title i shown and m_bTitleBesideValue, m_nTitleFactorPerc, m_eTitleAlign are ignored. */
		int32_t m_nTitleFactorPerc = 30; /**< The percentage of widget space for the title. Must be &gt;= 0 and &lt;= 100. Default: `30`. */
		bool m_bTitleBesideValue = false; /**< Whether title is left or right of the value or above or below. Default: false. */
		int32_t m_nVarId = -1; /**< The variable id. Must be `&gt;= 0` and exist for the widget's m_eVarOwnerType. Default: `-1`. */
		OwnerType m_eVarOwnerType = OwnerType::GAME; /**< The variable owner type. Team and mate must be defined accordingly.  Default is OwnerType::GAME.*/
		int32_t m_nVarDigits = 5; /**< The maximum number of digits of values. Used to calc a stable font size. Default: `5`. */
		std::array<WidgetAxisFit::ALIGN, g_nTotLayoutConfigs> m_aTitleAlign = {WidgetAxisFit::ALIGN_FILL, WidgetAxisFit::ALIGN_FILL};
					/**< The alignment of the title for each layout config. If ALIGN_FILL is used chosen by view. Default: WidgetAxisFit::ALIGN_FILL. */
		std::array<WidgetAxisFit::ALIGN, g_nTotLayoutConfigs> m_aValueAlign = {WidgetAxisFit::ALIGN_FILL, WidgetAxisFit::ALIGN_FILL};
					/**< The alignment of the value for each layout config. If ALIGN_FILL is used chosen by view. Default: WidgetAxisFit::ALIGN_FILL. */
		//
		void setTitleAlign(const WidgetAxisFit::ALIGN& eTitleAlign) noexcept
		{
			for (int32_t nLC = 0; nLC < g_nTotLayoutConfigs; ++nLC) {
				m_aTitleAlign[nLC] = eTitleAlign;
			}
		}
		void setValueAlign(const WidgetAxisFit::ALIGN& eValueAlign) noexcept
		{
			for (int32_t nLC = 0; nLC < g_nTotLayoutConfigs; ++nLC) {
				m_aValueAlign[nLC] = eValueAlign;
			}
		}
	};
	struct Init : public RelSizedGameWidget::Init, public LocalInit
	{
	};
	/** The title shown with the variable.
	 * @return The title. Can be empty.
	 */
	inline const std::string& getTitle() const noexcept { return m_oData.m_sTitle; }
	/** The percent of space of the widget the title should occupy.
	 * @return The percent. Must be `&gt;=0` and `&lt;=100`.
	 */
	inline int32_t getTitleFactorPerc() const noexcept { return m_oData.m_nTitleFactorPerc; }
	/** Whether the title and value are boxed horizontally or vertically.
	 * @return Whether title is beside value (boxed horizontally).
	 */
	inline bool getTitleBesideValue() const noexcept { return m_oData.m_bTitleBesideValue; }
	/** Guideline maximum size in digits the variable value can take.
	 *
	 * Example: If the variable is expected to vary from '-500' to '500' this
	 * function should return '4' (= sizeof('-500')).
	 * @return The maximum number of digits the variable can take.
	 */
	inline int32_t getValueDigits() const noexcept { return m_oData.m_nVarDigits; }
	/** How the title should be aligned by the view.
	 * If WidgetAxisFit::ALIGN_FILL is returned the view should decide the alignment.
	 * @param nLayoutConfig The layout config.
	 * @return The title alignment.
	 */
	WidgetAxisFit::ALIGN getTitleAlign(int32_t nLayoutConfig) const noexcept { return m_oData.m_aTitleAlign[nLayoutConfig]; }
	/** How the variable value should be aligned by the view.
	 * If WidgetAxisFit::ALIGN_FILL is returned the view should decide the alignment.
	 * @param nLayoutConfig The layout config.
	 * @return The variable  value alignment.
	 */
	WidgetAxisFit::ALIGN getValueAlign(int32_t nLayoutConfig) const noexcept { return m_oData.m_aValueAlign[nLayoutConfig]; }
	/** The variable shown by the widget.
	 * @return The variable.
	 */
	const Variable& variable() const noexcept;
	/** Whether the variable's value has changed.
	 * If reInit() is called the change is not detected.
	 */
	bool isChanged() const noexcept;

	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit VarWidget(Init&& oInit) noexcept
	: RelSizedGameWidget(std::move(oInit))
	, m_oData(std::move(oInit))
	{
		checkParams();
//std::cout << "  ++++m_oData.m_nVarDigits=" << m_oData.m_nVarDigits << "  this=" << reinterpret_cast<int64_t>(this) << '\n';
	}
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	/** Reinitialization.
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept
	{
		RelSizedGameWidget::reInit(std::move(oInit));
		m_oData = std::move(oInit);
		checkParams();
	}
private:
	void checkParams() noexcept;
private:
	LocalInit m_oData;
};

} // namespace stmg

#endif	/* STMG_VAR_WIDGET_H */

