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
 * File:   testCircularBuffer.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "util/circularbuffer.h"

#include <memory>
#include <cassert>
#include <iostream>
 
namespace stmg
{

namespace testing
{

class BufData
{
public:
	BufData()
	: m_nValue(-1)
	{
	};
	explicit BufData(int32_t nValue)
	: m_nValue(nValue)
	{
	};

	int32_t m_nValue;
};

TEST_CASE("testCircularBuffer, Constructor")
{
	{
	CircularBuffer<BufData> oBuf(5);

	REQUIRE( oBuf.capacity() == 5 );
	REQUIRE( oBuf.size() == 0 );
	REQUIRE( oBuf.isEmpty() == true );
	REQUIRE( oBuf.isFull() == false );

	oBuf.clear();

	REQUIRE( oBuf.capacity() == 5 );
	REQUIRE( oBuf.size() == 0 );
	REQUIRE( oBuf.isEmpty() == true );
	REQUIRE( oBuf.isFull() == false );

	}
}
TEST_CASE("testCircularBuffer, AddAndClear")
{
	{
	CircularBuffer<BufData> oBuf(5);

	oBuf.write(BufData{5});

	REQUIRE( oBuf.capacity() == 5 );
	REQUIRE( oBuf.size() == 1 );
	REQUIRE( oBuf.isEmpty() == false );
	REQUIRE( oBuf.isFull() == false );

	oBuf.clear();

	REQUIRE( oBuf.capacity() == 5 );
	REQUIRE( oBuf.size() == 0 );
	REQUIRE( oBuf.isEmpty() == true );
	REQUIRE( oBuf.isFull() == false );

	oBuf.write(BufData{7});

	REQUIRE( oBuf.capacity() == 5 );
	REQUIRE( oBuf.size() == 1 );
	REQUIRE( oBuf.isEmpty() == false );
	REQUIRE( oBuf.isFull() == false );

	}
}
TEST_CASE("testCircularBuffer, ReadAndWrite")
{
	{
	CircularBuffer<BufData> oBuf(5);

	oBuf.write(BufData{5});

	REQUIRE( oBuf.size() == 1 );
	REQUIRE( oBuf.isEmpty() == false );
	REQUIRE( oBuf.isFull() == false );

	BufData oRes = oBuf.read();
	REQUIRE( oRes.m_nValue == 5 );

	REQUIRE( oBuf.size() == 0 );
	REQUIRE( oBuf.isEmpty() == true );
	REQUIRE( oBuf.isFull() == false );

	for (int32_t nIdx = 0; nIdx < 5; ++nIdx) {
		oBuf.write(BufData{nIdx});
	}

	REQUIRE( oBuf.size() == 5 );
	REQUIRE( oBuf.isEmpty() == false );
	REQUIRE( oBuf.isFull() == true );

	for (int32_t nIdx = 0; nIdx < 5; ++nIdx) {
		BufData oRes = oBuf.read();
		REQUIRE( oRes.m_nValue == nIdx );
	}

	REQUIRE( oBuf.size() == 0 );
	REQUIRE( oBuf.isEmpty() == true );
	REQUIRE( oBuf.isFull() == false );
	}

	{
	CircularBuffer<BufData> oBuf(7);

	for (int32_t nIdx = 0; nIdx < 30; ++nIdx) {
		oBuf.write(BufData{nIdx});
		BufData oRes = oBuf.read();
		REQUIRE( oRes.m_nValue == nIdx );
	}

	REQUIRE( oBuf.size() == 0 );
	REQUIRE( oBuf.isEmpty() == true );
	REQUIRE( oBuf.isFull() == false );
	}

	{
	CircularBuffer<BufData> oBuf(7);

	oBuf.write(BufData{33});

	for (int32_t nIdx = 0; nIdx < 30; ++nIdx) {
		oBuf.write(BufData{nIdx});
		BufData oRes = oBuf.read();
		REQUIRE( oRes.m_nValue == (((nIdx - 1) < 0) ? 33 : nIdx - 1));

		REQUIRE( oBuf.size() == 1 );
		REQUIRE( oBuf.isEmpty() == false );
		REQUIRE( oBuf.isFull() == false );
	}
	BufData oRes = oBuf.read();
	REQUIRE( oRes.m_nValue == 29);

	REQUIRE( oBuf.size() == 0 );
	REQUIRE( oBuf.isEmpty() == true );
	REQUIRE( oBuf.isFull() == false );
	}
}
TEST_CASE("testCircularBuffer, NonCopyable")
{
	{
	CircularBuffer< std::unique_ptr<int32_t> > oBuf(3);
	oBuf.write( std::make_unique<int32_t>(44) );

	std::unique_ptr<int32_t> ref44 = oBuf.read();
	REQUIRE( ref44.get() != nullptr );
	REQUIRE( (*ref44) == 44 );
	}
}
TEST_CASE("testCircularBuffer, Peek")
{
	{
	CircularBuffer< std::unique_ptr<int32_t> > oBuf(3);
	oBuf.write( std::make_unique<int32_t>(44) );
	std::unique_ptr<int32_t>& ref44 = oBuf.peekValue(0);
	REQUIRE( ref44.get() != nullptr );
	REQUIRE( (*ref44) == 44 );

	oBuf.write( std::make_unique<int32_t>(55) );
	std::unique_ptr<int32_t>& ref55 = oBuf.peekValue(1);
	REQUIRE( ref55.get() != nullptr );
	REQUIRE( (*ref55) == 55 );
	std::unique_ptr<int32_t>& ref44b = oBuf.peekValue(0);
	REQUIRE( ref44b.get() != nullptr );
	REQUIRE( (*ref44b) == 44 );

	std::unique_ptr<int32_t> refRes = oBuf.read();
	REQUIRE( (*refRes) == 44 );
	std::unique_ptr<int32_t>& ref55b = oBuf.peekValue(0);
	REQUIRE( ref55b.get() != nullptr );
	REQUIRE( (*ref55b) == 55 );

	oBuf.write( std::make_unique<int32_t>(66) );
	std::unique_ptr<int32_t>& ref55c = oBuf.peekValue(0);
	REQUIRE( ref55c.get() != nullptr );
	REQUIRE( (*ref55c) == 55 );
	std::unique_ptr<int32_t>& ref66b = oBuf.peekValue(1);
	REQUIRE( ref66b.get() != nullptr );
	REQUIRE( (*ref66b) == 66 );

	oBuf.write( std::make_unique<int32_t>(77) );
	std::unique_ptr<int32_t>& ref77 = oBuf.peekValue(2);
	REQUIRE( ref77.get() != nullptr );
	REQUIRE( (*ref77) == 77 );

	*ref55c = 333;
	std::unique_ptr<int32_t>& ref55d = oBuf.peekValue(0);
	REQUIRE( ref55d.get() != nullptr );
	REQUIRE( (*ref55d) == 333 );

	std::unique_ptr<int32_t> refRes2 = oBuf.read();
	REQUIRE( (*refRes2) == 333 );
	}
}

//TEST_CASE("testCircularBuffer, GetSet")
//{
//	const int32_t nW = 5;
//	const int32_t nH = 7;
//	{
//	CircularBuffer<BufData> oBuf({nW,nH});
//
//	REQUIRE( oBuf.getW() == nW );
//	REQUIRE( oBuf.getH() == nH );
//
//	oBuf.set({3,2},BufData(111));
//	REQUIRE( oBuf.get({3,2}).m_fValue == 111 );
//	REQUIRE( oBuf.get({2,3}).m_fValue == -1 );
//	REQUIRE( oBuf.get({nW -1, nH -1}).m_fValue == -1 );
//	}
//	{
//	CircularBuffer<BufDataNoC> oBufC({nW,nH},BufDataNoC(77));
//
//	REQUIRE( oBufC.getW() == nW );
//	REQUIRE( oBufC.getH() == nH );
//
//	oBufC.set({3,2}, BufDataNoC(1212));
//	REQUIRE( oBufC.get({3,2}).m_nValue == 1212 );
//	REQUIRE( oBufC.get({2,3}).m_nValue == 77 );
//	REQUIRE( oBufC.get({nW -1,nH -1}).m_nValue == 77 );
//
//	oBufC.get({4,1}).m_nValue = 888;
//	REQUIRE( oBufC.get({4,1}).m_nValue == 888 );
//	}
//}

} // namespace testing

} // namespace stmg
