/*
 * File:   gamewindow.cc
 *
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

#include "gamewindow.h"

#include "mainwindow.h"
#include "stdview.h"

#include "allpreferences.h"
#include "game.h"
#include "gamegtkdrawingarea.h"
#include "gameproxy.h"
#include "highscore.h"
#include "level.h"
#include "stdconfig.h"
#include "theme.h"
#include "allpreferencesloader.h"
#include "gameloader.h"
#include "highscoresloader.h"
#include "themeloader.h"

#include "dialogs/playersdialog.h"
#include "dialogs/gamedialog.h"
#include "dialogs/highscoredialog.h"
#include "dialogs/themedialog.h"

#include "gtkutil/gtkutilpriv.h"

#include <stmm-games/util/util.h>
#include <stmm-games/stdpreferences.h>

#include <stmm-input/event.h>
#include <stmm-input/xyevent.h>
#include <stmm-input/devicemanager.h>

#include <stmm-input-gtk/gtkaccessor.h>

#include <gtkmm.h>

#include <iostream>
#include <cassert>
#include <memory>
#include <string>
#include <algorithm>
//#include <iterator>
#include <type_traits>


namespace stmg
{

static const Glib::ustring s_sScreenNamePlay = "Play";
static const Glib::ustring s_sScreenNameMain = "Main";
static const Glib::ustring s_sScreenNamePaused = "Paused";
static const Glib::ustring s_sScreenNameAnswer = "Answer";
static const Glib::ustring s_sScreenNameInfo = "Info";
static const Glib::ustring s_sScreenNameAbout = "About";

static const Glib::ustring s_sStmmGamesWebSite = "https://www.efanomars.com/libraries/stmm-games";
static const Glib::ustring s_sStmmGamesCopyright = "stmm-games © 2019-2020 Stefano Marsili";

static constexpr int32_t s_nMinDefaultPixW = 100;
static constexpr int32_t s_nMinDefaultPixH = 150;

static constexpr int32_t s_nNewGameTransitionMillisec = 100;

static constexpr const int32_t s_nButtonLeftRightMargin = 20;

std::pair<Glib::RefPtr<GameWindow>, std::string> GameWindow::create(MainWindowData&& oMainWindowData) noexcept
{
	auto refStdConfig = oMainWindowData.m_refStdConfig; // make copy
	assert(refStdConfig);
	auto refGameWin = Glib::RefPtr<GameWindow>(new GameWindow(std::move(oMainWindowData)));
	const std::string sError = refGameWin->init();
	if (!sError.empty()) {
		return std::make_pair(Glib::RefPtr<GameWindow>{}, sError); //-----------
	}
	refGameWin->m_refAccessor = std::make_shared<stmi::GtkAccessor>(refGameWin);
	refStdConfig->getDeviceManager()->addAccessor(refGameWin->m_refAccessor);
	return std::make_pair(std::move(refGameWin), "");
}

GameWindow::GameWindow(MainWindowData&& oMainWindowData) noexcept
: m_refStdConfig(oMainWindowData.m_refStdConfig)
, m_refDM(m_refStdConfig->getDeviceManager())
, m_refGameLoader(std::move(oMainWindowData.m_refGameLoader))
, m_refHighscoresLoader(std::move(oMainWindowData.m_refHighscoresLoader))
, m_refThemeLoader(std::move(oMainWindowData.m_refThemeLoader))
, m_refAllPreferencesLoader(std::move(oMainWindowData.m_refAllPreferencesLoader))
, m_oInitialSize(oMainWindowData.m_oInitialSize)
, m_sApplicationName(m_refStdConfig->getAppName())
, m_sCopyright(oMainWindowData.m_sCopyright)
, m_aAuthors(std::move(oMainWindowData.m_aAuthors))
, m_sWebSite(std::move(oMainWindowData.m_sWebSite))
, m_oIconFile(std::move(oMainWindowData.m_oIconFile))
, m_oLogoFile(std::move(oMainWindowData.m_oLogoFile))
, m_refPrefs((m_refAllPreferencesLoader.get() != nullptr)
			? m_refAllPreferencesLoader->getPreferences()
			: std::make_shared<AllPreferences>(m_refStdConfig))
, m_bPauseIfWindowDeactivated(oMainWindowData.m_bPauseIfWindowDeactivated)
, m_bFullscreen(oMainWindowData.m_bFullscreen)
, m_bGameIsTicking(false)
, m_eStatus(STATUS_NONE)
, m_nCurrentScreen(-1)
, m_bNextViewTickActive(false)
, m_bStartGameAtNextViewTick(false)
, m_nMaxViewTicks(8)
, m_fLastDelayedAllocation(-111)
, m_bWaitingForDrawingAreaSizeAllocate(false)
, m_nDrawingAreaBaseX(0)
, m_nDrawingAreaBaseY(0)
{
	assert(m_refPrefs->getStdConfig() == m_refStdConfig);
	if (m_bPauseIfWindowDeactivated) {
		property_is_active().signal_changed().connect(sigc::mem_fun(this, &GameWindow::onActiveChanged));
	}
	const std::string sGameName = m_refPrefs->getGameName();
	if (!sGameName.empty()) {
		const auto& aValidGameNames = m_refGameLoader->getGameNames();
		const auto itFind = std::find(aValidGameNames.begin(), aValidGameNames.end(), sGameName);
		if (itFind == aValidGameNames.end()) {
			m_refPrefs->setGameName("");
		}
	}
}
GameWindow::~GameWindow() noexcept
{
	m_p0GameGtkDrawingArea->buggyDestructor();
}

Glib::RefPtr<Gdk::Pixbuf> GameWindow::loadFile(const File& oFile, const std::string& sName) noexcept
{
	Glib::RefPtr<Gdk::Pixbuf> refFilePixbuf;
	if (oFile.isDefined()) {
		try {
			if (oFile.isBuffered()) {
				Glib::RefPtr<Gio::MemoryInputStream> refInput = Gio::MemoryInputStream::create();
				refInput->add_data(oFile.getBuffer(), oFile.getBufferSize());
				refFilePixbuf = Gdk::Pixbuf::create_from_stream(refInput);
			} else {
				refFilePixbuf = Gdk::Pixbuf::create_from_file(oFile.getFullPath());
			}
		} catch (...) {
			// no thumbnail
			std::cout << "Couldn't load " << sName << " file " << (oFile.isBuffered() ? "" : oFile.getFullPath()) << '\n';
		}
	}
	return refFilePixbuf;
}
std::string GameWindow::init() noexcept
{
	set_title(m_sApplicationName);
	set_default_size(std::max(m_oInitialSize.m_nW, s_nMinDefaultPixW), std::max(m_oInitialSize.m_nH, s_nMinDefaultPixH));
	set_resizable(true);

	Glib::RefPtr<Gdk::Pixbuf> refIconPixbuf = loadFile(m_oIconFile, "icon");
	if (refIconPixbuf) {
		set_icon(refIconPixbuf);
	}
	m_refLogoPixbuf = loadFile(m_oLogoFile, "logo");
	if (!m_refLogoPixbuf) {
		m_refLogoPixbuf = refIconPixbuf;
	}

	const int32_t nMaxPlayers = m_refStdConfig->getAppConstraints().getMaxPlayers();

	m_refGameView = std::make_shared<StdView>();

	Gtk::Box* m_p0VBoxMain = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	Gtk::Window::add(*m_p0VBoxMain);
	// add(m_oMainBox);
	// m_oMainBox.set_border_width(0);

	m_p0StackScreens = Gtk::manage(new Gtk::Stack());
	m_p0VBoxMain->pack_start(*m_p0StackScreens, true, true);
		m_p0StackScreens->set_transition_type(Gtk::StackTransitionType::STACK_TRANSITION_TYPE_NONE);

	m_p0GameGtkDrawingArea = Gtk::manage(new GameGtkDrawingArea(m_refGameView, m_oClock));
	m_p0StackScreens->add(*m_p0GameGtkDrawingArea, s_sScreenNamePlay);
		m_p0GameGtkDrawingArea->signal_size_allocate().connect(sigc::mem_fun(*this, &GameWindow::onDrawingAreaSizeAllocate));
	m_aScreens[s_nScreenPlay] = m_p0GameGtkDrawingArea;

	m_p0ScreenBoxMain = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_p0StackScreens->add(*m_p0ScreenBoxMain, s_sScreenNameMain);
		Gtk::Box* m_p0BoxMain = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0ScreenBoxMain->pack_start(*m_p0BoxMain, true, true);
			m_p0BoxMain->set_valign(Gtk::Align::ALIGN_CENTER);
			m_p0BoxMain->set_border_width(s_nButtonLeftRightMargin);
			m_p0BoxMain->set_spacing(15);
			if (m_refLogoPixbuf) {
				Gtk::Image* m_p0ImageLogo = Gtk::manage(new Gtk::Image(m_refLogoPixbuf));
				m_p0BoxMain->pack_start(*m_p0ImageLogo);
			}
			//
			Gtk::Box* m_p0BoxCurrent = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
			m_p0BoxMain->pack_start(*m_p0BoxCurrent, true, true);
				m_p0BoxCurrent->set_border_width(5);
				m_p0BoxCurrent->set_spacing(5);
				m_p0LabelCurrentGame = Gtk::manage(new Gtk::Label("Current game: -"));
				m_p0BoxCurrent->pack_start(*m_p0LabelCurrentGame);
				m_p0LabelCurrentPlayers = Gtk::manage(new Gtk::Label("Player: -"));
				m_p0BoxCurrent->pack_start(*m_p0LabelCurrentPlayers);
			//
			m_p0ButtonMainNewGame = Gtk::manage(new Gtk::Button("Play"));
			m_p0BoxMain->pack_start(*m_p0ButtonMainNewGame);
				m_p0ButtonMainNewGame->signal_clicked().connect(
								sigc::mem_fun(*this, &GameWindow::onButtonNewGame) );
			m_p0ButtonMainHighscores = Gtk::manage(new Gtk::Button("Highscores"));
			m_p0BoxMain->pack_start(*m_p0ButtonMainHighscores);
				m_p0ButtonMainHighscores->signal_clicked().connect(
								sigc::mem_fun(*this, &GameWindow::onButtonHighscores) );
			m_p0ButtonMainChoosePlayers = Gtk::manage(new Gtk::Button((nMaxPlayers > 1) ? "Choose Players" : "Options"));
			m_p0BoxMain->pack_start(*m_p0ButtonMainChoosePlayers);
				m_p0ButtonMainChoosePlayers->signal_clicked().connect(
								sigc::mem_fun(*this, &GameWindow::onButtonChoosePlayers) );
			m_p0ButtonMainChooseGame = Gtk::manage(new Gtk::Button("Choose Game"));
			m_p0BoxMain->pack_start(*m_p0ButtonMainChooseGame);
				m_p0ButtonMainChooseGame->signal_clicked().connect(
								sigc::mem_fun(*this, &GameWindow::onButtonChooseGame) );
			m_p0ButtonMainChooseTheme = Gtk::manage(new Gtk::Button("Choose Theme"));
			m_p0BoxMain->pack_start(*m_p0ButtonMainChooseTheme);
				m_p0ButtonMainChooseTheme->signal_clicked().connect(
								sigc::mem_fun(*this, &GameWindow::onButtonChooseTheme) );
			Gtk::Button* m_p0ButtonMainAbout = Gtk::manage(new Gtk::Button("About"));
			m_p0BoxMain->pack_start(*m_p0ButtonMainAbout);
				m_p0ButtonMainAbout->signal_clicked().connect(
								sigc::mem_fun(*this, &GameWindow::onButtonAbout) );
			Gtk::Button* m_p0ButtonMainQuit = Gtk::manage(new Gtk::Button("Quit"));
			m_p0BoxMain->pack_start(*m_p0ButtonMainQuit);
				m_p0ButtonMainQuit->signal_clicked().connect(
								sigc::mem_fun(*this, &GameWindow::onButtonQuit) );
	m_aScreens[s_nScreenMain] = m_p0ScreenBoxMain;

	m_p0ScreenBoxPaused = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_p0StackScreens->add(*m_p0ScreenBoxPaused, s_sScreenNamePaused);
		Gtk::Box* m_p0BoxPaused = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0ScreenBoxPaused->pack_start(*m_p0BoxPaused, true, true);
			m_p0BoxPaused->set_valign(Gtk::Align::ALIGN_CENTER);
			m_p0BoxPaused->set_border_width(s_nButtonLeftRightMargin);
			m_p0BoxPaused->set_spacing(20);
			Gtk::Button* m_p0ButtonPausedResume = Gtk::manage(new Gtk::Button("Resume"));
			m_p0BoxPaused->pack_start(*m_p0ButtonPausedResume);
				m_p0ButtonPausedResume->signal_clicked().connect(
								sigc::mem_fun(*this, &GameWindow::onButtonResume) );
			Gtk::Button* m_p0ButtonPausedAbortGame = Gtk::manage(new Gtk::Button("Abort"));
			m_p0BoxPaused->pack_start(*m_p0ButtonPausedAbortGame);
				m_p0ButtonPausedAbortGame->signal_clicked().connect(
								sigc::mem_fun(*this, &GameWindow::onButtonAbort) );
	m_aScreens[s_nScreenPaused] = m_p0ScreenBoxPaused;

	m_p0ScreenBoxAnswer = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_p0StackScreens->add(*m_p0ScreenBoxAnswer, s_sScreenNameAnswer);
		Gtk::Box* m_p0BoxAnswer = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0ScreenBoxAnswer->pack_start(*m_p0BoxAnswer, true, true);
			addBigSeparator(m_p0BoxAnswer);
			m_p0LabelAnswerQuestion = Gtk::manage(new Gtk::Label("Question?"));
			m_p0BoxAnswer->pack_start(*m_p0LabelAnswerQuestion);
				m_p0LabelAnswerQuestion->set_line_wrap(true);
				m_p0LabelAnswerQuestion->set_margin_top(5);
				m_p0LabelAnswerQuestion->set_margin_bottom(5);
			Gtk::Button* m_p0ButtonAnswerYes = Gtk::manage(new Gtk::Button("Yes"));
			m_p0BoxAnswer->pack_start(*m_p0ButtonAnswerYes, false, false);
				m_p0ButtonAnswerYes->set_margin_left(s_nButtonLeftRightMargin);
				m_p0ButtonAnswerYes->set_margin_right(s_nButtonLeftRightMargin);
				m_p0ButtonAnswerYes->set_margin_top(5);
				m_p0ButtonAnswerYes->set_margin_bottom(5);
				m_p0ButtonAnswerYes->signal_clicked().connect(
								sigc::mem_fun(*this, &GameWindow::onButtonAnswerYes) );
			Gtk::Button* m_p0ButtonAnswerNo = Gtk::manage(new Gtk::Button("No"));
			m_p0BoxAnswer->pack_start(*m_p0ButtonAnswerNo, false, false);
				m_p0ButtonAnswerNo->set_margin_left(s_nButtonLeftRightMargin);
				m_p0ButtonAnswerNo->set_margin_right(s_nButtonLeftRightMargin);
				m_p0ButtonAnswerNo->set_margin_top(5);
				m_p0ButtonAnswerNo->set_margin_bottom(5);
				m_p0ButtonAnswerNo->signal_clicked().connect(
								sigc::mem_fun(*this, &GameWindow::onButtonAnswerNo) );
			addBigSeparator(m_p0BoxAnswer);
	m_aScreens[s_nScreenAnswer] = m_p0ScreenBoxAnswer;

	m_p0ScreenBoxInfo = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_p0StackScreens->add(*m_p0ScreenBoxInfo, s_sScreenNameInfo);
		Gtk::Box* m_p0BoxInfo = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0ScreenBoxInfo->pack_start(*m_p0BoxInfo, true, true);
			addBigSeparator(m_p0BoxInfo);
			m_p0LabelInfoText = Gtk::manage(new Gtk::Label("Info"));
			m_p0BoxInfo->pack_start(*m_p0LabelInfoText);
				m_p0LabelInfoText->set_line_wrap(true);
				m_p0LabelInfoText->set_margin_top(5);
				m_p0LabelInfoText->set_margin_bottom(5);
			Gtk::Button* m_p0ButtonInfoOk = Gtk::manage(new Gtk::Button("Ok"));
			m_p0BoxInfo->pack_start(*m_p0ButtonInfoOk, false, false);
				m_p0ButtonInfoOk->set_margin_left(s_nButtonLeftRightMargin);
				m_p0ButtonInfoOk->set_margin_right(s_nButtonLeftRightMargin);
				m_p0ButtonInfoOk->set_margin_top(5);
				m_p0ButtonInfoOk->set_margin_bottom(5);
				m_p0ButtonInfoOk->signal_clicked().connect(
								sigc::mem_fun(*this, &GameWindow::onButtonInfoOk) );
			addBigSeparator(m_p0BoxInfo);
	m_aScreens[s_nScreenInfo] = m_p0ScreenBoxInfo;

	m_p0ScreenBoxAbout = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_p0StackScreens->add(*m_p0ScreenBoxAbout, s_sScreenNameAbout);
		Gtk::Box* m_p0BoxAbout = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0ScreenBoxAbout->pack_start(*m_p0BoxAbout, true, true);
			addBigSeparator(m_p0BoxAbout);
			Gtk::Image* m_p0ImageAboutLogo = Gtk::manage(new Gtk::Image(m_refLogoPixbuf));
			m_p0BoxAbout->pack_start(*m_p0ImageAboutLogo, false, true);
				m_p0ImageAboutLogo->set_margin_top(5);
				m_p0ImageAboutLogo->set_margin_bottom(5);
			Gtk::Label* m_p0LabelAboutAppName = Gtk::manage(new Gtk::Label(m_sApplicationName));
			m_p0BoxAbout->pack_start(*m_p0LabelAboutAppName, false, true);
				m_p0LabelAboutAppName->set_margin_top(5);
				m_p0LabelAboutAppName->set_margin_bottom(5);
				{
				Pango::AttrList oAttrList;
				Pango::AttrInt oAttrWeight = Pango::Attribute::create_attr_weight(Pango::WEIGHT_HEAVY);
				oAttrList.insert(oAttrWeight);
				Pango::AttrFloat oAttrScale = Pango::Attribute::create_attr_scale(1.8);
				oAttrList.insert(oAttrScale);
				m_p0LabelAboutAppName ->set_attributes(oAttrList);
				}
			Gtk::Label* m_p0LabelAboutVersion = Gtk::manage(new Gtk::Label(m_refStdConfig->getAppVersion()));
			m_p0BoxAbout->pack_start(*m_p0LabelAboutVersion, false, true);
				m_p0LabelAboutVersion->set_margin_top(5);
				m_p0LabelAboutVersion->set_margin_bottom(5);
			Gtk::Label* m_p0LabelAboutCopyright = Gtk::manage(new Gtk::Label(m_sCopyright.empty() ? s_sStmmGamesCopyright : m_sCopyright));
			m_p0BoxAbout->pack_start(*m_p0LabelAboutCopyright, false, true);
				m_p0LabelAboutCopyright->set_margin_top(3);
				m_p0LabelAboutCopyright->set_margin_bottom(3);
			if (! m_sWebSite.empty()) {
				addBigSeparator(m_p0BoxAbout);
				Gtk::Label* m_p0LabelAboutWebsite = Gtk::manage(new Gtk::Label("<i>Website</i>"));
				m_p0BoxAbout->pack_start(*m_p0LabelAboutWebsite, false, true);
					m_p0LabelAboutWebsite->set_use_markup(true);
				Gtk::Label* m_p0LabelAboutWebsiteAdr = Gtk::manage(new Gtk::Label("<a href=\"" + m_sWebSite + "\">" + m_sWebSite + "</a>"));
				m_p0BoxAbout->pack_start(*m_p0LabelAboutWebsiteAdr, false, true);
					m_p0LabelAboutWebsiteAdr->set_use_markup(true);
					m_p0LabelAboutWebsiteAdr->set_ellipsize(Pango::ELLIPSIZE_END);
					m_p0LabelAboutWebsiteAdr->set_margin_bottom(5);
			}
			if (! m_aAuthors.empty()) {
			addBigSeparator(m_p0BoxAbout);
			Gtk::ScrolledWindow* m_p0ScrolledAuthors = Gtk::manage(new Gtk::ScrolledWindow());
			m_p0BoxAbout->pack_start(*m_p0ScrolledAuthors, true, true);
				m_p0ScrolledAuthors->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
				Gtk::Box* m_p0VBoxScroller = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
				m_p0ScrolledAuthors->add(*m_p0VBoxScroller);
					Gtk::Label* m_p0LabelAboutAuthors = Gtk::manage(new Gtk::Label(Glib::ustring{"<big><b>Author"} + ((m_aAuthors.size() == 1) ? "" : "s") + "</b></big>"));
					m_p0VBoxScroller->pack_start(*m_p0LabelAboutAuthors, false, true);
						m_p0LabelAboutAuthors->set_use_markup(true);
						m_p0LabelAboutAuthors->set_margin_top(3);
						m_p0LabelAboutAuthors->set_margin_bottom(3);
						//use text in scroller!
					for (const MainAuthorData& oAuthor : m_aAuthors) {
						addSmallSeparator(m_p0VBoxScroller);
						assert(! oAuthor.m_sName.empty());
						Gtk::Label* m_p0LabelAboutAuthorName = Gtk::manage(new Gtk::Label("<b>" + oAuthor.m_sName + "</b>"));
						m_p0VBoxScroller->pack_start(*m_p0LabelAboutAuthorName, false, true);
							m_p0LabelAboutAuthorName->set_use_markup(true);
						if (! oAuthor.m_sEMail.empty()) {
						Gtk::Label* m_p0LabelAboutAuthorEMail = Gtk::manage(new Gtk::Label("<a href=\"mailto:" + oAuthor.m_sEMail + "\">" + oAuthor.m_sEMail + "</a>"));
						m_p0VBoxScroller->pack_start(*m_p0LabelAboutAuthorEMail, false, true);
							m_p0LabelAboutAuthorEMail->set_use_markup(true);
							m_p0LabelAboutAuthorEMail->set_ellipsize(Pango::ELLIPSIZE_END);
						}
						if (! oAuthor.m_sRole.empty()) {
						Gtk::Label* m_p0LabelAboutAuthorRole = Gtk::manage(new Gtk::Label("<i>" + oAuthor.m_sRole + "</i>"));
						m_p0VBoxScroller->pack_start(*m_p0LabelAboutAuthorRole, false, true);
							m_p0LabelAboutAuthorRole->set_use_markup(true);
							m_p0LabelAboutAuthorRole->set_ellipsize(Pango::ELLIPSIZE_END);
						}
					}
			}

			addBigSeparator(m_p0BoxAbout);
			Gtk::Label* m_p0LabelAboutStmmGamesWebsite = Gtk::manage(new Gtk::Label("<i>Based on the stmm-games library</i>"));
			m_p0BoxAbout->pack_start(*m_p0LabelAboutStmmGamesWebsite, false, true);
				m_p0LabelAboutStmmGamesWebsite->set_use_markup(true);
			Gtk::Label* m_p0LabelAboutStmmGamesWebsiteAdr = Gtk::manage(new Gtk::Label("<a href=\"" + s_sStmmGamesWebSite + "\">" + s_sStmmGamesWebSite + "</a>"));
			m_p0BoxAbout->pack_start(*m_p0LabelAboutStmmGamesWebsiteAdr, false, true);
				m_p0LabelAboutStmmGamesWebsiteAdr->set_use_markup(true);
				m_p0LabelAboutStmmGamesWebsiteAdr->set_ellipsize(Pango::ELLIPSIZE_END);
				m_p0LabelAboutStmmGamesWebsiteAdr->set_margin_bottom(5);

			addBigSeparator(m_p0BoxAbout);
			Gtk::Button* m_p0ButtonAboutOk = Gtk::manage(new Gtk::Button("Ok"));
			m_p0BoxAbout->pack_start(*m_p0ButtonAboutOk, false, false);
				m_p0ButtonAboutOk->set_margin_left(s_nButtonLeftRightMargin);
				m_p0ButtonAboutOk->set_margin_right(s_nButtonLeftRightMargin);
				m_p0ButtonAboutOk->signal_clicked().connect(
								sigc::mem_fun(*this, &GameWindow::onButtonAboutOk) );
			addBigSeparator(m_p0BoxAbout);
	m_aScreens[s_nScreenAbout] = m_p0ScreenBoxAbout;

	auto* p0VisibleWidget = m_p0StackScreens->get_visible_child();
	constexpr int32_t nTotScreens = static_cast<int32_t>(sizeof(m_aScreens) / sizeof(m_aScreens[0]));
	for (int32_t nIdx = 0; nIdx < nTotScreens; ++nIdx) {
		auto*& p0Widget = m_aScreens[nIdx];
		if (p0VisibleWidget == p0Widget) {
			m_nCurrentScreen = nIdx;
		}
	}
	if (m_bFullscreen) {
		fullscreen();
	}

	show_all_children();

	signal_touch_event().connect(sigc::mem_fun(*this, &GameWindow::on_sig_touch_event));
	signal_realize().connect(sigc::mem_fun(*this, &GameWindow::onWindowRealize));
	return "";
}
void GameWindow::onWindowRealize() noexcept
{
	// don't do the stuff in the callback itself
	Glib::signal_timeout().connect_once(sigc::mem_fun(*this, &GameWindow::setUp), 0);
}
void GameWindow::setUp() noexcept //TODO tearDown() ????
{
	changeScreen(STATUS_MENU, s_nScreenMain, "");

	onChangedGame();
	onChangedPlayers();
	onChangedTheme();

	m_p0ButtonMainNewGame->grab_focus();

	m_oClock.start();
}
int32_t GameWindow::getTotGames() const noexcept
{
	const bool bIsTestMode = m_refStdConfig->isTestMode();
	const auto& aValidGameNames = m_refGameLoader->getGameNames();
	int32_t nTotGames = 0;
	for (const auto& sGameName : aValidGameNames) {
		const GameLoader::GameInfo& oGameInfo = m_refGameLoader->getGameInfo(sGameName);
		if (oGameInfo.m_bTesting && ! bIsTestMode) {
			continue;
		}
		++nTotGames;
	}
	return nTotGames;
}
int32_t GameWindow::getTotThemes() const noexcept
{
	const bool bIsTestMode = m_refStdConfig->isTestMode();
	const auto& aValidThemeNames = m_refThemeLoader->getThemeNames();
	int32_t nTotThemes = 0;
	for (const auto& sThemeName : aValidThemeNames) {
		const ThemeLoader::ThemeInfo& oThemeInfo = m_refThemeLoader->getThemeInfo(sThemeName);
		if (oThemeInfo.m_bTesting && ! bIsTestMode) {
			continue;
		}
		++nTotThemes;
	}
	return nTotThemes;
}
void GameWindow::changeScreen(STATUS eStatus, int32_t nToScreen, const std::string& sMsg) noexcept
{
	m_eStatus = eStatus;
	if (m_nCurrentScreen == nToScreen) {
		return;
	}
	const bool bIsTestMode = m_refStdConfig->isTestMode();

	m_nCurrentScreen = nToScreen;
	m_p0StackScreens->set_visible_child(*m_aScreens[m_nCurrentScreen]);
	if (m_nCurrentScreen == s_nScreenPlay) {
		//
	} else if (m_nCurrentScreen == s_nScreenMain) {
		const bool bHasHighscores = m_refHighscoresLoader.operator bool();
		const bool bHasMoreThanOneGame = (getTotGames() > 1);
		const bool bHasMoreThanOneTheme = (getTotThemes() > 1);
		m_p0LabelCurrentGame->set_visible(bHasMoreThanOneGame || bIsTestMode);
		m_p0ButtonMainHighscores->set_sensitive(bHasHighscores || bIsTestMode);
		m_p0ButtonMainChooseGame->set_visible(bHasMoreThanOneGame || bIsTestMode);
		m_p0ButtonMainChooseTheme->set_visible(bHasMoreThanOneTheme || bIsTestMode);
	} else if (m_nCurrentScreen == s_nScreenPaused) {
	} else if (m_nCurrentScreen == s_nScreenAnswer) {
		m_p0LabelAnswerQuestion->set_text(sMsg);
	} else if (m_nCurrentScreen == s_nScreenInfo) {
		m_p0LabelInfoText->set_text(sMsg);
	} else if (m_nCurrentScreen == s_nScreenAbout) {
	} else {
		assert(false);
	}
}
void GameWindow::onActiveChanged() noexcept
{
	const bool bIsActive = get_realized() && get_visible() && is_active();
	if (!bIsActive) {
		if (m_bGameIsTicking && (m_nCurrentScreen == s_nScreenPlay)) {
			assert(m_refGame);
			if (m_refGame->isRunning()) {
				gameInterrupt(GameProxy::INTERRUPT_PAUSE);
			}
		}
	}
}

void GameWindow::activateResizeTimer() noexcept
{
	assert(!m_oResizeConn.connected());
	m_oResizeConn = Glib::signal_timeout().connect(
			sigc::mem_fun(*this, &GameWindow::onResizeTimeout), s_nResizeConnMillisec);
}
void GameWindow::deactivateResizeTimer() noexcept
{
	assert(m_oResizeConn.connected());
	m_oResizeConn.disconnect();
}
void GameWindow::startNextViewTickTimer(int32_t nMillisec) noexcept
{
	if (m_bNextViewTickActive) {
		return;
	}
//std::cout << "GameWindow::startNextViewTickTimer  nMillisec=" << nMillisec << '\n';
	m_bNextViewTickActive = true;
	Glib::signal_timeout().connect_once(
		sigc::mem_fun(*this, &GameWindow::callbackNextViewTick), nMillisec);
}

void GameWindow::gameEnded() noexcept
{
	endGame();

	activateResizeTimer();
	changeScreen(STATUS_WAIT_1SEC, s_nScreenPlay, "");
	int32_t nTotWaitMillisec = s_nGameEndedWaitMillisec;
	if (m_refHighscoresDefinition) {
		assert(m_refGame);
		nTotWaitMillisec += m_refGame->getAdditionalHighscoresWait();
	}
	Glib::signal_timeout().connect_once(
		sigc::mem_fun(*this, &GameWindow::gameEndedOut), nTotWaitMillisec);
}
void GameWindow::gameEndedOut() noexcept
{
	changeScreen(STATUS_HIGHSCORE, s_nScreenPlay, "");

	int32_t nWaitAgain = 0;
	if (m_refHighscoresDefinition) {
		if (m_refHighscoresLoader) {
			shared_ptr<Highscore> refHighscore = m_refHighscoresLoader->getHighscore(m_refGame->getName(), *m_refPrefs, m_refHighscoresDefinition);
			if (refHighscore) {
//std::cout << "GameWindow::gameEnded() before refHighscore->getTotScores()=" << refHighscore->getTotScores() << '\n';
				if (!m_refHighscoreDialog) {
					m_refHighscoreDialog = Glib::RefPtr<HighscoreDialog>(new HighscoreDialog());
					m_refHighscoreDialog->set_default_size(get_width() / 1.0, get_height() / 1.0);
					m_refHighscoreDialog->set_transient_for(*this);
				}
				auto& oHighscoreDialog = *(m_refHighscoreDialog.operator->());
				const auto nRet = oHighscoreDialog.run(refHighscore, m_refGame, m_refPrefs);
				if (Gtk::RESPONSE_CANCEL == nRet) {
					nWaitAgain = s_nGameEndedWaitMillisec;
				}
				//
				oHighscoreDialog.hide();
				//
				const bool bOk = m_refHighscoresLoader->updateHighscore(m_refGame->getName(), *m_refPrefs, *refHighscore);
				if (!bOk) {
					//msgWarningBox("Could not save highscores!");
					std::cout << "Could not save highscores!" << '\n';
				}
			}
		}
	}
	m_refHighscoresDefinition.reset();
	if (nWaitAgain > 0) {
		Glib::signal_timeout().connect_once(
			sigc::mem_fun(*this, &GameWindow::waitForKeypress), nWaitAgain);
	} else {
		waitForKeypress();
	}
}
void GameWindow::waitForKeypress() noexcept
{
	changeScreen(STATUS_WAIT_KEYPRESS, s_nScreenPlay, "");
}
void GameWindow::gamePauseOut() noexcept
{
	changeScreen(STATUS_MENU, s_nScreenPaused, "");
}
void GameWindow::gameInterrupt(GameProxy::INTERRUPT_TYPE eInterruptType) noexcept
{
	if (m_eStatus != STATUS_PLAY) {
		return; //--------------------------------------------------------------
	}
	if (eInterruptType == GameProxy::INTERRUPT_PAUSE) {
		pauseGame();
		Glib::signal_timeout().connect_once(
							sigc::mem_fun(*this, &GameWindow::gamePauseOut), 0);
		return; //--------------------------------------------------------------
	}
	const bool bAsk = (eInterruptType == GameProxy::INTERRUPT_ABORT_ASK) 
					|| (eInterruptType == GameProxy::INTERRUPT_RESTART_ASK)
					|| (eInterruptType == GameProxy::INTERRUPT_QUIT_APP_ASK);
	if (bAsk) {
		pauseGame();
		//changeScreen(STATUS_ARE_YOU_SURE, s_nScreenEmpty);
	}
	Glib::signal_timeout().connect_once(sigc::bind(
		sigc::mem_fun(*this, &GameWindow::gameInterruptOut), eInterruptType, bAsk), 0);
}
void GameWindow::gameInterruptOut(GameProxy::INTERRUPT_TYPE eInterruptType, bool bAsk) noexcept
{
//std::cout << "GameWindow::gameInterruptOut()  eInterruptType=" << static_cast<int32_t>(eInterruptType) << '\n';
	if (bAsk) {
		const auto p0Question = [&]()
		{
			if (eInterruptType == GameProxy::INTERRUPT_QUIT_APP_ASK) {
				return "Are you sure you want to QUIT app?";
			} else if (eInterruptType == GameProxy::INTERRUPT_ABORT_ASK) {
				return "Are you sure you want to ABORT game?";
			} else {
				assert(eInterruptType == GameProxy::INTERRUPT_RESTART_ASK);
				return "Are you sure you want to RESTART game?";
			}
		}();
		m_eAnsweringInterrupt = eInterruptType;
		changeScreen(STATUS_MENU, s_nScreenAnswer, p0Question);
		return; //--------------------------------------------------------------
	}
	if ((eInterruptType == GameProxy::INTERRUPT_ABORT) || (eInterruptType == GameProxy::INTERRUPT_ABORT_ASK)) {
		endGame();
		gameViewEnded();
		m_bGameIsTicking = false;
		changeScreen(STATUS_MENU, s_nScreenMain, "");
	} else if ((eInterruptType == GameProxy::INTERRUPT_RESTART) || (eInterruptType == GameProxy::INTERRUPT_RESTART_ASK)) {
		endGame();
		gameViewEnded();
		m_bGameIsTicking = false;
		const bool bOk = startGame();
		if (bOk) {
			changeScreen(STATUS_PLAY, s_nScreenPlay, "");
		}
	} else {
		assert((eInterruptType == GameProxy::INTERRUPT_QUIT_APP_ASK) || (eInterruptType == GameProxy::INTERRUPT_QUIT_APP));
		Gtk::Window::close();
	}
}
std::string GameWindow::getCouldNotLoadThemeString(const std::string& sThemeName) noexcept
{
	return sThemeName.empty() ? "Could not load any theme!" : "Could not load theme: "
							+ m_refThemeLoader->getThemeInfo(sThemeName).m_sThemeErrorString;
}
bool GameWindow::startGame() noexcept
{
//std::cout << "GameWindow::startGame()" << '\n';
	if (m_bNextViewTickActive) {
		m_bStartGameAtNextViewTick = true;
		return true; //---------------------------------------------------------
	}
	m_bStartGameAtNextViewTick = false;

	assert(!m_bGameIsTicking);
	assert((!m_refGame) || !(m_refGame->isRunning()));

	assert(m_refGameLoader);
	assert(m_refThemeLoader);
	//
	auto sThemeName = getTheme();
	if (!m_refTheme) {
		changeScreen(STATUS_MENU, s_nScreenInfo, getCouldNotLoadThemeString(sThemeName));
		return false; //--------------------------------------------------------
	}
	//
	std::string sGameName = m_refPrefs->getGameName();
	if (sGameName.empty()) {
		sGameName = m_refGameLoader->getDefaultGameName();
		if (sGameName.empty()) {
			auto aNames = m_refGameLoader->getGameNames(*m_refPrefs);
			if (! aNames.empty()) {
				sGameName = aNames[0];
			}
			if (sGameName.empty()) {
				const std::string sErr = "Could not load Game! No games found!";
				//msgWarningBox(sErr);
				changeScreen(STATUS_MENU, s_nScreenInfo, sErr);
				return false; //------------------------------------------------
			}
		}
		m_refPrefs->setGameName(sGameName);
		AllPreferencesLoader* p0TPL = m_refAllPreferencesLoader.get();
		if (p0TPL != nullptr) {
			p0TPL->updatePreferences(m_refPrefs);
		}
		onChangedGame();
	}
	m_refPrefs->addGameToPlayedHistory(sGameName);
	if (m_refAllPreferencesLoader) {
		m_refAllPreferencesLoader->updatePreferences(m_refPrefs);
	}
	const auto& oGameInfo = m_refGameLoader->getGameInfo(sGameName);
	shared_ptr<Highscore> refHighscore;
	if (oGameInfo.m_refHighscoresDefinition && m_refHighscoresLoader) {
		refHighscore = m_refHighscoresLoader->getHighscore(sGameName, *m_refPrefs, oGameInfo.m_refHighscoresDefinition);
		if (refHighscore) {
			assert(refHighscore->getHighscoresDefinition() == oGameInfo.m_refHighscoresDefinition);
		}
	}
//std::cout << "--->Highscore adr: " << reinterpret_cast<int64_t>(refHighscore.get()) << '\n';
//std::cout << "--->HighscoreDefinition adr: " << reinterpret_cast<int64_t>(oGameInfo.m_refHighscoresDefinition.get()) << '\n';
	auto oPairGame = m_refGameLoader->getNewGame(sGameName, *this, m_refPrefs, m_refTheme->getNamed(), refHighscore);
	auto& refGame = oPairGame.first;
	const bool bHighscoresIgnored = oPairGame.second;
	if (!refGame) {
		std::string sErrorStr = oGameInfo.m_sGameErrorString;
		if (sErrorStr.empty()) {
			sErrorStr = "Game '" + sGameName + "' not compatible with current Preferences."
						+ "\nSee Game Description tab in Games dialog.";
		}
		const std::string sErr = "Could not load Game!\n" + sErrorStr;
		changeScreen(STATUS_MENU, s_nScreenInfo, sErr);
		return false; //--------------------------------------------------------
	} else if (bHighscoresIgnored) {
		std::string sWarning = "Warning! Highscores no longer valid";
		//msgWarningBox(sWarning);
		std::cout << sWarning << 'n';
	}
	m_refGame = std::move(refGame);
	m_refHighscoresDefinition = oGameInfo.m_refHighscoresDefinition;

	m_nMaxViewTicks = m_refGame->getMaxViewTicks();

	m_refGame->setGameView(m_refGameView.get());
	m_refGame->start();

	const bool bOk = m_p0GameGtkDrawingArea->startGame(m_refGame, m_refPrefs, m_refTheme);
	if (!bOk) {
		const std::string sErr = "View error: theme couldn't create layout!";
		changeScreen(STATUS_MENU, s_nScreenInfo, sErr);
		return false; //--------------------------------------------------------
	}

	#ifndef NDEBUG
	const bool bRealized =
	#endif //NDEBUG
	m_p0GameGtkDrawingArea->translate_coordinates(*this, 0, 0, m_nDrawingAreaBaseX, m_nDrawingAreaBaseY);
//std::cout << " m_nDrawingAreaBaseX=" << m_nDrawingAreaBaseX << "  m_nDrawingAreaBaseY=" << m_nDrawingAreaBaseY << '\n';
	assert(bRealized);

	if (!m_refEventListener) {
		m_refEventListener = std::make_shared<stmi::EventListener>(
			[this](const shared_ptr<stmi::Event>& refEvent)
			{
				auto p0Event = refEvent.get();
				if (p0Event->getEventClass().isXYEvent()) {
					auto p0XYEvent = static_cast<stmi::XYEvent*>(p0Event);
					p0XYEvent->translateXY(- m_nDrawingAreaBaseX, - m_nDrawingAreaBaseY);
				}
				m_refGame->handleInput(refEvent);
			});
	}
	#ifndef NDEBUG
	const bool bAdded =
	#endif
	m_refDM->addEventListener(m_refEventListener);
	assert(bAdded);

	//OnChangedLevels

	m_oGT.reset();
	m_oGT.m_fGameInterval = m_refGame->gameInterval();
//std::cout << "GameWindow::startGame() game interval = " << m_oGT.m_fGameInterval << '\n';
	assert(m_oGT.m_nTotViewTicks > 0);
	const double fViewInterval = (m_oGT.m_fGameInterval / m_oGT.m_nTotViewTicks);
//std::cout << "GameWindow::startGame() view interval = " << fViewInterval << '\n';

	m_bGameIsTicking = true;
	m_refGameView->gameStarted();
	startNextViewTickTimer(fViewInterval);
//std::cout << "GameWindow::startGame 3" << '\n';
	// setting the edit mode this late allows the view to automatically
	// assign playback capabilities to players
	m_refPrefs->setEditMode(false);
	return true;
}
void GameWindow::endGame() noexcept
{
//std::cout << "GameWindow::endGame()  m_bGameIsTicking=" << m_bGameIsTicking << '\n';
	m_refPrefs->setEditMode(true);
	//m_refGameView->gameEnded();
	assert(m_refEventListener);
	m_refDM->removeEventListener(m_refEventListener, true);

	if (m_refGame->isRunning()) {
		m_refGame->end();
	}
}
void GameWindow::pauseGame() noexcept
{
//std::cout << "GameWindow::pauseGame()" << '\n';
	assert(m_bGameIsTicking);
	assert(m_refGame->isRunning());

	m_refGameView->gamePaused();
	m_refDM->removeEventListener(m_refEventListener, true);

	m_bGameIsTicking = false;
}

void GameWindow::resumeGame() noexcept
{
	assert(!m_bGameIsTicking);
	assert(m_refGame->isRunning());

	m_refDM->addEventListener(m_refEventListener);

	m_bGameIsTicking = true;
	m_refGameView->gameResumed();
	const double fViewInterval = (m_oGT.m_fGameInterval / m_oGT.m_nTotViewTicks);
	startNextViewTickTimer(fViewInterval);
}

void GameWindow::onChangedGame() noexcept
{
	const std::string sCurrentGameName = m_refPrefs->getGameName();
	m_p0LabelCurrentGame->set_text("Current game: " + (sCurrentGameName.empty() ? "-" : sCurrentGameName));
}
void GameWindow::onChangedPlayers() noexcept
{
	std::string sPlayers = "Player";
	const int32_t nTotPlayers = m_refPrefs->getTotPlayers();
	bool bPlayerNames = false;
	if (nTotPlayers <= 2) {
		if (nTotPlayers <= 2) {
			bPlayerNames = true;
		}
	}
	if (nTotPlayers > 1) {
		sPlayers += "s";
	}
	sPlayers += ":";
	const int32_t nTotTeams = m_refPrefs->getTotTeams();
	for (int32_t nTeam = 0; nTeam < nTotTeams; ++nTeam) {
		const auto& refTeam = m_refPrefs->getTeamFull(nTeam);
		const int32_t nTotMates = refTeam->getTotMates();
		if (nTeam > 0) {
			sPlayers += "  vs ";
		}
		if (bPlayerNames) {
			for (int32_t nMate = 0; nMate < nTotMates; ++nMate) {
				if (nMate > 0) {
					sPlayers += " +";
				}
				const auto& refPlayer = refTeam->getMateFull(nMate);
				if (refPlayer->isAI()) {
					sPlayers += " AI";
				} else {
					sPlayers += " " + refPlayer->getName();
				}
			}
		} else {
			if (refTeam->isAI()) {
				sPlayers += " AI";
			} else {
				sPlayers += " " + std::to_string(nTotMates);
			}
		}
	}
	m_p0LabelCurrentPlayers->set_text(sPlayers);
}
void GameWindow::onChangedTheme() noexcept
{
	//TODO reinit gameviews?
}

bool GameWindow::onResizeTimeout() noexcept
{
	const double fCurrentTime = m_oClock.elapsed();
//std::cout << "GameWindow::onResizeTimeout() m_fLastDelayedAllocation=" << m_fLastDelayedAllocation << " fCurrentTime=" << fCurrentTime << '\n';
	if (m_fLastDelayedAllocation + 1.0 * s_nResizeConnMillisec / 1000 < fCurrentTime) {
		const bool bDirty = m_p0GameGtkDrawingArea->delayedAllocation();
		if (bDirty) {
			//TODO to be changed as soon Gdk::Window is fixed
			gdk_window_invalidate_rect(get_window()->gobj(), nullptr, true);
			//Gdk::Window::invalidate_rect(nullptr, true);
		}
		m_fLastDelayedAllocation = m_oClock.elapsed();
	}
	return true;
}

void GameWindow::callbackNextViewTick() noexcept
{
	m_bNextViewTickActive = false;
	if (m_bStartGameAtNextViewTick) {
		//const bool bOk =
		startGame();
		//if (!bOk) {
		//	changeScreen(STATUS_MENU, s_nScreenMain, "");
		//}
		return; //--------------------------------------------------------------
	}
	if (!m_bGameIsTicking) {
		return; //--------------------------------------------------------------
	}
	const double fCurrentTime = m_oClock.elapsed();
	double fRest;

	const double fViewInterval = (m_oGT.m_fGameInterval / m_oGT.m_nTotViewTicks);

	++m_oGT.m_nCurViewTick;
	if (m_oGT.m_nCurViewTick == m_oGT.m_nTotViewTicks) {
		// new game interval started
		m_oGT.m_nTotViewTicks = m_oGT.m_nNextTotViewTicks;

		//if (m_refGame->isRunning()) {
		//	return;
		//}
		m_p0GameGtkDrawingArea->beforeGameTick();

		// game logic
		m_refGame->handleTimer(); // might change GameInterval

		m_oGT.m_fGameInterval = m_refGame->gameNextInterval();
		const double fNewViewInterval = (m_oGT.m_fGameInterval / m_oGT.m_nTotViewTicks);
		m_oGT.m_nCurViewTick = 0;

		m_p0GameGtkDrawingArea->sync(fNewViewInterval, m_oGT.m_nTotViewTicks);

		m_p0GameGtkDrawingArea->drawStep(0);

		const double fDiffTime = (m_oClock.elapsed() - fCurrentTime) * 1000;
		fRest = fNewViewInterval - fDiffTime; // millisec
		if (fRest < 1.0) { // frame rate too high
			fRest = 1.0;
			if (m_oGT.m_nTotViewTicks > 1) {
				m_oGT.m_nNextTotViewTicks = m_oGT.m_nTotViewTicks - 1; // decrease
			}
			m_oGT.m_nGameTicksPotentialViewTicksIncCounter = 0;
		} else if (fDiffTime <= (m_oGT.m_fGameInterval / (m_oGT.m_nTotViewTicks + 1))) {
			++m_oGT.m_nGameTicksPotentialViewTicksIncCounter;
			if (m_oGT.m_nGameTicksPotentialViewTicksIncCounter > s_nGameTicksPotentialViewTicksIncDo) {
				if (m_oGT.m_nNextTotViewTicks < m_nMaxViewTicks) {
					++m_oGT.m_nNextTotViewTicks; // increase frame rate
				}
				m_oGT.m_nGameTicksPotentialViewTicksIncCounter = 0;
			}
		} else { // just right frame rate
			m_oGT.m_nGameTicksPotentialViewTicksIncCounter = 0;
		}
	} else {
		m_p0GameGtkDrawingArea->drawStep(m_oGT.m_nCurViewTick);
		const double fDiffTime = (m_oClock.elapsed() - fCurrentTime) * 1000;
		fRest = fViewInterval - fDiffTime;
		if (fRest < 1.0) {
			fRest = 1.0;
		}
	}
	startNextViewTickTimer(fRest);
}

void GameWindow::onButtonNewGame() noexcept
{
	//startNextViewTickTimer(0);
	changeScreen(STATUS_PLAY, s_nScreenPlay, "");
	m_bWaitingForDrawingAreaSizeAllocate = true;

	const int32_t nWaitTransitionToFinish = s_nNewGameTransitionMillisec; // Doesn't work for 0
	Glib::signal_timeout().connect_once(sigc::mem_fun(*this, &GameWindow::onButtonNewGameOut), nWaitTransitionToFinish);
}
void GameWindow::onButtonNewGameOut() noexcept
{
	//const bool bOk =
	startGame();
	//if (!bOk) {
	//	changeScreen(STATUS_MENU, s_nScreenMain, "");
	//}
}
void GameWindow::onButtonHighscores() noexcept
{
	if (m_refHighscoresLoader) {
		const std::string& sGameName = (m_refPrefs->getGameName().empty() ? m_refGameLoader->getDefaultGameName() : m_refPrefs->getGameName());
		if (sGameName.empty()) {
			changeScreen(STATUS_MENU, s_nScreenInfo, "No game selected");
			return; //----------------------------------------------------------
		}
		const auto& oGameInfo = m_refGameLoader->getGameInfo(sGameName);
		if (!oGameInfo.m_refHighscoresDefinition) {
			changeScreen(STATUS_MENU, s_nScreenInfo, std::string{"No highscores defined for game "} + sGameName);
			return; //----------------------------------------------------------
		}
		auto aHighscores = m_refHighscoresLoader->getHighscores(sGameName, oGameInfo.m_refHighscoresDefinition);
		if (aHighscores.empty()) {
			changeScreen(STATUS_MENU, s_nScreenInfo, std::string{"No highscores yet for game "} + sGameName);
			return; //----------------------------------------------------------
		}
		if (!m_refHighscoreDialog) {
			m_refHighscoreDialog = Glib::RefPtr<HighscoreDialog>(new HighscoreDialog());
			m_refHighscoreDialog->set_default_size(get_width() / 1.0, get_height() / 1.0);
			m_refHighscoreDialog->set_transient_for(*this);
		}
		auto& oHighscoreDialog = *(m_refHighscoreDialog.operator->());
		oHighscoreDialog.run(aHighscores, m_refPrefs);
		//
		oHighscoreDialog.hide();
	}
}
void GameWindow::onButtonChoosePlayers() noexcept
{
	if (!m_refPlayersDialog) {
		m_refPlayersDialog = Glib::RefPtr<PlayersDialog>(new PlayersDialog(m_refStdConfig));
		m_refPlayersDialog->set_default_size(get_width() / 1.0, get_height() / 1.0);
		m_refPlayersDialog->set_transient_for(*this);
		//
		m_refPlayersDialogAccessor = std::make_shared<stmi::GtkAccessor>(m_refPlayersDialog);
	}
	auto& oPlayersDialog = *(m_refPlayersDialog.operator->());
	AllPreferencesLoader* p0TPL = m_refAllPreferencesLoader.get();
	auto refNewPrefs = (p0TPL == nullptr ? std::make_shared<AllPreferences>(*m_refPrefs) : p0TPL->getPreferencesCopy(m_refPrefs));
	auto refNewStdPrefs = std::static_pointer_cast<StdPreferences>(refNewPrefs);

	m_refStdConfig->getDeviceManager()->addAccessor(m_refPlayersDialogAccessor);
	// try to load a theme
	auto sThemeName = getTheme();
	if (! m_refTheme) {
		const bool bPerPlayerSounds = m_refStdConfig->soundEnabled() && m_refStdConfig->canPlayPerPlayerSounds();
		if (bPerPlayerSounds) {
			std::cout << "Sounds cannot be tested:\n  " << getCouldNotLoadThemeString(sThemeName) << '\n';
		}
	}
	oPlayersDialog.reInit(refNewStdPrefs, m_refTheme);
	const int nRet = oPlayersDialog.run();
	oPlayersDialog.hide();
	m_refStdConfig->getDeviceManager()->removeAccessor(m_refPlayersDialogAccessor);

//std::cout << "onMenuChoosePlayers() nRet=" << nRet << '\n';
	if (nRet == Gtk::RESPONSE_OK) {
		// Note: until the next game is started m_refGame and indirectly m_refGameView
		//       still point to the old instance
		m_refPrefs = refNewPrefs;
		if (p0TPL != nullptr) {
			p0TPL->updatePreferences(m_refPrefs);
		}
		onChangedPlayers();
	}
}
void GameWindow::onButtonChooseGame() noexcept
{
	assert(m_refGameLoader);
	if (!m_refGameDialog) {
		m_refGameDialog = Glib::RefPtr<GameDialog>(new GameDialog(m_refStdConfig, *m_refGameLoader, *this));
		m_refGameDialog->set_default_size(get_width() / 1.0, get_height() / 1.0);
		m_refGameDialog->set_transient_for(*this);
	}
	auto& oGameDialog = *(m_refGameDialog.operator->());

	auto sThemeName = getTheme();
	if (!m_refTheme) {
		changeScreen(STATUS_MENU, s_nScreenInfo, getCouldNotLoadThemeString(sThemeName));
		return; //--------------------------------------------------------------
	}

	const int nRet = oGameDialog.run(m_refPrefs, m_refTheme);
	if (nRet == Gtk::RESPONSE_OK) {
		AllPreferencesLoader* p0TPL = m_refAllPreferencesLoader.get();
		if (p0TPL != nullptr) {
			p0TPL->updatePreferences(m_refPrefs);
		}
		onChangedGame();
	}
	//
	oGameDialog.hide();
}
void GameWindow::onButtonChooseTheme() noexcept
{
	if (!m_refThemeDialog) {
		m_refThemeDialog = Glib::RefPtr<ThemeDialog>(new ThemeDialog(m_refStdConfig, *m_refThemeLoader, m_refTheme));
		m_refThemeDialog->set_default_size(get_width() / 1.0, get_height() / 1.0);
		m_refThemeDialog->set_transient_for(*this);
	}
	auto& oThemeDialog = *(m_refThemeDialog.operator->());
	assert(m_refThemeLoader);

	const int nRet = oThemeDialog.run(m_refPrefs);
	if (nRet == Gtk::RESPONSE_OK) {
		AllPreferencesLoader* p0TPL = m_refAllPreferencesLoader.get();
		if (p0TPL != nullptr) {
			p0TPL->updatePreferences(m_refPrefs);
		}
	}
	//
	oThemeDialog.hide();
}
void GameWindow::onButtonAbout() noexcept
{
	changeScreen(STATUS_MENU, s_nScreenAbout, "");
}
void GameWindow::onButtonQuit() noexcept
{
	Gtk::Window::close();
}
void GameWindow::onButtonResume() noexcept
{
	resumeGame();
	changeScreen(STATUS_PLAY, s_nScreenPlay, "");

	// If the size of the window changed inform view
	m_bWaitingForDrawingAreaSizeAllocate = true;
}
void GameWindow::onButtonAnswerYes() noexcept
{
	Glib::signal_timeout().connect_once(sigc::bind(
		sigc::mem_fun(*this, &GameWindow::gameInterruptOut), m_eAnsweringInterrupt, false), 0);
}
void GameWindow::onButtonAnswerNo() noexcept
{
	resumeGame();
	changeScreen(STATUS_PLAY, s_nScreenPlay, "");
	// If the size of the window changed inform view
	m_bWaitingForDrawingAreaSizeAllocate = true;
}
void GameWindow::onButtonInfoOk() noexcept
{
	changeScreen(STATUS_MENU, s_nScreenMain, "");
}
void GameWindow::onButtonAboutOk() noexcept
{
	onButtonInfoOk();
}

void GameWindow::onDrawingAreaSizeAllocate(Gtk::Allocation& /*oAllocation*/) noexcept
{
//std::cout << "++++++  oAllocation = " << oAllocation.get_x() << "," << oAllocation.get_y();
//std::cout << "," << oAllocation.get_width() << "," << oAllocation.get_height() << '\n';
	if (!m_bWaitingForDrawingAreaSizeAllocate) {
		return;
	}
	m_bWaitingForDrawingAreaSizeAllocate = false;
	Glib::signal_timeout().connect_once(
			sigc::mem_fun(*this, &GameWindow::onButtonResumeOut), s_nResumeResizeMillisec);
}
void GameWindow::onButtonResumeOut() noexcept
{
	const bool bDirty = m_p0GameGtkDrawingArea->delayedAllocation();
	if (bDirty) {
		//TODO to be changed as soon Gdk::Window is fixed
		gdk_window_invalidate_rect(get_window()->gobj(), nullptr, true);
		//Gdk::Window::invalidate_rect(nullptr, true);
	}
}
void GameWindow::onButtonAbort() noexcept
{
	endGame();
	gameViewEnded();
	changeScreen(STATUS_MENU, s_nScreenMain, "");
}
std::string GameWindow::getTheme() noexcept
{
	if (m_refTheme) {
		// already loaded
		return Util::s_sEmptyString;
	}
	std::string sThemeName = m_refPrefs->getThemeName();
	if (sThemeName.empty()) {
		sThemeName = m_refThemeLoader->getDefaultThemeName();
	}
	m_refTheme = m_refThemeLoader->getTheme(sThemeName);
	if (!m_refTheme) {
		// loading error: return the offending theme name
		return sThemeName;
	}
	// ok
	return Util::s_sEmptyString;
}
void GameWindow::gotoMainScreen() noexcept
{
	deactivateResizeTimer();
	m_bGameIsTicking = false;
	changeScreen(STATUS_MENU, s_nScreenMain, "");
}
void GameWindow::gameViewEnded()
{
//std::cout << "GameWindow::gameViewEnded()" << '\n';
	m_refGameView->gameEnded();
}
bool GameWindow::on_key_press_event(GdkEventKey* p0GdkEvent)
{
//std::cout << "on_key_press_event kv=" << p0GdkEvent->keyval << " kc=" << p0GdkEvent->hardware_keycode << " --- st=" << p0GdkEvent->state << "------" << '\n';
	#ifndef NDEBUG
	if (m_refStdConfig->isTestMode()) {
		if ((p0GdkEvent->keyval == GDK_KEY_F12) && ((p0GdkEvent->state & GDK_SHIFT_MASK) != 0)) {
			if (m_refGame) {
				m_refGame->level(0)->dump(true, true, true, true, false, true);
			}
		}
	}
	#endif
//std::cout << "                   m_eStatus=" << static_cast<int32_t>(m_eStatus) << '\n';
	if (m_eStatus == STATUS_PLAY) {
		// propagate to device manager
		return false; //--------------------------------------------------------
	}
	if (m_eStatus == STATUS_WAIT_KEYPRESS) {
		gameViewEnded();
		gotoMainScreen();
		return true; //---------------------------------------------------------
	}
	return Gtk::Window::on_key_press_event(p0GdkEvent);
}
bool GameWindow::on_key_release_event(GdkEventKey* p0GdkEvent)
{
	if (m_eStatus == STATUS_PLAY) {
		// propagate to device manager
		return false; //--------------------------------------------------------
	}
	return Gtk::Window::on_key_release_event(p0GdkEvent);
}
bool GameWindow::on_button_press_event(GdkEventButton* p0GdkEvent)
{
//std::cout << "on_button_press_event ---------------------------------------------" << '\n';
//std::cout << "  x=" << p0GdkEvent->x << '\n';
//std::cout << "  y=" << p0GdkEvent->y << '\n';
//std::cout << "  button=" << p0GdkEvent->button << '\n';
	if (m_eStatus == STATUS_PLAY) {
		// propagate to device manager
		return false; //--------------------------------------------------------
	}
	if (m_eStatus == STATUS_WAIT_KEYPRESS) {
		gameViewEnded();
		gotoMainScreen();
		return true; //---------------------------------------------------------
	}
	return Gtk::Window::on_button_press_event(p0GdkEvent);
}
bool GameWindow::on_button_release_event(GdkEventButton* p0GdkEvent)
{
	if (m_eStatus == STATUS_PLAY) {
		// propagate to device manager
		return false; //--------------------------------------------------------
	}
	return Gtk::Window::on_button_release_event(p0GdkEvent);
}
bool GameWindow::on_motion_notify_event(GdkEventMotion* p0GdkEvent)
{
	if (m_eStatus == STATUS_PLAY) {
		// propagate to device manager
		return false; //--------------------------------------------------------
	}
	return Gtk::Window::on_motion_notify_event(p0GdkEvent);
}
bool GameWindow::on_sig_touch_event(GdkEventTouch* p0TouchEvent)
{
	if (m_eStatus == STATUS_PLAY) {
		// propagate to device manager
		return false; //--------------------------------------------------------
	}
	if (m_eStatus == STATUS_WAIT_KEYPRESS) {
		if (p0TouchEvent->type == GDK_TOUCH_BEGIN) {
			gameViewEnded();
			gotoMainScreen();
			gotoMainScreen();
		}
		return true; //---------------------------------------------------------
	}
	return false;
}

} // namespace stmg
