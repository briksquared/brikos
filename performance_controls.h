#pragma once
// UI helpers do not run in the audio task.
void triggerPadHit(int tr);
void triggerJamSynth(int sy, int8_t semitone);
void recordLiveHit() {
  int tr = cursor/STEPS;
  int st = (currentStep + (sampleAccum >= curStepDur/2 ? 1 : 0)) % STEPS;
  pattern[tr][st] = 1;
  chance[tr][st] = 100;
  recLastHitStep = performanceStep;
  recordFlashMs = millis();
  if (!muted[tr]) {
    if (tr < 5) triggerPadHit(tr);
    else triggerJamSynth(tr-5, synthNote[tr-5][st]);
  }
  markDirty(); Serial.printf("REC track=%d step=%d\n",tr,st+1);
}
void updateRecordMode() {
  if (mode != MODE_REC) { recWasActive=false; return; }
  uint32_t step = performanceStep;
  if (!recWasActive) {
    recWasActive=true; recStartStep=recLastHitStep=step;
  } else if (step-recStartStep >= STEPS && step-recLastHitStep >= 2 && digitalRead(ENC2_BTN)!=HIGH) {
    mode=MODE_EDIT; secondaryTier=false; recWasActive=false;
    Serial.println("REC complete");
  }
}
void copyPasteSelectedTrack() {
  int tr=cursor/STEPS;
  if (clipSource < 0 || clipSource == tr) {
    memcpy(clipTrack.pat,pattern[tr],STEPS);
    memcpy(clipTrack.probability,chance[tr],STEPS);
    memcpy(clipTrack.repeats,ratchet[tr],STEPS);
    clipTrack.nudge=trackNudge[tr]; clipSource=tr;
    Serial.printf("COPY ROW %d\n",tr);
  } else {
    memcpy(pattern[tr],clipTrack.pat,STEPS);
    memcpy(chance[tr],clipTrack.probability,STEPS);
    memcpy(ratchet[tr],clipTrack.repeats,STEPS);
    trackNudge[tr]=clipTrack.nudge;
    pasteRow=tr; pasteMs=millis(); clipSource=-1;
    markDirty(); Serial.printf("PASTE ROW %d\n",tr);
  }
}
void changeMorph(int delta) {
  if (!morphActive) {
    morphTarget=(constrain(lastPreset,0,presetCount-1)+1)%presetCount;
    for(int tr=0;tr<TRACKS;++tr) {
      morphBaseGain[tr]=morphTransient ? morphGain[tr] : gTrack[tr];
      morphBaseSend[tr]=morphTransient ? morphSend[tr] : reverbSend[tr];
    }
    morphBaseVerb=morphTransient ? morphVerb : masterVerbAmt;
    morphAmt=0;
  }
  const Preset p=morphTarget<149 ? presetWithScene(presets[morphTarget],presetSwing[morphTarget]) : presets[morphTarget];
  morphAmt=constrain(morphAmt+delta/127.0f,0.0f,1.0f);
  for(int tr=0;tr<TRACKS;++tr) {
    morphGain[tr]=morphBaseGain[tr]+morphAmt*(p.gTrack[tr]/255.0f*2.0f-morphBaseGain[tr]);
    morphSend[tr]=morphBaseSend[tr]+morphAmt*(p.reverbSend[tr]/255.0f-morphBaseSend[tr]);
  }
  morphVerb=morphBaseVerb+morphAmt*(p.masterVerbAmt-morphBaseVerb);
  morphActive=true; encChordConsumed=true;
}
void finishMorph() {
  if (!morphActive) return;
  if (morphAmt>0.9f) {
    morphActive=false; morphTransient=false;
    loadPreset(morphTarget); presetSel=morphTarget;
  } else {
    if (morphAmt<0.1f) {
      memcpy(morphGain,morphBaseGain,sizeof(morphGain));
      memcpy(morphSend,morphBaseSend,sizeof(morphSend)); morphVerb=morphBaseVerb;
    }
    morphTransient=true; morphActive=false;
  }
  Serial.printf("MORPH %.0f%% (transient=%d)\n",morphAmt*100,morphTransient);
}
void savePerformanceState() {
  prefs.putBytes("trackNudge",trackNudge,sizeof(trackNudge));
  prefs.putBytes("trackDecay",trackDecayScale,sizeof(trackDecayScale));
  prefs.putBytes("trackPitch",trackPitchOffset,sizeof(trackPitchOffset));
  prefs.putBytes("reverseAcc",reverseAccent,sizeof(reverseAccent));
  prefs.putUChar("presetVer",3);
}
void loadPerformanceState() {
  memset(trackNudge,0,sizeof(trackNudge)); memset(trackPitchOffset,0,sizeof(trackPitchOffset));
  memset(reverseAccent,0,sizeof(reverseAccent));
  for(int tr=0;tr<TRACKS;++tr) trackDecayScale[tr]=1;
  if(prefs.getBytesLength("trackNudge")==sizeof(trackNudge)) prefs.getBytes("trackNudge",trackNudge,sizeof(trackNudge));
  if(prefs.getBytesLength("trackDecay")==sizeof(trackDecayScale)) prefs.getBytes("trackDecay",trackDecayScale,sizeof(trackDecayScale));
  if(prefs.getBytesLength("trackPitch")==sizeof(trackPitchOffset)) prefs.getBytes("trackPitch",trackPitchOffset,sizeof(trackPitchOffset));
  if(prefs.getBytesLength("reverseAcc")==sizeof(reverseAccent)) prefs.getBytes("reverseAcc",reverseAccent,sizeof(reverseAccent));
  for(int tr=0;tr<TRACKS;++tr) {
    trackNudge[tr]=constrain((int)trackNudge[tr],-8,8);
    trackPitchOffset[tr]=constrain((int)trackPitchOffset[tr],-24,24);
    if(!isfinite(trackDecayScale[tr]) || trackDecayScale[tr]<0.2f || trackDecayScale[tr]>3) trackDecayScale[tr]=1;
  }
}
