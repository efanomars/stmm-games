/*
 * File:  actionsboxwidget.h
 *
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

#ifndef STMG_ACTIONS_BOX_WIDGET_H
#define STMG_ACTIONS_BOX_WIDGET_H

#include "boxwidget.h"
#include "widgets/actionwidget.h"

#include <string>
#include <vector>
#include <cassert>
#include <memory>
#include <utility>

#include <stdint.h>

namespace stmg { class Event; }
namespace stmi { class Event; }

namespace stmg
{

/** Action grid box.
 * This is a BoxWidget extension that allows to define an action grid.
 * An action can be assigned to each cell of the grid. An action can either be
 * a key action, a msg to send to an event or an interrupt. The action is triggered
 * when for example a pointer device clicks in the cell area.
 *
 * Note that the action grid is completely independent of the placement and size
 * of the box's children.
 *
 * LocalInit::m_aColumns contains positive numbers that partition the width
 * of the box widget. A column claims the portion of the box equivalent to
 * the value divided by the sum of all the values in the vector.
 * LocalInit::m_aRows contains positive numbers that partition the height
 * of the box widget. A row claims the portion of the box equivalent to
 * the value divided by the sum of all the values in the vector.
 * If a LocalInit::m_aActionCells value is empty, no key action is triggered.
 *
 * Example: for a Tetris like game the grid has 2 columns and 3 rows, 
 * LocalInit could be filled with m_aColumns = {7, 7}, m_aRows = {20, 140, 40},
 * m_aActionCells = {interrupt "PAUSE", interrupt "ABORT_ASK", key action "Drop",
 * key action "Rotate", key action "Left", key action "Right"}.
 * Meaning: the box rectangle is partitioned in 6 cells with the two columns
 * claiming 50% each and the rows claiming 10%, 70% and 20% respectively.
 */
class ActionsBoxWidget : public BoxWidget
{
public:
	enum ACTION_TYPE
	{
		ACTION_NONE = 0 /**< No action. */
		, ACTION_KEY_ACTION = 1 /**< Execute a key action. */
		, ACTION_INTERRUPT = 2 /**< Execute an interrupt. Ex: PAUSE. */
		, ACTION_EVENT_MSG = 3 /**< Trigger an event with a message. */
	};
	struct BoxCell
	{
		ACTION_TYPE m_eActionType = ACTION_NONE; /**< The action type. */
		int32_t m_nActionId = -1; /**< Depending on m_eActionType, the valid key action id, the INTERRUPT_TYPE value or event's message (must be &gt;= -1). */
		int32_t m_nValue = ActionWidget::LocalInit{}.m_nValue; /**< Only used if m_eActionType is ACTION_EVENT_MSG. The event's value. Default is 0. */
		std::string m_sActionName; /**< If m_eActionType is ACTION_EVENT_MSG the non empty event id. See Level::addEvent(const std::string& sId, unique_ptr<Event>). */
	private:
		friend class ActionsBoxWidget;
		Event* m_p0Event = nullptr;
	};
	struct LocalInit
	{
		std::vector<int32_t> m_aColumns; /**< Column weights in the action grid. If empty, one column (with weight value 1) is created. Must all be &gt;1. */
		std::vector<int32_t> m_aRows; /**< Row weights in the action grid. If empty, one row (with weight value 1) is created. Must all be &gt;1. */
		std::vector<BoxCell> m_aActionCells; /**< The cells (`nIndex = nRow * nTotCols + nCol`). If cells are missing ACTION_NONE is inserted. */
	};
	struct Init : public BoxWidget::Init, public LocalInit
	{
	};
	/** The grid's number of columns.
	 * @return The number of columns. Is &gt;= 1.
	 */
	inline int32_t getTotColumns() const noexcept { return static_cast<int32_t>(m_oData.m_aColumns.size()); }
	/** The grid's number of rows.
	 * @return The number of rows. Is &gt;= 1.
	 */
	inline int32_t getTotRows() const noexcept { return static_cast<int32_t>(m_oData.m_aRows.size()); }
	/** The sum of all column parts.
	 * @return The sum of all column parts. Is &gt;= 1.
	 */
	inline int32_t getSumOfAllColumnParts() const noexcept { return m_nColumnsSumOfParts; }
	/** The sum of all row parts.
	 * @return The sum of all row parts. Is &gt;= 1.
	 */
	inline int32_t getSumOfAllRowParts() const noexcept { return m_nRowsSumOfParts; }
	/** The column weight of the width of the box.
	 * @param nCol The column.
	 * @return The column part. Is &gt;= 1.
	 */
	inline int32_t getColumnPart(int32_t nCol) const noexcept
	{
		assert((nCol >= 0) && (nCol < static_cast<int32_t>(m_oData.m_aColumns.size())));
		return m_oData.m_aColumns[nCol];
	}
	/** The row weight of the height of the box.
	 * @param nRow The row.
	 * @return The row part. Is &gt;= 1.
	 */
	inline int32_t getRowPart(int32_t nRow) const noexcept
	{
		assert((nRow >= 0) && (nRow < static_cast<int32_t>(m_oData.m_aRows.size())));
		return m_oData.m_aRows[nRow];
	}
	/** Execute action.
	 * If type of cell is ACTION_KEY_ACTION, a KeyActionEvent is created out of a stmi::XYEvent for a player.
	 * @param refXYEvent The event generated in the view. Cannot be null.
	 * @param nTeam The team of the player. Must be `&gt;=-1`.
	 * @param nMate The mate number of the player. Must be `&gt;=-1`. If team is -1 must be -1 too.
	 * @param nCol The column. Must be &gt;= 0 and &lt; getTotColumns().
	 * @param nRow The row. Must be &gt;= 0 and &lt; getTotRows().
	 */
	void handleXYInput(const shared_ptr<stmi::Event>& refXYEvent, int32_t nTeam, int32_t nMate, int32_t nCol, int32_t nRow) noexcept;

	/** Constructor.
	 * The target widget with given name must exist in the layout when the layout is created
	 * and must not be a ContainerWidget (subclass).
	 *
	 * The team and mate of the target widget are the same as this widget.
	 * @param oInit The initialization data.
	 */
	explicit ActionsBoxWidget(Init&& oInit) noexcept
	: BoxWidget(std::move(oInit))
	, m_oData(std::move(oInit))
	{
		adjustData();
		checkActions();
	}

	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	/** Reinitialization.
	 * See ActionsBoxWidget::ActionsBoxWidget(Init&& oInit).
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept
	{
		BoxWidget::reInit(std::move(oInit));
		m_oData = std::move(oInit);
		adjustData();
		checkActions();
	}
private:
	void adjustData() noexcept;
	void checkActions() noexcept;
	void checkAction(const BoxCell& oBoxCell) noexcept;
	int32_t getIdx(int32_t nCol, int32_t nRow) const noexcept
	{
		const int32_t nTotColumns = static_cast<int32_t>(m_oData.m_aColumns.size());
		assert((nCol >= 0) && (nCol < nTotColumns));
		assert((nRow >= 0) && (nRow < static_cast<int32_t>(m_oData.m_aRows.size())));
		return nCol + nTotColumns * nRow;
	}

private:
	LocalInit m_oData;
	int32_t m_nColumnsSumOfParts;
	int32_t m_nRowsSumOfParts;
private:
	ActionsBoxWidget() = delete;
};

} // namespace stmg

#endif	/* STMG_ACTIONS_BOX_WIDGET_H */

