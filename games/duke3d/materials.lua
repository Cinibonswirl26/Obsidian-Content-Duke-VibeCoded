------------------------------------------------------------------------
--  DUKE NUKEM 3D MATERIALS
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
--  The room themes and prefabs are shared with DOOM, so they use DOOM
--  texture names.  Every DOOM texture and flat is translated into a
--  Duke Nukem 3D tile number here, which the map converter writes as
--  the wall or sector picnum.
--
--  The translation works with rules: the first rule whose pattern
--  matches the (upper-case) name decides the category, and a tile is
--  picked from the category's list using a hash of the name.  So the
--  same DOOM texture always becomes the same Duke tile, while similar
--  DOOM textures still get some variety.
--
--  All tiles used here exist in the shareware DUKE3D.GRP (v1.3D).
--
------------------------------------------------------------------------

-- the special texture name which the converter turns into a parallax sky
DUKE3D.sky_tex = "SKY"

-- parallax sky tile for each theme
DUKE3D.SKY_TILES =
{
  urban   = 89,  -- LA : Los Angeles skyline at night
  tech    = 95,  -- starry space sky
  hell    = 95,
  default = 89,
}


-- WALL TEXTURE CATEGORIES --

DUKE3D.WALL_TILES =
{
  switch      = { 162, 1111, 1122, 164 },
  switch_tech = { 162, 164 },
  switch_hand = { 1111 },
  switch_pull = { 1122 },

  door        = { 151, 156, 717, 1179, 346 },
  door_wood   = { 879 },
  door_big    = { 843, 1178, 1102 },
  door_trim   = { 797, 827, 791 },
  key_trim    = { 353 },

  light       = { 701, 702, 184, 310 },
  computer    = { 297, 293, 305, 873, 874, 875 },

  tech        = { 442, 718, 367, 709, 222, 1120 },
  tech_white  = { 413, 412, 191 },
  tech_red    = { 1173, 829 },
  metal       = { 442, 709, 367, 1120, 829 },
  pipes       = { 222, 1120, 367 },

  concrete    = { 757, 815, 1189, 1191, 1182, 802 },
  building    = { 783, 832, 763, 764, 757 },
  brick       = { 781, 750, 0, 748 },
  wood        = { 880, 1188 },
  crate       = { 884 },

  rock        = { 1169, 772, 240, 852 },
  dirt        = { 419, 1169 },

  gothic      = { 1101, 1120, 742, 1098 },
  flesh       = { 1100, 1140, 1141, 1133, 1104 },

  lava        = { 1082 },
  slime       = { 200 },
  water       = { 336 },
  blood       = { 899 },

  step        = { 0, 797, 355 },
  hazard      = { 355, 353 },
  black       = { 1156 },

  bars        = { 915 },
  fence       = { 913 },
  grate       = { 609, 595 },
  glass       = { 503 },

  generic     = { 757, 442, 750, 815, 718, 1189 },
}


DUKE3D.WALL_RULES =
{
  -- switches
  { "^SW[12]EXIT",  "switch_hand" },
  { "^SW[12]COMP",  "switch_tech" },
  { "^SW[12]TEK",   "switch_tech" },
  { "^SW[12]MET",   "switch_tech" },
  { "^SW[12]SKIN",  "switch_hand" },
  { "^SW[12]SKUL",  "switch_hand" },
  { "^SW[12]GARG",  "switch_hand" },
  { "^SW[12]LION",  "switch_hand" },
  { "^SW[12]SATYR", "switch_hand" },
  { "^SW[12]HOT",   "switch_hand" },
  { "^SW[12]VINE",  "switch_hand" },
  { "^SW[12]WOOD",  "switch_pull" },
  { "^SW[12]WDMET", "switch_pull" },
  { "^SW[12]BR",    "switch_pull" },
  { "^SW[12]DIRT",  "switch_pull" },
  { "^SW[12]ROCK",  "switch_pull" },
  { "^SW[12]",      "switch" },

  -- sky (shouldn't happen for walls, but be safe)
  { "SKY",          "sky" },

  -- doors
  { "^DOOR[BRY]",   "key_trim" },
  { "^DOORTRAK",    "door_trim" },
  { "^DOORSTOP",    "door_trim" },
  { "^EXITDOOR",    "door" },
  { "^SPCDOOR",     "door" },
  { "^BIGDOOR[5-7]", "door_wood" },
  { "^BIGDOOR",     "door_big" },
  { "^ZDOOR",       "door" },
  { "^ZELDOOR",     "door_wood" },
  { "DOOR",         "door" },

  -- liquids and other animated stuff
  { "^NUKAGE",      "slime" },
  { "^NUKE",        "slime" },
  { "^SFALL",       "slime" },
  { "^SLADPOIS",    "slime" },
  { "^BRNPOIS",     "slime" },
  { "^GRAYPOIS",    "slime" },
  { "^FWATER",      "water" },
  { "^WFALL",       "water" },
  { "^BFALL",       "blood" },
  { "^BLOD",        "blood" },
  { "^BLOOD",       "blood" },
  { "^LAVA",        "lava" },
  { "^FIRELAV",     "lava" },
  { "^FIREWAL",     "lava" },
  { "^FIREMAG",     "lava" },
  { "^FIREBLU",     "lava" },
  { "^CRACKLE",     "lava" },
  { "^ROCKRED",     "lava" },

  -- middle textures
  { "^MIDBARS",     "bars" },
  { "^MIDGRATE",    "fence" },
  { "^MIDVINE",     "fence" },
  { "^MIDSPACE",    "glass" },
  { "^MIDBR",       "grate" },
  { "^MIDBRONZ",    "grate" },
  { "^MID",         "grate" },
  { "BARS",         "bars" },
  { "GRATE",        "fence" },
  { "FENCE",        "fence" },

  -- lights and computers
  { "^LITE",        "light" },
  { "^TEKLITE",     "light" },
  { "LITE",         "light" },
  { "^COMP",        "computer" },
  { "^SPACEW",      "computer" },
  { "^PLANET",      "computer" },

  -- steps and trims
  { "^STEP",        "step" },
  { "^SUPPORT",     "door_trim" },
  { "^PLAT",        "door_trim" },
  { "^METAL$",      "door_trim" },
  { "^EXIT",        "hazard" },
  { "^O_BLACK",     "black" },
  { "^O_",          "black" },

  -- tech
  { "^STARTAN",     "tech" },
  { "^STARG",       "tech" },
  { "^STARBR",      "tech" },
  { "^TEKWALL",     "tech" },
  { "^TEKGREN",     "tech" },
  { "^TEKBRON",     "tech_red" },
  { "^SHAWN",       "metal" },
  { "^SILVER",      "tech_white" },
  { "^MODWALL",     "tech_white" },
  { "^METAL",       "metal" },
  { "^BRONZE",      "tech_red" },
  { "^PIPE",        "pipes" },
  { "^ICKWALL",     "tech_white" },
  { "^ICKDOOR",     "door" },
  { "^GRAY",        "concrete" },
  { "^PANEL",       "wood" },
  { "^PANBOOK",     "wood" },
  { "^PANCASE",     "wood" },
  { "^PANBORD",     "wood" },
  { "^PANRED",      "tech_red" },
  { "^PANBLUE",     "tech" },
  { "^PANBLACK",    "black" },

  -- urban
  { "^BRWINDOW",    "building" },
  { "^CEMENT",      "concrete" },
  { "^STUCCO",      "building" },
  { "^BROWN",       "concrete" },
  { "^BRNSMAL",     "brick" },
  { "^BRICK",       "brick" },
  { "^BIGBRIK",     "brick" },
  { "^BSTONE",      "brick" },
  { "^STONE",       "brick" },
  { "^BROVINE",     "brick" },
  { "^CRATE",       "crate" },
  { "^CRATINY",     "crate" },
  { "^WOODMET",     "metal" },
  { "^WOODGARG",    "gothic" },
  { "^WOOD",        "wood" },
  { "^ZZWOLF",      "brick" },

  -- natural
  { "^ROCK",        "rock" },
  { "^TANROCK",     "rock" },
  { "^ASHWALL",     "rock" },
  { "^SP_ROCK",     "rock" },
  { "^RROCK",       "rock" },
  { "^ZIMMER",      "rock" },
  { "^GRASS",       "dirt" },
  { "^DIRT",        "dirt" },
  { "^MUD",         "dirt" },

  -- hell (alien hive)
  { "^MARB",        "gothic" },
  { "^GST",         "gothic" },
  { "^SLAD",        "gothic" },
  { "^SP_",         "gothic" },
  { "^ZZZFACE",     "gothic" },
  { "^SKIN",        "flesh" },
  { "^SK_",         "flesh" },
  { "^SKSNAKE",     "flesh" },
  { "^SKSPINE",     "flesh" },
  { "^SLOPPY",      "flesh" },
  { "^REDWALL",     "flesh" },
  { "^DBRAIN",      "flesh" },
  { "^BLAKWAL",     "black" },
  { "^FLESH",       "flesh" },
}


-- FLAT CATEGORIES --

DUKE3D.FLAT_TILES =
{
  water       = { 336 },
  slime       = { 200 },
  lava        = { 1082 },
  blood       = { 899 },

  light       = { 701, 326, 310, 184 },
  metal       = { 755, 382, 1192, 1190 },
  tech        = { 755, 790, 191, 1190, 626 },
  tiles       = { 790, 815, 189, 191, 1205, 417, 398 },
  carpet      = { 899, 332 },
  wood        = { 749 },
  dirt        = { 782, 1219, 771, 372, 419, 1185 },
  grass       = { 1219 },
  rock        = { 1185, 372, 782 },
  alien       = { 1105, 1133, 1104, 1100 },
  console     = { 1190 },
  teleport    = { 626 },
  black       = { 1156 },

  generic     = { 790, 815, 755, 191 },
}


DUKE3D.FLAT_RULES =
{
  { "SKY",          "sky" },

  -- liquids
  { "^FWATER",      "water" },
  { "^WATER",       "water" },
  { "^NUKAGE",      "slime" },
  { "^SLIME0",      "slime" },
  { "^SLIME1[0-2]", "slime" },
  { "^LAVA",        "lava" },
  { "^BLOOD",       "blood" },

  -- lights
  { "^TLITE",       "light" },
  { "^CEIL1_[23]",  "light" },
  { "^GRNLITE",     "light" },
  { "^FLOOR1_7",    "light" },
  { "^FLAT17",      "light" },
  { "^FLAT2$",      "light" },
  { "^FLAT22",      "light" },
  { "^CEIL3_[46]",  "light" },
  { "^CEIL4_3",     "light" },
  { "LITE",         "light" },

  -- special
  { "^GATE",        "teleport" },
  { "^CONS",        "console" },
  { "^COMP",        "console" },
  { "^O_",          "black" },
  { "^DOORTRAK",    "metal" },
  { "^LIFT",        "metal" },
  { "^STEP",        "metal" },
  { "^PLAT",        "metal" },
  { "^CRATOP",      "wood" },
  { "^SLIME1[3-6]", "tiles" },

  -- carpets and wood
  { "^FLOOR1_1",    "carpet" },
  { "^FLAT5_[12]",  "wood" },
  { "^CEIL1_1",     "wood" },
  { "^FLOOR7_1",    "wood" },
  { "^FLAT5_5",     "carpet" },

  -- tech
  { "^CEIL5",       "metal" },
  { "^FLOOR4",      "tech" },
  { "^FLOOR5",      "tech" },
  { "^FLAT4",       "tech" },
  { "^FLAT14",      "tech" },
  { "^FLAT20",      "metal" },
  { "^FLAT23",      "metal" },
  { "^SFLR",        "metal" },
  { "^CEIL3",       "tech" },
  { "^CEIL4",       "tech" },
  { "^MFLR",        "rock" },
  { "^FLAT5_4",     "tech" },
  { "^FLAT9",       "tech" },
  { "^FLAT3",       "tech" },
  { "^FLAT19",      "metal" },
  { "^DEM1",        "metal" },

  -- tiles and concrete
  { "^FLOOR0",      "tiles" },
  { "^FLOOR1",      "tiles" },
  { "^FLOOR3",      "tiles" },
  { "^FLAT1",       "tiles" },
  { "^FLAT8",       "rock" },
  { "^FLAT10",      "rock" },
  { "^FLAT18",      "tiles" },
  { "^FLAT5",       "tiles" },
  { "^GRAY",        "tiles" },

  -- outdoor stuff
  { "^GRASS",       "grass" },
  { "^RROCK",       "rock" },
  { "^GRNROCK",     "rock" },
  { "^FLOOR6",      "dirt" },
  { "^ASH",         "dirt" },
  { "^MUD",         "dirt" },
  { "^DIRT",        "dirt" },

  -- hell (alien hive)
  { "^FLAT5_[3678]", "alien" },
  { "^FLOOR7",      "alien" },
  { "^SFLR6",       "alien" },
  { "^SKIN",        "alien" },
  { "^MARB",        "alien" },
}

-- explicit translations for the textures which the DOOM room themes use
-- the most, so that rooms get a good variety of Duke tiles.

DUKE3D.WALL_EXACT =
{
  GRAY1    = 815,  GRAY4    = 191,  GRAY5    = 1189, GRAY7    = 0,
  GRAYBIG  = 757,  GRAYTALL = 757,

  WOOD1    = 880,  WOOD3    = 1188, WOOD5    = 880,  WOOD9    = 884,
  WOOD12   = 1188, WOODVERT = 1188, WOODMET1 = 829,

  ROCK1    = 1169, ROCK2    = 772,  ROCK3    = 240,  ROCK4    = 852,
  SP_ROCK1 = 240,  TANROCK5 = 1169, TANROCK7 = 772,  ZIMMER5  = 852,
  ASHWALL2 = 240,  ASHWALL3 = 772,  ASHWALL4 = 240,

  BROWN1   = 783,  BROWNGRN = 748,  BROWN96  = 829,  BROWNHUG = 1191,

  BIGBRIK1 = 781,  BIGBRIK2 = 750,  BSTONE1  = 781,  BRICK10  = 781,
  STONE2   = 750,  STONE3   = 0,    STONE4   = 742,  STONE5   = 748,
  STONE6   = 750,  STONE7   = 1182, STONE    = 0,

  STARTAN2 = 442,  STARTAN3 = 442,  STARG1   = 413,  STARG2   = 413,
  STARG3   = 413,  STARGR1  = 412,  STARGR2  = 412,  STARBR2  = 829,

  METAL1   = 367,  METAL2   = 709,  SHAWN2   = 1120, SILVER1  = 413,
  BRONZE1  = 829,  COMPBLUE = 873,  TEKGREN2 = 718,  TEKWALL1 = 222,
  TEKWALL4 = 222,  ICKWALL1 = 1189, ICKWALL3 = 1191,

  GSTONE1  = 1101, GSTVINE2 = 1101, MARBGRAY = 742,  MARBLE1  = 1098,
  SKIN2    = 1100, SKSPINE2 = 1140, SP_HOT1  = 1082,

  CEMENT7  = 802,  CEMENT9  = 757,  STUCCO3  = 783,

  PANEL2   = 880,  PANEL3   = 1188, PANEL6   = 880,  PANEL8   = 1188,
  PANEL9   = 880,
}


DUKE3D.FLAT_EXACT =
{
  FLAT1    = 790,  FLAT1_1  = 417,  FLAT1_2  = 790,  FLAT3    = 189,
  FLAT4    = 191,  FLAT5    = 398,  FLAT5_1  = 749,  FLAT5_2  = 749,
  FLAT5_4  = 815,  FLAT5_5  = 899,  FLAT8    = 1185, FLAT10   = 372,
  FLAT14   = 626,  FLAT18   = 815,  FLAT19   = 1192,

  FLOOR0_1 = 189,  FLOOR0_2 = 1205, FLOOR0_3 = 1205, FLOOR0_5 = 417,
  FLOOR3_3 = 189,  FLOOR4_6 = 755,  FLOOR4_8 = 755,  FLOOR5_1 = 755,
  FLOOR5_3 = 382,  FLOOR5_4 = 382,  FLOOR6_2 = 815,  FLOOR7_1 = 771,
  FLOOR7_2 = 1105,

  CEIL1_1  = 749,  CEIL3_2  = 191,  CEIL3_3  = 1190, CEIL3_5  = 1190,
  CEIL5_1  = 1190,

  MFLR8_2  = 1185, MFLR8_3  = 782,  MFLR8_4  = 1185, DEM1_6   = 382,

  RROCK03  = 782,  RROCK09  = 1219, RROCK13  = 372,  RROCK16  = 1219,
  GRNROCK  = 1219, SLIME14  = 417,  SLIME15  = 417,
}


local function name_hash(name)
  local h = 0

  for i = 1, #name do
    h = (h * 31 + string.byte(name, i)) % 65521
  end

  return h
end


local function translate(name, rules, tiles, exact)
  if not name or name == "" or name == "-" then return nil end

  local upper = string.upper(name)

  -- already a tile number?
  if string.match(upper, "^%d+$") then return upper end

  if exact[upper] then return tostring(exact[upper]) end

  local category = "generic"

  for _,rule in ipairs(rules) do
    if string.match(upper, rule[1]) then
      category = rule[2]
      break
    end
  end

  if category == "sky" then return DUKE3D.sky_tex end

  local list = assert(tiles[category])

  return tostring(list[1 + name_hash(upper) % #list])
end


function DUKE3D.wall_tile(name)
  return translate(name, DUKE3D.WALL_RULES, DUKE3D.WALL_TILES, DUKE3D.WALL_EXACT)
end


function DUKE3D.flat_tile(name)
  return translate(name, DUKE3D.FLAT_RULES, DUKE3D.FLAT_TILES, DUKE3D.FLAT_EXACT)
end


-- make a Duke material from a DOOM material (or a single DOOM name)
function DUKE3D.convert_material(src, name)
  local mat = {}

  if src then
    for k,v in pairs(src) do
      mat[k] = v
    end
  end

  local t = (src and src.t) or name
  local f = (src and src.f) or name

  mat.t = DUKE3D.wall_tile(t) or DUKE3D.WALL_TILES.generic[1]
  mat.f = DUKE3D.flat_tile(f) or DUKE3D.FLAT_TILES.generic[1]

  return mat
end


-- used by Mat_lookup_tex and Mat_lookup_flat for names which are not
-- in the materials table (e.g. textures used directly by prefabs).
function DUKE3D.material_fallback(name)
  return DUKE3D.convert_material(nil, name)
end


DUKE3D.MATERIALS = {}

for name,src in pairs(DOOM.MATERIALS) do
  DUKE3D.MATERIALS[name] = DUKE3D.convert_material(src, name)
end

-- special materials --

DUKE3D.MATERIALS._ERROR   = { t="757", f="790" }
DUKE3D.MATERIALS._DEFAULT = { t="757", f="790" }
DUKE3D.MATERIALS._SKY     = { t="757", f=DUKE3D.sky_tex }
DUKE3D.MATERIALS.F_SKY1   = { t="757", f=DUKE3D.sky_tex }


-- in Duke the floor tile itself does the damage
DUKE3D.LIQUIDS =
{
  water  = { mat="FWATER1", special=0 },
  blood  = { mat="BLOOD1",  special=0 },
  nukage = { mat="NUKAGE1", light_add=24, special=0, damage=5 },
  lava   = { mat="LAVA1",   light_add=56, special=0, damage=10 },
  slime  = { mat="SLIME01", light_add=8,  special=0, damage=5 },
}


DUKE3D.PREFAB_FIELDS = {}

DUKE3D.SKIN_DEFAULTS = {}
