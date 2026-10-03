----------------------------------------------------------------
--  Engine: Build (Duke Nukem 3D)
----------------------------------------------------------------
--
--  Obsidian Level Maker
--
--  Copyright (C) 2021-2025 The OBSIDIAN Team
--
--  This program is free software; you can redistribute it and/or
--  modify it under the terms of the GNU General Public License
--  as published by the Free Software Foundation; either version 2
--  of the License, or (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
----------------------------------------------------------------
--
--  Levels are written as Build MAP files (version 7, the format of
--  the original game) inside a GRP file.  Very large levels which go
--  over the original limits are written as version 8, which needs
--  EDuke32 (or another modern port such as Raze).
--
----------------------------------------------------------------

BUILD_PORT = {}

BUILD_PORT.PARAMETERS =
{
}


OB_PORTS["eduke32"] =
{
  label = _("EDuke32 / Build"),

  game = "duke3d",

  priority = 90,

  tables =
  {
    BUILD_PORT
  },
}
