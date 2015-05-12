package com.orcchg.arkanoid.surface;

public enum Prize {
  NONE(0), BLOCK(1), CLIMB(2), DESTROY(3), DRAGON(4), EASY(5), EASY_T(6),
  EVAPORATE(7), EXPLODE(8), EXTEND(9), FAST(10), FOG(11), GOO(12), HYPER(13),
  INIT(14), JUMP(15), LASER(16), MIRROR(17), PIERCE(18), PROTECT(19), RANDOM(20),
  SHORT(21), SLOW(22), UPGRADE(23), DEGRADE(24), VITALITY(25), WIN(26), ZYGOTE(27),
  SCORE_1(28), SCORE_2(29), SCORE_3(30), SCORE_4(31), SCORE_5(32);
  
  private int value;
  private static Prize[] values = Prize.values();
  
  Prize(int value) {
    this.value = value;
  }
  
  static Prize fromInt(int value) {
    return values[value];
  }
  
  int getValue() { return value; }
}
