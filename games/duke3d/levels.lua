------------------------------------------------------------------------
--  DUKE NUKEM 3D LEVELS
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
--  The episodes follow the structure of the original game (Atomic
--  Edition):
--
--    E1  L.A. Meltdown      : L1-L5, Battlelord on L5,
--                             secret L6 (from L4), secret L7 (from L5)
--    E2  Lunar Apocalypse   : L1-L9, Overlord on L9,
--                             secret L10 (from L5), secret L11 (from L8)
--    E3  Shrapnel City      : L1-L9, Cycloid Emperor on L9,
--                             secret L10 (from L5), secret L11 (from L8)
--    E4  The Birth          : L1-L10, Alien Queen on L10,
--                             secret L11 (from L5)
--
--  The shareware version only has episode 1, without level 7.
--
--  On the boss level killing the boss ends the episode, as in the
--  original game.  Secret exits are NUKEBUTTONs which take the player
--  to the secret level, and the secret level returns to the level after
--  the one with the secret exit.
--
--  Levels are named E#L# so that the MAP files in the GRP replace the
--  original levels, e.g. with EDuke32:
--
--      eduke32 -grp OBSIDIAN.GRP
--
--  A single level can also be played directly:
--
--      eduke32 -map E1L1.MAP
--
------------------------------------------------------------------------

DUKE3D.EPISODES =
{
  episode1 =
  {
    ep_index = 1,
    title = "L.A. Meltdown",
    theme = "urban",
    dark_prob = 20,

    normal_levels = 5,
    boss_level = 5,
    boss = "battlelord_boss",

    -- secret level = level with the secret exit
    secret_levels = { [6]=4, [7]=5 },
  },

  episode2 =
  {
    ep_index = 2,
    title = "Lunar Apocalypse",
    theme = "tech",
    dark_prob = 10,

    normal_levels = 9,
    boss_level = 9,
    boss = "overlord",

    secret_levels = { [10]=5, [11]=8 },
  },

  episode3 =
  {
    ep_index = 3,
    title = "Shrapnel City",
    theme = "urban",
    dark_prob = 30,

    normal_levels = 9,
    boss_level = 9,
    boss = "cycloid",

    secret_levels = { [10]=5, [11]=8 },
  },

  episode4 =
  {
    ep_index = 4,
    title = "The Birth",
    theme = "hell",
    dark_prob = 20,

    normal_levels = 10,
    boss_level = 10,
    boss = "alien_queen",

    secret_levels = { [11]=5 },
  },
}


-- the shareware DUKE3D.GRP only has E1L1 - E1L6
DUKE3D.SHAREWARE_MAX_LEVEL = 6


local function selected_episode()
  local ep = tonumber(PARAM.duke_episode or OB_CONFIG.duke_episode or 1) or 1

  if GAME.shareware then ep = 1 end

  return math.clamp(1, ep, 4)
end


local function add_episode_levels(EPI, info, max_maps)
  -- the order in which the levels are generated : normal levels, with
  -- each secret level following the level containing its secret exit.
  local order = {}

  for map = 1, info.normal_levels do
    table.insert(order, map)

    for secret,from in pairs(info.secret_levels) do
      if from == map then
        table.insert(order, secret)
      end
    end
  end

  local count = 0

  for _,map in ipairs(order) do
    if max_maps and count >= max_maps then break end

    local is_secret = (map > info.normal_levels)

    if GAME.shareware and map > DUKE3D.SHAREWARE_MAX_LEVEL then
      goto next_map
    end

    do
      local along = math.min(map, info.normal_levels) / info.normal_levels

      local LEV =
      {
        episode  = EPI,
        name     = string.format("E%dL%d", info.ep_index, map),
        duke_map = map,

        ep_along = along,
        game_along = along,
      }

      if OB_CONFIG.length == "single" then
        LEV.ep_along   = 0.75
        LEV.game_along = 0.57
      end

      if is_secret then
        LEV.is_secret = true
      end

      -- secret exit to a secret level?
      for secret,from in pairs(info.secret_levels) do
        if from == map and not max_maps and
           not (GAME.shareware and secret > DUKE3D.SHAREWARE_MAX_LEVEL) then
          LEV.secret_exit = true
          LEV.duke_secret_target = secret
        end
      end

      -- the episode boss
      if map == info.boss_level and not max_maps then
        LEV.forced_boss = info.boss
        LEV.duke_boss_level = true
        LEV.dist_to_end = 1
      elseif map == info.boss_level - 1 then
        LEV.dist_to_end = 2
      end

      LEV.has_streets = false
      LEV.is_nature = false

      if PARAM.float_streets_mode and rand.odds(PARAM.float_streets_mode) then
        LEV.has_streets = true
      elseif PARAM.float_nature_mode and rand.odds(PARAM.float_nature_mode) then
        LEV.is_nature = true
      end

      table.insert(EPI.levels, LEV)
      table.insert(GAME.levels, LEV)

      count = count + 1
    end

    ::next_map::
  end
end


function DUKE3D.get_levels()
  for ep_index = 1,4 do
    local ep_info = DUKE3D.EPISODES["episode" .. ep_index]
    assert(ep_info)

    local EPI = table.copy(ep_info)
    EPI.levels = { }

    table.insert(GAME.episodes, EPI)
  end

  if OB_CONFIG.length == "game" then
    local last = sel(GAME.shareware, 1, 4)

    for ep_index = 1, last do
      add_episode_levels(GAME.episodes[ep_index], DUKE3D.EPISODES["episode" .. ep_index])
    end

    -- spread the monster progression over the whole game
    for _,LEV in pairs(GAME.levels) do
      local ep = LEV.episode.ep_index
      LEV.game_along = (ep - 1 + LEV.ep_along) / last
    end

    return
  end

  local ep_index = selected_episode()
  local EPI  = GAME.episodes[ep_index]
  local info = DUKE3D.EPISODES["episode" .. ep_index]

  if OB_CONFIG.length == "episode" then
    add_episode_levels(EPI, info)
  elseif OB_CONFIG.length == "few" then
    add_episode_levels(EPI, info, 4)
  else
    add_episode_levels(EPI, info, 1)
  end
end
