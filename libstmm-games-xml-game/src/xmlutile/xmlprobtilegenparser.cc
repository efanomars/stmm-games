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
 * File:   xmlprobtilegenparser.cc
 */

#include "xmlutile/xmlprobtilegenparser.h"

#include <stmm-games-xml-base/conditionalctx.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmltraitsparser.h>

#include <stmm-games/traitset.h>
#include <stmm-games/traitsets/alphatraitset.h>
#include <stmm-games/traitsets/chartraitset.h>
#include <stmm-games/traitsets/colortraitset.h>
#include <stmm-games/traitsets/fonttraitset.h>
#include <stmm-games/utile/randomtiles.h>

#include <cassert>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sProbTileGenRandomNodeName = "Random";
static const std::string s_sProbTileGenRandomProbAttrName = "prob";
static const constexpr int32_t s_nProbTileGenRandomProbMax = 10000;

XmlProbTileGenParser::XmlProbTileGenParser(XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser)
: m_sRandomNodeName(s_sProbTileGenRandomNodeName)
, m_oXmlConditionalParser(oXmlConditionalParser)
, m_oXmlTraitsParser(oXmlTraitsParser)
{
}
XmlProbTileGenParser::XmlProbTileGenParser(const std::string& sRandomNodeName
											, XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser)
: m_sRandomNodeName(sRandomNodeName)
, m_oXmlConditionalParser(oXmlConditionalParser)
, m_oXmlTraitsParser(oXmlTraitsParser)
{
	assert(! m_sRandomNodeName.empty());
}

RandomTiles::ProbTileGen XmlProbTileGenParser::parseProbTileGen(ConditionalCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	RandomTiles::ProbTileGen oNewRowTileGen;
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, m_sRandomNodeName, [&](const xmlpp::Element* p0RandomElement)
	{
		auto oProbTraitSets = parseRandom(oCtx, p0RandomElement);
		oNewRowTileGen.m_aProbTraitSets.emplace_back(std::move(oProbTraitSets));
	});
	if (oNewRowTileGen.m_aProbTraitSets.empty()) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0Element, m_sRandomNodeName);
	}
	oCtx.removeChecker(p0Element, true);
	return oNewRowTileGen;
}

RandomTiles::ProbTraitSets XmlProbTileGenParser::parseRandom(ConditionalCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	//
	RandomTiles::ProbTraitSets oProbTraitSets;
	//
	const auto oPairProb = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sProbTileGenRandomProbAttrName);
	if (oPairProb.first) {
		const std::string& sProb = oPairProb.second;
		oProbTraitSets.m_nProb = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sProbTileGenRandomProbAttrName, sProb, false
																		, true, 1, true, s_nProbTileGenRandomProbMax);
	}

	std::vector< std::unique_ptr<TraitSet> > aTraitSets;
	//
	bool bHasNonEmptyValue = false;
	{
	auto refCharTraitSet = std::make_unique<CharTraitSet>();
	m_oXmlTraitsParser.parseChars(oCtx, p0Element, *refCharTraitSet);
	const int32_t nCharSetTotValues = refCharTraitSet->getTotValues();
	if (nCharSetTotValues > 0) {
		if ((nCharSetTotValues > 1) || (!refCharTraitSet->hasEmptyValue())) {
			bHasNonEmptyValue = true;
		}
		aTraitSets.push_back(std::move(refCharTraitSet));
	}
	}
	//
	{
	auto refColorTraitSet = std::make_unique<ColorTraitSet>();
	m_oXmlTraitsParser.parseColors(oCtx, p0Element, true, *refColorTraitSet);
	const int32_t nColorSetTotValues = refColorTraitSet->getTotValues();
	if (nColorSetTotValues > 0) {
		if ((nColorSetTotValues > 1) || (!refColorTraitSet->hasEmptyValue())) {
			bHasNonEmptyValue = true;
		}
		aTraitSets.push_back(std::move(refColorTraitSet));
	}
	}
	//
	{
	auto refFontTraitSet = std::make_unique<FontTraitSet>();
	m_oXmlTraitsParser.parseFonts(oCtx, p0Element, *refFontTraitSet);
	const int32_t nFontSetTotValues = refFontTraitSet->getTotValues();
	if (nFontSetTotValues > 0) {
		if ((nFontSetTotValues > 1) || (!refFontTraitSet->hasEmptyValue())) {
			bHasNonEmptyValue = true;
		}
		aTraitSets.push_back(std::move(refFontTraitSet));
	}
	}
	//
	{
	auto refAlphaTraitSet = std::make_unique<AlphaTraitSet>();
	m_oXmlTraitsParser.parseAlphas(oCtx, p0Element, *refAlphaTraitSet);
	const int32_t nAlphaSetTotValues = refAlphaTraitSet->getTotValues();
	if (nAlphaSetTotValues > 0) {
		if ((nAlphaSetTotValues > 1) || (!refAlphaTraitSet->hasEmptyValue())) {
			bHasNonEmptyValue = true;
		}
		aTraitSets.push_back(std::move(refAlphaTraitSet));
	}
	}

	if (!bHasNonEmptyValue) {
		throw XmlCommonErrors::error(oCtx, p0Element, "", "At least one trait set must have a non empty value");
	}
	oCtx.removeChecker(p0Element, true);
	oProbTraitSets.m_aTraitSets = std::move(aTraitSets);
	return oProbTraitSets;
}

} // namespace stmg
