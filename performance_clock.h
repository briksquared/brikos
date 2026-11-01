#pragma once
// Bounded audio-owned event queue for signed offsets, including repeats crossing
// a step boundary. Negative offsets are planned during the preceding step.
struct NudgeEvent { uint32_t due; uint8_t track, step; bool used; };
NudgeEvent nudgeEvents[64] = {};
uint32_t audioSampleClock = 0;
int nudgeEventCount = 0;
uint32_t earliestNudge = 0;
long nextStepDuration = 0;
void triggerStep();
bool rollChance(int tr, int st);
void enqueueNudgedTrack(int tr,int st,long start,long duration,bool initial) {
  if (muted[tr] || !pattern[tr][st] || !rollChance(tr,st)) return;
  int hits=constrain((int)ratchet[tr][st],0,3)+1;
  long offset=trackNudge[tr]*(samplesPerStep/24);
  for(int hit=0;hit<hits;++hit) {
    long relative=start+offset+duration*hit/hits;
    if(relative<0 && !initial) continue;
    if(relative<0) relative=0;
    for(int i=0;i<64;++i) if(!nudgeEvents[i].used) {
      nudgeEvents[i].due=audioSampleClock+(uint32_t)relative;
      nudgeEvents[i].track=tr; nudgeEvents[i].step=st; nudgeEvents[i].used=true;
      if (!nudgeEventCount || (int32_t)(nudgeEvents[i].due-earliestNudge)<0) earliestNudge=nudgeEvents[i].due;
      ++nudgeEventCount;
      break;
    }
  }
}
void planNudges(bool initial) {
  int next=(currentStep+1)%STEPS;
  for(int tr=0;tr<TRACKS;++tr) {
    if(fillActive && fillStepsLeft>0 && tr==fillTrack) {
      // Look ahead from the last fill step so an early first post-fill hit is not lost.
      if(fillStepsLeft==1 && trackNudge[tr]<0) enqueueNudgedTrack(tr,next,curStepDur,nextStepDuration,false);
      continue;
    }
    if(trackNudge[tr]>0 || (initial && trackNudge[tr]<0))
      enqueueNudgedTrack(tr,currentStep,0,curStepDur,initial);
    if(trackNudge[tr]<0)
      enqueueNudgedTrack(tr,next,curStepDur,nextStepDuration,false);
  }
}
void serviceNudges() {
  if (!nudgeEventCount || (int32_t)(audioSampleClock-earliestNudge)<0) return;
  for(int i=0;i<64;++i) if(nudgeEvents[i].used && (int32_t)(audioSampleClock-nudgeEvents[i].due)>=0) {
    int tr=nudgeEvents[i].track,st=nudgeEvents[i].step;
    nudgeEvents[i].used=false; --nudgeEventCount;
    if(fillActive && fillStepsLeft>0 && tr==fillTrack && !(fillStepsLeft==1 && st==0)) continue;
    repeatMask=1<<tr; repeatingStep=true; sequenceStepOverride=st;
    triggerStep();
    sequenceStepOverride=-1; repeatingStep=false;
  }
  bool first=true;
  for (int i=0;i<64;++i) if(nudgeEvents[i].used) {
    if(first || (int32_t)(nudgeEvents[i].due-earliestNudge)<0) earliestNudge=nudgeEvents[i].due;
    first=false;
  }
}
void advanceFillBar() {
  if(fillActive && fillStepsLeft>0) {
    --fillStepsLeft;
    if(!fillStepsLeft) fillActive=false;
  }
  if(fillRequested && currentStep==0) {
    fillRequested=false; fillActive=true; fillStepsLeft=STEPS;
    for(int i=0;i<64;++i)
      if(nudgeEvents[i].used && nudgeEvents[i].track==fillTrack) { nudgeEvents[i].used=false; --nudgeEventCount; }
  }
  if(fillActive && fillStepsLeft>0)
    fillPitch=powf(2.0f,(-2.0f*(STEPS-fillStepsLeft)/(STEPS-1))/12.0f);
}
