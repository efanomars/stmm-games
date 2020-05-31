/*
 * File:   xmlimageseqthanimationfactory.cc
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

#include "themeanimations/xmlimageseqthanimationfactory.h"

#include "themectx.h"
#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "xmlutil/xmlstrconv.h"
#include "xmlthemeimageparser.h"
 
#include <stmm-games-gtk/stdtheme.h>
#include <stmm-games-gtk/stdthemeanimationfactory.h>
#include <stmm-games-gtk/gtkutil/dynanimation.h>
#include <stmm-games-gtk/animations/imagesequencethanifactory.h>

#include <stmm-games/util/util.h>

#include <cassert>
//#include <iostream>
#include <cstdint>
#include <utility>

namespace stmg { class Image; }
namespace xmlpp { class Element; }

namespace stmg
{

const std::string XmlImageSeqThAnimationFactoryParser::s_sAnimationsImageSeqNodeName = "ImageSequence";
static const std::string s_sAnimationsImageSeqImageNodeName = "Image";
static const std::string s_sAnimationsImageSeqImageDurationAttr = "duration";
static const std::string s_sAnimationsImageSeqImageRelXAttr = "relX";
static const std::string s_sAnimationsImageSeqImageRelYAttr = "relY";
static const std::string s_sAnimationsImageSeqImageRelWAttr = "relW";
	static const int32_t s_nAnimationsImageSeqImageRelWAttrDefault = -1; // natural size
static const std::string s_sAnimationsImageSeqImageRelHAttr = "relH";
	static const int32_t s_nAnimationsImageSeqImageRelHAttrDefault = -1; // natural size
static const std::string s_sAnimationsImageSeqImagePriorityAttr = "priority";

XmlImageSeqThAnimationFactoryParser::XmlImageSeqThAnimationFactoryParser()
: XmlThAnimationFactoryParser(s_sAnimationsImageSeqNodeName)
{
}

unique_ptr<StdThemeAnimationFactory> XmlImageSeqThAnimationFactoryParser::parseAnimationFactory(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	auto refDyn = std::make_shared<DynAnimation>();

	getXmlConditionalParser()->visitNamedElementChildren(oCtx, p0Element, s_sAnimationsImageSeqImageNodeName, [&](const xmlpp::Element* p0Image)
	{
		parseAnimationsImageSeqImage(oCtx, p0Image, refDyn);
	});
	if (refDyn->getTotImages() == 0) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, "No images defined");
	}
	oCtx.removeChecker(p0Element, true);
	return std::make_unique<ImageSequenceThAniFactory>(&(oCtx.theme()), refDyn);
}
void XmlImageSeqThAnimationFactoryParser::parseAnimationsImageSeqImage(ThemeCtx& oCtx, const xmlpp::Element* p0Element, shared_ptr<DynAnimation>& refDyn)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	const auto oPairImgFile = getXmlThemeImageParser()->parseImage(oCtx, p0Element, true);
	assert(! oPairImgFile.first.empty());
	const std::string& sImgFile = oPairImgFile.second;
	shared_ptr<Image> refImg = oTheme.getImageByFileName(sImgFile);

	int32_t nDuration = 10;
	const auto oPairDuration = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sAnimationsImageSeqImageDurationAttr);
	if (oPairDuration.first) {
		const std::string& sDuration = oPairDuration.second;
		nDuration = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sAnimationsImageSeqImageDurationAttr, sDuration, false
														, true, 1, false, -1);
	}
	double fRelX = 0;
	const auto oPairRelX = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sAnimationsImageSeqImageRelXAttr);
	if (oPairRelX.first) {
		const std::string& sRelX = oPairRelX.second;
		fRelX = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sAnimationsImageSeqImageRelXAttr, sRelX, true
													, true, 0.01, false, -1);
	}
	double fRelY = 0;
	const auto oPairRelY = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sAnimationsImageSeqImageRelYAttr);
	if (oPairRelY.first) {
		const std::string& sRelY = oPairRelY.second;
		fRelY = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sAnimationsImageSeqImageRelYAttr, sRelY, true
													, true, 0.01, false, -1);
	}
	double fRelW = s_nAnimationsImageSeqImageRelWAttrDefault;
	const auto oPairRelW = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sAnimationsImageSeqImageRelWAttr);
	if (oPairRelW.first) {
		const std::string& sRelW = oPairRelW.second;
		fRelW = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sAnimationsImageSeqImageRelWAttr, sRelW, true
													, true, 0.01, false, -1);
	}
	double fRelH = s_nAnimationsImageSeqImageRelHAttrDefault;
	const auto oPairRelH = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sAnimationsImageSeqImageRelHAttr);
	if (oPairRelH.first) {
		const std::string& sRelH = oPairRelH.second;
		fRelH = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sAnimationsImageSeqImageRelHAttr, sRelH, true
													, true, 0.01, false, -1);
	}
	int32_t nPriority = 0;
	const auto oPairPriority = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sAnimationsImageSeqImagePriorityAttr);
	if (oPairPriority.first) {
		const std::string& sPriority = oPairPriority.second;
		nPriority = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sAnimationsImageSeqImagePriorityAttr, sPriority, false
										, true, 1, false, -1);
	}
	oCtx.removeChecker(p0Element, true);
	refDyn->addImage(nDuration, refImg, fRelX, fRelY, fRelW, fRelH, nPriority);
}

} // namespace stmg
