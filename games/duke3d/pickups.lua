------------------------------------------------------------------------
--  DUKE NUKEM 3D PICKUPS
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
--  Amounts are taken from USER.CON and GAME.CON.
--
------------------------------------------------------------------------

local S = DUKE3D.sprite

DUKE3D.PICKUPS =
{
  -- HEALTH --

  cola =
  {
    id = S(51),
    kind = "health",
    add_prob = 60,
    cluster = { 2,5 },
    give = { {health=10} },
    storage_prob = 40,
    storage_qty = 5
  },

  sixpak =
  {
    id = S(52),
    kind = "health",
    rank = 2,
    add_prob = 100,
    closet_prob = 20,
    secret_prob = 5,
    storage_prob = 80,
    storage_qty = 2,
    give = { {health=30} }
  },

  -- AMMO --

  clip =
  {
    id = S(40),
    kind = "ammo",
    add_prob = 30,
    cluster = { 2,4 },
    give = { {ammo="clip",count=12} },
  },

  cg_ammo =
  {
    id = S(41),
    kind = "ammo",
    rank = 2,
    add_prob = 50,
    storage_prob = 15,
    storage_qty = 3,
    give = { {ammo="cg_ammo",count=50} },
  },

  shells =
  {
    id = S(49),
    kind = "ammo",
    add_prob = 40,
    cluster = { 1,3 },
    give = { {ammo="shell",count=10} },
  },

  rockets =
  {
    id = S(44),
    kind = "ammo",
    rank = 2,
    add_prob = 30,
    closet_prob = 20,
    secret_prob = 5,
    storage_prob = 20,
    storage_qty = 3,
    give = { {ammo="rocket",count=5} },
  },

  pipebombs =
  {
    id = S(47),
    kind = "ammo",
    add_prob = 15,
    give = { {ammo="pipebomb",count=5} },
  },

  crystals =
  {
    id = S(46),
    kind = "ammo",
    add_prob = 15,
    give = { {ammo="crystal",count=5} },
  },

  dev_ammo =
  {
    id = S(42),
    kind = "ammo",
    add_prob = 20,
    give = { {ammo="dev_ammo",count=15} },
  },

  ice =
  {
    id = S(37),
    kind = "ammo",
    add_prob = 20,
    give = { {ammo="ice",count=25} },
  },
}


DUKE3D.NICE_ITEMS =
{
  armor =
  {
    id = S(54),
    kind = "armor",
    add_prob = 40,
    start_prob = 60,
    crazy_prob = 5,
    closet_prob = 10,
    secret_prob = 20,
    give = { {health=50} },
  },

  atomic_health =
  {
    id = S(100),
    kind = "health",
    level = 2,
    add_prob = 5,
    start_prob = 0,
    closet_prob = 5,
    secret_prob = 40,
    give = { {health=50} },
  },

  -- the portable medkit is an inventory item
  medkit =
  {
    id = S(53),
    kind = "health",
    add_prob = 15,
    start_prob = 20,
    closet_prob = 10,
    secret_prob = 20,
    give = { {health=40} },
  },

  steroids =
  {
    id = S(55),
    kind = "powerup",
    add_prob = 5,
    secret_prob = 10,
    time_limit = 60,
  },

  scuba =
  {
    id = S(56),
    kind = "powerup",
    secret_prob = 5,
    time_limit = 120,
  },

  jetpack =
  {
    id = S(57),
    kind = "powerup",
    add_prob = 2,
    secret_prob = 15,
    time_limit = 60,
  },

  nightvision =
  {
    id = S(59),
    kind = "powerup",
    add_prob = 3,
    secret_prob = 10,
    time_limit = 120,
  },

  boots =
  {
    id = S(61),
    kind = "powerup",
    add_prob = 5,
    closet_prob = 5,
    time_limit = 60,
  },

  holoduke =
  {
    id = S(1348),
    kind = "powerup",
    add_prob = 2,
    secret_prob = 10,
    time_limit = 60,
  },
}


DUKE3D.SHAREWARE_MISSING_ITEMS =
{
  crystals = true,
  dev_ammo = true,
  ice      = true,
}
