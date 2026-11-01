#pragma once
// Brief, nonblocking LCD help. Order matches Mode; no audio-side work.
const char menuHelp[][4][25] = {
  {"EDIT", "ENC1 turn: cell", "ENC1 tap: hit/accent/off", "ENC2 tap: mute row"},
  {"MUTE", "ENC1 turn: track", "ENC1 tap: mute/unmute", "POT: master volume"},
  {"MIX", "ENC1 turn: track", "POT: track level", "ENC2 tap: next mode"},
  {"NUDGE", "ENC1 turn: track", "ENC2 turn: timing", "Range: -8 to +8 ticks"},
  {"DUCK", "POT: duck depth", "Kick ducks other tracks", "ENC2 tap: next mode"},
  {"PRESET", "ENC1 browse; tap: load", "Hold ENC1 + turn ENC2", "to morph to next preset"},
  {"SOUND", "ENC1 turn: sound", "ENC1 tap: next track", "ENC2 turn: tempo"},
  {"KIT", "ENC1 turn: browse", "ENC1 tap: load kit", "Pattern stays unchanged"},
  {"NOTE", "ENC1 turn: pitch", "ENC1 tap: next step", "After B1 comes B2"},
  {"FX", "ENC1 effect; tap: track", "POT: master reverb", "ENC1 hold 0.7s: reverse"},
  {"GLITCH", "ENC1 turn: effect", "POT: wet/dry depth", "ENC2 tap: next mode"},
  {"LED", "Stored animation choice", "No matrix in this build", "ENC2 tap: next mode"},
  {"BRIGHT", "Stored LED brightness", "No matrix in this build", "ENC2 tap: next mode"},
  {"BANK", "ENC1 browse; tap: recall", "P4: copy to chosen bank", "Current bank auto-stored"},
  {"BANK SONG", "ENC1 tap: play/stop", "Chains non-empty banks", "ENC2 hold 1.5s: EDIT"},
  {"CHANCE", "ENC1 turn: probability", "ENC1 tap: next cell", "ENC2 tap: next mode"},
  {"TEMPO", "ENC1/ENC2 turn: BPM", "ENC1 tap: tap tempo", "Range: 40 to 240 BPM"},
  {"SWING", "ENC1 turn: shuffle", "0 straight / 9 maximum", "ENC2 tap: next mode"},
  {"GLIDE", "ENC1 turn: slide time", "0 off / 9 longest", "ENC2 tap: next mode"},
  {"TRANSPOSE", "ENC1 turn: key shift", "Both synths, +/-24", "ENC2 tap: next mode"},
  {"COMP", "POT: threshold", "Lower = more compression", "ENC2 tap: next mode"},
  {"CHOKE", "ENC1 tap: on/off", "Closed hat cuts open hat", "ENC2 tap: next mode"},
  {"HUMAN", "ENC1 turn: track", "ENC2 turn: variation", "0 exact / 9 loosest"},
  {"METRO", "ENC1 tap: click on/off", "Click on steps 1 and 5", "ENC2 tap: next mode"},
  {"DENSITY", "POT: pattern density", "0 silent / 100% full", "Pattern is not erased"},
  {"CHAIN", "ENC1 slot; tap: play", "ENC2 turn: preset", "Preset 0 ends chain"},
  {"SONG", "ENC1 slot; tap: play", "ENC2 turn: preset/count", "Hold ENC2: repeat edit"},
  {"VOICE", "ENC1 turn: track", "ENC1 tap: decay/pitch", "ENC2 turn: value"},
  {"REC", "ENC1 turn: target row", "P3/P4 taps: record", "ENC2 hold 1s: clear row"}
};
static_assert(sizeof(menuHelp)/sizeof(menuHelp[0]) == MODE_COUNT, "Help for every mode");
void drawMenuHelp() {
  if (!menuHintVisible || (uint32_t)(millis()-menuHintMs) >= 2200) return;
  canvas.setFont(NULL); canvas.setTextSize(1); canvas.setTextWrap(false);
  canvas.fillRect(2,24,156,47,C(14,8,20));
  canvas.drawRect(2,24,156,47,C(180,100,255));
  canvas.setTextColor(C(180,100,255)); canvas.setCursor(6,27);
  canvas.print(menuHelp[mode][0]);
  canvas.setCursor(132,27); canvas.print(secondaryTier ? "2/2" : "1/2");
  canvas.setTextColor(C(240,220,255));
  for (int i=1;i<4;++i) {
    canvas.setCursor(6,28+i*10); canvas.print(menuHelp[mode][i]);
  }
}
