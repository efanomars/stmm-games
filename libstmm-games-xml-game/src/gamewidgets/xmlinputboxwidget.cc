/*
 * File:   xmlinputboxwidget.cc
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

#include "gamewidgets/xmlinputboxwidget.h"
#include "gamewidgets/xmlboxwidget.h"
#include "layoutctx.h"
#include "xmlcommonerrors.h"
#include "xmlcommonparser.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>

#include <stmm-games/widgets/inputboxwidget.h>

#include <string>
#include <utility>

namespace stmg { class GameWidget; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sGameInputBoxWidgetNodeName = "InputBoxWidget";
static const std::string s_sGameInputBoxWidgetTargetNameAttr = "targetWidgetName";

XmlInputBoxWidgetParser::XmlInputBoxWidgetParser()
: XmlBoxWidgetParser(s_sGameInputBoxWidgetNodeName)
{
}
shared_ptr<GameWidget> XmlInputBoxWidgetParser::parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
//std::cout << "XmlInputBoxWidgetParser::parseInputBoxWidget" << '\n';
	oCtx.addChecker(p0WidgetElement);
	InputBoxWidget::Init oInit;
	parseBoxWidget(oInit, oCtx, p0WidgetElement);

	const auto oPairTargetName = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameInputBoxWidgetTargetNameAttr);
	if (!oPairTargetName.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0WidgetElement, s_sGameInputBoxWidgetTargetNameAttr);
	}
	oInit.m_sTargetWidgetName = oPairTargetName.second;
	XmlCommonParser::validateName(oCtx, p0WidgetElement, s_sGameInputBoxWidgetTargetNameAttr, oInit.m_sTargetWidgetName, false);
	// Child widget names are checked in parseBoxWidget
	oCtx.removeChecker(p0WidgetElement, false, true);
	return std::make_shared<InputBoxWidget>(std::move(oInit));
}

} // namespace stmg
