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
 * File:   plaintextthanifactory.h
 */

#ifndef STMG_PLAIN_TEXT_TH_ANI_FACTORY_H
#define STMG_PLAIN_TEXT_TH_ANI_FACTORY_H

#include "stdthemeanimationfactory.h"

#include "themeanimation.h"

#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <glibmm/refptr.h>

#include <pangomm/fontdescription.h>
#include <pangomm/layout.h>

#include <memory>
#include <vector>

#include <stdint.h>

namespace stmg { class LevelAnimation; }
namespace stmg { class StdTheme; }
namespace stmg { class StdThemeContext; }
namespace stmg { class TextAnimation; }
namespace stmg { class TileAlpha; }
namespace stmg { class TileColor; }
namespace stmg { class TileFont; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

using std::unique_ptr;

/** Creates theme animation that draws the text to fit in a rectangle.
 */
class PlainTextThAniFactory : public StdThemeAnimationFactory
{
public:
	PlainTextThAniFactory(StdTheme* p1Owner, bool bCenter
					, const TileColor& oColor, const TileAlpha& oAlpha, const TileFont& oFont
					, bool bFadeInIsFactor, double fFadeIn, bool bFadeOutIsFactor, double fFadeOut) noexcept;
	bool supports(const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;
	shared_ptr<ThemeAnimation> create(const shared_ptr<StdThemeContext>& refThemeContext, const shared_ptr<LevelAnimation>& refLevelAnimation) noexcept override;
private:
	class PlainTextThAni : public ThemeAnimation
	{
		friend class PlainTextThAniFactory;
	public:
		PlainTextThAni() noexcept;
		void reInit() noexcept;
		int32_t getZ(int32_t nViewTick, int32_t nTotViewTicks) noexcept override;
		bool isStarted(int32_t nViewTick, int32_t nTotViewTicks) noexcept override;
		bool isDone(int32_t nViewTick, int32_t nTotViewTicks) noexcept override;
		void draw(int32_t nViewTick, int32_t nTotViewTicks, const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override;
		void onRemoved() noexcept override;
	private:
		void reInitCommon() noexcept;
		void getRectAndScale(FRect& oRect, double& fScale, double& fPixHLine) noexcept;
	private:
		double m_fWidest;
		double m_fHighest;
		std::vector<NSize> m_aTextSize;

		double m_fFadeIn; // in millisec
		double m_fFadeOut; // in millisec

		shared_ptr<TextAnimation> m_refModel;
		int32_t m_nZ;

		shared_ptr<StdThemeContext> m_refThemeContext;
		Glib::RefPtr<Pango::Layout> m_refFontLayout;
		PlainTextThAniFactory* m_p1Owner;
	};

	unique_ptr<Pango::FontDescription> m_refFont;
	double m_fR1;
	double m_fG1;
	double m_fB1;
	double m_fA1;
	bool m_bCenter;
	bool m_bFadeInIsFactor;
	double m_fFadeIn;
	bool m_bFadeOutIsFactor;
	double m_fFadeOut;

	Recycler<PlainTextThAniFactory::PlainTextThAni> m_oPlainTextThAnis;

private:
	PlainTextThAniFactory() = delete;
	PlainTextThAniFactory(const PlainTextThAniFactory& oSource) = delete;
	PlainTextThAniFactory& operator=(const PlainTextThAniFactory& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_PLAIN_TEXT_TH_ANI_FACTORIES_H */

