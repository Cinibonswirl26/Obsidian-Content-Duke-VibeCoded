------------------------------------------------------------------------
--  BASE FILE for DUKE NUKEM 3D
------------------------------------------------------------------------
--
--  Obsidian Level Maker
--
--  Copyright (C) 2021-2025 The OBSIDIAN Team
--
--  This program is free software; you can redistribute it and/or
--  modify it under the terms of the GNU General Public License
--  as published by the Free Software Foundation; either version 2,
--  of the License, or (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
------------------------------------------------------------------------
--
--  Duke Nukem 3D levels are built with the same layout, room theme
--  and prefab machinery as the DOOM games.  The DOOM room themes and
--  prefabs are reused, with their textures translated into Duke tiles
--  (see materials.lua) and their things translated into Duke sprites
--  (see entities.lua).  The C++ side converts the resulting sectors
--  into Build engine MAP files, which are packed into a GRP file.
--
--  Only tiles from the shareware DUKE3D.GRP (v1.3D) are used, so the
--  levels work with every version of the game.
--
------------------------------------------------------------------------

DUKE3D = { }

-- the DOOM tables are reused for the room themes, so make sure they
-- are available even when only this game was loaded.
if not DOOM then
  gui.set_import_dir("games/doom")
  gui.import("base")
  gui.set_import_dir("games/duke3d")
end

gui.import("params")

gui.import("entities")
gui.import("monsters")
gui.import("pickups")
gui.import("weapons")

gui.import("materials")
gui.import("themes")
gui.import("levels")

------------------------------------------------------------------------

function DUKE3D.setup(self)
  -- textures used by prefabs which are not in the materials table
  GAME.material_fallback_func = DUKE3D.material_fallback

  -- DOOM things used by prefabs
  GAME.thing_remap_func = DUKE3D.remap_doom_thing
end


function DUKE3D.setup_shareware(self)
  DUKE3D.setup(self)

  GAME.shareware = true

  -- remove the stuff which is missing from the shareware DUKE3D.GRP
  for name,_ in pairs(DUKE3D.SHAREWARE_MISSING_MONSTERS) do
    GAME.MONSTERS[name] = nil
  end

  for name,_ in pairs(DUKE3D.SHAREWARE_MISSING_WEAPONS) do
    GAME.WEAPONS[name] = nil
  end

  for name,_ in pairs(DUKE3D.SHAREWARE_MISSING_ITEMS) do
    GAME.PICKUPS[name] = nil
  end
end


-- the Build engine has much lower limits than modern DOOM ports, and
-- the outdoor areas of large levels easily need 20000+ walls.  So keep
-- the level size within what EDuke32 can load.
function DUKE3D.limit_level_size(self)
  local max_size = DUKE3D.MAX_LEVEL_SIZE

  for _,name in pairs({ "float_size", "float_level_upper_bound", "float_level_lower_bound" }) do
    if type(PARAM[name]) == "number" and PARAM[name] > max_size then
      gui.printf("Duke Nukem 3D: limiting %s from %d to %d\n", name, PARAM[name], max_size)
      PARAM[name] = max_size
    end
  end
end


function DUKE3D.begin_level(self, LEVEL)
  local sky = DUKE3D.SKY_TILES[LEVEL.theme_name] or DUKE3D.SKY_TILES.default

  gui.property("sky_tile", sky)

  -- level number of the secret level which the secret exit leads to
  gui.property("secret_target", LEVEL.duke_secret_target or 0)

  -- on the episode's boss level, killing the boss ends the episode
  gui.property("boss_level", LEVEL.duke_boss_level and 1 or 0)
end


function DUKE3D.all_done()
  -- the C++ code writes the GRP file
end


OB_GAMES["duke3d"] =
{
  label = _("Duke Nukem 3D"),

  priority = 80,

  engine = "build",
  format = "duke3d",

  -- prefabs are shared with the DOOM games
  game_dir = "doom",

  tables =
  {
    DUKE3D
  },

  hooks =
  {
    setup       = DUKE3D.setup,
    setup2      = DUKE3D.limit_level_size,
    get_levels  = DUKE3D.get_levels,
    begin_level = DUKE3D.begin_level,
    all_done    = DUKE3D.all_done
  },
}


OB_GAMES["duke3d_sw"] =
{
  label = _("Duke Nukem 3D (Shareware)"),

  priority = 79,

  engine = "build",
  extends = "duke3d",

  hooks =
  {
    setup  = DUKE3D.setup_shareware,
    setup2 = DUKE3D.limit_level_size,
  },
}
