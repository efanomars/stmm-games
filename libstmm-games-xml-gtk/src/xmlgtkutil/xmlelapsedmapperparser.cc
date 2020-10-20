/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   xmlelapsedmapperparser.cc
 */

#include "xmlgtkutil/xmlelapsedmapperparser.h"

#include <stmm-games-xml-base/conditionalctx.h>
//#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>

#include <stmm-games-gtk/gtkutil/elapsedmapper.h>

#include <cassert>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>
//#include <iostream>

namespace xmlpp { class Element; }

namespace stmg
{

//static const std::string s_sElapsedMapperNodeName = "ElapsedMapper";
static const std::string s_sElapsedMapperAAttrName = "a";
	static const constexpr double s_nElapsedMapperAMin = -10000.0;
	static const constexpr double s_nElapsedMapperAMax = 10000.0;
static const std::string s_sElapsedMapperBAttrName = "b";
	static const constexpr double s_nElapsedMapperBMin = -10000.0;
	static const constexpr double s_nElapsedMapperBMax = 10000.0;
static const std::string s_sElapsedMapperMappingTypeAttr = "mappingType";
	static const std::string s_sElapsedMapperMappingTypeAttrIdentity = "IDENTITY";
	static const std::string s_sElapsedMapperMappingTypeAttrOORUndefined = "OOR_UNDEFINED";
	static const std::string s_sElapsedMapperMappingTypeAttrOORClamp = "OOR_CLAMP";
	static const std::string s_sElapsedMapperMappingTypeAttrOORModulo = "OOR_MODULO";
	static const std::string s_sElapsedMapperMappingTypeAttrOORAltModulo = "OOR_ALT_MODULO";
static const std::string s_sElapsedMapperMinAttrName = "min";
static const std::string s_sElapsedMapperMaxAttrName = "max";
static const std::string s_sElapsedMapperShrinkAttrName = "shrink";

XmlElapsedMapperParser::XmlElapsedMapperParser(XmlConditionalParser& oXmlConditionalParser)
: m_oXmlConditionalParser(oXmlConditionalParser)
{
}

ElapsedMapper XmlElapsedMapperParser::parseElapsedMapper(ConditionalCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	ElapsedMapper oElapsedMapper;
	const auto oPairA = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sElapsedMapperAAttrName);
	if (oPairA.first) {
		const std::string& sA = oPairA.second;
		oElapsedMapper.m_fA = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sElapsedMapperAAttrName, sA
															, true, true, s_nElapsedMapperAMin, true, s_nElapsedMapperAMax);
	}
	const auto oPairB = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sElapsedMapperBAttrName);
	if (oPairB.first) {
		const std::string& sB = oPairB.second;
		oElapsedMapper.m_fB = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sElapsedMapperBAttrName, sB
															, true, true, s_nElapsedMapperBMin, true, s_nElapsedMapperBMax);
	}
	const auto oPairMin = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sElapsedMapperMinAttrName);
	if (oPairMin.first) {
		const std::string& sMin = oPairMin.second;
		oElapsedMapper.m_fMin = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sElapsedMapperMinAttrName, sMin
															, true, true, 0.0, true, 1.0);
	}
	const auto oPairMax = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sElapsedMapperMaxAttrName);
	if (oPairMax.first) {
		const std::string& sMax = oPairMax.second;
		oElapsedMapper.m_fMax = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sElapsedMapperMaxAttrName, sMax
															, true, true, oElapsedMapper.m_fMin, true, 1.0);
	}
	const auto oPairShrink = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sElapsedMapperShrinkAttrName);
	if (oPairShrink.first) {
		const std::string& sShrink = oPairShrink.second;
		oElapsedMapper.m_bShrink = XmlUtil::strToBool(oCtx, p0Element, s_sElapsedMapperShrinkAttrName, sShrink);
	}
	static std::vector<char const *> s_aMappingTypeEnumString{
		  s_sElapsedMapperMappingTypeAttrIdentity.c_str()
		, s_sElapsedMapperMappingTypeAttrOORUndefined.c_str()
		, s_sElapsedMapperMappingTypeAttrOORClamp.c_str()
		, s_sElapsedMapperMappingTypeAttrOORModulo.c_str()
		, s_sElapsedMapperMappingTypeAttrOORAltModulo.c_str()
	};
	static const std::vector<ElapsedMapper::MAPPING_TYPE> s_aMappingTypeEnumValue{
		  ElapsedMapper::MAPPING_TYPE_IDENTITY
		, ElapsedMapper::MAPPING_TYPE_OOR_UNDEFINED
		, ElapsedMapper::MAPPING_TYPE_OOR_CLAMP
		, ElapsedMapper::MAPPING_TYPE_OOR_MODULO
		, ElapsedMapper::MAPPING_TYPE_OOR_ALT_MODULO
	};
	//
	const int32_t nIdxMappingType = m_oXmlConditionalParser.getEnumAttributeValue(oCtx, p0Element
												, s_sElapsedMapperMappingTypeAttr, s_aMappingTypeEnumString);
	if (nIdxMappingType >= 0) {
//std::cout << "XmlElapsedMapperParser::parseElapsedMapper nIdxMappingType " << nIdxMappingType << '\n';
		oElapsedMapper.m_eMappingType = s_aMappingTypeEnumValue[nIdxMappingType];
	}
	oCtx.removeChecker(p0Element, true);
	return oElapsedMapper;
}


} // namespace stmg
