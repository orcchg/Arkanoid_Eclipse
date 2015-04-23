package com.orcchg.arkanoid.surface;

public class Levels {
  public static String[] get(int index) {
    switch (index) {
      case -2:
        return SAMPLE2;
      case -1:
        return SAMPLE;
      default:
      case 0:
        return FIRST;
      case 1:
        return SECOND;
      case 2:
        return THIRD;
      case 3:
        return FOURTH;
      case 4:
        return FIFTH;
    }
  }
  
  private static final String[] SAMPLE2 = new String[] {"          ",
                                                        "          ",
                                                        "          ",
                                                        "          ",
                                                        "          ",
                                                        "          ",
                                                        "          ",
                                                        "          ",
                                                        "  TTTTTT  ",
                                                        "  TTBSTT  ",
                                                        "  TTSBTT  ",
                                                        "  TTBSTT  ",
                                                        "  TTSBTT  ",
                                                        "  TT  TT  ",
                                                        "  TTTTTT  "};
  
  private static final String[] SAMPLE = new String[] {"          ",
                                                       "     S    "};
  
  private static final String[] FIRST = new String[] {"          ",
                                                      "   SSSS   ",
                                                      "   SSSS   ",
                                                      "   SSSS   ",
                                                      "   SSSS   "};
  
  private static final String[] SECOND = new String[] {"          ",
                                                      "          ",
                                                      "          ",
                                                      "          ",
                                                      "          ",
                                                      "          ",
                                                      "          ",
                                                      "          ",
                                                      "          ",
                                                      "          ",
                                                      "          ",
                                                      "  BSSSSS  ",
                                                      "  BSSBSS  ",
                                                      "  BSSSSS  ",
                                                      "          ",
                                                      "          "};
  
  private static final String[] THIRD = new String[] {"          ",
                                                      "          ",
                                                      "S        S",
                                                      "B        B",
                                                      "S        S",
                                                      "B        S",
                                                      "S        B",
                                                      "S        S",
                                                      "S        S",
                                                      "B        B",
                                                      "S        S",
                                                      "B        S",
                                                      "S        B",
                                                      "S        S",
                                                      "B        B",
                                                      "S        S",
                                                      "B        S",
                                                      "S        B"};
  
  private static final String[] FOURTH = new String[] {"S B SS B S",
                                                       " S BBBS S ",
                                                       "B S SS B S"};
  
  private static final String[] FIFTH = new String[] {"          ",
                                                      "          ",
                                                      "         B",
                                                      "         B",
                                                      "         B",
                                                      "         B",
                                                      "         B",
                                                      "         B",
                                                      "         B",
                                                      "         B",
                                                      "         B",
                                                      "         B",
                                                      "         B",
                                                      "         B",
                                                      "         B",
                                                      "         B",
                                                      "         B",
                                                      "         B"};
}
