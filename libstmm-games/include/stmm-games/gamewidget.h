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
 * File:   gamewidget.h
 */

#ifndef STMG_GAME_WIDGET_H
#define STMG_GAME_WIDGET_H

#include <memory>
//#include <iostream>
#include <array>
#include <string>

#include <stdint.h>

namespace stmg { class GameProxy; }
namespace stmg { class Layout; }

namespace stmg
{

using std::shared_ptr;

/** The number of layouts configurations.
 */
static const constexpr int32_t g_nTotLayoutConfigs = 2;

class ContainerWidget;

////////////////////////////////////////////////////////////////////////////////
/** Widget alignment position and expansion behavior for one axis.
 */
struct WidgetAxisFit
{
	/** Widget alignment.
	 */
	enum ALIGN
	{
		ALIGN_FILL = 0 /**< Widget should fill all the available space. */
		, ALIGN_START = 1 /**< Depending on direction place at top or left of available space. */
		, ALIGN_CENTER = 2 /**< Place at center of available space. */
		, ALIGN_END = 3 /**< Depending on direction place at bottom or right of available space. */
	};

	/** Default constructor.
	 */
	WidgetAxisFit() noexcept
	{
	}
	/** Constructor.
	 */
	WidgetAxisFit(bool bExpand, ALIGN eAlign) noexcept
	: m_bExpand(bExpand)
	, m_eAlign(eAlign)
	{
	}
	bool m_bExpand = false; /**< Whether the widgets claims a share of extra
							 * available space in the container. Default: false. */
	ALIGN m_eAlign = ALIGN_CENTER; /**< The widget alignment. Default: ALIGN_CENTER. */

	/** Returns the align as a from 0 to 1 or -1 if ALIGN_FILL.
	 * @return The align as a number.
	 */
	double getAlign() const noexcept
	{
		switch (m_eAlign) {
		case WidgetAxisFit::ALIGN_FILL: return -1.0;
		case WidgetAxisFit::ALIGN_START: return 0.0;
		case WidgetAxisFit::ALIGN_END: return 1.0;
		default: return 0.5;
		}
	}
	/** Returns the align as a from 0 to 1 or -1 if ALIGN_FILL.
	 * @param eAlign The alignment. Must be valid.
	 * @return The align as a number.
	 */
	static double getAlign(ALIGN eAlign) noexcept
	{
		switch (eAlign) {
		case WidgetAxisFit::ALIGN_FILL: return -1.0;
		case WidgetAxisFit::ALIGN_START: return 0.0;
		case WidgetAxisFit::ALIGN_END: return 1.0;
		default: return 0.5;
		}
	}
	#ifndef NDEBUG
	void dump(int32_t nIndentSpaces) const noexcept;
	void dump() const noexcept;
	#endif //NDEBUG
};

////////////////////////////////////////////////////////////////////////////////
/** The widget base class.
 * Widgets can be assigned to:
 * - a player: (m_nTeam &gt;= 0, m_nMate &gt;= 0)
 * - a team:   (m_nTeam &gt;= 0, m_nMate = -1)
 * - the game: (m_nTeam = -1, m_nMate = -1)
 *
 * A game widget can potentially have two configurations. The normal one and the
 * switched one. This allows the view (ThemeWidget) of the widget and its layout
 * to adapt to a portrait (height bigger than width) or landscape (width bigger
 * than height) window or screen size.
 */
class GameWidget
{
public:
	/** Instance initialization data. */
	struct Init
	{
		std::array<WidgetAxisFit, g_nTotLayoutConfigs> m_aHorizAxisFit = {{WidgetAxisFit(), WidgetAxisFit()}}; /**< Widget alignment for the x axis for each layout config. */
		std::array<WidgetAxisFit, g_nTotLayoutConfigs> m_aVertAxisFit = {{WidgetAxisFit{}, WidgetAxisFit{}}}; /**< Widget alignment for the y axis. */
		int32_t m_nTeam = -1; /**< The team the widget belongs to. Default: `-1`. */
		int32_t m_nMate = -1; /**< The mate the widget belongs to. Default: `-1`. */
		int32_t m_nViewWidgetNameIdx = -1; /**< The name that might be used to select the view (ThemeWidget). Index into Named::widgets(). Default: `-1`. */
		std::string m_sName; /**< The name of the widget. Can be empty. */
		//
		void setHorizExpand(bool bExpand) noexcept
		{
			for (int32_t nLC = 0; nLC < g_nTotLayoutConfigs; ++nLC) {
				m_aHorizAxisFit[nLC].m_bExpand = bExpand;
			}
		}
		void setVertExpand(bool bExpand) noexcept
		{
			for (int32_t nLC = 0; nLC < g_nTotLayoutConfigs; ++nLC) {
				m_aVertAxisFit[nLC].m_bExpand = bExpand;
			}
		}
		void setExpand(bool bExpand) noexcept
		{
			setHorizExpand(bExpand);
			setVertExpand(bExpand);
		}
		void setHorizAlign(WidgetAxisFit::ALIGN eAlign) noexcept
		{
			for (int32_t nLC = 0; nLC < g_nTotLayoutConfigs; ++nLC) {
				m_aHorizAxisFit[nLC].m_eAlign = eAlign;
			}
		}
		void setVertAlign(WidgetAxisFit::ALIGN eAlign) noexcept
		{
			for (int32_t nLC = 0; nLC < g_nTotLayoutConfigs; ++nLC) {
				m_aVertAxisFit[nLC].m_eAlign = eAlign;
			}
		}
		void setAlign(WidgetAxisFit::ALIGN eAlign) noexcept
		{
			setHorizAlign(eAlign);
			setVertAlign(eAlign);
		}
	};
	virtual ~GameWidget() noexcept = default;

	/** The type (name) that might be used to choose a ThemeWidget.
	 * It should be used by Theme to choose the ThemeWidget object (view)
	 * drawing this object (model)
	 * @return The index in Named.widgets(), if -1 not defined.
	 */
	inline int32_t getViewWidgetNameIdx() const noexcept { return m_oData.m_nViewWidgetNameIdx; }
	/** The name of the widget.
	 * @return The name. Can be empty.
	 */
	inline const std::string& getName() const noexcept { return m_oData.m_sName; }
	/** The team the widget is assigned to.
	 * @return The team or -1 if global widget.
	 */
	inline int32_t getTeam() const noexcept { return m_oData.m_nTeam; }
	/** The mate the widget is assigned to.
	 * @return The mate or -1 if global or team widget.
	 */
	inline int32_t getMate() const noexcept { return m_oData.m_nMate; }
	/** Widget alignment and expansion for the x axis.
	 * @param nLayoutConfig The layout config.
	 * @return The horizontal axis fit.
	 */
	WidgetAxisFit getWidgetHorizAxisFit(int32_t nLayoutConfig) const noexcept;
	/** Widget alignment and expansion for the y axis.
	 * @param nLayoutConfig The layout config.
	 * @return The vertical axis fit.
	 */
	WidgetAxisFit getWidgetVertAxisFit(int32_t nLayoutConfig) const noexcept;

	/** Tells whether the widget is assigned to a human.
	 * The owner can be determined by getTeam() and getMate(). If the owner type is a player
	 * tells whether it is human. If the owner type is team tells whether at least one
	 * mate in the team is human. If the owner type is game tells whether there
	 * is at least one human player playing.
	 * @return Whether assigned to human.
	 */
	inline bool isAssignedToHuman() const noexcept { return m_bIsOrContainsHumanPlayers; }
	/** The container to which this widget was added to.
	 * @return The parent or null.
	 */
	ContainerWidget* getContainer() const noexcept { return m_p1Owner; }
	/** Writes object contents to std::cout.
	 * Should only do it in debug mode. The function is also exposed in release mode
	 * because it is virtual.
	 *
	 * If bHeader is true this function should also output the type of the object and its address.
	 *
	 * @param nIndentSpaces Number of spaces put in front of each output line.
	 * @param bHeader Whether the header of the object should be displayed.
	 */
	virtual void dump(int32_t nIndentSpaces, bool bHeader) const noexcept;
	/** Writes object contents to std::cout (header included).
	 * @param nIndentSpaces Number of spaces put in front of each output line.
	 */
	void dump(int32_t nIndentSpaces) const noexcept;
	/** Writes object contents to std::cout (header included).
	 */
	void dump() const noexcept;
protected:
	/** Constructor.
	 * @param oData The initialization data.
	 */
	explicit GameWidget(Init&& oData) noexcept;
	/** Reinitialization.
	 * @param oData The initialization data.
	 */
	void reInit(Init&& oData) noexcept;
	/** The layout the widget belongs to.
	 * @return The layout. Undefined if the widget wasn't added to the layout yet.
	 */
	Layout& layout() noexcept;
	/** The layout the widget belongs to.
	 * @return The layout. Undefined if the widget wasn't added to the layout yet.
	 */
	const Layout& layout() const noexcept;
	/** The game the widget belongs to.
	 * @return The game. Undefined if the widget wasn't added to the game yet.
	 */
	GameProxy& game() noexcept;
	/** The game the widget belongs to.
	 * @return The game. Undefined if the widget wasn't added to the game yet.
	 */
	const GameProxy& game() const noexcept;
protected:
	/** Signals widget was added to layout.
	 */
	virtual void onAddedToLayout() noexcept {};
	/** Signals widget was added to game.
	 * This happens after onAddedToLayout().
	 */
	virtual void onAddedToGame() noexcept {};
private:
	void commonInit() noexcept;
	friend class Layout;
	// called by layout
	void setLayout(Layout* p0Layout) noexcept; // called by layout
	void setGameProxy(GameProxy* p0ProxyGame) noexcept; // called by layout
	void setTeam(int32_t nTeam) noexcept;
private:
	bool m_bIsOrContainsHumanPlayers;

	friend class ContainerWidget;
	ContainerWidget* m_p1Owner; // set by ContainerWidget
	GameProxy* m_p0GameProxy;
	Layout* m_p0Layout;

	Init m_oData;
private:
	GameWidget(const GameWidget& oSource) = delete;
	GameWidget& operator=(const GameWidget& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_GAME_WIDGET_H */

