package com.orcchg.arkanoid.surface;

public class Levels {  
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
        break;
    }
    return levels[index];
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
                                                       " T        "};
  
  private static final String[] L0 = new String[] {"          ",
                                                   "     S    "};
  
  private static final String[] L1 = new String[]{"          ",
                                                  "   SSSS   ",
                                                  "   SSSS   ",
                                                  "   SSSS   ",
                                                  "   SSSS   "};
  
  private static final String[] L2 = new String[]{"          ",
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
  
  private static final String[] L3 = new String[]{"          ",
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
  
  private static final String[] L4 = new String[] {"S B SS B S",
                                                   " S BBBS S ",
                                                   "B S SS B S"};
  
  private static final String[] L5 = new String[]{"          ",
                                                  "          ",
                                                  "          ",
                                                  "          ",
                                                  "          ",
                                                  "         S",
                                                  "        SB",
                                                  "       SBB",
                                                  "      SBBB",
                                                  "     SBBBB",
                                                  "    SBBBBA",
                                                  "   SBBBBAI",
                                                  "    SBBBBA",
                                                  "     SBBBB",
                                                  "      SBBB",
                                                  "       SBB",
                                                  "        SB",
                                                  "         S"};
  
  private static final String[] L6 = new String[]{"          ",
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
  
  private static final String[] L7 = new String[] {"          ",
                                                   "          ",
                                                   "          ",
                                                   "          ",
                                                   "          ",
                                                   "AAAAAAAAAA",
                                                   "AASSSSSSAA",
                                                   "AAAAAASSAA",
                                                   "AASSSSSSAA",
                                                   "AASSAAAAAA",
                                                   "AASSSSSSAA",
                                                   "AAAAAAAAAA",
                                                   "BBBBBBBBBB"};
  
  private static final String[] L8 = new String[] {"          ",
                                                   "          ",
                                                   "          ",
                                                   "          ",
                                                   "          ",
                                                   "  TT   TT ",
                                                   "          ",
                                                   "JJ  ILI  J",
                                                   "JJ  LIL  J",
                                                   "WWWWWWWWWW",
                                                   "WWWWWWWWWW",
                                                   "WWWWWWWWWW",
                                                   "CC  SSS CC"};
  
  private static final String[] L9 = new String[] {"          ",
                                                   "          ",
                                                   "          ",
                                                   "          ",
                                                   "          ",
                                                   "TTTTTTTTTT",
                                                   "          ",
                                                   "  BBBBBBB ",
                                                   "  BBBBBBB ",
                                                   "  BBBLLBB ",
                                                   "  BBBBBBB ",
                                                   "  BBBBBBB ",
                                                   "  BBBBBBB ",
                                                   "          ",
                                                   "          ",
                                                   "FFFFFFFFFF",
                                                   "FFFFFFFFFF",
                                                   "FFFFFFFFFF",
                                                   "FFFFFFFFFF",
                                                   "SSSSSSSSSS"};
  
  private static final String[] L10 = new String[] {"          ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "  TTFFTT  ",
                                                    " FFFFFFFF ",
                                                    " TTFFFFTT ",
                                                    " GGTTFTGG ",
                                                    " RRJJJJRR ",
                                                    "  IIIIII  "};
  
  private static final String[] L11 = new String[] {"   IIII   ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "FFFFFFFFFF",
                                                    "FFFFFFFFFF",
                                                    "FFFFFFFFFF",
                                                    "CCCCCCCCCC",
                                                    "CCCCCCCCCC",
                                                    "WWWWWWWWWW",
                                                    "WWWWWWWWWW",
                                                    "WWWWWWWWWW"};
  
  private static final String[] L12 = new String[] {"     I    ",
                                                    "      I   ",
                                                    "    L     ",
                                                    "     L    ",
                                                    "      T   ",
                                                    " L  I     ",
                                                    "  L  I    ",
                                                    "   L  I   ",
                                                    "    L  T  ",
                                                    "B  T  T  S",
                                                    " B  T  T S"};
  
//  private static final String[] L13 = new String[] {"          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          "};
//  
//  private static final String[] L14 = new String[] {"          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          "};
//  
//  private static final String[] L15 = new String[] {"          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          "};
//  
//  private static final String[] L16 = new String[] {"          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          "};
//  
//  private static final String[] L17 = new String[] {"          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          "};
//  
//  private static final String[] L18 = new String[] {"          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          "};
//  
//  private static final String[] L19 = new String[] {"          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          "};
//  
//  private static final String[] L20 = new String[] {"          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          ",
//                                                    "          "};
  
  private static final String[][] levels = {L0, L1, L2, L3, L4, L5, L6, L7, L8, L9,
                                          L10, L11, L12/*, L13, L14, L15, L16, L17, L18,
                                          L19, L20*/};
  
  public static final int TOTAL_LEVELS = levels.length;
}
