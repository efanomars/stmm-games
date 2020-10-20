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
 * File:   xmlboxwidget.cc
 */

#include "gamewidgets/xmlboxwidget.h"

#include "layoutctx.h"

#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games/gamewidget.h>
#include <stmm-games/widgets/boxwidget.h>

#include <array>
#include <utility>

#include <stdint.h>

namespace xmlpp { class Element; }

namespace stmg
{


static const std::string s_sGameBoxWidgetNodeName = "BoxWidget";
static const std::string s_sGameBoxWidgetVerticalAttr = "vertical";
static const std::string s_sGameBoxWidgetInvertChildrenAttr = "invertChildren";

XmlBoxWidgetParser::XmlBoxWidgetParser()
: XmlGameWidgetParser(s_sGameBoxWidgetNodeName)
{
}
XmlBoxWidgetParser::XmlBoxWidgetParser(const std::string& sGameWidgetName)
: XmlGameWidgetParser(sGameWidgetName)
{
}

void XmlBoxWidgetParser::parseBoxWidget(BoxWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
	parseBoxWidget(oInit, oCtx, p0WidgetElement, std::vector<std::string const*>{});
}
void XmlBoxWidgetParser::parseBoxWidget(BoxWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement
									, const std::vector<std::string const*>& aSkipChildNames)
{
//std::cout << "XmlBoxWidgetParser::parseBoxWidget" << '\n';
	parseContainerWidget(oInit, oCtx, p0WidgetElement, aSkipChildNames);
	//
	getXmlConditionalParser().visitLayoutConfigNamedAttributes(oCtx, p0WidgetElement, s_sGameBoxWidgetVerticalAttr, g_nTotLayoutConfigs
																, [&](const std::string& sLCAttrName, const std::string& sValue, int32_t nLayoutConfig)
		{
			const bool bVertical = XmlUtil::strToBool(oCtx, p0WidgetElement, sLCAttrName, sValue);
			if (nLayoutConfig < 0) {
				oInit.setVertical(bVertical);
			} else {
				oInit.m_aVertical[nLayoutConfig] = bVertical;
			}
		});
	getXmlConditionalParser().visitLayoutConfigNamedAttributes(oCtx, p0WidgetElement, s_sGameBoxWidgetInvertChildrenAttr, g_nTotLayoutConfigs
																, [&](const std::string& sLCAttrName, const std::string& sValue, int32_t nLayoutConfig)
		{
			const bool bInvertChildren = XmlUtil::strToBool(oCtx, p0WidgetElement, sLCAttrName, sValue);
			if (nLayoutConfig < 0) {
				oInit.setInvertChildren(bInvertChildren);
			} else {
				oInit.m_aInvertChildren[nLayoutConfig] = bInvertChildren;
			}
		});
}
shared_ptr<GameWidget> XmlBoxWidgetParser::parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
//std::cout << "XmlBoxWidgetParser::parseBoxWidget" << '\n';
	oCtx.addChecker(p0WidgetElement);
	BoxWidget::Init oInit;
	parseBoxWidget(oInit, oCtx, p0WidgetElement);
	// Child widget names are checked already
	oCtx.removeChecker(p0WidgetElement, false, true);
	return std::make_shared<BoxWidget>(std::move(oInit));
}

} // namespace stmg
