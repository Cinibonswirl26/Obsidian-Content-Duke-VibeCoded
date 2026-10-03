//------------------------------------------------------------------------
//  LEVEL building - Duke Nukem 3D (Build engine) format
//------------------------------------------------------------------------
//
//  OBSIDIAN Level Maker
//
//  Copyright (C) 2021-2025 The OBSIDIAN Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//------------------------------------------------------------------------

#pragma once

namespace Duke
{

// Duke Nukem 3D tile numbers used by the map converter
// (names match the NAMES.H file of the original game).
enum tile_e
{
    SECTOREFFECTOR = 1,
    ACTIVATOR      = 2,
    TOUCHPLATE     = 3,
    ACCESSCARD     = 60,
    LA_SKY         = 89,
    ACCESSSWITCH   = 130,
    DIPSWITCH      = 162,
    DIPSWITCH2     = 164,
    TECHSWITCH     = 166,
    DIPSWITCH3     = 168,
    ACCESSSWITCH2  = 170,
    HANDSWITCH     = 1111,
    PULLSWITCH     = 1122,
    ALIENSWITCH    = 1142,
    APLAYER        = 1405,
};

// Things whose editor number is at least this value are native Duke
// sprites: the number encodes  ENTITY_BASE + pal * 10000 + picnum.
constexpr int ENTITY_BASE = 100000;

} // namespace Duke

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
