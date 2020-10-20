/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   gamescreen.cc
 */

#include "gamescreen.h"

#include "theme.h"
#include "../gtkutil/gtkutilpriv.h"
#include "../gamewindow.h"

#include <stmm-games-file/file.h>
#include <stmm-games-file/gameloader.h>
#include <stmm-games-file/gameconstraints.h>
#include <stmm-games-file/allpreferences.h>

#include <stmm-games/appconstraints.h>
#include <stmm-games/stdconfig.h>
#include <stmm-games/util/util.h>

#include <iostream>
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <limits>
#include <numeric>
#include <vector>
#include <utility>
#include <cassert>

namespace stmg { class Highscore; }

namespace stmg
{

static const Glib::ustring s_sGameScreenNameChoose = "Choose";
static const Glib::ustring s_sGameScreenNameInfo = "Info";

static constexpr const int32_t s_nButtonLeftRightMargin = 20;

GameScreen::GamesTreeView::GamesTreeView(GameScreen* p0Dialog, const Glib::RefPtr< Gtk::TreeModel >& refModel) noexcept
: Gtk::TreeView(refModel)
, m_p0Dialog(p0Dialog)
{
	set_enable_search(false);
	assert(p0Dialog != nullptr);
}
bool GameScreen::GamesTreeView::on_key_press_event(GdkEventKey* p0Event)
{
	if (m_p0Dialog->m_nCurrentScreen == s_nScreenGameChoose) {
		if (p0Event->keyval == GDK_KEY_Return) {
			m_p0Dialog->onButtonOk();
			return true; //-----------------------------------------------------
		} else if (p0Event->keyval == GDK_KEY_Escape) {
			m_p0Dialog->onButtonCancel();
			return true; //-----------------------------------------------------
		}
	}
	return Gtk::TreeView::on_key_press_event(p0Event);
}

////////////////////////////////////////////////////////////////////////////////
GameScreen::GameScreen(GameWindow& oGameWindow, const shared_ptr<StdConfig>& refStdConfig
						, GameLoader& oGameLoader) noexcept
: m_oGameWindow(oGameWindow)
, m_refStdConfig(refStdConfig)
, m_oGameLoader(oGameLoader)
, m_oGameOwner(oGameWindow)
, m_bRegenerateGamesListInProgress(false)
, m_bShowAllGames(false)
, m_bShowPlayedHistory(false)
{
	assert(refStdConfig);
}

Gtk::Widget* GameScreen::init() noexcept
{
	static_assert(s_nTabGames < s_nTabDescription, "");
	static_assert(s_nTabDescription < s_nTabLoading, "");

	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;

	m_p0GameScreenBoxMain = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

	//Gtk::Label* m_p0LabelTitle = Gtk::manage(new Gtk::Label("---- Choose game ----"));
	//m_p0GameScreenBoxMain->pack_start(*m_p0LabelTitle, false, false);
	//	m_p0LabelTitle->set_margin_top(3);
	//	m_p0LabelTitle->set_margin_bottom(3);
	//	{
	//	Pango::AttrList oAttrList;
	//	Pango::AttrInt oAttrWeight = Pango::Attribute::create_attr_weight(Pango::WEIGHT_HEAVY);
	//	oAttrList.insert(oAttrWeight);
	//	m_p0LabelTitle->set_attributes(oAttrList);
	//	}

	m_p0StackGameScreens = Gtk::manage(new Gtk::Stack());
	m_p0GameScreenBoxMain->pack_start(*m_p0StackGameScreens, true, true);
		m_p0StackGameScreens->set_transition_type(Gtk::StackTransitionType::STACK_TRANSITION_TYPE_NONE);

	Gtk::Box* m_p0GameScreenBoxGames = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_p0StackGameScreens->add(*m_p0GameScreenBoxGames, s_sGameScreenNameChoose);

		m_p0NotebookGames = Gtk::manage(new Gtk::Notebook());
		m_p0GameScreenBoxGames->pack_start(*m_p0NotebookGames, true, true);
			m_p0NotebookGames->signal_switch_page().connect(
							sigc::mem_fun(*this, &GameScreen::onNotebookSwitchPage) );

		std::iota(std::begin(m_aPageIndex), std::end(m_aPageIndex), 0);
		static_assert(s_nTabGames < s_nTabDescription, "");
		static_assert(s_nTabDescription < s_nTabLoading, "");
		constexpr int32_t nTotTabs = sizeof(m_aPageIndex) / sizeof(m_aPageIndex[0]);
		static_assert(nTotTabs == 3, "");

		Gtk::Label* m_p0TabLabelGames = Gtk::manage(new Gtk::Label("Games"));
		Gtk::Box* m_p0TabVBoxGamesShow = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_aPageIndex[s_nTabGames] = m_p0NotebookGames->append_page(*m_p0TabVBoxGamesShow, *m_p0TabLabelGames);
			Gtk::ScrolledWindow* m_p0ScrolledGames = Gtk::manage(new Gtk::ScrolledWindow());
			m_p0TabVBoxGamesShow->pack_start(*m_p0ScrolledGames, true, true);
				m_p0ScrolledGames->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
				m_refTreeModelGames = Gtk::TreeStore::create(m_oGamesColumns);
				m_p0TreeViewGames = Gtk::manage(new GamesTreeView(this, m_refTreeModelGames));
				m_p0ScrolledGames->add(*m_p0TreeViewGames);
					m_p0TreeViewGames->append_column("Name", m_oGamesColumns.m_oColNameStatus);
					m_p0TreeViewGames->append_column("Thumb", m_oGamesColumns.m_oColThumbnail);
					refTreeSelection = m_p0TreeViewGames->get_selection();
					refTreeSelection->signal_changed().connect(
									sigc::mem_fun(*this, &GameScreen::onGameSelectionChanged));
			m_p0CheckShowAllGames = Gtk::manage(new Gtk::CheckButton("All games"));
			m_p0TabVBoxGamesShow->pack_start(*m_p0CheckShowAllGames, false, false, 5);
				m_p0CheckShowAllGames->signal_clicked().connect(sigc::mem_fun(*this, &GameScreen::onShowAllGamesChanged));
			m_p0CheckShowPlayedHistory = Gtk::manage(new Gtk::CheckButton("Show played games history"));
			m_p0TabVBoxGamesShow->pack_start(*m_p0CheckShowPlayedHistory, false, false, 5);
				m_p0CheckShowPlayedHistory->signal_clicked().connect(sigc::mem_fun(*this, &GameScreen::onShowPlayedHistoryChanged));

		Gtk::Label* m_p0TabLabelDescription = Gtk::manage(new Gtk::Label("Description"));
		Gtk::Box* m_p0TabVBoxDescription = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_aPageIndex[s_nTabDescription] = m_p0NotebookGames->append_page(*m_p0TabVBoxDescription, *m_p0TabLabelDescription);
			m_p0TabVBoxDescription->set_border_width(5);
			m_p0ImageGameImage = Gtk::manage(new Gtk::Image());
			m_p0TabVBoxDescription->pack_start(*m_p0ImageGameImage, false, true);
			m_p0LabelGameName = Gtk::manage(new Gtk::Label("-"));
			m_p0TabVBoxDescription->pack_start(*m_p0LabelGameName, false, true);
				m_p0LabelGameName->set_margin_top(5);
				m_p0LabelGameName->set_margin_bottom(5);
				{
				Pango::AttrList oAttrList;
				Pango::AttrColor oAttrColor = Pango::Attribute::create_attr_foreground(std::numeric_limits<guint16>::max()
																						, std::numeric_limits<guint16>::min()
																						, std::numeric_limits<guint16>::min());
				oAttrList.insert(oAttrColor);
				Pango::AttrInt oAttrWeight = Pango::Attribute::create_attr_weight(Pango::WEIGHT_HEAVY);
				oAttrList.insert(oAttrWeight);
				Pango::AttrFloat oAttrScale = Pango::Attribute::create_attr_scale(2.0);
				oAttrList.insert(oAttrScale);
				m_p0LabelGameName->set_attributes(oAttrList);
				}
			Gtk::ScrolledWindow* m_p0ScrolledDescription = Gtk::manage(new Gtk::ScrolledWindow());
			m_p0TabVBoxDescription->pack_start(*m_p0ScrolledDescription, true, true);
				m_p0ScrolledDescription->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
				Gtk::Box* m_p0VBoxScroller = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
				m_p0ScrolledDescription->add(*m_p0VBoxScroller);
					m_p0TextDescription = Gtk::manage(new Gtk::TextView());
					m_p0VBoxScroller->pack_start(*m_p0TextDescription, true, true);
						m_p0TextDescription->set_wrap_mode(Gtk::WrapMode::WRAP_WORD);
						m_p0TextDescription->set_left_margin(5);
						m_p0TextDescription->set_right_margin(5);
						m_refTextBufferDescription = m_p0TextDescription->get_buffer();
					Gtk::Label* m_p0LabelConstraints = Gtk::manage(new Gtk::Label("Constraints:"));
					m_p0VBoxScroller->pack_start(*m_p0LabelConstraints, true, true);
						m_p0LabelConstraints->set_halign(Gtk::Align::ALIGN_START);
						m_p0LabelConstraints->set_margin_top(8);
						m_p0LabelConstraints->set_margin_bottom(3);
						{
						Pango::AttrList oAttrList;
						Pango::AttrInt oAttrWeight = Pango::Attribute::create_attr_weight(Pango::WEIGHT_HEAVY);
						oAttrList.insert(oAttrWeight);
						m_p0LabelConstraints->set_attributes(oAttrList);
						}
					m_p0TextConstraints = Gtk::manage(new Gtk::TextView());
					m_p0VBoxScroller->pack_start(*m_p0TextConstraints, true, true);
						m_p0TextConstraints->set_wrap_mode(Gtk::WrapMode::WRAP_WORD);
						m_refTextBufferConstraints = m_p0TextConstraints->get_buffer();
					Gtk::Label* m_p0LabelAuthors = Gtk::manage(new Gtk::Label("Authors:"));
					m_p0VBoxScroller->pack_start(*m_p0LabelAuthors, true, true);
						m_p0LabelAuthors->set_halign(Gtk::Align::ALIGN_START);
						m_p0LabelAuthors->set_margin_top(3);
						m_p0LabelAuthors->set_margin_bottom(3);
						{
						Pango::AttrList oAttrList;
						Pango::AttrInt oAttrWeight = Pango::Attribute::create_attr_weight(Pango::WEIGHT_HEAVY);
						oAttrList.insert(oAttrWeight);
						m_p0LabelAuthors->set_attributes(oAttrList);
						}
					m_p0TextAuthors = Gtk::manage(new Gtk::TextView());
					m_p0VBoxScroller->pack_start(*m_p0TextAuthors, true, true);
						m_p0TextAuthors->set_wrap_mode(Gtk::WrapMode::WRAP_WORD);
						m_p0TextAuthors->set_vscroll_policy(Gtk::ScrollablePolicy::SCROLL_MINIMUM);
						m_refTextBufferAuthors = m_p0TextAuthors->get_buffer();

		if (m_refStdConfig->isTestMode()) {
			Gtk::Label* m_p0TabLabelLoading = Gtk::manage(new Gtk::Label("Test"));
			Gtk::Box* m_p0TabVBoxLoading = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
			m_aPageIndex[s_nTabLoading] = m_p0NotebookGames->append_page(*m_p0TabVBoxLoading, *m_p0TabLabelLoading);
				m_p0TextLoadingError = Gtk::manage(new Gtk::TextView());
				m_p0TabVBoxLoading->pack_start(*m_p0TextLoadingError, true, true);
					m_p0TextLoadingError->set_wrap_mode(Gtk::WrapMode::WRAP_WORD);
					m_refTextBufferLoadingError = m_p0TextLoadingError->get_buffer();
		}

		Gtk::Button* m_p0ButtonOk = Gtk::manage(new Gtk::Button("Ok"));
		m_p0GameScreenBoxGames->pack_start(*m_p0ButtonOk, false, false);
			m_p0ButtonOk->set_margin_left(s_nButtonLeftRightMargin);
			m_p0ButtonOk->set_margin_right(s_nButtonLeftRightMargin);
			m_p0ButtonOk->set_margin_top(5);
			m_p0ButtonOk->set_margin_bottom(5);
			m_p0ButtonOk->signal_clicked().connect(
							sigc::mem_fun(*this, &GameScreen::onButtonOk) );
		Gtk::Button* m_p0ButtonCancel  = Gtk::manage(new Gtk::Button("Cancel"));
		m_p0GameScreenBoxGames->pack_start(*m_p0ButtonCancel, false, false);
			m_p0ButtonCancel->set_margin_left(s_nButtonLeftRightMargin);
			m_p0ButtonCancel->set_margin_right(s_nButtonLeftRightMargin);
			m_p0ButtonCancel->set_margin_top(5);
			m_p0ButtonCancel->set_margin_bottom(5);
			m_p0ButtonCancel->signal_clicked().connect(
							sigc::mem_fun(*this, &GameScreen::onButtonCancel) );
	m_aGameScreens[s_nScreenGameChoose] = m_p0GameScreenBoxGames;

	m_p0GameScreenBoxInfo = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_p0StackGameScreens->add(*m_p0GameScreenBoxInfo, s_sGameScreenNameInfo);
		Gtk::Box* m_p0BoxInfo = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0GameScreenBoxInfo->pack_start(*m_p0BoxInfo, true, true);
			addBigSeparator(m_p0BoxInfo, true);
			m_p0LabelGameInfoText = Gtk::manage(new Gtk::Label("Info"));
			m_p0BoxInfo->pack_start(*m_p0LabelGameInfoText, true, true);
				m_p0LabelGameInfoText->set_line_wrap(true);
				m_p0LabelGameInfoText->set_margin_top(5);
				m_p0LabelGameInfoText->set_margin_bottom(5);
			Gtk::Button* m_p0ButtonGameInfoOk = Gtk::manage(new Gtk::Button("Ok"));
			m_p0BoxInfo->pack_start(*m_p0ButtonGameInfoOk, false, false);
				m_p0ButtonGameInfoOk->set_margin_left(s_nButtonLeftRightMargin);
				m_p0ButtonGameInfoOk->set_margin_right(s_nButtonLeftRightMargin);
				m_p0ButtonGameInfoOk->set_margin_top(5);
				m_p0ButtonGameInfoOk->set_margin_bottom(5);
				m_p0ButtonGameInfoOk->signal_clicked().connect(
								sigc::mem_fun(*this, &GameScreen::onButtonGameInfoOk) );
			addBigSeparator(m_p0BoxInfo, true);
	m_aGameScreens[s_nScreenGameInfo] = m_p0GameScreenBoxInfo;

	return m_p0GameScreenBoxMain;
}
void GameScreen::onButtonGameInfoOk() noexcept
{
	changeScreen(s_nScreenGameChoose, "");
}
void GameScreen::changeScreen(int32_t nToScreen, const std::string& sMsg) noexcept
{
	if (m_nCurrentScreen == nToScreen) {
		return;
	}
	//const bool bIsTestMode = m_refStdConfig->isTestMode();

	m_nCurrentScreen = nToScreen;
	m_p0StackGameScreens->set_visible_child(*m_aGameScreens[m_nCurrentScreen]);
	if (m_nCurrentScreen == s_nScreenGameChoose) {
		//
	} else if (m_nCurrentScreen == s_nScreenGameInfo) {
		m_p0LabelGameInfoText->set_text(sMsg);
	} else {
		assert(false);
	}
}

bool GameScreen::changeTo(const shared_ptr<AllPreferences>& refPrefs, const shared_ptr<Theme>& refTheme) noexcept
{
	assert(refPrefs);
	assert(m_refStdConfig == refPrefs->getStdConfig());
	assert(refTheme);

	m_refPrefs = refPrefs;
	m_refTheme = refTheme;

	m_sOldGameName = m_refPrefs->getGameName();
	m_sSelectedGameName = m_sOldGameName;

	m_p0CheckShowAllGames->set_sensitive(true);
	m_p0CheckShowAllGames->set_active(m_bShowAllGames);
	m_p0CheckShowAllGames->set_visible(true);
	bool bShowHistory = (m_refStdConfig->getMaxGamePlayedHistory() > 0);
	m_p0CheckShowPlayedHistory->set_sensitive(true);
	m_p0CheckShowPlayedHistory->set_active(m_bShowPlayedHistory);
	m_p0CheckShowPlayedHistory->set_visible(bShowHistory);

	m_p0NotebookGames->set_current_page(s_nTabGames);

	regenerateGamesList();

	m_p0TreeViewGames->grab_focus();

	changeScreen(s_nScreenGameChoose, "");

	return true;
}
void GameScreen::onButtonOk() noexcept
{
//std::cout << "GameScreen::onButtonOk()  m_sSelectedGameName=" << m_sSelectedGameName << '\n';
	if (m_sSelectedGameName.empty()) {
		changeScreen(s_nScreenGameInfo, "No game selected");
		return; //--------------------------------------------------------------
	}
	std::string sNewGameName = m_sSelectedGameName;
//std::cout << "GameScreen::onButtonOk() sNewGameName=" << sNewGameName << '\n';
	if (sNewGameName != m_sOldGameName) {
		// try to create the game instance
		auto oPairGame = m_oGameLoader.getNewGame(sNewGameName, m_oGameOwner, m_refPrefs, m_refTheme->getNamed(), shared_ptr<Highscore>{});
		auto& refGame = oPairGame.first;
		if (! refGame) {
			const std::string& sErrorString = m_oGameLoader.getGameInfo(sNewGameName).m_sGameErrorString;
			if (sErrorString.empty()) {
				changeScreen(s_nScreenGameInfo, "This game is not compatible with the current preferences");
			} else {
				changeScreen(s_nScreenGameInfo, "Couldn't load game\n" + sErrorString);
			}
			return; //----------------------------------------------------------
		}
	} else {
		sNewGameName.clear();
	}
	m_oGameWindow.afterChooseGame(sNewGameName);
}
void GameScreen::onButtonCancel() noexcept
{
	m_oGameWindow.afterChooseGame(Util::s_sEmptyString);
}
void GameScreen::regenerateGamesList() noexcept
{
	if (!m_refPrefs) {
		return;
	}
	m_bRegenerateGamesListInProgress = true;

	m_refTreeModelGames->clear();

	const bool bIsTestMode = m_refStdConfig->isTestMode();
	int32_t nSelectedPosInList = -1;

	std::vector<std::string> aNames = (m_bShowPlayedHistory ? m_refPrefs->getPlayedGameHistory() : m_oGameLoader.getGameNames());
	int32_t nPosInList = 0;
	for (const auto& sGameName : aNames) {
//std::cout << "GameScreen::regenerateGamesList() sGameName=" << sGameName << '\n';
		if (m_bShowPlayedHistory) {
			const auto& aValidNames = m_oGameLoader.getGameNames();
			const auto& itFoundName = std::find(aValidNames.begin(), aValidNames.end(), sGameName);
			if (itFoundName == aValidNames.end()) {
				continue; // for(sGameName) -------
			}
		}
		const auto& oGameInfo = m_oGameLoader.getGameInfo(sGameName);
		if (oGameInfo.m_bTesting && !bIsTestMode) {
			continue; // for(sGameName) -------
		}
		const bool bGameIsSelectedByPrefs =  oGameInfo.m_oGameConstraints.isSelectedBy(*m_refPrefs);
//std::cout << "GameScreen::regenerateGamesList() bGameIsSelectedByPrefs=" << bGameIsSelectedByPrefs<< '\n';
		const bool bLoadError = !oGameInfo.m_sGameErrorString.empty();
//std::cout << "GameScreen::regenerateGamesList() bLoadError=" << bLoadError << '\n';
		if ((!bGameIsSelectedByPrefs) && !m_bShowAllGames)  {
			continue; // for(sGameName) -------
		}
		if (m_sSelectedGameName == sGameName) {
			assert(nSelectedPosInList < 0);
			nSelectedPosInList = nPosInList;
		}
//std::cout << "GameScreen::regenerateGamesList() nSelectedPosInList=" << nSelectedPosInList << '\n';
		Gtk::TreeModel::Row oRow = *(m_refTreeModelGames->append());
		oRow[m_oGamesColumns.m_oColHiddenName] = sGameName;
		const File& oFile = oGameInfo.m_oThumbnailFile;
//std::cout << "GameScreen::regenerateGamesList() oGameInfo.m_oThumbnailFile.isDefined()=" << oFile.isDefined() << '\n';
		if (oFile.isDefined()) {
			try {
				if (oFile.isBuffered()) {
					Glib::RefPtr<Gio::MemoryInputStream> refInput = Gio::MemoryInputStream::create();
					refInput->add_data(oFile.getBuffer(), oFile.getBufferSize());
					oRow[m_oGamesColumns.m_oColThumbnail] = Gdk::Pixbuf::create_from_stream(refInput);
				} else {
					oRow[m_oGamesColumns.m_oColThumbnail] = Gdk::Pixbuf::create_from_file(oFile.getFullPath());
				}
			} catch (...) {
				// no thumbnail
				std::cout << "Couldn't load game thumbnail " << (oFile.isBuffered() ? "" : oFile.getFullPath()) << '\n';
			}
		}
		std::string sNameStatus = sGameName;
		if (bLoadError) {
			sNameStatus += " [Err]";
		} else if (! bGameIsSelectedByPrefs) {
			sNameStatus = "(" + sNameStatus + ")";
		}
		oRow[m_oGamesColumns.m_oColNameStatus] = sNameStatus;
		++nPosInList;
	}
	m_p0TreeViewGames->expand_all();
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_p0TreeViewGames->get_selection();
	Gtk::TreeModel::Path oPath;
	if (nSelectedPosInList >= 0) {
		oPath.push_back(nSelectedPosInList);
	} else {
		m_sSelectedGameName = "";
	}
	refTreeSelection->select(oPath);
	m_p0TreeViewGames->scroll_to_row(oPath);
	//
	m_p0CheckShowAllGames->set_active(m_bShowAllGames);
	m_p0CheckShowPlayedHistory->set_active(m_bShowPlayedHistory);
	//
	m_bRegenerateGamesListInProgress = false;
	//
	regenerateGameInfos();
}
void GameScreen::regenerateGameInfos() noexcept
{
	if (m_sSelectedGameName.empty()) {
		m_p0ImageGameImage->clear();
		m_p0LabelGameName->set_label("");
		m_refTextBufferDescription->set_text("");
		m_refTextBufferConstraints->set_text("");
		m_refTextBufferAuthors->set_text("");
		if (m_refStdConfig->isTestMode()) {
			m_refTextBufferLoadingError->set_text("");
		}
		return; //--------------------------------------------------------------
	}
	const auto& oGameInfo = m_oGameLoader.getGameInfo(m_sSelectedGameName);
	const auto& oFile = oGameInfo.m_oThumbnailFile;
	if (oFile.isDefined()) {
		if (oFile.isBuffered()) {
			Glib::RefPtr<Gio::MemoryInputStream> refInput = Gio::MemoryInputStream::create();
			refInput->add_data(oFile.getBuffer(), oFile.getBufferSize());
			m_p0ImageGameImage->set(Gdk::Pixbuf::create_from_stream(refInput));
		} else {
			m_p0ImageGameImage->set(Gdk::Pixbuf::create_from_file(oFile.getFullPath()));
		}
	} else {
		m_p0ImageGameImage->clear();
	}
	m_p0LabelGameName->set_label(m_sSelectedGameName);
	m_refTextBufferDescription->set_text(oGameInfo.m_sGameDescription);
	const GameConstraints& oGameConstraints = oGameInfo.m_oGameConstraints;
	std::string sConstraints;
	const AppConstraints& oAppConstraints = m_refStdConfig->getAppConstraints();
	if (oAppConstraints.m_nTeamsMin != oAppConstraints.m_nTeamsMax) {
		const int32_t nMaxTeams = oGameConstraints.getMaxTeams();
		const int32_t nMinTeams = oGameConstraints.getMinTeams();
		const bool bATIOL = oGameConstraints.isAllTeamsInOneLevel();
		if (nMaxTeams > 1) {
			sConstraints.append(Util::stringCompose("Type: %1\n", (bATIOL ? "All teams in one level" : "One team per level")));
		}
		if (nMaxTeams == nMinTeams) {
			sConstraints.append(Util::stringCompose("Teams: %1\n", nMaxTeams));
		} else {
			sConstraints.append(Util::stringCompose("Teams: %1 - %2\n", nMinTeams, nMaxTeams));
		}
	}
	const int32_t nMaxPlayers = oGameConstraints.getMaxPlayers();
	const int32_t nMinPlayers = oGameConstraints.getMinPlayers();
	if (oGameConstraints.m_nMatesPerTeamMin == oGameConstraints.m_nMatesPerTeamMax) {
		sConstraints.append(Util::stringCompose("Mates per team: %1\n", oGameConstraints.m_nMatesPerTeamMax));
	} else {
		sConstraints.append(Util::stringCompose("Mates per team: %1 - %2\n", oGameConstraints.m_nMatesPerTeamMin, oGameConstraints.m_nMatesPerTeamMax));
	}
	if (nMinPlayers == nMaxPlayers) {
		sConstraints.append(Util::stringCompose("Players: %1\n", nMaxPlayers));
	} else {
		sConstraints.append(Util::stringCompose("Players: %1 - %2\n", nMinPlayers, nMaxPlayers));
	}
	if (m_refStdConfig->getAppConstraints().m_nAIMatesPerTeamMax > 0) {
		if (oGameConstraints.m_nAITeamsMax == oGameConstraints.m_nAITeamsMin) {
			sConstraints.append(Util::stringCompose("AI teams: %1\n", oGameConstraints.m_nAITeamsMax));
		} else {
			sConstraints.append(Util::stringCompose("AI teams: %1 - %2\n", oGameConstraints.m_nAITeamsMin, oGameConstraints.m_nAITeamsMax));
		}
		if (m_refStdConfig->getAppConstraints().m_bAllowMixedAIHumanTeam) {
			sConstraints.append(Util::stringCompose("AI-human mixed team: %1\n", (oGameConstraints.m_bAllowMixedAIHumanTeam ? "true" : "false")));
		}
	}
	if (oGameConstraints.m_refPrefSelector) {
		sConstraints.append("Game has additional constraints (game options related)\n");
	}
	m_refTextBufferConstraints->set_text(sConstraints);
	std::string sAuthors;
	for (const GameLoader::GameInfo::Author& oAuthor : oGameInfo.m_aAuthors) {
		if (! sAuthors.empty()) {
			sAuthors += "\n";
		}
		sAuthors += "Name: " + oAuthor.m_sName;
		if (! oAuthor.m_sEmail.empty()) {
			sAuthors += "\n  email: " + oAuthor.m_sEmail;
		}
		if (! oAuthor.m_sWebSite.empty()) {
			sAuthors += "\n  website: " + oAuthor.m_sWebSite;
		}
		if (! oAuthor.m_sRole.empty()) {
			sAuthors += "\n  role: " + oAuthor.m_sRole;
		}
	}
	m_refTextBufferAuthors->set_text(sAuthors);
	if (m_refStdConfig->isTestMode()) {
		std::string sError = oGameInfo.m_sGameErrorString;
		if (oGameInfo.m_bLoaded && sError.empty()) {
			sError = "The game was successfully loaded.";
		}
		m_refTextBufferLoadingError->set_text(sError);
	}
}
void GameScreen::onGameSelectionChanged() noexcept
{
	if (m_bRegenerateGamesListInProgress) {
		return;
	}
	if (! m_refPrefs) {
		return;
	}

	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_p0TreeViewGames->get_selection();
	Gtk::TreeModel::iterator it = refTreeSelection->get_selected();
	bool bSelectedGameNameChanged = false;
	if (it) {
		Gtk::TreeModel::Row row = *it;
		const Glib::ustring& sNewGameName = row[m_oGamesColumns.m_oColHiddenName];
		if (sNewGameName != m_sSelectedGameName) {
			bSelectedGameNameChanged = true;
			m_sSelectedGameName = sNewGameName;
		}
	} else if (!m_sSelectedGameName.empty()) {
		m_sSelectedGameName.clear();
		bSelectedGameNameChanged = true;
	}
	if (bSelectedGameNameChanged) {
		regenerateGameInfos();
	}
}
void GameScreen::onShowAllGamesChanged() noexcept
{
	if (m_bRegenerateGamesListInProgress) {
		return;
	}
	const bool bValue = m_p0CheckShowAllGames->get_active();
	m_bShowAllGames = bValue;
	regenerateGamesList();
}
void GameScreen::onShowPlayedHistoryChanged() noexcept
{
	if (m_bRegenerateGamesListInProgress) {
		return;
	}
	const bool bValue = m_p0CheckShowPlayedHistory->get_active();
	m_bShowPlayedHistory = bValue;
	regenerateGamesList();
}
void GameScreen::onNotebookSwitchPage(Gtk::Widget*, guint /*nPageNum*/) noexcept
{
	if (! m_refPrefs) {
		return;
	}
	const int32_t nCurPage = m_p0NotebookGames->get_current_page();
	if ((nCurPage == m_aPageIndex[s_nTabLoading]) || (nCurPage == m_aPageIndex[s_nTabDescription])) {
		if (m_sSelectedGameName.empty()) {
			m_p0NotebookGames->set_current_page(s_nTabGames);
			changeScreen(s_nScreenGameInfo, "Select a game first");
			return; //----------------------------------------------------------
		}
		if (nCurPage == m_aPageIndex[s_nTabLoading]) {
			const auto& oGameInfo = m_oGameLoader.getGameInfo(m_sSelectedGameName);
//std::cout << "GameScreen::onNotebookSwitchPage   1   err='" << oGameInfo.m_sGameErrorString << "'" << '\n';
			if (!oGameInfo.m_bLoaded) {
				// load the game
				auto oPairGame = m_oGameLoader.getNewGame(m_sSelectedGameName, m_oGameOwner, m_refPrefs, m_refTheme->getNamed(), shared_ptr<Highscore>{});
				if (! oPairGame.first) {
					if (oGameInfo.m_sGameErrorString.empty()) {
						changeScreen(s_nScreenGameInfo, "This game is not compatible with the current preferences");
					}
				}
			}
			regenerateGameInfos();
			regenerateGamesList();
		} else {
			//
		}
	} else {
		//
	}
}

} // namespace stmg
