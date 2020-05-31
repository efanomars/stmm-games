/*
 * File:   gameinitctx.cc
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

#include "gameinitctx.h"

#include <stmm-games/level.h>
#include <stmm-games/tile.h>
#include <stmm-games/util/basictypes.h>

#include <cassert>
//#include <iostream>
#include <vector>

namespace stmg { class AppPreferences; }
namespace stmg { class File; }
namespace stmg { class Layout; }
namespace stmg { class Named; }

namespace stmg
{

GameInitCtx::GameInitCtx(const shared_ptr<AppPreferences>& refAppPreferences, Named& oNamed, const File& oFile)
: LayoutCtx(refAppPreferences, oNamed, oFile)
{
}
void GameInitCtx::setName(const std::string& sGameName)
{
	m_sGameName = sGameName;
}
shared_ptr<Layout> GameInitCtx::getLayout() const
{
	return m_refLayout;
}
void GameInitCtx::setLayout(const shared_ptr<Layout>& refLayout)
{
	m_refLayout = refLayout;
}
//
void GameInitCtx::boardSetSize(int32_t nW, int32_t nH)
{
	assert((nW > 0) && (nH > 0));
	m_oLevelInit.m_nBoardW = nW;
	m_oLevelInit.m_nBoardH = nH;
	m_oLevelInit.m_aBoard.resize(nW * nH);
	adjustShow();
	adjustSubshow();
}
void GameInitCtx::boardSetTile(int32_t nX, int32_t nY, const Tile& oTile)
{
	if ((nX < 0) || (nX >= m_oLevelInit.m_nBoardW) || (nY < 0) || (nY >= m_oLevelInit.m_nBoardH)) {
		return;
	}
	m_oLevelInit.m_aBoard[m_oLevelInit.getBoardIndex(NPoint{nX, nY})] = oTile;
}
void GameInitCtx::showSet(int32_t nW, int32_t nH, int32_t nInitialX, int32_t nInitialY)
{
	assert((nW > 0) && (nH > 0));
	m_oLevelInit.m_nShowW = nW;
	m_oLevelInit.m_nShowH = nH;
	m_oLevelInit.m_nShowInitX = nInitialX;
	m_oLevelInit.m_nShowInitY = nInitialY;
	adjustSubshow();
}
void GameInitCtx::subshowSet(int32_t nW, int32_t nH)
{
	m_oLevelInit.m_nSubshowW = nW;
	m_oLevelInit.m_nSubshowH = nH;
}
void GameInitCtx::setInitialFallEachTicks(int32_t nInitialFallEachTicks)
{
	m_oLevelInit.m_nInitialFallEachTicks = nInitialFallEachTicks;
}
void GameInitCtx::adjustShow()
{
	if ((m_oLevelInit.m_nShowW <= 0) || (m_oLevelInit.m_nShowW > m_oLevelInit.m_nBoardW)) {
		m_oLevelInit.m_nShowW = m_oLevelInit.m_nBoardW;
	}
	if ((m_oLevelInit.m_nShowH <= 0) || (m_oLevelInit.m_nShowH > m_oLevelInit.m_nBoardH)) {
		m_oLevelInit.m_nShowH = m_oLevelInit.m_nBoardH;
	}
}
void GameInitCtx::adjustSubshow()
{
	if (m_oLevelInit.m_nSubshowW > m_oLevelInit.m_nShowW) {
		m_oLevelInit.m_nSubshowW = m_oLevelInit.m_nShowW;
	}
	if (m_oLevelInit.m_nSubshowH > m_oLevelInit.m_nShowH) {
		m_oLevelInit.m_nSubshowH = m_oLevelInit.m_nShowH;
	}
}

} // namespace stmg
