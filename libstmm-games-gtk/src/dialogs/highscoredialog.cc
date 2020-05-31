/*
 * File:   highscoredialog.cc
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

#include "highscoredialog.h"
#include "allpreferences.h"

#include <stmm-games/game.h>

#include <stmm-games/highscore.h>
#include <stmm-games/highscoresdefinition.h>

#include <gtkmm.h>

//#include <iostream>
#include <cassert>
#include <string>
#include <utility>
#include <algorithm>

namespace stmg
{

static const std::string s_sHighScoreWindowTitle = "Highscores";
static const std::string s_sHighScoreColumnTitleRank = "Rank";
static const std::string s_sHighScoreColumnTitleName = "Name";

static constexpr const int32_t s_nButtonLeftRightMargin = 20;

HighscoreDialog::HighscoreDialog() noexcept
: Gtk::Dialog(s_sHighScoreWindowTitle, true)
, m_p0ButtonOk(nullptr)
, m_p0BoxContent(nullptr)
, m_p0NotebookHighscoreDesc(nullptr)
, m_p0TreeViewHighscores(nullptr)
, m_bRegenerateHighscoresListInProgress(false)
, m_bEditableName(false)
, m_bGameIncluded(false)
, m_nCurrentPage(0)
{
	//set_title("Highscores");
	set_default_size(150, 250);

	////////////////////////////////////////////////////////////////////////////
	m_p0ButtonOk = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	assert(m_p0ButtonOk != nullptr);
		m_p0ButtonOk->signal_clicked().connect(
						sigc::mem_fun(*this, &HighscoreDialog::onHighscoreButtonOk) );

	Gtk::ButtonBox* m_p0ButtonBoxActions = get_action_area();
	m_p0ButtonBoxActions->set_layout(Gtk::ButtonBoxStyle::BUTTONBOX_EXPAND);
	m_p0ButtonBoxActions->set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);
	m_p0ButtonBoxActions->set_margin_left(s_nButtonLeftRightMargin);
	m_p0ButtonBoxActions->set_margin_right(s_nButtonLeftRightMargin);

	m_p0BoxContent = get_content_area();
	assert(m_p0BoxContent != nullptr);
	m_p0BoxContent->set_orientation(Gtk::ORIENTATION_VERTICAL);

	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;

		m_p0NotebookHighscoreDesc = Gtk::manage(new Gtk::Notebook());
		m_p0BoxContent->pack_start(*m_p0NotebookHighscoreDesc, false, true);
			m_p0NotebookHighscoreDesc->signal_switch_page().connect(
											sigc::mem_fun(*this, &HighscoreDialog::onNotebookSwitchPage) );

				//p0TabLabelDesc = Gtk::manage(new Gtk::Label(""));
				//p0TabLabelInvisible = Gtk::manage(new Gtk::Label(""));

		m_refTreeModelHighscores = Gtk::TreeStore::create(m_oHighscoresColumns);
		m_p0TreeViewHighscores = Gtk::manage(new Gtk::TreeView(m_refTreeModelHighscores));
		m_p0BoxContent->pack_start(*m_p0TreeViewHighscores, true, true);
			m_p0TreeViewHighscores->append_column(s_sHighScoreColumnTitleRank, m_oHighscoresColumns.m_oColRank);
			auto p0CellRenderer = m_p0TreeViewHighscores->get_column_cell_renderer(0);
			if (p0CellRenderer != nullptr) {
				p0CellRenderer->set_alignment(0.5, 0.5); // it's a number or time
			}
			refTreeSelection = m_p0TreeViewHighscores->get_selection();
			refTreeSelection->signal_changed().connect(
							sigc::mem_fun(*this, &HighscoreDialog::onHighscoreSelectionChanged));

	show_all_children();
}
int HighscoreDialog::run(const shared_ptr<Highscore>& refHighscore, const shared_ptr<Game>& refGame, const shared_ptr<AllPreferences>& refPrefs) noexcept
{
	assert(refGame);
	return runInternal({refHighscore}, refGame, refPrefs);
}
int HighscoreDialog::run(const std::vector<shared_ptr<Highscore>>& aHighscores, const shared_ptr<AllPreferences>& refPrefs) noexcept
{
	static const shared_ptr<Game> s_refEmptyGame{};
	return runInternal(aHighscores, s_refEmptyGame, refPrefs);
}
int HighscoreDialog::runInternal(const std::vector<shared_ptr<Highscore>>& aHighscores
								, const shared_ptr<Game>& refGame, const shared_ptr<AllPreferences>& refPrefs) noexcept
{
	assert(! aHighscores.empty());
	assert(refPrefs);

	m_aHighscores = aHighscores;
	m_refGame = refGame;
	m_bGameIncluded = false;
	m_refPrefs = refPrefs;
	assert((!m_refGame) || ((&(m_refGame->prefs())) == refPrefs.operator->()));

	m_nCurrentPage = -1;
	while (m_p0NotebookHighscoreDesc->get_n_pages() > 0) {
		m_p0NotebookHighscoreDesc->remove_page(0);
	}
	shared_ptr<HighscoresDefinition> refHighscoreDef;
	std::string sSelectTitle;
	int32_t nSelectPage = -1;
	for (auto& refHighscore : m_aHighscores) {
		assert(refHighscore);
		if (!refHighscoreDef) {
			refHighscoreDef = refHighscore->getHighscoresDefinition();
			const auto oPair = refHighscoreDef->getTitleFromPreferences(*refPrefs);
			if (oPair.first) {
				sSelectTitle = oPair.second;
			}
		} else {
			assert(refHighscoreDef == refHighscore->getHighscoresDefinition());
		}
		const std::string sCurTitle = refHighscore->getTitleString();
		auto p0TabLabelTitle = Gtk::manage(new Gtk::Label(sCurTitle));
		auto p0TabLabelInvisible = Gtk::manage(new Gtk::Label(""));
		//p0TabLabelInvisible->set_visible(false);
		const auto nCurPage = m_p0NotebookHighscoreDesc->append_page(*p0TabLabelInvisible, *p0TabLabelTitle);
		if (sCurTitle == sSelectTitle) {
			assert(nSelectPage < 0);
			nSelectPage = nCurPage;
		}
	}
	if (nSelectPage < 0) {
		nSelectPage = 0;
	}
	m_p0NotebookHighscoreDesc->set_current_page(nSelectPage);
	m_nCurrentPage = nSelectPage;

	regenerateColumns();
	const int32_t nTotAdded = regenerateHighscoresList();
	if ((m_refGame.get() != nullptr) && (nTotAdded == 0)) {
		return Gtk::RESPONSE_CANCEL;
	}
	return Gtk::Dialog::run();
}
void HighscoreDialog::onNotebookSwitchPage(Gtk::Widget*, guint nPageNum) noexcept
{
	if (m_nCurrentPage < 0) {
		// initializing
		return;
	}
	if (static_cast<int32_t>(nPageNum) == m_nCurrentPage) {
		return;
	}
	assert(nPageNum < m_aHighscores.size());
	m_nCurrentPage = nPageNum;
	regenerateHighscoresList();
}
void HighscoreDialog::regenerateColumns() noexcept
{
	assert(!m_aHighscores.empty());
	//TODO just the newly added highscore row should be editable
	//TODO not the whole column
	//TODO maybe there is a way to set the focus to the row for modification
	//TODO and as soon as enter is pressed the whole row is made read only?
	m_bEditableName = false; // (m_refGame && (m_refPrefs->getTotHumanTeams() == 1));
	// Remove all columns except rank
	const int32_t nTotColumns = m_p0TreeViewHighscores->get_n_columns();
	const int32_t nToDeleteColumns = nTotColumns - 1;
	for (int32_t nCount = 0; nCount < nToDeleteColumns; ++nCount) {
		auto p0TreeViewColumn = m_p0TreeViewHighscores->get_column(1);
		assert(p0TreeViewColumn != nullptr);
		m_p0TreeViewHighscores->remove_column(*p0TreeViewColumn);
	}
	assert(m_p0TreeViewHighscores->get_n_columns() == 1);
	// Add name column
	if (m_bEditableName) {
		m_p0TreeViewHighscores->append_column_editable(s_sHighScoreColumnTitleName, m_oHighscoresColumns.m_oColName);
	} else {
		m_p0TreeViewHighscores->append_column(s_sHighScoreColumnTitleName, m_oHighscoresColumns.m_oColName);
	}
	//Gtk::TreeView::Column* p0NameColumn = m_p0TreeViewHighscores->get_column(1);
	//if (p0NameColumn != nullptr) {
	//	p0NameColumn->set_resizable(true);
	//	p0NameColumn->set_min_width(16);
	//}
	// Add the visible score columns
	auto& refHighscoresDefinition = m_aHighscores[0]->getHighscoresDefinition();
	const auto& aScoreElements = refHighscoresDefinition->getScoreElements();
	const int32_t nTotScoreElements = static_cast<int32_t>(aScoreElements.size());
	int32_t nShown = 0;
	for (int32_t nIdx = 0; nIdx < nTotScoreElements; ++nIdx) {
		const std::string& sVarDesc = aScoreElements[nIdx].m_sVarDescription;
		if (sVarDesc.empty()) {
			continue; // for(nIdx) ----
		}
		if (nShown >= nMaxScoreElements) {
			continue; // for(nIdx) ----
		}
		m_p0TreeViewHighscores->append_column(sVarDesc, m_oHighscoresColumns.m_aScoreElems[nShown]);
		const auto nTotColumns = m_p0TreeViewHighscores->get_n_columns();
		auto p0CellRenderer = m_p0TreeViewHighscores->get_column_cell_renderer(nTotColumns - 1);
		if (p0CellRenderer != nullptr) {
			p0CellRenderer->set_alignment(1.0, 0.5); // it's a number or time
		}
		// the first score element is the most significant
		const bool bExpandCol = true; //(nIdx == 0);
		Gtk::TreeView::Column* p0ScoreColumn = m_p0TreeViewHighscores->get_column(nTotColumns - 1);
		if (p0ScoreColumn != nullptr) {
			p0ScoreColumn->set_expand(bExpandCol);
			//p0ScoreColumn->set_resizable(true);
			//p0ScoreColumn->set_min_width(10);
		}
		++nShown;
	}
}
int32_t HighscoreDialog::regenerateHighscoresList() noexcept
{
//std::cout << "HighscoreDialog::regenerateHighscoresList()" << '\n';
	assert(!m_aHighscores.empty());

	std::vector<int32_t> aNewlyAddedPositions;
	std::vector<Gtk::TreeModel::Path> aPaths;
	if (m_refGame && !m_bGameIncluded) {
		aNewlyAddedPositions = m_aHighscores[m_nCurrentPage]->includeGame(*m_refGame);
		m_bGameIncluded = true;
	}
	m_bRegenerateHighscoresListInProgress = true;

	m_refTreeModelHighscores->clear();

	auto& refHighscoresDefinition = m_aHighscores[0]->getHighscoresDefinition();
	auto& aScoreElements = refHighscoresDefinition->getScoreElements();
	const int32_t nTotScoreEls = static_cast<int32_t>(aScoreElements.size());

	shared_ptr<Highscore>& refHighscore = m_aHighscores[m_nCurrentPage];
	const int32_t nTotScores = refHighscore->getTotScores();
	for (int32_t nPosition = 0; nPosition < nTotScores; ++nPosition) {
		const auto itFound = std::find(aNewlyAddedPositions.begin(), aNewlyAddedPositions.end(), nPosition);
		const bool bSelected = (itFound != aNewlyAddedPositions.end());
		if (bSelected) {
			aPaths.emplace_back();
			aPaths.back().push_back(nPosition);
		}
		const Highscore::Score& oScore = refHighscore->getScore(nPosition);
		Gtk::TreeModel::Row oRow = *(m_refTreeModelHighscores->append());
		oRow[m_oHighscoresColumns.m_oColRank] = (bSelected ? "*" : "") + std::to_string(nPosition + 1) + (bSelected ? "*" : "");
		oRow[m_oHighscoresColumns.m_oColName] = oScore.m_sTeam;
		assert(aScoreElements.size() == oScore.m_aValues.size());
		for (int32_t nScoreEleIdx = 0; nScoreEleIdx < nTotScoreEls; ++nScoreEleIdx) {
			oRow[m_oHighscoresColumns.m_aScoreElems[nScoreEleIdx]] = oScore.m_aValues[nScoreEleIdx].toFormattedString();
		}
	}
	m_p0TreeViewHighscores->expand_all();
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_p0TreeViewHighscores->get_selection();
	refTreeSelection->set_mode(Gtk::SELECTION_MULTIPLE);
	refTreeSelection->unselect_all();
	for (const auto& oPath : aPaths) {
		refTreeSelection->select(oPath);
	}

	if (aPaths.empty()) {
		m_p0ButtonOk->grab_focus();
	}

	m_bRegenerateHighscoresListInProgress = false;
	return static_cast<int32_t>(aNewlyAddedPositions.size());
}
void HighscoreDialog::onHighscoreSelectionChanged() noexcept
{
	//if (m_bRegenerateHighscoresListInProgress) {
	//	return;
	//}
	//if (! m_refPrefs) {
	//	return;
	//}
}
void HighscoreDialog::onHighscoreButtonOk() noexcept
{
}

} // namespace stmg
