#ifndef __ARKANOID_PARAMS__H__
#define __ARKANOID_PARAMS__H__

#include "utils.h"

namespace game {

struct BiteParams {
  constexpr static float biteWidth = 0.5f;  //!< Normalized width of bite.
  constexpr static float biteHalfWidth = 0.25f;  //!< Normalized half-width of bite.
  constexpr static float biteHeight = 0.08f;  //!< Normalized height of bite.
  constexpr static float neg_biteHalfWidth = 0.75f;  //!< One minus bite half-width.
  constexpr static float biteTouchArea = 0.15f;  //!< Radius of touch area to affect bite.
  /// @brief Elevation of bite above the lower boundary of the playground.
  constexpr static float biteElevation = 0.2f;
  constexpr static float neg_biteElevation = 0.8f;  //!< One minus bite elevation.
};

struct BallParams {
  constexpr static float ballSize = 0.05f;  //!< Size of any of 4 sides of ball.
  constexpr static float ballHalfSize = 0.025f;  //!< Half-size of any of 4 sides of ball.
  constexpr static float ballAngle = util::PI4;  //!< Initial angle at game start
  constexpr static float ballSpeed = 0.05f;   //!< Initial speed at game start
};

}

#endif  // __ARKANOID_PARAMS__H__
