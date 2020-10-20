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
 * File:   gamewidget.cc
 */

#include "gamewidget.h"

#include "apppreferences.h"
#include "layout.h"

#include <cassert>
#include <iostream>
#include <utility>

namespace stmg { class GameProxy; }

namespace stmg
{

#ifndef NDEBUG
void WidgetAxisFit::dump(int32_t nIndentSpaces) const noexcept
{
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent;
	std::cout << "Expand=" << (m_bExpand ? "T" : "F");
	std::cout << " Align=";
	if (m_eAlign == ALIGN_FILL) {
		std::cout << "FILL";
	} else if (m_eAlign == ALIGN_START) {
		std::cout << "START";
	} else if (m_eAlign == ALIGN_CENTER) {
		std::cout << "CENTER";
	} else if (m_eAlign == ALIGN_END) {
		std::cout << "END";
	} else {
		assert(false);
	}
	std::cout << '\n';
}
void WidgetAxisFit::dump() const noexcept
{
	dump(0);
}
#endif //NDEBUG


WidgetAxisFit GameWidget::getWidgetHorizAxisFit(int32_t nLayoutConfig) const noexcept
{
	assert((nLayoutConfig >= 0) && (nLayoutConfig < g_nTotLayoutConfigs));
	return m_oData.m_aHorizAxisFit[nLayoutConfig];
}
WidgetAxisFit GameWidget::getWidgetVertAxisFit(int32_t nLayoutConfig) const noexcept
{
	assert((nLayoutConfig >= 0) && (nLayoutConfig < g_nTotLayoutConfigs));
	return m_oData.m_aVertAxisFit[nLayoutConfig];
}
GameWidget::GameWidget(Init&& oData) noexcept
: m_oData(std::move(oData))
{
	commonInit();
}
void GameWidget::reInit(Init&& oData) noexcept
{
	m_oData = std::move(oData);
	commonInit();
}
Layout& GameWidget::layout() noexcept
{
	assert(m_p0Layout != nullptr);
	return *m_p0Layout;
}
const Layout& GameWidget::layout() const noexcept
{
	assert(m_p0Layout != nullptr);
	return *m_p0Layout;
}
GameProxy& GameWidget::game() noexcept
{
	assert(m_p0GameProxy != nullptr);
	return *m_p0GameProxy;
}
const GameProxy& GameWidget::game() const noexcept
{
	assert(m_p0GameProxy != nullptr);
	return *m_p0GameProxy;
}
void GameWidget::commonInit() noexcept
{
	assert((m_oData.m_nViewWidgetNameIdx == -1) || (m_oData.m_nViewWidgetNameIdx >= 0));
	assert(m_oData.m_nTeam >= -1);
	assert(((m_oData.m_nMate >= 0) && (m_oData.m_nTeam >= 0)) || (m_oData.m_nMate == -1));
	m_bIsOrContainsHumanPlayers = false;
	m_p1Owner = nullptr;
	m_p0GameProxy = nullptr;
	m_p0Layout = nullptr;
}

void GameWidget::setLayout(Layout* p0Layout) noexcept
{
	if (p0Layout != nullptr) {
		const auto& oAppPrefs = *(p0Layout->getPrefs());
		if (m_oData.m_nTeam >= 0) {
			auto refPrefTeam = oAppPrefs.getTeam(m_oData.m_nTeam);
			assert(refPrefTeam);
			if (m_oData.m_nMate >= 0) {
				m_bIsOrContainsHumanPlayers = !refPrefTeam->getMate(m_oData.m_nMate)->isAI();
			} else {
				m_bIsOrContainsHumanPlayers = !refPrefTeam->isAI();
			}
		} else {
			m_bIsOrContainsHumanPlayers = (oAppPrefs.getTotHumanTeams() > 0);
		}
	}
	m_p0Layout = p0Layout;
}
void GameWidget::setGameProxy(GameProxy* p0GameProxy) noexcept
{
	assert(p0GameProxy != nullptr);
	m_p0GameProxy = p0GameProxy;
}
void GameWidget::setTeam(int32_t nTeam) noexcept
{
	m_oData.m_nTeam = nTeam;
	if (nTeam < 0) {
		m_oData.m_nMate = -1;
	}
}

void GameWidget::dump(int32_t
#ifndef NDEBUG
nIndentSpaces
#endif //NDEBUG
, bool /*bHeader*/) const noexcept
{
	#ifndef NDEBUG
	auto sIndent = std::string(nIndentSpaces, ' ');

	for (int32_t nIdx = 0; nIdx < g_nTotLayoutConfigs; ++nIdx) {
		std::cout << sIndent << "m_aHorizAxisFit[" << nIdx << "]: "; m_oData.m_aHorizAxisFit[nIdx].dump();
		std::cout << sIndent << "m_aVertAxisFit[" << nIdx << "]:  "; m_oData.m_aVertAxisFit[nIdx].dump();
	}
	std::cout << sIndent << "m_nTeam:              " << m_oData.m_nTeam << '\n';
	std::cout << sIndent << "m_nMate:              " << m_oData.m_nMate << '\n';
	std::cout << sIndent << "m_nViewWidgetNameIdx: " << m_oData.m_nViewWidgetNameIdx << '\n';
	std::cout << sIndent << "m_sName:              " << m_oData.m_sName << '\n';
	#endif //NDEBUG
}
void GameWidget::dump(int32_t nIndentSpaces) const noexcept
{
	dump(nIndentSpaces, true);
}
/** Writes object contents to std::cout (header included).
 */
void GameWidget::dump() const noexcept
{
	dump(0);
}

} // namespace stmg
