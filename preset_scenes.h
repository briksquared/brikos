#pragma once
#include <string.h>

// Shared byte-valued scene recipes. New presets supply their own final swing.
#define SC_HOUSE {0,0,1,0,0,0,0}, {60,80,100,80,60,120,120}, {140,120,80,100,110,95,95}, 140,0,0,3
#define SC_HIP {6,0,0,0,0,0,0}, {40,100,30,80,40,80,80}, {160,130,70,90,100,100,100}, 100,0,0,2
#define SC_DUB {0,2,2,2,0,2,2}, {30,200,200,180,120,200,200}, {120,100,70,80,90,110,110}, 180,0,0,6
#define SC_BREAK {0,0,0,0,0,0,0}, {50,120,80,60,60,80,80}, {140,130,90,100,90,100,100}, 120,0,0,2
#define SC_HARD {6,6,0,0,0,6,6}, {80,100,60,60,50,100,100}, {160,140,90,90,100,110,110}, 90,1,0,1
#define SC_FUNK {0,0,5,0,0,0,0}, {40,100,80,120,80,100,100}, {130,120,95,110,105,100,100}, 130,0,0,3
#define SC_WAVE {0,2,0,0,0,5,5}, {60,120,80,100,100,180,180}, {130,110,80,90,100,110,110}, 150,0,0,5
#define SC_CHIP {0,0,0,0,0,7,7}, {30,60,30,40,30,40,40}, {140,120,80,90,100,120,120}, 130,0,0,0

// Token-delimited lists avoid accidental substring matches (e.g. DUB/DUBSTEP).
inline bool sceneNameIn(const char *name, const char *list) {
  const size_t n = strlen(name);
  while (*list) {
    const char *end = strchr(list, '|');
    const size_t len = end ? (size_t)(end - list) : strlen(list);
    if (len == n && strncmp(name, list, n) == 0) return true;
    if (!end) break;
    list = end + 1;
  }
  return false;
}

inline Preset presetWithScene(Preset p, int oldSwing) {
  // Eight genre recipes plus an acoustic/neutral recipe for CLEAR and rock.
  static const Preset recipes[] = {
    {"HOUSE",0,{}, {}, SC_HOUSE,0},
    {"HIP",0,{}, {}, SC_HIP,5},
    {"DUB",0,{}, {}, SC_DUB,2},
    {"BREAK",0,{}, {}, SC_BREAK,3},
    {"HARD",0,{}, {}, SC_HARD,0},
    {"FUNK",0,{}, {}, SC_FUNK,3},
    {"WAVE",0,{}, {}, SC_WAVE,0},
    {"CHIP",0,{}, {}, SC_CHIP,0},
    {"ACOUSTIC",0,{}, {}, {0,0,0,0,0,0,0}, {40,100,80,120,80,100,100}, {130,120,95,110,105,100,100},150,0,0,3,0}
  };
  int genre = 8;
  // LATINHSE uses the more specific Latin/Funk group rather than House.
  if (sceneNameIn(p.name,"FOURFLOOR|TECHNO|DEEPHOUSE|TECHHOUSE|PROGHOUSE|DEEPTECH|BERLIN|DETROIT|CHICAGO|MINIMAL|IBIZA|ORGANHSE|PLUCKHSE|NUDISCO|FRENCHHS|SOULHOUSE|VOXHOUSE|MELODTECH|ACIDLINE|ACIDTECHNO|NASALTECH|DARKMIN|AMSTERDAM|SUNRISE|BALEARIC|DUOHOUSE|DUOTECHNO|DUOACID|DUODEEP|DUODARK")) genre=0;
  else if (sceneNameIn(p.name,"BOOMBAP|TRAP|HIPHOP|NODIGGITY|PHONK|DRILLUK|CLOUDRAP|PLUGGNB|GRIME|LOFIHOP|MOOGHOP|DUOHIP|BILLIEJEAN|INDACLUB|CRAZYLOVE|NEWYORK|GLITCHHOP|MIAMIBASS|ALTRNB")) genre=1;
  else if (sceneNameIn(p.name,"DUB|DUBSTEP|AMBIENT|CELLOAMB|BOWEDAMB|ORGANDUB|RUMBLDUB|CHIMEDUB|DUODUB|VAPORWAVE|SLOWBURN|COSMIC|DUOAMBNT|DEEPSUB|MOODY|BRISTOL|RIDDIM")) genre=2;
  else if (sceneNameIn(p.name,"DNB|DUODNB|JUNGLE|FOOTWORK|BREAKS|BREAKCORE|LIQUID|JERSEY|BALTIMORE")) genre=3;
  else if (sceneNameIn(p.name,"HARDSTYLE|HARDTECH|GABBER|PSYTRANCE|RAVE|INDUSTRL|HYPERPOP|BIGROOM|SQUELCHR|GROWLBASS|WOBBLER|SANDMAN|TEENSPIRIT|DUOEDM|GQOM")) genre=4;
  else if (sceneNameIn(p.name,"FUNK|MOOGFUNK|DUOFUNK|TWANGFNK|DISCO|GARAGE|LATIN|DUOLATIN|LATINHSE|AFROBEAT|DANCEHALL|REGGAETON|AMAPIANO|LAGOS|KUDURO|BAILEFUNK|STAYALIVE|SUPERSTIT|ANOTHRBITE|GETLUCKY|UPTOWNFNK|BASSLINE|TROPICAL|LONDONUK|ORGANIC|CUMBIA|BOSSANOVA|DUOJAZZ")) genre=5;
  else if (sceneNameIn(p.name,"SYNTHWAVE|DARKWAVE|NEONWAVE|SYNTHPOP|ELECTROPOP|INDIEDANCE|DUOWAVE|VOXWAVE|CHIMEPOP|BELLTECH|TRANCE|LEADTRNCE|DUOTRANCE|SUPERSAW|MELODRAMA|BLUEMONDAY|SWEETDREAM|TAKEONME|BABYMORE|HOTNCOLD|BLINDLGHT|BADGUY|ELECTRO|LEADPOP|TOKYO")) genre=6;
  else if (sceneNameIn(p.name,"CHIPTUNE|DUOCHIP")) genre=7;
  const Preset &s = recipes[genre];
  memcpy(p.trackFX,s.trackFX,sizeof(p.trackFX));
  memcpy(p.reverbSend,s.reverbSend,sizeof(p.reverbSend));
  memcpy(p.gTrack,s.gTrack,sizeof(p.gTrack));
  p.compThr=s.compThr; p.globalFX=s.globalFX; p.transposeAmt=s.transposeAmt;
  p.glideAmt=s.glideAmt; p.swing=s.swing;
  // Preserve prior groove where it lies inside the requested genre range.
  if (genre==0) p.swing=oldSwing<0 ? 0 : (oldSwing>2 ? 2 : oldSwing);
  if (genre==1) p.swing=oldSwing>=6 ? 6 : 5;
  if (genre==5) p.swing=oldSwing<3 ? 3 : (oldSwing>6 ? 6 : oldSwing);
  if (genre==6) p.swing=oldSwing>0 ? 1 : 0;
  if (strcmp(p.name,"GLITCHHOP")==0) p.globalFX=12;
  return p;
}
