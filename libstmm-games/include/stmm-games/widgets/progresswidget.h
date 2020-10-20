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
 * File:   progresswidget.h
 */

#ifndef STMG_PROGRESS_WIDGET_H
#define STMG_PROGRESS_WIDGET_H

#include "relsizedgamewidget.h"

#include "util/intset.h"
#include "gameproxy.h"

#include <cassert>
#include <utility>

#include <stdint.h>

namespace stmg { class Variable; }

namespace stmg
{

/** Progress widget.
 * Used for representing a variable as a progress bar.
 * The bar can have two colors, normal and danger depending
 * on the value of the threshold.
 *
 * The bar can also have marks.
 */
class ProgressWidget : public RelSizedGameWidget
{
public:
	struct LocalInit
	{
		int32_t m_nMinValue = 0; /**< The "empty" value. Default: 0. */
		int32_t m_nMaxValue = 100; /**< The "full" value. Must be &gt;= m_nMinValue.  Default: 100. */
		int32_t m_nThresholdValue = 101; /**< The value &gt;= m_nMinValue and &lt;= m_nMaxValue for which
										 * the color of the progress changes. If ouside ther is no
										 * color change (no danger color). Default: 101. */
		bool m_bDangerBelow = false; /**< Whether the danger color is &lt;= m_nThresholdValue. Default: false. */
		IntSet m_oMarks; /**< At what values the view should show marks.
							 * All values must be &gt;= m_nMinValue and &lt;= m_nMaxValue. */
		int32_t m_nVariableId = -1; /**< The variable id holding the value.
									 * Must be `&gt;= 0` and exist for the widget's owner. Default: `-1`. */
	};
	struct Init : public RelSizedGameWidget::Init, public LocalInit
	{
	};
	/** The minimum value.
	 * @return The minimum value.
	 */
	inline int32_t getMinValue() const noexcept { return m_oData.m_nMinValue; }
	/** The maximum value.
	 * @return The maximum value.
	 */
	inline int32_t getMaxValue() const noexcept { return m_oData.m_nMaxValue; }
	/** The threshold value.
	 * @return The threshold value.
	 */
	inline int32_t getThresholdValue() const noexcept { return m_oData.m_nThresholdValue; }
	/** Whether the danger color is below the thresold.
	 * @return Whether danger.
	 */
	inline bool getDangerBelow() const noexcept { return m_oData.m_bDangerBelow; }
	/** The marks.
	 * @return The main marks.
	 */
	inline const IntSet& getMarks() const noexcept { return m_oData.m_oMarks; }
	/** The variable shown by the widget.
	 * @return The variable.
	 */
	inline const Variable& variable() const noexcept
	{
		const bool bATIOL = game().isAllTeamsInOneLevel();
		const int32_t nTeam = getTeam();
		const int32_t nLevel = (bATIOL ? 0 : nTeam);
		const int32_t nLevelTeam = (bATIOL ? nTeam : ((nLevel < 0) ? -1 : 0));
		return game().variable(m_oData.m_nVariableId, nLevel, nLevelTeam, getMate());
	}
	/** Whether the variable's value has changed.
	 * If reInit() is called the change is not detected.
	 */
	bool isChanged() const noexcept;

	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit ProgressWidget(Init&& oInit) noexcept
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
	void checkParams() noexcept
	{
		assert(m_oData.m_nVariableId >= 0);
		assert(m_oData.m_nMinValue <= m_oData.m_nMaxValue);
		#ifndef NDEBUG
		const int32_t nTotMarks = m_oData.m_oMarks.size();
		for (int32_t nIdx = 0; nIdx < nTotMarks; ++ nIdx) {
			const int32_t nMark = m_oData.m_oMarks.getValueByIndex(nIdx);
			assert(nMark >= m_oData.m_nMinValue);
			assert(nMark <= m_oData.m_nMaxValue);
		}
		#endif //NDEBUG
	}
private:
	LocalInit m_oData;
};

} // namespace stmg

#endif	/* STMG_PROGRESS_WIDGET_H */

