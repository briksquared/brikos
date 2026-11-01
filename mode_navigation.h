#pragma once
// UI-only navigation. Audio and arrangement clocks do not depend on this tier.
const Mode primaryModes[] = {MODE_EDIT,MODE_MUTE,MODE_MIX,MODE_PRESET,MODE_SOUND,MODE_KIT,MODE_NOTE,MODE_FX,MODE_GLITCH,MODE_LED,MODE_BANK,MODE_CHAIN,MODE_SONG,MODE_TEMPO,MODE_SWING,MODE_REC};
const Mode secondaryModes[] = {MODE_NUDGE,MODE_DUCK,MODE_VOICE,MODE_GLIDE,MODE_TRANSPOSE,MODE_COMP,MODE_CHOKE,MODE_HUMAN,MODE_METRO,MODE_DENSITY,MODE_BRIGHT,MODE_BANK_SONG,MODE_CHANCE};
bool secondaryTier = false;
uint32_t tierAdvancedMs = 0;
bool menuHintVisible = false;
uint32_t menuHintMs = 0;
void menuInteraction() {
  menuHintVisible = false;
  if (secondaryTier) tierAdvancedMs = millis();
}
uint32_t recStartStep = 0, recLastHitStep = 0;
bool recWasActive = false;
void advanceMode(uint32_t held) {
  uint32_t now = millis();
  if (held >= 3000) {
    secondaryTier = true;
    mode = secondaryModes[0];
  } else if (secondaryTier && now - tierAdvancedMs > 5000) {
    secondaryTier = false;
    mode = MODE_EDIT;
  } else {
    const Mode *list = secondaryTier ? secondaryModes : primaryModes;
    int count = secondaryTier ? sizeof(secondaryModes)/sizeof(Mode) : sizeof(primaryModes)/sizeof(Mode);
    int index = -1;
    for (int i=0; i<count; ++i) if (list[i] == mode) index=i;
    mode = list[(index+1)%count];
  }
  tierAdvancedMs = now;
  menuHintMs = now;
  menuHintVisible = true;
  Serial.printf("MODE %d tier=%s\n", (int)mode, secondaryTier ? "SECONDARY" : "PRIMARY");
}
