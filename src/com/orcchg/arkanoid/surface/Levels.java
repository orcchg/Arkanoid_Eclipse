package com.orcchg.arkanoid.surface;

public class Levels {
  public static final int TOTAL_LEVELS = 7;
  
  public static String[] get(int index) {
    switch (index) {
      case -5:
        return SAMPLE5;
      case -4:
        return SAMPLE4;
      case -3:
        return SAMPLE3;
      case -2:
        return SAMPLE2;
      case -1:
        return SAMPLE;
      default:
      case 0:
        return ZERO;
      case 1:
        return FIRST;
      case 2:
        return SECOND;
      case 3:
        return THIRD;
      case 4:
        return FOURTH;
      case 5:
        return FIFTH;
      case 6:
        return SIXTH;
      case 7:
        return SEVENTH;
    }
  }
  
  private static final String[] SAMPLE5 = new String[] {"SSSSSSSSSS",
                                                        "FFFFFFFFFF",
                                                        "GGGGGGGGGG",
                                                        "FFFFFFFFFF",
                                                        "FFFFFFFFFF",
                                                        "FFFFFFFFFF",
                                                        "FFFFFFFFFF",
                                                        "FFFFFFFFFF"};
  
  private static final String[] SAMPLE4 = new String[] {"SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS",
                                                        "SSSSSSSSSS"};
  
  private static final String[] SAMPLE3 = new String[] {"SSSSSSSSSS"};
  
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
                                                        "  TFFFFT  "};
  
  private static final String[] SAMPLE = new String[] {"          ",
                                                       "     S    "};
  
  private static final String[] ZERO = new String[] {"          ",
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
                                                      "         S",
                                                      "         S",
                                                      "         S",
                                                      "         S",
                                                      "         S",
                                                      "         S",
                                                      "         S",
                                                      "         S",
                                                      "         S",
                                                      "         S",
                                                      "         S",
                                                      "         S",
                                                      "         S",
                                                      "         S",
                                                      "         S",
                                                      "         S"};
  
  private static final String[] SIXTH = new String[] {"          ",
                                                      "          ",
                                                      "          ",
                                                      "          ",
                                                      "          ",
                                                      "          ",
                                                      "    T     ",
                                                      "   TSB    ",
                                                      "  TSSSB   ",
                                                      " TSSSSSB  ",
                                                      "  TSSSSB  ",
                                                      "   TSSB   ",
                                                      "    TB    ",
                                                      "     B    ",
                                                      "    BST   ",
                                                      "   BSSST  ",
                                                      "  BSSSSST ",
                                                      "   BSSST  ",
                                                      "    BBT   ",
                                                      "     B    "};
  
  private static final String[] SEVENTH = new String[] {"          ",
                                                        "          ",
                                                        "          ",
                                                        "          ",
                                                        "          ",
                                                        "    RC    ",
                                                        "   WWWW   ",
                                                        " SSSCCSSS ",
                                                        "  SWWJJW  ",
                                                        " SJJCCWSS ",
                                                        "  RRWRRC  "};
}
