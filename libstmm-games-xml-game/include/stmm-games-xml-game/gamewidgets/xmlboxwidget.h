/*
 * File:   xmlboxwidget.h
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

#ifndef STMG_XML_BOX_WIDGET_H
#define STMG_XML_BOX_WIDGET_H

#include "xmlgamewidgetparser.h"

#include <stmm-games/widgets/boxwidget.h>

#include <memory>
#include <vector>
#include <string>

namespace stmg { class GameWidget; }
namespace stmg { class LayoutCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class XmlBoxWidgetParser : public XmlGameWidgetParser
{
public:
	XmlBoxWidgetParser();

	shared_ptr<GameWidget> parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0Element) override;

protected:
	/** Constructor for subclasses.
	 * @param sGameWidgetName The game widget element name. Cannot be empty.
	 */
	explicit XmlBoxWidgetParser(const std::string& sGameWidgetName);
	/** See parseBoxWidget with additional aSkipChildNames parameter. */
	void parseBoxWidget(BoxWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement);
	/** Parse box widget for subclasses.
	 * Even the child names that are skipped are added as valid child names with
	 * oCtx.addValidChildElementName(). There is therefore no need to check the
	 * valid names when oCtx.removeChecker() is called.
	 * @param oInit The parameters to the model to be filled.
	 * @param oCtx The context.
	 * @param p0WidgetElement The widget element. Cannot be null.
	 * @param aSkipChildNames The child elements that BoxWidget parser has to skip.
	 */
	void parseBoxWidget(BoxWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement
						, const std::vector<std::string const*>& aSkipChildNames);

private:
	XmlBoxWidgetParser(const XmlBoxWidgetParser& oSource) = delete;
	XmlBoxWidgetParser& operator=(const XmlBoxWidgetParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_BOX_WIDGET_H */

