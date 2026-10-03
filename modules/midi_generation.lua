MIDI_CONFIG = {}

MIDI_CONFIG.CHOICES =
{
  "sane",  _("Safe Defaults"),
  "classical", _("Classical"),
  "doom", _("Doomish"),
  "jazz", _("Jazz"),
}

function MIDI_CONFIG.setup(self)
  module_param_up(self)
end

function MIDI_CONFIG.all_done()
  for _,song in pairs(GAME.RESOURCES.MUSIC_LUMPS) do
    gui.prog_step("Generating MIDI...")
    gui.generate_midi_track("scripts/midi/" .. PARAM.midi_config_selection .. ".steve.json", song)
  end
end

OB_MODULES["midi_generation"] =
{

  name = "midi_generation",

  label = _("MIDI Generation [Unstable]"),

  game = "!duke3d",

  where = "experimental",
  priority = 5,

  tooltip = _("Procedurally generate replacement MIDI tracks"),

  hooks =
  {
    setup = MIDI_CONFIG.setup,
    all_done = MIDI_CONFIG.all_done
  },

  options =
  {
    {
      name = "midi_config_selection",
      label=_("Generator Config"),
      choices=MIDI_CONFIG.CHOICES,
      default = "doom",
      tooltip = _("Choose which procedural MIDI config to use")
    },
  },
}