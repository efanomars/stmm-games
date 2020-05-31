/*
 * File:   actionsboxwidget.cc
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

#include "widgets/actionsboxwidget.h"

#include "gameproxy.h"
#include "level.h"
#include "widgets/boxwidget.h"

#include <stmm-input/xyevent.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>

namespace stmi { class Event; }


namespace stmg
{

void ActionsBoxWidget::adjustData() noexcept
{
	m_nColumnsSumOfParts = 0;
	m_nRowsSumOfParts = 0;
	//
	if (m_oData.m_aColumns.empty()) {
		m_oData.m_aColumns.push_back(1);
	}
	for (const auto& nValue : m_oData.m_aColumns) {
		assert(nValue > 0);
		assert(m_nColumnsSumOfParts <= std::numeric_limits<int32_t>::max() - nValue);
		m_nColumnsSumOfParts += nValue;
	}
	if (m_oData.m_aRows.empty()) {
		m_oData.m_aRows.push_back(1);
	}
	for (const auto& nValue : m_oData.m_aRows) {
		assert(nValue > 0);
		assert(m_nRowsSumOfParts <= std::numeric_limits<int32_t>::max() - nValue);
		m_nRowsSumOfParts += nValue;
	}
	assert(m_oData.m_aActionCells.size() <= m_oData.m_aColumns.size() * m_oData.m_aRows.size());
	m_oData.m_aActionCells.resize(m_oData.m_aColumns.size() * m_oData.m_aRows.size());
}
void ActionsBoxWidget::checkAction(const BoxCell& oBoxCell) noexcept
{
	if (oBoxCell.m_eActionType == ACTION_KEY_ACTION) {
		assert(oBoxCell.m_nActionId >= 0);
	} else if (oBoxCell.m_eActionType == ACTION_INTERRUPT) {
		#ifndef NDEBUG
		const auto eIT = static_cast<GameProxy::INTERRUPT_TYPE>(oBoxCell.m_nActionId);
		#endif //NDEBUG
		assert((eIT == GameProxy::INTERRUPT_PAUSE)
				|| (eIT == GameProxy::INTERRUPT_ABORT)
				|| (eIT == GameProxy::INTERRUPT_ABORT_ASK)
				|| (eIT == GameProxy::INTERRUPT_RESTART)
				|| (eIT == GameProxy::INTERRUPT_RESTART_ASK)
				|| (eIT == GameProxy::INTERRUPT_QUIT_APP)
				|| (eIT == GameProxy::INTERRUPT_QUIT_APP_ASK));
	} else if (oBoxCell.m_eActionType == ACTION_EVENT_MSG) {
		#ifndef NDEBUG
		const int32_t nMsg = oBoxCell.m_nActionId;
		const std::string& sEvId = oBoxCell.m_sActionName;
		#endif //NDEBUG
		assert(! sEvId.empty());
		assert(nMsg >= -1);
	} else {
		assert(ACTION_NONE == oBoxCell.m_eActionType);
	}
}
void ActionsBoxWidget::checkActions() noexcept
{
	#ifndef NDEBUG
	int32_t nIdx = 0;
	for (std::size_t nRow = 0; nRow < m_oData.m_aRows.size(); ++nRow) {
		for (std::size_t nCol = 0; nCol < m_oData.m_aColumns.size(); ++nCol) {
			const BoxCell& oBoxCell = m_oData.m_aActionCells[nIdx];
//std::cout << "ActionsBoxWidget::checkActions row=" << nRow << "  col=" << nCol << "  " << oBoxCell.m_nActionId << '\n';
			checkAction(oBoxCell);
			++nIdx;
		}
	}
	#endif //NDEBUG
}

void ActionsBoxWidget::dump(int32_t
#ifndef NDEBUG
nIndentSpaces
#endif //NDEBUG
, bool
#ifndef NDEBUG
bHeader
#endif //NDEBUG
) const noexcept
{
	#ifndef NDEBUG
	auto sIndent = std::string(nIndentSpaces, ' ');
	if (bHeader) {
		std::cout << sIndent << "ActionsBoxWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	static const std::string s_sEmptySpaces{"              "};
	BoxWidget::dump(nIndentSpaces, false);
	std::cout << sIndent << "  " << "Row \\ Column   ";
	for (std::size_t nCol = 0; nCol < m_oData.m_aColumns.size(); ++nCol) {
		std::cout << (std::to_string(m_oData.m_aColumns[nCol]) + s_sEmptySpaces).substr(0, 15);
	}
	std::cout << '\n';
	int32_t nIdx = 0;
	for (std::size_t nRow = 0; nRow < m_oData.m_aRows.size(); ++nRow) {
		std::cout << sIndent << "  " << (std::to_string(m_oData.m_aRows[nRow]) + s_sEmptySpaces).substr(0, 15);
		for (std::size_t nCol = 0; nCol < m_oData.m_aColumns.size(); ++nCol) {
			const BoxCell& oBoxCell = m_oData.m_aActionCells[nIdx];
			std::string sAction;
			if (oBoxCell.m_eActionType == ACTION_NONE) {
				sAction = "-";
			} else if (oBoxCell.m_eActionType == ACTION_KEY_ACTION) {
				const int32_t nKeyActionId = oBoxCell.m_nActionId;
				sAction = "KA(" + std::to_string(nKeyActionId) + ")";
			} else if (oBoxCell.m_eActionType == ACTION_INTERRUPT) {
				const auto eInterruptType = static_cast<GameProxy::INTERRUPT_TYPE>(oBoxCell.m_nActionId);
				sAction = [&]()
				{
					if (eInterruptType == GameProxy::INTERRUPT_PAUSE) {
						return "PAUSE";
					} else if (eInterruptType == GameProxy::INTERRUPT_ABORT) {
						return "ABORT";
					} else if (eInterruptType == GameProxy::INTERRUPT_ABORT_ASK) {
						return "ABORT_ASK";
					} else if (eInterruptType == GameProxy::INTERRUPT_RESTART) {
						return "RESTART";
					} else if (eInterruptType == GameProxy::INTERRUPT_RESTART_ASK) {
						return "RESTART_ASK";
					} else if (eInterruptType == GameProxy::INTERRUPT_QUIT_APP) {
						return "QUIT_APP";
					} else if (eInterruptType == GameProxy::INTERRUPT_QUIT_APP_ASK) {
						return "QUIT_APP_ASK";
					}
					return "ERROR";
				}();
			} else {
				assert(ACTION_EVENT_MSG == oBoxCell.m_eActionType);
				const int32_t nMsg = oBoxCell.m_nActionId;
				const std::string& sEvId = oBoxCell.m_sActionName;
				sAction = "(" + std::to_string(nMsg) + " -> " + sEvId + ")";
			}
			std::cout << (sAction + s_sEmptySpaces).substr(0, 14) << " ";
			++nIdx;
		}
		std::cout << '\n';
	}
	#endif //NDEBUG
}
void ActionsBoxWidget::handleXYInput(const shared_ptr<stmi::Event>& refXYEvent, int32_t nTeam, int32_t nMate, int32_t nCol, int32_t nRow) noexcept
{
//std::cout << "ActionsBoxWidget::handleXYInput  nCol=" << nCol << "  nRow=" << nRow << '\n';
	const int32_t nIdx = getIdx(nCol, nRow);
	if (! ((nIdx >= 0) && (nIdx < static_cast<int32_t>(m_oData.m_aActionCells.size())))) {
		return;
	}
	auto& oGame = game();
	const bool bATIOL = oGame.isAllTeamsInOneLevel();
	int32_t nLevel = (bATIOL ? 0 : nTeam);
	const int32_t nLevelTeam = (bATIOL ? nTeam : 0);
	BoxCell& oCell = m_oData.m_aActionCells[nIdx];
	if (oCell.m_eActionType == ACTION_KEY_ACTION) {
		if (nMate < 0) {
			return; //----------------------------------------------------------
		}
		const int32_t nKeyActionId = oCell.m_nActionId;
		oGame.createKeyActionFromXYEvent(nLevel, nLevelTeam, nMate, nKeyActionId, refXYEvent);
		return; //--------------------------------------------------------------
	} else if (oCell.m_eActionType == ACTION_NONE) {
		return; //--------------------------------------------------------------
	}
	const auto p0XYEvent = static_cast<stmi::XYEvent*>(refXYEvent.get());
	if (p0XYEvent->getXYGrabType() != stmi::XYEvent::XY_GRAB) {
		return; //--------------------------------------------------------------
	}
	if (oCell.m_eActionType == ACTION_INTERRUPT) {
		const auto eInterruptType = static_cast<GameProxy::INTERRUPT_TYPE>(oCell.m_nActionId);
		oGame.interrupt(eInterruptType);
	} else {
		assert(oCell.m_eActionType == ACTION_EVENT_MSG);
		if (nLevel < 0) {
			if (oGame.getTotLevels() > 1) {
				return; //------------------------------------------------------
			}
			nLevel = 0;
		}
		auto& oLevel = oGame.level(nLevel);
		if (oCell.m_p0Event == nullptr) {
			const std::string& sEvId = oCell.m_sActionName;
			oCell.m_p0Event = oLevel.getEventById(sEvId);
		}
		if (oCell.m_p0Event != nullptr) {
			const int32_t nMsg = oCell.m_nActionId;
			oLevel.triggerEvent(oCell.m_p0Event, nMsg, oCell.m_nValue, oCell.m_p0Event);
		}
	}
}

} // namespace stmg
