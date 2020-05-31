/*
 * File:   soundevent.h
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

#ifndef STMG_SOUND_EVENT_H
#define STMG_SOUND_EVENT_H

#include "event.h"
#include "util/basictypes.h"

#include <memory>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

class GameSound;

/** Sound event.
 * Instances of this class create a sound and optionally
 * pause, resume, stop, change the position and change the
 * volume of it.
 *
 * The sound listener is the center of the show area or, in subshow mode, the center
 * of the subshow area.
 *
 * This class can also play a sound passed as nValue to the trigger function. The value
 * is the index into Named::sounds(). Free sounds can also ignore the LocalInit::m_bLooping
 * value and be played once.
 */
class SoundEvent : public Event
{
public:
	struct LocalInit
	{
		int32_t m_nSoundTeam = -1; /**< The preferences team (not the level team) the sound is played for or -1 if all players. Default is -1. */
		int32_t m_nSoundMate = -1; /**< The mate nr of the player the sound is played for or -1 if a team or whole game sound. Default is -1. */
		int32_t m_nSoundIdx = -1; /**< The sound id. An index into Named::sounds(). Must be valid or -1 if not defined. Default is -1.*/
		FPoint m_oPosXY; /**< The initial x and y position of the sound in tiles within the board. */
		double m_fPosZ = 0; /**< The initial z position of the sound in tiles. Default is 0. */
		bool m_bListenerRelative = true; /**< Whether the sound is listener relative. Default is true. */
		bool m_bLooping = false; /**< Whether the sound is looping. Default is false. */
		double m_fVolume = 1.0; /**< The volume. Must be a value between 0 (inaudible) and 1 (max). Default is 1.*/
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit SoundEvent(Init&& oInit) noexcept;
protected:
	/** Reinitialization.
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept;
public:

	/** The triggered function.
	 * @param nMsg If one MESSAGE_OP_TYPE_ENUM then operation is performed otherwise nDefaultIncBy (see constructor).
	 * @param nValue The triggered add value. Is only used if nMsg is MESSAGE_OP_TYPE_ENUM.
	 * @param p0TriggeringEvent The triggering event or null if event activated itself.
	 */
	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	// input
	enum MESSAGE_OP_TYPE_ENUM {
		MESSAGE_SET_SOUND_IDX = 100 /**< Sets the sound to play. nValue is the index into Named::sounds(). */
		, MESSAGE_PRELOAD = 101 /**< Preloads the sound so that when MESSAGE_PLAY or MESSAGE_PLAY_FREE are triggered it's faster. */
		, MESSAGE_PLAY = 102 /**< Plays the sound if not already playing. */
		, MESSAGE_PLAY_FREE = 103 /**< Plays the sound without keeping a reference to it (cannot pause, set position, etc.). */
		, MESSAGE_PLAY_FREE_ONCE = 104 /**< Plays the sound without keeping a reference to it (cannot pause, set position, etc.)
										 * without (potential) looping. */
		, MESSAGE_PLAY_FREE_IDX = 105 /**< Plays the sound identified by nValue without keeping a reference to it (cannot pause, set position, etc.). */
		, MESSAGE_PLAY_FREE_IDX_ONCE = 106 /**< Plays the sound identified by nValue without keeping a reference to it (cannot pause, set position, etc.)
											 * without (potential) looping. */
		, MESSAGE_PAUSE = 110 /**< Pauses the sound if not already paused. */
		, MESSAGE_RESUME = 111 /**< Resumes the sound if it was paused. */
		, MESSAGE_STOP = 112 /**< Stops the sound. */
		, MESSAGE_SET_VOL = 120 /**< Sets the volume of the sound the sound. The nValue 0 is inaudible, 1000 max volume. */
		, MESSAGE_SET_POS_X = 150 /**< The sound x position is set to nValue. */
		, MESSAGE_SET_POS_Y = 151 /**< The sound y position is set to nValue. */
		, MESSAGE_SET_POS_Z = 152 /**< The sound z position is set to nValue. */
		, MESSAGE_SET_POS_XY = 153 /**< The sound xy position is set to Util::unpackPointFromInt32(nValue). */
		, MESSAGE_ADD_TO_POS_X = 160 /**< Add nValue to the current x position. */
		, MESSAGE_ADD_TO_POS_Y = 161 /**< Add nValue to the current y position. */
		, MESSAGE_ADD_TO_POS_Z = 162 /**< Add nValue to the current y position. */
		, MESSAGE_ADD_PERC_TO_POS_X = 170 /**< Add nValue divided by 100 to the current x position. */
		, MESSAGE_ADD_PERC_TO_POS_Y = 171 /**< Add nValue divided by 100 to the current y position. */
		, MESSAGE_ADD_PERC_TO_POS_Z = 172 /**< Add nValue divided by 100 to the current y position. */
		, MESSAGE_RESTART = 200 /**< Restarts the sound. */
		, MESSAGE_RESTART_RESET = 201 /**< Restarts the sound by resetting the initial values for position and volume. */
	};
	// output
	enum {
		LISTENER_GROUP_SOUND_FINISHED = 20 /**< Listener group triggered when the played sound is finished playing. */
	};

private:
	void initCommon() noexcept;

private:
	LocalInit m_oData;

	shared_ptr<GameSound> m_refSound;

	FPoint m_oCurPosXY;
	double m_fCurPosZ;
	double m_fCurVolume;

private:
	SoundEvent() = delete;
	SoundEvent(const SoundEvent& oSource) = delete;
	SoundEvent& operator=(const SoundEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_SOUND_EVENT_H */

