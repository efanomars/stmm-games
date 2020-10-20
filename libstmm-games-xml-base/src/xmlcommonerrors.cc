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
 * File:   xmlcommonerrors.cc
 */

#include "xmlcommonerrors.h"

#include "parserctx.h"

#include <stmm-games/util/util.h>

//#include <cassert>
//#include <iostream>

#include <stdint.h>

namespace xmlpp { class Element; }

namespace stmg
{

std::runtime_error XmlCommonErrors::error(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr, const std::string& sErr)
{
	const int32_t nLine = oCtx.getLine(p0Element, sAttr);
	return oCtx.error(Util::stringCompose("Error%1: %2"
					, ((nLine >= 0) ? (" at line " + std::to_string(nLine)) : "") , sErr));
}
std::runtime_error XmlCommonErrors::errorAttrCannotBothBeDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
																, const std::string& sAttr2)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1' and '%2' cannot both be defined", sAttr, sAttr2));
}
std::runtime_error XmlCommonErrors::errorAttrAlreadyDefinedByAnother(ParserCtx& oCtx, const xmlpp::Element* p0Element
														, const std::string& sAttr, const std::string& sOtherAttr)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1' already defined by attribute '%2'"
															, sAttr, sOtherAttr));
}
std::runtime_error XmlCommonErrors::errorAttrAlreadyDefinedByOthers(ParserCtx& oCtx, const xmlpp::Element* p0Element
														, const std::string& sAttr, const std::string& sOtherAttr, const std::string& sOtherAttr2)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1' already defined by attributes '%2'-'%3'"
															, sAttr, sOtherAttr, sOtherAttr2));
}
std::runtime_error XmlCommonErrors::errorAttrWithValueAlreadyDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
																	, const std::string& sValue)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose(
					"Attribute '%1' with value '%2' already defined", sAttr, sValue));
}
std::runtime_error XmlCommonErrors::errorAttrWithValueNotDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
																	, const std::string& sValue)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose(
					"Attribute '%1' with value '%2' not defined", sAttr, sValue));
}
std::runtime_error XmlCommonErrors::errorAttrCannotBeEmpty(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1' cannot be empty", sAttr));
}
std::runtime_error XmlCommonErrors::errorAttrHexColorFormat(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1' must have"
					" format #RRGGBB where R,G and B are hex numbers (0123456789ABCDEF)", sAttr));
}
std::runtime_error XmlCommonErrors::errorAttrOneTooManyNames(ParserCtx& oCtx, const xmlpp::Element* p0Element
																, const std::string& sAttr)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1' created one too many names", sAttr));
}
std::runtime_error XmlCommonErrors::errorAttrNotWellFormedNumber(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1' not a well formed number", sAttr));
}
std::runtime_error XmlCommonErrors::errorAttrNotWellFormedName(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1' not a well formed name", sAttr));
}
std::runtime_error XmlCommonErrors::errorAttrUTF8CharInvalid(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
															, const std::string& sValue)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1' contains invalid UTF8 character %2"
																, sAttr, sValue));
}
std::runtime_error XmlCommonErrors::errorAttrCannotHaveEmptyValue(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1' cannot have empty value", sAttr));
}
std::runtime_error XmlCommonErrors::errorAttrCannotDefineRangeOfNames(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1' can't define range of names", sAttr));
}
std::runtime_error XmlCommonErrors::errorAttrNotFound(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose("expected attribute '%1' not found", sAttr));
}
std::runtime_error XmlCommonErrors::errorAttrInvalidValue(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1' has invalid value", sAttr));
}
std::runtime_error XmlCommonErrors::errorAttrVariableNotDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
														, const std::string& sVarName)
{
	return error(oCtx, p0Element, sAttr, Util::stringCompose("attribute '%1':\n variable named %2 not defined"
																			, sAttr, sVarName));
}
std::runtime_error XmlCommonErrors::errorElementExpected(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sElementName)
{
	return error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("expected subelement '%1' not found", sElementName));
}
std::runtime_error XmlCommonErrors::errorElementEitherMustBeDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element
																	, const std::string& sElementName1, const std::string& sElementName2)
{
	return error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("either element '%1' or '%2' must be defined"
																			, sElementName1, sElementName2));
}
std::runtime_error XmlCommonErrors::errorAttrEitherMustBeDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element
																	, const std::string& sAttrName1, const std::string& sAttrName2)
{
	return error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("either attribute '%1' or '%2' must be defined"
																			, sAttrName1, sAttrName2));
}
std::runtime_error XmlCommonErrors::errorAttrEitherMustBeDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName1
																, const std::string& sAttrName2, const std::string& sAttrName3)
{
	return error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("either attribute '%1', '%2' or '%3' must be defined"
																			, sAttrName1, sAttrName2, sAttrName3));
}
std::runtime_error XmlCommonErrors::errorAttrEitherMustBeDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName1
																, const std::string& sAttrName2, const std::string& sAttrName3, const std::string& sAttrName4)
{
	return error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("either attribute '%1', '%2', '%3' or '%4' must be defined"
																			, sAttrName1, sAttrName2, sAttrName3, sAttrName4));
}
std::runtime_error XmlCommonErrors::errorElementTooMany(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sElementName)
{
	return error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("Too many node elements '%1'", sElementName));
}
std::runtime_error XmlCommonErrors::errorElementInvalid(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sElementName)
{
	return error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("Invalid element '%1'", sElementName));
}
std::runtime_error XmlCommonErrors::errorElementCannotBothBeDefined(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sElementName
																	, const std::string& sElementName2)
{
	return error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("Elements '%1' and '%2' cannot both be defined", sElementName, sElementName2));
}

} // namespace stmg
