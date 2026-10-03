------------------------------------------------------------------------
--  DUKE NUKEM 3D PARAMETERS
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

-- largest "Level Size" setting which is allowed, see base.lua
DUKE3D.MAX_LEVEL_SIZE = 26


DUKE3D.PARAMETERS =
{
  -- Duke has transporters, but they need sector effector pairs which
  -- the converter does not create (yet).
  teleporters = false,

  jump_height = 24,

  -- one DOOM unit becomes 16 Build units, and the Build editors limit
  -- maps to +/- 131072 units.
  map_limit = 12800,

  max_name_length = 28,

  skip_monsters = { 20,30 },

  monster_factor = 1.0,
  health_factor  = 1.0,
  ammo_factor    = 1.0,
  time_factor    = 1.0,

  mon_along_factor = 7.0,

  -- reduce the amount of detail (wall groups, plain walls) earlier
  -- than for DOOM, since the Build engine has much lower limits on
  -- the number of sectors and walls.
  autodetail_svolume_kickin   = 350,
  autodetail_perimeter_kickin = 450,
}


DUKE3D.ACTIONS =
{
  --
  -- These keywords are used by prefabs that are remotely triggered
  -- (by a switch or walk-over line).  They use the DOOM special numbers,
  -- the map converter turns them into Duke switches, ACTIVATOR and
  -- TOUCHPLATE sprites.
  --

  S1_OpenDoor = { id=103,  kind="open" },
  W1_OpenDoor = { id=2,    kind="open" },
  GR_OpenDoor = { id=46,   kind="open" },

  W1_OpenDoorFast = { id=109, kind="open" },

  S1_UnlockBlue   = { id=133, kind="unlock" },
  S1_UnlockRed    = { id=135, kind="unlock" },
  S1_UnlockYellow = { id=137, kind="unlock" },

  S1_RaiseStair = { id=127,  kind="stair" },
  W1_RaiseStair = { id=100,  kind="stair" },

  S1_FloorUp  = { id=18,   kind="floor_up" },
  W1_FloorUp  = { id=119,  kind="floor_up" },

  S1_LowerFloor = { id=23, kind="lower" },
  W1_LowerFloor = { id=38, kind="lower" },
}
