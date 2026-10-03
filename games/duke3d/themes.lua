------------------------------------------------------------------------
--  DUKE NUKEM 3D THEMES
------------------------------------------------------------------------
--
--  Copyright (C) 2021-2025 The OBSIDIAN Team
--
--  This program is free software; you can redistribute it and/or
--  modify it under the terms of the GNU General Public License
--  as published by the Free Software Foundation; either version 2,
--  of the License, or (at your option) any later version.
--
------------------------------------------------------------------------
--
--  The DOOM room themes are reused.  Their textures become Duke tiles
--  via the material translation in materials.lua, which gives:
--
--    urban  : L.A. Meltdown style city levels (brick, concrete,
--             building facades, the LA skyline)
--    tech   : Lunar Apocalypse style space stations (steel panels,
--             computers, a starry sky)
--    hell   : Shrapnel City / alien hive style levels (organic alien
--             walls)
--
------------------------------------------------------------------------

DUKE3D.THEMES      = table.deep_copy(DOOM.THEMES)
DUKE3D.ROOM_THEMES = table.deep_copy(DOOM.ROOM_THEMES)
DUKE3D.SINKS       = table.deep_copy(DOOM.SINKS)
DUKE3D.NAMES       = table.deep_copy(DOOM.NAMES)


-- Duke has no teleporting monster closets or 3D floors, and the keys
-- are access cards.
DUKE3D.THEMES.DEFAULTS.keys =
{
  k_red    = 50,
  k_blue   = 50,
  k_yellow = 50,
}

-- some extra Duke decorations
for _,name in pairs({ "urban", "tech" }) do
  local T = DUKE3D.THEMES[name]

  T.barrels =
  {
    barrel    = 60,
    fuel_tank = 20,
    trash_can = 20,
  }
end

DUKE3D.THEMES.urban.park_decor =
{
  big_tree  = 60,
  burnt_tree = 20,
  hydrant   = 30,
  trash_can = 20,
}

DUKE3D.THEMES.urban.cliff_trees =
{
  big_tree   = 60,
  burnt_tree = 30,
}
