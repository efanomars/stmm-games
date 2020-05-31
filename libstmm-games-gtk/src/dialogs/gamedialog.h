/*
 * File:  gamedialog.h
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

#ifndef STMG_GAME_DIALOG_H
#define STMG_GAME_DIALOG_H

#include <gtkmm.h>

#include <memory>
#include <cassert>
#include <string>

#include <stdint.h>

namespace stmg { class AllPreferences; }
namespace stmg { class GameLoader; }
namespace stmg { class GameOwner; }
namespace stmg { class Theme; }
namespace stmg { class StdConfig; }

namespace stmg
{

using std::shared_ptr;

class GameDialog : public Gtk::Dialog
{
public:
	GameDialog(const shared_ptr<StdConfig>& refStdConfig, GameLoader& oGameLoader, GameOwner& oGameOwner) noexcept;

	/* Initialized an instance and runs the dialog.
	 * @param refPrefs The preferences. Cannot be null.
	 * @param refTheme The theme Named stuff when creating game.
	 * @return Same as Gtk::Dialog::run().
	 */
	int run(const shared_ptr<AllPreferences>& refPrefs, const shared_ptr<Theme>& refTheme) noexcept;

private:
	//Signal handlers:
	void onButtonOk() noexcept;
	void onButtonCancel() noexcept;
	void onGameSelectionChanged() noexcept;
	void onNotebookSwitchPage(Gtk::Widget*, guint nPageNum) noexcept;
	void onShowAllGamesChanged() noexcept;
	void onShowPlayedHistoryChanged() noexcept;

	void changeScreen(int32_t nToScreen, const std::string& sMsg) noexcept;
	void onButtonGameInfoOk() noexcept;

	void regenerateGamesList() noexcept;
	void regenerateGameInfos() noexcept;

	//void msgWarningBox(const std::string& sText) noexcept;
private:
	class GamesTreeView : public Gtk::TreeView
	{
	public:
		GamesTreeView() = delete;
		GamesTreeView(GameDialog* p0Dialog, const Glib::RefPtr< Gtk::TreeModel >& refModel) noexcept
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
				m_p0Dialog->onButtonOk();
				m_p0Dialog->response(Gtk::RESPONSE_OK);
				return false;
			} else if (p0Event->keyval == GDK_KEY_Escape) {
				m_p0Dialog->onButtonCancel();
				m_p0Dialog->response(Gtk::RESPONSE_CANCEL);
				return false;
			}
			return Gtk::TreeView::on_key_press_event(p0Event);
		}
	private:
		GameDialog* m_p0Dialog;
	};
	friend class GamesTreeView;

	int32_t m_nCurrentScreen;
	static constexpr const int32_t s_nScreenGameChoose = 0;
	static constexpr const int32_t s_nScreenGameInfo = 1;
	static constexpr const int32_t s_nTotGameScreens = 2;

	Gtk::ButtonBox* m_p0ButtonBoxActions = nullptr;
	//Gtk::Button* m_p0ButtonOk = nullptr;
	//Gtk::Button* m_p0ButtonCancel = nullptr;
	//Gtk::Box* m_p0BoxContent = nullptr;

	Gtk::Stack* m_p0StackGameScreens = nullptr;

	Gtk::Notebook* m_p0NotebookGames = nullptr;

	int32_t m_aPageIndex[3];

	static const int32_t s_nTabGames = 0;
	//Gtk::Label* m_p0TabLabelGames = nullptr;
	//Gtk::Box* m_p0TabVBoxGamesShow = nullptr;
		//Gtk::ScrolledWindow* m_p0ScrolledGames = nullptr;
			GamesTreeView* m_p0TreeViewGames = nullptr;
		Gtk::CheckButton* m_p0CheckShowAllGames = nullptr;
		Gtk::CheckButton* m_p0CheckShowPlayedHistory = nullptr;

	static const int32_t s_nTabDescription = 1;
	//Gtk::Label* m_p0TabLabelDescription = nullptr;
	//Gtk::Box* m_p0TabVBoxDescription = nullptr;
		Gtk::Image* m_p0ImageGameImage = nullptr;
		Gtk::Label* m_p0LabelGameName = nullptr;
		//Gtk::ScrolledWindow* m_p0ScrolledDescription = nullptr;
			//Gtk::Box* m_p0VBoxScroller = nullptr;
				Gtk::TextView* m_p0TextDescription = nullptr;
					Glib::RefPtr<Gtk::TextBuffer> m_refTextBufferDescription;
				//Gtk::Label* m_p0LabelAuthors = nullptr;
				Gtk::TextView* m_p0TextAuthors = nullptr;
					Glib::RefPtr<Gtk::TextBuffer> m_refTextBufferAuthors;
				//Gtk::Label* m_p0LabelConstraints = nullptr;
				Gtk::TextView* m_p0TextConstraints = nullptr;
					Glib::RefPtr<Gtk::TextBuffer> m_refTextBufferConstraints;

	static const int32_t s_nTabLoading = 2;
	//Gtk::Label* m_p0TabLabelLoading = nullptr;
	//Gtk::Box* m_p0TabVBoxLoading = nullptr;
		Gtk::TextView* m_p0TextLoadingError = nullptr;
			Glib::RefPtr<Gtk::TextBuffer> m_refTextBufferLoadingError;

	Gtk::Box* m_p0GameScreenBoxInfo = nullptr;
		//Gtk::Box* m_p0BoxInfo = nullptr;
			Gtk::Label* m_p0LabelGameInfoText = nullptr;
			//Gtk::Button* m_p0ButtonGameInfoOk = nullptr;

	Gtk::Widget* m_aGameScreens[s_nTotGameScreens]; // non owning pointers

	class GamesColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		GamesColumns() noexcept	{ add(m_oColNameStatus); add(m_oColThumbnail); add(m_oColHiddenName); }
		Gtk::TreeModelColumn<Glib::ustring> m_oColNameStatus;
		Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> m_oColThumbnail;
		Gtk::TreeModelColumn<Glib::ustring> m_oColHiddenName;
	};
	GamesColumns m_oGamesColumns;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModelGames;

	const shared_ptr<StdConfig> m_refStdConfig;
	GameLoader& m_oGameLoader;
	GameOwner& m_oGameOwner;

	shared_ptr<AllPreferences> m_refPrefs;
	shared_ptr<Theme> m_refTheme;

	bool m_bRegenerateGamesListInProgress;

	std::string m_sSelectedGameName;
	bool m_bShowAllGames;
	bool m_bShowPlayedHistory;
	bool m_bReRun;

	//bool m_bShowSelectAll;

};

} // namespace stmg

#endif	/* STMG_GAME_DIALOG_H */

