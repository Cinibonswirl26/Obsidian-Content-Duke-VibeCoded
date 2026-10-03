------------------------------------------------------------------------
--  DUKE NUKEM 3D ENTITIES
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
--  Duke sprites are identified by their tile number (picnum).  The
--  editor number given to the CSG code encodes the tile number and the
--  palette:  100000 + pal * 10000 + picnum.  The map converter decodes
--  this again (see source/g_duke.cc).
--
--  Player starts keep the DOOM numbers (1..4 and 11), the converter
--  turns them into the map's start position and APLAYER sprites.
--
--  The names of the DOOM decorations are kept, since the room themes
--  and prefabs refer to them, but they place Duke sprites.
--
------------------------------------------------------------------------

function DUKE3D.sprite(picnum, pal)
  return 100000 + (pal or 0) * 10000 + picnum
end

local S = DUKE3D.sprite

-- Duke tile numbers (from NAMES.H)
DUKE3D.TILES =
{
  APLAYER         = 1405,
  ACCESSCARD      = 60,

  NUKEBARREL      = 1227,
  EXPLODINGBARREL = 1238,
  FIREBARREL      = 1240,
  SEENINE         = 1247,

  TREE1           = 908,
  TREE2           = 910,
  CACTUS          = 911,
  FIREEXT         = 916,
  GENERICPOLE     = 977,
  HANGLIGHT       = 979,
  HYDRENT         = 981,
  TIRE            = 990,
  BOX             = 951,
  CANWITHSOMETHING = 1232,
  RUBBERCAN       = 1062,
  BLOODPOOL       = 1226,
  PODFEM1         = 1294,
  TOILET          = 569,
  CHAIR1          = 556,

  DUKELYINGDEAD     = 1518,
  LIZTROOPDSPRITE   = 1734,
  OCTADEADSPRITE    = 1855,
  PIGCOPDEADSPRITE  = 2060,

  -- red key = pal 21, yellow key = pal 23, blue key = pal 0
  PAL_RED_KEY    = 21,
  PAL_YELLOW_KEY = 23,
}

local T = DUKE3D.TILES


DUKE3D.ENTITIES =
{
  --- PLAYERS ---

  player1 = { id=1, r=16, h=56 },
  player2 = { id=2, r=16, h=56 },
  player3 = { id=3, r=16, h=56 },
  player4 = { id=4, r=16, h=56 },

  dm_player = { id=11 },

  -- teleporters are disabled, but some code still looks this up
  teleport_spot = { id=0 },

  --- KEYS (access cards) ---

  k_red    = { id=S(T.ACCESSCARD, T.PAL_RED_KEY) },
  k_yellow = { id=S(T.ACCESSCARD, T.PAL_YELLOW_KEY) },
  k_blue   = { id=S(T.ACCESSCARD) },

  ks_red    = { id=S(T.ACCESSCARD, T.PAL_RED_KEY) },
  ks_yellow = { id=S(T.ACCESSCARD, T.PAL_YELLOW_KEY) },
  ks_blue   = { id=S(T.ACCESSCARD) },

  --- SCENERY ---

  -- lights --
  lamp          = { id=S(T.GENERICPOLE), r=16, h=48, light=255 },
  tech_column   = { id=S(T.GENERICPOLE), r=16, h=128, light=255 },
  mercury_lamp  = { id=S(T.GENERICPOLE), r=16, h=80, light=255 },
  mercury_small = { id=S(T.GENERICPOLE), r=16, h=60, light=255 },

  candle         = { id=0, r=16, h=16, light=111, pass=true },
  candelabra     = { id=S(T.FIREBARREL), r=16, h=56, light=255 },
  burning_barrel = { id=S(T.FIREBARREL), r=16, h=44, light=255 },

  blue_torch     = { id=S(T.FIREBARREL), r=16, h=96, light=255 },
  blue_torch_sm  = { id=S(T.FIREBARREL), r=16, h=72, light=255 },
  green_torch    = { id=S(T.FIREBARREL), r=16, h=96, light=255 },
  green_torch_sm = { id=S(T.FIREBARREL), r=16, h=72, light=255 },
  red_torch      = { id=S(T.FIREBARREL), r=16, h=96, light=255 },
  red_torch_sm   = { id=S(T.FIREBARREL), r=16, h=72, light=255 },

  -- decoration --
  barrel = { id=S(T.NUKEBARREL), r=12, h=44 },

  green_pillar     = { id=S(T.HYDRENT), r=16, h=56 },
  green_column     = { id=S(T.GENERICPOLE), r=16, h=40 },
  green_column_hrt = { id=S(T.PODFEM1), r=16, h=56, add_mode="island" },

  red_pillar     = { id=S(T.HYDRENT), r=16, h=52 },
  red_column     = { id=S(T.GENERICPOLE), r=16, h=56 },
  red_column_skl = { id=S(T.PODFEM1), r=16, h=56, add_mode="island" },

  burnt_tree = { id=S(T.CACTUS), r=16, h=56, add_mode="island" },
  brown_stub = { id=S(T.TIRE),   r=16, h=56, add_mode="island" },
  big_tree   = { id=S(T.TREE1),  r=31, h=120,add_mode="island" },

  -- gore (alien hive stuff) --
  evil_eye    = { id=S(T.PODFEM1), r=16, h=56, add_mode="island" },
  skull_rock  = { id=S(T.RUBBERCAN), r=16, h=48 },
  skull_pole  = { id=S(T.PODFEM1), r=16, h=52 },
  skull_kebab = { id=S(T.PODFEM1), r=20, h=64 },
  skull_cairn = { id=S(T.RUBBERCAN), r=20, h=40, add_mode="island" },

  impaled_human  = { id=S(T.PODFEM1), r=20, h=64 },
  impaled_twitch = { id=S(T.PODFEM1), r=16, h=64 },

  gutted_victim1 = { id=S(T.HANGLIGHT), r=16, h=88, ceil=true },
  gutted_victim2 = { id=S(T.HANGLIGHT), r=16, h=88, ceil=true },
  gutted_torso1  = { id=S(T.HANGLIGHT), r=16, h=64, ceil=true },
  gutted_torso2  = { id=S(T.HANGLIGHT), r=16, h=64, ceil=true },
  gutted_torso3  = { id=S(T.HANGLIGHT), r=16, h=64, ceil=true },
  gutted_torso4  = { id=S(T.HANGLIGHT), r=16, h=64, ceil=true },

  hang_arm_pair  = { id=S(T.HANGLIGHT), r=20, h=84, ceil=true, pass=true },
  hang_leg_gone  = { id=S(T.HANGLIGHT), r=20, h=52, ceil=true, pass=true },
  hang_torso     = { id=S(T.HANGLIGHT), r=20, h=68, ceil=true, pass=true },
  hang_leg       = { id=S(T.HANGLIGHT), r=20, h=52, ceil=true, pass=true },
  hang_twitching = { id=S(T.HANGLIGHT), r=20, h=68, ceil=true, pass=true },

  gibs          = { id=S(T.BLOODPOOL), r=20, h=16, pass=true },
  gibbed_player = { id=S(T.BLOODPOOL), r=20, h=16, pass=true },

  pool_blood_1  = { id=S(T.BLOODPOOL), r=20, h=16, pass=true },
  pool_blood_2  = { id=S(T.BLOODPOOL), r=20, h=16, pass=true },
  pool_brains   = { id=S(T.BLOODPOOL), r=20, h=16, pass=true },

  dead_player  = { id=S(T.DUKELYINGDEAD),    r=16, h=16, pass=true },
  dead_zombie  = { id=S(T.LIZTROOPDSPRITE),  r=16, h=16, pass=true },
  dead_shooter = { id=S(T.PIGCOPDEADSPRITE), r=16, h=16, pass=true },
  dead_imp     = { id=S(T.LIZTROOPDSPRITE),  r=16, h=16, pass=true },
  dead_demon   = { id=S(T.OCTADEADSPRITE),   r=16, h=16, pass=true },
  dead_caco    = { id=S(T.OCTADEADSPRITE),   r=16, h=16, pass=true },
  dead_skull   = { id=S(T.OCTADEADSPRITE),   r=16, h=16, pass=true },

  -- extra Duke decorations --
  fire_ext     = { id=S(T.FIREEXT),   r=8,  h=24 },
  hydrant      = { id=S(T.HYDRENT),   r=12, h=30 },
  trash_can    = { id=S(T.CANWITHSOMETHING), r=16, h=48 },
  cardboard    = { id=S(T.BOX),       r=16, h=24 },
  fuel_tank    = { id=S(T.SEENINE),   r=12, h=64 },

  -- special stuff (not used in Duke) --
  keen          = { id=0, r=16, h=72, ceil=true },
  brain_boss    = { id=0, r=16, h=16 },
  brain_shooter = { id=0, r=20, h=32 },
  brain_target  = { id=0, r=20, h=32, pass=true },

  dummy = { id=0, r=16, h=16, pass=true },

  light     = { id="light", r=1, h=1, pass=true },
  secret    = { id="oblige_secret", r=1, h=1, pass=true },
  depot_ref = { id="oblige_depot", r=1, h=1, pass=true },
}


DUKE3D.PLAYER_MODEL =
{
  duke =
  {
    stats   = { health=0 },
    weapons = { pistol=1, kick=1 }
  }
}


--
-- DOOM editor numbers found in the shared DOOM prefabs, and the Duke
-- entity which replaces them.  Things which are not listed here are
-- dropped (except the player starts, which the converter handles).
--
DUKE3D.DOOM_THING_REMAP =
{
  -- keys
  [5]  = "k_blue",
  [6]  = "k_yellow",
  [13] = "k_red",
  [38] = "k_red",
  [39] = "k_yellow",
  [40] = "k_blue",

  -- decoration
  [2035] = "barrel",
  [70]   = "burning_barrel",
  [2028] = "lamp",
  [48]   = "tech_column",
  [85]   = "mercury_lamp",
  [86]   = "mercury_small",
  [35]   = "candelabra",
  [44]   = "blue_torch",
  [45]   = "green_torch",
  [46]   = "red_torch",
  [55]   = "blue_torch_sm",
  [56]   = "green_torch_sm",
  [57]   = "red_torch_sm",

  [30] = "green_pillar",
  [31] = "green_column",
  [32] = "red_pillar",
  [33] = "red_column",
  [36] = "green_column_hrt",
  [37] = "red_column_skl",

  [43] = "burnt_tree",
  [47] = "brown_stub",
  [54] = "big_tree",

  [25] = "impaled_human",
  [26] = "impaled_twitch",
  [27] = "skull_pole",
  [28] = "skull_kebab",
  [29] = "skull_cairn",
  [41] = "evil_eye",
  [42] = "skull_rock",

  [59] = "hang_arm_pair",
  [60] = "hang_torso",
  [61] = "hang_leg_gone",
  [62] = "hang_leg",
  [63] = "hang_twitching",
  [73] = "gutted_victim1",
  [74] = "gutted_victim2",
  [75] = "gutted_torso1",
  [76] = "gutted_torso2",
  [77] = "gutted_torso3",
  [78] = "gutted_torso4",

  [10] = "gibbed_player",
  [12] = "gibbed_player",
  [24] = "gibs",
  [79] = "pool_blood_1",
  [80] = "pool_blood_2",
  [81] = "pool_brains",

  [15] = "dead_player",
  [18] = "dead_zombie",
  [19] = "dead_shooter",
  [20] = "dead_imp",
  [21] = "dead_demon",
  [22] = "dead_caco",
  [23] = "dead_skull",

  -- pickups
  [2011] = "cola",
  [2014] = "cola",
  [2012] = "sixpak",
  [2015] = "armor_shard",
  [2018] = "armor",
  [2019] = "armor",
  [2013] = "atomic_health",
  [83]   = "atomic_health",
  [2007] = "clip",
  [2048] = "clip",
  [2008] = "shells",
  [2049] = "shells",
  [2010] = "rockets",
  [2046] = "rockets",
  [2047] = "cg_ammo",
  [17]   = "cg_ammo",
  [8]    = "medkit",

  -- weapons
  [2001] = "shotgun",
  [82]   = "shotgun",
  [2002] = "chaingun",
  [2003] = "rpg",
  [2004] = "chaingun",
  [2006] = "rpg",
}


function DUKE3D.remap_doom_thing(id)
  -- native Duke sprites, player starts, lights etc. are kept as is
  if type(id) ~= "number" then return id end
  if id >= 100000 then return id end
  if id >= 1 and id <= 4 then return id end
  if id == 11 then return id end

  local name = DUKE3D.DOOM_THING_REMAP[id]

  if not name then return nil end

  local info = GAME.ENTITIES[name] or
               GAME.MONSTERS[name] or
               GAME.WEAPONS[name] or
               GAME.PICKUPS[name] or
               GAME.NICE_ITEMS[name]

  if not info or not info.id or info.id == 0 then return nil end

  return info.id
end
