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
 * File:   xmlconditionalparser.h
 */

#ifndef STMG_XML_CONDITIONAL_PARSER_H
#define STMG_XML_CONDITIONAL_PARSER_H

#include "conditionalctx.h"
#include "xmlcommonparser.h"
#include "xmlutil/xmlstrconv.h"
#include "xmlcommonerrors.h"

#include <stmm-games/ownertype.h>
#include <stmm-games/util/util.h>

#include <glibmm/ustring.h>

#include <libxml++/libxml++.h>

#include <vector>
#include <stdexcept>
#include <limits>
#include <list>
#include <string>
#include <tuple>
#include <utility>

#include <stdint.h>

namespace stmg { class IntSet; }
namespace stmg { class ParserCtx; }

namespace stmg
{

class XmlVariantSetParser;

class XmlConditionalParser
{
public:
	static const std::string s_sConditionalExistIfNode;
	static const std::string s_sConditionalPassthroughIfNode;
	static const std::string s_sConditionalAttrIfNode;

	static const std::string s_sConditionalAttrIfUndefAttr;
	static const std::string s_sConditionalAttrIfUndefAttrSeparator;

	static const std::string s_sConditionalOwnerTeamAttr;
	static const std::string s_sConditionalOwnerMateAttr;
	static const std::string s_sConditionalOwnerPlayerAttr;

	/** Constructor. */
	XmlConditionalParser(XmlVariantSetParser& oXmlVariantSetParser);

	/** Visit children elements with a given name applying nested ExistIf conditions.
	 * If sName is empty, ignored elements are skipped.
	 * @param oCtx The context.
	 * @param p0Element The element the children of which should be visited.
	 * @param sName The name of the child elements. If empty all are visited.
	 * @param oChildVisitor The visitor. Takes const xmlpp::Element* p0Element as parameter.
	 */
	template<class ChildVisitor>
	void visitNamedElementChildren(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sName, ChildVisitor oChildVisitor) const
	{
//std::cout << "visitNamedElementChildren sName=" << sName << '\n';
		oCtx.addChecker(p0Element);
		const bool bAllChildren = sName.empty();
		if (!bAllChildren) {
			oCtx.addValidChildElementName(p0Element, sName);
		}
		const xmlpp::Node::NodeList oChildren = p0Element->get_children(sName);
		for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
			const xmlpp::Element* p0ChildElement = dynamic_cast<const xmlpp::Element*>(*it);
			if (p0ChildElement == nullptr) {
				continue; // for (it -----
			}
			if (bAllChildren) {
				const auto sName = p0ChildElement->get_name();
				if ((sName == s_sConditionalExistIfNode) || (sName == s_sConditionalPassthroughIfNode) || (sName == s_sConditionalAttrIfNode)) {
					continue; // for (it -----
				}
				if (sName.substr(0, XmlCommonParser::s_sElementNameIgnorePrefix.size()) == XmlCommonParser::s_sElementNameIgnorePrefix) {
					continue; // for (it -----
				}
			}
			auto oTuple = parseOwnerExists(oCtx, p0ChildElement);
			const bool bOwnerExists = std::get<0>(oTuple);
			if (! bOwnerExists) {
				continue; // for (it -----
			}
			const xmlpp::Element* p0ExistIf = XmlCommonParser::parseUniqueElement(oCtx, p0ChildElement, s_sConditionalExistIfNode, false);
			if (p0ExistIf != nullptr) {
				// ExistIf logical OR of all children
				const bool bExists = evalCondition(oCtx, p0ExistIf);
				// ExistIf is not supposed to have attributes
				XmlCommonParser::checkAllAttributesNames(oCtx, p0ExistIf, [&](const std::string& /*sAttrName*/)
				{
					//TODO allow not="true" attribute to negate the whole ExistIf condition
					return false;
				});
				if (! bExists) {
					continue; // for (it -----
				}
			}
			bool bPassthrough = false;
			const xmlpp::Element* p0PassthroughIf = XmlCommonParser::parseUniqueElement(oCtx, p0ChildElement, s_sConditionalPassthroughIfNode, false);
			if (p0PassthroughIf != nullptr) {
				bPassthrough = evalCondition(oCtx, p0PassthroughIf);
				// PassthroughIf is not supposed to have attributes
				XmlCommonParser::checkAllAttributesNames(oCtx, p0PassthroughIf, [&](const std::string& /*sAttrName*/)
				{
					//TODO allow not="true" attribute to negate the whole PassthroughIf condition
					return false;
				});
			}
			if (! bPassthrough) {
				oChildVisitor(p0ChildElement);
			} else {
				visitNamedElementChildren(oCtx, p0ChildElement, sName, oChildVisitor);
			}
		}
		oCtx.removeChecker(p0Element, false, false);
	}
	/** Visit all children applying nested ExistIf conditions.
	 * Ignored elements are skipped.
	 * @param oCtx The context.
	 * @param p0Element The element the children of which should be visited.
	 * @param oChildVisitor The visitor. Takes const xmlpp::Element* p0Element as parameter.
	 */
	template<class ChildVisitor>
	void visitElementChildren(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, ChildVisitor oChildVisitor) const
	{
		visitNamedElementChildren(oCtx, p0Element, Util::s_sEmptyString, oChildVisitor);
	}
	/** Parse at most one child element with given name.
	 * @param oCtx The context.
	 * @param p0Parent The parent element. Cannot be null.
	 * @param sName The name of the child element. Cannot be empty.
	 * @param bMandatory Whether the child element must be present.
	 * @return The child element or null if not present.
	 */
	const xmlpp::Element* parseUniqueElement(ConditionalCtx& oCtx, const xmlpp::Element* p0Parent
											, const std::string& sName, bool bMandatory) const;
	/** Checks whether the names of all the child elements of a node satisfy a predicate.
	 * The function object must define `bool operator()(const std::string& sName)`.
	 * The `ExistIf` and `AttrIf` subelements are skipped.
	 * Ignored elements are skipped.
	 * If one predicate call returns false an invalid element exception is thrown.
	 * @param oCtx The context.
	 * @param p0Parent The parent node. Cannot be null.
	 * @param oP The function object (predicate).
	 */
	template <class P>
	void checkAllChildElementsName(ConditionalCtx& oCtx, const xmlpp::Node* p0Parent, P oP) const
	{
		assertTrue(p0Parent != nullptr);
		const xmlpp::Node::NodeList oChildren = p0Parent->get_children();
		for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
			const xmlpp::Element* p0ChildElement = dynamic_cast<const xmlpp::Element*>(*it);
			if (p0ChildElement == nullptr) {
				continue; // for (it -----
			}
			const std::string sName = p0ChildElement->get_name();
			if ((sName == s_sConditionalExistIfNode) || (sName == s_sConditionalPassthroughIfNode)  || (sName == s_sConditionalAttrIfNode)) {
				continue; // for (it -----
			}
			if (sName.substr(0, XmlCommonParser::s_sElementNameIgnorePrefix.size()) == XmlCommonParser::s_sElementNameIgnorePrefix) {
				continue; // for (it -----
			}
			if (! oP(sName)) {
				throw XmlCommonErrors::errorElementInvalid(oCtx, p0ChildElement, sName);
			}
		}
	}

	/** Get attribute applying nested AttrIf conditions.
	 * @param oCtx The context.
	 * @param p0Element The element the attribute of which should be retrieved. Cannot be null.
	 * @param sName The attribute name. Cannot be empty.
	 * @return Whether the attribute is defined and its value.
	 */
	std::pair<bool, std::string> getAttributeValue(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sName) const;
	/** Visit layout config dependant attributes applying nested ExistIf conditions.
	 * Layout config attributes are named by appending a '_' and the layout config
	 * number (&gt;= 0 and &lt; g_nTotLayoutConfigs).
	 *
	 * The first attribute to be tried is the one in sAttrName. After that
	 * sAttrName + '_0', sAttrName + '_1', and so on.
	 *
	 * The function object must define
	 * `void operator()(const std::string& sLCAttrName, const std::string& sValue, int32_t nLayoutConfig)`.
	 * Parameter nLayoutConfig can be -1 (which means attribute without '_N' is defined).
	 * Parameter sLCAttrName is sAttrName with possibly suffix '_N' (N is nLayoutConfig).
	 *
	 * @param oCtx The context.
	 * @param p0Element The element the attribute(s) of which should be visited.
	 * @param sAttrName The base name of the attribute. Cannot be empty.
	 * @param nTotLayoutConfigs The number of layout configurations (suffixes).
	 * @param oAttrVisitor The visitor.
	 */
	template<class AttrVisitor>
	void visitLayoutConfigNamedAttributes(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName
										, int32_t nTotLayoutConfigs, AttrVisitor oAttrVisitor) const
	{
		const auto oPair = getAttributeValue(oCtx, p0Element, sAttrName);
		if (oPair.first) {
			oAttrVisitor(sAttrName, oPair.second, -1);
		}
		for (int32_t nLC = 0; nLC < nTotLayoutConfigs; ++nLC) {
			const std::string sLCAttrName = sAttrName + "_" + std::to_string(nLC);
			const auto oLCPair = getAttributeValue(oCtx, p0Element, sLCAttrName);
			if (oLCPair.first) {
				oAttrVisitor(sLCAttrName, oLCPair.second, nLC);
			}
		}
	}
	/** Get enumeration attribute applying nested AttrIf conditions.
	 * @param oCtx The context.
	 * @param p0Element The element the attribute of which should be retrieved. Cannot be null.
	 * @param sAttrName The attribute name. Cannot be empty.
	 * @param aValues The valid enum strings.
	 * @return The chosen enum as an index into aValues or -1 if the attribute is not defined.
	 */
	int32_t getEnumAttributeValue(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName
								, const std::vector<char const *>& aValues) const;
	/** Visit layout config dependant attributes applying nested ExistIf conditions.
	 * Layout config attributes are named by appending a '_' and the layout config
	 * number (&gt;= 0 and &lt; g_nTotLayoutConfigs).
	 *
	 * The first attribute to be tried is the one in sAttrName. After that
	 * sAttrName + '_0', sAttrName + '_1', and so on.
	 *
	 * The function object must define
	 * `void operator()(const std::string& sLCAttrName, int32_t nEnum, int32_t nLayoutConfig)`.
	 * Parameter nLayoutConfig can be -1 (which means attribute without '_N' is defined).
	 * Parameter sLCAttrName is sAttrName with possibly suffix '_N' (N is nLayoutConfig).
	 * Parameter nEnum is the index into aValues.
	 *
	 * @param oCtx The context.
	 * @param p0Element The element the attribute(s) of which should be visited.
	 * @param sAttrName The base name of the attribute. Cannot be empty.
	 * @param nTotLayoutConfigs The number of layout configurations (suffixes).
	 * @param aValues The valid enum strings.
	 * @param oAttrVisitor The visitor.
	 */
	template<class AttrVisitor>
	void visitLayoutConfigNamedEnumAttributes(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName
											, int32_t nTotLayoutConfigs
											, const std::vector<char const *>& aValues, AttrVisitor oAttrVisitor) const
	{
		const int32_t nEnum = getEnumAttributeValue(oCtx, p0Element, sAttrName, aValues);
		if (nEnum >= 0) {
			oAttrVisitor(sAttrName, nEnum, -1);
		}
		for (int32_t nLC = 0; nLC < nTotLayoutConfigs; ++nLC) {
			const std::string sLCAttrName = sAttrName + "_" + std::to_string(nLC);
			const int32_t nLCEnum = getEnumAttributeValue(oCtx, p0Element, sLCAttrName, aValues);
			if (nLCEnum >= 0) {
				oAttrVisitor(sLCAttrName, nLCEnum, nLC);
			}
		}
	}
	/** Gets the team attribute.
	 * If the team is not defined as attribute the context team is returned.
	 * @param oCtx The context.
	 * @param p0Element The element
	 * @return Pair(Whether the team exists according to the preferences, the team).
	 * @throws If the team cannot be determined.
	 */
	std::pair<bool, int32_t> parseTeamExists(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	/** Parse the owner of an element.
	 * If the owner is the game, returns (true, -1, -1).
	 * If the owner is a team, returns (true, nTeam, -1) or (false, nTeam, -1) if the team doesn't exist.
	 * If the owner is a player, returns (true, nTeam, nMate) or (false, nTeam, nMate) if the player doesn't exist.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @return Whether the owner exists, the owning team and the owning mate (bExists, nTeam, nMate).
	 */
	std::tuple<bool, int32_t, int32_t> parseOwnerExists(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	/** The owner of an element (for a required owner type).
	 * If the owner doesn't exist this function throws.
	 *
	 * If the required owner type is OwnerType::PLAYER but the player isn't defined either directly or
	 * by the context the function throws.
	 *
	 * If the required owner type is OwnerType::TEAM but the team isn't defined either directly or by the context
	 * the function throws. Note that if the element belongs to a player the function succeeds and also sets the
	 * mate value.
	 *
	 * If the required owner type is OwnerType::GAME the owner (whatever it is) is returned.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param eRequireOwnerType The owner type.
	 * @return The team and mate.
	 */
	std::pair<int32_t, int32_t> parseOwner(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, OwnerType eRequireOwnerType) const;
	/** The owner of an element.
	 * Same as parseOwner(oCtx, p0Element, OwnerType::GAME).
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @return The team and mate.
	 */
	std::pair<int32_t, int32_t> parseOwner(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	/** The owner type.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param sAttrName The attribute name containing the type. If empty the default attribute name ("ownerType") is used.
	 * @return Whether the attribute exists and the owner type value ("GAME", "TEAM" or "PLAYER").
	 */
	std::pair<bool, OwnerType> parseOwnerType(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName) const;

	/** Parse int set attribute(s).
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param sSingleAttr The single value attribute. Cannot be empty.
	 * @param sFromAttr The from value attribute. Cannot be empty.
	 * @param sToAttr The to value attribute. Cannot be empty.
	 * @param sMultiAttr The multi value attribute. Cannot be empty.
	 * @param sSeparatorAttr The separator within the multi value. Can be empty (default is used).
	 * @param bMin Whether the value(s) have a minimum.
	 * @param nMin The minimum for each int value. Only applied if bMin is true.
	 * @param bMax Whether the value(s) have a maximum.
	 * @param nMax The maximum for each int value. Only applied if bMax is true.
	 * @return Whether the set is defined and the int set itself.
	 */
	std::pair<bool, IntSet> parseIntSetAttributes(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
												, const std::string& sSingleAttr, const std::string& sFromAttr, const std::string& sToAttr
												, const std::string& sMultiAttr, const std::string& sSeparatorAttr
												, bool bMin, int32_t nMin, bool bMax, int32_t nMax) const;
	/** Parse single or from-to pair attribute.
	 * If bMin is false oMin is overridden to std::numeric_limits<T>::lowest().
	 * If bMax is false oMax is overridden to std::numeric_limits<T>::max().
	 * If both true, oMax must not be smaller than oMin.
	 *
	 * If no attribute is defined, the values oFrom and oTo are untouched if
	 * their current value satisfies the optional min and max conditions. Otherwise
	 * they are clamped.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param sSingleAttr The single value attribute. Cannot be empty.
	 * @param sFromAttr The from value attribute. Cannot be empty.
	 * @param sToAttr The to value attribute. Cannot be empty.
	 * @param bMandatory If true and no attribute is defined throws exception.
	 * @param bMin If true any of the values is smaller than oMin throws an exception. If false it is as if std::numeric_limits<T>::lowest() was passed to oMin.
	 * @param oMin The minimum value. Only is considered if bMin is set.
	 * @param bMax If true any of the values is bigger than oMax throws an exception. If false it is as if std::numeric_limits<T>::max() was passed to oMax.
	 * @param oMax The maximum value. Only is considered if bMax is set.
	 * @param oFrom The resulting 'from' value of the range. If single attribute is defined equals oTo.
	 * @param oTo The resulting 'to' value of the range. If single attribute is defined equals oFrom.
	 * @return Whether any of attributes is defined.
	 */
	template<typename T>
	bool parseAttributeFromTo(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
							, const std::string& sSingleAttr, const std::string& sFromAttr, const std::string& sToAttr
							, bool bMandatory, bool bMin, T oMin, bool bMax, T oMax, T& oFrom, T& oTo) const
	{
		if (bMin && bMax) {
			assertTrue(oMin <= oMax);
		}
		auto oTempFrom = (bMin ? oMin : std::numeric_limits<T>::lowest());
		auto oTempTo = (bMax ? oMax : std::numeric_limits<T>::max());
		const auto oPairsSingle = getAttributeValue(oCtx, p0Element, sSingleAttr);
		const bool bSingleDefined = oPairsSingle.first;
		if (bSingleDefined) {
			const std::string& sSingle = oPairsSingle.second;
			oTempFrom = XmlUtil::strToNumber<T>(oCtx, p0Element, sSingleAttr, sSingle, false, bMin, oMin, bMax, oMax);
			oTempTo = oTempFrom;
		}
		const auto oPairFrom = getAttributeValue(oCtx, p0Element, sFromAttr);
		const bool bFromDefined = oPairFrom.first;
		if (bFromDefined) {
			if (bSingleDefined) {
				throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sFromAttr, sSingleAttr);
			}
			const std::string& sFrom = oPairFrom.second;
			oTempFrom = XmlUtil::strToNumber<T>(oCtx, p0Element, sFromAttr, sFrom, false, bMin, oMin, bMax, oMax);
		}
		const auto oPairTo = getAttributeValue(oCtx, p0Element, sToAttr);
		const bool bToDefined = oPairTo.first;
		if (bToDefined) {
			if (bSingleDefined) {
				throw XmlCommonErrors::errorAttrAlreadyDefinedByAnother(oCtx, p0Element, sToAttr, sSingleAttr);
			}
			const std::string& sTo = oPairTo.second;
			oTempTo = XmlUtil::strToNumber<T>(oCtx, p0Element, sToAttr, sTo, false, bMin, oMin, bMax, oMax);
		}
		if (oTempFrom > oTempTo) {
			throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
					"Attribute '%1' cannot be bigger than '%2'", sFromAttr, sToAttr));
		}
		const bool bDefined = (bSingleDefined || bFromDefined || bToDefined);
		if (bMandatory && !bDefined) {
			throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
					"Either attribute '%1', '%2' or '%3' have to be defined", sSingleAttr, sFromAttr, sToAttr));
		}
		if (bDefined) {
			oFrom = std::move(oTempFrom);
			oTo = std::move(oTempTo);
		} else {
			if (bMin && (oFrom < oMin)) {
				oFrom = oMin;
				if (oTo < oFrom) {
					oTo = oFrom;
				}
			}
			if (bMax && (oTo > oMax)) {
				oTo = oMax;
				if (oFrom > oTo) {
					oFrom = oTo;
				}
			}
		}
		return bDefined;
	}

private:
	bool parseIntSetAttributes(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
								, const std::string& sMultiAttr, const std::string& sSeparatorAttr
								, IntSet& oIntSet
								, const std::string& sAlreadyAttr, const std::string& sAlreadyAttr2
								, bool bMin, int32_t nMin, bool bMax, int32_t nMax) const;
	//
	bool evalConditionOption(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	bool evalConditionTotMates(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	bool evalConditionTrue(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	bool evalConditionFalse(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	bool evalConditionTraverse(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	bool evalConditionAnd(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	bool evalConditionOr(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, bool bCheckAttrs) const;
	bool evalConditionValue(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, int32_t nValue) const;
	bool evalCondition(ConditionalCtx& oCtx, const xmlpp::Element* p0Element) const;
	//
	static std::runtime_error errorElementCannotDetermineTeamFromContext(ParserCtx& oCtx, const xmlpp::Element* p0Element);
	static std::runtime_error errorElementCannotDeterminePlayerFromContext(ParserCtx& oCtx, const xmlpp::Element* p0Element);

	static void assertTrue(bool bCond);
private:
	XmlVariantSetParser& m_oXmlVariantSetParser;
private:
	XmlConditionalParser(const XmlConditionalParser& oSource) = delete;
	XmlConditionalParser& operator=(const XmlConditionalParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_CONDITIONAL_PARSER_H */

