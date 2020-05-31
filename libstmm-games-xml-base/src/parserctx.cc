/*
 * File:   parserctx.cc
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

#include "parserctx.h"

#include "xmlcommonparser.h"

#include <glibmm/ustring.h>

#include <libxml++/libxml++.h>

#include <cassert>
#include <iostream>
#include <exception>

namespace stmg
{

ParserCtx::~ParserCtx()
{
	for (const auto& oChecker : m_aCheckers) {
		assert(oChecker.m_p0Element != nullptr);
		std::cout << "Unclosed Checker: element: " << oChecker.m_p0Element->get_name()
					<< "  line: " << oChecker.m_p0Element->get_line() << '\n';
	}
	#ifndef NDEBUG
	//TODO to be changed in c++17 to:  if (std::uncaught_exceptions() == 0)
	if (!(std::uncaught_exception())) {
		assert(m_aCheckers.empty());
	}
	#endif //NDEBUG
}
#ifndef NDEBUG
void ParserCtx::dump() const
{
	std::cout << "ParserCtx::dump():" << '\n';
	for (const auto& oChecker : m_aCheckers) {
		std::cout << "  -> unclosed Checker: element: " << oChecker.m_p0Element->get_name()
					<< " (" << oChecker.m_nCounter << ")"
					<< "  line: " << oChecker.m_p0Element->get_line() << '\n';
	}
}
#endif //NDEBUG

void ParserCtx::addChecker(const xmlpp::Element* p0Element)
{
//std::cout << "ParserCtx::addChecker    adr: " << reinterpret_cast<int64_t>(p0Element) << "  name: " << p0Element->get_name();
	auto itFindChecker = getChecker(p0Element);
	if (itFindChecker == m_aCheckers.end()) {
		m_aCheckers.resize(m_aCheckers.size() + 1);
		Checker& oChecker = m_aCheckers.back();
		oChecker.m_p0Element = p0Element;
		oChecker.m_nCounter = 1;
//std::cout << "  count: 1   size=" << m_aCheckers.size() << '\n';
	} else {
		Checker& oChecker = *itFindChecker;
		++oChecker.m_nCounter;
//std::cout << "  count: " << oChecker.m_nCounter << '\n';
	}
}
void ParserCtx::addValidChildElementName(const xmlpp::Element* p0Element, const std::string& sChildElementName)
{
	if (sChildElementName.empty()) {
		return;
	}
	auto itFindChecker = getChecker(p0Element);
	assert(itFindChecker != m_aCheckers.end());
	Checker& oChecker = *itFindChecker;
	auto itFindName = std::find(oChecker.m_aValidChildrenNames.begin(), oChecker.m_aValidChildrenNames.end(), sChildElementName);
	if (itFindName == oChecker.m_aValidChildrenNames.end()) {
		oChecker.m_aValidChildrenNames.push_back(sChildElementName);
	}
}
void ParserCtx::addValidAttrName(const xmlpp::Element* p0Element, const std::string& sAttrName)
{
	if (sAttrName.empty()) {
		return;
	}
	auto itFindChecker = getChecker(p0Element);
	assert(itFindChecker != m_aCheckers.end());
	Checker& oChecker = *itFindChecker;
	auto itFindName = std::find(oChecker.m_aValidAttrNames.begin(), oChecker.m_aValidAttrNames.end(), sAttrName);
	if (itFindName == oChecker.m_aValidAttrNames.end()) {
		oChecker.m_aValidAttrNames.push_back(sAttrName);
	}
}
void ParserCtx::removeChecker(const xmlpp::Element* p0Element, bool bCheckChildElements, bool bCheckAttrs)
{
	auto itFindChecker = getChecker(p0Element);
	assert(itFindChecker != m_aCheckers.end());
	Checker& oChecker = *itFindChecker;
//std::cout << "ParserCtx::removeChecker adr: " << reinterpret_cast<int64_t>(p0Element) << "  name: " << p0Element->get_name();
	--oChecker.m_nCounter;
//std::cout  << "  count: " << oChecker.m_nCounter;
	oChecker.m_bCheckChildrenElements = oChecker.m_bCheckChildrenElements || bCheckChildElements;
	oChecker.m_bCheckAttrs = oChecker.m_bCheckAttrs || bCheckAttrs;
	if (oChecker.m_nCounter == 0) {
		if (oChecker.m_bCheckChildrenElements) {
			XmlCommonParser::checkAllChildElementsName(*this, p0Element, [&](const std::string& sChildElementName)
			{
				const auto& aNames = oChecker.m_aValidChildrenNames;
				auto itFindName = std::find(aNames.begin(), aNames.end(), sChildElementName);
				return (itFindName != aNames.end());
			});
		}
		if (oChecker.m_bCheckAttrs) {
			XmlCommonParser::checkAllAttributesNames(*this, p0Element, [&](const std::string& sAttrName)
			{
				const auto& aNames = oChecker.m_aValidAttrNames;
				auto itFindName = std::find(aNames.begin(), aNames.end(), sAttrName);
				return (itFindName != aNames.end());
			});
		}
		m_aCheckers.erase(itFindChecker);
//std::cout << "   REMOVING -> size=" << m_aCheckers.size();
	}
//std::cout << '\n';
}

std::string ParserCtx::err(const std::string& s)
{
	std::string sShow;
	for (const auto& sCtx : m_oStack) {
		sShow.append(sCtx);
	}
	sShow.append(s);
	return sShow;
}
int32_t ParserCtx::getLine(const xmlpp::Element* p0Element, const std::string& sAttr)
{
	int32_t nLine = -1;
	if (p0Element != nullptr) {
		xmlpp::Attribute* p0Attr = p0Element->get_attribute(sAttr);
		nLine = ((p0Attr != nullptr) ? p0Attr->get_line() : p0Element->get_line());
	}
	return nLine;
}
std::runtime_error ParserCtx::error(const std::string& sErr)
{
	return std::runtime_error(err(sErr));
}

} // namespace stmg
