------------------------------------------------------------------------
--  DUKE NUKEM 3D WEAPONS
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
--  Damage values are based on the weapon strengths in USER.CON, the
--  meaning of the fields is the same as for the DOOM weapons.
--
--  Ammo types:
--    clip      : pistol ammo
--    cg_ammo   : chaingun ammo
--    shell     : shotgun shells
--    rocket    : RPG rockets
--    pipebomb  : pipe bombs
--    crystal   : shrinker crystals
--    dev_ammo  : devastator rockets
--    ice       : freezethrower ammo
--
------------------------------------------------------------------------

local S = DUKE3D.sprite

DUKE3D.WEAPONS =
{
  kick =
  {
    attack = "melee",
    rate = 1.5,
    damage = 10,
  },

  pistol =
  {
    pref = 5,
    attack = "hitscan",
    rate = 3.0,
    accuracy = 75,
    damage = 8,
    ammo = "clip",
    per = 1,
  },

  shotgun =
  {
    id = S(28),
    level = 1,
    pref = 50,
    add_prob = 50,
    attack = "hitscan",
    rate = 1.1,
    accuracy = 65,
    damage = 70,
    splash = { 15 },
    ammo = "shell",
    per = 1,
    give = { {ammo="shell",count=10} },
    bonus_ammo = 10,
  },

  chaingun =
  {
    id = S(22),
    level = 1.5,
    pref = 70,
    upgrades = "pistol",
    add_prob = 45,
    attack = "hitscan",
    rate = 10,
    accuracy = 80,
    damage = 9,
    ammo = "cg_ammo",
    per = 1,
    give = { {ammo="cg_ammo",count=50} },
    bonus_ammo = 50,
  },

  pipebomb =
  {
    id = S(26),
    level = 2,
    pref = 15,
    add_prob = 20,
    attack = "missile",
    rate = 0.8,
    accuracy = 60,
    damage = 140,
    splash = { 60,20,5 },
    ammo = "pipebomb",
    per = 1,
    give = { {ammo="pipebomb",count=1} },
    bonus_ammo = 2,
  },

  rpg =
  {
    id = S(23),
    level = 3,
    pref = 35,
    add_prob = 60,
    hide_prob = 10,
    attack = "missile",
    rate = 1.2,
    accuracy = 80,
    damage = 140,
    splash = { 65,20,5 },
    ammo = "rocket",
    per = 1,
    give = { {ammo="rocket",count=5} },
    bonus_ammo = 5,
  },

  shrinker =
  {
    id = S(25),
    level = 4,
    pref = 10,
    add_prob = 15,
    hide_prob = 20,
    attack = "missile",
    rate = 1.0,
    accuracy = 80,
    -- shrunk monsters can be stomped, so the damage is fairly high
    damage = 60,
    ammo = "crystal",
    per = 1,
    give = { {ammo="crystal",count=5} },
    bonus_ammo = 5,
  },

  freezer =
  {
    id = S(24),
    level = 4.5,
    pref = 20,
    add_prob = 25,
    attack = "missile",
    rate = 8,
    accuracy = 75,
    damage = 16,
    ammo = "ice",
    per = 1,
    give = { {ammo="ice",count=25} },
    bonus_ammo = 25,
  },

  devastator =
  {
    id = S(29),
    level = 6,
    pref = 25,
    add_prob = 30,
    hide_prob = 25,
    attack = "missile",
    rate = 6,
    accuracy = 70,
    damage = 38,
    splash = { 20,10 },
    ammo = "dev_ammo",
    per = 1,
    give = { {ammo="dev_ammo",count=15} },
    bonus_ammo = 15,
  },
}


DUKE3D.SHAREWARE_MISSING_WEAPONS =
{
  shrinker   = true,
  freezer    = true,
  devastator = true,
}
