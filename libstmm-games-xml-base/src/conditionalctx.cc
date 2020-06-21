/*
 * File:   conditionalctx.cc
 *
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

#include "conditionalctx.h"

#include "xmlconditionalparser.h"

#include "xmlutil/xmlstrconv.h"
#include "xmlcommonerrors.h"
#include "xmlcommonparser.h"

#include <stmm-games/util/util.h>

#include <cassert>
//#include <iostream>
#include <algorithm>
#include <string>
#include <utility>
#include <vector>

namespace stmg { class AppConfig; }
namespace stmg { class AppPreferences; }
namespace stmg { class Named; }
namespace xmlpp { class Element; }

namespace stmg
{

ConditionalCtx::ConditionalCtx(const shared_ptr<AppPreferences>& refAppPreferences, Named& oNamed)
: ParserCtx(refAppPreferences, oNamed)
{
}
ConditionalCtx::ConditionalCtx(const shared_ptr<AppConfig>& refAppConfig, Named& oNamed)
: ParserCtx(refAppConfig, oNamed)
{
}

void ConditionalCtx::addChecker(const xmlpp::Element* p0Element)
{
	ParserCtx::addChecker(p0Element);
	ParserCtx::addValidChildElementNames(p0Element
										, XmlConditionalParser::s_sConditionalExistIfNode
										, XmlConditionalParser::s_sConditionalPassthroughIfNode
										, XmlConditionalParser::s_sConditionalAttrIfNode);
}
void ConditionalCtx::removeChecker(const xmlpp::Element* p0Element, bool bCheckChildElements, bool bCheckAttrs)
{
	auto itFindChecker = getChecker(p0Element);
//#ifndef NDEBUG
//if (itFindChecker == m_aCheckers.end()) {
//std::cout << "ConditionalCtx::removeChecker p0Element=" << p0Element->get_name() << '\n';
//}
//#endif //NDEBUG
	assert(itFindChecker != m_aCheckers.end());
	Checker& oChecker = *itFindChecker;
	if (oChecker.m_nCounter == 1) {
		// The checker will be decremented to 0 within the super class implementation called further down
		if (bCheckAttrs || oChecker.m_bCheckAttrs) {
			// Check all the attributes
			XmlCommonParser::visitNamedElementChildren(*this, p0Element, XmlConditionalParser::s_sConditionalAttrIfNode, [&](const xmlpp::Element* p0AttrIf)
			{
				const auto& aNames = oChecker.m_aValidAttrNames;
				bool bHasUndef = false;
				XmlCommonParser::checkAllAttributesNames(*this, p0AttrIf, [&](const std::string& sAttrName)
				{
					if (sAttrName == XmlConditionalParser::s_sConditionalAttrIfUndefAttr) {
						bHasUndef = true;
						return true; //------
					}
					auto itFindName = std::find(aNames.begin(), aNames.end(), sAttrName);
					return (itFindName != aNames.end());
				});
				if (bHasUndef) {
					const auto oPairUndefAttrs = XmlCommonParser::getAttributeValue(*this, p0AttrIf, XmlConditionalParser::s_sConditionalAttrIfUndefAttr);
					assert(oPairUndefAttrs.first);
					// check that all the attributes in the list are valid
					XmlUtil::tokenizer(oPairUndefAttrs.second, XmlConditionalParser::s_sConditionalAttrIfUndefAttrSeparator, [&](const std::string& sToken)
					{
						auto itFindName = std::find(aNames.begin(), aNames.end(), sToken);
						if (itFindName == aNames.end()) {
							throw XmlCommonErrors::error(*this, p0AttrIf, sToken, Util::stringCompose("Attribute %1: invalid", sToken));
						}
					});
				}
			});
		}
	}
	// call super class
	ParserCtx::removeChecker(p0Element, bCheckChildElements, bCheckAttrs);
}

} // namespace stmg
