#ifndef __ARKANOID_PARAMS__H__
#define __ARKANOID_PARAMS__H__

namespace game {

struct BiteParams {
  constexpr static float biteWidth = 0.5f;  //!< Normalized width of bite.
  constexpr static float biteHeight = 0.08f;  //!< Normalized height of bite.
  constexpr static float biteTouchArea = 0.15f;  //!< Radius of touch area to affect bite.
  /// @brief Elevation of bite above the lower boundary of the playground.
  constexpr static float biteElevation = 0.3f;
  constexpr static float neg_biteElevation = 1.0f - biteElevation;
  constexpr static float radius = biteWidth * 0.64f;  //!< Curvature radius of bite.
};

struct BallParams {
  constexpr static float ballSize = 0.05f;  //!< Size of any of 4 sides of ball.
  constexpr static float ballHalfSize = ballSize * 0.5f;
  constexpr static float ballSpeed = 0.002f;   //!< Initial speed at game start
};

struct PrizeParams {
  constexpr static float prizeSpeed = 0.002f;
  constexpr static float prizeWidth = 0.1f;
  constexpr static float prizeHeight = 0.1f;
  constexpr static float prizeHalfWidth = 0.5f * prizeWidth;
  constexpr static float prizeHalfHeight = 0.5f * prizeHeight;
};

struct ProcessorParams {
  constexpr static int milliDelay = 1;  //!< Delay between sequential frames.
};

}

#endif  // __ARKANOID_PARAMS__H__
