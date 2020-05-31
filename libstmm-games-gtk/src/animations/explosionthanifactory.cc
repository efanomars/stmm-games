/*
 * File:   explosionthanifactory.cc
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

#include "animations/explosionthanifactory.h"

#include "animations/imagesequencethanifactory.h"

#include <stmm-games/animations/explosionanimation.h>

#include <cassert>
//#include <iostream>

namespace stmg { class LevelAnimation; }
namespace stmg { class StdTheme; }
namespace stmg { class StdThemeContext; }
namespace stmg { class ThemeAnimation; }


namespace stmg
{

ExplosionThAniFactory::ExplosionThAniFactory(StdTheme* p1Owner, ImageSequenceThAniFactory* p0ImageSequenceThAniFactory) noexcept
: StdThemeAnimationFactory(p1Owner)
, m_p0ImageSequenceThAniFactory(p0ImageSequenceThAniFactory)
{
	assert(p0ImageSequenceThAniFactory != nullptr);
}
bool ExplosionThAniFactory::supports(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refLevelAnimation);
	return (nullptr != dynamic_cast<ExplosionAnimation*>(refLevelAnimation.get()));
}
shared_ptr<ThemeAnimation> ExplosionThAniFactory::create(const shared_ptr<StdThemeContext>& refThemeContext
														, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept
{
	assert(refThemeContext);
	assert(refLevelAnimation);

	auto refModel = std::dynamic_pointer_cast<ExplosionAnimation>(refLevelAnimation);
	if (!refModel) {
		return shared_ptr<ThemeAnimation>{};
	}
	return m_p0ImageSequenceThAniFactory->createAny(refThemeContext, refModel);
}

} // namespace stmg
