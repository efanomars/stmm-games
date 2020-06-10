/*
 * File:  themescreen.h
 *
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

#ifndef STMG_THEME_SCREEN_H
#define STMG_THEME_SCREEN_H

#include <gtkmm.h>

#include <memory>
#include <string>

#include <stdint.h>

namespace stmg { class GameWindow; }
namespace stmg { class AllPreferences; }
namespace stmg { class StdConfig; }
namespace stmg { class ThemeLoader; }
namespace stmg { class Theme; }

namespace stmg
{

using std::shared_ptr;

class ThemeScreen
{
public:
	ThemeScreen(GameWindow& oGameWindow, const shared_ptr<StdConfig>& refStdConfig
				, ThemeLoader& oThemeLoader) noexcept;

	// returns widget to add to container
	Gtk::Widget* init() noexcept;

	// returns whether could change to screen
	bool changeTo(const shared_ptr<AllPreferences>& refPrefs) noexcept;

private:
	//Signal handlers:
	void onButtonOk() noexcept;
	void onButtonCancel() noexcept;
	void onThemeSelectionChanged() noexcept;
	void onNotebookSwitchPage(Gtk::Widget*, guint nPageNum) noexcept;

	void changeScreen(int32_t nToScreen, const std::string& sMsg) noexcept;
	void onButtonThemeInfoOk() noexcept;

	void regenerateThemesList() noexcept;
	void regenerateThemeInfos() noexcept;

private:
	GameWindow& m_oGameWindow;

	int32_t m_nCurrentScreen;
	static constexpr const int32_t s_nScreenThemeChoose = 0;
	static constexpr const int32_t s_nScreenThemeInfo = 1;
	static constexpr const int32_t s_nTotThemeScreens = 2;

	class ThemesTreeView : public Gtk::TreeView
	{
	public:
		ThemesTreeView() = delete;
		ThemesTreeView(ThemeScreen* p0Dialog, const Glib::RefPtr< Gtk::TreeModel >& refModel) noexcept;
	protected:
		bool on_key_press_event(GdkEventKey* p0Event) override;
	private:
		ThemeScreen* m_p0Dialog;
	};
	friend class ThemesTreeView;

	Gtk::Box* m_p0ThemeScreenBoxMain = nullptr;
		//Gtk::Label* m_p0LabelTitle = nullptr;
		Gtk::Stack* m_p0StackThemeScreens = nullptr;

			//Gtk::Box* m_p0ThemeScreenBoxThemes = nullptr;
				Gtk::Notebook* m_p0NotebookThemes = nullptr;

					static const int32_t s_nTabThemes = 0;
					//Gtk::Label* m_p0TabLabelThemes = nullptr;
					//Gtk::Box* m_p0TabVBoxThemesShow = nullptr;
						//Gtk::ScrolledWindow* m_p0ScrolledThemes = nullptr;
							ThemesTreeView* m_p0TreeViewThemes = nullptr;

					static const int32_t s_nTabLoading = 1;
					//Gtk::Label* m_p0TabLabelLoading = nullptr;
					//Gtk::Box* m_p0TabVBoxLoading = nullptr;
						Gtk::TextView* m_p0TextLoadingError = nullptr;
							Glib::RefPtr<Gtk::TextBuffer> m_refTextBufferLoadingError;

				//Gtk::Button* m_p0ButtonOk = nullptr;
				//Gtk::Button* m_p0ButtonCancel = nullptr;

			Gtk::Box* m_p0ThemeScreenBoxInfo = nullptr;
				//Gtk::Box* m_p0BoxInfo = nullptr;
					Gtk::Label* m_p0LabelThemeInfoText = nullptr;
					//Gtk::Button* m_p0ButtonThemeInfoOk = nullptr;

	Gtk::Widget* m_aThemeScreens[s_nTotThemeScreens]; // non owning pointers

	class ThemesColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		ThemesColumns() noexcept { add(m_oColNameStatus); add(m_oColThumbnail); add(m_oColHiddenName); }
		Gtk::TreeModelColumn<Glib::ustring> m_oColNameStatus;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> m_oColThumbnail;
		Gtk::TreeModelColumn<Glib::ustring> m_oColHiddenName;
	};
	ThemesColumns m_oThemesColumns;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModelThemes;

	const shared_ptr<StdConfig> m_refStdConfig;
	ThemeLoader& m_oThemeLoader;
	shared_ptr<AllPreferences> m_refPrefs;

	bool m_bRegenerateThemesListInProgress;

	std::string m_sOldThemeName;
	std::string m_sSelectedThemeName;
};

} // namespace stmg

#endif	/* STMG_THEME_SCREEN_H */
