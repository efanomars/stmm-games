/*
 * File:  themedialog.h
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

#ifndef STMG_THEME_DIALOG_H
#define STMG_THEME_DIALOG_H

#include <gtkmm.h>

#include <memory>
#include <string>
#include <cassert>

#include <stdint.h>

namespace stmg { class AllPreferences; }
namespace stmg { class StdConfig; }
namespace stmg { class ThemeLoader; }
namespace stmg { class Theme; }

namespace stmg
{

using std::shared_ptr;

class ThemeDialog  : public Gtk::Dialog
{
public:
	ThemeDialog(const shared_ptr<StdConfig>& refStdConfig, ThemeLoader& oThemeLoader, shared_ptr<Theme>& refTheme) noexcept;

	int run(const shared_ptr<AllPreferences>& refPrefs) noexcept;

private:
	//Signal handlers:
	void onThemeButtonOk() noexcept;
	void onThemeButtonCancel() noexcept;
	void onThemeSelectionChanged() noexcept;
	void onNotebookSwitchPage(Gtk::Widget*, guint nPageNum) noexcept;

	void changeScreen(int32_t nToScreen, const std::string& sMsg) noexcept;
	void onButtonThemeInfoOk() noexcept;

	void regenerateThemesList() noexcept;
	void regenerateThemeInfos() noexcept;

	//void msgWarningBox(const std::string& sText) noexcept;
private:
	class ThemesTreeView : public Gtk::TreeView
	{
	public:
		ThemesTreeView() = delete;
		ThemesTreeView(ThemeDialog* p0Dialog, const Glib::RefPtr< Gtk::TreeModel >& refModel) noexcept
		: Gtk::TreeView(refModel)
		, m_p0Dialog(p0Dialog)
		{
			set_enable_search(false);
			assert(p0Dialog != nullptr);
		}
	protected:
		bool on_key_press_event(GdkEventKey* p0Event) override
		{
			if (p0Event->keyval == GDK_KEY_Return) {
				m_p0Dialog->onThemeButtonOk();
				m_p0Dialog->response(Gtk::RESPONSE_OK);
				return false;
			} else if (p0Event->keyval == GDK_KEY_Escape) {
				m_p0Dialog->onThemeButtonCancel();
				m_p0Dialog->response(Gtk::RESPONSE_CANCEL);
				return false;
			}
			return Gtk::TreeView::on_key_press_event(p0Event);
		}
	private:
		ThemeDialog* m_p0Dialog;
	};
	friend class ThemesTreeView;

	int32_t m_nCurrentScreen;
	static constexpr const int32_t s_nScreenThemeChoose = 0;
	static constexpr const int32_t s_nScreenThemeInfo = 1;
	static constexpr const int32_t s_nTotThemeScreens = 2;

	Gtk::ButtonBox* m_p0ButtonBoxActions = nullptr;
	//Gtk::Button* m_p0ButtonOk = nullptr;
	//Gtk::Button* m_p0ButtonCancel = nullptr;
	//Gtk::Box* m_p0BoxContent = nullptr;

	Gtk::Stack* m_p0StackThemeScreens = nullptr;

	Gtk::Notebook* m_p0NotebookThemes = nullptr;

	static const int32_t s_nTabThemes = 0;
	//Gtk::Label* m_p0TabLabelThemes = nullptr;
	//Gtk::Box* m_p0TabVBoxThemesShow = nullptr;
		ThemesTreeView* m_p0TreeViewThemes = nullptr;

	static const int32_t s_nTabLoading = 1;
	//Gtk::Label* m_p0TabLabelLoading = nullptr;
	//Gtk::Box* m_p0TabVBoxLoading = nullptr;
		Gtk::TextView* m_p0TextLoadingError = nullptr;
			Glib::RefPtr<Gtk::TextBuffer> m_refTextBufferLoadingError;

	Gtk::Box* m_p0ThemeScreenBoxInfo = nullptr;
		//Gtk::Box* m_p0BoxInfo = nullptr;
			Gtk::Label* m_p0LabelThemeInfoText = nullptr;
			//Gtk::Button* m_p0ButtonThemeInfoOk = nullptr;

	Gtk::Widget* m_aThemeScreens[s_nTotThemeScreens]; // non owning pointers

	class ThemesColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
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
	shared_ptr<Theme>& m_refTheme;
	shared_ptr<AllPreferences> m_refPrefs;

	bool m_bRegenerateThemesListInProgress;

	std::string m_sSelectedThemeName;
	bool m_bReRun;
};

} // namespace stmg

#endif	/* STMG_THEME_DIALOG_H */
