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
 * File:   xmlimagemodifier.cc
 */

#include "modifiers/xmlimagemodifier.h"

#include "themectx.h"
#include "xmlthemeimageparser.h"

#include <stmm-games-gtk/modifiers/imagemodifier.h>
#include <stmm-games-gtk/stdtheme.h>
#include <stmm-games-gtk/stdthememodifier.h>

#include <stmm-games/util/util.h>

#include <string>
#include <utility>

namespace stmg { class Image; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sModifierImageNodeName = "Image";

XmlImageModifierParser::XmlImageModifierParser()
: XmlModifierParser(s_sModifierImageNodeName)
{
}

unique_ptr<StdThemeModifier> XmlImageModifierParser::parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	oCtx.pushCtx(Util::stringCompose("Modifier '%1': ", s_sModifierImageNodeName));
	const auto oPairImgFile = this->getXmlThemeImageParser()->parseImage(oCtx, p0Element, true);
	oCtx.popCtx();
	oCtx.removeChecker(p0Element, true);
	//
	const std::string& sImgFile = oPairImgFile.second;
	const shared_ptr<Image>& refImage = oCtx.theme().getImageByFileName(sImgFile);
	return std::make_unique<ImageModifier>(&oCtx.theme(), refImage);
}

} // namespace stmg

