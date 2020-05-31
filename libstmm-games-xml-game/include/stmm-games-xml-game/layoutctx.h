/*
 * File:   layoutctx.h
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

#ifndef STMG_LAYOUT_CTX_H
#define STMG_LAYOUT_CTX_H

#include "gameinfoctx.h"

#include <stmm-games/level.h>

#include <memory>
#include <string>
#include <vector>

#include <stdint.h>

namespace stmg { class AppPreferences; }
namespace stmg { class File; }
namespace stmg { class Named; }
namespace stmg { class Tile; }

namespace stmg
{

class LayoutCtx : public GameInfoCtx
{
public:
	LayoutCtx(const shared_ptr<AppPreferences>& refAppPreferences, Named& oNamed, const File& oFile)
	: GameInfoCtx(refAppPreferences, oNamed, oFile)
	, m_nTeam(-1)
	, m_nMate(-1)
	{
	}
	int32_t getContextTeam() const override { return m_nTeam; }
	int32_t getContextMate() const override { return m_nMate; }
protected:
	friend class XmlLayoutParser;
	void setTeam(int32_t nTeam)
	{
		m_nTeam = nTeam;
	}
	void setMate(int32_t nMate)
	{
		m_nMate = nMate;
	}
	void setName(const std::string& sGameName)
	{
		m_sGameName = sGameName;
	}
public:
	std::string getName() const
	{
		return m_sGameName;
	}
	inline Level::Init& getLevelInit() { return m_oLevelInit; }
	int32_t boardGetWidth() const
	{
		return m_oLevelInit.m_nBoardW;
	}
	int32_t boardGetHeight() const
	{
		return m_oLevelInit.m_nBoardH;
	}
	const std::vector< Tile >& boardGet() const
	{
		return m_oLevelInit.m_aBoard;
	}
	int32_t showGetWidth() const
	{
		return m_oLevelInit.m_nShowW;
	}
	int32_t showGetHeight() const
	{
		return m_oLevelInit.m_nShowH;
	}
	int32_t showGetInitialX() const
	{
		return m_oLevelInit.m_nShowInitX;
	}
	int32_t showGetInitialY() const
	{
		return m_oLevelInit.m_nShowInitY;
	}
	bool subshowIsSet() const
	{
		return (m_oLevelInit.m_nSubshowW > 0) && (m_oLevelInit.m_nSubshowH > 0);
	}
	int32_t subshowGetWidth() const
	{
		return m_oLevelInit.m_nSubshowW;
	}
	int32_t subshowGetHeight() const
	{
		return m_oLevelInit.m_nSubshowH;
	}
	int32_t getInitialFallEachTicks() const
	{
		return m_oLevelInit.m_nInitialFallEachTicks;
	}
private:
	friend class GameInitCtx;
	int32_t m_nTeam;
	int32_t m_nMate;
	Level::Init m_oLevelInit;
	std::string m_sGameName;
private:
	LayoutCtx() = delete;
	LayoutCtx(const LayoutCtx& oSource) = delete;
	LayoutCtx& operator=(const LayoutCtx& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_LAYOUT_CTX_H */

