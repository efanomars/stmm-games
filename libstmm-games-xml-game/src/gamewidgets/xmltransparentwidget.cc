/*
 * File:   xmltransparentwidget.cc
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

#include "gamewidgets/xmltransparentwidget.h"

#include <stmm-games/widgets/transparentwidget.h>

#include <string>
#include <utility>
#include "layoutctx.h"

namespace stmg { class GameWidget; }
namespace xmlpp { class Element; }

namespace stmg
{


static const std::string s_sGameTransparentWidgetNodeName = "TransparentWidget";


XmlTransparentWidgetParser::XmlTransparentWidgetParser()
: XmlGameWidgetParser(s_sGameTransparentWidgetNodeName)
{
}

shared_ptr<GameWidget> XmlTransparentWidgetParser::parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
//std::cout << "XmlTransparentWidgetParser::parseGameWidget" << '\n';
	oCtx.addChecker(p0WidgetElement);
	TransparentWidget::Init oInit;
	parseRelSizedGameWidget(oInit, oCtx, p0WidgetElement);
	//
	oCtx.removeChecker(p0WidgetElement, true);
	return std::make_shared<TransparentWidget>(std::move(oInit));
}

} // namespace stmg
