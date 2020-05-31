/*
 * File:   xmlstrconv.h
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

#ifndef STMG_XML_STR_CONV_H
#define STMG_XML_STR_CONV_H

#include "../xmlcommonerrors.h"

#include <stmm-games/tile.h>
#include <stmm-games/util/util.h>
#include <stmm-games/util/basictypes.h>

#include <iosfwd>
#include <string>
#include <utility>

#include <stdint.h>

namespace stmg { class ParserCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

namespace XmlUtil
{

/** Non ODR breaking assert.
 * Use this instead of the assert macro in header files (usually templates).
 * @param bCond The condition to assert.
 */
void assertTrue(bool bCond);

/** Convert a string to a number.
 *
 * @param oCtx The parser context.
 * @param p0Element The element of the attribute. Cannot be null.
 * @param sAttrName The attribute name. Cannot be empty.
 * @param sNr The string value.
 * @param bAllowPerc Whether to allow a trailing '%' character, in which case the preceding number is divided by 100.
 * @param bMin Whether the result must be bigger or equal oMin.
 * @param oMin The minimum value. Only used if bMin is true.
 * @param bMax Whether the result must be smaller or equal oMax.
 * @param oMax The maximum value. Only used if bMax is true.
 * @return The result value.
 * @throws If number incorrect or not in the specified range.
 */
template<typename T>
T strToNumber(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName
					, const std::string& sNr, bool bAllowPerc, bool bMin, T oMin, bool bMax, T oMax)
{
	auto oPair = Util::strToNumber<T>(sNr, bAllowPerc, bMin, oMin, bMax, oMax);
	std::string& sError = oPair.second;
	if (! sError.empty()) {
		throw XmlCommonErrors::error(oCtx, p0Element, sAttrName, (sAttrName.empty() ? "" : "Attribute '" + sAttrName + "': ") + sError);
	}
	return oPair.first;
}
/** Convert a string to a number in a range.
 * If the number has a trailing '%' it is parsed as a double and must be &gt;= 0 and &lt;= 100.
 * In this case the result is `oMin + fPerc * (oMax - oMin)`.
 *
 * @param oCtx The parser context.
 * @param p0Element The element of the attribute. Cannot be null.
 * @param sAttrName The attribute name. Cannot be empty.
 * @param sNr The string value.
 * @param oMin The minimum value.
 * @param oMax The maximum value.
 * @return The result value.
 * @throws If number incorrect or not in the specified range.
 */
template<typename T>
T strToRangeNumber(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName
						, const std::string& sNr, T oMin, T oMax)
{
	auto oPair = Util::strToRangeNumber<T>(sNr, oMin, oMax);
	std::string& sError = oPair.second;
	if (! sError.empty()) {
		throw XmlCommonErrors::error(oCtx, p0Element, sAttrName, (sAttrName.empty() ? "" : "Attribute '" + sAttrName + "': ") + sError);
	}
	return oPair.first;
}
/** Converts a string to a boolean value.
 * The string can be "t", "T", "true", "True", "TRUE", "y", "Y", "yes", "Yes", "YES" or
 * "f", "F", "false", "False", "FALSE", "n", "N", "no", "No", "NO".
 * @param oCtx The parser context.
 * @param p0Element The element of the attribute. Cannot be null.
 * @param sAttrName The attribute name. Cannot be empty.
 * @param sTF The string value.
 * @return The result value.
 * @throws If string not valid.
 */
bool strToBool(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName, const std::string& sTF);

/** Converts a string to an unsigned integer.
 * The string can contain the hexadecimal representation, in which case the string
 * must have leading characters "0x". No whitespace is allowed.
 *
 * Examples: valid strings "42", "0xA6C". Invalid strings: " 0xA6C", "0x A6C"
 * @param oCtx The parser context.
 * @param p0Element The element of the attribute. Cannot be null.
 * @param sAttrName The attribute name. Cannot be empty.
 * @param sUint The string value.
 * @param bMin Whether the result must be bigger or equal nMin.
 * @param nMin The minimum value. Only used if bMin is true.
 * @param bMax Whether the result must be smaller or equal nMax.
 * @param nMax The maximum value. Only used if bMax is true.
 * @return The result value.
 * @throws If number incorrect or not in the specified range.
 */
uint32_t strToUint32(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName
							, const std::string& sUint, bool bMin, uint32_t nMin, bool bMax, uint32_t nMax);

TileChar strToTileChar(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName
							, const std::string& sChar);

/** Convert string to tile color.
	* Expects the string to be stripped of leading and trailing whitespace.
	* @param oCtx The parsing context.
	* @param p0Element The element. Can be null.
	* @param sAttrName The attribute name.
	* @param sColor The string. Cannot be empty.
	* @return The tile color. 
	* @throws If error.
	*/
TileColor strToTileColor(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName, const std::string& sColor);

RgbColor strHexToRGB(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrName, const std::string& sHexNr);

/** Separates a string into tokens and passes them to a handler.
	* The tokens are stripped of leading and trailing spaces and can be empty.
	* @param sStr The string to tokenize.
	* @param sSeparator The separator string. Cannot be empty.
	* @param oTokenHandler The function object taking a string as parameter.
	*/
template<class TokenHandler>
void tokenizer(const std::string& sStr, const std::string& sSeparator, TokenHandler oTokenHandler)
{
	std::string sToken;
	std::size_t nCurPos = 0;
	const std::size_t nStrLen = sStr.length();
	const std::size_t nSeparatorLen = sSeparator.length();
	assertTrue(nSeparatorLen > 0);
	while (nCurPos < nStrLen) {
		const std::size_t nSepPos = sStr.find(sSeparator, nCurPos);
		if (nSepPos == std::string::npos) {
			sToken = sStr.substr(nCurPos, nStrLen - nCurPos);
			nCurPos = nStrLen;
		} else {
			sToken = sStr.substr(nCurPos, nSepPos - nCurPos);
			nCurPos = nSepPos + nSeparatorLen;
		}
		sToken = Util::strStrip(sToken);
		oTokenHandler(sToken);
	}
}


} // namespace XmlUtil

} // namespace stmg

#endif	/* STMG_XML_STR_CONV_H */

