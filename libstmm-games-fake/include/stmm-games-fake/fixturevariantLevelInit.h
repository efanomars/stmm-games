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
 * File:   fixturevariantLevelInit.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_VARIANT_LEVEL_INIT_H
#define STMG_TESTING_FIXTURE_VARIANT_LEVEL_INIT_H

#include <stmm-games/level.h>
#include <stmm-games/tile.h>

#include <vector>
#include <memory>
#include <cassert>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

namespace testing
{

// Variants for the Level::Init structure to be passed to the Game instance

///////////////////////////////////////////////////////////////////////////////
template<int32_t WIDTH>
class FixtureVariantLevelInitBoardWidth // Default: FixtureVariantLevelInitBoardWidth<10>
{
};
///////////////////////////////////////////////////////////////////////////////
template<int32_t HEIGHT>
class FixtureVariantLevelInitBoardHeight // Default: FixtureVariantLevelInitBoardHeight<6>
{
};

///////////////////////////////////////////////////////////////////////////////
template<int32_t WIDTH>
class FixtureVariantLevelInitShowWidth // Default: FixtureVariantLevelInitShowWidth<10>
{
};
///////////////////////////////////////////////////////////////////////////////
template<int32_t HEIGHT>
class FixtureVariantLevelInitShowHeight // Default: FixtureVariantLevelInitShowHeight<6>
{
};

///////////////////////////////////////////////////////////////////////////////
template<int32_t WIDTH>
class FixtureVariantLevelInitSubshowWidth // Default: FixtureVariantLevelInitSubshowWidth<6>
{
};
///////////////////////////////////////////////////////////////////////////////
template<int32_t HEIGHT>
class FixtureVariantLevelInitSubshowHeight // Default: FixtureVariantLevelInitSubshowHeight<4>
{
};


///////////////////////////////////////////////////////////////////////////////
class FixtureVariantLevelInit
{
public:
	virtual ~FixtureVariantLevelInit() = default;
protected:
	int32_t getBoardWidth()
	{
		if (dynamic_cast<FixtureVariantLevelInitBoardWidth<1>*>(this) != nullptr) {
			return 1;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardWidth<2>*>(this) != nullptr) {
			return 2;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardWidth<3>*>(this) != nullptr) {
			return 3;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardWidth<4>*>(this) != nullptr) {
			return 4;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardWidth<5>*>(this) != nullptr) {
			return 5;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardWidth<6>*>(this) != nullptr) {
			return 6;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardWidth<7>*>(this) != nullptr) {
			return 7;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardWidth<8>*>(this) != nullptr) {
			return 8;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardWidth<9>*>(this) != nullptr) {
			return 9;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardWidth<10>*>(this) != nullptr) {
			return 10;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardWidth<11>*>(this) != nullptr) {
			return 11;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardWidth<12>*>(this) != nullptr) {
			return 12;
		}
		return 10;
	}
	int32_t getBoardHeight()
	{
		if (dynamic_cast<FixtureVariantLevelInitBoardHeight<1>*>(this) != nullptr) {
			return 1;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardHeight<2>*>(this) != nullptr) {
			return 2;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardHeight<3>*>(this) != nullptr) {
			return 3;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardHeight<4>*>(this) != nullptr) {
			return 4;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardHeight<5>*>(this) != nullptr) {
			return 5;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardHeight<7>*>(this) != nullptr) {
			return 7;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardHeight<8>*>(this) != nullptr) {
			return 8;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardHeight<9>*>(this) != nullptr) {
			return 9;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardHeight<10>*>(this) != nullptr) {
			return 10;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardHeight<11>*>(this) != nullptr) {
			return 11;
		} else if (dynamic_cast<FixtureVariantLevelInitBoardHeight<12>*>(this) != nullptr) {
			return 12;
		}
		return 6;
	}
	int32_t getShowWidth()
	{
		if (dynamic_cast<FixtureVariantLevelInitShowWidth<1>*>(this) != nullptr) {
			return 1;
		} else if (dynamic_cast<FixtureVariantLevelInitShowWidth<2>*>(this) != nullptr) {
			return 2;
		} else if (dynamic_cast<FixtureVariantLevelInitShowWidth<3>*>(this) != nullptr) {
			return 3;
		} else if (dynamic_cast<FixtureVariantLevelInitShowWidth<4>*>(this) != nullptr) {
			return 4;
		} else if (dynamic_cast<FixtureVariantLevelInitShowWidth<5>*>(this) != nullptr) {
			return 5;
		} else if (dynamic_cast<FixtureVariantLevelInitShowWidth<6>*>(this) != nullptr) {
			return 6;
		} else if (dynamic_cast<FixtureVariantLevelInitShowWidth<7>*>(this) != nullptr) {
			return 7;
		} else if (dynamic_cast<FixtureVariantLevelInitShowWidth<8>*>(this) != nullptr) {
			return 8;
		} else if (dynamic_cast<FixtureVariantLevelInitShowWidth<9>*>(this) != nullptr) {
			return 9;
		}
		return 10;
	}
	int32_t getShowHeight()
	{
		if (dynamic_cast<FixtureVariantLevelInitShowHeight<1>*>(this) != nullptr) {
			return 1;
		} else if (dynamic_cast<FixtureVariantLevelInitShowHeight<2>*>(this) != nullptr) {
			return 2;
		} else if (dynamic_cast<FixtureVariantLevelInitShowHeight<3>*>(this) != nullptr) {
			return 3;
		} else if (dynamic_cast<FixtureVariantLevelInitShowHeight<4>*>(this) != nullptr) {
			return 4;
		} else if (dynamic_cast<FixtureVariantLevelInitShowHeight<5>*>(this) != nullptr) {
			return 5;
		} else if (dynamic_cast<FixtureVariantLevelInitShowHeight<7>*>(this) != nullptr) {
			return 7;
		} else if (dynamic_cast<FixtureVariantLevelInitShowHeight<8>*>(this) != nullptr) {
			return 8;
		} else if (dynamic_cast<FixtureVariantLevelInitShowHeight<9>*>(this) != nullptr) {
			return 9;
		} else if (dynamic_cast<FixtureVariantLevelInitShowHeight<10>*>(this) != nullptr) {
			return 10;
		}
		return 6;
	}
	int32_t getSubshowWidth()
	{
		if (dynamic_cast<FixtureVariantLevelInitSubshowWidth<1>*>(this) != nullptr) {
			return 1;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowWidth<2>*>(this) != nullptr) {
			return 2;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowWidth<3>*>(this) != nullptr) {
			return 3;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowWidth<4>*>(this) != nullptr) {
			return 4;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowWidth<5>*>(this) != nullptr) {
			return 5;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowWidth<7>*>(this) != nullptr) {
			return 7;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowWidth<8>*>(this) != nullptr) {
			return 8;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowWidth<9>*>(this) != nullptr) {
			return 9;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowWidth<10>*>(this) != nullptr) {
			return 10;
		}
		return 6;
	}
	int32_t getSubshowHeight()
	{
		if (dynamic_cast<FixtureVariantLevelInitSubshowHeight<1>*>(this) != nullptr) {
			return 1;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowHeight<2>*>(this) != nullptr) {
			return 2;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowHeight<3>*>(this) != nullptr) {
			return 3;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowHeight<5>*>(this) != nullptr) {
			return 5;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowHeight<6>*>(this) != nullptr) {
			return 6;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowHeight<7>*>(this) != nullptr) {
			return 7;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowHeight<8>*>(this) != nullptr) {
			return 8;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowHeight<9>*>(this) != nullptr) {
			return 9;
		} else if (dynamic_cast<FixtureVariantLevelInitSubshowHeight<10>*>(this) != nullptr) {
			return 10;
		}
		return 4;
	}
	/* Board initialization.
	 * If not implemented board will be all empty tiles.
	 * @param nBoardW The board width.
	 * @param nBoardH The board height.
	 * @param aBoard The board tiles. Size: `nBoardW * nBoardH`.
	 */
	virtual void fillBoard(int32_t /*nBoardW*/, int32_t /*nBoardH*/, std::vector<Tile>& /*aBoard*/)
	{
		//aBoard[(nBoardW - 1) + (nBoardH - 1) * nBoardW] =
	}

	void getLevelInit(Level::Init& oInit, bool bShowMode)
	{
		oInit.m_nBoardW = getBoardWidth();
		oInit.m_nBoardH = getBoardHeight();
		oInit.m_nShowW = getShowWidth();
		oInit.m_nShowH = getShowHeight();
		assert(oInit.m_nShowW <= oInit.m_nBoardW);
		assert(oInit.m_nShowH <= oInit.m_nBoardH);
		if (! bShowMode) {
			oInit.m_nSubshowW = getSubshowWidth();
			oInit.m_nSubshowH = getSubshowHeight();
			assert(oInit.m_nSubshowW <= oInit.m_nShowW);
			assert(oInit.m_nSubshowH <= oInit.m_nShowH);
		} else {
			oInit.m_nSubshowW = 0;
			oInit.m_nSubshowH = 0;
		}
		//
		oInit.m_aBoard.resize(oInit.m_nBoardW * oInit.m_nBoardH);
		//
		fillBoard(oInit.m_nBoardW, oInit.m_nBoardH, oInit.m_aBoard);
	}
};

} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_VARIANT_LEVEL_INIT_H */
