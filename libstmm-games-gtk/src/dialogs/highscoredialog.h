/*
 * File:  highscoredialog.h
 *
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
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

#ifndef STMG_HIGHSCORE_DIALOG_H
#define STMG_HIGHSCORE_DIALOG_H

#include <gtkmm.h>

#include <memory>
#include <vector>

#include <stdint.h>

namespace stmg { class AllPreferences; }
namespace stmg { class Game; }
namespace stmg { class Highscore; }

namespace stmg
{

using std::shared_ptr;


class HighscoreDialog : public Gtk::Dialog
{
public:
	HighscoreDialog() noexcept;

	/** Runs the dialog adding the game scores to the highscores.
	 * @param refHighscore The highscores. Cannot be null.
	 * @param refGame The game. Cannot be null.
	 * @param refPrefs The preferences. Cannot be null.
	 * @return The response. See Gtk::Dialog::run().
	 */
	int run(const shared_ptr<Highscore>& refHighscore, const shared_ptr<Game>& refGame, const shared_ptr<AllPreferences>& refPrefs) noexcept;
	/** Runs the dialog showing the highscores.
	 * @param refHighscore The highscores. Cannot be null.
	 * @param refPrefs The preferences. Cannot be null.
	 * @return The response. See Gtk::Dialog::run().
	 */
	int run(const std::vector<shared_ptr<Highscore>>& aHighscores, const shared_ptr<AllPreferences>& refPrefs) noexcept;

private:
	int runInternal(const std::vector<shared_ptr<Highscore>>& aHighscores, const shared_ptr<Game>& refGame, const shared_ptr<AllPreferences>& refPrefs) noexcept;

	//Signal handlers:
	void onHighscoreButtonOk() noexcept;
	//virtual void onHighscoreButtonCancel();
	void onNotebookSwitchPage(Gtk::Widget*, guint nPageNum) noexcept;
	void onHighscoreSelectionChanged() noexcept;

	void regenerateColumns() noexcept;
	int32_t regenerateHighscoresList() noexcept; // returns the number of added scores

private:
	Gtk::Button* m_p0ButtonOk = nullptr;
	//Gtk::Button* m_p0ButtonCancel = nullptr;
	Gtk::Box* m_p0BoxContent = nullptr;
		Gtk::Notebook* m_p0NotebookHighscoreDesc = nullptr;

		Gtk::TreeView* m_p0TreeViewHighscores = nullptr;

	static constexpr int32_t nMaxScoreElements = 5;
	class HighscoresColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		HighscoresColumns() noexcept
		{
			add(m_oColRank);
			add(m_oColName);
			m_aScoreElems.resize(nMaxScoreElements);
			for (int32_t nIdx = 0; nIdx < nMaxScoreElements; ++nIdx) {
				add(m_aScoreElems[nIdx]);
			}
		}
		Gtk::TreeModelColumn<Glib::ustring> m_oColRank;
		Gtk::TreeModelColumn<Glib::ustring> m_oColName;
		std::vector< Gtk::TreeModelColumn<Glib::ustring> > m_aScoreElems;
	};
	HighscoresColumns m_oHighscoresColumns;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModelHighscores;

	bool m_bRegenerateHighscoresListInProgress;

	std::vector<shared_ptr<Highscore>> m_aHighscores;
	shared_ptr<Game> m_refGame; // null if just showing highscores
	shared_ptr<AllPreferences> m_refPrefs; // null if just showing highscores
	bool m_bEditableName;
	bool m_bGameIncluded;
	int32_t m_nCurrentPage;
};

} // namespace stmg

#endif	/* STMG_HIGHSCORE_DIALOG_H */

