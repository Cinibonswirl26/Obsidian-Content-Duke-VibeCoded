----------------------------------------------------------------
--  MODULE: Duke Nukem 3D episode choice
----------------------------------------------------------------
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

DUKE3D_EPISODE = {}

DUKE3D_EPISODE.CHOICES =
{
  "1", _("E1: L.A. Meltdown"),
  "2", _("E2: Lunar Apocalypse"),
  "3", _("E3: Shrapnel City"),
  "4", _("E4: The Birth"),
}

function DUKE3D_EPISODE.setup(self)
  module_param_up(self)
end

OB_MODULES["duke3d_episode"] =
{
  name = "duke3d_episode",

  label = _("Duke Nukem 3D Episode"),

  game = "duke3d",

  where = "other",
  priority = 90,

  tooltip = _("Which episode of Duke Nukem 3D to make when the length is a single level, a few levels or one episode. Episodes 2 to 4 need the full version of the game, episode 4 needs the Atomic Edition."),

  hooks =
  {
    setup = DUKE3D_EPISODE.setup,
  },

  options =
  {
    {
      name = "duke_episode",
      label = _("Episode"),
      choices = DUKE3D_EPISODE.CHOICES,
      default = "1",
      tooltip = _("The episode replaced by the generated levels."),
      priority = 100,
    },
  },
}
