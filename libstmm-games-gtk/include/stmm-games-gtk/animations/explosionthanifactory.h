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
 * File:   explosionthanifactory.h
 */

#ifndef STMG_EXPLOSION_TH_ANI_FACTORY_H
#define STMG_EXPLOSION_TH_ANI_FACTORY_H

#include "stdthemeanimationfactory.h"

#include <memory>

namespace stmg { class ImageSequenceThAniFactory; }
namespace stmg { class LevelAnimation; }
namespace stmg { class StdTheme; }
namespace stmg { class StdThemeContext; }
namespace stmg { class ThemeAnimation; }

namespace stmg
{

class ExplosionThAniFactory : public StdThemeAnimationFactory
{
public:
	/** Constructor.
	 * @param p1Owner The StdTheme object pointer. Cannot be null.
	 * @param p0ImageSequenceThAniFactory The factory that actually does the animation. Cannot be null.
	 */
	ExplosionThAniFactory(StdTheme* p1Owner, ImageSequenceThAniFactory* p0ImageSequenceThAniFactory) noexcept;

	bool supports(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;
	shared_ptr<ThemeAnimation> create(const shared_ptr<StdThemeContext>& refThemeContext, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;

private:
	ImageSequenceThAniFactory* m_p0ImageSequenceThAniFactory;
private:
	ExplosionThAniFactory() = delete;
	ExplosionThAniFactory(const ExplosionThAniFactory& oSource) = delete;
	ExplosionThAniFactory& operator=(const ExplosionThAniFactory& oSource) = delete;
};


} // namespace stmg

#endif	/* STMG_EXPLOSION_TH_ANI_FACTORY_H */

