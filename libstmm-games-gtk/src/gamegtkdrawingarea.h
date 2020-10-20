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
 * File:   gamegtkdrawingarea.h
 */

#ifndef STMG_GAME_GTK_DRAWING_AREA_H
#define STMG_GAME_GTK_DRAWING_AREA_H

#include <gtkmm/drawingarea.h>
#include <cairomm/refptr.h>
#include <cairomm/region.h>

#include <memory>

#include <stdint.h>

namespace stmg { class Game; }
namespace stmg { class StdPreferences; }
namespace stmg { class StdView; }
namespace stmg { class Theme; }

namespace Cairo { class Context; }
namespace Glib { class Timer; }

namespace stmg
{

using std::shared_ptr;

class GameGtkDrawingArea  : public Gtk::DrawingArea
{
public:
	GameGtkDrawingArea(shared_ptr<StdView>& refGameView, Glib::Timer& oTimer) noexcept;
	void buggyDestructor() noexcept;

	bool delayedAllocation() noexcept;

	bool startGame(const shared_ptr<Game>& refGame, const shared_ptr<StdPreferences>& refPrefs
					, const shared_ptr<Theme>& refTheme) noexcept;
	void endGame() noexcept;

	void beforeGameTick() noexcept;
	void sync(double fViewInterval, int32_t nTotViewTicks) noexcept;
	void drawStep(int32_t nViewTick) noexcept;

private:
	bool onCustomDraw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;

	bool isRealized() noexcept;

private:
	shared_ptr<StdView> m_refGameView;

	bool m_bIsRealized;

	int32_t m_nDelayedAllocW;
	int32_t m_nDelayedAllocH;

	Cairo::RefPtr<Cairo::Region> m_refCurrentDrawingRegion;

private:
	GameGtkDrawingArea(const GameGtkDrawingArea& oSource) = delete;
	GameGtkDrawingArea& operator=(const GameGtkDrawingArea& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_GAME_GTK_DRAWING_AREA_H */

