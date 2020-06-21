/*
 * File:   themescreen.cc
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

#include "themescreen.h"

#include "themeloader.h"
#include "../gtkutil/gtkutilpriv.h"
#include "../gamewindow.h"

#include <stmm-games-file/file.h>
#include <stmm-games-file/allpreferences.h>

#include <stmm-games/stdconfig.h>
#include <stmm-games/util/util.h>

#include <iostream>
#include <vector>
#include <cassert>

namespace stmg
{

static const Glib::ustring s_sThemeScreenNameChoose = "Choose";
static const Glib::ustring s_sThemeScreenNameInfo = "Info";

static constexpr const int32_t s_nButtonLeftRightMargin = 20;

ThemeScreen::ThemesTreeView::ThemesTreeView(ThemeScreen* p0Dialog, const Glib::RefPtr< Gtk::TreeModel >& refModel) noexcept
: Gtk::TreeView(refModel)
, m_p0Dialog(p0Dialog)
{
	set_enable_search(false);
	assert(p0Dialog != nullptr);
}
bool ThemeScreen::ThemesTreeView::on_key_press_event(GdkEventKey* p0Event)
{
	if (m_p0Dialog->m_nCurrentScreen == s_nScreenThemeChoose) {
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
ThemeScreen::ThemeScreen(GameWindow& oGameWindow, const shared_ptr<StdConfig>& refStdConfig
						, ThemeLoader& oThemeLoader) noexcept
: m_oGameWindow(oGameWindow)
, m_refStdConfig(refStdConfig)
, m_oThemeLoader(oThemeLoader)
, m_bRegenerateThemesListInProgress(false)
{
	assert(refStdConfig);
}

Gtk::Widget* ThemeScreen::init() noexcept
{
	static_assert(s_nTabThemes < s_nTabLoading, "");

	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;

	m_p0ThemeScreenBoxMain = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

	//Gtk::Label* m_p0LabelTitle = Gtk::manage(new Gtk::Label("---- Choose theme ----"));
	//m_p0ThemeScreenBoxMain->pack_start(*m_p0LabelTitle, false, false);
	//	m_p0LabelTitle->set_margin_top(3);
	//	m_p0LabelTitle->set_margin_bottom(3);
	//	{
	//	Pango::AttrList oAttrList;
	//	Pango::AttrInt oAttrWeight = Pango::Attribute::create_attr_weight(Pango::WEIGHT_HEAVY);
	//	oAttrList.insert(oAttrWeight);
	//	m_p0LabelTitle->set_attributes(oAttrList);
	//	}

	m_p0StackThemeScreens = Gtk::manage(new Gtk::Stack());
	m_p0ThemeScreenBoxMain->pack_start(*m_p0StackThemeScreens, true, true);
		m_p0StackThemeScreens->set_transition_type(Gtk::StackTransitionType::STACK_TRANSITION_TYPE_NONE);

	Gtk::Box* m_p0ThemeScreenBoxThemes = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_p0StackThemeScreens->add(*m_p0ThemeScreenBoxThemes, s_sThemeScreenNameChoose);

		m_p0NotebookThemes = Gtk::manage(new Gtk::Notebook());
		m_p0ThemeScreenBoxThemes->pack_start(*m_p0NotebookThemes, true, true);
			m_p0NotebookThemes->signal_switch_page().connect(
							sigc::mem_fun(*this, &ThemeScreen::onNotebookSwitchPage) );

		Gtk::Label* m_p0TabLabelThemes = Gtk::manage(new Gtk::Label("Themes"));
		Gtk::Box* m_p0TabVBoxThemesShow = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0NotebookThemes->append_page(*m_p0TabVBoxThemesShow, *m_p0TabLabelThemes);
			Gtk::ScrolledWindow* m_p0ScrolledThemes = Gtk::manage(new Gtk::ScrolledWindow());
			m_p0TabVBoxThemesShow->pack_start(*m_p0ScrolledThemes, true, true);
				m_p0ScrolledThemes->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
				m_refTreeModelThemes = Gtk::TreeStore::create(m_oThemesColumns);
				m_p0TreeViewThemes = Gtk::manage(new ThemesTreeView(this, m_refTreeModelThemes));
				m_p0ScrolledThemes->add(*m_p0TreeViewThemes);
					m_p0TreeViewThemes->append_column("Name", m_oThemesColumns.m_oColNameStatus);
					m_p0TreeViewThemes->append_column("Thumb", m_oThemesColumns.m_oColThumbnail);
					refTreeSelection = m_p0TreeViewThemes->get_selection();
					refTreeSelection->signal_changed().connect(
									sigc::mem_fun(*this, &ThemeScreen::onThemeSelectionChanged));

		if (m_refStdConfig->isTestMode()) {
			Gtk::Label* m_p0TabLabelLoading = Gtk::manage(new Gtk::Label("Test"));
			Gtk::Box* m_p0TabVBoxLoading = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
			m_p0NotebookThemes->append_page(*m_p0TabVBoxLoading, *m_p0TabLabelLoading);
				m_p0TextLoadingError = Gtk::manage(new Gtk::TextView());
				m_p0TabVBoxLoading->pack_start(*m_p0TextLoadingError, true, true);
					m_p0TextLoadingError->set_wrap_mode(Gtk::WrapMode::WRAP_WORD);
					m_refTextBufferLoadingError = m_p0TextLoadingError->get_buffer();
		}

		Gtk::Button* m_p0ButtonOk = Gtk::manage(new Gtk::Button("Ok"));
		m_p0ThemeScreenBoxThemes->pack_start(*m_p0ButtonOk, false, false);
			m_p0ButtonOk->set_margin_left(s_nButtonLeftRightMargin);
			m_p0ButtonOk->set_margin_right(s_nButtonLeftRightMargin);
			m_p0ButtonOk->set_margin_top(5);
			m_p0ButtonOk->set_margin_bottom(5);
			m_p0ButtonOk->signal_clicked().connect(
							sigc::mem_fun(*this, &ThemeScreen::onButtonOk) );
		Gtk::Button* m_p0ButtonCancel  = Gtk::manage(new Gtk::Button("Cancel"));
		m_p0ThemeScreenBoxThemes->pack_start(*m_p0ButtonCancel, false, false);
			m_p0ButtonCancel->set_margin_left(s_nButtonLeftRightMargin);
			m_p0ButtonCancel->set_margin_right(s_nButtonLeftRightMargin);
			m_p0ButtonCancel->set_margin_top(5);
			m_p0ButtonCancel->set_margin_bottom(5);
			m_p0ButtonCancel->signal_clicked().connect(
							sigc::mem_fun(*this, &ThemeScreen::onButtonCancel) );
	m_aThemeScreens[s_nScreenThemeChoose] = m_p0ThemeScreenBoxThemes;

	m_p0ThemeScreenBoxInfo = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_p0StackThemeScreens->add(*m_p0ThemeScreenBoxInfo, s_sThemeScreenNameInfo);
		Gtk::Box* m_p0BoxInfo = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0ThemeScreenBoxInfo->pack_start(*m_p0BoxInfo, true, true);
			addBigSeparator(m_p0BoxInfo, true);
			m_p0LabelThemeInfoText = Gtk::manage(new Gtk::Label("Info"));
			m_p0BoxInfo->pack_start(*m_p0LabelThemeInfoText, true, true);
				m_p0LabelThemeInfoText->set_line_wrap(true);
				m_p0LabelThemeInfoText->set_margin_top(5);
				m_p0LabelThemeInfoText->set_margin_bottom(5);
			Gtk::Button* m_p0ButtonThemeInfoOk = Gtk::manage(new Gtk::Button("Ok"));
			m_p0BoxInfo->pack_start(*m_p0ButtonThemeInfoOk, false, false);
				m_p0ButtonThemeInfoOk->set_margin_left(s_nButtonLeftRightMargin);
				m_p0ButtonThemeInfoOk->set_margin_right(s_nButtonLeftRightMargin);
				m_p0ButtonThemeInfoOk->set_margin_top(5);
				m_p0ButtonThemeInfoOk->set_margin_bottom(5);
				m_p0ButtonThemeInfoOk->signal_clicked().connect(
								sigc::mem_fun(*this, &ThemeScreen::onButtonThemeInfoOk) );
			addBigSeparator(m_p0BoxInfo, true);
	m_aThemeScreens[s_nScreenThemeInfo] = m_p0ThemeScreenBoxInfo;

	return m_p0ThemeScreenBoxMain;
}
void ThemeScreen::onButtonThemeInfoOk() noexcept
{
	changeScreen(s_nScreenThemeChoose, "");
}
void ThemeScreen::changeScreen(int32_t nToScreen, const std::string& sMsg) noexcept
{
	if (m_nCurrentScreen == nToScreen) {
		return;
	}
	//const bool bIsTestMode = m_refStdConfig->isTestMode();

	m_nCurrentScreen = nToScreen;
	m_p0StackThemeScreens->set_visible_child(*m_aThemeScreens[m_nCurrentScreen]);
	if (m_nCurrentScreen == s_nScreenThemeChoose) {
		//
	} else if (m_nCurrentScreen == s_nScreenThemeInfo) {
		m_p0LabelThemeInfoText->set_text(sMsg);
	} else {
		assert(false);
	}
}

bool ThemeScreen::changeTo(const shared_ptr<AllPreferences>& refPrefs) noexcept
{
	assert(refPrefs);
	assert(m_refStdConfig == refPrefs->getStdConfig());

	m_refPrefs = refPrefs;

	m_sOldThemeName = m_refPrefs->getThemeName();
	m_sSelectedThemeName = m_sOldThemeName;

	regenerateThemesList();

	m_p0TreeViewThemes->grab_focus();

	changeScreen(s_nScreenThemeChoose, "");

	return true;
}
void ThemeScreen::onButtonOk() noexcept
{
	if (m_sSelectedThemeName.empty()) {
		changeScreen(s_nScreenThemeInfo, "No theme selected");
		return; //--------------------------------------------------------------
	}
	std::string sNewThemeName = m_sSelectedThemeName;
	if (sNewThemeName != m_sOldThemeName) {
		// try to create the theme instance
		auto refTheme = m_oThemeLoader.getTheme(sNewThemeName);
		if (! refTheme) {
			const std::string& sErrorString = m_oThemeLoader.getThemeInfo(sNewThemeName).m_sThemeErrorString;
			changeScreen(s_nScreenThemeInfo, "Couldn't load theme\n" + sErrorString);
			return; //----------------------------------------------------------
		}
	} else {
		sNewThemeName.clear();
	}
	m_oGameWindow.afterChooseTheme(sNewThemeName);
}
void ThemeScreen::onButtonCancel() noexcept
{
	m_oGameWindow.afterChooseTheme(Util::s_sEmptyString);
}

void ThemeScreen::regenerateThemesList() noexcept
{
//std::cout << "ThemeScreen::regenerateThemesList()" << '\n';
	m_bRegenerateThemesListInProgress = true;

	m_refTreeModelThemes->clear();

	const bool bIsTestMode = m_refStdConfig->isTestMode();
	int32_t nSelectedPosInList = -1;

	const auto& aNames = m_oThemeLoader.getThemeNames();
	int32_t nPosInList = 0;
	for (const auto& sThemeName : aNames) {
//std::cout << "ThemeScreen::regenerateThemesList()   sThemeName=" << sThemeName << '\n';
		const auto& oThemeInfo = m_oThemeLoader.getThemeInfo(sThemeName);
		if (oThemeInfo.m_bTesting && !bIsTestMode) {
			continue; // for(sThemeName) -------
		}
		const bool bLoadError = !oThemeInfo.m_sThemeErrorString.empty();
		if (m_sSelectedThemeName == sThemeName) {
			assert(nSelectedPosInList < 0);
			nSelectedPosInList = nPosInList;
		}
//std::cout << "ThemeScreen::regenerateThemesList()   bLoadError=" << bLoadError << '\n';
		Gtk::TreeModel::Row oRow = *(m_refTreeModelThemes->append());
		oRow[m_oThemesColumns.m_oColHiddenName] = sThemeName;
		const File& oFile = oThemeInfo.m_oThumbnailFile;
		if (oFile.isDefined()) {
			try {
				if (oFile.isBuffered()) {
					Glib::RefPtr<Gio::MemoryInputStream> refInput = Gio::MemoryInputStream::create();
					refInput->add_data(oFile.getBuffer(), oFile.getBufferSize());
					oRow[m_oThemesColumns.m_oColThumbnail] = Gdk::Pixbuf::create_from_stream(refInput);
				} else {
					oRow[m_oThemesColumns.m_oColThumbnail] = Gdk::Pixbuf::create_from_file(oFile.getFullPath());
				}
			} catch (...) {
				// no thumbnail
				std::cout << "Couldn't load theme thumbnail " << (oFile.isBuffered() ? "" : oFile.getFullPath()) << '\n';
			}
		}
		std::string sNameStatus = sThemeName;
		if (bLoadError) {
			sNameStatus += " [Err]";
		}
//std::cout << "ThemeScreen::regenerateThemesList()   sNameStatus=" << sNameStatus << '\n';
		oRow[m_oThemesColumns.m_oColNameStatus] = sNameStatus;
		++nPosInList;
	}
	m_p0TreeViewThemes->expand_all();
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_p0TreeViewThemes->get_selection();
	Gtk::TreeModel::Path oPath;
	if (nSelectedPosInList >= 0) {
		oPath.push_back(nSelectedPosInList);
	} else {
		m_sSelectedThemeName = "";
	}
	refTreeSelection->select(oPath);
	m_p0TreeViewThemes->scroll_to_row(oPath);

	m_bRegenerateThemesListInProgress = false;
	//
	regenerateThemeInfos();
}
void ThemeScreen::regenerateThemeInfos() noexcept
{
	if (m_sSelectedThemeName.empty()) {
		if (m_refStdConfig->isTestMode()) {
			m_refTextBufferLoadingError->set_text("");
		}
	} else {
		if (m_refStdConfig->isTestMode()) {
			const auto& oThemeInfo = m_oThemeLoader.getThemeInfo(m_sSelectedThemeName);
			std::string sError = oThemeInfo.m_sThemeErrorString;
			if (oThemeInfo.m_bLoaded && sError.empty()) {
				sError = "The game was successfully loaded.";
			}
			m_refTextBufferLoadingError->set_text(sError);
		}
	}
}
void ThemeScreen::onThemeSelectionChanged() noexcept
{
	if (m_bRegenerateThemesListInProgress) {
		return; //--------------------------------------------------------------
	}
	if (! m_refPrefs) {
		return; //--------------------------------------------------------------
	}
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_p0TreeViewThemes->get_selection();
	Gtk::TreeModel::iterator it = refTreeSelection->get_selected();
	bool bSelectedThemeNameChanged = false;
	if (it) {
		Gtk::TreeModel::Row row = *it;
		const Glib::ustring& sNewThemeName = row[m_oThemesColumns.m_oColHiddenName];
		if (sNewThemeName != m_sSelectedThemeName) {
			bSelectedThemeNameChanged = true;
			m_sSelectedThemeName = sNewThemeName;
		}
	} else if (!m_sSelectedThemeName.empty()) {
		m_sSelectedThemeName.clear();
		bSelectedThemeNameChanged = true;
	}
	if (bSelectedThemeNameChanged) {
		regenerateThemeInfos();
	}
}

void ThemeScreen::onNotebookSwitchPage(Gtk::Widget*, guint /*nPageNum*/) noexcept
{
	if (!m_refPrefs) {
		return; //--------------------------------------------------------------
	}
	const int32_t nCurPage = m_p0NotebookThemes->get_current_page();
	if (nCurPage == s_nTabLoading) {
		if (m_sSelectedThemeName.empty()) {
			m_p0NotebookThemes->set_current_page(s_nTabThemes);
			changeScreen(s_nScreenThemeInfo, "Select a theme first");
			return; //----------------------------------------------------------
		}
		const auto& oThemeInfo = m_oThemeLoader.getThemeInfo(m_sSelectedThemeName);
		if (!oThemeInfo.m_bLoaded) {
			// load the theme
			m_oThemeLoader.getTheme(m_sSelectedThemeName);
		}
		regenerateThemeInfos();
		regenerateThemesList();
	} else {
		//
	}
}

} // namespace stmg
