/*
 * File:  playersscreen.h
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

#ifndef STMG_PLAYERS_SCREEN_H
#define STMG_PLAYERS_SCREEN_H

#include <stmm-games/ownertype.h>
#include <stmm-games/util/variant.h>
#include <stmm-games/stdconfig.h>
#include <stmm-games/stdpreferences.h>

#include <stmm-input/capability.h>
#include <stmm-input/devicemanager.h>
#include <stmm-input/hardwarekey.h>

#include <gtkmm.h>

#include <vector>
#include <cassert>
#include <functional>
#include <memory>
#include <string>

#include <stdint.h>

namespace stmg { class BoolOption; }
namespace stmg { class EnumOption; }
namespace stmg { class IntOption; }
namespace stmg { class Option; }
namespace stmg { class Theme; }
namespace stmg { template <class T> class NamedObjIndex; }
namespace stmi { class Event; }
namespace stmg { class GameWindow; }
namespace stmg { class AllPreferences; }

namespace stmg
{

using std::shared_ptr;

class PlayersScreen
{
public:
	PlayersScreen(GameWindow& oGameWindow, const shared_ptr<StdConfig>& refStdConfig) noexcept;

	// returns widget to add to container
	Gtk::Widget* init() noexcept;

	// returns whether could change to screen
	bool changeTo(const shared_ptr<AllPreferences>& refPrefs, const shared_ptr<Theme>& refTheme) noexcept;

	bool on_key_press_event(GdkEventKey* p0Event) noexcept;
	bool on_button_press_event(GdkEventButton* p0Event) noexcept;

private:
	/* Creates (composite) widgets out of options and adds them to a box.
	 * @param oOptions The options.
	 * @param aWidgetSetters The widget setters.
	 * @param eOptionOwner The owner type.
	 * @param oBox The box the widgets should be added to.
	 */
	void addOptions(const NamedObjIndex< shared_ptr<Option> >& oOptions, std::vector< std::function<void()> >& aWidgetSetters
					, OwnerType eOptionOwner, Gtk::Box* p0Box) noexcept;
	void createIntOptionWidget(IntOption* p0IntOption, OwnerType eOptionOwner, bool bReadOnly
								, Gtk::Box* p0ContainerBox, std::function<void()>& oWidgetSetter) noexcept;
	void createBoolOptionWidget(BoolOption* p0BoolOption, OwnerType eOptionOwner, bool bReadOnly
								, Gtk::Box* p0ContainerBox, std::function<void()>& oWidgetSetter) noexcept;
	void createEnumOptionWidget(EnumOption* p0EnumOption, OwnerType eOptionOwner, bool bReadOnly
								, Gtk::Box* p0ContainerBox, std::function<void()>& oWidgetSetter) noexcept;
	Variant getOptionValue(OwnerType eOptionOwner, const std::string& sOptionName) noexcept;
	bool isSlaveOptionVisible(Option* p0SlaveOption) noexcept;

	//Signal handlers:
	void onIntOptionChanged(OwnerType eOptionOwner, IntOption* p0IntOption, Gtk::SpinButton* p0SB) noexcept;
	void onBoolOptionChanged(OwnerType eOptionOwner, BoolOption* p0BoolOption, Gtk::CheckButton* p0CB) noexcept;
	void onEnumOptionChanged(OwnerType eOptionOwner, EnumOption* p0EnumOption, Gtk::RadioButton* p0RB, int32_t nEnumIdx) noexcept;
	void onButtonOk() noexcept;
	void onButtonCancel() noexcept;
	void onNotebookSwitchPage(Gtk::Widget*, guint nPageNum) noexcept;
	void onSpinNrPlayersChanged() noexcept;
	void onSpinNrTeamsChanged() noexcept;
	void onPlayerSelectionChanged() noexcept;
	void onButtonRename() noexcept;
	void onButtonMoveUp() noexcept;
	void onButtonMoveDown() noexcept;
	void onButtonTeamUp() noexcept;
	void onButtonTeamDown() noexcept;
	void onChangedEditingMode() noexcept;
	bool onPlayerNameChanged(const Glib::ustring& sPlayerName) noexcept;
	bool onTeamNameChanged(const Glib::ustring& sPlayerName) noexcept;

	void onKeySelectionChanged() noexcept;
	void onStmiEvent(const shared_ptr<stmi::Event>& refEvent) noexcept;
	bool onDevicesTimeout() noexcept;
	//
	void onAssignedCapabilitiesSelectionChanged() noexcept;
	void onSoundCapabilitiesSelectionChanged() noexcept;

	void setOptionValue(OwnerType eOptionOwner, const std::string& sOptionName
						, const Variant& oVar) noexcept;

	void changeScreen(int32_t nToScreen, const std::string& sMsg) noexcept;
	void onButtonPlayersInfoOk() noexcept;

	const shared_ptr<StdPreferences::Player>& getSelectedPlayer() const noexcept;
	const shared_ptr<stmi::Capability> getSelectedPlayerPlayback() const noexcept;
	const shared_ptr<stmi::Capability> getPlayerPlayback(const shared_ptr<StdPreferences::Player>& refPlayer) const noexcept;
	void onButtonPlayTestPlayerSound() noexcept;
	void onButtonPlayTestGlobalSound() noexcept;
	void onButtonPlayTestSound(const shared_ptr<stmi::Capability>& refCapa, double fVolume) noexcept;
	bool isPerPlayerSound() noexcept;

	void regeneratePlayersModel() noexcept;
	void regenerateKeysModel() noexcept;
	static Glib::ustring hkToString(stmi::HARDWARE_KEY eKey) noexcept;
	void regenerateKeyActionRow(const std::string& sDesc, int32_t nKeyAction) noexcept;
	void regenerateAssignedDevicesList() noexcept;
	void regenerateGameOptions() noexcept;
	void regenerateTeamOptions() noexcept;
	void regeneratePlayerOptions() noexcept;
	void regenerateSoundDevicesList() noexcept;
	void refreshPlayTestGlobalSound() noexcept;

	std::string getCapabilityClassId(const stmi::Capability::Class& oClass) const noexcept;
private:
	GameWindow& m_oGameWindow;

	class KeysNotebook : public Gtk::Notebook
	{
	public:
		KeysNotebook() = delete;
		explicit KeysNotebook(PlayersScreen* p0Dialog) noexcept;
	protected:
		bool on_button_press_event(GdkEventButton* p0Event) override;
	private:
		PlayersScreen* m_p0Dialog;
	};
	friend class KeysNotebook;
	class KeysTreeView : public Gtk::TreeView
	{
	public:
		KeysTreeView() = delete;
		KeysTreeView(PlayersScreen* p0Dialog, const Glib::RefPtr< Gtk::TreeModel >& refModel) noexcept;
	protected:
		bool on_key_press_event(GdkEventKey* p0Event) override;
		bool on_button_press_event(GdkEventButton* p0Event) override;
		bool on_focus_in_event(GdkEventFocus* p0Event) override;
		bool on_focus_out_event(GdkEventFocus* p0Event) override;
	private:
		PlayersScreen* m_p0Dialog;
	};
	friend class KeysTreeView;

	// Only keep a pointer to the following widgets if they are
	// used past the constructor!
	Gtk::Box* m_p0PlayersScreenBoxMain = nullptr;
		//Gtk::Label* m_p0LabelTitle = nullptr;

		KeysNotebook* m_p0NotebookPlayers = nullptr;

			Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentNrPlayers;
			Glib::RefPtr<Gtk::Adjustment> m_refAdjustmentNrTeams;

			static const int32_t s_nTabTeamPlayers = 0;
			//Gtk::Label* m_p0TabLabelPlayersTeams = nullptr;
			//Gtk::Box* m_p0TabVBoxPlayersTeams = nullptr;
				Gtk::Box* m_p0HBoxNrPlayersTeams = nullptr;
					//Gtk::Box* m_p0HBoxNrPlayers = nullptr;
						//Gtk::Label* m_p0LabelNrPlayers = nullptr;
						Gtk::SpinButton* m_p0SpinNrPlayers = nullptr;
					//Gtk::Box* m_p0HBoxNrTeams = nullptr;
						//Gtk::Label* m_p0LabelNrTeams = nullptr;
						Gtk::SpinButton* m_p0SpinNrTeams = nullptr;
				//Gtk::Box* m_p0HBoxPlayersTeams = nullptr;
					//Gtk::Box* m_p0VBoxPlayers = nullptr;
						Gtk::Box* m_p0HBoxRename = nullptr;
							Gtk::Label* m_p0LabelRename = nullptr;
							Gtk::Entry* m_p0EntryRename = nullptr;
						//Gtk::ScrolledWindow* m_p0ScrolledPlayers = nullptr;
							Gtk::TreeView* m_p0TreeViewPlayers = nullptr;
					//Gtk::Box* m_p0VBoxRenameMovePlayers = nullptr;
						Gtk::Button* m_p0ButtonRename = nullptr;
						Gtk::Box* m_p0VBoxMovePlayers = nullptr;
							Gtk::Button* m_p0ButtonMoveUp = nullptr;
							Gtk::Button* m_p0ButtonMoveDown = nullptr;
							Gtk::Button* m_p0ButtonTeamUp = nullptr;
							Gtk::Button* m_p0ButtonTeamDown = nullptr;
			//
			static const int32_t s_nTabGameOptions = 1;
			//Gtk::Label* m_p0TabLabelGameOptions = nullptr;
			//Gtk::Box* m_p0TabVBoxGameOptions = nullptr;
				//Gtk::Box* m_p0VBoxPGameOptions = nullptr;
					//std::vector< Gtk::Widget* > m_aGameOptions;
				Gtk::Button* m_p0ButtonPlayTestGlobalSound = nullptr;
			//
			static const int32_t s_nTabTeamOptions = 2;
			//Gtk::Label* m_p0TabLabelTeamOptions = nullptr;
			//Gtk::Box* m_p0TabVBoxTeamOptions = nullptr;
				//Gtk::Box* m_p0VBoxTeamOptions = nullptr;
					//std::vector< Gtk::Widget* > m_aTeamOptions;
			//
			static const int32_t s_nTabPlayerOptions = 3;
			//Gtk::Label* m_p0TabLabelPlayerOptions = nullptr;
			//Gtk::Box* m_p0TabVBoxPlayerOptions = nullptr;
				//Gtk::Box* m_p0VBoxPlayerOptions = nullptr;
					//std::vector< Gtk::Widget* > m_aPlayerOptions;
				Gtk::ScrolledWindow* m_p0ScrolledSoundCapabilities = nullptr;
					Gtk::TreeView* m_p0TreeViewSoundCapabilities = nullptr;
				Gtk::Button* m_p0ButtonPlayTestPlayerSound = nullptr;
			//
			static const int32_t s_nTabPlayerDeviceKeyActions = 4;
			//Gtk::Label* m_p0TabLabelPlayerDeviceKeyActions = nullptr;
			//Gtk::Box* m_p0TabVBoxPlayerDeviceKeyActions = nullptr;
				//Gtk::Box* m_p0VBoxKeyActionsKeys = nullptr;
					Gtk::Label* m_p0LabelKeysModifyKey = nullptr;
					const Glib::ustring m_sLabelKeysModifyKey = nullptr;
					//Gtk::ScrolledWindow* m_p0ScrolledKeys = nullptr;
						KeysTreeView* m_p0TreeViewKeys = nullptr;
				//Gtk::ScrolledWindow* m_p0ScrolledAssignedCapabilities = nullptr;
					Gtk::TreeView* m_p0TreeViewAssignedCapabilities = nullptr;

		//Gtk::Button* m_p0ButtonOk = nullptr;
		//Gtk::Button* m_p0ButtonCancel = nullptr;

	// the vector index is the option index
	std::vector< std::function<void()> > m_aGameOptionWidgetSetters;
	std::vector< std::function<void()> > m_aTeamOptionWidgetSetters;
	std::vector< std::function<void()> > m_aPlayerOptionWidgetSetters;

	std::vector<Glib::RefPtr<Gtk::Adjustment> > m_aAdjustments;

	int32_t m_aPageIndex[5];

	class PlayersColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		PlayersColumns() noexcept { add(m_oColPlayer); add(m_oColHiddenTeam); add(m_oColHiddenMate); }
		Gtk::TreeModelColumn<Glib::ustring> m_oColPlayer;
		Gtk::TreeModelColumn<int32_t> m_oColHiddenTeam;
		Gtk::TreeModelColumn<int32_t> m_oColHiddenMate;
	};
	PlayersColumns m_oPlayersColumns;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModelPlayers;

	class KeysColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		KeysColumns() noexcept
		{
			add(m_oColIndex); add(m_oColAction); add(m_oColKey);
			add(m_oColDeviceName); add(m_oColCapabilityClassId); add(m_oColCapabilityId);
		}
		Gtk::TreeModelColumn<int32_t> m_oColIndex;
		Gtk::TreeModelColumn<Glib::ustring> m_oColAction;
		Gtk::TreeModelColumn<Glib::ustring> m_oColKey;
		Gtk::TreeModelColumn<Glib::ustring> m_oColDeviceName;
		Gtk::TreeModelColumn<Glib::ustring> m_oColCapabilityClassId;
		Gtk::TreeModelColumn<int32_t> m_oColCapabilityId;
	};
	KeysColumns m_oKeysColumns;
	Glib::RefPtr<Gtk::ListStore> m_refTreeModelKeys;

	class AssignedCapabilitiesColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		AssignedCapabilitiesColumns() noexcept
		{
			add(m_oColDeviceName); add(m_oColCapabilityClassId); add(m_oColCapabilityId);
			add(m_oColAssignedToPlayerName); add(m_oColHiddenDeviceId);
		}
		Gtk::TreeModelColumn<Glib::ustring> m_oColDeviceName;
		Gtk::TreeModelColumn<Glib::ustring> m_oColCapabilityClassId;
		Gtk::TreeModelColumn<int32_t> m_oColCapabilityId;
		Gtk::TreeModelColumn<Glib::ustring> m_oColAssignedToPlayerName;
		Gtk::TreeModelColumn<int32_t> m_oColHiddenDeviceId;
	};
	AssignedCapabilitiesColumns m_oAssignedCapabilitiesColumns;
	Glib::RefPtr<Gtk::ListStore> m_refTreeModelAssignedCapabilities;

	class SoundCapabilitiesColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		SoundCapabilitiesColumns() noexcept
		{
			add(m_oColDeviceName); add(m_oColCapabilityId);
			add(m_oColSoundToPlayerName); add(m_oColHiddenDeviceId);
		}
		Gtk::TreeModelColumn<Glib::ustring> m_oColDeviceName;
		Gtk::TreeModelColumn<int32_t> m_oColCapabilityId;
		Gtk::TreeModelColumn<Glib::ustring> m_oColSoundToPlayerName;
		Gtk::TreeModelColumn<int32_t> m_oColHiddenDeviceId;
	};
	SoundCapabilitiesColumns m_oSoundCapabilitiesColumns;
	Glib::RefPtr<Gtk::ListStore> m_refTreeModelSoundCapabilities;

	const shared_ptr<StdConfig> m_refStdConfig;
	const StdConfig::CapabilityAssignment& m_oCapabilityAssignment;
	shared_ptr<stmi::DeviceManager> m_refDM;
	shared_ptr<stmi::EventListener> m_refEventListener;
	shared_ptr<AllPreferences> m_refPrefs;
	shared_ptr<Theme> m_refTheme;
	bool m_bPrefsInitialized;

	bool m_bShowGameTab;
	bool m_bShowTeamTab;
	bool m_bShowTeamTreeNode;
	bool m_bShowPlayerTab;
	bool m_bShowPlayerOptions;
	bool m_bShowKeyActions;
	bool m_bShowAssignedCapabilities;
	bool m_bAlwaysOnePlayer;

	bool m_bRegeneratePlayersInProgress;
	bool m_bRegenerateOptionsInProgress;
	bool m_bRegenerateAssignedDevicesInProgress;
	bool m_bRegenerateSoundDevicesInProgress;

	bool m_bEditingTeamOrPlayer;

	int32_t m_nSoundTestIdx; // Index into Named::sounds()

	int32_t m_nSelectedTeam;
	int32_t m_nSelectedMate;

	int32_t m_nSelectedKeyAction; // -1: no selection
	bool m_bListenToKey;
	bool m_bExpectingKey;

	// The following must be the same key
	static const stmi::HARDWARE_KEY s_eModifyHK = stmi::HK_S;
	static const guint s_nModifyKeyVal1 = GDK_KEY_s;
	static const guint s_nModifyKeyVal2 = GDK_KEY_S;
};

} // namespace stmg

#endif	/* STMG_LEVEL_SCREEN_H */

