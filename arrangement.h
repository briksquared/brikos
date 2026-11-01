#pragma once

// Preset IDs in slots are 1-based; zero terminates an arrangement.
struct Arrangement {
  uint16_t chain[8] = {};
  SongSlot song[16] = {};
};
void loadPreset(int idx);
Arrangement arrangements[presetCount];
int arrangementOwner = 0;
int chainEdit = 0, songEdit = 0;
int chainPosition = -1, songPosition = -1, songRepeat = 0;
bool chainPlay = false, presetSongPlay = false, songRepeatEdit = false;
bool arrangementLoading = false;

void stopPresetArrangement() {
  chainPlay = false;
  presetSongPlay = false;
  chainPosition = songPosition = -1;
}

void loadArrangementPreset(int number) {
  if (number < 1 || number > presetCount) return;
  arrangementLoading = true;
  loadPreset(number - 1);
  presetSel = number - 1;
  arrangementLoading = false;
}

void toggleChain() {
  chainPlay = !chainPlay;
  chainPosition = -1;
  Serial.printf("CHAIN %s (starts next bar)\n", chainPlay ? "ON" : "OFF");
}

void togglePresetSong() {
  presetSongPlay = !presetSongPlay;
  songPosition = -1;
  songRepeat = 0;
  Serial.printf("SONG %s (starts next bar)\n", presetSongPlay ? "ON" : "OFF");
}

// Called only by loop(), never by the audio task. A loaded scene does not
// replace its controlling arrangement: the owner remains the starting preset.
bool advancePresetArrangement() {
  Arrangement &a = arrangements[arrangementOwner];
  if (presetSongPlay) {
    if (songPosition >= 0 && ++songRepeat < max(1, (int)a.song[songPosition].repeats))
      return true;
    int next = songPosition + 1;
    if (next >= 16 || a.song[next].preset == 0) {
      presetSongPlay = false;
      songPosition = -1;
      Serial.println("SONG END");
      return true;
    }
    songPosition = next;
    songRepeat = 0;
    loadArrangementPreset(a.song[next].preset);
    Serial.printf("SONG slot=%d preset=%d repeats=%d\n", next + 1,
                  a.song[next].preset, max(1, (int)a.song[next].repeats));
    return true;
  }
  if (chainPlay) {
    int next = chainPosition + 1;
    if (next >= 8 || a.chain[next] == 0) {
      chainPlay = false;
      chainPosition = -1;
      Serial.println("CHAIN END");
      return true;
    }
    chainPosition = next;
    loadArrangementPreset(a.chain[next]);
    Serial.printf("CHAIN slot=%d preset=%d\n", next + 1, a.chain[next]);
    return true;
  }
  return false;
}

// Legacy arrNNN records were exactly 40 bytes. Decode by record length,
// not the global version, so interrupted migrations remain recoverable.
static_assert(sizeof(SongSlot) == 4, "16-bit song slot layout");
static_assert(sizeof(Arrangement) == 80, "Arrangement storage layout");
bool readArrangement(const char *key, Arrangement &out) {
  size_t len = prefs.getBytesLength(key);
  if (len == sizeof(Arrangement))
    return prefs.getBytes(key, &out, sizeof(out)) == sizeof(out);
  if (len == 40) {
    uint8_t old[40];
    if (prefs.getBytes(key, old, sizeof(old)) != sizeof(old)) return false;
    for (int j=0;j<8;++j) out.chain[j]=old[j];
    for (int j=0;j<16;++j) {
      out.song[j].preset=old[8+j*2];
      out.song[j].repeats=old[9+j*2];
    }
    return true;
  }
  if (len) Serial.printf("NVS ERROR: unknown arrangement size %u for %s\n", (unsigned)len,key);
  return false;
}

void saveArrangements() {
  // Only write changed preset records. Keys stay below the NVS 15-character limit.
  for (int i = 0; i < presetCount; ++i) {
    Arrangement previous;
    for (int s = 0; s < 16; ++s) previous.song[s].repeats = 1;
    char key[12];
    snprintf(key, sizeof(key), "arr%03d", i);
    size_t storedLength = prefs.getBytesLength(key);
    bool exists = readArrangement(key, previous);
    if ((exists && storedLength == 40) || memcmp(&previous, &arrangements[i], sizeof(previous)) != 0) {
      if (prefs.putBytes(key, &arrangements[i], sizeof(arrangements[i])) != sizeof(arrangements[i]))
        Serial.printf("NVS ERROR: arrangement %d was not saved\n", i + 1);
    }
  }
  prefs.putInt("arrOwner", arrangementOwner);
  prefs.putUChar("presetVer", 3); // 3: 16-bit preset IDs; same arrNNN keys
}

void loadArrangements() {
  for (int i = 0; i < presetCount; ++i) {
    memcpy(arrangements[i].chain, presets[i].chain, sizeof(arrangements[i].chain));
    memcpy(arrangements[i].song, presets[i].song, sizeof(arrangements[i].song));
    char key[12];
    snprintf(key, sizeof(key), "arr%03d", i);
    readArrangement(key, arrangements[i]);
    for (int j = 0; j < 8; ++j)
      if (arrangements[i].chain[j] > presetCount) arrangements[i].chain[j] = 0;
    for (int j = 0; j < 16; ++j) {
      SongSlot &s = arrangements[i].song[j];
      if (s.preset > presetCount) s.preset = 0;
      s.repeats = constrain((int)s.repeats, 1, 16);
    }
  }
  arrangementOwner = constrain(prefs.getInt("arrOwner", lastPreset < 0 ? 0 : lastPreset), 0, presetCount - 1);
}
