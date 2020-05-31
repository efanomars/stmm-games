/*
 * File:   xmlarrayevent.cc
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

#include "events/xmlarrayevent.h"

#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "gamectx.h"

#include "xmlutil/xmlstrconv.h"

#include <stmm-games/ownertype.h>
#include <stmm-games/event.h>
#include <stmm-games/named.h>
#include <stmm-games/events/arrayevent.h>
#include <stmm-games/util/util.h>
#include <stmm-games/util/namedindex.h>

//#include <libxml++/libxml++.h>

#include <vector>
#include <cassert>
//#include <iostream>
#include <cstdint>
#include <utility>
#include <algorithm>

namespace stmg { class ConditionalCtx; }

namespace stmg
{

static const std::string s_sEventArrayNodeName = "ArrayEvent";
static const std::string s_sEventArrayValueTypeAttr = "valueType";
	static const std::string s_sEventArrayValueTypeAttrValue = "VALUE";
	static const std::string s_sEventArrayValueTypeAttrImage = "IMAGE";
	static const std::string s_sEventArrayValueTypeAttrSound = "SOUND";
	static const std::string s_sEventArrayValueTypeAttrTileAni = "TILEANI";
static const std::string s_sEventArrayDefaultValueAttr = "defaultValue";
static const std::string s_sEventArrayDimensionNodeName = "Dim";
static const std::string s_sEventArrayDimensionSizeAttr = "size";
static const std::string s_sEventArrayDimensionVarNameAttr = "varName";
static const std::string s_sEventArrayArrayNodeName = "Array";
static const std::string s_sEventArrayArrayIdxAttr = "idx";
static const std::string s_sEventArrayArrayElsAttr = "els";
static const std::string s_sEventArrayElNodeName = "El";
static const std::string s_sEventArrayElIdxAttr = "idx";
static const std::string s_sEventArrayElValueAttr = "value";

XmlArrayEventParser::XmlArrayEventParser()
: XmlEventParser(s_sEventArrayNodeName)
{
}
Event* XmlArrayEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventArray(oCtx, p0Element), p0Element);
}
unique_ptr<Event> XmlArrayEventParser::parseEventArray(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlArrayEventParser::parseEventArray" << '\n';
	ArrayEvent::Init oAInit;
	oCtx.addChecker(p0Element);
	parseEventBase(oCtx, p0Element, oAInit);

	//
	static std::vector<char const *> s_aValueTypeEnumString{
		  s_sEventArrayValueTypeAttrValue.c_str()
		, s_sEventArrayValueTypeAttrImage.c_str()
		, s_sEventArrayValueTypeAttrSound.c_str()
		, s_sEventArrayValueTypeAttrTileAni.c_str()
	};
	static const std::vector<ARRAY_VALUE_TYPE> s_aValueTypeEnumValue{
		  ARRAY_VALUE_TYPE_VALUE
		, ARRAY_VALUE_TYPE_IMAGE
		, ARRAY_VALUE_TYPE_SOUND
		, ARRAY_VALUE_TYPE_TILEANI
	};
	//
	ARRAY_VALUE_TYPE eValueType = ARRAY_VALUE_TYPE_VALUE;
	const int32_t nIdxValueType = getXmlConditionalParser().getEnumAttributeValue(oCtx, p0Element, s_sEventArrayValueTypeAttr, s_aValueTypeEnumString);
	if (nIdxValueType >= 0) {
		eValueType = s_aValueTypeEnumValue[nIdxValueType];
		if (eValueType != ARRAY_VALUE_TYPE_VALUE) {
			oAInit.m_nDefaultValue = -1;
		}
	}

	const auto oPairDefaultValue = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventArrayDefaultValueAttr);
	if (oPairDefaultValue.first) {
		if (eValueType != ARRAY_VALUE_TYPE_VALUE) {
			//TODO crappy error
			throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, s_sEventArrayDefaultValueAttr, s_sEventArrayValueTypeAttr);
		}
		const std::string& sDefaultValue = oPairDefaultValue.second;
		oAInit.m_nDefaultValue = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventArrayDefaultValueAttr, sDefaultValue, false
															, false, -1, false, -1);
	}

	parseEventArrayDimensions(oCtx, p0Element, oAInit.m_aDimensions);

	int32_t nArraySize = 1;
	std::for_each(oAInit.m_aDimensions.begin(), oAInit.m_aDimensions.end(), [&](const ArrayEvent::Dimension& oDim)
	{
		nArraySize = nArraySize * oDim.m_nSize;
	});

	oAInit.m_aValues.resize(nArraySize, oAInit.m_nDefaultValue);

	const xmlpp::Element* p0ArrayElement = getXmlConditionalParser().parseUniqueElement(oCtx, p0Element, s_sEventArrayArrayNodeName, true);

	parseEventArrayArray(oCtx, p0ArrayElement, oAInit, eValueType);

	oCtx.removeChecker(p0Element, false, true);
	auto refArrayEvent = std::make_unique<ArrayEvent>(std::move(oAInit));
	return refArrayEvent;
}
std::tuple<int32_t, int32_t, int32_t> XmlArrayEventParser::parseVariableAndOwner(GameCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory)
{
	oCtx.addChecker(p0Element);

	int32_t nVarIndex = -1;
	int32_t nVarTeam = -1;
	int32_t nVarMate = -1;

	const auto oPairName = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventArrayDimensionVarNameAttr);
	if (oPairName.first) {
		const std::string& sName = oPairName.second;
		if (sName.empty()) {
			throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, s_sEventArrayDimensionVarNameAttr);
		}
		//
		const auto oPairOwner = getXmlConditionalParser().parseOwner(oCtx, p0Element);
		nVarTeam = oPairOwner.first;
		nVarMate = oPairOwner.second;
		const auto oPair = oCtx.getVariableIdAndOwnerTypeFromContext(sName, nVarTeam, nVarMate);
		nVarIndex = oPair.first;
		if (nVarIndex < 0) {
			throw XmlCommonErrors::errorAttrVariableNotDefined(oCtx, p0Element, s_sEventArrayDimensionVarNameAttr, sName);
		}
		const OwnerType eOwnerType = oPair.second;
		if (eOwnerType == OwnerType::GAME) {
			nVarTeam = -1;
			nVarMate = -1;
		} else if (eOwnerType == OwnerType::TEAM) {
			assert(nVarTeam >= 0);
			nVarMate = -1;
		} else {
			assert(nVarTeam >= 0);
			assert(nVarMate >= 0);
		}
	} else if (bMandatory) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sEventArrayDimensionVarNameAttr);
	} else {
		nVarIndex = -1;
		nVarTeam = -1;
		nVarMate = -1;
	}
	oCtx.removeChecker(p0Element, true);	

	return std::make_tuple(nVarIndex, nVarTeam, nVarMate);
}
void XmlArrayEventParser::parseEventArrayDimensions(GameCtx& oCtx, const xmlpp::Element* p0Element, std::vector<ArrayEvent::Dimension>& aDimensions)
{
	oCtx.addChecker(p0Element);

	int32_t nValueDimensions = 0;
	getXmlConditionalParser().visitNamedElementChildren(oCtx, p0Element, s_sEventArrayDimensionNodeName, [&](const xmlpp::Element* p0Dimension)
	{
		oCtx.addChecker(p0Dimension);
		ArrayEvent::Dimension oDim;
		const auto oTuple = parseVariableAndOwner(oCtx, p0Dimension, false);
		oDim.m_nVarIndex = std::get<0>(oTuple);
		oDim.m_nVarTeam = std::get<1>(oTuple);
		oDim.m_nVarMate = std::get<2>(oTuple);

		if (oDim.m_nVarIndex < 0) {
			++nValueDimensions;
		}
		const auto oPairDimensionSize = getXmlConditionalParser().getAttributeValue(oCtx, p0Dimension, s_sEventArrayDimensionSizeAttr);
		if (oPairDimensionSize.first) {
			const std::string& sDimensionSize = oPairDimensionSize.second;
			oDim.m_nSize = XmlUtil::strToNumber<int32_t>(oCtx, p0Dimension, s_sEventArrayDimensionSizeAttr, sDimensionSize, false
															, true, 1, false, -1);
		}

		aDimensions.push_back(std::move(oDim));
		oCtx.removeChecker(p0Dimension, true);
	});

	if (aDimensions.empty()) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0Element, s_sEventArrayDimensionNodeName);
	}
	if (nValueDimensions > 1) {
		throw XmlCommonErrors::error(oCtx, p0Element, "",  "At most one element '" + s_sEventArrayDimensionNodeName + "' can have attribute '"
									+ s_sEventArrayDimensionVarNameAttr + "' undefined");
	}
	oCtx.removeChecker(p0Element, true);
}
void XmlArrayEventParser::parseEventArrayArray(GameCtx& oCtx, const xmlpp::Element* p0Element, ArrayEvent::Init& oAInit
												, ARRAY_VALUE_TYPE eValueType)
{
//std::cout << "XmlArrayEventParser::parseEventArrayArray" << '\n';
	oCtx.addChecker(p0Element);

	const int32_t nDepth = static_cast<int32_t>(oAInit.m_aDimensions.size()) - 1;

	const int32_t nFromIdx = 0;
	const int32_t nSize = oAInit.m_aDimensions[nDepth].m_nSize;
	if (nDepth == 0) {
		parseEventArrayArrayEl(oCtx, p0Element, oAInit.m_aValues, nFromIdx, nSize, eValueType);
	} else {
		std::vector<int32_t> aIdxs;
		parseEventArraySubArray(oCtx, p0Element, oAInit, aIdxs, eValueType);
	}

	oCtx.removeChecker(p0Element, true);
}
int32_t calcAbsIndex(ArrayEvent::Init& oAInit, std::vector<int32_t>& aIdxs)
{
	const auto nTotIdxs = static_cast<int32_t>(aIdxs.size());
	assert(aIdxs.size() == (oAInit.m_aDimensions.size() - 1));
	int32_t nAbsIndex = 0;
	for (int32_t nCurIdx = 0; nCurIdx < nTotIdxs; ++nCurIdx) {
		const int32_t nIndex = aIdxs[nCurIdx];
		const int32_t nSize = oAInit.m_aDimensions[nCurIdx + 1].m_nSize;
		nAbsIndex = nAbsIndex * nSize + nIndex;
	}
	return nAbsIndex;
}
void XmlArrayEventParser::parseEventArraySubArray(GameCtx& oCtx, const xmlpp::Element* p0Element
												, ArrayEvent::Init& oAInit, std::vector<int32_t>& aIdxs
												, ARRAY_VALUE_TYPE eValueType)
{
	oCtx.addChecker(p0Element);
	const int32_t nTotDims = static_cast<int32_t>(oAInit.m_aDimensions.size());
	const int32_t nCurDim = static_cast<int32_t>(aIdxs.size());
	const int32_t nSize = oAInit.m_aDimensions[nCurDim].m_nSize;
	if (nCurDim == nTotDims - 1) {
		const int32_t nAbsFromIdx = calcAbsIndex(oAInit, aIdxs);
		parseEventArrayArrayEl(oCtx, p0Element, oAInit.m_aValues, nAbsFromIdx, nSize, eValueType);
	} else {
		int32_t nCurRelIdx = 0;
		getXmlConditionalParser().visitNamedElementChildren(oCtx, p0Element, s_sEventArrayArrayNodeName, [&](const xmlpp::Element* p0SubArray)
		{
			oCtx.addChecker(p0SubArray);

			int32_t nRelIdx = nCurRelIdx;
			const auto oPairArrayIdx = getXmlConditionalParser().getAttributeValue(oCtx, p0SubArray, s_sEventArrayArrayIdxAttr);
			if (oPairArrayIdx.first) {
				const std::string& sArrayIdx = oPairArrayIdx.second;
				nRelIdx = XmlUtil::strToNumber<int32_t>(oCtx, p0SubArray, s_sEventArrayArrayIdxAttr, sArrayIdx, false
														, true, nRelIdx, true, nSize - 1);
			}

			aIdxs.push_back(nRelIdx);
			parseEventArraySubArray(oCtx, p0SubArray, oAInit, aIdxs, eValueType);
			aIdxs.pop_back();
			
			nCurRelIdx = nRelIdx + 1;

			oCtx.removeChecker(p0SubArray, true);
		});
	}
	oCtx.removeChecker(p0Element, true);
}
void XmlArrayEventParser::parseEventArrayArrayEl(GameCtx& oCtx, const xmlpp::Element* p0Element
												, std::vector<int32_t>& aValues, int32_t nAbsFromIdx, int32_t nSize
												, ARRAY_VALUE_TYPE eValueType)
{
	oCtx.addChecker(p0Element);

	const auto oTokenToValue = [&](const std::string& sToken) -> int32_t
	{
		switch (eValueType) {
		case ARRAY_VALUE_TYPE_VALUE:
		{
			return XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventArrayArrayElsAttr, sToken, false
												, false, -1, false, -1);
		} break;
		case ARRAY_VALUE_TYPE_IMAGE:
		{
			const NamedIndex& oImages = oCtx.named().images();
			return oImages.getIndex(sToken);
		} break;
		case ARRAY_VALUE_TYPE_SOUND:
		{
			const NamedIndex& oSounds = oCtx.named().sounds();
			return oSounds.getIndex(sToken);
		} break;
		case ARRAY_VALUE_TYPE_TILEANI:
		{
			const NamedIndex& oTileAnis = oCtx.named().tileAnis();
			return oTileAnis.getIndex(sToken);
		} break;
		default:
		{
			assert(false);
			return -1;
		}
		}
	};
	const auto oPairArrayEls = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventArrayArrayElsAttr);
	if (oPairArrayEls.first) {
		const std::string& sArrayEls = oPairArrayEls.second;

		int32_t nRelIdx = 0;
		XmlUtil::tokenizer(sArrayEls, ",", [&](const std::string& sToken)
		{
			if (nRelIdx >= nSize) {
				throw XmlCommonErrors::error(oCtx, p0Element, s_sEventArrayArrayElsAttr
											, std::string("Attribute ") + s_sEventArrayArrayElsAttr
												+ " must have at most " + std::to_string(nSize) + " values");
			}
			const int32_t nValue = oTokenToValue(sToken);
			aValues[nAbsFromIdx + nRelIdx] = nValue;
			++nRelIdx;
		});
	}

	int32_t nCurRelIdx = 0;
	getXmlConditionalParser().visitNamedElementChildren(oCtx, p0Element, s_sEventArrayElNodeName, [&](const xmlpp::Element* p0ArrayEl)
	{
		oCtx.addChecker(p0ArrayEl);

		int32_t nRelIdx = nCurRelIdx;
		const auto oPairArrayIdx = getXmlConditionalParser().getAttributeValue(oCtx, p0ArrayEl, s_sEventArrayElIdxAttr);
		if (oPairArrayIdx.first) {
			const std::string& sArrayIdx = oPairArrayIdx.second;
			nRelIdx = XmlUtil::strToNumber<int32_t>(oCtx, p0ArrayEl, s_sEventArrayElIdxAttr, sArrayIdx, false
													, true, nRelIdx, true, nSize - 1);
		}

		const auto oPairArrayValue = getXmlConditionalParser().getAttributeValue(oCtx, p0ArrayEl, s_sEventArrayElValueAttr);
		if (! oPairArrayValue.first) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0ArrayEl, s_sEventArrayElValueAttr);
		}
		const std::string& sArrayValue = oPairArrayValue.second;
		const std::string sToken = Util::strStrip(sArrayValue);
		const int32_t nValue = oTokenToValue(sToken);

		aValues[nAbsFromIdx + nRelIdx] = nValue;
		nCurRelIdx = nRelIdx + 1;

		oCtx.removeChecker(p0ArrayEl, true);
	});

	oCtx.removeChecker(p0Element, true);
}
int32_t XmlArrayEventParser::parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
												, const std::string& sMsgName)
{
	int32_t nMsg;
	if (sMsgName == "ARRAY_GET") {
		nMsg = ArrayEvent::MESSAGE_ARRAY_GET;
	} else if (sMsgName == "ARRAY_GET_1") {
		nMsg = ArrayEvent::MESSAGE_ARRAY_GET_1;
	} else if (sMsgName == "ARRAY_GET_2") {
		nMsg = ArrayEvent::MESSAGE_ARRAY_GET_2;
	} else if (sMsgName == "ARRAY_GET_3") {
		nMsg = ArrayEvent::MESSAGE_ARRAY_GET_3;
	} else if (sMsgName == "ARRAY_GET_4") {
		nMsg = ArrayEvent::MESSAGE_ARRAY_GET_4;
	} else if (sMsgName == "ARRAY_GET_5") {
		nMsg = ArrayEvent::MESSAGE_ARRAY_GET_5;
	} else {
		return XmlEventParser::parseEventMsgName(oCtx, p0Element, sAttr, sMsgName);
	}
	return nMsg;
}
int32_t XmlArrayEventParser::parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
														, const std::string& sListenerGroupName)
{
	int32_t nListenerGroup;
	if (sListenerGroupName == "ARRAY_RESULT") {
		nListenerGroup = ArrayEvent::LISTENER_GROUP_ARRAY_RESULT;
	} else if (sListenerGroupName == "ARRAY_RESULT_1") {
		nListenerGroup = ArrayEvent::LISTENER_GROUP_ARRAY_RESULT_1;
	} else if (sListenerGroupName == "ARRAY_RESULT_2") {
		nListenerGroup = ArrayEvent::LISTENER_GROUP_ARRAY_RESULT_2;
	} else if (sListenerGroupName == "ARRAY_RESULT_3") {
		nListenerGroup = ArrayEvent::LISTENER_GROUP_ARRAY_RESULT_3;
	} else if (sListenerGroupName == "ARRAY_RESULT_4") {
		nListenerGroup = ArrayEvent::LISTENER_GROUP_ARRAY_RESULT_4;
	} else if (sListenerGroupName == "ARRAY_RESULT_5") {
		nListenerGroup = ArrayEvent::LISTENER_GROUP_ARRAY_RESULT_5;
	} else {
		return XmlEventParser::parseEventListenerGroupName(oCtx, p0Element, sAttr, sListenerGroupName);
	}
	return nListenerGroup;
}

} // namespace stmg
