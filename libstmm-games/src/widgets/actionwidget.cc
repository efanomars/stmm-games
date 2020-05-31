/*
 * File:   actionwidget.cc
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

#include "widgets/actionwidget.h"

#include "gameproxy.h"
#include "level.h"

#include <stmm-input/xyevent.h>

#include <cassert>
#include <iostream>
#include <utility>

namespace stmi { class Event; }


namespace stmg
{

ActionWidget::ActionWidget(Init&& oInit) noexcept
: RelSizedGameWidget(std::move(oInit))
, m_oData(std::move(oInit))
, m_p0Event(nullptr)
, m_bEventChecked(false)
{
	checkData();
}
void ActionWidget::reInit(Init&& oInit) noexcept
{
	RelSizedGameWidget::reInit(std::move(oInit));
	m_oData = std::move(oInit);
	m_p0Event = nullptr;
	m_bEventChecked = false;
	checkData();
}

void ActionWidget::checkData() noexcept
{
	if (m_oData.m_eActionType == ACTION_KEY_ACTION) {
		assert(m_oData.m_nActionId >= 0);
	} else if (m_oData.m_eActionType == ACTION_INTERRUPT) {
		#ifndef NDEBUG
		const auto eIT = static_cast<GameProxy::INTERRUPT_TYPE>(m_oData.m_nActionId);
		#endif //NDEBUG
		assert((eIT == GameProxy::INTERRUPT_PAUSE)
				|| (eIT == GameProxy::INTERRUPT_ABORT)
				|| (eIT == GameProxy::INTERRUPT_ABORT_ASK)
				|| (eIT == GameProxy::INTERRUPT_RESTART)
				|| (eIT == GameProxy::INTERRUPT_RESTART_ASK)
				|| (eIT == GameProxy::INTERRUPT_QUIT_APP)
				|| (eIT == GameProxy::INTERRUPT_QUIT_APP_ASK));
	} else {
		assert(ACTION_EVENT_MSG == m_oData.m_eActionType);
		#ifndef NDEBUG
		const int32_t nMsg = m_oData.m_nActionId;
		const std::string& sEvId = m_oData.m_sActionName;
		#endif //NDEBUG
		assert(! sEvId.empty());
		assert(nMsg >= -1);
	}
}

void ActionWidget::dump(int32_t
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
		std::cout << sIndent << "ActionWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	RelSizedGameWidget::dump(nIndentSpaces + 2, false);
	std::string sAction;
	if (m_oData.m_eActionType == ACTION_KEY_ACTION) {
		const int32_t nKeyActionId = m_oData.m_nActionId;
		sAction = "KA(" + std::to_string(nKeyActionId) + ")";
	} else if (m_oData.m_eActionType == ACTION_INTERRUPT) {
		const auto eInterruptType = static_cast<GameProxy::INTERRUPT_TYPE>(m_oData.m_nActionId);
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
		assert(ACTION_EVENT_MSG == m_oData.m_eActionType);
		const int32_t nMsg = m_oData.m_nActionId;
		const std::string& sEvId = m_oData.m_sActionName;
		sAction = "EvMsg(" + std::to_string(nMsg) + " -> " + sEvId + ")";
	}
	std::cout << sIndent << "  " << "Action:     " << sAction << '\n';
	std::cout << sIndent << "  " << "m_nImgId:   " << m_oData.m_nImgId << '\n';
	std::cout << sIndent << "  " << "m_sText:    " << m_oData.m_sText << '\n';
	#endif //NDEBUG
}

void ActionWidget::handleXYInput(const shared_ptr<stmi::Event>& refXYEvent
									, int32_t nTeam, int32_t nMate) noexcept
{
	assert(refXYEvent);
	//assert(nMate >= 0);
	auto& oGame = game();
	const bool bATIOL = oGame.isAllTeamsInOneLevel();
	int32_t nLevel = (bATIOL ? 0 : nTeam);
	const int32_t nLevelTeam = (bATIOL ? nTeam : 0);
	if (m_oData.m_eActionType == ACTION_KEY_ACTION) {
		if (nMate < 0) {
			return; //----------------------------------------------------------
		}
		const int32_t nKeyActionId = m_oData.m_nActionId;
		oGame.createKeyActionFromXYEvent(nLevel, nLevelTeam, nMate, nKeyActionId, refXYEvent);
		return; //--------------------------------------------------------------
	}
	assert(dynamic_cast<stmi::XYEvent*>(refXYEvent.get()) != nullptr);
	auto p0XYEvent = static_cast<stmi::XYEvent*>(refXYEvent.get());
	if (p0XYEvent->getXYGrabType() != stmi::XYEvent::XY_GRAB) {
		return; //--------------------------------------------------------------
	}
	if (m_oData.m_eActionType == ACTION_INTERRUPT) {
		const auto eInterruptType = static_cast<GameProxy::INTERRUPT_TYPE>(m_oData.m_nActionId);
		oGame.interrupt(eInterruptType);
	} else {
		if (nLevel < 0) {
			if (oGame.getTotLevels() > 1) {
				return; //------------------------------------------------------
			}
			nLevel = 0;
		}
		assert(m_oData.m_eActionType == ACTION_EVENT_MSG);
		auto& oLevel = oGame.level(nLevel);
		if (! m_bEventChecked) {
			const std::string& sEvId = m_oData.m_sActionName;
			m_p0Event = oLevel.getEventById(sEvId);
			if (m_p0Event == nullptr) {
				#ifndef NDEBUG
				std::cout << "ActionWidget::handleXYInput() event with published id '" << m_oData.m_sActionName << "' not found!" << '\n';
				#endif //NDEBUG
			}
			m_bEventChecked = true;
		}
		if (m_p0Event != nullptr) {
			const int32_t nMsg = m_oData.m_nActionId;
			oLevel.triggerEvent(m_p0Event, nMsg, m_oData.m_nValue, m_p0Event);
		}
	}
}

} // namespace stmg
