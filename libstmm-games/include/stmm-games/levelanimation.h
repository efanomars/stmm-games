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
 * File:   levelanimation.h
 */

#ifndef STMG_LEVEL_ANIMATION_H
#define STMG_LEVEL_ANIMATION_H

#include "util/direction.h"
#include "util/basictypes.h"

#include <utility>

#include <stdint.h>

namespace stmg
{

class Level;
class GameProxy;

/** Animation model class.
 * The animation has a position in tiles that can be changed at runtime, but
 * it hasn't a size or a z position, which has to be defined by subclasses.
 */
class LevelAnimation
{
public:
	virtual ~LevelAnimation() noexcept = default;

	/** Returns unique Id of this object.
	 * @return The (non negative) unique id.
	 */
	inline int32_t getId() const noexcept { return m_nId; }

	/** Whether the animation was added to the level (and not removed yet).
	 * @return Whether animation managed by level.
	 */
	inline bool isActive() const noexcept { return (m_p0Level != nullptr); }
	/** The optional view animation name index.
	 * This value can be used to select the view that draws this animation model.
	 * If not set only the (RTTI) type of the this animation object is taken into account.
	 * @return The name index (or -1 if not set).
	 */
	inline int32_t getViewAnimationNameIdx() const noexcept { return m_nAnimationIdx; }

	/** Reference system extended enumeration.
	 * In subshow mode animations can also be drawn in the subshow square and
	 * only seen by the respective level player.
	 *
	 * The reference system for `nLevelPlayer` is `REFSYS_SUBSHOW + nLevelPlayer`.
	 */
	enum REFSYS {
		REFSYS_BOARD = -3 /**< Board. */
		, REFSYS_SHOW = -2 /**< Show. */
		, REFSYS_INVALID = -1
		, REFSYS_SUBSHOW = 0 /**< Subshow level player 0. */
		//, REFSYS_SUBSHOW_0 = 0 level player 0
		//, REFSYS_SUBSHOW_1 = 1 level player 1
		//, REFSYS_SUBSHOW_2 = 2 level player 2
		//, REFSYS_SUBSHOW_3 = 3 level player 3
		// etc. the REFSYS_SUBSHOW_n should be cast to int32_t
	};
	/** The reference system of the animation.
	 * This value cannot change during the lifetime of the animation.
	 * @return The reference system.
	 */
	inline REFSYS getRefSys() noexcept { return m_eRefSys; }

	/** The current position of the animation within its reference system.
	 * Although it is usually the top-left corner of the animation rectangle,
	 * the position is implementation (subclass) specific. For some animations
	 * it might represent the center of the animation rectangle, which is defined
	 * by the view (ThemeAnimation).
	 *
	 * If the animation is scrolled the level will automatically update the position.
	 * @return The position in tiles.
	 */
	inline FPoint getPos() const noexcept { return m_oPos; }
	/** The size of the animation.
	 * Note that as for the position the view is allowed to use a bigger or
	 * smaller area.
	 *
	 * Either width, height or both might be undefined (<= 0), in which case
	 * it must be calculated by the view.
	 * @return The size in tiles.
	 */
	inline FSize getSize() const noexcept { return m_oSize; }
	/** The depth of the animation.
	 * Higher z animations are drawn on top of lower z animations.
	 * @return The z of the animation.
	 */
	inline int32_t getZ() const noexcept { return m_nZ; }

	/** The current game interval.
	 * This is a shortcut to level().game().gameInterval().
	 * @return The game interval in milliseconds.
	 */
	double gameInterval() const noexcept;

public:
	/** Undefined duration constant. The duration should be determined by the theme. */
	static const double s_fDurationUndefined;
	/** Infinite duration constant. */
	static const double s_fDurationInfinity;

protected:
	struct Init
	{
		int32_t m_nAnimationNamedIdx = -1; /**< The animation index as of Named::animations() or
											 * -1 if not defined. Default: -1. */
		double m_fDuration = s_fDurationUndefined; /**< The duration of the animation in milliseconds or
													 * s_fDurationUndefined. Default is s_fDurationUndefined.*/
		FPoint m_oPos; /**< The initial position of the animation. */
		FSize m_oSize; /**< The initial size of the animation. If width is 0 means undefined. */
		int32_t m_nZ = 100; /**< The initial z position (the depth). Animations with higher z value are drawn after
							 * (over) lower z ones. Value 0 is the z of the board and shouldn't be used.
							 * Default is 100. */
	};
	/** Constructor.
	 * @param oInit The parameters.
	 */
	explicit LevelAnimation(const Init& oInit) noexcept;
	/** Reinitialization.
	 * See constructor.
	 */
	void reInit(const Init& oInit) noexcept;

public:
	/** The duration in milliseconds.
	 * @return The duration. `-1.0` if undefined, `0.0` if infinity.
	 */
	inline double getDuration() const noexcept { return m_fDuration; }

	/** Sets new position within reference system.
	 * @param oPos The new position.
	 */
	inline void moveTo(FPoint oPos) noexcept
	{
		m_oPos = std::move(oPos);
	}
	/** Sets new position within reference system.
	 * @param nZ The new z position.
	 */
	inline void moveTo(int32_t nZ) noexcept
	{
		m_nZ = nZ;
	}

	/** Called by Level when the animation is scrolled by Level::boardScroll.
	 * Called even if the reference system is REFSYS_SUBSHOW.
	 * The LevelAnimation implementation does nothing and therefore must
	 * not be called by subclasses.
	 *
	 * To be scrolled the animation has to be added with Level::animationAddScrolled()
	 * rather than Level::animationAdd().
	 * @param eDir The direction of the scroll.
	 */
	virtual void onScrolled(Direction::VALUE eDir) noexcept;

	/** The time elapsed since the animation was started in milliseconds.
	 * @param nViewTick The current view tick.
	 * @param nTotViewTicks The number of view ticks in this game interval.
	 * @return The elapsed time in milliseconds.
	 */
	double getElapsed(int32_t nViewTick, int32_t nTotViewTicks) const noexcept;
	/** Whether the animation has already started.
	 * @param nViewTick The current view tick.
	 * @param nTotViewTicks The number of view ticks in this game interval.
	 * @return Whether the animation was started.
	 */
	bool isStarted(int32_t nViewTick, int32_t nTotViewTicks) const noexcept;
	/** Whether the animation has finished.
	 * @param nViewTick The current view tick.
	 * @param nTotViewTicks The number of view ticks in this game interval.
	 * @return Whether the animation is done.
	 */
	bool isDone(int32_t nViewTick, int32_t nTotViewTicks) const noexcept;

	/** The level.
	 * This function can only be called if the animation is active.
	 * @return The level.
	 */
	inline Level& level() noexcept { return *m_p0Level; }
	/** The level.
	 * This function can only be called if the animation is active.
	 * @return The level.
	 */
	inline const Level& level() const noexcept { return *m_p0Level; }
	/** The game.
	 * This function can only be called if the animation is active.
	 * @return The level.
	 */
	inline GameProxy& game() noexcept { return *m_p0Game; }
	/** The game.
	 * This function can only be called if the animation is active.
	 * @return The level.
	 */
	inline const GameProxy& game() const noexcept { return *m_p0Game; }
private:
	friend class Level;
	void setLevel(Level* p0Level) noexcept;
private:
	int32_t m_nId;
	static int32_t s_nId;

	int32_t m_nAnimationIdx;
	double m_fDuration;
	FPoint m_oPos;
	int32_t m_nZ;
	FSize m_oSize;

	double m_fStartedMillisec; // set by Level
	Level* m_p0Level; // set by Level
	REFSYS m_eRefSys; // set by Level

	GameProxy* m_p0Game;

	uint32_t m_nScrolledUnique; // set by Level
private:
	LevelAnimation() = delete;
	LevelAnimation(const LevelAnimation& oSource) = delete;
	LevelAnimation& operator=(const LevelAnimation& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_LEVEL_ANIMATION_H */

