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
 * File:   gamewindow.h
 */

#ifndef STMG_GAME_WINDOW_H
#define STMG_GAME_WINDOW_H

#include "mainwindow.h"

#include <stmm-games-file/file.h>

#include <stmm-games/util/basictypes.h>
#include <stmm-games/gameproxy.h>
#include <stmm-games/gameowner.h>

#include <stmm-input/devicemanager.h>

#include <gtkmm.h>
#include <sigc++/connection.h>

#include <string>
#include <memory>
#include <utility>
#include <vector>

#include <stdint.h>

namespace stmg { class AllPreferencesLoader; }
namespace stmg { class GameScreen; }
namespace stmg { class GameLoader; }
namespace stmg { class HighscoreScreen; }
namespace stmg { class HighscoresLoader; }
namespace stmg { class PlayersScreen; }
namespace stmg { class ThemeScreen; }
namespace stmg { class ThemeLoader; }
namespace stmg { class AllPreferences; }
namespace stmg { class Game; }
namespace stmg { class GameGtkDrawingArea; }
namespace stmg { class HighscoresDefinition; }
namespace stmg { class StdConfig; }
namespace stmg { class Theme; }
namespace stmg { class AboutScreen; }
namespace stmi { class Accessor; }
namespace stmi { class GtkAccessor; }

namespace stmg
{

using std::shared_ptr;

class StdView;

class GameWindow : public Gtk::Window, private GameOwner
{
public:
	/** Creates a game window.
	 * @param oMainWindowData The initialization data.
	 * @return The window and an empty string or if, an error occurred, null and and error string.
	 */
	static std::pair<Glib::RefPtr<GameWindow>, std::string> create(MainWindowData&& oMainWindowData) noexcept;

	~GameWindow() noexcept;

protected:
	/** Constructor.
	 * @param oMainWindowData The initialization data.
	 */
	explicit GameWindow(MainWindowData&& oMainWindowData) noexcept;

	/** Initialization.
	 * @return Error string or empty if successful.
	 */
	std::string init() noexcept;

private:
	Glib::RefPtr<Gdk::Pixbuf> loadFile(const File& oFile, const std::string& sName) noexcept;
	void onWindowRealize() noexcept;
	void setUp() noexcept;

	//Signal handlers:
	void onButtonNewGame() noexcept;
	void onButtonNewGameOut() noexcept;
	void onButtonHighscores() noexcept;
	void afterHighscores(const shared_ptr<Highscore>& refHighscore) noexcept;
	void onButtonChoosePlayers() noexcept;
	void afterChoosePlayers(const shared_ptr<AllPreferences>& refNewPrefs) noexcept;
	void onButtonChooseGame() noexcept;
	void afterChooseGame(const std::string& sGameName) noexcept;
	void onButtonChooseTheme() noexcept;
	void afterChooseTheme(const std::string& sThemeName) noexcept;
	void onButtonAbout() noexcept;
	void afterAbout() noexcept;
	void onButtonQuit() noexcept;
	void onButtonResume() noexcept;
		void onDrawingAreaSizeAllocate(Gtk::Allocation&) noexcept;
		void onButtonResumeOut() noexcept;
	void onButtonAbort() noexcept;

	void onButtonAnswerYes() noexcept;
	void onButtonAnswerNo() noexcept;
	void onButtonInfoOk() noexcept;

	void onActiveChanged() noexcept;
	bool onConfigureEvent(GdkEventConfigure* p0ConfigureEvent) noexcept;
	bool onWindowStateEvent(GdkEventWindowState* p0WindowStateEvent) noexcept;
	void onResizedOut() noexcept;

	bool onResizeTimeout() noexcept;

	bool on_key_press_event(GdkEventKey* p0GdkEvent) override;
	bool on_key_release_event(GdkEventKey* p0GdkEvent) override;
	bool on_button_press_event(GdkEventButton* p0GdkEvent) override;
	bool on_button_release_event(GdkEventButton* p0GdkEvent) override;
	bool on_motion_notify_event(GdkEventMotion* p0GdkEvent) override;

	bool onTouchEvent(GdkEventTouch* p0TouchEvent) noexcept;

	void callbackNextViewTick() noexcept;

	enum STATUS {
		STATUS_NONE = -1
		, STATUS_MENU = 0
		, STATUS_PLAY = 1
		, STATUS_WAIT_1SEC = 2
		, STATUS_WAIT_KEYPRESS = 3
		, STATUS_ARE_YOU_SURE = 4
	};
	void changeScreen(STATUS eStatus, int32_t nToScreen, const std::string& sMsg) noexcept;
	void onChangedPlayers() noexcept;
	void onChangedGame() noexcept;
	void onChangedTheme() noexcept;

	std::string getTheme() noexcept;
	std::string getCouldNotLoadThemeString(const std::string& sThemeName) noexcept;

	bool startGame() noexcept;
	void endGame() noexcept;
	void pauseGame() noexcept;
	void resumeGame() noexcept;

	void gameViewEnded();

	void gameEnded() noexcept override;
	void gameEndedOut() noexcept;
	void gameEndedShowHighscore() noexcept;
	void waitForKeypress() noexcept;
	void gamePauseOut() noexcept;
	void gameInterrupt(GameProxy::INTERRUPT_TYPE eInterruptType) noexcept override;
	void gameInterruptOut(GameProxy::INTERRUPT_TYPE eInterruptType, bool bAsk) noexcept;

	void activateResizeTimer() noexcept;
	void deactivateResizeTimer() noexcept;
	void startNextViewTickTimer(int32_t nMillisec) noexcept;

	int32_t getTotGames() const noexcept;
	int32_t getTotThemes() const noexcept;

private:
	MainWindowData m_oD;
	const shared_ptr<stmi::DeviceManager> m_refDM;

	shared_ptr<stmi::EventListener> m_refEventListener;
	shared_ptr<stmi::GtkAccessor> m_refAccessor;

	const Glib::ustring m_sApplicationName;
	Glib::RefPtr<Gdk::Pixbuf> m_refLogoPixbuf;

	// the game model
	shared_ptr<Game> m_refGame;
	shared_ptr<HighscoresDefinition> m_refHighscoresDefinition;
	// the game view
	shared_ptr<StdView> m_refGameView;
	// the theme
	shared_ptr<Theme> m_refTheme;

	shared_ptr<AllPreferences> m_refPrefs;

	bool m_bIsTestMode;

	bool m_bGameIsTicking; // If true implies m_refGame not null

	GameProxy::INTERRUPT_TYPE m_eAnsweringInterrupt;

	// STATUS_MENU           m_bGameIsTicking == false, s_nScreenMain | s_nScreenPaused, -
	// STATUS_PLAY           m_bGameIsTicking == true,  s_nScreenPlay                  , -
	// STATUS_WAIT_1SEC      m_bGameIsTicking == true,  s_nScreenPlay                  , resize
	// STATUS_WAIT_KEYPRESS  m_bGameIsTicking == true,  s_nScreenPlay                  , resize
	// STATUS_MENU           m_bGameIsTicking == false, s_nScreenAnswer                , resize?
	// STATUS_MENU           m_bGameIsTicking == false, s_nScreenInfo                  , -
	// STATUS_MENU           m_bGameIsTicking == false, s_nScreenAbout                 , -
	// STATUS_MENU           m_bGameIsTicking == false, s_nScreenHighscore             , -
	// STATUS_MENU           m_bGameIsTicking == false, s_nScreenChooseGame            , -
	// STATUS_MENU           m_bGameIsTicking == false, s_nScreenChooseTheme           , -
	// STATUS_MENU           m_bGameIsTicking == false, s_nScreenChoosePlayers         , -
	STATUS m_eStatus;
	int32_t m_nCurrentScreen;
	static constexpr const int32_t s_nScreenPlay = 0;
	static constexpr const int32_t s_nScreenMain = 1;
	static constexpr const int32_t s_nScreenPaused = 2;
	static constexpr const int32_t s_nScreenAnswer = 3;
	static constexpr const int32_t s_nScreenInfo = 4;
	static constexpr const int32_t s_nScreenAbout = 5;
	static constexpr const int32_t s_nScreenHighscore = 6;
	static constexpr const int32_t s_nScreenChooseGame = 7;
	static constexpr const int32_t s_nScreenChooseTheme = 8;
	static constexpr const int32_t s_nScreenChoosePlayers = 9;
	static constexpr const int32_t s_nTotScreens = 10;

	static constexpr const int32_t s_nGameEndedWaitMillisec = 1000;
	bool m_bNextViewTickActive;
	bool m_bStartGameAtNextViewTick;

	Glib::Timer m_oClock;

	// Example:
	// 0-----------------------------------------------1-------------------  game ticks
	// 0---------------1---------------2---------------0---------------1---  view ticks
	// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA                      game interval
	// BBBBBBBBBBBBBBBB                                                      view interval
	//
	// If a char represents a millisecond the game interval is 48 msec,
	// the view interval 16 msec (=> 62 "frames" per second).
	// The game logic is calculated at each game tick before view tick 0.
	// The game is redrawn at each view tick. While no game events happen at view ticks,
	// the animations are (where possible) drawn with interleaved positions to make
	// movements look smoother.
	// The number of view ticks per game interval is variable and is adjusted during
	// the game. If the drawing takes more than a view interval, the number is decreased.
	// If the drawing consistently takes less, the number is increased.
	// Also the game interval can change (it's the minimum of all the level intervals).
	// The initial level interval is 100 milliseconds.

	// Maximum number of view ticks in a game interval.
	int32_t m_nMaxViewTicks;
	// Number of game ticks the drawing takes less time than the view interval
	// (for m_nTotViewTicks + 1) for the number of view ticks per game interval to be increased.
	// Example:
	// m_fGameInterval = 60,  m_nTotViewTicks = 3
	// If the drawing takes less than 15 milliseconds ( = 60 / (3 + 1) ) for 10
	// game intervals in a row ( that is 30 view intervals ) the m_nNextTotViewTicks
	// is set to 4 so that in the next game interval there will be 4 view ticks.
	static constexpr const int32_t s_nGameTicksPotentialViewTicksIncDo = 10;

	struct GameTiming
	{
		double m_fGameInterval; // The interval between game ticks in milliseconds
		// The number of view ticks in the next game interval. If the drawing takes
		// more than a view tick in the current game tick, this value is set to
		// m_nTotViewTicks - 1 so that the frame rate will be decreased in the
		// next game interval.
		// Value: 1 .. s_nMaxTotViewTicks
		int32_t m_nNextTotViewTicks;
		// Counts the number of game ticks without lagging. This value is reset to 0
		// as soon as the drawing lags. Value: 0 .. s_nGameTicksPotentialViewTicksIncDo
		int32_t m_nGameTicksPotentialViewTicksIncCounter;
		int32_t m_nTotViewTicks; // The number of view ticks in current game interval. Value: 1 .. s_nMaxTotViewTicks
		int32_t m_nCurViewTick; // The current view tick in the game interval. Value: 0 .. s_nMaxTotViewTicks - 1
		void reset() noexcept
		{
			m_fGameInterval = -1.0;
			m_nNextTotViewTicks = 4;
			m_nGameTicksPotentialViewTicksIncCounter = 0;
			m_nTotViewTicks = m_nNextTotViewTicks;
			m_nCurViewTick = m_nTotViewTicks - 1;
		}
	};
	GameTiming m_oGT;

	sigc::connection m_oResizeConn;
	double m_fLastDelayedAllocation;
	bool m_bWaitingForDrawingAreaSizeAllocate;

	Gtk::Stack* m_p0StackScreens = nullptr;
		GameGtkDrawingArea* m_p0GameGtkDrawingArea = nullptr;
			int32_t m_nDrawingAreaBaseX;
			int32_t m_nDrawingAreaBaseY;
		Gtk::Box* m_p0ScreenBoxMain = nullptr;
			//Gtk::Box* m_p0BoxMain = nullptr;
				//Gtk::Image* m_p0ImageLogo = nullptr;
				//Gtk::Box* m_p0BoxCurrent = nullptr;
					Gtk::Label* m_p0LabelCurrentGame = nullptr;
					Gtk::Label* m_p0LabelCurrentPlayers = nullptr;
				Gtk::Button* m_p0ButtonMainNewGame = nullptr;
				Gtk::Button* m_p0ButtonMainHighscores = nullptr;
				Gtk::Button* m_p0ButtonMainChoosePlayers = nullptr;
				Gtk::Button* m_p0ButtonMainChooseGame = nullptr;
				Gtk::Button* m_p0ButtonMainChooseTheme = nullptr;
				//Gtk::Button* m_p0ButtonMainAbout = nullptr;
				//Gtk::Button* m_p0ButtonMainQuit = nullptr;
		Gtk::Box* m_p0ScreenBoxPaused = nullptr;
			//Gtk::Box* m_p0BoxPaused = nullptr;
				//Gtk::Button* m_p0ButtonPausedResume = nullptr;
				//Gtk::Button* m_p0ButtonPausedAbort = nullptr;
		Gtk::Box* m_p0ScreenBoxAnswer = nullptr;
			//Gtk::Box* m_p0BoxAnswer = nullptr;
				Gtk::Label* m_p0LabelAnswerQuestion = nullptr;
				//Gtk::Button* m_p0ButtonAnswerYes = nullptr;
				//Gtk::Button* m_p0ButtonAnswerNo = nullptr;
		Gtk::Box* m_p0ScreenBoxInfo = nullptr;
			//Gtk::Box* m_p0BoxInfo = nullptr;
				Gtk::Label* m_p0LabelInfoText = nullptr;
				//Gtk::Button* m_p0ButtonInfoOk = nullptr;
		Gtk::Widget* m_p0ScreenBoxAbout = nullptr;
			//Gtk::Box* m_p0BoxAbout = nullptr;

		Gtk::Widget* m_p0ScreenBoxHighscores = nullptr;
			//Gtk::Box* m_p0BoxHighscores = nullptr;

		Gtk::Widget* m_p0ScreenBoxChooseGame = nullptr;
			//Gtk::Box* m_p0BoxChooseGame = nullptr;

		Gtk::Widget* m_p0ScreenBoxChooseTheme = nullptr;
			//Gtk::Box* m_p0BoxChooseTheme = nullptr;

		Gtk::Widget* m_p0ScreenBoxChoosePlayers = nullptr;
			//Gtk::Box* m_p0BoxChooseTheme = nullptr;

	Gtk::Widget* m_aScreens[s_nTotScreens]; // non owning pointers

	friend class HighscoreScreen;
	unique_ptr<HighscoreScreen> m_refHighscoreScreen;
	friend class GameScreen;
	unique_ptr<GameScreen> m_refChooseGameScreen;
	friend class ThemeScreen;
	unique_ptr<ThemeScreen> m_refChooseThemeScreen;
	friend class PlayersScreen;
	unique_ptr<PlayersScreen> m_refChoosePlayersScreen;
	friend class AboutScreen;
	unique_ptr<AboutScreen> m_refAboutScreen;
private:
	GameWindow(const GameWindow& oSource) = delete;
	GameWindow& operator=(const GameWindow& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_GAME_WINDOW_H */

