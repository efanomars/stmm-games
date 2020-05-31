/*
 * File:   xmlstdmodifierparsers.cc
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

#include "xmlstdmodifierparsers.h"

#include "xmlmodifierparser.h"

#include "modifiers/xmlalphamodifier.h"
#include "modifiers/xmlcasemodifier.h"
#include "modifiers/xmlfademodifier.h"
#include "modifiers/xmlfillmodifier.h"
#include "modifiers/xmlgrowmodifier.h"
#include "modifiers/xmlifelsemodifier.h"
#include "modifiers/xmlimagemodifier.h"
#include "modifiers/xmlmaskmodifier.h"
#include "modifiers/xmlnextthememodifier.h"
#include "modifiers/xmlrotatemodifier.h"
#include "modifiers/xmlselectanimodifier.h"
#include "modifiers/xmlselectcasemodifier.h"
#include "modifiers/xmlselectedimagemodifier.h"
#include "modifiers/xmlstopmodifier.h"
#include "modifiers/xmltextmodifier.h"
#include "modifiers/xmltileanimodifier.h"

namespace stmg
{

std::vector<unique_ptr<XmlModifierParser>> getXmlStdModifierParsers()
{
	std::vector<unique_ptr<XmlModifierParser>> aXmlModifierParsers;
	//
	aXmlModifierParsers.push_back(std::make_unique<XmlAlphaModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlCaseModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlFadeModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlFillModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlGrowModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlIfElseModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlImageModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlMaskModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlNextThemeModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlRotateModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlSelectAniModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlSelectCaseModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlSelectedImageModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlStopModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlTextModifierParser>());
	aXmlModifierParsers.push_back(std::make_unique<XmlTileAniModifierParser>());

	return aXmlModifierParsers;
}

} // namespace stmg
