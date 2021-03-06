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
 * File:   xmlnextthememodifier.cc
 */

#include "modifiers/xmlnextthememodifier.h"

#include "themectx.h"

#include <stmm-games-gtk/modifiers/nextsubpaintermodifier.h>
#include <stmm-games-gtk/stdthememodifier.h>

#include <string>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sModifierNextThemeNodeName = "Next";

XmlNextThemeModifierParser::XmlNextThemeModifierParser()
: XmlModifierParser(s_sModifierNextThemeNodeName)
{
}

unique_ptr<StdThemeModifier> XmlNextThemeModifierParser::parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	oCtx.removeChecker(p0Element, true);
	return std::make_unique<NextSubPainterModifier>(&oCtx.theme());
}

} // namespace stmg

