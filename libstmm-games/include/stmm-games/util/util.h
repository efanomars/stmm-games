/*
 * File:   util.h
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

#ifndef STMG_UTIL_H
#define STMG_UTIL_H

#include "basictypes.h"

#include <list>
#include <vector>
#include <string>
#include <initializer_list>
#include <utility>
//#include <cassert>
//#include <iostream>
#include <cstdint>
#include <type_traits>
#include <sstream>

#include <stdint.h>

namespace stmg
{

namespace Util
{
static const std::string s_sEmptyString = "";

///////////////////////////////////////////////////////////////////////////////
namespace Private
{
void assertTrue(bool bCond);

//std::string stringComposeSobst(const std::string& sFormat, const std::string& sFind, const std::string& sReplace);
std::string stringComposeSobst(const std::string& sFormat, const std::vector<std::string>& aParams) noexcept;

// from Jason Turner - youtube C++ Weekly - Ep10
template <typename ...T>
std::vector<std::string> stringComposeParams(const T& ... oParam) noexcept
{
	std::vector<std::string> aParams;
	std::stringstream oSs;
	(void)std::initializer_list<int>{ (oSs.str(""), oSs << oParam, aParams.push_back(oSs.str()), 0)... };
	return aParams;
}
} // namespace Private

//TODO constexpr ???
/** Format a string.
 * Ex. stringCompose("%2ce I saw %1 b%2s", 2, "one") returns
 * "once I saw 2 bones"
 * @param sFormat The format string.
 * @param oParam The parameters to be inserted in the format string.
 * @return The formatted string.
 */
template <typename ...T>
std::string stringCompose(const std::string& sFormat, const T& ... oParam) noexcept
{
	const auto aParams = Private::stringComposeParams(oParam...);
	return Private::stringComposeSobst(sFormat, aParams);
}


/** Strips a string of its leading and trailing space characters.
 * Ex. "  HI  there   " to "HI  there".
 * @param sStr The string to strip.
 * @return The stripped string.
 */
std::string strStrip(const std::string& sStr) noexcept;

/** Whether a string contains whitespace characters.
 * @param sStr The string. Can be empty.
 * @return Whether has any space.
 */
bool strContainsWhitespace(const std::string& sStr) noexcept;

/** Adds lines contained in a string to a vector of strings.
 * @param sText The string containing the text.
 * @param aLine The vector of strings to which the single lines are added.
 */
void strTextToLines(const std::string& sText, std::vector<std::string>& aLine) noexcept;
/** UTF8 string size in code points rather than bytes.
 * @param sUtf8 The UTF8 string.
 * @return The size.
 */
int32_t strUTF8SizeInCodePoints(const std::string& sUtf8) noexcept;

/** Formats integer with every 3 digits separator.
 * Ex. 12345678 is formatted as "12'345'678".
 * @param nValue The value.
 * @return The string.
 */
std::string intToMillString(int32_t nValue) noexcept;
/** Formats milliseconds to minutes and seconds.
 * Ex. 1001 outputs string "0:01.001"
 * @param nMilliseconds The value in milliseconds.
 * @return The formatted string.
 */
std::string millisecToMinSecString(int32_t nMilliseconds) noexcept;
/** Formats seconds to minutes.
 * Ex. 61 outputs string "1:01"
 * @param nSeconds The value in seconds.
 * @return The formatted string.
 */
std::string secToMinSecString(int32_t nSeconds) noexcept;

/** Convert a string to boolean.
 * The string can be "t", "T", "true", "True", "TRUE", "y", "Y", "yes", "Yes", "YES" or
 * "f", "F", "false", "False", "FALSE", "n", "N", "no", "No", "NO".
 * @param sStr The string. Cannot be empty.
 * @return The result and an empty string or an error string.
 */
std::pair<bool, std::string> strToBool(const std::string& sStr);

namespace Private
{
template<typename T>
static std::string errorStrToNumberNotWellFormed() noexcept
{
	if (std::is_same<T,char>::value) {
		return "Not a well formed character";
	} else {
		return "Not a well formed number";
	}
}
template<typename T>
static std::string errorStrToNumberNotDefined() noexcept
{
	if (std::is_same<T,char>::value) {
		return "No character defined";
	} else {
		return "No number defined";
	}
}
template<typename T>
static std::string errorStrToNumberMustBeGE(T oMin) noexcept
{
	if (std::is_same<T,char>::value) {
		return Util::stringCompose("Character must be >= '%1'", std::string(1, oMin));
	} else {
		return Util::stringCompose("Number must be >= %1", std::to_string(oMin));
	}
}
template<typename T>
static std::string errorStrToNumberMustBeLE(T oMax) noexcept
{
	if (std::is_same<T,char>::value) {
		return Util::stringCompose("Character must be <= '%1'", std::string(1, oMax));
	} else {
		return Util::stringCompose("Number must be <= %1", std::to_string(oMax));
	}
}
} // namespace Private

/** Converts string to a number.
 * If the number is a percentage the number is divided by 100. Integral types
 * can't use percentage.
 *
 * If both bMin and bMax are true then oMin must be &lt;= oMax.
 * @param sNr The string to convert.
 * @param bAllowPerc Whether the number can have a trailing '%' (percentage).
 * @param bMin Whether the number has a minimum.
 * @param oMin The minimum.
 * @param bMax Whether the number has a maximum.
 * @param oMax The maximum.
 * @return The number and an empty string or an error if invalid or outside min-max.
 */
template<typename T>
static std::pair<T, std::string> strToNumber(const std::string& sNr, bool bAllowPerc, bool bMin, T oMin, bool bMax, T oMax)
{
//std::cout << "Util::strToNumber sNr=" << sNr << '\n';
	static_assert(std::is_arithmetic<T>::value, "");
	Private::assertTrue(! (bAllowPerc && std::is_integral<T>::value) );
	if (sNr.empty()) {
		return std::make_pair(oMin, Private::errorStrToNumberNotDefined<T>()); //-------
	}
	std::istringstream oISS(sNr);
	T oNr;
	if (!(oISS >> std::skipws >> std::dec >> oNr)) {
		return std::make_pair(oMin, Private::errorStrToNumberNotWellFormed<T>()); //-----
	}
	if (!oISS.eof()) {
		// at least trailing spaces are present
		std::string sRest;
		// read
		if (!(oISS >> std::skipws >> sRest)) {
			// error or no string (only trailing spaces)
			if (!oISS.eof()) {
				// error
				return std::make_pair(oMin, Private::errorStrToNumberNotWellFormed<T>()); //---------
			}
		} else {
			// string found
			if (bAllowPerc && (sRest == "%")) {
				oNr = oNr / 100;
			} else {
				// can't have string after number
				return std::make_pair(oMin, Private::errorStrToNumberNotWellFormed<T>()); //---------;
			}
		}
	}
	if (bMin && bMax) {
		Private::assertTrue(oMin <= oMax);
	}
	if (bMin && (oNr < oMin)) {
		return std::make_pair(oMin, Private::errorStrToNumberMustBeGE<T>(oMin)); //-------
	}
	if (bMax && (oNr > oMax)) {
		return std::make_pair(oMin, Private::errorStrToNumberMustBeLE<T>(oMax)); //--------
	}
	return std::make_pair(oNr, Util::s_sEmptyString);
}
/** Converts string to a number in range.
 * If the number has a trailing '%' it is parsed as a double and must be &gt;= 0 and &lt;= 100.
 * In this case the result is `oMin + fPerc * (oMax - oMin)`.
 *
 * The value of oMin must be &lt;= oMax.
 * @param sNr The string to convert.
 * @param oMin The minimum.
 * @param oMax The maximum.
 * @return The number and an empty string or an error string if invalid or outside min-max (or 0-100 for percentage).
 */
template<typename T>
static std::pair<T, std::string> strToRangeNumber(const std::string& sNr, T oMin, T oMax)
{
//std::cout << "Util::strToNumber sNr=" << sNr << '\n';
	static_assert(std::is_arithmetic<T>::value, "");
	Private::assertTrue(oMin <= oMax);
	if (sNr.empty()) {
		return std::make_pair(oMin, Private::errorStrToNumberNotDefined<T>()); //-------
	}
	std::string sNrPerc = Util::strStrip(sNr);
	if (sNrPerc.empty()) {
		return std::make_pair(oMin, Private::errorStrToNumberNotWellFormed<T>()); //-----
	}
	if (sNrPerc.back() == '%') {
		sNrPerc.pop_back();
		auto oPairPerc = strToNumber<double>(sNrPerc, false, true, 0.0, true, 100.0);
		auto& sPercError = oPairPerc.second;
		if (! sPercError.empty()) {
			return std::make_pair(oMin, std::move(sPercError)); //--------------
		}
		const double& fPerc = oPairPerc.first;
		T oNr = oMin + fPerc * 0.01 * (oMax - oMin);
		return std::make_pair(oNr, Util::s_sEmptyString); //--------------------
	}
	std::istringstream oISS(sNr);
	T oNr;
	if (!(oISS >> std::skipws >> std::dec >> oNr)) {
		return std::make_pair(oMin, Private::errorStrToNumberNotWellFormed<T>()); //-----
	}
	if (!oISS.eof()) {
		// at least trailing spaces are present
		std::string sRest;
		// read
		if (!(oISS >> std::skipws >> sRest)) {
			// error or no string (only trailing spaces)
			if (!oISS.eof()) {
				// error
				return std::make_pair(oMin, Private::errorStrToNumberNotWellFormed<T>()); //---------
			}
		} else {
			// can't have string after number
			return std::make_pair(0, Private::errorStrToNumberNotWellFormed<T>()); //---------
		}
	}
	if (oNr < oMin) {
		return std::make_pair(0, Private::errorStrToNumberMustBeGE<T>(oMin)); //-------
	}
	if (oNr > oMax) {
		return std::make_pair(oMin, Private::errorStrToNumberMustBeLE<T>(oMax)); //--------
	}
	return std::make_pair(oNr, Util::s_sEmptyString);
}

////////////////////////////////////////////////////////////////////////////////
template<class LT>
bool listContains(std::list<LT>& oList, const LT& oValue, typename std::list<LT>::iterator& it) noexcept
{
	for (; it != oList.end(); ++it) {
		if (*it == oValue) {
			return true;
		}
	}
	return false;
}
template<class LT>
bool listContains(const std::list<LT>& oList, const LT& oValue) noexcept
{
	for (typename std::list<LT>::const_iterator it = oList.begin()
				; it != oList.end(); ++it) {
		if (*it == oValue) {
			return true;
		}
	}
	return false;
}
template<class LT>
bool listExtract(std::list<LT>& oList, const LT& oValue) noexcept
{
	typename std::list<LT>::iterator it = oList.begin();
	if (listContains<LT>(oList, oValue, it)) {
		oList.erase(it);
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
/** Packs a point (64 bit) to an int32_t.
 * @param oXY The point to pack. Both m_nX and m_nY must be &gt;= -16384 and &lt; 16384.
 * @return The packed value.
 */
inline int32_t packPointToInt32(NPoint oXY) noexcept
{
	// The following is used because of ODR !!!??? (Titus Winters talk 2018)
	Private::assertTrue((oXY.m_nX >= -16384) && (oXY.m_nX < 16384));
	Private::assertTrue((oXY.m_nY >= -16384) && (oXY.m_nY < 16384));
	return (oXY.m_nX + 16384) | ((oXY.m_nY + 16384) << 16);
}
/** Unpacks a point (64 bit) from an int32_t.
 * @param nXY The packed value.
 * @return The point.
 */
inline NPoint unpackPointFromInt32(int32_t nXY) noexcept
{
	const int32_t nX = (nXY & ((1 << 16) - 1)) - 16384;
	const int32_t nY = ((nXY >> 16) & ((1 << 16) - 1)) - 16384;
	return NPoint{nX, nY};
}

/** Packs a point to an int64_t.
 * @param oXY The point to pack.
 * @return The packed value.
 */
inline int64_t packPointToInt64(NPoint oXY) noexcept
{
	return ((static_cast<int64_t>(oXY.m_nY)) << 32) | (static_cast<uint32_t>(oXY.m_nX));
}
/** Unpacks a point from an int64_t.
 * @param nXY The packed value.
 * @return The point.
 */
inline NPoint unpackPointFromInt64(int64_t nXY) noexcept
{
	const int32_t nX = static_cast<int32_t>(nXY);
	const int32_t nY = static_cast<int32_t>(static_cast<int64_t>(nXY >> 32));
	return NPoint{nX, nY};
}
/** Unpacks a XY pair from an int64_t.
 * @param nXY The packed value.
 * @return The point.
 */
inline std::pair<int32_t, int32_t> unpackPairFromInt64(int64_t nXY) noexcept
{
	const int32_t nX = static_cast<int32_t>(nXY);
	const int32_t nY = static_cast<int32_t>(static_cast<int64_t>(nXY >> 32));
	return std::make_pair(nX, nY);
}

} // namespace Util

} // namespace stmg

#endif	/* STMG_UTIL_H */

