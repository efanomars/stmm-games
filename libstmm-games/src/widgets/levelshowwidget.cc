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
 * File:   levelshowwidget.cc
 */

#include "widgets/levelshowwidget.h"

#include "level.h"
#include "levelshow.h"

#include <cassert>
#include <iostream>
#include <string>
#include <utility>


namespace stmg
{

LevelShowWidget::LevelShowWidget(Init&& oInit) noexcept
: GameWidget(std::move(oInit))
, m_oData(std::move(oInit))
{
	commonInit();
}
void LevelShowWidget::reInit(Init&& oInit) noexcept
{
	GameWidget::reInit(std::move(oInit));
	m_oData = std::move(oInit);
	commonInit();
}
void LevelShowWidget::commonInit() noexcept
{
	m_p0Level = nullptr;
	m_p0Show = nullptr;
}
void LevelShowWidget::onAddedToGame() noexcept
{
	GameWidget::onAddedToGame();

	const int32_t nTeam = getTeam();
	const bool bATIOL = game().isAllTeamsInOneLevel();
	const int32_t nLevel = (bATIOL ? 0 : nTeam);
	const int32_t nLevelTeam = (bATIOL ? nTeam : 0);
	const int32_t nMate = getMate();
//std::cout << "LevelShowWidget::setGame nTeam=" << nTeam << " nMate=" << nMate << '\n';
	auto& oLevel = game().level(nLevel);
	m_p0Level = &oLevel;
	//
	if ((nLevelTeam >= 0) && (nMate >= 0)) {
		assert( m_p0Level->subshowMode() );
		m_p0Show = &(m_p0Level->subshowGet(nLevelTeam, nMate));
	} else {
		assert( ! m_p0Level->subshowMode() );
		m_p0Show = &(m_p0Level->showGet());
	}
//std::cout << "LevelShowWidget::setGame W=" << m_p0Show->getW() << " H=" << m_p0Show->getH() << '\n';
}
bool LevelShowWidget::isSubshow() const noexcept
{
	const int32_t nMate = getMate();
	return (nMate >= 0);
}
double LevelShowWidget::getWidth() const noexcept
{
	assert(m_p0Show != nullptr);
	return m_p0Show->getW();
}
double LevelShowWidget::getHeight() const noexcept
{
	assert(m_p0Show != nullptr);
	return m_p0Show->getH();
}

void LevelShowWidget::dump(int32_t
#ifndef NDEBUG
nIndentSpaces
#endif //NDEBUG
, bool
#ifndef NDEBUG
bHeader
#endif //NDEBUG
) const noexcept
{
	#ifndef NDEBUG
	auto sIndent = std::string(nIndentSpaces, ' ');
	if (bHeader) {
		std::cout << sIndent << "LevelShowWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	GameWidget::dump(nIndentSpaces + 2, false);
	std::cout << sIndent << "  " << "m_fTopInTiles:    " << m_oData.m_fTopInTiles << '\n';
	std::cout << sIndent << "  " << "m_fBottomInTiles: " << m_oData.m_fBottomInTiles << '\n';
	std::cout << sIndent << "  " << "m_fLeftInTiles:   " << m_oData.m_fLeftInTiles << '\n';
	std::cout << sIndent << "  " << "m_fRightInTiles:  " << m_oData.m_fRightInTiles << '\n';
	std::cout << sIndent << "  " << "m_bIsInteractive: " << m_oData.m_bIsInteractive << '\n';
	#endif //NDEBUG
}

} // namespace stmg
