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
 * File:   xmlinputboxwidget.h
 */

#ifndef STMG_XML_INPUT_BOX_WIDGET_H
#define STMG_XML_INPUT_BOX_WIDGET_H

#include "gamewidgets/xmlboxwidget.h"

#include <memory>

namespace stmg { class GameWidget; }
namespace stmg { class LayoutCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class XmlInputBoxWidgetParser : public XmlBoxWidgetParser
{
public:
	XmlInputBoxWidgetParser();

	shared_ptr<GameWidget> parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0Element) override;

private:
	XmlInputBoxWidgetParser(const XmlInputBoxWidgetParser& oSource) = delete;
	XmlInputBoxWidgetParser& operator=(const XmlInputBoxWidgetParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_INPUT_BOX_WIDGET_H */

