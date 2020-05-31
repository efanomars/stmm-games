/*
 * File:   xmlmaskmodifier.cc
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

#include "modifiers/xmlmaskmodifier.h"

#include "themectx.h"

#include <stmm-games-gtk/modifiers/maskmodifier.h>
#include <stmm-games-gtk/stdthememodifier.h>

#include <string>
#include <tuple>
#include <utility>

namespace stmg { class StdTheme; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sModifierMaskNodeName = "Mask";

XmlMaskModifierParser::XmlMaskModifierParser()
: XmlModifierParser(s_sModifierMaskNodeName)
{
}

unique_ptr<StdThemeModifier> XmlMaskModifierParser::parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	// Typical usage:
	// <Mask idAss="Imgs">
	//     <SelectedImage/>
	// </Mask>
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	oCtx.pushCtx(s_sModifierMaskNodeName + ": ");

	auto oTupleTileAni = parseTileAniIdNameImgDefault(oCtx, p0Element);

	MaskModifier::Init oInit;
	oInit.m_refTileAni = std::move(std::get<4>(oTupleTileAni));
	oInit.m_refMask = std::move(std::get<3>(oTupleTileAni));
	oInit.m_nElapsedTileAniIdx = std::get<0>(oTupleTileAni);
	oInit.m_fDefaultElapsed = std::get<1>(oTupleTileAni);
	oInit.m_bInvert = std::get<2>(oTupleTileAni);

	unique_ptr<MaskModifier> refMaskModifier = std::make_unique<MaskModifier>(&oTheme, std::move(oInit));
	
	auto aModifiers = parseSubModifiers(oCtx, p0Element);
	refMaskModifier->addSubModifiers(std::move(aModifiers));
	oCtx.popCtx();
	// child elements already checked by parseModifiers
	oCtx.removeChecker(p0Element, false, true);
	return refMaskModifier;
}


} // namespace stmg

