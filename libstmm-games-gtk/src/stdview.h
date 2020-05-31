/*
 * File:   stdview.h
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

#ifndef STMG_STD_VIEW_H
#define STMG_STD_VIEW_H

#include "stdviewlayout.h"
#include "stdlevelview.h"

#include <stmm-games/gameview.h>
#include <stmm-games/util/basictypes.h>

#include <stmm-input/devicemanager.h>

#include <glibmm/refptr.h>
#include <pangomm/context.h>

#include <memory>
#include <utility>
#include <vector>
#include <limits>

#include <stdint.h>

namespace stmg { class GameSound; }
namespace stmg { class StdConfig; }
namespace stmg { class StdPreferences; }
namespace stmg { class ThemeContext; }
namespace stmg { class Game; }
namespace stmg { class Theme; }
namespace stmg { class ThemeSound; }
namespace stmg { class ThemeWidgetInteractive; }

namespace stmi { class PlaybackCapability; }
namespace stmi { class Event; }
namespace stmi { class XYEvent; }
namespace stmi { class CallIfEventClass; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

using std::shared_ptr;

class StdView : public GameView
{
public:
	/** Constructor.
	 */
	StdView() noexcept;

	/** Initialization.
	 * Can be called multiple times.
	 * Failure is usually caused by the creation of the theme layout and makes the
	 * instance unusable (unless another successful reInit is called).
	 *
	 * The returned active area could be bigger than the oPixSize.
	 * @param refGame The game. Cannot be null.
	 * @param refTheme The theme. Cannot be null.
	 * @param oPixSize The available size in pixel.
	 * @return Returns false if it fails. True and the active area if it succeeds.
	 */
	std::pair<bool, NRect> reInit(const shared_ptr<Game>& refGame, const shared_ptr<StdPreferences>& refPrefs
								, const shared_ptr<Theme>& refTheme, NSize oPixSize
								, const Glib::RefPtr<Pango::Context>& refPaCtx) noexcept;

	void gameStarted() noexcept;
	void gamePaused() noexcept;
	void gameResumed() noexcept;
	void gameEnded() noexcept;
	/** Resize the available area.
	 * It should be called when the game is paused or ended.
	 * It changes the size of the view and possibly of the layout.
	 * This function doesn't actually redraw.
	 *
	 * The returned active area could be bigger than the oPixSize because of
	 * the minimum tile and widget sizes.
	 * @param oPixSize The available size in pixel.
	 * @return Returns false if it fails. True and the active area if it succeeds.
	 */
	std::pair<bool, NRect> resizeCanvas(NSize oPixSize) noexcept;

	/** Redraws the whole view.
	 * This disrupts drawStep(), which will draw the last view tick until the
	 * next game tick.
	 * @param refCc The context. Cannot be null.
	 */
	void redraw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;

	/** Signals the view to prepare for a game tick.
	 * Called before Game::handleTimer is called.
	 */
	void beforeGameTick() noexcept; // TODO rename to syncBeforeGameTick
	/** Sets the number of view ticks for the game interval.
	 * This function should be called after Game::handleTimer.
	 *
	 * A view tick is a call to drawStep, which will be called nTotViewTicks times
	 * each fViewInterval milliseconds before the next beforeGameTick().
	 * @param fViewInterval The duration in milliseconds of a view tick. Must be &gt; 0.
	 * @param nTotViewTicks How many view ticks will follow. Must be &gt; 0.
	 */
	void sync(double fViewInterval, int32_t nTotViewTicks) noexcept; // TODO rename to syncBeforeViewTicks
	/** Draws the view.
	 * The nViewTick must be &lt; nTotViewTicks, the parameter passed to sync().
	 * @param nViewTick The view tick. Must be &gt;= 0.
	 * @param refCc The context. Cannot be null.
	 */
	void drawStep(int32_t nViewTick, const Cairo::RefPtr<Cairo::Context>& refCc) noexcept; // TODO rename

	void handleXYEvent(const shared_ptr<stmi::Event>& refXYEvent) noexcept override;
	shared_ptr<GameSound> createSound(int32_t nSoundIdx, int32_t nTeam, int32_t nMate
									, FPoint oXYPos, double fZPos, bool bListenerRelative
									, double fVolume01, bool bLooping) noexcept override;
	void preloadSound(int32_t nSoundIdx) noexcept override;
	bool removeSound(const shared_ptr<GameSound>& refSound) noexcept override;

#ifndef NDEBUG
	void dump(bool bTickTileAnis) const noexcept;
#endif //NDEBUG

private:
	friend class StdLevelView;
	const Glib::RefPtr<Pango::Context>& getFontContext() noexcept { return m_refPaCtx; }

	// The view was initialized
	bool isReady() const noexcept;

	void checkNewPlaybackDevices() noexcept;
	void lazilySetListenerPosIfNecessary(bool bListenerRelative) noexcept;

	NRect setSize(NSize oPixSize, bool bInit) noexcept;

	void drawStep(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;

	NRect calcLayout() noexcept;

	bool handleXYEvent(const shared_ptr<stmi::Event>& refXYEvent, stmi::XYEvent* p0XYEvent, int32_t nCapabilityId
						, const std::vector< shared_ptr<ThemeWidgetInteractive> >& aInteractiveTWs
						, int32_t& nTeam, int32_t& nMate, int32_t& nInteractiveIdx) noexcept;
	void handleSndFinishedEvent(const shared_ptr<stmi::Event>& refSndFinishedEvent) noexcept;
	void handleDeviceMgmtEvent(const shared_ptr<stmi::Event>& refDeviceMgmtEvent) noexcept;

private:
	bool m_bAssignXYEventToActivePlayer;
	bool m_bSoundEnabled;
	bool m_bPerPlayerSound;
	bool m_bIsSubshows;
	shared_ptr<stmi::EventListener> m_refSndFinishedListener;
	shared_ptr<stmi::CallIfEventClass> m_refSndFinishedCallIf;
	shared_ptr<stmi::EventListener> m_refDeviceMgmtListener;
	shared_ptr<stmi::CallIfEventClass> m_refDeviceMgmtCallIf;

	shared_ptr<Game> m_refGame;
	shared_ptr<StdPreferences> m_refPrefs;
	shared_ptr<StdConfig> m_refStdConfig;
	shared_ptr<Theme> m_refTheme;
	shared_ptr<ThemeContext> m_refThemeCtx; // The  main theme context
	double m_fWHTileRatio;
	std::unique_ptr<StdViewLayout> m_refViewLayout;

	int32_t m_nTotLevels; // shortcut for m_refGame->getTotLevels()
	// m_aLevelViews: only m_nTotLevels are active, the rest is kept to avoid allocations
	std::vector< std::unique_ptr<StdLevelView> > m_aLevelViews; // Size: >= m_nTotLevels

	// Size: 1 if m_bPerPlayerSound is false, otherwise m_refPrefs->getTotPlayers().
	// Value: is null if removed or player is AI.
	std::vector< shared_ptr<stmi::PlaybackCapability> > m_aPlayerPlaybacks;
	struct PlaybackData
	{
		FPoint m_oCurrentListenerPos = FPoint{std::numeric_limits<float>::lowest() / 2, std::numeric_limits<float>::lowest() / 2};
	};
	// used by friend StdLevelView
	std::vector< PlaybackData > m_aPlayerPlaybackDatas; // Size: m_aPlayerPlaybacks.size()

	int32_t m_nTotHumanPlayers; // The number of human players playing
	// The number of human players that have a non null playback capability
	int32_t m_nTotPlayersWithPlayback;

	struct SoundData
	{
		shared_ptr<ThemeSound> m_refThemeSound;
		int32_t m_nUniqueCapabilityId = -1; // The unique capability id playing the sound or -1 if more than one (per player mode)
		bool m_bIsListenerRelative = false;
	};
	std::vector< SoundData > m_aActiveSounds; // Value: cannot be null
	// The number of active sounds that are not listener relative
	int32_t m_nTotAbsActiveSounds;

	int32_t m_nUniqueHumanPrefPlayer; // The only human player or -1
	int32_t m_nUniqueHumanPrefTeam; // The only human team or -1. A human team can have multiple human mates.
	int32_t m_nUniqueHumanMate; // The only human mate. Is -1 exactly when m_nUniqueHumanPrefPlayer is -1.

	int32_t m_nDestructBoardTileAnimation; // Used by friend StdLevelView: index into Named::animations()
	int32_t m_nDestructBrickAnimation; // Used by friend StdLevelView: index into Named::animations()

	int32_t m_nTileW;
	int32_t m_nTileH;
	int32_t m_nPixW;
	int32_t m_nPixH;

	bool m_bCheckNewPlaybackDevices;

	bool m_bFirstDrawAfterInitialization;
	int32_t m_nViewTick;
	int32_t m_nTotViewTicks;
	enum VIEW_STATUS {
		VIEW_STATUS_INVALID = -1
		, VIEW_STATUS_GAME_TICK = 1
		, VIEW_STATUS_SYNC = 3
		, VIEW_STATUS_ASYNC_REDRAW = 9
		, VIEW_STATUS_VIEW_TICK = 10
	};
	VIEW_STATUS m_eViewStatus;

	struct OpenSequence
	{
		explicit OpenSequence(int32_t nCapabilityId) noexcept
		: m_nCapabilityId(nCapabilityId)
		, m_nGrabId(-1)
		, m_nInteractiveIdx(-1)
		, m_nTeam(-1)
		, m_nMate(-1)
		{
		}
		OpenSequence(OpenSequence&& /*oSource*/) noexcept = default;
		OpenSequence& operator=(OpenSequence&& /*oSource*/) noexcept = default;
		// The capability that generated sequence
		int32_t m_nCapabilityId;
		// The grab id of the xy event.
		int64_t m_nGrabId;
		int32_t m_nInteractiveIdx; // the target interactive widget index
		int32_t m_nTeam; // the team of the destination player
		int32_t m_nMate;  // the mate of the destination player
	};
	std::vector<OpenSequence> m_aOpenSequences;

	Glib::RefPtr<Pango::Context> m_refPaCtx;

private:
	StdView(const StdView& oSource) = delete;
	StdView& operator=(const StdView& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_STD_VIEW_H */

