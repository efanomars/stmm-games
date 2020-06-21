/*
 * File:   keyactionsboxthwidgetfactory.cc
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

#include "widgets/actionsboxthwidgetfactory.h"

#include <stmm-games/widgets/actionsboxwidget.h>

#include <stmm-games/util/basictypes.h>

#include <stmm-input/xyevent.h>

#include <cstdint>
#include <iostream>
#include <string>

namespace stmi { class Event; }

namespace stmg
{

void ActionsBoxThWidget::dump(int32_t
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
		std::cout << sIndent << "ActionsBoxThWidget adr:" << reinterpret_cast<int64_t>(this) << '\n';
	}
	const NSize oPixSize = getSize();
	std::cout << sIndent << "  m_aColX: 0";
	for (int32_t nPixX : m_aColX) {
		std::cout << ", " << nPixX;
	}
	std::cout << ", " << oPixSize.m_nW << '\n';
	std::cout << sIndent << "  m_aRowY: 0";
	for (int32_t nPixY : m_aRowY) {
		std::cout << ", " << nPixY;
	}
	std::cout << ", " << oPixSize.m_nH << '\n';
	BoxThemeWidget<ActionsBoxThWidgetFactory, ActionsBoxThWidget, ActionsBoxWidget>::dump(nIndentSpaces, false);
	#endif //NDEBUG
}
void ActionsBoxThWidget::sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept
{
	BoxThemeWidget<ActionsBoxThWidgetFactory, ActionsBoxThWidget, ActionsBoxWidget>::sizeAndConfig(nTileW, nLayoutConfig);
	calcGrid();
}
void ActionsBoxThWidget::calcGrid() noexcept
{
	ActionsBoxWidget* p0ActionsBoxWidget = getBoxWidgetModel();
	const NSize oPixSize = getSize();
	m_aColX.clear();
	m_aRowY.clear();
	const int32_t nTotCols = p0ActionsBoxWidget->getTotColumns();
	const int32_t nTotRows = p0ActionsBoxWidget->getTotRows();
	const int32_t nSumColParts = p0ActionsBoxWidget->getSumOfAllColumnParts();
	const int32_t nSumRowParts = p0ActionsBoxWidget->getSumOfAllRowParts();
	int32_t nCumulColPart = 0;
	for (int32_t nCol = 0; nCol < nTotCols - 1; ++nCol) {
		nCumulColPart += p0ActionsBoxWidget->getColumnPart(nCol);
		int32_t nColX = 1.0 * oPixSize.m_nW * nCumulColPart / (1.0 * nSumColParts);
		m_aColX.push_back(nColX);
	}
	int32_t nCumulRowPart = 0;
	for (int32_t nRow = 0; nRow < nTotRows - 1; ++nRow) {
		nCumulRowPart += p0ActionsBoxWidget->getRowPart(nRow);
		int32_t nRowY = 1.0 * oPixSize.m_nH * nCumulRowPart / (1.0 * nSumRowParts);
		m_aRowY.push_back(nRowY);
	}
}

bool ActionsBoxThWidget::handleXYInput(const shared_ptr<stmi::Event>& refXYEvent, int32_t nTeam, int32_t nMate) noexcept
{
	const auto p0XYEvent = static_cast<stmi::XYEvent*>(refXYEvent.get());
	if (p0XYEvent->getXYGrabType() == stmi::XYEvent::XY_HOVER) {
		return false; //--------------------------------------------------------
	}
	const NPoint oPixPos = getPos();
	const int32_t nRelX = p0XYEvent->getX() - oPixPos.m_nX;
	const int32_t nRelY = p0XYEvent->getY() - oPixPos.m_nY;
	int32_t nIdxCol = 0;
	const int32_t nTotCumulCols = static_cast<int32_t>(m_aColX.size());
	for (nIdxCol = 0; nIdxCol < nTotCumulCols; ++nIdxCol) {
		if (nRelX < m_aColX[nIdxCol]) {
			break;
		}
	}
	int32_t nIdxRow = 0;
	const int32_t nTotCumulRows = static_cast<int32_t>(m_aRowY.size());
	for (nIdxRow = 0; nIdxRow < nTotCumulRows; ++nIdxRow) {
		if (nRelY < m_aRowY[nIdxRow]) {
			break;
		}
	}
//if (p0XYEvent->getXYGrabType() == stmi::XYEvent::XY_GRAB) {
//std::cout << "ActionsBoxThWidget::handleXYEvent  nIdxCol=" << nIdxCol << " nIdxRow=" << nIdxRow << '\n';
//}
	ActionsBoxWidget* p0ActionsBoxWidget = getBoxWidgetModel();
	p0ActionsBoxWidget->handleXYInput(refXYEvent, nTeam, nMate, nIdxCol, nIdxRow);
	return true;
}


} // namespace stmg
