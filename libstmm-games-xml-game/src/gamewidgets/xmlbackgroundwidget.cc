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
 * File:   xmlbackgroundwidget.cc
 */

#include "gamewidgets/xmlbackgroundwidget.h"

#include "layoutctx.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlutil/xmlimageparser.h>

#include <stmm-games/util/util.h>
#include <stmm-games/widgets/backgroundwidget.h>

#include <string>
#include <utility>
#include <vector>

namespace stmg { class GameWidget; }
namespace xmlpp { class Element; }

namespace stmg
{


static const std::string s_sGameBackgroundWidgetNodeName = "BackgroundWidget";
static const std::string s_sGameBackgroundWidgetImageAttr = "image";


XmlBackgroundWidgetParser::XmlBackgroundWidgetParser()
: XmlGameWidgetParser(s_sGameBackgroundWidgetNodeName)
{
}

shared_ptr<GameWidget> XmlBackgroundWidgetParser::parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
//std::cout << "XmlBackgroundWidgetParser::parseGameWidget" << '\n';
	oCtx.addChecker(p0WidgetElement);
	BackgroundWidget::Init oInit;
	parseContainerWidget(oInit, oCtx, p0WidgetElement);
	if (oInit.m_aChildWidgets.size() != 1) {
		throw XmlCommonErrors::error(oCtx, p0WidgetElement, s_sGameBackgroundWidgetNodeName, Util::stringCompose(
																			"Background widget must have one child"));
	}
	//
	oInit.m_nImgId = getXmlImageParser().parseImageId(oCtx, p0WidgetElement, s_sGameBackgroundWidgetImageAttr, true);

	oCtx.removeChecker(p0WidgetElement, false, true);
	return std::make_shared<BackgroundWidget>(std::move(oInit));
}

} // namespace stmg
