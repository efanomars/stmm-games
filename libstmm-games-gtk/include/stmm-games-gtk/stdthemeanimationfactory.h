/*
 * File:   stdthemeanimationfactory.h
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

#ifndef STMG_STD_THEME_ANIMATION_FACTORY_H
#define STMG_STD_THEME_ANIMATION_FACTORY_H


#include <memory>

namespace stmg { class LevelAnimation; }
namespace stmg { class ThemeAnimation; }

namespace stmg
{

using std::shared_ptr;

class StdTheme;
class StdThemeContext;

/** Base class for ThemeAnimation factories.
 */
class StdThemeAnimationFactory
{
public:
	virtual ~StdThemeAnimationFactory() noexcept = default;
	/** Constructor.
	 * @param p1Owner The owner theme. Cannot be null.
	 */
	explicit StdThemeAnimationFactory(StdTheme* p1Owner) noexcept;

	/** Tells whether the factory supports the model animation.
	 * Tells whether the class of the model (not the actual model instance) is supported.
	 * @param refLevelAnimation The level animation (the model). Cannot be null.
	 * @return Whether the factory can create a theme animation for the model class.
	 */
	virtual bool supports(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept = 0;
	/** Creates a theme animation for a level animation.
	 * The creation may fail even if supports() returns true.
	 * @param refThemeContext The theme context. Cannot be null.
	 * @param refLevelAnimation The model. Cannot be null.
	 * @return The new theme animation instance or null if failed.
	 */
	virtual shared_ptr<ThemeAnimation> create(const shared_ptr<StdThemeContext>& refThemeContext
											, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept = 0;

	/** Clears the owner passed in the constructor.
	 * Makes the instance unusable.
	 */
	virtual void clearOwner() noexcept;
protected:
	/** The owner.
	 * @return The owner StdTheme.
	 */
	inline StdTheme* owner() noexcept { return m_p1Owner; }
	/** The owner.
	 * @return The owner StdTheme.
	 */
	inline const StdTheme* owner() const noexcept { return m_p1Owner; }
private:
	StdTheme* m_p1Owner;
private:
	StdThemeAnimationFactory() = delete;
	StdThemeAnimationFactory(const StdThemeAnimationFactory& oSource) = delete;
	StdThemeAnimationFactory& operator=(const StdThemeAnimationFactory& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_STD_THEME_ANIMATION_FACTORY_H */

