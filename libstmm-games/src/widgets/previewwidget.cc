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
 * File:   previewwidget.cc
 */

#include "widgets/previewwidget.h"

#include "gameproxy.h"

#include <cassert>
#include <algorithm>
#include <iostream>
#include <utility>

namespace stmg
{

PreviewWidget::PreviewWidget(Init&& oInit) noexcept
: RelSizedGameWidget(std::move(oInit))
, m_oData(std::move(oInit))
, m_nChangeGameTick(-1)
{
}
void PreviewWidget::reInit(Init&& oInit) noexcept
{
	RelSizedGameWidget::reInit(std::move(oInit));
	m_oData = std::move(oInit);
	m_sText.clear();
	m_aBlocks.clear();
	m_nChangeGameTick = -1;
}

void PreviewWidget::changed() noexcept
{
	assert(game().isInGameTick());
	m_nChangeGameTick = game().gameElapsed();
}
bool PreviewWidget::isChanged() const noexcept
{
	const int32_t nTick = game().gameElapsed();
	if (game().isInGameTick()) {
		return m_nChangeGameTick == nTick;
	} else {
		return m_nChangeGameTick == (nTick - 1);
	}
}
void PreviewWidget::set(const std::string& sText, std::vector<Block>& oBlocks) noexcept
{
	assert(game().isInGameTick());
	m_sText = sText;
	m_aBlocks = oBlocks;
	changed();
}

void PreviewWidget::dump(int32_t
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
		std::cout << sIndent << "PreviewWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	RelSizedGameWidget::dump(nIndentSpaces + 2, false);
	std::cout << sIndent << "  " << "m_nMinTilesW:        " << m_oData.m_nMinTilesW << '\n';
	std::cout << sIndent << "  " << "m_nMinTilesH:        " << m_oData.m_nMinTilesH << '\n';
	if (! m_sText.empty()) {
		std::cout << sIndent << "  " << "m_sText:             " << m_sText << '\n';
	}
	if (! m_aBlocks.empty()) {
		for (const auto& oBlock : m_aBlocks) {
			std::cout << sIndent << "  " << "Block:" << '\n';
			oBlock.dump(nIndentSpaces + 2 + 2);
		}
	}
	#endif //NDEBUG
}

} // namespace stmg
