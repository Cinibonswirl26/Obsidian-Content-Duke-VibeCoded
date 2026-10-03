------------------------------------------------------------------------
--  DUKE NUKEM 3D MONSTERS
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
--  Health values are the "strength" values from USER.CON.  The other
--  fields have the same meaning as for the DOOM monsters, see the
--  file games/doom/monsters.lua for a description.
--
--  Sizes are given in DOOM units (one DOOM unit is 16 Build units).
--
------------------------------------------------------------------------

local S = DUKE3D.sprite

DUKE3D.MONSTERS =
{
  -- Assault Trooper
  trooper =
  {
    id = S(1680),
    r = 20,
    h = 56,
    level = 1,
    prob = 140,
    health = 30,
    damage = 1.5,
    attack = "hitscan",
    density = 1.3,
    weap_prefs = { shotgun=1.2, chaingun=1.4 },
    room_size = "any",
    trap_factor = 0.75,
    infight_damage = 2.0,
  },

  -- Assault Captain (a trooper with a different palette, which
  -- teleports around)
  captain =
  {
    id = S(1680, 21),
    r = 20,
    h = 56,
    level = 2.5,
    prob = 30,
    health = 50,
    damage = 2.0,
    attack = "hitscan",
    density = 0.7,
    weap_prefs = { shotgun=1.2, chaingun=1.4 },
    species = "trooper",
    replaces = "trooper",
    replace_prob = 15,
    room_size = "any",
    trap_factor = 0.75,
    infight_damage = 2.5,
  },

  -- Pig Cop
  pigcop =
  {
    id = S(2000),
    r = 20,
    h = 64,
    level = 1.5,
    prob = 100,
    health = 100,
    damage = 4.0,
    attack = "hitscan",
    density = 0.9,
    give = { {weapon="shotgun"}, {ammo="shell",count=4} },
    weap_prefs = { shotgun=1.4, chaingun=1.3, rpg=0.8 },
    room_size = "any",
    trap_factor = 1.0,
    infight_damage = 6.0,
  },

  -- Enforcer (Lizard man)
  enforcer =
  {
    id = S(2120),
    r = 20,
    h = 64,
    level = 2.5,
    prob = 70,
    health = 100,
    damage = 4.5,
    attack = "hitscan",
    density = 0.8,
    weap_prefs = { shotgun=1.3, chaingun=1.5, rpg=1.0 },
    room_size = "any",
    trap_factor = 0.8,
    infight_damage = 7.0,
  },

  -- Octabrain
  octabrain =
  {
    id = S(1820),
    r = 24,
    h = 64,
    level = 3.5,
    prob = 40,
    health = 175,
    damage = 5.0,
    attack = "missile",
    density = 0.5,
    float = true,
    weap_min_damage = 30,
    weap_prefs = { chaingun=1.5, rpg=1.3, shotgun=1.0 },
    room_size = "any",
    trap_factor = 0.5,
    infight_damage = 10,
  },

  -- Sentry Drone (flies at the player and explodes)
  drone =
  {
    id = S(1880),
    r = 16,
    h = 40,
    level = 3,
    prob = 25,
    health = 150,
    damage = 4.0,
    attack = "melee",
    density = 0.4,
    float = true,
    weap_prefs = { chaingun=1.5, shotgun=1.2 },
    room_size = "any",
    trap_factor = 0.4,
    cage_factor = 0,
    infight_damage = 5,
  },

  -- Assault Commander
  commander =
  {
    id = S(1920),
    r = 32,
    h = 72,
    level = 5,
    prob = 20,
    health = 350,
    damage = 9.0,
    attack = "missile",
    density = 0.3,
    float = true,
    weap_min_damage = 50,
    weap_prefs = { rpg=1.6, chaingun=1.4, devastator=1.6 },
    room_size = "large",
    trap_factor = 0.4,
    infight_damage = 25,
  },

  -- Recon Patrol Vehicle
  recon =
  {
    id = S(1960),
    r = 24,
    h = 48,
    level = 2,
    prob = 20,
    health = 50,
    damage = 3.0,
    attack = "hitscan",
    density = 0.5,
    float = true,
    weap_prefs = { chaingun=1.4, shotgun=1.2 },
    room_size = "any",
    trap_factor = 0.5,
    cage_factor = 0,
    infight_damage = 3,
  },

  -- Protozoid Slimer
  slimer =
  {
    id = S(2370),
    r = 16,
    h = 24,
    level = 2,
    prob = 15,
    health = 10,
    damage = 1.0,
    attack = "melee",
    density = 0.8,
    weap_prefs = { shotgun=1.0, kick=1.2 },
    room_size = "small",
    trap_factor = 0.3,
    cage_factor = 0,
    infight_damage = 0.5,
  },

  -- Mini Battlelord (a coloured Battlelord, which does not end the
  -- episode when killed).
  battlelord =
  {
    id = S(2630, 21),
    r = 40,
    h = 112,
    level = 7,
    boss_type = "tough",
    boss_prob = 50,
    prob = 2,
    crazy_prob = 10,
    health = 1000,
    damage = 30,
    attack = "missile",
    density = 0.1,
    weap_min_damage = 100,
    weap_prefs = { rpg=2.0, devastator=3.0, chaingun=1.2 },
    room_size = "large",
    infight_damage = 300,
    cage_factor = 0,
  },
}



--
-- The episode bosses.  They are never placed at random, only on the
-- boss level at the end of each episode (see levels.lua), where killing
-- the boss ends the episode just like in the original game.
--
DUKE3D.EPISODE_BOSSES =
{
  battlelord_boss =
  {
    id = S(2630),
    r = 40,
    h = 112,
    level = 1,
    prob = 0,
    boss_prob = 0,
    boss_type = "tough",
    health = 4500,
    damage = 30,
    attack = "missile",
    density = 0.1,
    weap_prefs = { rpg=2.0, devastator=3.0, chaingun=1.2 },
    room_size = "large",
    infight_damage = 300,
    cage_factor = 0,
  },

  overlord =
  {
    id = S(2710),
    r = 40,
    h = 112,
    level = 1,
    prob = 0,
    boss_prob = 0,
    boss_type = "tough",
    health = 4500,
    damage = 35,
    attack = "missile",
    density = 0.1,
    weap_prefs = { rpg=2.0, devastator=3.0 },
    room_size = "large",
    infight_damage = 300,
    cage_factor = 0,
  },

  cycloid =
  {
    id = S(2760),
    r = 40,
    h = 128,
    level = 1,
    prob = 0,
    boss_prob = 0,
    boss_type = "tough",
    health = 4500,
    damage = 35,
    attack = "missile",
    density = 0.1,
    weap_prefs = { rpg=2.0, devastator=3.0 },
    room_size = "large",
    infight_damage = 300,
    cage_factor = 0,
  },

  alien_queen =
  {
    id = S(4740),
    r = 40,
    h = 112,
    level = 1,
    prob = 0,
    boss_prob = 0,
    boss_type = "tough",
    health = 4500,
    damage = 35,
    attack = "melee",
    density = 0.1,
    weap_prefs = { rpg=2.0, devastator=3.0 },
    room_size = "large",
    infight_damage = 300,
    cage_factor = 0,
  },
}

for name,info in pairs(DUKE3D.EPISODE_BOSSES) do
  DUKE3D.MONSTERS[name] = info
end


-- Only the trooper, pig cop, octabrain, recon vehicle and Battlelord
-- are present in the shareware version.
DUKE3D.SHAREWARE_MISSING_MONSTERS =
{
  enforcer  = true,
  drone     = true,
  commander = true,
  slimer    = true,

  overlord    = true,
  cycloid     = true,
  alien_queen = true,
}
