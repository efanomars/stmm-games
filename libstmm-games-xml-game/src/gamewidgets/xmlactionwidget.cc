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
 * File:   xmlactionwidget.cc
 */

#include "gamewidgets/xmlactionwidget.h"

#include "gamewidgets/xmlactionbasewidget.h"
#include "layoutctx.h"
#include "xmlgamewidgetparser.h"

#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games/named.h>
#include <stmm-games/util/namedindex.h>
#include <stmm-games/widgets/actionwidget.h>

#include <iostream>
#include <string>
#include <utility>

namespace stmg { class GameWidget; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sGameActionWidgetNodeName = "ActionWidget";
static const std::string s_sGameActionWidgetTextAttr = "text";
static const std::string s_sGameActionWidgetTextHeightAttr = "textHeight";
static const std::string s_sGameActionWidgetImgIdAttr = "icon";


XmlActionWidgetParser::XmlActionWidgetParser()
: XmlActionBaseWidgetParser<XmlGameWidgetParser>(s_sGameActionWidgetNodeName)
{
}

shared_ptr<GameWidget> XmlActionWidgetParser::parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
	oCtx.addChecker(p0WidgetElement);
	ActionWidget::Init oInit;
	parseRelSizedGameWidget(oInit, oCtx, p0WidgetElement);

	XmlAction oAction = parseActionAttr(oCtx, p0WidgetElement, true);
	oInit.m_eActionType = oAction.m_eActionType;
	oInit.m_nActionId = oAction.m_nActionId;
	oInit.m_nValue = oAction.m_nValue;
	oInit.m_sActionName = std::move(oAction.m_sActionName);
	const auto oPairText = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameActionWidgetTextAttr);
	if (oPairText.first) {
		const std::string& sText = oPairText.second;
		oInit.m_sText = sText;
	}
	const auto oPairTextHeight = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameActionWidgetTextHeightAttr);
	if (oPairTextHeight.first) {
		const std::string& sTextHeight = oPairTextHeight.second;
		oInit.m_fTextSize = XmlUtil::strToNumber(oCtx, p0WidgetElement, s_sGameActionWidgetTextHeightAttr, sTextHeight
														, true, true, 0.0, false, -1.0);
	}
	const auto oPairImage = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameActionWidgetImgIdAttr);
	if (oPairImage.first) {
		const std::string& sImage = oPairImage.second;
		if (! sImage.empty()) {
			oInit.m_nImgId = oCtx.named().images().getIndex(sImage);
			if (oInit.m_nImgId < 0) {
				oInit.m_nImgId = oCtx.named().images().addName(sImage);
				std::cout << "Warning! ActionWidget: : image '" << sImage << "' not defined!" << '\n';
			}
		}
	}
	oCtx.removeChecker(p0WidgetElement, true);
	return std::make_shared<ActionWidget>(std::move(oInit));
}

} // namespace stmg
