/*
 * File:   gameinitctx.h
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

#ifndef STMG_GAME_INIT_CTX_H
#define STMG_GAME_INIT_CTX_H

#include "layoutctx.h"

#include <memory>
#include <string>

#include <stdint.h>

namespace stmg { class AppPreferences; }
namespace stmg { class File; }
namespace stmg { class Layout; }
namespace stmg { class Named; }
namespace stmg { class Tile; }

namespace stmg
{

class GameInitCtx : public LayoutCtx
{
public:
	GameInitCtx(const shared_ptr<AppPreferences>& refAppPreferences, Named& oNamed, const File& oFile);
	void setName(const std::string& sGameName);
	shared_ptr<Layout> getLayout() const;
	void setLayout(const shared_ptr<Layout>& refLayout);
	//
	void boardSetSize(int32_t nW, int32_t nH);
	void boardSetTile(int32_t nX, int32_t nY, const Tile& oTile);
	void showSet(int32_t nW, int32_t nH, int32_t nInitialX, int32_t nInitialY);
	void subshowSet(int32_t nW, int32_t nH);
	void setInitialFallEachTicks(int32_t nInitialFallEachTicks);
private:
	void adjustShow();
	void adjustSubshow();
private:
	shared_ptr<Layout> m_refLayout;

private:
	GameInitCtx() = delete;
	GameInitCtx(const GameInitCtx& oSource) = delete;
	GameInitCtx& operator=(const GameInitCtx& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_GAME_INIT_CTX_H */

