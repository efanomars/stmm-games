/*
 * File:   xmlstdgamewidgetparsers.cc
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

#include "xmlstdgamewidgetparsers.h"

#include "xmlgamewidgetparser.h"

#include "gamewidgets/xmlactionsboxwidget.h"
#include "gamewidgets/xmlactionwidget.h"
#include "gamewidgets/xmlbackgroundwidget.h"
#include "gamewidgets/xmlboxwidget.h"
#include "gamewidgets/xmlinputboxwidget.h"
#include "gamewidgets/xmllevelshowwidget.h"
#include "gamewidgets/xmlpreviewwidget.h"
#include "gamewidgets/xmlprogresswidget.h"
#include "gamewidgets/xmltransparentwidget.h"
#include "gamewidgets/xmlvarwidget.h"
#include "gamewidgets/xmlvolatilewidget.h"

#include <utility>

namespace stmg
{

std::vector<unique_ptr<XmlGameWidgetParser>> getXmlStdGameWidgetParsers()
{
	std::vector<unique_ptr<XmlGameWidgetParser>> aXmlGameWidgetParsers;
	//
	aXmlGameWidgetParsers.push_back(std::make_unique<XmlBoxWidgetParser>());
	aXmlGameWidgetParsers.push_back(std::make_unique<XmlInputBoxWidgetParser>());
	aXmlGameWidgetParsers.push_back(std::make_unique<XmlActionsBoxWidgetParser>());
	aXmlGameWidgetParsers.push_back(std::make_unique<XmlActionWidgetParser>());
	aXmlGameWidgetParsers.push_back(std::make_unique<XmlPreviewWidgetParser>());
	aXmlGameWidgetParsers.push_back(std::make_unique<XmlVarWidgetParser>());
	aXmlGameWidgetParsers.push_back(std::make_unique<XmlBackgroundWidgetParser>());
	aXmlGameWidgetParsers.push_back(std::make_unique<XmlLevelShowWidgetParser>());
	aXmlGameWidgetParsers.push_back(std::make_unique<XmlTransparentWidgetParser>());
	aXmlGameWidgetParsers.push_back(std::make_unique<XmlProgressWidgetParser>());
	aXmlGameWidgetParsers.push_back(std::make_unique<XmlVolatileWidgetParser>());

	return aXmlGameWidgetParsers;
}

} // namespace stmg
