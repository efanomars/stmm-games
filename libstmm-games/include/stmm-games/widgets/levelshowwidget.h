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
 * File:   levelshowwidget.h
 */

#ifndef STMG_LEVEL_SHOW_WIDGET_H
#define STMG_LEVEL_SHOW_WIDGET_H

#include "gamewidget.h"
#include "gameproxy.h"

#include <memory>

#include <stdint.h>

namespace stmg { class Level; }
namespace stmi { class Event; }

namespace stmg
{

class LevelShow;

/** The widget that displays the level show area or a mate's subshow area.
 */
class LevelShowWidget : public GameWidget
{
public:
	struct LocalInit
	{
		double m_fTopInTiles = 0; /**< The top margin in tiles. Default: `0.0`.*/
		double m_fBottomInTiles = 0; /**< The bottom margin in tiles. Default: `0.0`.*/
		double m_fLeftInTiles = 0; /**< The left margin in tiles. Default: `0.0`.*/
		double m_fRightInTiles = 0; /**< The right margin in tiles. Default: `0.0`.*/
		bool m_bIsInteractive = true; /**< Whether the widget should receive XYEvents. Default: true. */
	};
	struct Init : public GameWidget::Init, public LocalInit
	{
	};
	/** The shown width in tiles.
	 * Margins excluded.
	 * @return The width in tiles.
	 */
	double getWidth() const noexcept;
	/** The shown height in tiles.
	 * Margins excluded.
	 * @return The height in tiles.
	 */
	double getHeight() const noexcept;
	/** The top margin in tiles.
	 * @return The top margin in tiles.
	 */
	double getTopMarginInTiles() const noexcept { return m_oData.m_fTopInTiles; }
	/** The bottom margin in tiles.
	 * @return The bottom margin in tiles.
	 */
	double getBottomMarginInTiles() const noexcept { return m_oData.m_fBottomInTiles; }
	/** The left margin in tiles.
	 * @return The left margin in tiles.
	 */
	double getLeftMarginInTiles() const noexcept { return m_oData.m_fLeftInTiles; }
	/** The right margin in tiles.
	 * @return The right margin in tiles.
	 */
	double getRightMarginInTiles() const noexcept { return m_oData.m_fRightInTiles; }

	/** Whether the widget should receive events of type stmi::XYEvent.
	 * @return Whether interactive.
	 */
	bool isInteractive() const noexcept { return m_oData.m_bIsInteractive; }

	/** Whether the widget displays the level's show area or mate's subshow area.
	 * @return Whether subshow mode.
	 */
	bool isSubshow() const noexcept;
	/** Sends an input event generated into the widget to a player.
	 * The input event can also be generated outside of the widget's rectangle.
	 * The event's x and y are in tiles, not pixels!
	 * @param refXYEvent The event generated in the view. Cannot be null.
	 * @param nTeam The team of the player. Must be `&gt;=0`.
	 * @param nMate The mate number of the player. Must be `&gt;=0`.
	 */
	inline void handleXYInput(const shared_ptr<stmi::Event>& refXYEvent
							, int32_t nTeam, int32_t nMate) noexcept
	{
		game().handleInput(nTeam, nMate, refXYEvent);
	}

	/** Constructor.
	 * oInit.m_nLevel must be `&gt;=0`.
	 * @param oInit The initialization data.
	 */
	explicit LevelShowWidget(Init&& oInit) noexcept;
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
protected:
	/** Reinitialization.
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept;
	//
	void onAddedToGame() noexcept override;
private:
	void commonInit() noexcept;
private:
	LocalInit m_oData;
	Level* m_p0Level;
	LevelShow* m_p0Show;
};


} // namespace stmg

#endif	/* STMG_LEVEL_SHOW_WIDGET_H */

