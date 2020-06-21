/*
 * File:   xmlcommonparser.h
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

#ifndef STMG_XML_COMMON_PARSER_H
#define STMG_XML_COMMON_PARSER_H

#include "parserctx.h"
#include "xmlcommonerrors.h"
#include "xmlutil/xmlstrconv.h"

#include <stmm-games/util/util.h>

#include <libxml++/libxml++.h>

#include <utility>
#include <limits>
#include <list>
#include <string>

#include <stdint.h>

namespace stmg { class IntSet; }

namespace stmg
{

////////////////////////////////////////////////////////////////////////////////
class XmlCommonParser
{
public:
	static const std::string s_sAttrNameIgnorePrefix;
	static const std::string s_sElementNameIgnorePrefix;

	/** Tells whether a string is a valid name.
	 * @param oCtx The parser context. Used to throw.
	 * @param p0Element The element. Can be null.
	 * @param sAttrName The attribute name the value of which is checked.
	 * @param sName The UTF8 name.
	 * @param bAllowAny Whether to allow "*" (any) name.
	 * @return Whether it is the "*" (any) name.
	 * @throws If name not valid.
	 */
	static bool validateName(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName
							, const std::string& sName, bool bAllowAny);

	static const xmlpp::Element* parseUniqueElement(ParserCtx& oCtx, const xmlpp::Element* p0Parent
													, const std::string& sName, bool bMandatory);

	static std::pair<bool, std::string> getAttributeValue(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr0);
	static std::pair<bool, std::string> getAttributeValue(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr0, const std::string& sAttr1);
	/** Parse for value separator string.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param sSeparatorAttr The separator attribute. If empty the default separator is returned.
	 * @return The separator string. Is not empty.
	 */
	static std::string parseSeparator(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sSeparatorAttr);
	/** Parse IntSet attributes.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param sSingleAttr The single attribute name.
	 * @param sFromAttr The 'from' range attribute name.
	 * @param sToAttr The 'to' range attribute name.
	 * @param sStepAttr The 'step' attribute name.
	 * @param sMultiAttr The attribute name containing (usually) comma separated values.
	 * @param sSeparatorAttr The separator string. Cannot contain spaces. If empty comma is used.
	 * @param bMin Whether the range has a minimum.
	 * @param nMin The minimum value, if bMin is true.
	 * @param bMax Whether the range has a maximum.
	 * @param nMax The maximum value, if bMax is true.
	 * @return Whether one of the attributes was defined and the resulting IntSet.
	 */
	static std::pair<bool, IntSet> parseIntSetAttrs(ParserCtx& oCtx, const xmlpp::Element* p0Element
									, const std::string& sSingleAttr
									, const std::string& sFromAttr, const std::string& sToAttr, const std::string& sStepAttr
									, const std::string& sMultiAttr, const std::string& sSeparatorAttr
									, bool bMin, int32_t nMin, bool bMax, int32_t nMax);

	/** Parse from to attributes.
	 * If bMin is false oMin is overridden to std::numeric_limits<T>::lowest().
	 * If bMax is false oMax is overridden to std::numeric_limits<T>::max().
	 * If both true, oMin must not be smaller than oMin.
	 *
	 * If no attribute is defined, the values oFrom and oTo are untouched if
	 * their current value satisfies the optional min and max conditions. Otherwise
	 * they are clamped to (oMin, oMax).
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param sSingleAttr The single attribute name.
	 * @param sFromAttr The 'from' attribute name.
	 * @param sToAttr The 'to' attribute name.
	 * @param bMandatory Whether one of the attributes must be defined.
	 * @param bMin Whether the range has a minimum.
	 * @param oMin The minimum value, if bMin is true.
	 * @param bMax Whether the range has a maximum.
	 * @param oMax The maximum value, if bMax is true.
	 * @param oFrom The resulting 'from' value. Untouched if no attribute defined.
	 * @param oTo The resulting 'to' value. Untouched if no attribute defined.
	 * @return Whether one of the attributes was defined.
	 */
	template<typename T>
	static bool parseAttrFromTo(ParserCtx& oCtx, const xmlpp::Element* p0Element
							, const std::string& sSingleAttr, const std::string& sFromAttr, const std::string& sToAttr
							, bool bMandatory, bool bMin, T oMin, bool bMax, T oMax, T& oFrom, T& oTo)
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
	template<typename T>
	static bool parseAttrFromToClamp(ParserCtx& oCtx, const xmlpp::Element* p0Element
							, const std::string& sSingleAttr, const std::string& sFromAttr, const std::string& sToAttr
							, bool bMandatory, bool bMin, T oMin, bool bMax, T oMax, T& oFrom, T& oTo)
	{
		auto oTempFrom = oFrom;
		auto oTempTo = oTo;
		const bool bDefined = parseAttrFromTo<T>(oCtx, p0Element, sSingleAttr, sFromAttr, sToAttr
												, bMandatory, false, oMin, false, oMax, oTempFrom, oTempTo);
		if (bMin && (oTempFrom < oMin)) {
			oTempFrom = oMin;
		}
		if (bMax && (oTempTo > oMax)) {
			oTempTo = oMax;
		}
		if (oTempTo < oTempFrom) {
			throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
					"Attributes '%1', '%2' or '%3' incompatible with (%4,%5) constraint", sSingleAttr, sFromAttr, sToAttr
												, std::to_string(oFrom), std::to_string(oTo)));
		}
		oFrom = std::move(oTempFrom);
		oTo = std::move(oTempTo);
		return bDefined;
	}
	/** Visits all child elements of a node that have a certain name.
	 * The function object must define `operator()(const xmlpp::Element* p0ChildElement)`.
	 * If sName is empty, ignored elements are skipped.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param sName The name. If empty all children are visited.
	 * @param oChildVisitor The function object taking a (child) element as parameter.
	 */
	template<class ChildVisitor>
	static void visitNamedElementChildren(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sName, ChildVisitor oChildVisitor)
	{
		oCtx.addChecker(p0Element);
		const bool bAllChildren = sName.empty();
		if (!bAllChildren) {
			oCtx.addValidChildElementName(p0Element, sName);
		}
		const xmlpp::Node::NodeList oChildren = p0Element->get_children(sName);
		for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
			const xmlpp::Element* p0ItemElement = dynamic_cast<const xmlpp::Element*>(*it);
			if (p0ItemElement == nullptr) {
				continue; // for (it -----
			}
			const std::string sItemName = p0ItemElement->get_name();
			if (bAllChildren) {
				if (sItemName.substr(0, s_sElementNameIgnorePrefix.size()) == s_sElementNameIgnorePrefix) {
					continue; // for (it -----
				}
			}
			oChildVisitor(p0ItemElement);
		}
		oCtx.removeChecker(p0Element, false);
	}
	/** Visits all child elements of a node.
	 * The function object must define `operator()(const xmlpp::Element* p0ChildElement)`.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param oChildVisitor The function object taking a (child) element as parameter.
	 */
	template<class ChildVisitor>
	static void visitElementChildren(ParserCtx& oCtx, const xmlpp::Element* p0Element, ChildVisitor oChildVisitor)
	{
		visitNamedElementChildren(oCtx, p0Element, Util::s_sEmptyString, oChildVisitor);
	}
	/** Checks whether the names of all the child elements of a node satisfy a predicate.
	 * The function object must define `bool operator()(const std::string& sName)`.
	 * If one predicate call returns false an invalid element exception is thrown.
	 * Ignored elements are skipped.
	 * @param oCtx The context.
	 * @param p0Parent The parent node. Cannot be null.
	 * @param oP The function object (predicate).
	 */
	template <class P>
	static void checkAllChildElementsName(ParserCtx& oCtx, const xmlpp::Node* p0Parent, P oP)
	{
		assertTrue(p0Parent != nullptr);
		const xmlpp::Node::NodeList oChildren = p0Parent->get_children();
		for (xmlpp::Node::NodeList::const_iterator it = oChildren.begin(); it != oChildren.end(); ++it) {
			const xmlpp::Element* p0ChildElement = dynamic_cast<const xmlpp::Element*>(*it);
			if (p0ChildElement == nullptr) {
				continue; // for (it -----
			}
			const std::string sName = p0ChildElement->get_name();
			if (sName.substr(0, s_sElementNameIgnorePrefix.size()) == s_sElementNameIgnorePrefix) {
				continue; // for (it -----
			}
			if (! oP(sName)) {
				throw XmlCommonErrors::errorElementInvalid(oCtx, p0ChildElement, sName);
			}
		}
	}
	/** Checks whether all the attributes of an element have a name that satisfies a predicate.
	 * The function object must define `bool operator()(const std::string& sName)`.
	 *
	 * Attributes that start with s_sAttrNameIgnorePrefix ("_ign_") are ignored.
	 * This can be useful for testing purposes.
	 *
	 * If one predicate call returns false an invalid attribute exception is thrown.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param oP The function object (predicate).
	 */
	template <class P>
	static void checkAllAttributesNames(ParserCtx& oCtx, const xmlpp::Element* p0Element, P oP)
	{
		assertTrue(p0Element!= nullptr);
		const xmlpp::Element::AttributeList oAttributes = p0Element->get_attributes();
		for (xmlpp::Element::AttributeList::const_iterator it = oAttributes.begin(); it != oAttributes.end(); ++it) {
			const xmlpp::Attribute* p0Attr = *it;
			if (p0Attr == nullptr) {
				continue; // for (it -----
			}
			const std::string sName = p0Attr->get_name();
			if (sName.substr(0, s_sAttrNameIgnorePrefix.size()) == s_sAttrNameIgnorePrefix) {
				continue; // for (it -----
			}
			if (! oP(sName)) {
				throw XmlCommonErrors::error(oCtx, p0Element, sName, Util::stringCompose("Attribute %1: invalid", sName));
			}
		}
	}
private:
	static void assertTrue(bool bCond);
};

} // namespace stmg

#endif	/* STMG_XML_COMMON_PARSER_H */

