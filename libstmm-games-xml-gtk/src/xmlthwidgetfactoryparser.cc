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
 * File:   xmlthwidgetfactoryparser.cc
 */

#include "xmlthwidgetfactoryparser.h"

#include "xmlthemewidgetparser.h"

#include <stmm-games-gtk/gtkutil/frame.h>
#include <stmm-games-gtk/gtkutil/tilesizing.h>

namespace stmg { class ThemeCtx; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlTraitsParser; }
namespace xmlpp { class Element; }

namespace stmg
{

XmlThWidgetFactoryParser::XmlThWidgetFactoryParser(const std::string& sFactoryName)
: m_sFactoryName(sFactoryName)
, m_p1Owner(nullptr)
{
}
const std::string& XmlThWidgetFactoryParser::getFactoryName() const
{
	return m_sFactoryName;
}
XmlConditionalParser* XmlThWidgetFactoryParser::getXmlConditionalParser()
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return nullptr;
	}
	#endif //NDEBUG
	return &(m_p1Owner->m_oXmlConditionalParser);
}
XmlTraitsParser* XmlThWidgetFactoryParser::getXmlTraitsParser()
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return nullptr;
	}
	#endif //NDEBUG
	return &(m_p1Owner->m_oXmlTraitsParser);
}
XmlThemeImageParser* XmlThWidgetFactoryParser::getXmlThemeImageParser()
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return nullptr;
	}
	#endif //NDEBUG
	return &(m_p1Owner->m_oXmlThemeImageParser);
}
Frame XmlThWidgetFactoryParser::parseLayoutFrame(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return Frame{};
	}
	#endif //NDEBUG
	return m_p1Owner->parseLayoutFrame(oCtx, p0Element);
}
TileSizing XmlThWidgetFactoryParser::parseLayoutTileSizing(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return TileSizing{};
	}
	#endif //NDEBUG
	return m_p1Owner->parseLayoutTileSizing(oCtx, p0Element);
}

} // namespace stmg
