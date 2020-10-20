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
 * File:   xmlactionbasewidget.h
 */

#ifndef STMG_XML_ACTION_BASE_WIDGET_H
#define STMG_XML_ACTION_BASE_WIDGET_H

#include "layoutctx.h"

#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/xmlcommonerrors.h>

#include <stmm-games/appconfig.h>
#include <stmm-games/gameproxy.h>
#include <stmm-games/util/util.h>
#include <stmm-games/widgets/actionwidget.h>

#include <type_traits>
#include <memory>
#include <string>

#include <stdint.h>

namespace stmg { class XmlGameWidgetParser; }
namespace xmlpp { class Element; }

namespace stmg
{

template<class TXmlGameWidgetParser>
class XmlActionBaseWidgetParser : public TXmlGameWidgetParser
{
public:
	explicit XmlActionBaseWidgetParser(const std::string& sActionWidgetNodeName)
	: TXmlGameWidgetParser(sActionWidgetNodeName)
	{
		static_assert(std::is_base_of<XmlGameWidgetParser, TXmlGameWidgetParser>::value, "");
	}

	struct XmlAction
	{
		bool m_bDefined = false; /**< Whether any action attribute was defined. */
		ActionWidget::ACTION_TYPE m_eActionType; /**< The action type. */
		int32_t m_nActionId = ActionWidget::LocalInit{}.m_nActionId; /**< See ActionWidget::LocalInit::m_nActionId . */
		int32_t m_nValue = ActionWidget::LocalInit{}.m_nValue; /**< See ActionWidget::LocalInit::m_nValue . */
		std::string m_sActionName; /**< See ActionWidget::LocalInit::m_sActionName . */
	};
	XmlAction parseActionAttr(LayoutCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory)
	{
		static const char* const s_sGameActionWidgetKeyActionNameAttr = "keyAction";
		static const char* const s_sGameActionWidgetInterruptNameAttr = "interrupt";
			static const char* const s_sGameActionWidgetInterruptAttrPause = "PAUSE";
			static const char* const s_sGameActionWidgetInterruptAttrAbort = "ABORT";
			static const char* const s_sGameActionWidgetInterruptAttrAbortAsk = "ABORT_ASK";
			static const char* const s_sGameActionWidgetInterruptAttrRestart = "RESTART";
			static const char* const s_sGameActionWidgetInterruptAttrRestartAsk = "RESTART_ASK";
			static const char* const s_sGameActionWidgetInterruptAttrQuitApp = "QUIT_APP";
			static const char* const s_sGameActionWidgetInterruptAttrQuitAppAsk = "QUIT_APP_ASK";
		static const char* const s_sGameActionWidgetEventMsgEventIdAttr = "event";
		static const char* const s_sGameActionWidgetEventMsgMsgNameAttr = "msgName";
		static const char* const s_sGameActionWidgetEventMsgMsgAttr = "msg";
		static const char* const s_sGameActionWidgetEventMsgValueAttr = "value";
		//
		XmlAction oAction;
		auto& oXmlConditionalParser = TXmlGameWidgetParser::getXmlConditionalParser();
		const auto oPairKeyActionName = oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sGameActionWidgetKeyActionNameAttr);
		const bool bKeyActionDefined = oPairKeyActionName.first;
		if (bKeyActionDefined) {
			const std::string& sKeyActionName = oPairKeyActionName.second;
			oAction.m_eActionType = ActionWidget::ACTION_KEY_ACTION;
			oAction.m_nActionId = oCtx.appConfig()->getKeyActionId(sKeyActionName);
			if (oAction.m_nActionId < 0) {
				throw XmlCommonErrors::error(oCtx, p0Element, s_sGameActionWidgetKeyActionNameAttr, Util::stringCompose(
																			"Key action name '%1' not found!", sKeyActionName));
			}
		}
		const auto oPairInterruptName = oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sGameActionWidgetInterruptNameAttr);
		const bool bInterruptDefined = oPairInterruptName.first;
		if (bInterruptDefined) {
			if (bKeyActionDefined) {
				throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sGameActionWidgetInterruptNameAttr, s_sGameActionWidgetKeyActionNameAttr);
			}
			const std::string& sInterruptName = oPairInterruptName.second;
			oAction.m_eActionType = ActionWidget::ACTION_INTERRUPT;
			GameProxy::INTERRUPT_TYPE eIT;
			if (sInterruptName == s_sGameActionWidgetInterruptAttrPause) {
				eIT = GameProxy::INTERRUPT_PAUSE;
			} else if (sInterruptName == s_sGameActionWidgetInterruptAttrAbort) {
				eIT = GameProxy::INTERRUPT_ABORT;
			} else if (sInterruptName == s_sGameActionWidgetInterruptAttrAbortAsk) {
				eIT = GameProxy::INTERRUPT_ABORT_ASK;
			} else if (sInterruptName == s_sGameActionWidgetInterruptAttrRestart) {
				eIT = GameProxy::INTERRUPT_RESTART;
			} else if (sInterruptName == s_sGameActionWidgetInterruptAttrRestartAsk) {
				eIT = GameProxy::INTERRUPT_RESTART_ASK;
			} else if (sInterruptName == s_sGameActionWidgetInterruptAttrQuitApp) {
				eIT = GameProxy::INTERRUPT_QUIT_APP;
			} else if (sInterruptName == s_sGameActionWidgetInterruptAttrQuitAppAsk) {
				eIT = GameProxy::INTERRUPT_QUIT_APP_ASK;
			} else {
				throw XmlCommonErrors::errorAttrInvalidValue(oCtx, p0Element, s_sGameActionWidgetInterruptNameAttr);
			}
			oAction.m_nActionId = static_cast<int32_t>(eIT);
		}
		//
		const auto oPairEvMsg = TXmlGameWidgetParser::parseEvIdMessage(oCtx, p0Element, false, s_sGameActionWidgetEventMsgEventIdAttr
																		, s_sGameActionWidgetEventMsgMsgNameAttr, s_sGameActionWidgetEventMsgMsgAttr);
		const std::string& sEvId = oPairEvMsg.first;
		const bool bEvIdMsgDefined = ! sEvId.empty();
		if (bEvIdMsgDefined) {
			if (bKeyActionDefined) {
				throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sGameActionWidgetEventMsgEventIdAttr, s_sGameActionWidgetKeyActionNameAttr);
			}
			if (bInterruptDefined) {
				throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sGameActionWidgetEventMsgEventIdAttr, s_sGameActionWidgetInterruptNameAttr);
			}
			const int32_t nMsg = oPairEvMsg.second;
			oAction.m_eActionType = ActionWidget::ACTION_EVENT_MSG;
			oAction.m_nActionId = nMsg;
			oAction.m_sActionName = sEvId;
		}
		const auto oPairMsgValue = oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sGameActionWidgetEventMsgValueAttr);
		if (oPairMsgValue.first) {
			if (! bEvIdMsgDefined) {
				throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sGameActionWidgetEventMsgEventIdAttr);
			}
			const std::string& sMsgValue = oPairMsgValue.second;
			oAction.m_nValue = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sGameActionWidgetEventMsgValueAttr, sMsgValue
															, false, false, -1, false, -1);
		}
		//
		const bool bDefined = (bKeyActionDefined || bInterruptDefined || bEvIdMsgDefined);
		if (bMandatory && !bDefined) {
			throw XmlCommonErrors::errorAttrEitherMustBeDefined(oCtx, p0Element
																, s_sGameActionWidgetKeyActionNameAttr
																, s_sGameActionWidgetInterruptNameAttr
																, s_sGameActionWidgetEventMsgEventIdAttr);
		}
		oAction.m_bDefined = bDefined;
		return oAction;
	}

private:
	XmlActionBaseWidgetParser(const XmlActionBaseWidgetParser& oSource) = delete;
	XmlActionBaseWidgetParser& operator=(const XmlActionBaseWidgetParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_ACTION_BASE_WIDGET_H */

