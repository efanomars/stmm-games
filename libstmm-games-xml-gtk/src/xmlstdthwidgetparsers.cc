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
 * File:   xmlstdthwidgetparsers.cc
 */

#include "xmlstdthwidgetparsers.h"

#include "xmlthwidgetfactoryparser.h"

#include "themewidgets/xmlactionsboxthwidgetfactory.h"
#include "themewidgets/xmlactionthwidgetfactory.h"
#include "themewidgets/xmlbackgroundthwidgetfactory.h"
#include "themewidgets/xmlboxthwidgetfactory.h"
#include "themewidgets/xmlinputboxthwidgetfactory.h"
#include "themewidgets/xmllevelshowthwidgetfactory.h"
#include "themewidgets/xmlpreviewthwidgetfactory.h"
#include "themewidgets/xmlprogressthwidgetfactory.h"
#include "themewidgets/xmltransparentthwidgetfactory.h"
#include "themewidgets/xmlvarthwidgetfactory.h"
#include "themewidgets/xmlvolatilethwidgetfactory.h"

namespace stmg
{

std::vector<unique_ptr<XmlThWidgetFactoryParser>> getXmlStdThWidgetParsers()
{
	std::vector<unique_ptr<XmlThWidgetFactoryParser>> aXmlThWidgetParsers;
	//
	aXmlThWidgetParsers.push_back(std::make_unique<XmlPreviewThWidgetFactoryParser>());
	aXmlThWidgetParsers.push_back(std::make_unique<XmlVarThWidgetFactoryParser>());
	aXmlThWidgetParsers.push_back(std::make_unique<XmlActionThWidgetFactoryParser>());
	aXmlThWidgetParsers.push_back(std::make_unique<XmlLevelShowThWidgetFactoryParser>());
	aXmlThWidgetParsers.push_back(std::make_unique<XmlBoxThWidgetFactoryParser>());
	aXmlThWidgetParsers.push_back(std::make_unique<XmlInputBoxThWidgetFactoryParser>());
	aXmlThWidgetParsers.push_back(std::make_unique<XmlActionsBoxThWidgetFactoryParser>());
	aXmlThWidgetParsers.push_back(std::make_unique<XmlBackgroundThWidgetFactoryParser>());
	aXmlThWidgetParsers.push_back(std::make_unique<XmlTransparentThWidgetFactoryParser>());
	aXmlThWidgetParsers.push_back(std::make_unique<XmlProgressThWidgetFactoryParser>());
	aXmlThWidgetParsers.push_back(std::make_unique<XmlVolatileThWidgetFactoryParser>());

	return aXmlThWidgetParsers;
}

} // namespace stmg
