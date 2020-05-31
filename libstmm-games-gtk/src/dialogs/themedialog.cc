/*
 * File:   themedialog.cc
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

#include "themedialog.h"

#include "allpreferences.h"
#include "themeloader.h"
#include "../gtkutil/gtkutilpriv.h"

#include <stmm-games-file/file.h>

#include <stmm-games/stdconfig.h>

#include <iostream>
#include <vector>

namespace stmg
{

static const Glib::ustring s_sThemeScreenNameChoose = "Choose";
static const Glib::ustring s_sThemeScreenNameInfo = "Info";

static constexpr const int32_t s_nButtonLeftRightMargin = 20;

ThemeDialog::ThemeDialog(const shared_ptr<StdConfig>& refStdConfig, ThemeLoader& oThemeLoader, shared_ptr<Theme>& refTheme) noexcept
: Gtk::Dialog("Choose theme", true)
, m_refStdConfig(refStdConfig)
, m_oThemeLoader(oThemeLoader)
, m_refTheme(refTheme)
, m_bRegenerateThemesListInProgress(false)
, m_bReRun(false)
{
	//set_title("Choose theme");
	set_default_size(400, 200);

	static_assert(s_nTabThemes < s_nTabLoading, "");

	////////////////////////////////////////////////////////////////////////////
	Gtk::Button* m_p0ButtonOk = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	assert(m_p0ButtonOk != nullptr);
		m_p0ButtonOk->signal_clicked().connect(
						sigc::mem_fun(*this, &ThemeDialog::onThemeButtonOk) );

	Gtk::Button* m_p0ButtonCancel = add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	assert(m_p0ButtonCancel != nullptr);
		m_p0ButtonCancel->signal_clicked().connect(
						sigc::mem_fun(*this, &ThemeDialog::onThemeButtonCancel) );

	m_p0ButtonBoxActions = get_action_area();
	m_p0ButtonBoxActions->set_layout(Gtk::ButtonBoxStyle::BUTTONBOX_EXPAND);
	m_p0ButtonBoxActions->set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);
	m_p0ButtonBoxActions->set_spacing(10);
	m_p0ButtonBoxActions->set_margin_left(s_nButtonLeftRightMargin);
	m_p0ButtonBoxActions->set_margin_right(s_nButtonLeftRightMargin);
	m_p0ButtonBoxActions->set_margin_top(5);
	m_p0ButtonBoxActions->set_margin_bottom(5);

	Gtk::Box* m_p0BoxContent = get_content_area();
	assert(m_p0BoxContent != nullptr);
	m_p0BoxContent->set_orientation(Gtk::ORIENTATION_VERTICAL);

	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;

	m_p0StackThemeScreens = Gtk::manage(new Gtk::Stack());
	m_p0BoxContent->pack_start(*m_p0StackThemeScreens, true, true);
		m_p0StackThemeScreens->set_transition_type(Gtk::StackTransitionType::STACK_TRANSITION_TYPE_NONE);

	m_p0NotebookThemes = Gtk::manage(new Gtk::Notebook());
	m_p0StackThemeScreens->add(*m_p0NotebookThemes, s_sThemeScreenNameChoose);
		m_p0NotebookThemes->signal_switch_page().connect(
						sigc::mem_fun(*this, &ThemeDialog::onNotebookSwitchPage) );

	Gtk::Label* m_p0TabLabelThemes = Gtk::manage(new Gtk::Label("Themes"));
	Gtk::Box* m_p0TabVBoxThemesShow = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_p0NotebookThemes->append_page(*m_p0TabVBoxThemesShow, *m_p0TabLabelThemes);
		m_refTreeModelThemes = Gtk::TreeStore::create(m_oThemesColumns);
		m_p0TreeViewThemes = Gtk::manage(new ThemesTreeView(this, m_refTreeModelThemes));
		m_p0TabVBoxThemesShow->pack_start(*m_p0TreeViewThemes, true, true);
			m_p0TreeViewThemes->append_column("Name", m_oThemesColumns.m_oColNameStatus);
			m_p0TreeViewThemes->append_column("Thumb", m_oThemesColumns.m_oColThumbnail);
			refTreeSelection = m_p0TreeViewThemes->get_selection();
			refTreeSelection->signal_changed().connect(
							sigc::mem_fun(*this, &ThemeDialog::onThemeSelectionChanged));

	if (m_refStdConfig->isTestMode()) {
		Gtk::Label* m_p0TabLabelLoading = Gtk::manage(new Gtk::Label("Test"));
		Gtk::Box* m_p0TabVBoxLoading = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0NotebookThemes->append_page(*m_p0TabVBoxLoading, *m_p0TabLabelLoading);
			m_p0TextLoadingError = Gtk::manage(new Gtk::TextView());
			m_p0TabVBoxLoading->pack_start(*m_p0TextLoadingError, true, true);
				m_p0TextLoadingError->set_wrap_mode(Gtk::WrapMode::WRAP_WORD);
				m_refTextBufferLoadingError = m_p0TextLoadingError->get_buffer();
	}
	m_aThemeScreens[s_nScreenThemeChoose] = m_p0NotebookThemes;

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
								sigc::mem_fun(*this, &ThemeDialog::onButtonThemeInfoOk) );
			addBigSeparator(m_p0BoxInfo, true);
	m_aThemeScreens[s_nScreenThemeInfo] = m_p0ThemeScreenBoxInfo;

	show_all_children();
}
void ThemeDialog::onButtonThemeInfoOk() noexcept
{
	changeScreen(s_nScreenThemeChoose, "");
}
void ThemeDialog::changeScreen(int32_t nToScreen, const std::string& sMsg) noexcept
{
	if (m_nCurrentScreen == nToScreen) {
		return;
	}
	//const bool bIsTestMode = m_refStdConfig->isTestMode();

	m_nCurrentScreen = nToScreen;
	m_p0StackThemeScreens->set_visible_child(*m_aThemeScreens[m_nCurrentScreen]);
	if (m_nCurrentScreen == s_nScreenThemeChoose) {
		//
		m_p0ButtonBoxActions->set_visible(true);
	} else if (m_nCurrentScreen == s_nScreenThemeInfo) {
		m_p0LabelThemeInfoText->set_text(sMsg);
		m_p0ButtonBoxActions->set_visible(false);
	} else {
		assert(false);
	}
}

int ThemeDialog::run(const shared_ptr<AllPreferences>& refPrefs) noexcept
{
	assert(refPrefs);
	assert(m_refStdConfig == refPrefs->getStdConfig());

	m_refPrefs = refPrefs;

	const std::string sSaveThemeName = m_refPrefs->getThemeName();
	m_sSelectedThemeName = sSaveThemeName;

	regenerateThemesList();

	m_p0TreeViewThemes->grab_focus();

	changeScreen(s_nScreenThemeChoose, "");

	int nRet;
	while (true) {
		do {
			m_bReRun = false;
			nRet = Gtk::Dialog::run();
		} while (m_bReRun);
		if (nRet != Gtk::RESPONSE_OK) {
			// theme choice canceled
			m_refPrefs->setThemeName(sSaveThemeName);
			break; // while --------
		}
		const std::string sNewThemeName = m_refPrefs->getThemeName();
		if (sNewThemeName == sSaveThemeName) {
			break; // while --------
		}
		// try to create the theme instance
		auto refTheme = m_oThemeLoader.getTheme(sNewThemeName);
		if (!refTheme) {
			const std::string& sErrorString = m_oThemeLoader.getThemeInfo(sNewThemeName).m_sThemeErrorString;
			changeScreen(s_nScreenThemeInfo, "Couldn't load theme\n" + sErrorString);
			continue; // while --------
		}
		// store theme
		m_refTheme = refTheme;
		break;  // while --------
	}
	return nRet;
}

void ThemeDialog::regenerateThemesList() noexcept
{
//std::cout << "ThemeDialog::regenerateThemesList()" << '\n';
	m_bRegenerateThemesListInProgress = true;

	m_refTreeModelThemes->clear();

	const bool bIsTestMode = m_refStdConfig->isTestMode();
	int32_t nSelectedPosInList = -1;

	const auto& aNames = m_oThemeLoader.getThemeNames();
	int32_t nPosInList = 0;
	for (const auto& sThemeName : aNames) {
//std::cout << "ThemeDialog::regenerateThemesList()   sThemeName=" << sThemeName << '\n';
		const auto& oThemeInfo = m_oThemeLoader.getThemeInfo(sThemeName);
		if (oThemeInfo.m_bTesting && !bIsTestMode) {
			continue; // for(sThemeName) -------
		}
		const bool bLoadError = !oThemeInfo.m_sThemeErrorString.empty();
		if (m_sSelectedThemeName == sThemeName) {
			assert(nSelectedPosInList < 0);
			nSelectedPosInList = nPosInList;
		}
//std::cout << "ThemeDialog::regenerateThemesList()   bLoadError=" << bLoadError << '\n';
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
//std::cout << "ThemeDialog::regenerateThemesList()   sNameStatus=" << sNameStatus << '\n';
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
void ThemeDialog::regenerateThemeInfos() noexcept
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
void ThemeDialog::onThemeSelectionChanged() noexcept
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

void ThemeDialog::onNotebookSwitchPage(Gtk::Widget*, guint /*nPageNum*/) noexcept
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

void ThemeDialog::onThemeButtonOk() noexcept
{
	m_bReRun = true;
	if (m_sSelectedThemeName.empty()) {
		changeScreen(s_nScreenThemeInfo, "No theme selected");
		return; //--------------------------------------------------------------
	}
	m_refPrefs->setThemeName(m_sSelectedThemeName);
	m_bReRun = false;
}
void ThemeDialog::onThemeButtonCancel() noexcept
{
	m_bReRun = false;
}

} // namespace stmg
