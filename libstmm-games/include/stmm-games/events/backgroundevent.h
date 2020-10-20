/*
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
/*
 * File:   backgroundevent.h
 */

#ifndef STMG_BACKGROUND_EVENT_H
#define STMG_BACKGROUND_EVENT_H

#include "event.h"

#include "levelanimation.h"
#include "animations/backgroundanimation.h"
#include "levellisteners.h"
#include "util/basictypes.h"
#include "util/direction.h"
#include "util/recycler.h"

#include <vector>
#include <memory>

#include <stdint.h>

namespace stmg { class TileRect; }

namespace stmg
{

using std::shared_ptr;

class BackgroundEvent : public Event, public BoardScrollListener
{
public:
	/** Image data used to fill background animation. */
	struct PatternImage {
		int32_t m_nImgId = -1; /**< The image id. Must be valid id in `level().getNamed().images()`. */
		double m_fImgW = 0.0; /**< Width in tiles. If &lt;= 0 either natural size or ratio will be used. */
		double m_fImgH = 0.0; /**< Height in tiles. If &lt;= 0 either natural size or ratio will be used. */
		double m_fImgRelPosX = 0.0; /**< Initial x position relative to the animation origin. In tile widths. */
		double m_fImgRelPosY =  0.0; /**< Initial y position relative to the animation origin. In tile heights. */
		bool m_bRelPosScrolled = false; /**< Whether the relative position is scrolled.
										 * If true the animation is also scrolled and the relative position
										 * is counterscrolled. Default: true. */
	};
	struct LocalInit
	{
		std::vector<PatternImage> m_aPatternImages; /**< Vector of image data. */
		LevelAnimation::REFSYS m_eRefSys = LevelAnimation::REFSYS_BOARD; /**< The reference system of the position.
																		 * Default: LevelAnimation::REFSYS_BOARD. */
		bool m_bScrolled = false; /**< Whether the background scrolls with the board.
									 * Ignored if m_eRefSys not LevelAnimation::REFSYS_BOARD. Default: false. */
		FPoint m_oMove; /**< The movement at each tick in tiles. Default: (0,0). */
		int32_t m_nAnimationNamedIdx = -1; /**< The animation index as of Named::animations() or -1 if not defined. */
		FRect m_oRect; /**< The animation rectangle. */
		int32_t m_nZ = -500; /**< Animations with higher z value are drawn after (over) lower z ones.
							 * Value 0 is the z of the board and shouldn't be used. Default: -500. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * @param oInit The parameters.
	 */
	explicit BackgroundEvent(Init&& oInit) noexcept;
protected:
	/** See BackgroundEvent::BackgroundEvent().
	 */
	void reInit(Init&& oInit) noexcept;
public:
	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	// Inputs
	enum {
		MESSAGE_CONTROL_STOP = 100 /**< Removes the Animation. */
		, MESSAGE_CONTROL_RESTART = 105 /**< Restarts the (stopped) animation. */
		, MESSAGE_MOVE_PAUSE = 110 /**< Pauses the animation. */
		, MESSAGE_MOVE_RESUME = 115 /**< Resumes the paused animation. */
		, MESSAGE_MOVE_SET_FACTOR = 125  /**< Changes animation size keeping the center.
											* nValue contains perc: ex. 150 is factor ´1.5´. */
		, MESSAGE_ADD_POS_X = 140  /**< Adds nValue to the x position of the animation. */
		, MESSAGE_ADD_POS_Y = 141  /**< Adds nValue to the y position of the animation. */
		, MESSAGE_ADD_POS_X_PERC = 142  /**< Adds nValue percent (of a tile) to the x position of the animation. */
		, MESSAGE_ADD_POS_Y_PERC = 142  /**< Adds nValue percent (of a tile) to the y position of the animation. */
		, MESSAGE_IMG_NEXT = 150  /**< Uses the next pattern to fill the background. */
		, MESSAGE_IMG_PREV = 151  /**< Uses the previous pattern to fill the background. */
	};
	void boardPreScroll(Direction::VALUE eDir, const shared_ptr<TileRect>& refTiles) noexcept override;
	void boardPostScroll(Direction::VALUE eDir) noexcept override;

	// Outputs
	//enum {
	//	LISTENER_GROUP_CHANGED = 10 /**< nValue contains the index into LocalInit::m_aPatternImages of the image it changed to. */
	//};

private:
	void commonInit(LocalInit&& oInit) noexcept;
	void keepMoving(int32_t nGameTick) noexcept;

private:
	std::vector<PatternImage> m_aPatternImages;
	LevelAnimation::REFSYS m_eRefSys;
	bool m_bScrolled;
	FPoint m_oMove;
	BackgroundAnimation::Init m_oBAInit;

	int32_t m_nCurPatternImg; // Index in m_aPatternImage
	bool m_bPaused;
	bool m_bMoving; // = (m_fMoveX != 0.0) || (m_fMoveY != 0.0)
	double m_fMoveFactor;

	struct PrivateBackgroundAnimation : public BackgroundAnimation
	{
		using BackgroundAnimation::BackgroundAnimation;
		using BackgroundAnimation::reInit;
	};

	Recycler<PrivateBackgroundAnimation> m_oBGAnimations;
	shared_ptr<PrivateBackgroundAnimation> m_refBGAnimation;
private:
	BackgroundEvent() = delete;
	BackgroundEvent(const BackgroundEvent& oSource) = delete;
	BackgroundEvent& operator=(const BackgroundEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_BACKGROUND_EVENT_H */

