/*
 * File:   stdthemedrawingcontext.h
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

#ifndef STMG_STD_THEME_DRAWING_CONTEXT_H
#define STMG_STD_THEME_DRAWING_CONTEXT_H

#include <stmm-games/util/basictypes.h>

#include <memory>

namespace stmg { class Image; }
namespace stmg { class StdThemeContext; }

namespace Glib { template <class T_CppObject> class RefPtr; }
namespace Pango { class Context; }


namespace stmg
{

using std::shared_ptr;

/** The tile drawing context.
 * Used to draw Modifiers.
 */
class StdThemeDrawingContext
{
public:
	StdThemeDrawingContext() noexcept;
	/** The tile size.
	 * @return The size of the tile.
	 */
	NSize getTileSize() const noexcept;
	/** The font layout.
	 * @return The pango layout.
	 */
	const Glib::RefPtr<Pango::Context>& getFontContext() noexcept;

	/** The selected (context) image.
	 * @return The selected image. Can be null.
	 */
	Image* getSelectedImage() noexcept;
	/** Set selected (context) image.
	 * @param p0Image The selected image. Can be null.
	 */
	void setSelectedImage(Image* p0Image) noexcept;
protected:
	void reInit() noexcept;
private:
	friend class StdThemeContext;
	friend class StdTheme;
	StdThemeContext* m_p1Owner;
	Image* m_p0SelectedImage;
};

} // namespace stmg

#endif	/* STMG_STD_THEME_DRAWING_CONTEXT_H */

