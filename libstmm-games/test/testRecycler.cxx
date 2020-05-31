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
 * File:   testRecycler.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "util/recycler.h"

#include <string>
#include <cassert>
#include <iostream>

namespace stmg
{

namespace testing
{

struct TestRecyclerA
{
	TestRecyclerA() : m_nId(++s_nId) { m_nData = 11; }
	void reInit() { m_nData = 22; }

	int32_t m_nData = 0;
	const int32_t m_nId;
	static int32_t s_nId;
};
int32_t TestRecyclerA::s_nId = 0;

constexpr bool s_bTestImplementation = true;

TEST_CASE("testRecycler, EmptyConstructor")
{
	Recycler<TestRecyclerA> oRecyclerA;
	shared_ptr<TestRecyclerA> refA;
	oRecyclerA.create(refA);
	REQUIRE(refA.get() != nullptr);
	REQUIRE(refA->m_nData == 11);
	if (s_bTestImplementation) {
		REQUIRE(refA->m_nId == 1);
	}
	oRecyclerA.create(refA);
	REQUIRE(refA.get() != nullptr);
	REQUIRE(refA->m_nData == 11);
	if (s_bTestImplementation) {
		REQUIRE(refA->m_nId == 2);
	}
	oRecyclerA.create(refA);
	REQUIRE(refA.get() != nullptr);
	REQUIRE(refA->m_nData == 22);
	if (s_bTestImplementation) {
		REQUIRE(refA->m_nId == 1);
	}
	refA.reset();
	oRecyclerA.create(refA);
	REQUIRE(refA.get() != nullptr);
	REQUIRE(refA->m_nData == 22);
	if (s_bTestImplementation) {
		REQUIRE(refA->m_nId == 1);
	}

	shared_ptr<TestRecyclerA> refA2;
	oRecyclerA.create(refA2);
	REQUIRE(refA2.get() != nullptr);
	REQUIRE(refA2->m_nData == 22);
	if (s_bTestImplementation) {
		REQUIRE(refA2->m_nId == 2);
	}
	shared_ptr<TestRecyclerA> refA3;
	oRecyclerA.create(refA3);
	REQUIRE(refA3.get() != nullptr);
	REQUIRE(refA3->m_nData == 11);
	if (s_bTestImplementation) {
		REQUIRE(refA3->m_nId == 3);
	}
}

struct TestRecyclerB
{
	TestRecyclerB(int32_t nP1, std::string sP2)
	: m_nData(11)
	, m_nP1(nP1)
	, m_sP2(sP2)
	, m_nId(++s_nId)
	{
	}
	void reInit(int32_t nP1, std::string sP2)
	{
		m_nData = 22;
		m_nP1 = nP1;
		m_sP2 = sP2;
	}

	int32_t m_nData = 0;
	int32_t m_nP1;
	std::string m_sP2;

	const int32_t m_nId;
	static int32_t s_nId;
};
int32_t TestRecyclerB::s_nId = 0;

TEST_CASE("testRecycler, LvalueConstructor")
{
	Recycler<TestRecyclerB> oRecyclerB;
	shared_ptr<TestRecyclerB> refB;
	oRecyclerB.create(refB, 55, "Ciao");
	REQUIRE(refB.get() != nullptr);
	REQUIRE(refB->m_nData == 11);
	if (s_bTestImplementation) {
		REQUIRE(refB->m_nId == 1);
	}
}

struct TestRecyclerC
{
	TestRecyclerC(std::pair<int32_t, int32_t>&& oP1, std::string&& sP2)
	: m_nId(++s_nId)
	{
		m_oP1 = std::move(oP1);
		m_sP2 = std::move(sP2);
		m_nData = 11;
	}
	void reInit(std::pair<int32_t, int32_t>&& oP1, std::string&& sP2)
	{
		m_oP1 = std::move(oP1);
		m_sP2 = std::move(sP2);
		m_nData = 22;
	}

	int32_t m_nData = 0;
	std::pair<int32_t, int32_t> m_oP1;
	std::string m_sP2;

	const int32_t m_nId;
	static int32_t s_nId;
};
int32_t TestRecyclerC::s_nId = 0;

TEST_CASE("testRecycler, RvalueConstructor")
{
	Recycler<TestRecyclerC> oRecyclerC;
	shared_ptr<TestRecyclerC> refC;
	oRecyclerC.create(refC, std::move(std::make_pair(55,77)), "Ciao");
	REQUIRE(refC.get() != nullptr);
	REQUIRE(refC->m_nData == 11);
	if (s_bTestImplementation) {
		REQUIRE(refC->m_nId == 1);
	}
}

} // namespace testing

} // namespace stmg
