/*
 * File:   showtextevent.h
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

#ifndef STMG_SHOW_TEXT_EVENT_H
#define STMG_SHOW_TEXT_EVENT_H

#include "event.h"

#include "levelanimation.h"
#include "animations/textanimation.h"

#include "util/basictypes.h"
#include "util/recycler.h"

#include <vector>
#include <string>
#include <memory>
#include <utility>

#include <stdint.h>

namespace stmg
{

using std::shared_ptr;

class ShowTextEvent : public Event
{
public:
	enum SOBST_TYPE {
		SOBST_TYPE_INPUT = 0 /**< Input message. %%im%% %%iv%% %%it%% %%ie%% etc. */
		, SOBST_TYPE_VAR_ID = 1 /**< Variable content of game, level team or player. %%vm00Points%% %%vgTime%% etc. */
		, SOBST_TYPE_INTERVAL = 10 /**< Game interval in milliseconds of game and level. %%fg%% %%fl%% etc. */
	};
	/** If SobstItem::m_eSobstType is SOBST_TYPE_INPUT, SobstItem::m_nParam1 contains
	 * a value from this enumeration.
	 */
	enum SOBST_INPUT_TYPE {
		SOBST_INPUT_TYPE_MSG = 0 /**< The message number passed to the trigger function. %%im%%. */
		, SOBST_INPUT_TYPE_VALUE = 1 /**< The value passed to the trigger function. %%iv%%. */
		, SOBST_INPUT_TYPE_VALUE_X = 2 /**< The value as unpacked coords x passed to the trigger function. %%iv%%. */
		, SOBST_INPUT_TYPE_VALUE_Y = 3 /**< The value as unpacked coords x passed to the trigger function. %%iv%%. */
		, SOBST_INPUT_TYPE_TICK = 10 /**< The current game tick. %%it%%. */
		, SOBST_INPUT_TYPE_EVENT = 11 /**< Whether the event was triggered by another event (bool) or by the timer. %%ie%%. */
	};
	/** If SobstItem::m_eSobstType is SOBST_TYPE_INTERVAL, SobstItem::m_nParam1 contains
	 * a value from this enumeration.
	 */
	enum SOBST_INTERVAL_TYPE {
		SOBST_INTERVAL_TYPE_GAME = 0  /**< The current game interval in milliseconds. %%dg%% */
		, SOBST_INTERVAL_TYPE_LEVEL = 1 /**< The current level interval in milliseconds. %%dl%% */
	};

	struct SobstItem {
		int32_t m_nLine = 0; /**< The line number (start from 0) */
		int32_t m_nPos = 0; /**< The position within the string (for unicode it's not the code point!). */
		SOBST_TYPE m_eSobstType = SOBST_TYPE_INPUT; /**< The sobstitution type.*/
		int32_t m_nParam1; /**< For SOBST_TYPE_VAR_ID contains the OwnerType (example: OwnerType::GAME). */
		int32_t m_nParam2; /**< For SOBST_TYPE_VAR_ID contains the variable idx for the owner type. */
		int32_t m_nParam3; /**< For SOBST_TYPE_VAR_ID and OwnerType::TEAM or OwnerType::PLAYER contains the level team or -1. */
		int32_t m_nParam4; /**< For SOBST_TYPE_VAR_ID and OwnerType::PLAYER contains the teammate or -1. */
	};

	static constexpr int32_t s_nShowTextEventDefaultZ = 100000;

	struct LocalInit
	{
		std::vector<std::string> m_aSobstLines; /**< The text to be shown (without the sobstitutions).*/
		std::vector<SobstItem> m_aSobsts; /**< The sobstitutions. */
		int32_t m_nAnimationNamedIdx = -1; /**< The animation index as of Named::animations() or -1 if not defined. */
		double m_fTextSize = 1; /**< The font height in tile heights. Default is 1. */
		int32_t m_nDuration = LevelAnimation::s_fDurationInfinity; /**< Duration in milliseconds or LevelAnimation::s_fDurationUndefined or LevelAnimation::s_fDurationInfinity.
																	* Default is LevelAnimation::s_fDurationInfinity.*/
		FRect m_oRect; /**< Rectangle in which to fit the text. If size is 0 then depending on the reference system, the whole board, show or subshow is meant. */
		LevelAnimation::REFSYS m_eRefSys = LevelAnimation::REFSYS_SHOW; /**< The reference system m_oRect is in. LevelAnimation::REFSYS_SUBSHOW can only be set if in subshow mode.*/
		int32_t m_nZ = s_nShowTextEventDefaultZ; /**< The z position of the animation. Lower z animations are overdrawn by higher z. The default is s_nShowTextEventDefaultZ.*/
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * @param oInit The parameters.
	 */
	explicit ShowTextEvent(Init&& oInit) noexcept;
protected:
	/** See constructor.
	 */
	void reInit(Init&& oInit) noexcept;
public:

	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	// input
	enum {
		MESSAGE_STOP_ANIMATION = 100 /**< Stop animation (if one is active). */
		, MESSAGE_START_ANIMATION_AT = 110 /**< Starts animation at position nValue packed with Util::packPointToInt32(x,y).
											 * The width and the height are unchanged. */
	};

	//enum {
	//	LISTENER_GROUP_SHOWED_TEXT = 10
	//};

private:
	void commonInit() noexcept;
	void calcRectSortItems() noexcept;
	void animationCreateShowText(std::vector<std::string>&& aLines, double fPosX, double fPosY) noexcept;

private:
	LocalInit m_oData;
	shared_ptr<TextAnimation> m_refCurrentShowText;

	class PrivateTextAnimation : public TextAnimation
	{
	public:
		using TextAnimation::TextAnimation;
		void reInit(TextAnimation::Init&& oInit)
		{
			TextAnimation::reInit(std::move(oInit));
		}
	};
	static Recycler<PrivateTextAnimation, TextAnimation> s_oTextAnimationRecycler;

private:
	ShowTextEvent() = delete;
	ShowTextEvent(const ShowTextEvent& oSource) = delete;
	ShowTextEvent& operator=(const ShowTextEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_SHOW_TEXT_EVENT_H */

