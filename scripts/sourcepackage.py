#!/usr/bin/env python3

#  Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
# 
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public
#  License along with this program; if not, see <http://www.gnu.org/licenses/>

# File:   sourcepackage.py

# Creates a source package containing all stmm-games related projects.

g_sProjectName = "stmm-games"

g_aProjDirs = ["libstmm-games", "libstmm-games-fake", "libstmm-games-file"
				, "libstmm-games-gtk", "libstmm-games-xml-base", "libstmm-games-xml-game"
				, "libstmm-games-xml-gtk"
				, "libstmm-games-xml-gtk/examples/omni"
				]

g_aExtraExcludes = ["test__*.thm"]

import os

g_sScriptDirPath = os.path.dirname(os.path.abspath(__file__))

sTempX1X2X3 = g_sScriptDirPath + "/../share/python/frag_sourcepackage.py"

exec(compile(source=open(sTempX1X2X3).read(), filename=sTempX1X2X3, mode="exec"))
