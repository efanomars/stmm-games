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
 * File:   xmlboxthwidgetfactory.h
 */

#ifndef STMG_XML_BOX_TH_WIDGET_FACTORY_H
#define STMG_XML_BOX_TH_WIDGET_FACTORY_H

#include "xmlthwidgetfactoryparser.h"

#include <memory>

namespace stmg { class StdThemeWidgetFactory; }
namespace stmg { class ThemeCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

/** Parses BoxThWidgetFactory.
 */
class XmlBoxThWidgetFactoryParser : public XmlThWidgetFactoryParser
{
public:
	XmlBoxThWidgetFactoryParser();

	unique_ptr<StdThemeWidgetFactory> parseWidgetFactory(ThemeCtx& oCtx, const xmlpp::Element* p0Element) override;

private:
	XmlBoxThWidgetFactoryParser(const XmlBoxThWidgetFactoryParser& oSource) = delete;
	XmlBoxThWidgetFactoryParser& operator=(const XmlBoxThWidgetFactoryParser& oSource) = delete;
};

} // namespace stmg

#endif /* STMG_XML_BOX_TH_WIDGET_FACTORY_H */

