/*
 * File:   playersscreen.cc
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

#include "playersscreen.h"

#include "inputstrings.h"
#include "theme.h"
#include "themecontext.h"
#include "../gamewindow.h"

#include <stmm-games-file/allpreferences.h>

#include <stmm-games/ownertype.h>
#include <stmm-games/option.h>
#include <stmm-games/named.h>
#include <stmm-games/appconstraints.h>
#include <stmm-games/options/booloption.h>
#include <stmm-games/options/enumoption.h>
#include <stmm-games/options/intoption.h>
#include <stmm-games/stdconfig.h>
#include <stmm-games/util/namedobjindex.h>
#include <stmm-games/util/variant.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/namedindex.h>

#include <stmm-input-au/playbackcapability.h>

#include <stmm-input/capability.h>
#include <stmm-input/devicemanager.h>
#include <stmm-input/device.h>
#include <stmm-input/event.h>

#include <gtkmm.h>

#include <cassert>
//#include <iostream>
#include <numeric>
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <utility>


namespace stmg
{

static const std::string s_sPlayerOrTeamRenameEllipsize = "Rename ...";
static const std::string s_sPlayerOrTeamRenameDoIt = "Confirm";
static const std::string s_sTestSoundName = "SystemTest";

static constexpr const int32_t s_nButtonLeftRightMargin = 20;

PlayersScreen::KeysNotebook::KeysNotebook(PlayersScreen* p0Dialog) noexcept
: Gtk::Notebook()
, m_p0Dialog(p0Dialog)
{
	assert(p0Dialog != nullptr);
}
bool PlayersScreen::KeysNotebook::on_button_press_event(GdkEventButton* p0Event)
{
	if (m_p0Dialog->m_bExpectingKey) {
		// allow button press to get to the device manager as a key
		return false;
	}
	return Gtk::Notebook::on_button_press_event(p0Event);
}

////////////////////////////////////////////////////////////////////////////////
PlayersScreen::KeysTreeView::KeysTreeView(PlayersScreen* p0Dialog, const Glib::RefPtr< Gtk::TreeModel >& refModel) noexcept
: Gtk::TreeView(refModel)
, m_p0Dialog(p0Dialog)
{
	set_enable_search(false);
	assert(p0Dialog != nullptr);
	const Gdk::EventMask oCurMask = get_events();
	const Gdk::EventMask oNewMask = oCurMask | Gdk::FOCUS_CHANGE_MASK;
	if (oNewMask != oCurMask) {
		set_events(oNewMask);
	}
}
bool PlayersScreen::KeysTreeView::on_key_press_event(GdkEventKey* p0Event)
{
	if (m_p0Dialog->m_bExpectingKey) {
		return false;
	}
	if (p0Event->keyval == s_nModifyKeyVal1) {
		return false;
	}
	if ((s_nModifyKeyVal2 != 0) && (p0Event->keyval == s_nModifyKeyVal2)) {
		return false;
	}
	return Gtk::TreeView::on_key_press_event(p0Event);
}
bool PlayersScreen::KeysTreeView::on_button_press_event(GdkEventButton* p0Event)
{
	if (m_p0Dialog->m_bExpectingKey) {
		// allow button press to get to the device manager as a key
		return false;
	}
	return Gtk::TreeView::on_button_press_event(p0Event);
}
bool PlayersScreen::KeysTreeView::on_focus_in_event(GdkEventFocus* p0Event)
{
//std::cout << "KeysTreeView::on_focus_in_event()" << '\n';
	// start listening to keys
	m_p0Dialog->m_bListenToKey = true;
	return Gtk::TreeView::on_focus_in_event(p0Event);
}
bool PlayersScreen::KeysTreeView::on_focus_out_event(GdkEventFocus* p0Event)
{
//std::cout << "KeysTreeView::on_focus_out_event()" << '\n';
	// stop listening to keys
	m_p0Dialog->m_bListenToKey = false;
	m_p0Dialog->m_bExpectingKey = false;
	m_p0Dialog->m_p0LabelKeysModifyKey->set_label(m_p0Dialog->m_sLabelKeysModifyKey);
	return Gtk::TreeView::on_focus_out_event(p0Event);
}

////////////////////////////////////////////////////////////////////////////////
PlayersScreen::PlayersScreen(GameWindow& oGameWindow, const shared_ptr<StdConfig>& refStdConfig) noexcept
: m_oGameWindow(oGameWindow)
, m_sLabelKeysModifyKey(Glib::ustring::compose("(press '%1' to modify the selected action)", ::gdk_keyval_name(s_nModifyKeyVal1)))
, m_refStdConfig(refStdConfig)
, m_oCapabilityAssignment(m_refStdConfig->getCapabilityAssignment())
, m_bPrefsInitialized(false)
, m_bShowGameTab(false)
, m_bShowTeamTab(false)
, m_bShowTeamTreeNode(false)
, m_bShowPlayerTab(false)
, m_bRegeneratePlayersInProgress(false)
, m_bRegenerateOptionsInProgress(false)
, m_bRegenerateAssignedDevicesInProgress(false)
, m_bRegenerateSoundDevicesInProgress(false)
, m_bEditingTeamOrPlayer(false)
, m_nSoundTestIdx(-1)
, m_nSelectedTeam(-1)
, m_nSelectedMate(-1)
, m_nSelectedKeyAction(-1)
, m_bListenToKey(false)
, m_bExpectingKey(false)
{
	assert(refStdConfig);
}
Gtk::Widget* PlayersScreen::init() noexcept
{
	m_bShowGameTab = (m_refStdConfig->getTotVisibleOptions(OwnerType::GAME) > 0) || m_refStdConfig->soundEnabled();

	const auto& oAppConstraints = m_refStdConfig->getAppConstraints();
	m_bAlwaysOnePlayer = (oAppConstraints.getMaxPlayers() == 1);

	const bool bOneTeammatePerTeam = (oAppConstraints.getMaxTeammates() == 1);
	// When both more than one team and more than one mate per team are true, the team name is needed
	// - max one team: the team gets the name of the first mate
	// - max one mate per team: the team gets the name of the mate
	m_bShowTeamTreeNode = ! ((oAppConstraints.getMaxTeams() == 1) || bOneTeammatePerTeam);
	// If there are no team options the team tab is not shown
	m_bShowTeamTab = (m_refStdConfig->getTotVisibleOptions(OwnerType::TEAM) > 0);

	m_bShowPlayerOptions = (m_refStdConfig->getTotVisibleOptions(OwnerType::PLAYER) > 0);
	const bool bPerPlayerSounds = m_refStdConfig->soundEnabled() && m_refStdConfig->canPlayPerPlayerSounds();
	m_bShowPlayerTab = m_bShowPlayerOptions || bPerPlayerSounds;

	m_bShowKeyActions = (m_refStdConfig->getTotKeyActions() > 0);
	m_bShowAssignedCapabilities = (m_oCapabilityAssignment.m_nMaxCapabilitiesExplicitlyAssignedToPlayer > 0);

//std::cout << "PlayersScreen::PlayersScreen()" << '\n';
//std::cout << "    m_bShowTeamTreeNode        =" << m_bShowTeamTreeNode << '\n';
//std::cout << "    m_bShowTeam                =" << m_bShowTeam << '\n';
//std::cout << "    m_bShowPlayerOptions       =" << m_bShowPlayerOptions << '\n';
//std::cout << "    m_bShowKeyActions          =" << m_bShowKeyActions << '\n';
//std::cout << "    m_bShowAssignedCapabilities=" << m_bShowAssignedCapabilities << '\n';
//std::cout << "    m_bAlwaysOnePlayer         =" << m_bAlwaysOnePlayer << '\n';
//std::cout << "    bPerPlayerSounds           = " << bPerPlayerSounds << '\n';
	std::iota(std::begin(m_aPageIndex), std::end(m_aPageIndex), 0);

	static_assert(s_nTabTeamPlayers < s_nTabGameOptions, "");
	static_assert(s_nTabGameOptions < s_nTabTeamOptions, "");
	static_assert(s_nTabTeamOptions < s_nTabPlayerOptions, "");
	static_assert(s_nTabPlayerOptions < s_nTabPlayerDeviceKeyActions, "");
	constexpr int32_t nTotTabs = sizeof(m_aPageIndex) / sizeof(m_aPageIndex[0]);
	static_assert(nTotTabs == 5, "");

	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;

	m_refAdjustmentNrPlayers = Gtk::Adjustment::create(1, 1, 77, 1, 1, 0);
	m_refAdjustmentNrTeams = Gtk::Adjustment::create(1, 1, 77, 1, 1, 0);

	m_p0PlayersScreenBoxMain = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

	//Gtk::Label* m_p0LabelTitle = Gtk::manage(new Gtk::Label(m_bAlwaysOnePlayer ?  "---- Player ----" : "---- Choose players ----"));
	//m_p0PlayersScreenBoxMain->pack_start(*m_p0LabelTitle, false, false);
	//	m_p0LabelTitle->set_margin_top(3);
	//	m_p0LabelTitle->set_margin_bottom(3);
	//	{
	//	Pango::AttrList oAttrList;
	//	Pango::AttrInt oAttrWeight = Pango::Attribute::create_attr_weight(Pango::WEIGHT_HEAVY);
	//	oAttrList.insert(oAttrWeight);
	//	m_p0LabelTitle->set_attributes(oAttrList);
	//	}

	m_p0NotebookPlayers = Gtk::manage(new KeysNotebook(this));
	m_p0PlayersScreenBoxMain->pack_start(*m_p0NotebookPlayers, true, true, 2);
		m_p0NotebookPlayers->set_scrollable(true);
		m_p0NotebookPlayers->signal_switch_page().connect(
						sigc::mem_fun(*this, &PlayersScreen::onNotebookSwitchPage) );

	Gtk::Label* m_p0TabLabelPlayersTeams = Gtk::manage(new Gtk::Label(m_bAlwaysOnePlayer ? "Player" : "Players"));
	Gtk::Box* m_p0TabVBoxPlayersTeams = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	m_aPageIndex[s_nTabTeamPlayers] = m_p0NotebookPlayers->append_page(*m_p0TabVBoxPlayersTeams, *m_p0TabLabelPlayersTeams);

		//addSeparator(m_p0TabVBoxPlayersTeams, 2);
		m_p0HBoxNrPlayersTeams = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
		m_p0TabVBoxPlayersTeams->pack_start(*m_p0HBoxNrPlayersTeams, false, false);
			m_p0HBoxNrPlayersTeams->set_margin_top(3);
			Gtk::Box* m_p0HBoxNrPlayers = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
			m_p0HBoxNrPlayersTeams->pack_start(*m_p0HBoxNrPlayers);
				Gtk::Label* m_p0LabelNrPlayers = Gtk::manage(new Gtk::Label("Players:"));
				m_p0HBoxNrPlayers->pack_start(*m_p0LabelNrPlayers);
				m_p0SpinNrPlayers = Gtk::manage(new Gtk::SpinButton(m_refAdjustmentNrPlayers));
				m_p0HBoxNrPlayers->pack_start(*m_p0SpinNrPlayers);
					m_p0SpinNrPlayers->signal_value_changed().connect(
									sigc::mem_fun(*this, &PlayersScreen::onSpinNrPlayersChanged) );
			Gtk::Box* m_p0HBoxNrTeams = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
			m_p0HBoxNrPlayersTeams->pack_start(*m_p0HBoxNrTeams);
				Gtk::Label* m_p0LabelNrTeams = Gtk::manage(new Gtk::Label("Teams:"));
				m_p0HBoxNrTeams->pack_start(*m_p0LabelNrTeams);
				m_p0SpinNrTeams = Gtk::manage(new Gtk::SpinButton(m_refAdjustmentNrTeams));
				m_p0HBoxNrTeams->pack_start(*m_p0SpinNrTeams);
					m_p0SpinNrTeams->signal_value_changed().connect(
									sigc::mem_fun(*this, &PlayersScreen::onSpinNrTeamsChanged) );

		//addSeparator(m_p0TabVBoxPlayersTeams, 5);
		Gtk::Box* m_p0HBoxPlayersTeams = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
		m_p0TabVBoxPlayersTeams->pack_start(*m_p0HBoxPlayersTeams, true, true);
			Gtk::Box* m_p0VBoxPlayers = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
			m_p0HBoxPlayersTeams->pack_start(*m_p0VBoxPlayers, true, true);
				m_p0HBoxRename = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
				m_p0VBoxPlayers->pack_start(*m_p0HBoxRename, false, false);
					m_p0HBoxRename->set_margin_top(3);
					m_p0LabelRename = Gtk::manage(new Gtk::Label("Player:"));
						m_p0LabelRename->set_margin_start(5);
						m_p0LabelRename->set_margin_end(3);
					m_p0HBoxRename->pack_start(*m_p0LabelRename, false, false);
					m_p0EntryRename = Gtk::manage(new Gtk::Entry());
					m_p0HBoxRename->pack_start(*m_p0EntryRename, true, true);
						m_p0EntryRename->set_margin_end(5);
						//m_p0EntryRename->signal_focus_out_event().connect(
						//				sigc::mem_fun(*this, &PlayersScreen::onRenameChangedFocus) );
				Gtk::ScrolledWindow* m_p0ScrolledPlayers = Gtk::manage(new Gtk::ScrolledWindow());
				m_p0VBoxPlayers->pack_start(*m_p0ScrolledPlayers, true, true);
					m_p0ScrolledPlayers->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
					m_p0ScrolledPlayers->set_margin_top(3);
					//Create the Players tree model
					m_refTreeModelPlayers = Gtk::TreeStore::create(m_oPlayersColumns);
					m_p0TreeViewPlayers = Gtk::manage(new Gtk::TreeView(m_refTreeModelPlayers));
					m_p0ScrolledPlayers->add(*m_p0TreeViewPlayers);
						m_p0TreeViewPlayers->append_column("Team/Player", m_oPlayersColumns.m_oColPlayer);
						refTreeSelection = m_p0TreeViewPlayers->get_selection();
						refTreeSelection->signal_changed().connect(
										sigc::mem_fun(*this, &PlayersScreen::onPlayerSelectionChanged));
			Gtk::Box* m_p0VBoxRenameMovePlayers = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
			m_p0HBoxPlayersTeams->pack_start(*m_p0VBoxRenameMovePlayers, false, false);
				m_p0ButtonRename = Gtk::manage(new Gtk::Button(s_sPlayerOrTeamRenameEllipsize));
				m_p0VBoxRenameMovePlayers->pack_start(*m_p0ButtonRename, false, false);
					m_p0ButtonRename->set_margin_top(3);
					m_p0ButtonRename->set_margin_bottom(3);
					m_p0ButtonRename->signal_clicked().connect(
									sigc::mem_fun(*this, &PlayersScreen::onButtonRename) );
				m_p0VBoxMovePlayers = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
				m_p0VBoxRenameMovePlayers->pack_start(*m_p0VBoxMovePlayers, true, true);
					m_p0ButtonMoveUp = Gtk::manage(new Gtk::Button("Move Up"));
					m_p0VBoxMovePlayers->pack_start(*m_p0ButtonMoveUp, true, true);
						m_p0ButtonMoveUp->signal_clicked().connect(
										sigc::mem_fun(*this, &PlayersScreen::onButtonMoveUp) );
					m_p0ButtonMoveDown = Gtk::manage(new Gtk::Button("Move Down"));
					m_p0VBoxMovePlayers->pack_start(*m_p0ButtonMoveDown, true, true);
						m_p0ButtonMoveDown->signal_clicked().connect(
										sigc::mem_fun(*this, &PlayersScreen::onButtonMoveDown) );
					m_p0ButtonTeamUp = Gtk::manage(new Gtk::Button("Team Up"));
					m_p0VBoxMovePlayers->pack_start(*m_p0ButtonTeamUp, true, true);
						m_p0ButtonTeamUp->signal_clicked().connect(
										sigc::mem_fun(*this, &PlayersScreen::onButtonTeamUp) );
					m_p0ButtonTeamDown = Gtk::manage(new Gtk::Button("Team Down"));
					m_p0VBoxMovePlayers->pack_start(*m_p0ButtonTeamDown, true, true);
						m_p0ButtonTeamDown->signal_clicked().connect(
										sigc::mem_fun(*this, &PlayersScreen::onButtonTeamDown) );


	auto& oGameOptions = m_refStdConfig->getOptions(OwnerType::GAME);
	auto& oTeamOptions = m_refStdConfig->getOptions(OwnerType::TEAM);
	auto& oPlayerOptions = m_refStdConfig->getOptions(OwnerType::PLAYER);

	Gtk::Label* m_p0TabLabelGameOptions = Gtk::manage(new Gtk::Label("Options"));
	Gtk::Box* m_p0TabVBoxGameOptions = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	if (m_bShowGameTab) {
		m_aPageIndex[s_nTabGameOptions] = m_p0NotebookPlayers->append_page(*m_p0TabVBoxGameOptions, *m_p0TabLabelGameOptions);
	} else {
		m_aPageIndex[s_nTabGameOptions] = -1;
	}
		Gtk::Box* m_p0VBoxGameOptions = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0TabVBoxGameOptions->pack_start(*m_p0VBoxGameOptions, false, false);
			addOptions(oGameOptions, m_aGameOptionWidgetSetters, OwnerType::GAME, m_p0VBoxGameOptions);

		m_p0ButtonPlayTestGlobalSound = Gtk::manage(new Gtk::Button("Test sound"));
		m_p0TabVBoxGameOptions->pack_start(*m_p0ButtonPlayTestGlobalSound, false, false, 3);
			m_p0ButtonPlayTestGlobalSound->set_margin_left(s_nButtonLeftRightMargin);
			m_p0ButtonPlayTestGlobalSound->set_margin_right(s_nButtonLeftRightMargin);
			m_p0ButtonPlayTestGlobalSound->set_margin_top(5);
			m_p0ButtonPlayTestGlobalSound->set_margin_bottom(5);
			m_p0ButtonPlayTestGlobalSound->signal_clicked().connect(
							sigc::mem_fun(*this, &PlayersScreen::onButtonPlayTestGlobalSound) );

	Gtk::Label* m_p0TabLabelTeamOptions = Gtk::manage(new Gtk::Label("Team"));
	Gtk::Box* m_p0TabVBoxTeamOptions = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	if (m_bShowTeamTab) {
		m_aPageIndex[s_nTabTeamOptions] = m_p0NotebookPlayers->append_page(*m_p0TabVBoxTeamOptions, *m_p0TabLabelTeamOptions);
	} else {
		m_aPageIndex[s_nTabTeamOptions] = -1;
	}
		Gtk::Box* m_p0VBoxTeamOptions = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0TabVBoxTeamOptions->pack_start(*m_p0VBoxTeamOptions, true, true);
			addOptions(oTeamOptions, m_aTeamOptionWidgetSetters, OwnerType::TEAM, m_p0VBoxTeamOptions);

	Gtk::Label* m_p0TabLabelPlayerOptions = Gtk::manage(new Gtk::Label((m_bAlwaysOnePlayer && ! m_bShowGameTab) ? "Options" : "Player"));
	Gtk::Box* m_p0TabVBoxPlayerOptions = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	if (m_bShowPlayerTab) {
		m_aPageIndex[s_nTabPlayerOptions] = m_p0NotebookPlayers->append_page(*m_p0TabVBoxPlayerOptions, *m_p0TabLabelPlayerOptions);
	} else {
		m_aPageIndex[s_nTabPlayerOptions] = -1;
	}
		Gtk::Box* m_p0VBoxPlayerOptions = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0TabVBoxPlayerOptions->pack_start(*m_p0VBoxPlayerOptions, false, false);
			addOptions(oPlayerOptions, m_aPlayerOptionWidgetSetters, OwnerType::PLAYER, m_p0VBoxPlayerOptions);

		m_p0ScrolledSoundCapabilities = Gtk::manage(new Gtk::ScrolledWindow());
		m_p0TabVBoxPlayerOptions->pack_start(*m_p0ScrolledSoundCapabilities, true, true, 3);
			m_p0ScrolledSoundCapabilities->set_policy(Gtk::POLICY_ALWAYS, Gtk::POLICY_ALWAYS);
			m_refTreeModelSoundCapabilities = Gtk::ListStore::create(m_oSoundCapabilitiesColumns);
			m_p0TreeViewSoundCapabilities = Gtk::manage(new Gtk::TreeView(m_refTreeModelSoundCapabilities));
			if (bPerPlayerSounds) {
				m_p0ScrolledSoundCapabilities->add(*m_p0TreeViewSoundCapabilities);
					m_p0TreeViewSoundCapabilities->append_column("Device", m_oSoundCapabilitiesColumns.m_oColDeviceName);
					m_p0TreeViewSoundCapabilities->append_column("Id", m_oSoundCapabilitiesColumns.m_oColCapabilityId);
					m_p0TreeViewSoundCapabilities->append_column("Player", m_oSoundCapabilitiesColumns.m_oColSoundToPlayerName);
					refTreeSelection = m_p0TreeViewSoundCapabilities->get_selection();
					refTreeSelection->set_mode(Gtk::SELECTION_SINGLE);
					refTreeSelection->signal_changed().connect(
										sigc::mem_fun(*this, &PlayersScreen::onSoundCapabilitiesSelectionChanged));
			}
		m_p0ButtonPlayTestPlayerSound = Gtk::manage(new Gtk::Button("Test sound"));
		m_p0TabVBoxPlayerOptions->pack_start(*m_p0ButtonPlayTestPlayerSound, false, false, 3);
			m_p0ButtonPlayTestPlayerSound->signal_clicked().connect(
							sigc::mem_fun(*this, &PlayersScreen::onButtonPlayTestPlayerSound) );

	Gtk::Label* m_p0TabLabelPlayerDeviceKeyActions = Gtk::manage(new Gtk::Label("Keys"));
	Gtk::Box* m_p0TabVBoxPlayerDeviceKeyActions = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	if (m_bShowKeyActions || m_bShowAssignedCapabilities) {
		m_aPageIndex[s_nTabPlayerDeviceKeyActions] = m_p0NotebookPlayers->append_page(*m_p0TabVBoxPlayerDeviceKeyActions, *m_p0TabLabelPlayerDeviceKeyActions);
	} else {
		m_aPageIndex[s_nTabPlayerDeviceKeyActions] = -1;
	}

		Gtk::Box* m_p0VBoxKeyActionsKeys = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 0));
		m_p0TabVBoxPlayerDeviceKeyActions->pack_start(*m_p0VBoxKeyActionsKeys);
			m_p0LabelKeysModifyKey = Gtk::manage(new Gtk::Label(m_sLabelKeysModifyKey));
			m_p0VBoxKeyActionsKeys->pack_start(*m_p0LabelKeysModifyKey, false, false, 5);
				m_p0LabelKeysModifyKey->set_alignment(Gtk::ALIGN_START);
				m_p0LabelKeysModifyKey->set_visible(m_bShowKeyActions);
			Gtk::ScrolledWindow* m_p0ScrolledKeys = Gtk::manage(new Gtk::ScrolledWindow());
			m_p0VBoxKeyActionsKeys->pack_start(*m_p0ScrolledKeys, true, true, 3);
				//Create the Keys tree model
				m_refTreeModelKeys = Gtk::ListStore::create(m_oKeysColumns);
				m_p0TreeViewKeys = Gtk::manage(new KeysTreeView(this, m_refTreeModelKeys));
				m_p0ScrolledKeys->add(*m_p0TreeViewKeys);
					m_p0TreeViewKeys->append_column("Action", m_oKeysColumns.m_oColAction);
					m_p0TreeViewKeys->append_column("Key", m_oKeysColumns.m_oColKey);
					m_p0TreeViewKeys->append_column("Device", m_oKeysColumns.m_oColDeviceName);
					m_p0TreeViewKeys->append_column("Capability", m_oKeysColumns.m_oColCapabilityClassId);
					m_p0TreeViewKeys->append_column("Id", m_oKeysColumns.m_oColCapabilityId);
					//
					if (m_bShowKeyActions) {
						refTreeSelection = m_p0TreeViewKeys->get_selection();
						refTreeSelection->signal_changed().connect(
												sigc::mem_fun(*this, &PlayersScreen::onKeySelectionChanged));
					} else {
						m_p0ScrolledKeys->set_visible(false);
					}

		Gtk::ScrolledWindow* m_p0ScrolledAssignedCapabilities = Gtk::manage(new Gtk::ScrolledWindow());
		if (m_bShowAssignedCapabilities) {
			m_p0TabVBoxPlayerDeviceKeyActions->pack_start(*m_p0ScrolledAssignedCapabilities, true, true, 5);
		}
			//Create the assigned capabilities tree model
			m_refTreeModelAssignedCapabilities = Gtk::ListStore::create(m_oAssignedCapabilitiesColumns);
			m_p0TreeViewAssignedCapabilities = Gtk::manage(new Gtk::TreeView(m_refTreeModelAssignedCapabilities));
			if (m_bShowAssignedCapabilities) {
				m_p0ScrolledAssignedCapabilities->add(*m_p0TreeViewAssignedCapabilities);
					m_p0TreeViewAssignedCapabilities->append_column("Device", m_oAssignedCapabilitiesColumns.m_oColDeviceName);
					m_p0TreeViewAssignedCapabilities->append_column("Capability", m_oAssignedCapabilitiesColumns.m_oColCapabilityClassId);
					m_p0TreeViewAssignedCapabilities->append_column("Id", m_oAssignedCapabilitiesColumns.m_oColCapabilityId);
					m_p0TreeViewAssignedCapabilities->append_column("Player", m_oAssignedCapabilitiesColumns.m_oColAssignedToPlayerName);
					refTreeSelection = m_p0TreeViewAssignedCapabilities->get_selection();
					refTreeSelection->set_mode(Gtk::SELECTION_MULTIPLE);
					refTreeSelection->signal_changed().connect(
										sigc::mem_fun(*this, &PlayersScreen::onAssignedCapabilitiesSelectionChanged));
			}

		Gtk::Button* m_p0ButtonOk = Gtk::manage(new Gtk::Button("Ok"));
		m_p0PlayersScreenBoxMain->pack_start(*m_p0ButtonOk, false, false);
			m_p0ButtonOk->set_margin_left(s_nButtonLeftRightMargin);
			m_p0ButtonOk->set_margin_right(s_nButtonLeftRightMargin);
			m_p0ButtonOk->set_margin_top(5);
			m_p0ButtonOk->set_margin_bottom(5);
			m_p0ButtonOk->signal_clicked().connect(
							sigc::mem_fun(*this, &PlayersScreen::onButtonOk) );
		Gtk::Button* m_p0ButtonCancel  = Gtk::manage(new Gtk::Button("Cancel"));
		m_p0PlayersScreenBoxMain->pack_start(*m_p0ButtonCancel, false, false);
			m_p0ButtonCancel->set_margin_left(s_nButtonLeftRightMargin);
			m_p0ButtonCancel->set_margin_right(s_nButtonLeftRightMargin);
			m_p0ButtonCancel->set_margin_top(5);
			m_p0ButtonCancel->set_margin_bottom(5);
			m_p0ButtonCancel->signal_clicked().connect(
							sigc::mem_fun(*this, &PlayersScreen::onButtonCancel) );

	m_refDM = m_refStdConfig->getDeviceManager();
	m_refEventListener = std::make_shared<stmi::EventListener>(
		[this](const shared_ptr<stmi::Event>& refEvent)
		{
			onStmiEvent(refEvent);
		});
	#ifndef NDEBUG
	const bool bAdded =
	#endif
	m_refDM->addEventListener(m_refEventListener);
	assert(bAdded);

	return m_p0PlayersScreenBoxMain;
}
bool PlayersScreen::changeTo(const shared_ptr<AllPreferences>& refPrefs, const shared_ptr<Theme>& refTheme) noexcept
{
	assert(refPrefs);
	assert(m_refStdConfig == refPrefs->getStdConfig());

	m_nSelectedTeam = -1;
	m_nSelectedMate = -1;
	m_nSelectedKeyAction = -1;
	m_bExpectingKey = false;
	if (m_bShowKeyActions) {
		m_p0LabelKeysModifyKey->set_label(m_sLabelKeysModifyKey);
	}

	m_refPrefs = refPrefs;
	m_refTheme = refTheme;
	m_bPrefsInitialized = true;

	m_p0NotebookPlayers->set_current_page(m_aPageIndex[s_nTabTeamPlayers]);

	if (m_refTheme) {
		const Named& oNamed = m_refTheme->getNamed();
		m_nSoundTestIdx = oNamed.sounds().getIndex(s_sTestSoundName);
		if (m_nSoundTestIdx < 0) {
			std::cout << "Theme doesn't provide sound with id '" << s_sTestSoundName << "'" << '\n';
		}
	} else {
		m_nSoundTestIdx = -1;
	}
	m_bEditingTeamOrPlayer = false;
	onChangedEditingMode();
	regeneratePlayersModel();
	//
	m_p0HBoxNrPlayersTeams->set_visible(! m_bAlwaysOnePlayer);
	m_p0VBoxMovePlayers->set_visible(! m_bAlwaysOnePlayer);

	return true;
}
void PlayersScreen::addOptions(const NamedObjIndex< shared_ptr<Option> >& oOptions, std::vector< std::function<void()> >& aWidgetSetters
								, OwnerType eOptionOwner, Gtk::Box* p0Box) noexcept
{
	assert(aWidgetSetters.size() == 0);
	assert(p0Box != nullptr);
	const int32_t nTotOptions = oOptions.size();
//std::cout << "PlayersScreen::addOptions  nTotOptions=" << nTotOptions << '\n';
	if (nTotOptions == 0) {
		return;
	}
	aWidgetSetters.resize(nTotOptions);
	for (int32_t nIdx = 0; nIdx < nTotOptions; ++nIdx) {
		const auto& refOption = oOptions.getObj(nIdx);
//std::cout << "  refOption->getName()" << refOption->getName() << '\n';
		if (! refOption->isVisible()) {
			aWidgetSetters[nIdx] = [](){};
			continue; // for ---------
		}
//std::cout << "  refOption->getDesc()" << refOption->getDesc() << '\n';
		const bool bReadOnly = refOption->isReadonly();
		//TODO define factory of widget selected by option type string (NamedIndex?) ?
		Option* p0Option = refOption.operator->();
		IntOption* p0IntOption = dynamic_cast<IntOption*>(p0Option);
		if (p0IntOption != nullptr) {
			createIntOptionWidget(p0IntOption, eOptionOwner, bReadOnly, p0Box, aWidgetSetters[nIdx]);
		} else {
			BoolOption* p0BoolOption = dynamic_cast<BoolOption*>(p0Option);
			if (p0BoolOption != nullptr) {
				createBoolOptionWidget(p0BoolOption, eOptionOwner, bReadOnly, p0Box, aWidgetSetters[nIdx]);
			} else {
				EnumOption* p0EnumOption = dynamic_cast<EnumOption*>(p0Option);
				if (p0EnumOption != nullptr) {
					createEnumOptionWidget(p0EnumOption, eOptionOwner, bReadOnly, p0Box, aWidgetSetters[nIdx]);
				} else {
					continue; // for --------
				}
			}
		}
	}
}
void PlayersScreen::createIntOptionWidget(IntOption* p0IntOption, OwnerType eOptionOwner, bool bReadOnly
										, Gtk::Box* p0ContainerBox, std::function<void()>& oWidgetSetter) noexcept
{
//std::cout << "createIntOptionWidget(p0IntOption)" << '\n';
	const bool bSensitive = !bReadOnly;
	assert(p0IntOption != nullptr);
	assert(p0ContainerBox != nullptr);
	const auto& sDesc = p0IntOption->getDesc();
	Gtk::Box* p0HB = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
	p0ContainerBox->pack_start(*p0HB, false, false, 5);
	//
	Gtk::Label* p0L = Gtk::manage(new Gtk::Label(sDesc));
	p0HB->pack_start(*p0L, false, false);
	//
	Gtk::SpinButton* p0SB = Gtk::manage(new Gtk::SpinButton(Gtk::Adjustment::create(
			p0IntOption->getDefaultValue().getInt(), p0IntOption->getMin(), p0IntOption->getMax(), 1, 10, 0)));
	p0HB->pack_start(*p0SB, false, false);
	if (bSensitive) {
		p0SB->signal_value_changed().connect(sigc::bind(
						sigc::mem_fun(*this, &PlayersScreen::onIntOptionChanged), eOptionOwner, p0IntOption, p0SB));
	}
	//
	p0L->set_margin_start(3);
	p0L->set_margin_end(5);
	p0L->set_sensitive(bSensitive);
	p0SB->set_sensitive(bSensitive);
	p0HB->set_sensitive(bSensitive);

	oWidgetSetter = [this, p0HB, p0SB, p0IntOption, eOptionOwner]() {
//std::cout << "oWidgetSetter p0IntOption" << '\n';
		const auto& sName = p0IntOption->getName();
		const bool bVisible = isSlaveOptionVisible(p0IntOption);
		const int32_t nValue = getOptionValue(eOptionOwner, sName).getInt();
		p0SB->set_value(nValue);
		p0HB->set_visible(bVisible);
	};
}
void PlayersScreen::createBoolOptionWidget(BoolOption* p0BoolOption, OwnerType eOptionOwner, bool bReadOnly
										, Gtk::Box* p0ContainerBox, std::function<void()>& oWidgetSetter) noexcept
{
//std::cout << "createBoolOptionWidget(p0BoolOption)" << '\n';
	const bool bSensitive = ! bReadOnly;
	assert(p0BoolOption != nullptr);
	assert(p0ContainerBox != nullptr);
	const auto& sDesc = p0BoolOption->getDesc();
	Gtk::CheckButton* p0CB = Gtk::manage(new Gtk::CheckButton(sDesc));
	p0ContainerBox->pack_start(*p0CB, false, false, 5);
	//
	if (bSensitive) {
		p0CB->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &PlayersScreen::onBoolOptionChanged), eOptionOwner, p0BoolOption, p0CB));
	}
	p0CB->set_sensitive(bSensitive);

	oWidgetSetter = [this, p0CB, p0BoolOption, eOptionOwner]() {
		const auto& sName = p0BoolOption->getName();
		const bool bVisible = isSlaveOptionVisible(p0BoolOption);
		const bool bValue = getOptionValue(eOptionOwner, sName).getBool();
		p0CB->set_active(bValue);
		p0CB->set_visible(bVisible);
	};
}
void PlayersScreen::createEnumOptionWidget(EnumOption* p0EnumOption, OwnerType eOptionOwner, bool bReadOnly
										, Gtk::Box* p0ContainerBox, std::function<void()>& oWidgetSetter) noexcept
{
//std::cout << "createEnumOptionWidget(p0EnumOption)" << '\n';
	const bool bSensitive = ! bReadOnly;
	assert(p0EnumOption != nullptr);
	assert(p0ContainerBox != nullptr);
	Gtk::Box* p0VB = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
	p0ContainerBox->pack_start(*p0VB, false, false);

	const auto& sDesc = p0EnumOption->getDesc();
	Gtk::Label* p0L = Gtk::manage(new Gtk::Label(sDesc));
	p0VB->pack_start(*p0L);
	p0L->set_alignment(Gtk::ALIGN_START);

	p0L->set_sensitive(bSensitive);
	//TODO invisible separator?

	Gtk::RadioButton::Group oGroup;
	std::vector<Gtk::RadioButton*> aRBs; // non owning pointers
	const int32_t nSize = p0EnumOption->size();
	for (int32_t nEnumIdx = 0; nEnumIdx < nSize; ++nEnumIdx) {
		auto& sEnumDesc = p0EnumOption->getEnumDesc(nEnumIdx);
		Gtk::RadioButton* p0RB = Gtk::manage(new Gtk::RadioButton(sEnumDesc));
		p0VB->pack_start(*p0RB, false, false);
		p0RB->set_vexpand(false);
		aRBs.push_back(p0RB);
		if (nEnumIdx == 0) {
			oGroup = p0RB->get_group();
		} else {
			p0RB->set_group(oGroup);
		}
		if (bSensitive) {
			p0RB->signal_clicked().connect(sigc::bind(
						sigc::mem_fun(*this, &PlayersScreen::onEnumOptionChanged), eOptionOwner, p0EnumOption, p0RB, nEnumIdx));
		}
		p0RB->set_sensitive(bSensitive);
	}
	p0VB->set_sensitive(bSensitive);

	oWidgetSetter = [this, p0VB, aRBs, p0EnumOption, eOptionOwner]() {
//std::cout << "oWidgetSetter aRBs.size()=" << aRBs.size() << '\n';
		const auto& sName = p0EnumOption->getName();
		const bool bVisible = isSlaveOptionVisible(p0EnumOption);
		p0VB->set_visible(bVisible);
		const int32_t nEnum = getOptionValue(eOptionOwner, sName).getInt();
		const int32_t nEnumIdx = p0EnumOption->getIdx(nEnum);
		assert((nEnumIdx >= 0) && (nEnumIdx < p0EnumOption->size()));
		aRBs[nEnumIdx]->set_active(true);
	};
}
bool PlayersScreen::isSlaveOptionVisible(Option* p0SlaveOption) noexcept
{
	assert(p0SlaveOption != nullptr);
	const auto& aMastersValues = p0SlaveOption->getMastersValues();
	for (const auto& oPair : aMastersValues) {
		const shared_ptr<Option>& refMaster = oPair.first;
		const std::vector<Variant>& aValues = oPair.second;
		Variant oMasterValue = getOptionValue(refMaster->getOwnerType(), refMaster->getName());
//std::cout << "Option " << refMaster->getName() << " type " << static_cast<int32_t>(refMaster->getOwnerType()) << "  aValues " << aValues.size() << '\n';
//std::cout << "  master value "; oMasterValue.dump(3);
//if (aValues.size() > 0) {
//std::cout << "  enable value "; aValues[0].dump(3);
//}
		auto itFind = std::find(aValues.begin(), aValues.end(), oMasterValue);
		if (itFind == aValues.end()) {
			// a master disables the slave
			return false;
		}
	}
	return true;
}
Variant PlayersScreen::getOptionValue(OwnerType eOptionOwner, const std::string& sOptionName) noexcept
{
	if (eOptionOwner == OwnerType::GAME) {
		return m_refPrefs->getOptionValue(sOptionName); //---------------------------
	}
	assert(m_nSelectedTeam >= 0);
	auto& refTeam = m_refPrefs->getTeamFull(m_nSelectedTeam);
	if (eOptionOwner == OwnerType::TEAM) {
		return refTeam->getOptionValue(sOptionName); //------------------------------
	}
	assert(eOptionOwner == OwnerType::PLAYER);
	assert(m_nSelectedMate >= 0);
	return refTeam->getMateFull(m_nSelectedMate)->getOptionValue(sOptionName);
}
void PlayersScreen::regenerateGameOptions() noexcept
{
	if (!m_bShowGameTab) {
		return;
	}
	m_bRegenerateOptionsInProgress = true;
//std::cout << "regenerateGameOptions() m_aGameOptionWidgetSetters.size()=" << m_aGameOptionWidgetSetters.size() << '\n';
	for (auto& oWidgetSetter : m_aGameOptionWidgetSetters) {
		oWidgetSetter();
	}
	m_bRegenerateOptionsInProgress = false;
}
void PlayersScreen::regenerateTeamOptions() noexcept
{
	if (! m_bShowTeamTab) {
		return;
	}
	if (m_nSelectedTeam < 0) {
		return;
	}
	m_bRegenerateOptionsInProgress = true;
	for (auto& oWidgetSetter : m_aTeamOptionWidgetSetters) {
		oWidgetSetter();
	}
	m_bRegenerateOptionsInProgress = false;
}
void PlayersScreen::regeneratePlayerOptions() noexcept
{
	if (! m_bShowPlayerTab) {
		return;
	}
	if (m_nSelectedMate < 0) {
		return;
	}
	assert(m_nSelectedTeam >= 0);
	m_bRegenerateOptionsInProgress = true;
	for (auto& oWidgetSetter : m_aPlayerOptionWidgetSetters) {
		oWidgetSetter();
	}
	m_bRegenerateOptionsInProgress = false;
}

void PlayersScreen::onIntOptionChanged(OwnerType eOptionOwner, IntOption* p0IntOption, Gtk::SpinButton* p0SB) noexcept
{
	const double fValue = p0SB->get_value();
	const int32_t nValue = std::round<int32_t>(fValue);
	setOptionValue(eOptionOwner, p0IntOption->getName(), Variant{nValue});
}
void PlayersScreen::onBoolOptionChanged(OwnerType eOptionOwner, BoolOption* p0BoolOption, Gtk::CheckButton* p0CB) noexcept
{
	const bool bValue = p0CB->get_active();
	const std::string& sOptionName = p0BoolOption->getName();
	setOptionValue(eOptionOwner, sOptionName, Variant{bValue});
	const bool bIsPerPlayerSoundOption = ((eOptionOwner == OwnerType::GAME) && (sOptionName == m_refStdConfig->getPerPlayerSoundOptionName()));
	if (bIsPerPlayerSoundOption || ((eOptionOwner == OwnerType::PLAYER) && (sOptionName == m_refStdConfig->getAIOptionName()))) {
		regenerateSoundDevicesList();
	}
	if (bIsPerPlayerSoundOption) {
		refreshPlayTestGlobalSound();
	}
}
void PlayersScreen::onEnumOptionChanged(OwnerType eOptionOwner, EnumOption* p0EnumOption, Gtk::RadioButton* p0RB, int32_t nEnumIdx) noexcept
{
	const bool bSet = p0RB->get_active();
	if (!bSet) {
		// some other in the group has been set
		return; //--------------------------------------------------------------
	}
	int32_t nValue = p0EnumOption->getEnum(nEnumIdx);
	setOptionValue(eOptionOwner, p0EnumOption->getName(), Variant{nValue});
}
void PlayersScreen::setOptionValue(OwnerType eOptionOwner, const std::string& sOptionName, const Variant& oVar) noexcept
{
	if (m_bRegenerateOptionsInProgress) {
		return;
	}
	if (!m_refPrefs) {
		return;
	}
	if (eOptionOwner == OwnerType::GAME) {
		m_refPrefs->setOptionValue(sOptionName, oVar);
		regenerateGameOptions();
	} else {
		assert(m_nSelectedTeam != -1);
		const shared_ptr<StdPreferences::Team>& refTeam = m_refPrefs->getTeamFull(m_nSelectedTeam);
		assert(refTeam);
		if (eOptionOwner == OwnerType::TEAM) {
			refTeam->setOptionValue(sOptionName, oVar);
			regenerateTeamOptions();
		} else {
			assert(eOptionOwner == OwnerType::PLAYER);
			assert(m_nSelectedMate != -1);
			const shared_ptr<StdPreferences::Player>& refMate = refTeam->getMateFull(m_nSelectedMate);
			assert(refMate);
			const bool bOk = refMate->setOptionValue(sOptionName, oVar);
			if (! bOk) {
				std::cout << "Couldn't set option " << sOptionName;
				if (sOptionName == m_refStdConfig->getAIOptionName()) {
					std::cout << ": probably would violate some condition such as" << '\n';
					std::cout << " -> 'mixed human-AI teams not allowed'";
				}
				std::cout << '\n';
			}
			regeneratePlayerOptions();
		}
	}
}
void PlayersScreen::onButtonOk() noexcept
{
	m_oGameWindow.afterChoosePlayers(m_refPrefs);
}
void PlayersScreen::onButtonCancel() noexcept
{
	m_oGameWindow.afterChoosePlayers(shared_ptr<AllPreferences>{});
}

void PlayersScreen::regeneratePlayersModel() noexcept
{
	if (!m_bPrefsInitialized) {
		return;
	}
	m_bRegeneratePlayersInProgress = true;

	m_refTreeModelPlayers->clear();

	const int32_t nTotTeams = m_refPrefs->getTotTeams();
//std::cout << "regeneratePlayersModel()  nTotTeams=" << nTotTeams << '\n';
	if (m_nSelectedTeam >= nTotTeams) {
		m_nSelectedTeam = nTotTeams - 1;
	}
	for (int32_t nTeam = 0; nTeam < nTotTeams; ++nTeam) {
		const StdPreferences::Team& oTeam = *(m_refPrefs->getTeamFull(nTeam));
		Glib::ustring sTeamName = oTeam.getName();
		if (sTeamName.empty()) {
			sTeamName = Glib::ustring("Team") + " " + std::to_string(nTeam + 1);
		}
		Gtk::TreeModel::Row oTeamRow;
		if (m_bShowTeamTreeNode) {
			oTeamRow = *(m_refTreeModelPlayers->append()); //oGameRow.children()
			oTeamRow[m_oPlayersColumns.m_oColPlayer] = sTeamName;
			oTeamRow[m_oPlayersColumns.m_oColHiddenTeam] = nTeam;
			oTeamRow[m_oPlayersColumns.m_oColHiddenMate] = -1;
		}
		const int32_t nTotTeammates = oTeam.getTotMates();
		if ((nTeam == m_nSelectedTeam) && (m_nSelectedMate >= nTotTeammates)) {
			m_nSelectedMate = nTotTeammates - 1;
		}
		for (int32_t nMate = 0; nMate < nTotTeammates; ++nMate) {
			const StdPreferences::Player& oMate = *(oTeam.getMateFull(nMate));
			Glib::ustring sMateName = oMate.getName();
			Gtk::TreeModel::Row oMateRow = (Gtk::TreeModel::Row::BoolExpr(oTeamRow)
						? *(m_refTreeModelPlayers->append(oTeamRow.children()))
						: *(m_refTreeModelPlayers->append()));
			if (oMate.isAI()) {
				sMateName.append(" (AI)");
			}
			oMateRow[m_oPlayersColumns.m_oColPlayer] = sMateName;
			oMateRow[m_oPlayersColumns.m_oColHiddenTeam] = nTeam;
			oMateRow[m_oPlayersColumns.m_oColHiddenMate] = nMate;
		}
	}

	m_p0SpinNrPlayers->set_value(m_refPrefs->getTotPlayers());
	m_p0SpinNrTeams->set_value(m_refPrefs->getTotTeams());

	m_p0TreeViewPlayers->expand_all();
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_p0TreeViewPlayers->get_selection();
	Gtk::TreeModel::Path oPath;
	if (m_nSelectedTeam >= 0) {
		oPath.push_back(m_nSelectedTeam);
		if (m_nSelectedMate >= 0) {
			oPath.push_back(m_nSelectedMate);
		}
	}
//std::cout << "   oPath.size=" << oPath.size() << '\n';
	refTreeSelection->select(oPath);

	m_p0ButtonRename->set_label(m_bEditingTeamOrPlayer ? s_sPlayerOrTeamRenameDoIt : s_sPlayerOrTeamRenameEllipsize);
	m_p0ButtonRename->set_sensitive(m_nSelectedTeam >= 0);

	m_bRegeneratePlayersInProgress = false;
}
bool PlayersScreen::onDevicesTimeout() noexcept
{
	if (m_bShowAssignedCapabilities) {
		regenerateAssignedDevicesList();
	}
	regenerateSoundDevicesList();

	if (m_bShowKeyActions) {
		regenerateKeysModel();
	}
	// only once timeout
	const bool bContinue = false;
	return bContinue;
}
void PlayersScreen::regenerateAssignedDevicesList() noexcept
{
	if (!m_bPrefsInitialized) {
		return;
	}
//std::cout << "regenerateAssignedDevicesList()" << '\n';
	assert(!m_bRegenerateAssignedDevicesInProgress);
	m_bRegenerateAssignedDevicesInProgress = true;
	m_refTreeModelAssignedCapabilities->clear();
	std::vector<int32_t> aSelectedRows;
	int32_t nRowIdx = 0;
	const auto aDeviceIds = m_refDM->getDevices();
	for (int32_t nDeviceId : aDeviceIds) {
		auto refDevice = m_refDM->getDevice(nDeviceId);
		assert(refDevice);
		const std::string& sDeviceName = refDevice->getName();
//std::cout << "regenerateAssignedDevicesList()  sDeviceName " << sDeviceName << "   nDeviceId=" << nDeviceId << '\n';
		const auto aCapabilityIds = refDevice->getCapabilities();
		for (int32_t nCapabilityId : aCapabilityIds) {
			auto refCapability = refDevice->getCapability(nCapabilityId);
//std::cout << "regenerateAssignedDevicesList()  nCapabilityId " << nCapabilityId << '\n';
			assert(refCapability);
			const stmi::Capability::Class& oClass = refCapability->getCapabilityClass();
			assert(oClass);
			if (oClass == stmi::PlaybackCapability::getClass()) {
				// assigned playback are shown elsewhere and only if sound is enabled
				continue;
			}
			if (! m_oCapabilityAssignment.m_bAllCapabilityClasses) {
				const auto& aAllowed = m_oCapabilityAssignment.m_aCapabilityClasses;
				if (std::find(aAllowed.begin(), aAllowed.end(), oClass) == aAllowed.end()) {
					continue; // for nCapabilityId ----
				}
			}
			const std::string sCapaClassId = getCapabilityClassId(oClass);
			int32_t nTeam;
			int32_t nMate;
			const bool bAssigned = m_refPrefs->getCapabilityPlayer(refCapability->getId(), nTeam, nMate);
			std::string sPlayerName = (bAssigned ? m_refPrefs->getTeamFull(nTeam)->getMateFull(nMate)->getName() : "");
			if (bAssigned && (nTeam == m_nSelectedTeam) && (nMate == m_nSelectedMate)) {
				aSelectedRows.push_back(nRowIdx);
			}
			//
			Gtk::TreeModel::Row oRow;
			oRow = *(m_refTreeModelAssignedCapabilities->append());
			oRow[m_oAssignedCapabilitiesColumns.m_oColDeviceName] = sDeviceName;
			oRow[m_oAssignedCapabilitiesColumns.m_oColCapabilityClassId] = sCapaClassId;
			oRow[m_oAssignedCapabilitiesColumns.m_oColAssignedToPlayerName] = sPlayerName;
			oRow[m_oAssignedCapabilitiesColumns.m_oColHiddenDeviceId] = nDeviceId;
			oRow[m_oAssignedCapabilitiesColumns.m_oColCapabilityId] = nCapabilityId;
			++nRowIdx;
		}
	}
	m_p0TreeViewAssignedCapabilities->expand_all();
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_p0TreeViewAssignedCapabilities->get_selection();
	Gtk::TreeModel::Path oPath;
	for (const auto& nRowIdx : aSelectedRows) {
//std::cout << "regenerateAssignedDevicesList()  select row " << nRowIdx << '\n';
		oPath.clear();
		oPath.push_back(nRowIdx);
		refTreeSelection->select(oPath);
	}
	m_bRegenerateAssignedDevicesInProgress = false;
}
std::string PlayersScreen::getCapabilityClassId(const stmi::Capability::Class& oClass) const noexcept
{
	std::string sCapaClassId = oClass.getId();
	auto nLastPos = sCapaClassId.rfind("::");
	if (nLastPos != std::string::npos) {
		sCapaClassId.erase(0, nLastPos + 2);
	}
	return sCapaClassId;
}
void PlayersScreen::onAssignedCapabilitiesSelectionChanged() noexcept
{
	if (m_nSelectedMate < 0) {
		return;
	}
	if (m_bRegenerateAssignedDevicesInProgress) {
		return;
	}
	assert(m_nSelectedTeam >= 0);
	assert(m_refPrefs);
	const shared_ptr<StdPreferences::Player>& refPlayer = m_refPrefs->getTeamFull(m_nSelectedTeam)->getMateFull(m_nSelectedMate);
	auto aPlayerCapabilities = refPlayer->getCapabilities();
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_p0TreeViewAssignedCapabilities->get_selection();
	std::vector<Gtk::TreeModel::Path> aSelectedRows = refTreeSelection->get_selected_rows();
//std::cout << "onAssignedCapabilitiesSelectionChanged()  aSelectedRows.size() " << aSelectedRows.size() << '\n';
	//
	// find the difference between assigned and selected capabilities
	// and try to modify the prefs accordingly
	int32_t nPlayerCapaIdx = aPlayerCapabilities.size() - 1;
	for ( ; nPlayerCapaIdx >= 0; --nPlayerCapaIdx) {
//std::cout << "onAssignedCapabilitiesSelectionChanged()  nPlayerCapaIdx " << nPlayerCapaIdx << '\n';
		shared_ptr<stmi::Capability>& refCapa = aPlayerCapabilities[nPlayerCapaIdx];
		if (!refCapa) {
			continue; // for -----
		}
		if (refCapa->getCapabilityClass() == stmi::PlaybackCapability::getClass()) {
			refCapa.reset();
			continue; // for -----
		}
		const int32_t nCapaId = refCapa->getId();
		const int32_t nTotSelected = aSelectedRows.size();
		for (int32_t nSelIdx = 0; nSelIdx < nTotSelected; ++nSelIdx) {
			Gtk::TreeModel::Path& oSelPath = aSelectedRows[nSelIdx];
			assert(oSelPath.size() == 1);
			const int32_t nSelRowIdx = oSelPath[0];
			const Gtk::TreeModel::Row& oRow = m_refTreeModelAssignedCapabilities->children()[nSelRowIdx];
			const int32_t nSelCapaId = oRow[m_oAssignedCapabilitiesColumns.m_oColCapabilityId];
			if (nSelCapaId == nCapaId) {
//std::cout << "onAssignedCapabilitiesSelectionChanged()  selected and assigned nSelCapaId=" << nSelCapaId << '\n';
				// capability selected and already assigned to player
				// remove from both arrays
				aSelectedRows.erase(aSelectedRows.begin() + nSelIdx);
				aPlayerCapabilities.erase(aPlayerCapabilities.begin() + nPlayerCapaIdx);
				//
				break; // for nSelIdx -------
			}
		}
	}
	// aPlayerCapabilities now contains the capabilities that have to be unassigned
	// aSelectedRows contains the capabilities that have to be assigned to the player
	for (Gtk::TreeModel::Path& oSelPath : aSelectedRows) {
		assert(oSelPath.size() == 1);
		const int32_t nRowIdx = oSelPath[0];
//std::cout << "onAssignedCapabilitiesSelectionChanged()  nRowIdx=" << nRowIdx << '\n';
		const Gtk::TreeModel::Row& oRow = m_refTreeModelAssignedCapabilities->children()[nRowIdx];
		const int32_t nDeviceId = oRow[m_oAssignedCapabilitiesColumns.m_oColHiddenDeviceId];
		const int32_t nCapabilityId = oRow[m_oAssignedCapabilitiesColumns.m_oColCapabilityId];
		shared_ptr<stmi::Device> refDevice = m_refDM->getDevice(nDeviceId);
		if (!refDevice) {
//std::cout << "onAssignedCapabilitiesSelectionChanged()  NOT FOUND nDeviceId=" << nDeviceId << '\n';
			continue; // for ---------
		}
		shared_ptr<stmi::Capability> refCapability = refDevice->getCapability(nCapabilityId);
		if (!refCapability) {
//std::cout << "onAssignedCapabilitiesSelectionChanged()  NOT FOUND nCapabilityId=" << nCapabilityId << '\n';
			continue; // for ---------
		}
//std::cout << "onAssignedCapabilitiesSelectionChanged()  assign nCapaId=" << refCapability->getId() << '\n';
		refPlayer->assignCapability(refCapability);
	}
	for (auto& refCapa : aPlayerCapabilities) {
		if (!refCapa) {
			continue;
		}
//std::cout << "onAssignedCapabilitiesSelectionChanged()  unassign nCapaId=" << refCapa->getId() << '\n';
		int32_t nTeam;
		int32_t nMate;
		const bool bAssigned = m_refPrefs->getCapabilityPlayer(refCapa->getId(), nTeam, nMate);
		if (bAssigned && (nTeam == m_nSelectedTeam) && (nMate == m_nSelectedMate)) {
			refPlayer->unassignCapability(refCapa);
		}
	}
	regenerateAssignedDevicesList();
}
bool PlayersScreen::isPerPlayerSound() noexcept
{
	return (m_refPrefs->getOptionValue(m_refStdConfig->getPerPlayerSoundOptionName()) != Variant{false});
}
void PlayersScreen::regenerateSoundDevicesList() noexcept
{
	if (!m_bPrefsInitialized) {
		return;
	}
	if (m_nSelectedMate < 0) {
		// Only per player mode
		return;
	}
	const bool bPerPlayerSound = isPerPlayerSound();
	const bool bIsAI = m_refPrefs->getTeamFull(m_nSelectedTeam)->getMateFull(m_nSelectedMate)->isAI();
	const bool bShowSoundStuff = (m_refStdConfig->soundEnabled() && bPerPlayerSound && ! bIsAI);
//std::cout << "regenerateSoundDevicesList()  bShowSoundStuff = " << bShowSoundStuff << '\n';
	m_p0ScrolledSoundCapabilities->set_visible(bShowSoundStuff);
	m_p0ButtonPlayTestPlayerSound->set_visible(bShowSoundStuff);
	if (! bShowSoundStuff) {
		return; //--------------------------------------------------------------
	}
	const auto refCapa = getSelectedPlayerPlayback();
	m_p0ButtonPlayTestPlayerSound->set_sensitive((m_refTheme.get() != nullptr) && (m_nSoundTestIdx >= 0) && refCapa);

	assert(!m_bRegenerateSoundDevicesInProgress);
	m_bRegenerateSoundDevicesInProgress = true;
	m_refTreeModelSoundCapabilities->clear();
	std::vector<int32_t> aSelectedRows;
	int32_t nRowIdx = 0;
	const auto aDeviceIds = m_refDM->getDevices();
	for (int32_t nDeviceId : aDeviceIds) {
		auto refDevice = m_refDM->getDevice(nDeviceId);
		assert(refDevice);
		const std::string& sDeviceName = refDevice->getName();
//std::cout << "regenerateSoundDevicesList()  sDeviceName " << sDeviceName << "   nDeviceId=" << nDeviceId << '\n';
		const auto aCapabilityIds = refDevice->getCapabilities();
		for (int32_t nCapabilityId : aCapabilityIds) {
			auto refCapability = refDevice->getCapability(nCapabilityId);
//std::cout << "regenerateSoundDevicesList()  nCapabilityId " << nCapabilityId << '\n';
			assert(refCapability);
			const stmi::Capability::Class& oClass = refCapability->getCapabilityClass();
			assert(oClass);
			if (oClass != stmi::PlaybackCapability::getClass()) {
				// non sound capabilities are shown elsewhere
//std::cout << "regenerateSoundDevicesList()  No PLAYBACK" << '\n';
				continue;
			}
			int32_t nTeam;
			int32_t nMate;
			const bool bAssigned = m_refPrefs->getCapabilityPlayer(refCapability->getId(), nTeam, nMate);
			std::string sPlayerName = (bAssigned ? m_refPrefs->getTeamFull(nTeam)->getMateFull(nMate)->getName() : "");
			if (bAssigned && (nTeam == m_nSelectedTeam) && (nMate == m_nSelectedMate)) {
				aSelectedRows.push_back(nRowIdx);
			}
			//
			Gtk::TreeModel::Row oRow;
			oRow = *(m_refTreeModelSoundCapabilities->append());
			oRow[m_oSoundCapabilitiesColumns.m_oColDeviceName] = sDeviceName;
			oRow[m_oSoundCapabilitiesColumns.m_oColSoundToPlayerName] = sPlayerName;
			oRow[m_oSoundCapabilitiesColumns.m_oColHiddenDeviceId] = nDeviceId;
			oRow[m_oSoundCapabilitiesColumns.m_oColCapabilityId] = nCapabilityId;
			++nRowIdx;
		}
	}
	m_p0TreeViewSoundCapabilities->expand_all();
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_p0TreeViewSoundCapabilities->get_selection();
	Gtk::TreeModel::Path oPath;
	for (const auto& nRowIdx : aSelectedRows) {
//std::cout << "regenerateSoundDevicesList()  select row " << nRowIdx << '\n';
		//oPath.clear();
		oPath.push_back(nRowIdx);
		refTreeSelection->select(oPath);
	}
	m_bRegenerateSoundDevicesInProgress = false;
}
void PlayersScreen::onSoundCapabilitiesSelectionChanged() noexcept
{
	if (m_nSelectedMate < 0) {
		return;
	}
	if (m_bRegenerateSoundDevicesInProgress) {
		return;
	}
	assert(m_nSelectedTeam >= 0);
	assert(m_refPrefs);
	const shared_ptr<StdPreferences::Player>& refPlayer = m_refPrefs->getTeamFull(m_nSelectedTeam)->getMateFull(m_nSelectedMate);
	auto aPlayerCapabilities = refPlayer->getCapabilities();
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_p0TreeViewSoundCapabilities->get_selection();
	std::vector<Gtk::TreeModel::Path> aSelectedRows = refTreeSelection->get_selected_rows();
	assert(aSelectedRows.size() <= 1);
//std::cout << "onSoundCapabilitiesSelectionChanged()  aSelectedRows.size() " << aSelectedRows.size() << '\n';
	//
	// find the difference between assigned and selected capabilities
	// and try to modify the prefs accordingly
	int32_t nPlayerCapaIdx = aPlayerCapabilities.size() - 1;
	for ( ; nPlayerCapaIdx >= 0; --nPlayerCapaIdx) {
//std::cout << "onSoundCapabilitiesSelectionChanged()  nPlayerCapaIdx " << nPlayerCapaIdx << '\n';
		shared_ptr<stmi::Capability>& refCapa = aPlayerCapabilities[nPlayerCapaIdx];
		if (!refCapa) {
			continue; // for -----
		}
		if (!(refCapa->getCapabilityClass() == stmi::PlaybackCapability::getClass())) {
			refCapa.reset();
			continue; // for -----
		}
		const int32_t nCapaId = refCapa->getId();
		if (! aSelectedRows.empty()) {
			Gtk::TreeModel::Path& oSelPath = aSelectedRows[0];
			assert(oSelPath.size() == 1);
			const int32_t nSelRowIdx = oSelPath[0];
			const Gtk::TreeModel::Row& oRow = m_refTreeModelSoundCapabilities->children()[nSelRowIdx];
			const int32_t nSelCapaId = oRow[m_oSoundCapabilitiesColumns.m_oColCapabilityId];
			if (nSelCapaId == nCapaId) {
//std::cout << "onSoundCapabilitiesSelectionChanged()  selected and assigned nSelCapaId=" << nSelCapaId << '\n';
				// capability selected and already assigned to player
				// remove from both arrays
				aSelectedRows.clear();
				aPlayerCapabilities.erase(aPlayerCapabilities.begin() + nPlayerCapaIdx);
				//
				break; // for nSelIdx -------
			}
		}
	}
	// aPlayerCapabilities contains the capabilities that have to be unassigned
	// aSelectedRows contains the capabilities that have to be assigned to the player
	if (! aSelectedRows.empty()) {
		Gtk::TreeModel::Path& oSelPath = aSelectedRows[0];
		assert(oSelPath.size() == 1);
		const int32_t nRowIdx = oSelPath[0];
		const Gtk::TreeModel::Row& oRow = m_refTreeModelSoundCapabilities->children()[nRowIdx];
		const int32_t nDeviceId = oRow[m_oSoundCapabilitiesColumns.m_oColHiddenDeviceId];
		const int32_t nCapabilityId = oRow[m_oSoundCapabilitiesColumns.m_oColCapabilityId];
		shared_ptr<stmi::Device> refDevice = m_refDM->getDevice(nDeviceId);
		if (refDevice) {
			shared_ptr<stmi::Capability> refCapability = refDevice->getCapability(nCapabilityId);
			if (refCapability) {
				refPlayer->assignCapability(refCapability);
			}
		}
	}
	for (auto& refCapa : aPlayerCapabilities) {
		if (!refCapa) {
			continue;
		}
		int32_t nTeam;
		int32_t nMate;
		const bool bAssigned = m_refPrefs->getCapabilityPlayer(refCapa->getId(), nTeam, nMate);
		if (bAssigned && (nTeam == m_nSelectedTeam) && (nMate == m_nSelectedMate)) {
			refPlayer->unassignCapability(refCapa);
		}
	}
	regenerateSoundDevicesList();
}
void PlayersScreen::refreshPlayTestGlobalSound() noexcept
{
	const bool bPerPlayerSound = isPerPlayerSound();
	const bool bTestGlobalSoundVisible = m_refStdConfig->soundEnabled() && ! bPerPlayerSound;
	m_p0ButtonPlayTestGlobalSound->set_visible(bTestGlobalSoundVisible);
	if (bTestGlobalSoundVisible) {
		m_p0ButtonPlayTestGlobalSound->set_sensitive(m_nSoundTestIdx >= 0);
	}
}
void PlayersScreen::onNotebookSwitchPage(Gtk::Widget*, guint /*nPageNum*/) noexcept
{
//std::cout << "onNotebookSwitchPage  nPageNum=" << nPageNum << '\n';
	if (!m_bPrefsInitialized) {
		return;
	}
	const int32_t nCurPage = m_p0NotebookPlayers->get_current_page();
//std::cout << "onNotebookSwitchPage() nCurPage=" << nCurPage << '\n';
	if (m_bEditingTeamOrPlayer) {
		m_bEditingTeamOrPlayer = false;
		onChangedEditingMode();
	}
	if (nCurPage == m_aPageIndex[s_nTabGameOptions]) {
		assert(m_bShowGameTab);
		regenerateGameOptions();
		refreshPlayTestGlobalSound();
	} else if (nCurPage == m_aPageIndex[s_nTabTeamOptions]) {
		assert(m_bShowTeamTab);
		if (m_nSelectedTeam < 0) {
			assert(m_nSelectedMate < 0);
			m_nSelectedTeam = 0;
			regeneratePlayersModel();
		}
//std::cout << "Team Page  m_nSelectedTeam=" << m_nSelectedTeam << '\n';
		regenerateTeamOptions();

	} else if (nCurPage == m_aPageIndex[s_nTabPlayerOptions]) {
		assert(m_bShowPlayerTab);
		if (m_nSelectedMate < 0) {
			m_nSelectedMate = 0;
			if (m_nSelectedTeam < 0) {
				m_nSelectedTeam = 0;
			}
			regeneratePlayersModel();
		}
//std::cout << "Player Page  m_nSelectedTeam=" << m_nSelectedTeam << "  m_nSelectedMate=" << m_nSelectedMate << '\n';
		regeneratePlayerOptions();
		regenerateSoundDevicesList();

	} else if (nCurPage == m_aPageIndex[s_nTabPlayerDeviceKeyActions]) {
		//
		if (m_nSelectedMate < 0) {
			m_nSelectedMate = 0;
			if (m_nSelectedTeam < 0) {
				m_nSelectedTeam = 0;
			}
			regeneratePlayersModel();
		}

		const bool bIsAI = m_refPrefs->getTeamFull(m_nSelectedTeam)->getMateFull(m_nSelectedMate)->isAI();
		m_p0LabelKeysModifyKey->set_visible(! bIsAI);
		m_p0TreeViewKeys->set_visible(! bIsAI);
		if (! bIsAI) {
			regenerateKeysModel();
		}
		if (m_bShowAssignedCapabilities) {
			m_p0TreeViewAssignedCapabilities->set_visible(! bIsAI);
			if (! bIsAI) {
				regenerateAssignedDevicesList();
			}
		}

	} else if (nCurPage == m_aPageIndex[s_nTabTeamPlayers]) {
		regeneratePlayersModel();
	}
}

void PlayersScreen::onPlayerSelectionChanged() noexcept
{
//std::cout << "onPlayerSelectionChanged()" << '\n';
	if (!m_bPrefsInitialized) {
		return;
	}
	if (m_bRegeneratePlayersInProgress) {
		return;
	}
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_p0TreeViewPlayers->get_selection();
	Gtk::TreeModel::iterator it = refTreeSelection->get_selected();
	if (it)	{
		Gtk::TreeModel::Row oRow = *it;
		const int32_t nTeam = oRow[m_oPlayersColumns.m_oColHiddenTeam];
		const int32_t nMate = oRow[m_oPlayersColumns.m_oColHiddenMate];
		m_nSelectedTeam = nTeam;
		m_nSelectedMate = nMate;
//std::cout << "onPlayerSelectionChanged() m_nSelectedTeam=" << m_nSelectedTeam << "  m_nSelectedMate=" << m_nSelectedMate << '\n';
	} else {
//std::cout << "onPlayerSelectionChanged() no selection" << '\n';
		m_nSelectedTeam = -1;
		m_nSelectedMate = -1;
	}
	m_bEditingTeamOrPlayer = false;
	const bool bSomethingSelected = (m_nSelectedTeam >= 0);
	m_p0ButtonRename->set_sensitive(bSomethingSelected);
	onChangedEditingMode();
}
void PlayersScreen::onChangedEditingMode() noexcept
{
	m_p0ButtonRename->set_label(m_bEditingTeamOrPlayer ? s_sPlayerOrTeamRenameDoIt : s_sPlayerOrTeamRenameEllipsize);
	m_p0HBoxRename->set_visible(m_bEditingTeamOrPlayer);
	if (! m_bAlwaysOnePlayer) {
		m_p0HBoxNrPlayersTeams->set_sensitive(! m_bEditingTeamOrPlayer);
	}
	m_p0ButtonMoveUp->set_sensitive(! m_bEditingTeamOrPlayer);
	m_p0ButtonMoveDown->set_sensitive(! m_bEditingTeamOrPlayer);
	m_p0ButtonTeamUp->set_sensitive(! m_bEditingTeamOrPlayer);
	m_p0ButtonTeamDown->set_sensitive(! m_bEditingTeamOrPlayer);
}
void PlayersScreen::onSpinNrPlayersChanged() noexcept
{
	if (!m_bPrefsInitialized) {
		return;
	}

	const int32_t nTotPlayers = m_p0SpinNrPlayers->get_value_as_int();
	const int32_t nMaxPlayers = m_refStdConfig->getAppConstraints().getMaxPlayers();
	if (nTotPlayers > nMaxPlayers) {
		m_p0SpinNrPlayers->set_value(nMaxPlayers);
		return;
	}
	m_refPrefs->setTotPlayers(nTotPlayers);

	regeneratePlayersModel();
}
void PlayersScreen::onSpinNrTeamsChanged() noexcept
{
	if (!m_bPrefsInitialized) {
		return;
	}

	const int32_t nTotTeams = m_p0SpinNrTeams->get_value_as_int();
	const int32_t nMaxTeams = m_refPrefs->getStdConfig()->getAppConstraints().getMaxTeams();
	if (nTotTeams > nMaxTeams) {
		m_p0SpinNrTeams->set_value(nMaxTeams);
		return;
	}
	m_refPrefs->setTotTeams(nTotTeams);

	regeneratePlayersModel();
}
void PlayersScreen::onButtonRename() noexcept
{
//std::cout << "---" << '\n';
	if (m_nSelectedTeam < 0) {
		m_p0HBoxRename->set_visible(false);
		return;
	}
	if (m_bEditingTeamOrPlayer) {
		m_bEditingTeamOrPlayer = false;
		Glib::ustring sName = m_p0EntryRename->get_text();
		if (m_nSelectedMate >= 0) {
			onPlayerNameChanged(sName);
		} else {
			onTeamNameChanged(sName);
		}
		onChangedEditingMode();
		regeneratePlayersModel();
		return; //--------------------------------------------------------------
	}
	m_bEditingTeamOrPlayer = true;
	onChangedEditingMode();
	const bool bPlayerSelected = (m_nSelectedMate >= 0);
	const int32_t nMaxTeamNameLength = m_refPrefs->getMaxTeamNameLength();
	const int32_t nMaxPlayerNameLength = m_refPrefs->getMaxPlayerNameLength();
	m_p0EntryRename->set_max_length(bPlayerSelected ? nMaxPlayerNameLength : nMaxTeamNameLength);
	m_p0LabelRename->set_text(bPlayerSelected ? "Player:" : "Team:");
	const Glib::ustring sName = [&]() -> Glib::ustring
	{
		auto& refTeam = m_refPrefs->getTeamFull(m_nSelectedTeam);
		if (m_nSelectedMate >= 0) {
			auto& refPlayer = refTeam->getMateFull(m_nSelectedMate);
			return refPlayer->getName();
		}
		return refTeam->getName();
	}();
	m_p0EntryRename->set_text(sName);
	m_p0EntryRename->grab_focus();
}
void PlayersScreen::onButtonMoveUp() noexcept
{
	if (m_nSelectedMate == -1) {
		return;
	}
	assert(m_nSelectedTeam >= 0);
	auto& refSelectedTeam = m_refPrefs->getTeamFull(m_nSelectedTeam);
	auto refSelectedMate = refSelectedTeam->getMateFull(m_nSelectedMate);
	m_refPrefs->playerMoveUp(refSelectedMate->getName());
	m_nSelectedMate = refSelectedMate->getMate();
	m_nSelectedTeam = refSelectedMate->getTeamFull()->get();

	regeneratePlayersModel();
}
void PlayersScreen::onButtonMoveDown() noexcept
{
	if (m_nSelectedMate == -1) {
		return;
	}
	assert(m_nSelectedTeam >= 0);
	auto& refSelectedTeam = m_refPrefs->getTeamFull(m_nSelectedTeam);
	auto refSelectedMate = refSelectedTeam->getMateFull(m_nSelectedMate);
	m_refPrefs->playerMoveDown(refSelectedMate->getName());
	m_nSelectedMate = refSelectedMate->getMate();
	m_nSelectedTeam = refSelectedMate->getTeamFull()->get();

	regeneratePlayersModel();
}
void PlayersScreen::onButtonTeamUp() noexcept
{
	if (m_nSelectedMate == -1) {
		return;
	}
	assert(m_nSelectedTeam >= 0);
	auto& refSelectedTeam = m_refPrefs->getTeamFull(m_nSelectedTeam);
	auto refSelectedMate = refSelectedTeam->getMateFull(m_nSelectedMate);
	m_refPrefs->playerTeamUp(refSelectedMate->getName());
	m_nSelectedMate = refSelectedMate->getMate();
	m_nSelectedTeam = refSelectedMate->getTeamFull()->get();

	regeneratePlayersModel();
}
void PlayersScreen::onButtonTeamDown() noexcept
{
	if (m_nSelectedMate == -1) {
		return;
	}
	assert(m_nSelectedTeam >= 0);
	auto& refSelectedTeam = m_refPrefs->getTeamFull(m_nSelectedTeam);
	auto refSelectedMate = refSelectedTeam->getMateFull(m_nSelectedMate);
	m_refPrefs->playerTeamDown(refSelectedMate->getName());
	m_nSelectedMate = refSelectedMate->getMate();
	m_nSelectedTeam = refSelectedMate->getTeamFull()->get();

	regeneratePlayersModel();
}
const shared_ptr<StdPreferences::Player>& PlayersScreen::getSelectedPlayer() const noexcept
{
	assert(m_nSelectedMate >= 0);
	auto& refPlayer = m_refPrefs->getTeamFull(m_nSelectedTeam)->getMateFull(m_nSelectedMate);
	return refPlayer;
}
const shared_ptr<stmi::Capability> PlayersScreen::getSelectedPlayerPlayback() const noexcept
{
	return getPlayerPlayback(getSelectedPlayer());
}
const shared_ptr<stmi::Capability> PlayersScreen::getPlayerPlayback(const shared_ptr<StdPreferences::Player>& refPlayer) const noexcept
{
	std::vector<shared_ptr<stmi::Capability>> aCapas = refPlayer->getCapabilities();
	const auto itFind = std::find_if(aCapas.begin(), aCapas.end(), [&](const shared_ptr<stmi::Capability>& refCapa)
	{
		return (refCapa->getCapabilityClass() == stmi::PlaybackCapability::getClass());
	});
	if (itFind == aCapas.end()) {
		return shared_ptr<stmi::Capability>{}; //--------------------------------
	}
	return *itFind;
}
void PlayersScreen::onButtonPlayTestPlayerSound() noexcept
{
	if (m_nSelectedMate < 0) {
		return; //--------------------------------------------------------------
	}
	if (m_nSoundTestIdx < 0) {
		return; //--------------------------------------------------------------
	}
	if (! m_refTheme) {
		return; //--------------------------------------------------------------
	}
	const auto& refPlayer = getSelectedPlayer();
	const auto refCapa = getPlayerPlayback(refPlayer);
	if (! refCapa) {
		return; //--------------------------------------------------------------
	}
	const double fVolume = 0.01 * refPlayer->getOptionValue(m_refStdConfig->getSoundVolumeOptionName()).getInt();
	onButtonPlayTestSound(refCapa, fVolume);
}
void PlayersScreen::onButtonPlayTestGlobalSound() noexcept
{
	if (m_nSoundTestIdx < 0) {
		return; //--------------------------------------------------------------
	}
	if (! m_refTheme) {
		return; //--------------------------------------------------------------
	}
	const auto refCapa = m_refStdConfig->getDefaultPlayback();
	if (! refCapa) {
		return; //--------------------------------------------------------------
	}
	const double fVolume = 0.01 * m_refPrefs->getOptionValue(m_refStdConfig->getSoundVolumeOptionName()).getInt();
	onButtonPlayTestSound(refCapa, fVolume);
}
void PlayersScreen::onButtonPlayTestSound(const shared_ptr<stmi::Capability>& refCapa, double fVolume) noexcept
{
	auto refPlayback = std::static_pointer_cast<stmi::PlaybackCapability>(refCapa);
	refPlayback->setListenerVol(fVolume);

	auto refThemeCtx = m_refTheme->createContext(NSize{10,10}, false, 1.0, 1.0, 1.0, m_oGameWindow.get_pango_context());
	auto refThemeSound = refThemeCtx->createSound(m_nSoundTestIdx, {refPlayback}, FPoint{}, 0.0, true, 1.0, false);
	if (! refThemeSound) {
		std::cout << "Couldn't play test sound '" << s_sTestSoundName << "'" << '\n';
	}
}

bool PlayersScreen::onPlayerNameChanged(const Glib::ustring& sPlayerName) noexcept
{
//std::cout << "PlayersScreen::onPlayerNameChanged()  sPlayerName=" << sPlayerName << '\n';
	auto& refTeam = m_refPrefs->getTeamFull(m_nSelectedTeam);
	auto& refPlayer = refTeam->getMateFull(m_nSelectedMate);
	if (sPlayerName.empty() || (static_cast<int32_t>(sPlayerName.size()) > m_refPrefs->getMaxPlayerNameLength())) {
		m_p0EntryRename->set_text(refPlayer->getName());
		return true; //---------------------------------------------------------
	}
	const bool bDone = refPlayer->setName(sPlayerName);
//std::cout << "PlayersScreen::onPlayerNameChanged  bDone=" << bDone << '\n';
	if (!bDone) {
		m_p0EntryRename->set_text(refPlayer->getName());
		return true; //---------------------------------------------------------
	}
	if (!m_bShowTeamTreeNode) {
		// The team automatically takes the name of the first human player
		auto& refTeam = refPlayer->getTeamFull();
		refTeam->setName(sPlayerName);
	}
	return true;
}
bool PlayersScreen::onTeamNameChanged(const Glib::ustring& sTeamName) noexcept
{
//std::cout << "PlayersScreen::onTeamNameChanged()  sTeamName=" << sTeamName << '\n';
	auto& refTeam = m_refPrefs->getTeamFull(m_nSelectedTeam);
	if (sTeamName.empty() || (static_cast<int32_t>(sTeamName.size()) > m_refPrefs->getMaxTeamNameLength())) {
		m_p0EntryRename->set_text(refTeam->getName());
		return true; //---------------------------------------------------------
	}
	const bool bDone = refTeam->setName(sTeamName);
	if (!bDone) {
		m_p0EntryRename->set_text(refTeam->getName());
		return true; //---------------------------------------------------------
	}
	return true;
}
void PlayersScreen::regenerateKeyActionRow(const std::string& sDesc, int32_t nKeyAction) noexcept
{
	Gtk::TreeModel::Row oRow;
	oRow = *(m_refTreeModelKeys->append());
	oRow[m_oKeysColumns.m_oColIndex] = nKeyAction;
	oRow[m_oKeysColumns.m_oColAction] = sDesc;
	auto& refPlayer = m_refPrefs->getTeamFull(m_nSelectedTeam)->getMateFull(m_nSelectedMate);
	const auto oPair = refPlayer->getKeyValue(nKeyAction);
	stmi::Capability* const& p0Capability = oPair.first;
	if (p0Capability == nullptr) {
		return; //--------------------------------------------------------------
	}
	const stmi::HARDWARE_KEY& eKey = oPair.second;
	auto refDevice = p0Capability->getDevice();
	const std::string sCapaClassId = getCapabilityClassId(p0Capability->getCapabilityClass());
	oRow[m_oKeysColumns.m_oColDeviceName] = (refDevice ? refDevice->getName() : "");
	oRow[m_oKeysColumns.m_oColCapabilityClassId] = sCapaClassId;
	oRow[m_oKeysColumns.m_oColCapabilityId] = p0Capability->getId();
	oRow[m_oKeysColumns.m_oColKey] = hkToString(eKey);
}
void PlayersScreen::regenerateKeysModel() noexcept
{
	if (m_nSelectedMate < 0) {
		return;
	}

	m_refTreeModelKeys->clear();

	const int32_t nTotKeyActions = m_refStdConfig->getTotKeyActions();
	for (int32_t nKeyActionId = 0; nKeyActionId < nTotKeyActions; ++nKeyActionId) {
		const auto& sDesc = m_refStdConfig->getKeyAction(nKeyActionId).getDescription();
		regenerateKeyActionRow(sDesc, nKeyActionId);
	}
}

void PlayersScreen::onKeySelectionChanged() noexcept
{
	if (!m_bPrefsInitialized) {
		return;
	}
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_p0TreeViewKeys->get_selection();
	Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
	if (iter)
	{
		Gtk::TreeModel::Row oRow = *iter;
		m_nSelectedKeyAction = oRow[m_oKeysColumns.m_oColIndex];
	} else {
		m_nSelectedKeyAction = -1;
	}
//std::cout << ""; if (m_bExpectingKey) {
//std::cout << "STOP expecting key" << '\n';
//std::cout << ""; }
	m_bExpectingKey = false;
	m_p0LabelKeysModifyKey->set_label(m_sLabelKeysModifyKey);
}
void PlayersScreen::onStmiEvent(const shared_ptr<stmi::Event>& refEvent) noexcept
{
//std::cout << "onStmiEvent() " << refEvent->getEventClass().getId() << '\n';
	const bool bDeviceMgmt = refEvent->getCapability()->getCapabilityClass().isDeviceManagerCapability();
	if (bDeviceMgmt) {
		Glib::signal_timeout().connect(sigc::mem_fun(*this, &PlayersScreen::onDevicesTimeout), 0);
		// Since we don't know whether the m_refPrefs callback is called before
		// this method by the device manager we update the device list later
	}
//std::cout << "onStmiEvent() 1" << '\n';
	if (m_nSelectedKeyAction < 0) {
		return; //--------------------------------------------------------------
	}
	stmi::HARDWARE_KEY eKey;
	stmi::Event::AS_KEY_INPUT_TYPE eType;
	bool bMore;
	const bool bOk = refEvent->getAsKey(eKey, eType, bMore);
	if ((!bOk) || bMore || (eType != stmi::Event::AS_KEY_PRESS)) {
		return; //--------------------------------------------------------------
	}
//std::cout << "onStmiEvent() 3  m_bListenToKey = " << m_bListenToKey << "  m_bExpectingKey=" << m_bExpectingKey << '\n';
	if (!m_bExpectingKey) {
		if (!m_bListenToKey) {
			return; //----------------------------------------------------------
		}
		if (eKey != s_eModifyHK) {
			return; //----------------------------------------------------------
		}
//std::cout << "START expecting key" << '\n';
		m_p0LabelKeysModifyKey->set_label("Please press a key or button (pointing here)");
		m_bExpectingKey = true;
		return; //--------------------------------------------------------------
	}
	assert(m_bListenToKey);
	const int32_t nKeyActionsPage = m_aPageIndex[s_nTabPlayerDeviceKeyActions];
	if (nKeyActionsPage == -1) {
		return; //--------------------------------------------------------------
	}
	const int32_t nCurPage = m_p0NotebookPlayers->get_current_page();
	if (nCurPage != nKeyActionsPage) {
		return; //--------------------------------------------------------------
	}
	assert(m_nSelectedTeam >= 0);
	assert(m_nSelectedMate >= 0);
	auto& refPlayer = m_refPrefs->getTeamFull(m_nSelectedTeam)->getMateFull(m_nSelectedMate);
	refPlayer->setKeyValue(m_nSelectedKeyAction, &(*(refEvent->getCapability())), eKey);
//std::cout << "STOP expecting key" << '\n';
	m_bExpectingKey = false;
	m_p0LabelKeysModifyKey->set_label(m_sLabelKeysModifyKey);
	regenerateKeysModel();
}
bool PlayersScreen::on_key_press_event(GdkEventKey* p0Event) noexcept
{
	// allows to get cursor, tab, control keys if m_bExpectingKey is true
	if (m_bExpectingKey) {
		return false; // propagate to device manager
	}
	if (m_bEditingTeamOrPlayer && m_p0EntryRename->is_focus() && m_p0EntryRename->has_focus()
			&& (p0Event->keyval == GDK_KEY_Return) && ((p0Event->state & GDK_MODIFIER_MASK) == 0)) {
		onButtonRename();
		return true; // don't propagate to device manager
	}
	// standard: tab, control and so on are used by Gtk
	return (&m_oGameWindow)->Gtk::Window::on_key_press_event(p0Event);
}
bool PlayersScreen::on_button_press_event(GdkEventButton* p0Event) noexcept
{
	if (m_bExpectingKey) {
		// allow button press to get to the device manager as a key
		return false;
	}
	return (&m_oGameWindow)->Gtk::Window::on_button_press_event(p0Event);
}

static const InputStrings s_oInputStrings{};

Glib::ustring PlayersScreen::hkToString(stmi::HARDWARE_KEY eKey) noexcept
{
	if (eKey == stmi::HK_NULL) {
		return "Undefined";
	}
	std::string sStr = s_oInputStrings.getKeyString(eKey);
	if (sStr.empty()) {
		return Glib::ustring::compose("No name (%1)", std::to_string(static_cast<int32_t>(eKey)));
	}
	return sStr;
}

} // namespace stmg
