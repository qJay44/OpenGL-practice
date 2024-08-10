#ifndef WINDOW_H
#define WINDOW_H

struct State {
  int winWidth, winHeight;
  const float nearPlane, farPlane;
  u8 aaSamples;
};

extern struct State _gState;

#endif

