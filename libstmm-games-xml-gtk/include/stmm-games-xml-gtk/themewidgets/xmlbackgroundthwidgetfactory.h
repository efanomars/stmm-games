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
 * File:   xmlbackgroundthwidgetfactory.h
 */

#ifndef STMG_XML_BACKGROUND_TH_WIDGET_FACTORY_H
#define STMG_XML_BACKGROUND_TH_WIDGET_FACTORY_H

#include "xmlthwidgetfactoryparser.h"

#include <memory>

namespace stmg { class StdThemeWidgetFactory; }
namespace stmg { class ThemeCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

/** Parses BackgroundThWidgetFactory.
 */
class XmlBackgroundThWidgetFactoryParser : public XmlThWidgetFactoryParser
{
public:
	XmlBackgroundThWidgetFactoryParser();

	unique_ptr<StdThemeWidgetFactory> parseWidgetFactory(ThemeCtx& oCtx, const xmlpp::Element* p0Element) override;

private:
	XmlBackgroundThWidgetFactoryParser(const XmlBackgroundThWidgetFactoryParser& oSource) = delete;
	XmlBackgroundThWidgetFactoryParser& operator=(const XmlBackgroundThWidgetFactoryParser& oSource) = delete;
};

} // namespace stmg

#endif /* STMG_XML_BACKGROUND_TH_WIDGET_FACTORY_H */

