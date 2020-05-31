/*
 * File:   xmlselectanimodifier.h
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

#ifndef STMG_XML_SELECT_ANI_MODIFIER_PARSER_H
#define STMG_XML_SELECT_ANI_MODIFIER_PARSER_H

#include "xmlmodifierparser.h"

#include <memory>

#include <stdint.h>

namespace stmg { class StdThemeModifier; }
namespace stmg { class ThemeCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;
using std::shared_ptr;

class XmlSelectAniModifierParser : public XmlModifierParser
{
public:
	/** Constructor.
	 */
	XmlSelectAniModifierParser();

	unique_ptr<StdThemeModifier> parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element) override;

private:
	void parseModifierSelectAniCase(ThemeCtx& oCtx, const xmlpp::Element* p0Element
									, double& fFrom, bool& bFromExcl, double& fTo, bool& bToExcl);

private:
	XmlSelectAniModifierParser(const XmlSelectAniModifierParser& oSource) = delete;
	XmlSelectAniModifierParser& operator=(const XmlSelectAniModifierParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_SELECT_ANI_MODIFIER_PARSER_H */

