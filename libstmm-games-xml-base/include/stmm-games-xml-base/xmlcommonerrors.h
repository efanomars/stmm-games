/*
 * File:   xmlcommonerrors.h
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

#ifndef STMG_XML_COMMON_ERRORS_H
#define STMG_XML_COMMON_ERRORS_H

#include <stdexcept>
#include <string>

namespace stmg { class ParserCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

////////////////////////////////////////////////////////////////////////////////
class XmlCommonErrors
{
public:
	/** Create error exception.
	 * Adds line number and context.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param sAttr The attribute: Can be empty.
	 * @param sErr The main error string.
	 * @return The exception.
	 */
	static std::runtime_error error(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr, const std::string& sErr);
	static std::runtime_error errorAttrCannotBothBeDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr, const std::string& sOtherAttr);
	static std::runtime_error errorAttrAlreadyDefinedByAnother(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr, const std::string& sOtherAttr);
	static std::runtime_error errorAttrAlreadyDefinedByOthers(ParserCtx& oCtx, const xmlpp::Element* p0Element
																, const std::string& sAttr, const std::string& sOtherAttr, const std::string& sOtherAttr2);
	static std::runtime_error errorAttrWithValueAlreadyDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr, const std::string& sValue);
	static std::runtime_error errorAttrWithValueNotDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr, const std::string& sValue);
	static std::runtime_error errorAttrCannotBeEmpty(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr);
	static std::runtime_error errorAttrHexColorFormat(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr);
	static std::runtime_error errorAttrOneTooManyNames(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr);
	static std::runtime_error errorAttrNotWellFormedNumber(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr);
	static std::runtime_error errorAttrNotWellFormedName(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr);
	static std::runtime_error errorAttrUTF8CharInvalid(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr, const std::string& sValue);
	static std::runtime_error errorAttrCannotHaveEmptyValue(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr);
	static std::runtime_error errorAttrCannotDefineRangeOfNames(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr);
	static std::runtime_error errorAttrNotFound(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr);
	static std::runtime_error errorAttrInvalidValue(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr);
	static std::runtime_error errorAttrVariableNotDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr, const std::string& sVarName);
	static std::runtime_error errorAttrEitherMustBeDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName1, const std::string& sAttrName2);
	static std::runtime_error errorAttrEitherMustBeDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName1
															, const std::string& sAttrName2, const std::string& sAttrName3);
	static std::runtime_error errorAttrEitherMustBeDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName1
															, const std::string& sAttrName2, const std::string& sAttrName3, const std::string& sAttrName4);
	static std::runtime_error errorElementExpected(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sElementName);
	static std::runtime_error errorElementEitherMustBeDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sElementName1, const std::string& sElementName2);
	static std::runtime_error errorElementTooMany(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sElementName);
	static std::runtime_error errorElementInvalid(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sElementName);
	static std::runtime_error errorElementCannotBothBeDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sElementName
															, const std::string& sElementName2);
};

} // namespace stmg

#endif	/* STMG_XML_COMMON_ERRORS_H */

