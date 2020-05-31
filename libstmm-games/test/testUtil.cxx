/*
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   testUtil.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "util/util.h"

#include <limits>

namespace stmg
{

namespace testing
{

TEST_CASE("testUtil, StringCompose")
{
	std::string sRes = Util::stringCompose("Hi There");
	REQUIRE(sRes == "Hi There");
	sRes = Util::stringCompose("");
	REQUIRE(sRes.empty());
	sRes = Util::stringCompose("%1", "  Hi  ");
	REQUIRE(sRes == "  Hi  ");
	sRes = Util::stringCompose("-%2-%1-", "P1", "P2");
	REQUIRE(sRes == "-P2-P1-");
	sRes = Util::stringCompose("%%", "P1", "P2");
	REQUIRE(sRes == "%");
	sRes = Util::stringCompose("%10", "xP1x", "P2", "P3", "P4", "P5", "P6", "P7", "P8", "P9", "P10");
	REQUIRE(sRes == "xP1x0");
}
TEST_CASE("testUtil, StrStrip")
{
	std::string sRes = Util::strStrip(" Hi There  ");
	REQUIRE(sRes == "Hi There");
	sRes = Util::strStrip("Hi There");
	REQUIRE(sRes == "Hi There");
	sRes = Util::strStrip("  ");
	REQUIRE(sRes.empty());
}
TEST_CASE("testUtil, StrTextToLines")
{
	std::vector<std::string> aRes;
	Util::strTextToLines("", aRes);
	REQUIRE(aRes.size() == 0);
	Util::strTextToLines("A", aRes);
	REQUIRE(aRes.size() == 1);
	REQUIRE(aRes[0] == "A");
	aRes.clear();
	Util::strTextToLines("\n", aRes);
	REQUIRE(aRes.size() == 1);
	REQUIRE(aRes[0] == "");
	aRes.clear();
	std::string sStr = "AA\r\nBBB";
	assert(sStr.size() == 7);
	assert(sStr[2] == 13);
	assert(sStr[3] == 10);
	Util::strTextToLines(sStr, aRes);
	REQUIRE(aRes.size() == 2);
	REQUIRE(aRes[0] == "AA");
	REQUIRE(aRes[1] == "BBB");
}

TEST_CASE("testUtil, StrUTF8SizeInCodePoints")
{
	std::string sUtf8 = "";
	REQUIRE(Util::strUTF8SizeInCodePoints(sUtf8) == 0);
	sUtf8 = "abc";
	REQUIRE(Util::strUTF8SizeInCodePoints(sUtf8) == 3);
	sUtf8 = u8"\U000020AC"; // euro
	REQUIRE(Util::strUTF8SizeInCodePoints(sUtf8) == 1);
	sUtf8 = u8"\U0001F30Dmm\U000020AC"; // globe
	REQUIRE(Util::strUTF8SizeInCodePoints(sUtf8) == 4);
}
TEST_CASE("testUtil, StrToBool")
{
	REQUIRE(Util::strToBool("true").first == true);
	REQUIRE(Util::strToBool("true").second.empty());
	REQUIRE(Util::strToBool("false").first == false);
	REQUIRE(Util::strToBool("false").second.empty());
	REQUIRE(Util::strToBool("True").first == true);
	REQUIRE(Util::strToBool("True").second.empty());
	REQUIRE(Util::strToBool("False").first == false);
	REQUIRE(Util::strToBool("False").second.empty());
	REQUIRE(Util::strToBool("TRUE").first == true);
	REQUIRE(Util::strToBool("TRUE").second.empty());
	REQUIRE(Util::strToBool("FALSE").first == false);
	REQUIRE(Util::strToBool("FALSE").second.empty());
	REQUIRE(Util::strToBool("yes").first == true);
	REQUIRE(Util::strToBool("yes").second.empty());
	REQUIRE(Util::strToBool("no").first == false);
	REQUIRE(Util::strToBool("no").second.empty());
	REQUIRE(Util::strToBool("  y  ").first == true);
	REQUIRE(Util::strToBool("  y  ").second.empty());
	REQUIRE(Util::strToBool(" n").first == false);
	REQUIRE(Util::strToBool(" n").second.empty());
	REQUIRE(! Util::strToBool("").second.empty());
	REQUIRE(! Util::strToBool("0").second.empty());
	REQUIRE(! Util::strToBool("tRUE").second.empty());
}
TEST_CASE("testUtil, StrToNumber")
{
	REQUIRE(Util::strToNumber<int32_t>("12", false, true, 0, true, 20).first == 12);
	REQUIRE(Util::strToNumber<int32_t>("12", false, true, 0, true, 20).second.empty());
	REQUIRE(Util::strToNumber<int32_t>("0 ", false, true, 0, true, 20).first == 0);
	REQUIRE(Util::strToNumber<int32_t>("0 ", false, true, 0, true, 20).second.empty());
	REQUIRE(Util::strToNumber<int32_t>(" 20", false, true, 0, true, 20).first == 20);
	REQUIRE(Util::strToNumber<int32_t>(" 20", false, true, 0, true, 20).second.empty());
	REQUIRE(! Util::strToNumber<int32_t>("21", false, true, 0, true, 20).second.empty());
	REQUIRE(! Util::strToNumber<int32_t>("-11", false, true, 0, true, 20).second.empty());
	REQUIRE(Util::strToNumber<int32_t>("-11", false, false, -1, true, 20).first == -11);
	REQUIRE(Util::strToNumber<int32_t>("-11", false, false, -1, true, 20).second.empty());
	REQUIRE(! Util::strToNumber<int32_t>("-11-", false, false, 0, true, 20).second.empty());
	//
	REQUIRE(Util::strToNumber<double>("12.5", false, true, 0, true, 20).first == 12.5);
	REQUIRE(Util::strToNumber<double>("12.5", false, true, 0, true, 20).second.empty());
	REQUIRE(Util::strToNumber<double>("12%", true, true, 0, true, 20).first == 0.12);
	REQUIRE(Util::strToNumber<double>("12%", true, true, 0, true, 20).second.empty());
	REQUIRE(! Util::strToNumber<double>("12%", true, true, 10, true, 20).second.empty());
	REQUIRE(! Util::strToNumber<double>("12%", false, true, 0, true, 20).second.empty());
	REQUIRE(Util::strToNumber<double>(" 12 %", true, true, 0, true, 20).first == 0.12);
	REQUIRE(Util::strToNumber<double>(" 12 %", true, true, 0, true, 20).second.empty());
	REQUIRE(! Util::strToNumber<double>("12%%", true, true, 0, true, 20).second.empty());
	REQUIRE(! Util::strToNumber<double>("12%1", true, true, 0, true, 20).second.empty());
//REQUIRE(Util::strToNumber<double>(" 12 %%", false, true, 0, true, 20) == 0.12);
	//
	REQUIRE(Util::strToNumber<char>("A", false, true, 'A', true, 'Z').first == 'A');
	REQUIRE(Util::strToNumber<char>("A", false, true, 'A', true, 'Z').second.empty());
	REQUIRE(Util::strToNumber<char>("Z", false, true, 'A', true, 'Z').first == 'Z');
	REQUIRE(Util::strToNumber<char>("Z", false, true, 'A', true, 'Z').second.empty());
	REQUIRE(! Util::strToNumber<char>("AA", false, true, 'A', true, 'Z').second.empty());
	REQUIRE(! Util::strToNumber<char>("11", false, true, 'A', true, 'Z').second.empty());
	REQUIRE(! Util::strToNumber<char>("1", false, true, 'A', true, 'Z').second.empty());
//REQUIRE(Util::strToNumber<char>("7", false, true, 'A', true, 'Z') == 'A');
}
TEST_CASE("testUtil, StrToRangeNumber")
{
	REQUIRE(Util::strToRangeNumber<int32_t>("12", 0, 20).first == 12);
	REQUIRE(Util::strToRangeNumber<int32_t>("12", 0, 20).second.empty());
	REQUIRE(Util::strToRangeNumber<int32_t>("0 ", 0, 20).first == 0);
	REQUIRE(Util::strToRangeNumber<int32_t>("0 ", 0, 20).second.empty());
	REQUIRE(Util::strToRangeNumber<int32_t>(" 20", 0, 20).first == 20);
	REQUIRE(Util::strToRangeNumber<int32_t>(" 20", 0, 20).second.empty());
	REQUIRE(! Util::strToRangeNumber<int32_t>("21", 0, 20).second.empty());
	REQUIRE(! Util::strToRangeNumber<int32_t>("-1", 0, 20).second.empty());
	REQUIRE(Util::strToRangeNumber<int32_t>("-11", -11, 20).first == -11);
	REQUIRE(Util::strToRangeNumber<int32_t>("-11", -11, 20).second.empty());
	REQUIRE(! Util::strToRangeNumber<int32_t>("-11-", -11, 20).second.empty());
	//
	REQUIRE(Util::strToRangeNumber<int32_t>("10%", 0, 20).first == 2);
	REQUIRE(Util::strToRangeNumber<int32_t>("10%", 0, 20).second.empty());
	REQUIRE(Util::strToRangeNumber<int32_t>("100%", 0, 20).first == 20);
	REQUIRE(Util::strToRangeNumber<int32_t>("100%", 0, 20).second.empty());
	REQUIRE(Util::strToRangeNumber<int32_t>("10%", -11, 9).first == -9);
	REQUIRE(Util::strToRangeNumber<int32_t>("10%", -11, 9).second.empty());
	REQUIRE(Util::strToRangeNumber<int32_t>("100 % ", -11, 9).first == 9);
	REQUIRE(Util::strToRangeNumber<int32_t>("100 % ", -11, 9).second.empty());
//	//
//	REQUIRE(Util::strToNumber<double>("12.5", false, true, 0, true, 20).first == 12.5);
//	REQUIRE(Util::strToNumber<double>("12.5", false, true, 0, true, 20).second.empty());
//	REQUIRE(Util::strToNumber<double>("12%", true, true, 0, true, 20).first == 0.12);
//	REQUIRE(Util::strToNumber<double>("12%", true, true, 0, true, 20).second.empty());
//	REQUIRE(! Util::strToNumber<double>("12%", true, true, 10, true, 20).second.empty());
//	REQUIRE(! Util::strToNumber<double>("12%", false, true, 0, true, 20).second.empty());
//	REQUIRE(Util::strToNumber<double>(" 12 %", true, true, 0, true, 20).first == 0.12);
//	REQUIRE(Util::strToNumber<double>(" 12 %", true, true, 0, true, 20).second.empty());
//	REQUIRE(! Util::strToNumber<double>("12%%", true, true, 0, true, 20).second.empty());
//	REQUIRE(! Util::strToNumber<double>("12%1", true, true, 0, true, 20).second.empty());
////REQUIRE(Util::strToNumber<double>(" 12 %%", false, true, 0, true, 20) == 0.12);
//	//
//	REQUIRE(Util::strToNumber<char>("A", false, true, 'A', true, 'Z').first == 'A');
//	REQUIRE(Util::strToNumber<char>("A", false, true, 'A', true, 'Z').second.empty());
//	REQUIRE(Util::strToNumber<char>("Z", false, true, 'A', true, 'Z').first == 'Z');
//	REQUIRE(Util::strToNumber<char>("Z", false, true, 'A', true, 'Z').second.empty());
//	REQUIRE(! Util::strToNumber<char>("AA", false, true, 'A', true, 'Z').second.empty());
//	REQUIRE(! Util::strToNumber<char>("11", false, true, 'A', true, 'Z').second.empty());
//	REQUIRE(! Util::strToNumber<char>("1", false, true, 'A', true, 'Z').second.empty());
////REQUIRE(Util::strToNumber<char>("7", false, true, 'A', true, 'Z') == 'A');
}
TEST_CASE("testUtil, IntToMillString")
{
	REQUIRE(Util::intToMillString(0) == "0");
	REQUIRE(Util::intToMillString(12) == "12");
	REQUIRE(Util::intToMillString(-1) == "-1");
	REQUIRE(Util::intToMillString(-123) == "-123");
	REQUIRE(Util::intToMillString(999) == "999");
	REQUIRE(Util::intToMillString(1000) == "1'000");
	REQUIRE(Util::intToMillString(1001) == "1'001");
	REQUIRE(Util::intToMillString(1608476783) == "1'608'476'783");
	REQUIRE(Util::intToMillString(-1608476783) == "-1'608'476'783");
}
TEST_CASE("testUtil, MillisecToMinSecString")
{
	REQUIRE(Util::millisecToMinSecString(0) == "0:00.000");
	REQUIRE(Util::millisecToMinSecString(19) == "0:00.019");
	REQUIRE(Util::millisecToMinSecString(131234) == "2:11.234");
	REQUIRE(Util::millisecToMinSecString(-131234) == "-2:11.234");
	REQUIRE(Util::millisecToMinSecString(-3999) == "-0:03.999");
}
TEST_CASE("testUtil, SecToMinSecString")
{
	REQUIRE(Util::secToMinSecString(0) == "0:00");
	REQUIRE(Util::secToMinSecString(131) == "2:11");
	REQUIRE(Util::secToMinSecString(-131) == "-2:11");
	REQUIRE(Util::secToMinSecString(-9) == "-0:09");
}
TEST_CASE("testUtil, PackXYToInt32")
{
	{
	const int32_t nPackXY = Util::packPointToInt32(NPoint{0, 0});
	REQUIRE(Util::unpackPointFromInt32(nPackXY) == NPoint{0, 0});
	}
	{
	const int32_t nPackXY = Util::packPointToInt32(NPoint{5, 7});
	REQUIRE(Util::unpackPointFromInt32(nPackXY) == NPoint{5, 7});
	}
	{
	const int32_t nPackXY = Util::packPointToInt32(NPoint{-5, 7});
	REQUIRE(Util::unpackPointFromInt32(nPackXY) == NPoint{-5, 7});
	}
	{
	const int32_t nPackXY = Util::packPointToInt32(NPoint{-5, -7});
	REQUIRE(Util::unpackPointFromInt32(nPackXY) == NPoint{-5, -7});
	}
	{
	const int32_t nPackXY = Util::packPointToInt32(NPoint{5, -7});
	REQUIRE(Util::unpackPointFromInt32(nPackXY) == NPoint{5, -7});
	}
	{
	const int32_t nPackXY = Util::packPointToInt32(NPoint{-16384, -16384});
	REQUIRE(Util::unpackPointFromInt32(nPackXY) == NPoint{-16384, -16384});
	}
	{
	const int32_t nPackXY = Util::packPointToInt32(NPoint{-16384, +16383});
	REQUIRE(Util::unpackPointFromInt32(nPackXY) == NPoint{-16384, +16383});
	}
	{
	const int32_t nPackXY = Util::packPointToInt32(NPoint{+16383, +16383});
	REQUIRE(Util::unpackPointFromInt32(nPackXY) == NPoint{+16383, +16383});
	}
	{
	const int32_t nPackXY = Util::packPointToInt32(NPoint{+16383, -16384});
	REQUIRE(Util::unpackPointFromInt32(nPackXY) == NPoint{+16383, -16384});
	}
	{
	const int32_t nPackXY = Util::packPointToInt32(NPoint{6788, -454});
	REQUIRE(Util::unpackPointFromInt32(nPackXY) == NPoint{6788, -454});
	}
}
TEST_CASE("testUtil, PackXYToInt64")
{
	{
	const int64_t nPackXY = Util::packPointToInt64(NPoint{0, 0});
	REQUIRE(Util::unpackPointFromInt64(nPackXY) == NPoint{0, 0});
	}
	{
	const int64_t nPackXY = Util::packPointToInt64(NPoint{5, 7});
	REQUIRE(Util::unpackPointFromInt64(nPackXY) == NPoint{5, 7});
	}
	{
	const int64_t nPackXY = Util::packPointToInt64(NPoint{-5, 7});
	REQUIRE(Util::unpackPointFromInt64(nPackXY) == NPoint{-5, 7});
	}
	{
	const int64_t nPackXY = Util::packPointToInt64(NPoint{-5, -7});
	REQUIRE(Util::unpackPointFromInt64(nPackXY) == NPoint{-5, -7});
	}
	{
	const int64_t nPackXY = Util::packPointToInt64(NPoint{5, -7});
	REQUIRE(Util::unpackPointFromInt64(nPackXY) == NPoint{5, -7});
	}
	{
	const int64_t nPackXY = Util::packPointToInt64(NPoint{std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::min()});
	REQUIRE(Util::unpackPointFromInt64(nPackXY) == NPoint{std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::min()});
	}
	{
	const int64_t nPackXY = Util::packPointToInt64(NPoint{std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max()});
	REQUIRE(Util::unpackPointFromInt64(nPackXY) == NPoint{std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max()});
	}
	{
	const int64_t nPackXY = Util::packPointToInt64(NPoint{std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max()});
	REQUIRE(Util::unpackPointFromInt64(nPackXY) == NPoint{std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max()});
	}
	{
	const int64_t nPackXY = Util::packPointToInt64(NPoint{std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::min()});
	REQUIRE(Util::unpackPointFromInt64(nPackXY) == NPoint{std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::min()});
	}
	{
	const int64_t nPackXY = Util::packPointToInt64(NPoint{6788, -454});
	REQUIRE(Util::unpackPointFromInt64(nPackXY) == NPoint{6788, -454});
	}
}

//TEST_CASE("testCoords, XY")
//{
////std::cout << "testCoords::XY()" << '\n';
//	int32_t nX, nY;
//	uint64_t nXY;
//	nX = 0; nY = 0; nXY = Coords::packXY(nX,nY);
//	REQUIRE(( (Coords::getXFromXY(nXY) == 0) && (Coords::getYFromXY(nXY) == 0) ));
//	nX = -1; nY = 0; nXY = Coords::packXY(nX,nY);
//	REQUIRE(( (Coords::getXFromXY(nXY) == -1) && (Coords::getYFromXY(nXY) == 0) ));
//	nX = 0; nY = -1; nXY = Coords::packXY(nX,nY);
//	REQUIRE(( (Coords::getXFromXY(nXY) == 0) && (Coords::getYFromXY(nXY) == -1) ));
//	nX = 125; nY = -34; nXY = Coords::packXY(nX,nY);
//	REQUIRE(( (Coords::getXFromXY(nXY) == 125) && (Coords::getYFromXY(nXY) == -34) ));
//	nX = -34; nY = 125; nXY = Coords::packXY(nX,nY);
//	REQUIRE(( (Coords::getXFromXY(nXY) == -34) && (Coords::getYFromXY(nXY) == 125) ));
//	nX = 3; nY = 45; nXY = Coords::packXY(nX,nY);
//	REQUIRE(( (Coords::getXFromXY(nXY) == 3) && (Coords::getYFromXY(nXY) == 45) ));
//	nX = -76; nY = -12121; nXY = Coords::packXY(nX,nY);
//	REQUIRE(( (Coords::getXFromXY(nXY) == -76) && (Coords::getYFromXY(nXY) == -12121) ));
//	nX = 2147483647; nY = -1; nXY = Coords::packXY(nX,nY);
//	REQUIRE(( (Coords::getXFromXY(nXY) == 2147483647) && (Coords::getYFromXY(nXY) == -1) ));
//	nX = -2147483648; nY = -2147483648; nXY = Coords::packXY(nX,nY);
//	REQUIRE(( (Coords::getXFromXY(nXY) == -2147483648) && (Coords::getYFromXY(nXY) == -2147483648) ));
//}


} // namespace testing

} // namespace stmg
