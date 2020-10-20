/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   testXmlCommonParser.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "xmlcommonparser.h"

namespace stmg
{

using std::unique_ptr;
using std::make_unique;

namespace testing
{

TEST_CASE("testXmlCommonParser, checkIsOneOf")
{
//	static const char* const s_p0Str1 = "Ciao";
//	static const char* const s_p0Str2 = "Boh";
//	static const char* const s_p0Str3 = "Foo";
//	static const char* const s_p0Str4 = "Bar";
// 	REQUIRE(XmlCommonParser::checkStringIsOneOf("Ciao", s_p0Str1, s_p0Str2, s_p0Str3, s_p0Str4));
// 	REQUIRE(XmlCommonParser::checkStringIsOneOf("Bar", s_p0Str1, s_p0Str2, s_p0Str3, s_p0Str4));
// 	REQUIRE_FALSE(XmlCommonParser::checkStringIsOneOf("bar", s_p0Str1, s_p0Str2, s_p0Str3, s_p0Str4));
//
//	static const std::string s_sStr1{"Ciao"};
//	static const std::string s_sStr2 = "Boh";
//	static const std::string s_sStr3 = "Foo";
//	static const std::string s_sStr4{"Bar"};
// 	REQUIRE(XmlCommonParser::checkStringIsOneOf("Ciao", s_sStr1, s_sStr2, s_sStr3, s_sStr4));
// 	REQUIRE(XmlCommonParser::checkStringIsOneOf("Bar", s_sStr1, s_sStr2, s_sStr3, s_sStr4));
// 	REQUIRE_FALSE(XmlCommonParser::checkStringIsOneOf("bar", s_sStr1, s_sStr2, s_sStr3, s_sStr4));
//
// 	REQUIRE(XmlCommonParser::checkStringIsOneOf("Ciao", s_sStr1.c_str(), s_p0Str2, s_p0Str3, s_sStr4.c_str()));
}


} // namespace testing

} // namespace stmg
