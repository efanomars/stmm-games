/*
 * File:   xmlvolatilewidget.h
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

#ifndef STMG_XML_VOLATILE_WIDGET_H
#define STMG_XML_VOLATILE_WIDGET_H

#include "xmlgamewidgetparser.h"

#include <stmm-games/widgets/volatilewidget.h>

#include <memory>

namespace stmg { class GameWidget; }
namespace stmg { class LayoutCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class XmlVolatileWidgetParser : public XmlGameWidgetParser
{
public:
	XmlVolatileWidgetParser();

	shared_ptr<GameWidget> parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0Element) override;

private:
	void parseSelect(VolatileWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0Element);
	VolatileWidget::ValueImg parseCase(LayoutCtx& oCtx, const xmlpp::Element* p0Element);

private:
	XmlVolatileWidgetParser(const XmlVolatileWidgetParser& oSource) = delete;
	XmlVolatileWidgetParser& operator=(const XmlVolatileWidgetParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_VOLATILE_WIDGET_H */

