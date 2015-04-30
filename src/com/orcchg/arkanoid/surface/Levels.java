package com.orcchg.arkanoid.surface;

public class Levels {  
  public static String[] get(int index) {
    if (index < 0 || index >= TOTAL_LEVELS) {
      index = 0;
    }
    return levels[index];
  }
  
  private static final String[] L0 = new String[] {"TTTTTTTTTT",
                                                   "          ",
                                                   "          ",
                                                   "          ",
                                                   "          ",
                                                   "  G    G  ",
                                                   "   G  G   ",
                                                   "    GG    "};
  
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
                                                  "S   T    S",
                                                  "B   T    B",
                                                  "S   T    S",
                                                  "B   T    S",
                                                  "S   T    B",
                                                  "S   TG   S",
                                                  "S   TG  GS",
                                                  "B   TG GGB",
                                                  "S   TGGGGS",
                                                  "B   TGGGGS",
                                                  "S   TGGGGB",
                                                  "S   TGGGGS",
                                                  "B   TGG GB",
                                                  "S   TG  GS",
                                                  "B   TG   S",
                                                  "S   T    B"};
  
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
  
  private static final String[] L13 = new String[] {"    LL    ",
                                                    "    LL    ",
                                                    "    LL    ",
                                                    "    LL    ",
                                                    "VGGGLLGGGV",
                                                    "VGGGGGGGGV",
                                                    "VGGGGGGGGV",
                                                    "VGGTTTTGGV",
                                                    "VGGGGGGGGV",
                                                    "VGGGGGGGGV",
                                                    "VGGVVVVGGV",
                                                    "TPPIIIIPPT",
                                                    "T        T"};
  
  private static final String[] L14 = new String[] {"          ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "AA BB CC D",
                                                    "AA BB CC D",
                                                    "          ",
                                                    "EE FF GG H",
                                                    "EE FF GG H",
                                                    "          ",
                                                    "II JJ KK T",
                                                    "II JJ KK T",
                                                    "          ",
                                                    "LL MM NN V",
                                                    "LL MM NN V",
                                                    "          ",
                                                    "OO PP QQ R",
                                                    "OO PP QQ R",
                                                    "          ",
                                                    "SS UU WW E",
                                                    "SS UU WW E",
                                                    "          ",
                                                    "YY ZZ HH D",
                                                    "YY ZZ HH D"};
  
  private static final String[] L15 = new String[] {"          ",
                                                    "SS  SSSR S",
                                                    "S S S SRSS",
                                                    "SS  SSSR S",
                                                    "          ",
                                                    "    DD    ",
                                                    "  N TT N  ",
                                                    "  TTKKTT  ",
                                                    "  BBBBBB  ",
                                                    " BBBBBBBB ",
                                                    "BBRRRRRRBB",
                                                    "BRRRJJRRRB",
                                                    "BBBBJJBBBB",
                                                    "BZZJJJJZZB",
                                                    "BZZJJJJZZB",
                                                    "BBJRRRRJBB",
                                                    "BJJBRRBJJB",
                                                    "JJZBBBBZJJ",
                                                    "JJJJBBJJJJ",
                                                    "BBJJRRJJBB",
                                                    "BJJRRRRJJB",
                                                    " BJJBBJJB ",
                                                    " BBBBBBBB ",
                                                    "BBBBBBBBBB",
                                                    "FBBBYYBBBF",
                                                    "FFBBFFBBFF",
                                                    "FFFFFFFFFF",
                                                    "FFFFFFFFFF"};
  
  private static final String[] L16 = new String[] {"          ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "         G",
                                                    "        GG",
                                                    "       GGG",
                                                    "      GGGL",
                                                    "     GGGLL",
                                                    "    GGGLLL",
                                                    "  GGLLLLLL",
                                                    "GGGLLLLLLL",
                                                    "GGGGGLLLLL",
                                                    " GGGGGLLLL",
                                                    "   GGGGGLL",
                                                    "     GGGGL",
                                                    "       GGG",
                                                    "        GG",
                                                    "         G"};
  
  private static final String[] L17 = new String[] {"          ",
                                                    "          ",
                                                    "  SSSSSS  ",
                                                    "  SSSSSS  ",
                                                    "  SSSSSS  ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "    OB    ",
                                                    "  O    O  "};
  
  private static final String[] L18 = new String[] {"XX      XX",
                                                    "          ",
                                                    "  SSSSSS  ",
                                                    "  SSSSSS  ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "  XXXXXX  "};
  
  private static final String[] L19 = new String[] {" BB       ",
                                                    "          ",
                                                    "      ZZ  ",
                                                    "    ZZ  ZZ",
                                                    "  ZZ  ZZ  "};
  
  private static final String[] L20 = new String[] {"SSSSSSSSES",
                                                    "SSSESSSSSS",
                                                    "SSSSSSSSSS",
                                                    "ESSSSSSSSE",
                                                    "SSSSSSSSSS",
                                                    "SSSSSESSSS"};
  
  private static final String[] L21 = new String[] {"SSSSSISSSS",
                                                    "SSSSSBSSSS",
                                                    "SSSSSTSSSS",
                                                    "SSSSSSSSSS",
                                                    "SSSSSTSSSS",
                                                    "SSSSSKSSSS",
                                                    "          ",
                                                    "     K    "};
  
  private static final String[] L22 = new String[] {"SSSSSSSSSS",
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
  
  private static final String[] L23 = new String[] {"          ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "    $     "};
  
  private static final String[] L24 = new String[] {"TTTTTTTTTT",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "          ",
                                                    "  VVVVVVVV"};
  
  private static final String[][] levels = {L0, L1, L2, L3, L4, L5, L6, L7, L8, L9,
                                          L10, L11, L12, L13, L14, L15, L16, L17, L18,
                                          L19, L20, L21, L22, L23, L24};
  
  public static final int TOTAL_LEVELS = levels.length;
}
