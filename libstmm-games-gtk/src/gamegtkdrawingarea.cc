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
 * File:   gamegtkdrawingarea.cc
 */

#include "gamegtkdrawingarea.h"

#include "stdview.h"

#include <stmm-games/util/basictypes.h>

#include <gtkmm.h>
#include <cairomm/context.h>

//#include <iostream>
//#include <cassert>
#include <algorithm>
#include <utility>

namespace stmg { class Game; }
namespace stmg { class Theme; }


namespace stmg
{

GameGtkDrawingArea::GameGtkDrawingArea(shared_ptr<StdView>& refGameView, Glib::Timer& /*oTimer*/) noexcept
: m_refGameView(refGameView)
, m_bIsRealized(false)
, m_nDelayedAllocW(-777)
, m_nDelayedAllocH(-777)
	//, m_oTimer(oTimer)
{
	signal_draw().connect(sigc::mem_fun(*this, &GameGtkDrawingArea::onCustomDraw));
}
void GameGtkDrawingArea::buggyDestructor() noexcept
{
	// This is needed because the  destructor of this class isn't called
	// when its parent is destroyed
	//TODO figure out what's going on
	m_refGameView.reset();
//std::cout << "GameGtkDrawingArea::~GameGtkDrawingArea   Delete this destructor" << '\n';
}

bool GameGtkDrawingArea::isRealized() noexcept
{
	return m_bIsRealized;
}

bool GameGtkDrawingArea::startGame(const shared_ptr<Game>& refGame, const shared_ptr<StdPreferences>& refPrefs
									, const shared_ptr<Theme>& refTheme) noexcept
{
//std::cout << "GameGtkDrawingArea::startGame: " << m_oTimer.elapsed()
//<< "  delayW=" << m_nDelayedAllocW
//<< "  delayH=" << m_nDelayedAllocH
//<< '\n';

	m_bIsRealized = true;

	Gtk::Allocation oAllocation = get_allocation();
	const int nW = oAllocation.get_width();
	const int nH = oAllocation.get_height();
//std::cout << "GameGtkDrawingArea::startGame: " << m_oTimer.elapsed()
//<< "  oAllocation.W=" << nW
//<< "  oAllocation.H=" << nH
//<< '\n';
	const std::pair<bool, NRect> oPair = m_refGameView->reInit(refGame, refPrefs, refTheme, NSize{nW, nH}, get_pango_context());
	const bool bOk = oPair.first;
	if (!bOk) {
		return false; //--------------------------------------------------------
	}

	const NRect& oActiveRect = oPair.second;
//std::cout << "GameGtkDrawingArea::startGame: active rect "
//<< "  m_nX=" << oActiveRect.m_nX
//<< "  m_nY=" << oActiveRect.m_nY
//<< "  m_nW=" << oActiveRect.m_nW
//<< "  m_nH=" << oActiveRect.m_nH
//<< '\n';
	// Create the region for the drawStep calls
	Cairo::RectangleInt oRect;
	oRect.x = oActiveRect.m_nX;
	oRect.y = oActiveRect.m_nY;
	oRect.width = oActiveRect.m_nW;
	oRect.height = oActiveRect.m_nH;
	m_refCurrentDrawingRegion = Cairo::Region::create(oRect);

	gdk_window_invalidate_rect(get_window()->gobj(), nullptr, true);

	return true;
}
void GameGtkDrawingArea::endGame() noexcept
{
}

void GameGtkDrawingArea::beforeGameTick() noexcept
{
	m_refGameView->beforeGameTick();
}
void GameGtkDrawingArea::sync(double fViewInterval, int32_t nTotViewTicks) noexcept
{
	m_refGameView->sync(fViewInterval, nTotViewTicks);
}

void GameGtkDrawingArea::drawStep(int32_t nViewTick) noexcept
{
	Glib::RefPtr<Gdk::Window> refGdkWindow = get_window();
	if (!refGdkWindow) {
		return;
	}
	auto refDrawingCtx = refGdkWindow->begin_draw_frame(m_refCurrentDrawingRegion);
	auto refCc = refDrawingCtx->get_cairo_context();
	//Cairo::RefPtr<Cairo::Context> refCc = refGdkWindow->create_cairo_context();
	m_refGameView->drawStep(nViewTick, refCc);
	refGdkWindow->end_draw_frame(refDrawingCtx);
}

bool GameGtkDrawingArea::onCustomDraw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept
{
//std::cout << "GameGtkDrawingArea::onCustomDraw() time: " << m_oTimer.elapsed() << '\n';

	// This is where we draw on the window
	Glib::RefPtr<Gdk::Window> refWindow = get_window();
	if (!refWindow) {
		return true; //---------------------------------------------------------
	}

	Gtk::Allocation oAllocation = get_allocation();
	const int32_t nWidth = oAllocation.get_width();
	const int32_t nHeight = oAllocation.get_height();
//std::cout << "GameGtkDrawingArea::onCustomDraw() "
//<< "  x=" << oAllocation.get_x()
//<< "  y=" << oAllocation.get_y()
//<< "  w=" << oAllocation.get_width()
//<< "  h=" << oAllocation.get_height()
//<< "  delayW=" << m_nDelayedAllocW
//<< "  delayH=" << m_nDelayedAllocH
//<< '\n';

	if (m_nDelayedAllocW < 0) {
		m_nDelayedAllocW = std::max(nWidth, 35);
		m_nDelayedAllocH = std::max(nHeight, 35);
	}

	refCc->save();
	m_refGameView->redraw(refCc);
	refCc->restore();

	return true;
}

bool GameGtkDrawingArea::delayedAllocation() noexcept
{
//std::cout << "GameGtkDrawingArea::delayedAllocation: " << m_oTimer.elapsed() << '\n';
	const Gtk::Allocation oAllocation = get_allocation();
//std::cout << "GameGtkDrawingArea::delayedAllocation: "
//<< "  x=" << oAllocation.get_x()
//<< "  y=" << oAllocation.get_y()
//<< "  w=" << oAllocation.get_width()
//<< "  h=" << oAllocation.get_height()
//<< "  delayW=" << m_nDelayedAllocW
//<< "  delayH=" << m_nDelayedAllocH
//<< '\n';

	const auto nAlloW = oAllocation.get_width();
	const auto nAlloH = oAllocation.get_height();
	if ((m_nDelayedAllocW == nAlloW) && (m_nDelayedAllocH == nAlloH)) {
		return false; //--------------------------------------------------------
	}
	m_nDelayedAllocW = nAlloW;
	m_nDelayedAllocH = nAlloH;
	const std::pair<bool, NRect> oPair = m_refGameView->resizeCanvas({m_nDelayedAllocW, m_nDelayedAllocH});
	const bool bOk = oPair.first;
	if (!bOk) {
		return false; //--------------------------------------------------------
	}

	const NRect& oActiveRect = oPair.second;
//std::cout << "GameGtkDrawingArea::delayedAllocation: active rect "
//<< "  m_nX=" << oActiveRect.m_nX
//<< "  m_nY=" << oActiveRect.m_nY
//<< "  m_nW=" << oActiveRect.m_nW
//<< "  m_nH=" << oActiveRect.m_nH
//<< '\n';
	// Create the region for the drawStep calls
	Cairo::RectangleInt oRect;
	oRect.x = oActiveRect.m_nX;
	oRect.y = oActiveRect.m_nY;
	oRect.width = oActiveRect.m_nW;
	oRect.height = oActiveRect.m_nH;
	m_refCurrentDrawingRegion = Cairo::Region::create(oRect);

	// redraw
	return true;
}

} // namespace stmg
