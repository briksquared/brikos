#pragma once
// Trigger-time work only: raising a decay coefficient to 1/scale is exactly
// decayC(originalSeconds * scale), without duplicating every voice recipe.
bool hatWasOpen = false;
float openTailBaseF = 1080.0f;
float clapPitchRatio = 1.0f, clapPitchLP = 0, clapPitchCoef = 0.5f;
float *voiceEnvPtr(int tr) {
  switch(tr) { case 0:return &kickEnv; case 1:return &snareEnv;
    case 2:return &hatEnv; case 3:return &clapEnv; case 4:return &tomEnv;
    default:return &synthEnv[tr-5]; }
}
float *voiceDecayPtr(int tr) {
  switch(tr) { case 0:return &kAmpD; case 1:return &snD;
    case 2:return &hatD; case 3:return &clapD; case 4:return &tAmpD;
    default:return &synD[tr-5]; }
}
void finishVoiceTrigger(int tr, int st) {
  float &env = *voiceEnvPtr(tr), &d = *voiceDecayPtr(tr);
  reverseRunning[tr] = false; reverseLeft[tr] = 0;
  bool filling = st >= 0 && sequenceStepOverride < 0 && fillActive && fillStepsLeft > 0 && tr == fillTrack;
  if(tr==2) hatWasOpen=(soundVariant[2]==1);
  float pitch = trackPitchOffset[tr] ? powf(2.0f, trackPitchOffset[tr]/12.0f) : 1.0f;
  if (filling) {
    pitch *= fillPitch; env *= 0.9f; fillFlashMs = millis();
    if(tr==0) { kickClickEnv*=0.9f; kickSnapEnv*=0.9f; kickBodyEnv*=0.9f; }
    if(tr==1) { snClickEnv*=0.9f; snWireEnv*=0.9f; }
    if(tr==2) { hatClickEnv*=0.9f; rideBellEnv*=0.9f; }
    if(tr==4) { tomClickEnv*=0.9f; tomRingEnv*=0.9f; }
  }
  if(tr==3) {
    // Unpitched white-noise clap: transpose its spectral brightness instead.
    clapPitchRatio=pitch; clapPitchCoef=1.0f-expf(-TWO_PI_F*4000.0f*pitch/SR);
    clapPitchLP=0;
  }
  if (trackDecayScale[tr] != 1.0f) d = powf(d, 1.0f/trackDecayScale[tr]);
  if (pitch != 1.0f) {
    switch(tr) {
      case 0:kBaseF*=pitch; kSweepF*=pitch; kickSnapF*=pitch; break;
      case 1:snToneF*=pitch; break;
      case 2:hatBaseF*=pitch; break;
      case 4:tBaseF*=pitch; tSweepF*=pitch; break;
      case 5:case 6:synF[tr-5]*=pitch; if (!glideAmt) synSlideF[tr-5]=synF[tr-5]; break;
    }
  }
  if (st >= 0 && reverseAccent[tr] && pattern[tr][st] == 2 && d > 0 && d < 1) {
    reversePeak[tr] = env;
    // Five time constants: start at -43dB and grow to the original peak.
    reverseLeft[tr] = (uint32_t)(-5.0f/logf(d));
    env *= 0.006737947f;
    d = 1.0f/d;
    reverseRunning[tr] = true;
    // Suppress independent attack transients on reversed hits.
    switch(tr) {
      case 0:kickClickEnv=kickSnapEnv=kickBodyEnv=0; break;
      case 1:snClickEnv=snWireEnv=0; break;
      case 2:hatClickEnv=rideBellEnv=0; break;
      case 3:clapCnt2=clapCnt3=0; clapEnv2=clapEnv3=0; break;
      case 4:tomClickEnv=tomRingEnv=0; break;
    }
  }
}
inline void advanceVoiceEnvelope(int tr, float &env, float d) {
  if (!reverseRunning[tr]) { env *= d; return; }
  if (reverseLeft[tr]) {
    --reverseLeft[tr]; env *= d;
    if (env > reversePeak[tr]) env = reversePeak[tr];
  } else {
    // 2ms click-free final release, rather than a full-amplitude discontinuity.
    env -= reversePeak[tr] / (SR * 0.002f);
    if (env <= 1e-4f) { env = 0; reverseRunning[tr] = false; }
  }
}
