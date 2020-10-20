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
 * File:   xmlexplosionthanimationfactory.cc
 */

#include "themeanimations/xmlexplosionthanimationfactory.h"

#include "themectx.h"
#include "themeanimations/xmlimageseqthanimationfactory.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>

#include <stmm-games-gtk/animations/explosionthanifactory.h>
#include <stmm-games-gtk/animations/imagesequencethanifactory.h>
#include <stmm-games-gtk/stdtheme.h>
#include <stmm-games-gtk/stdthemeanimationfactory.h>

#include <stmm-games/util/util.h>

#include <cassert>
//#include <iostream>
#include <string>
#include <utility>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sAnimationsExplosionImSeqNodeName = "ExplosionIsImageSequence";
static const std::string s_sAnimationsExplosionImSeqNameAttr = "imageSequenceName";

XmlExplosionThAnimationFactoryParser::XmlExplosionThAnimationFactoryParser()
: XmlThAnimationFactoryParser(s_sAnimationsExplosionImSeqNodeName)
{
}

unique_ptr<StdThemeAnimationFactory> XmlExplosionThAnimationFactoryParser::parseAnimationFactory(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	const auto oPairAnimationId = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sAnimationsExplosionImSeqNameAttr);
	if (!oPairAnimationId.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sAnimationsExplosionImSeqNameAttr);
	}
	const std::string& sAnimationId = oPairAnimationId.second;
	if (!oTheme.hasAnimationFactory(sAnimationId)) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sAnimationsExplosionImSeqNameAttr, Util::stringCompose(s_sAnimationsExplosionImSeqNodeName
					+ ": Attribute '%1': value '%2' not defined", s_sAnimationsExplosionImSeqNameAttr, sAnimationId));
	}

	StdThemeAnimationFactory* p0AnimationFactory = oTheme.getAnimationFactory(sAnimationId);
	assert(p0AnimationFactory != nullptr);
	auto p0ImageSequenceFactory = dynamic_cast<ImageSequenceThAniFactory*>(p0AnimationFactory);
	if (p0ImageSequenceFactory == nullptr) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sAnimationsExplosionImSeqNameAttr, Util::stringCompose(s_sAnimationsExplosionImSeqNodeName
					+ ": attribute '%1': value '%2' not of type %3", s_sAnimationsExplosionImSeqNameAttr, sAnimationId, XmlImageSeqThAnimationFactoryParser::s_sAnimationsImageSeqNodeName));
	}
	oCtx.removeChecker(p0Element, true);
	return std::make_unique<ExplosionThAniFactory>(&(oCtx.theme()), p0ImageSequenceFactory);
}

} // namespace stmg
