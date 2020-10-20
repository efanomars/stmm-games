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
 * File:   xmlpreviewwidget.cc
 */

#include "gamewidgets/xmlpreviewwidget.h"

#include "layoutctx.h"

#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games/widgets/previewwidget.h>

#include <cstdint>
#include <string>
#include <utility>

namespace stmg { class GameWidget; }
namespace xmlpp { class Element; }

namespace stmg
{


static const std::string s_sGamePreviewWidgetNodeName = "PreviewWidget";
static const std::string s_sGamePreviewWidgetMinTilesWAttr = "minTilesW";
static const std::string s_sGamePreviewWidgetMinTilesHAttr = "minTilesH";


XmlPreviewWidgetParser::XmlPreviewWidgetParser()
: XmlGameWidgetParser(s_sGamePreviewWidgetNodeName)
{
}

shared_ptr<GameWidget> XmlPreviewWidgetParser::parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
	oCtx.addChecker(p0WidgetElement);
	PreviewWidget::Init oInit;
	parseRelSizedGameWidget(oInit, oCtx, p0WidgetElement);
	//
	const auto oPairMinTilesW = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGamePreviewWidgetMinTilesWAttr);
	if (oPairMinTilesW.first) {
		const std::string& sMinTilesW = oPairMinTilesW.second;
		oInit.m_nMinTilesW = XmlUtil::strToNumber<int32_t>(oCtx, p0WidgetElement, s_sGamePreviewWidgetMinTilesWAttr, sMinTilesW
																	, false, true, 1, false, -1);
	}
	const auto oPairMinTilesH = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGamePreviewWidgetMinTilesHAttr);
	if (oPairMinTilesH.first) {
		const std::string& sMinTilesH = oPairMinTilesH.second;
		oInit.m_nMinTilesH = XmlUtil::strToNumber<int32_t>(oCtx, p0WidgetElement, s_sGamePreviewWidgetMinTilesHAttr, sMinTilesH
																	, false, true, 1, false, -1);
	}
//std::cout << "XmlPreviewWidgetParser::parseGameWidget  oInit.m_nTeam=" << oInit.m_nTeam << "  oInit.m_nMate=" << oInit.m_nMate << '\n';
	oCtx.removeChecker(p0WidgetElement, true);
	return std::make_shared<PreviewWidget>(std::move(oInit));
}

} // namespace stmg
