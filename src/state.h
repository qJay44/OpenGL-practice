#ifndef WINDOW_H
#define WINDOW_H

struct State {
  const float nearPlane, farPlane;
  int winWidth, winHeight;
  float gamma;
  float time;
  u8 aaSamples;
};

extern struct State _gState;

#endif

