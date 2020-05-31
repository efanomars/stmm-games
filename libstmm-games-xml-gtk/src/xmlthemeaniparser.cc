/*
 * File:   xmlthemeaniparser.cc
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

#include "xmlthemeaniparser.h"

#include "xmlthanimationfactoryparser.h"
#include "xmlutil/xmlstrconv.h"
#include "themectx.h"
#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "xmlthemeimageparser.h"

#include <stmm-games-gtk/gtkutil/tileani.h>
#include <stmm-games-gtk/stdtheme.h>
#include <stmm-games-gtk/stdthemeanimationfactory.h>

#include <stmm-games/util/util.h>

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>

#include <cassert>
//#include <iostream>
#include <cstdint>
#include <string>
#include <utility>

#include <stdint.h>

namespace stmg { class Image; }
namespace stmg { class XmlTraitsParser; }

namespace stmg
{

static const std::string s_sAnimationsAnimationNameAttr = "name";
static const std::string s_sAnimationsAnimationAnonymousAttr = "anonymousAnimations";

static const std::string s_sTileAnisAniNodeName = "TileAnimation";
static const std::string s_sTileAnisAniIdAttr = "aniId";
static const std::string s_sTileAnisAniDefaultImageFileAttr = "defaultImgFile";
static const std::string s_sTileAnisAniDefaultImageIdAttr = "defaultImgId";
static const std::string s_sTileAnisAniImageNodeName = "Image";
static const std::string s_sTileAnisAniImageDurationAttr = "duration";

XmlThemeAniParser::XmlThemeAniParser(XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser
									, XmlThemeImageParser& oXmlThemeImageParser)
: m_oXmlConditionalParser(oXmlConditionalParser)
, m_oXmlTraitsParser(oXmlTraitsParser)
, m_oXmlThemeImageParser(oXmlThemeImageParser)
{
}

void XmlThemeAniParser::addXmlThAnimationFactoryParser(unique_ptr<XmlThAnimationFactoryParser> refXmlThAnimationFactoryParser)
{
	assert(refXmlThAnimationFactoryParser);
	assert(refXmlThAnimationFactoryParser->m_p1Owner == nullptr);
	refXmlThAnimationFactoryParser->m_p1Owner = this;
	m_aXmlThAnimationFactoryParsers.push_back(std::move(refXmlThAnimationFactoryParser));
}

void XmlThemeAniParser::parseAnimations(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	m_oXmlConditionalParser.visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0AnimationElement)
	{
		parseAnimationsAnimation(oCtx, p0AnimationElement);
	});
	// just check it has no attributes
	oCtx.removeChecker(p0Element, false, true);
}
void XmlThemeAniParser::parseAnimationsAnimation(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	const std::string sAnimationFactoryName = p0Element->get_name();

	const auto oPairAnimationName = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sAnimationsAnimationNameAttr);
	if (! oPairAnimationName.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sAnimationsAnimationNameAttr);
	}
	const std::string& sAnimationName = oPairAnimationName.second;
	if (! oPairAnimationName.first) {
		throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, s_sAnimationsAnimationNameAttr);
	}
	if (oCtx.isMain() && oTheme.hasAnimationFactory(sAnimationName)) {
		throw XmlCommonErrors::errorAttrWithValueAlreadyDefined(oCtx, p0Element, s_sAnimationsAnimationNameAttr, sAnimationName);
	}
	bool bAnonymousAnimations = true;
	const auto oPairAnonymous = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sAnimationsAnimationAnonymousAttr);
	const bool bAnonymousDefined = oPairAnonymous.first;
	if (bAnonymousDefined) {
		const std::string& sAnonymous = oPairAnonymous.second;
		bAnonymousAnimations = XmlUtil::strToBool(oCtx, p0Element, s_sAnimationsAnimationAnonymousAttr, sAnonymous);
	}
	unique_ptr<StdThemeAnimationFactory> refFactory;
	for (auto& refParser : m_aXmlThAnimationFactoryParsers) {
		if (refParser->getAnimationFactoryName() == sAnimationFactoryName) {
			refFactory = refParser->parseAnimationFactory(oCtx, p0Element);
			if (refFactory) {
				break; // for (refParser) ---
			}
		}
	}
	if (!refFactory) {
		throw XmlCommonErrors::errorElementInvalid(oCtx, p0Element, sAnimationFactoryName);
	}
	oCtx.removeChecker(p0Element, false);
	oTheme.addAnimationFactory(sAnimationName, std::move(refFactory), bAnonymousAnimations);
}

void XmlThemeAniParser::parseTileAnis(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlThemeAniParser::parseTileAnis()" << '\n';
	oCtx.addChecker(p0Element);
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sTileAnisAniNodeName, [&](const xmlpp::Element* p0TileAni)
	{
		parseTileAnisAni(oCtx, p0TileAni);
	});
	// just check it has no attributes
	oCtx.removeChecker(p0Element, true);
}
void XmlThemeAniParser::parseTileAnisAni(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	const auto oPairTileAniId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTileAnisAniIdAttr);
	if (! oPairTileAniId.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sTileAnisAniIdAttr);
	}
	const std::string& sTileAniId = oPairTileAniId.second;
	if ((oCtx.isMain()) && oTheme.hasTileAniId(sTileAniId)) {
		throw XmlCommonErrors::errorAttrWithValueAlreadyDefined(oCtx, p0Element, s_sTileAnisAniIdAttr, sTileAniId);
	}

	const auto oPairDefaultImage = m_oXmlThemeImageParser.parseImage(oCtx, p0Element
															, s_sTileAnisAniDefaultImageFileAttr, s_sTileAnisAniDefaultImageIdAttr, false);
	const std::string& sImgAttr = oPairDefaultImage.first;
	const bool bDefaultDefined = ! sImgAttr.empty();
	const std::string& sDefaultImgFile = oPairDefaultImage.second;

	auto refTileAni = std::make_shared<TileAni>();
	if (bDefaultDefined) {
		shared_ptr<Image> refImg = oTheme.getImageByFileName(sDefaultImgFile);
		refTileAni->setDefaultImage(refImg);
	}

	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sTileAnisAniImageNodeName, [&](const xmlpp::Element* p0ImgDur)
	{
		parseTileAnisAniImgDur(oCtx, p0ImgDur, *refTileAni);
	});
	if ((!bDefaultDefined) && (refTileAni->getTotImages() == 0)) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, "No images defined");
	}
	oCtx.removeChecker(p0Element, true);
	oTheme.addTileAni(sTileAniId, refTileAni);
}
void XmlThemeAniParser::parseTileAnisAniImgDur(ThemeCtx& oCtx, const xmlpp::Element* p0Element, TileAni& oTileAni)
{
	oCtx.addChecker(p0Element);
	const auto oPairImgFile = m_oXmlThemeImageParser.parseImage(oCtx, p0Element, false);
	const std::string& sImgAttr = oPairImgFile.first;
	const bool bImageDefined = ! sImgAttr.empty();
	const std::string& sImgFile = oPairImgFile.second;

	int32_t nDuration = 1;
	const auto oPairDuration = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sTileAnisAniImageDurationAttr);
	if (oPairDuration.first) {
		const std::string& sDuration = oPairDuration.second;
		nDuration = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sTileAnisAniImageDurationAttr, sDuration, false
														, true, 1, false, -1);
	} else if (! bImageDefined) {
		throw XmlCommonErrors::errorAttrEitherMustBeDefined(oCtx, p0Element, s_sTileAnisAniImageDurationAttr
											, m_oXmlThemeImageParser.s_sImagesImageIdAttr, m_oXmlThemeImageParser.s_sImagesImageFileAttr);
	}
	shared_ptr<Image> refImg;
	if (bImageDefined)  {
		refImg = oCtx.theme().getImageByFileName(sImgFile);
		assert(refImg);
	}
	oCtx.removeChecker(p0Element, true);
	oTileAni.addImage(nDuration, refImg);
}

} // namespace stmg
