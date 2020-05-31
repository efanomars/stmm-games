/*
 * File:   commontheme.cc
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

#include "commontheme.h"

#include <stmm-games/util/basictypes.h>

#include <vector>
#include <cassert>
//#include <iostream>
//#include <algorithm>
#include <utility>


namespace stmg
{

namespace CommonTheme
{

static std::vector<NamedRgbColor> s_aNamedRgbColors;

static void addSvgColor(std::string&& sName, uint8_t nR, uint8_t nG, uint8_t nB)
{
	NamedRgbColor oNamedRgbColor;
	oNamedRgbColor.m_sName = std::move(sName);
	oNamedRgbColor.m_oColor.m_nR = nR;
	oNamedRgbColor.m_oColor.m_nG = nG;
	oNamedRgbColor.m_oColor.m_nB = nB;
	s_aNamedRgbColors.push_back(std::move(oNamedRgbColor));
}
static void loadSvgColors()
{
	addSvgColor("black", 0, 0, 0);
	addSvgColor("white", 255, 255, 255);
	addSvgColor("red", 255, 0, 0);
	addSvgColor("green", 0, 128, 0);
	addSvgColor("blue", 0, 0, 255);
	addSvgColor("yellow", 255, 255, 0);
	addSvgColor("magenta", 255, 0, 255);
	addSvgColor("cyan", 0, 255, 255);
	addSvgColor("orange", 255, 165, 0);
	addSvgColor("brown", 165, 42, 42);
	addSvgColor("pink", 255, 192, 203);
	addSvgColor("grey", 128, 128, 128);

	addSvgColor("aliceblue", 240, 248, 255);
	addSvgColor("antiquewhite", 250, 235, 215);
	addSvgColor("aqua", 0, 255, 255);
	addSvgColor("aquamarine", 127, 255, 212);
	addSvgColor("azure", 240, 255, 255);
	addSvgColor("beige", 245, 245, 220);
	addSvgColor("bisque", 255, 228, 196);
	addSvgColor("blanchedalmond", 255, 235, 205);
	addSvgColor("blueviolet", 138, 43, 226);
	addSvgColor("burlywood", 222, 184, 135);
	addSvgColor("cadetblue", 95, 158, 160);
	addSvgColor("chartreuse", 127, 255, 0);
	addSvgColor("chocolate", 210, 105, 30);
	addSvgColor("coral", 255, 127, 80);
	addSvgColor("cornflowerblue", 100, 149, 237);
	addSvgColor("cornsilk", 255, 248, 220);
	addSvgColor("crimson", 220, 20, 60);
	addSvgColor("darkblue", 0, 0, 139);
	addSvgColor("darkcyan", 0, 139, 139);
	addSvgColor("darkgoldenrod", 184, 134, 11);
	addSvgColor("darkgray", 169, 169, 169);
	addSvgColor("darkgreen", 0, 100, 0);
	addSvgColor("darkgrey", 169, 169, 169);
	addSvgColor("darkkhaki", 189, 183, 107);
	addSvgColor("darkmagenta", 139, 0, 139);
	addSvgColor("darkolivegreen", 85, 107, 47);
	addSvgColor("darkorange", 255, 140, 0);
	addSvgColor("darkorchid", 153, 50, 204);
	addSvgColor("darkred", 139, 0, 0);
	addSvgColor("darksalmon", 233, 150, 122);
	addSvgColor("darkseagreen", 143, 188, 143);
	addSvgColor("darkslateblue", 72, 61, 139);
	addSvgColor("darkslategray", 47, 79, 79);
	addSvgColor("darkslategrey", 47, 79, 79);
	addSvgColor("darkturquoise", 0, 206, 209);
	addSvgColor("darkviolet", 148, 0, 211);
	addSvgColor("deeppink", 255, 20, 147);
	addSvgColor("deepskyblue", 0, 191, 255);
	addSvgColor("dimgray", 105, 105, 105);
	addSvgColor("dimgrey", 105, 105, 105);
	addSvgColor("dodgerblue", 30, 144, 255);
	addSvgColor("firebrick", 178, 34, 34);
	addSvgColor("floralwhite", 255, 250, 240);
	addSvgColor("forestgreen", 34, 139, 34);
	addSvgColor("fuchsia", 255, 0, 255);
	addSvgColor("gainsboro", 220, 220, 220);
	addSvgColor("ghostwhite", 248, 248, 255);
	addSvgColor("gold", 255, 215, 0);
	addSvgColor("goldenrod", 218, 165, 32);
	addSvgColor("gray", 128, 128, 128);
	addSvgColor("greenyellow", 173, 255, 47);
	addSvgColor("honeydew", 240, 255, 240);
	addSvgColor("hotpink", 255, 105, 180);
	addSvgColor("indianred", 205, 92, 92);
	addSvgColor("indigo", 75, 0, 130);
	addSvgColor("ivory", 255, 255, 240);
	addSvgColor("khaki", 240, 230, 140);
	addSvgColor("lavender", 230, 230, 250);
	addSvgColor("lavenderblush", 255, 240, 245);
	addSvgColor("lawngreen", 124, 252, 0);
	addSvgColor("lemonchiffon", 255, 250, 205);
	addSvgColor("lightblue", 173, 216, 230);
	addSvgColor("lightcoral", 240, 128, 128);
	addSvgColor("lightcyan", 224, 255, 255);
	addSvgColor("lightgoldenrodyellow", 250, 250, 210);
	addSvgColor("lightgray", 211, 211, 211);
	addSvgColor("lightgreen", 144, 238, 144);
	addSvgColor("lightgrey", 211, 211, 211);
	addSvgColor("lightpink", 255, 182, 193);
	addSvgColor("lightsalmon", 255, 160, 122);
	addSvgColor("lightseagreen", 32, 178, 170);
	addSvgColor("lightskyblue", 135, 206, 250);
	addSvgColor("lightslategray", 119, 136, 153);
	addSvgColor("lightslategrey", 119, 136, 153);
	addSvgColor("lightsteelblue", 176, 196, 222);
	addSvgColor("lightyellow", 255, 255, 224);
	addSvgColor("lime", 0, 255, 0);
	addSvgColor("limegreen", 50, 205, 50);
	addSvgColor("linen", 250, 240, 230);
	addSvgColor("maroon", 128, 0, 0);
	addSvgColor("mediumaquamarine", 102, 205, 170);
	addSvgColor("mediumblue", 0, 0, 205);
	addSvgColor("mediumorchid", 186, 85, 211);
	addSvgColor("mediumpurple", 147, 112, 219);
	addSvgColor("mediumseagreen", 60, 179, 113);
	addSvgColor("mediumslateblue", 123, 104, 238);
	addSvgColor("mediumspringgreen", 0, 250, 154);
	addSvgColor("mediumturquoise", 72, 209, 204);
	addSvgColor("mediumvioletred", 199, 21, 133);
	addSvgColor("midnightblue", 25, 25, 112);
	addSvgColor("mintcream", 245, 255, 250);
	addSvgColor("mistyrose", 255, 228, 225);
	addSvgColor("moccasin", 255, 228, 181);
	addSvgColor("navajowhite", 255, 222, 173);
	addSvgColor("navy", 0, 0, 128);
	addSvgColor("oldlace", 253, 245, 230);
	addSvgColor("olive", 128, 128, 0);
	addSvgColor("olivedrab", 107, 142, 35);
	addSvgColor("orangered", 255, 69, 0);
	addSvgColor("orchid", 218, 112, 214);
	addSvgColor("palegoldenrod", 238, 232, 170);
	addSvgColor("palegreen", 152, 251, 152);
	addSvgColor("paleturquoise", 175, 238, 238);
	addSvgColor("palevioletred", 219, 112, 147);
	addSvgColor("papayawhip", 255, 239, 213);
	addSvgColor("peachpuff", 255, 218, 185);
	addSvgColor("peru", 205, 133, 63);
	addSvgColor("plum", 221, 160, 221);
	addSvgColor("powderblue", 176, 224, 230);
	addSvgColor("purple", 128, 0, 128);
	addSvgColor("rosybrown", 188, 143, 143);
	addSvgColor("royalblue", 65, 105, 225);
	addSvgColor("saddlebrown", 139, 69, 19);
	addSvgColor("salmon", 250, 128, 114);
	addSvgColor("sandybrown", 244, 164, 96);
	addSvgColor("seagreen", 46, 139, 87);
	addSvgColor("seashell", 255, 245, 238);
	addSvgColor("sienna", 160, 82, 45);
	addSvgColor("silver", 192, 192, 192);
	addSvgColor("skyblue", 135, 206, 235);
	addSvgColor("slateblue", 106, 90, 205);
	addSvgColor("slategray", 112, 128, 144);
	addSvgColor("slategrey", 112, 128, 144);
	addSvgColor("snow", 255, 250, 250);
	addSvgColor("springgreen", 0, 255, 127);
	addSvgColor("steelblue", 70, 130, 180);
	addSvgColor("tan", 210, 180, 140);
	addSvgColor("teal", 0, 128, 128);
	addSvgColor("thistle", 216, 191, 216);
	addSvgColor("tomato", 255, 99, 71);
	addSvgColor("turquoise", 64, 224, 208);
	addSvgColor("violet", 238, 130, 238);
	addSvgColor("wheat", 245, 222, 179);
	addSvgColor("whitesmoke", 245, 245, 245);
	addSvgColor("yellowgreen", 154, 205, 50);
}
int32_t getTotSvgColors() noexcept
{
	if (s_aNamedRgbColors.empty()) {
		loadSvgColors();
	}
	return static_cast<int32_t>(s_aNamedRgbColors.size());
}
const NamedRgbColor& getSvgColor(int32_t nPal) noexcept
{
	if (s_aNamedRgbColors.empty()) {
		loadSvgColors();
	}
	assert((nPal >= 0) && (nPal < getTotSvgColors()));
	return s_aNamedRgbColors[nPal];
}

} // namespace CommonTheme

} // namespace stmg
