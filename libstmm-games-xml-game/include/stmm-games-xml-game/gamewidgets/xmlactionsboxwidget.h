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
 * File:   xmlactionsboxwidget.h
 */

#ifndef STMG_XML_ACTIONS_BOX_WIDGET_H
#define STMG_XML_ACTIONS_BOX_WIDGET_H

#include "gamewidgets/xmlactionbasewidget.h"

#include "gamewidgets/xmlboxwidget.h"

#include <stmm-games/widgets/actionsboxwidget.h>

#include <memory>

#include <stdint.h>

namespace stmg { class GameWidget; }
namespace stmg { class LayoutCtx; }
namespace stmg { class XmlBoxWidgetParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class XmlActionsBoxWidgetParser : public XmlActionBaseWidgetParser<XmlBoxWidgetParser>
{
public:
	XmlActionsBoxWidgetParser();

	shared_ptr<GameWidget> parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0Element) override;
private:
	int32_t parseActionsBoxRow(LayoutCtx& oCtx, const xmlpp::Element* p0RowElement, ActionsBoxWidget::Init& oInit, int32_t nTotColumns);

private:
	XmlActionsBoxWidgetParser(const XmlActionsBoxWidgetParser& oSource) = delete;
	XmlActionsBoxWidgetParser& operator=(const XmlActionsBoxWidgetParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_ACTIONS_BOX_WIDGET_H */

