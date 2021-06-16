/**
 *  Moln Spelare
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2021 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Disassembler.h"

int Disassembler::disassemble(uint8_t *code, int pc, char *text)
{
  Opcode &opcode = opcodes[code[0]];

  switch (opcode.type)
  {
    case OP_NONE:
      sprintf(text, "%s", opcode.name);
      return 1;
    case OP_IMMEDIATE:
      sprintf(text, "%s #0x%02x", opcode.name, code[1]);
      return 2;
    case OP_ADDRESS8:
      sprintf(text, "%s 0x%02x", opcode.name, code[1]);
      return 2;
    case OP_ADDRESS16:
      sprintf(text, "%s 0x%04x", opcode.name, code[1] | (code[2] << 8));
      return 3;
    case OP_INDEXED8_X:
      sprintf(text, "%s 0x%02x, x", opcode.name, code[1]);
      return 2;
    case OP_INDEXED8_Y:
      sprintf(text, "%s 0x%02x, y", opcode.name, code[1]);
      return 2;
    case OP_INDEXED16_X:
      sprintf(text, "%s 0x%04x, x", opcode.name, code[1] | (code[2] << 8));
      return 3;
    case OP_INDEXED16_Y:
      sprintf(text, "%s 0x%04x, y", opcode.name, code[1] | (code[2] << 8));
      return 3;
    case OP_INDIRECT16:
      sprintf(text, "%s (0x%04x)", opcode.name, code[1] | (code[2] << 8));
      return 3;
    case OP_X_INDIRECT8:
      sprintf(text, "%s (0x%02x, x)", opcode.name, code[1]);
      return 2;
    case OP_INDIRECT8_Y:
      sprintf(text, "%s (0x%02x), y", opcode.name, code[1]);
      return 2;
    case OP_INDIRECT8:
      sprintf(text, "%s (0x%02x)", opcode.name, code[1]);
      return 2;
    case OP_X_INDIRECT16:
      sprintf(text, "%s (0x%04x,x)", opcode.name, code[1] | (code[2] << 8));
      return 3;
    case OP_RELATIVE:
      sprintf(text, "%s 0x%04x", opcode.name, pc + 2 + (int8_t)code[1]);
      return 2;
    case OP_ADDRESS8_RELATIVE:
      sprintf(text, "%s 0x%02x 0x%04x", opcode.name, code[1], pc + 2 + (int8_t)code[2]);
      return 3;
    default:
      sprintf(text, "error");
      return 0;
  }
}

Disassembler::Opcode Disassembler::opcodes[] =
{
  { "brk",  OP_NONE              }, // 0x00
  { "ora",  OP_X_INDIRECT8       }, // 0x01
  { "???",  OP_NONE              }, // 0x02
  { "???",  OP_NONE              }, // 0x03
  { "tsb",  OP_ADDRESS8          }, // 0x04
  { "ora",  OP_ADDRESS8          }, // 0x05
  { "asl",  OP_ADDRESS8          }, // 0x06
  { "rmb0", OP_ADDRESS8          }, // 0x07
  { "php",  OP_NONE              }, // 0x08
  { "ora",  OP_IMMEDIATE         }, // 0x09
  { "asl",  OP_NONE              }, // 0x0a
  { "???",  OP_NONE              }, // 0x0b
  { "tsb",  OP_ADDRESS16         }, // 0x0c
  { "ora",  OP_ADDRESS16         }, // 0x0d
  { "asl",  OP_ADDRESS16         }, // 0x0e
  { "bbr0", OP_ADDRESS8_RELATIVE }, // 0x0f
  { "bpl",  OP_RELATIVE          }, // 0x10
  { "ora",  OP_INDIRECT8_Y       }, // 0x11
  { "ora",  OP_INDIRECT8         }, // 0x12
  { "???",  OP_NONE              }, // 0x13
  { "trb",  OP_ADDRESS8          }, // 0x14
  { "ora",  OP_INDEXED8_X        }, // 0x15
  { "asl",  OP_INDEXED8_X        }, // 0x16
  { "rmb1", OP_ADDRESS8          }, // 0x17
  { "clc",  OP_NONE              }, // 0x18
  { "ora",  OP_INDEXED16_Y       }, // 0x19
  { "inc",  OP_NONE              }, // 0x1a
  { "???",  OP_NONE              }, // 0x1b
  { "trb",  OP_ADDRESS16         }, // 0x1c
  { "ora",  OP_INDEXED16_X       }, // 0x1d
  { "asl",  OP_INDEXED16_X       }, // 0x1e
  { "bbr1", OP_ADDRESS8_RELATIVE }, // 0x1f
  { "jsr",  OP_ADDRESS16         }, // 0x20
  { "and",  OP_X_INDIRECT8       }, // 0x21
  { "???",  OP_NONE              }, // 0x22
  { "???",  OP_NONE              }, // 0x23
  { "bit",  OP_ADDRESS8          }, // 0x24
  { "and",  OP_ADDRESS8          }, // 0x25
  { "rol",  OP_ADDRESS8          }, // 0x26
  { "rmb2", OP_ADDRESS8          }, // 0x27
  { "plp",  OP_NONE              }, // 0x28
  { "and",  OP_IMMEDIATE         }, // 0x29
  { "rol",  OP_NONE              }, // 0x2a
  { "???",  OP_NONE              }, // 0x2b
  { "bit",  OP_ADDRESS16         }, // 0x2c
  { "and",  OP_ADDRESS16         }, // 0x2d
  { "rol",  OP_ADDRESS16         }, // 0x2e
  { "bbr2", OP_ADDRESS8_RELATIVE }, // 0x2f
  { "bmi",  OP_RELATIVE          }, // 0x30
  { "and",  OP_INDIRECT8_Y       }, // 0x31
  { "and",  OP_INDIRECT8         }, // 0x32
  { "???",  OP_NONE              }, // 0x33
  { "bit",  OP_INDEXED8_X        }, // 0x34
  { "and",  OP_INDEXED8_X        }, // 0x35
  { "rol",  OP_INDEXED8_X        }, // 0x36
  { "rmb3", OP_ADDRESS8          }, // 0x37
  { "sec",  OP_NONE              }, // 0x38
  { "and",  OP_INDEXED16_Y       }, // 0x39
  { "dec",  OP_NONE              }, // 0x3a
  { "???",  OP_NONE              }, // 0x3b
  { "bit",  OP_INDEXED16_X       }, // 0x3c
  { "and",  OP_INDEXED16_X       }, // 0x3d
  { "rol",  OP_INDEXED16_X       }, // 0x3e
  { "bbr3", OP_ADDRESS8_RELATIVE }, // 0x3f
  { "rti",  OP_NONE              }, // 0x40
  { "eor",  OP_X_INDIRECT8       }, // 0x41
  { "???",  OP_NONE              }, // 0x42
  { "???",  OP_NONE              }, // 0x43
  { "???",  OP_NONE              }, // 0x44
  { "eor",  OP_ADDRESS8          }, // 0x45
  { "lsr",  OP_ADDRESS8          }, // 0x46
  { "rmb4", OP_ADDRESS8          }, // 0x47
  { "pha",  OP_NONE              }, // 0x48
  { "eor",  OP_IMMEDIATE         }, // 0x49
  { "lsr",  OP_NONE              }, // 0x4a
  { "???",  OP_NONE              }, // 0x4b
  { "jmp",  OP_ADDRESS16         }, // 0x4c
  { "eor",  OP_ADDRESS16         }, // 0x4d
  { "lsr",  OP_ADDRESS16         }, // 0x4e
  { "bbr4", OP_ADDRESS8_RELATIVE }, // 0x4f
  { "bvc",  OP_RELATIVE          }, // 0x50
  { "eor",  OP_INDIRECT8_Y       }, // 0x51
  { "eor",  OP_INDIRECT8         }, // 0x52
  { "???",  OP_NONE              }, // 0x53
  { "???",  OP_NONE              }, // 0x54
  { "eor",  OP_INDEXED8_X        }, // 0x55
  { "lsr",  OP_INDEXED8_X        }, // 0x56
  { "rmb5", OP_ADDRESS8          }, // 0x57
  { "cli",  OP_NONE              }, // 0x58
  { "eor",  OP_INDEXED16_Y       }, // 0x59
  { "phy",  OP_NONE              }, // 0x5a
  { "???",  OP_NONE              }, // 0x5b
  { "???",  OP_NONE              }, // 0x5c
  { "eor",  OP_INDEXED16_X       }, // 0x5d
  { "lsr",  OP_INDEXED16_X       }, // 0x5e
  { "bbr5", OP_ADDRESS8_RELATIVE }, // 0x5f
  { "rts",  OP_NONE              }, // 0x60
  { "adc",  OP_X_INDIRECT8       }, // 0x61
  { "???",  OP_NONE              }, // 0x62
  { "???",  OP_NONE              }, // 0x63
  { "stz",  OP_ADDRESS8          }, // 0x64
  { "adc",  OP_ADDRESS8          }, // 0x65
  { "ror",  OP_ADDRESS8          }, // 0x66
  { "rmb6", OP_ADDRESS8          }, // 0x67
  { "pla",  OP_NONE              }, // 0x68
  { "adc",  OP_IMMEDIATE         }, // 0x69
  { "ror",  OP_NONE              }, // 0x6a
  { "???",  OP_NONE              }, // 0x6b
  { "jmp",  OP_INDIRECT16        }, // 0x6c
  { "adc",  OP_ADDRESS16         }, // 0x6d
  { "ror",  OP_ADDRESS16         }, // 0x6e
  { "bbr6", OP_ADDRESS8_RELATIVE }, // 0x6f
  { "bvs",  OP_RELATIVE          }, // 0x70
  { "adc",  OP_INDIRECT8_Y       }, // 0x71
  { "adc",  OP_INDIRECT8         }, // 0x72
  { "???",  OP_NONE              }, // 0x73
  { "stz",  OP_INDEXED8_X        }, // 0x74
  { "adc",  OP_INDEXED8_X        }, // 0x75
  { "ror",  OP_INDEXED8_X        }, // 0x76
  { "rmb7", OP_ADDRESS8          }, // 0x77
  { "sei",  OP_NONE              }, // 0x78
  { "adc",  OP_INDEXED16_Y       }, // 0x79
  { "ply",  OP_NONE              }, // 0x7a
  { "???",  OP_NONE              }, // 0x7b
  { "jmp",  OP_X_INDIRECT16      }, // 0x7c
  { "adc",  OP_INDEXED16_X       }, // 0x7d
  { "ror",  OP_INDEXED16_X       }, // 0x7e
  { "bbr7", OP_ADDRESS8_RELATIVE }, // 0x7f
  { "bra",  OP_RELATIVE          }, // 0x80
  { "sta",  OP_X_INDIRECT8       }, // 0x81
  { "???",  OP_NONE              }, // 0x82
  { "???",  OP_NONE              }, // 0x83
  { "sty",  OP_ADDRESS8          }, // 0x84
  { "sta",  OP_ADDRESS8          }, // 0x85
  { "stx",  OP_ADDRESS8          }, // 0x86
  { "smb0", OP_ADDRESS8          }, // 0x87
  { "dey",  OP_NONE              }, // 0x88
  { "bit",  OP_IMMEDIATE         }, // 0x89
  { "txa",  OP_NONE              }, // 0x8a
  { "???",  OP_NONE              }, // 0x8b
  { "sty",  OP_ADDRESS16         }, // 0x8c
  { "sta",  OP_ADDRESS16         }, // 0x8d
  { "stx",  OP_ADDRESS16         }, // 0x8e
  { "bbs0", OP_ADDRESS8_RELATIVE }, // 0x8f
  { "bcc",  OP_RELATIVE          }, // 0x90
  { "sta",  OP_INDIRECT8_Y       }, // 0x91
  { "sta",  OP_INDIRECT8         }, // 0x92
  { "???",  OP_NONE              }, // 0x93
  { "sty",  OP_INDEXED8_X        }, // 0x94
  { "sta",  OP_INDEXED8_X        }, // 0x95
  { "stx",  OP_INDEXED8_Y        }, // 0x96
  { "smb1", OP_ADDRESS8          }, // 0x97
  { "tya",  OP_NONE              }, // 0x98
  { "sta",  OP_INDEXED16_Y       }, // 0x99
  { "txs",  OP_NONE              }, // 0x9a
  { "???",  OP_NONE              }, // 0x9b
  { "stz",  OP_ADDRESS16         }, // 0x9c
  { "sta",  OP_INDEXED16_X       }, // 0x9d
  { "stz",  OP_INDEXED16_X       }, // 0x9e
  { "bbs1", OP_ADDRESS8_RELATIVE }, // 0x9f
  { "ldy",  OP_IMMEDIATE         }, // 0xa0
  { "lda",  OP_X_INDIRECT8       }, // 0xa1
  { "ldx",  OP_IMMEDIATE         }, // 0xa2
  { "???",  OP_NONE              }, // 0xa3
  { "ldy",  OP_ADDRESS8          }, // 0xa4
  { "lda",  OP_ADDRESS8          }, // 0xa5
  { "ldx",  OP_ADDRESS8          }, // 0xa6
  { "smb2", OP_ADDRESS8          }, // 0xa7
  { "tay",  OP_NONE              }, // 0xa8
  { "lda",  OP_IMMEDIATE         }, // 0xa9
  { "tax",  OP_NONE              }, // 0xaa
  { "???",  OP_NONE              }, // 0xab
  { "ldy",  OP_ADDRESS16         }, // 0xac
  { "lda",  OP_ADDRESS16         }, // 0xad
  { "ldx",  OP_ADDRESS16         }, // 0xae
  { "bbs2", OP_ADDRESS8_RELATIVE }, // 0xaf
  { "bcs",  OP_RELATIVE          }, // 0xb0
  { "lda",  OP_INDIRECT8_Y       }, // 0xb1
  { "lda",  OP_INDIRECT8         }, // 0xb2
  { "???",  OP_NONE              }, // 0xb3
  { "ldy",  OP_INDEXED8_X        }, // 0xb4
  { "lda",  OP_INDEXED8_X        }, // 0xb5
  { "ldx",  OP_INDEXED8_Y        }, // 0xb6
  { "smb3", OP_ADDRESS8          }, // 0xb7
  { "clv",  OP_NONE              }, // 0xb8
  { "lda",  OP_INDEXED16_Y       }, // 0xb9
  { "tsx",  OP_NONE              }, // 0xba
  { "???",  OP_NONE              }, // 0xbb
  { "ldy",  OP_INDEXED16_X       }, // 0xbc
  { "lda",  OP_INDEXED16_X       }, // 0xbd
  { "ldx",  OP_INDEXED16_Y       }, // 0xbe
  { "bbs3", OP_ADDRESS8_RELATIVE }, // 0xbf
  { "cpy",  OP_IMMEDIATE         }, // 0xc0
  { "cmp",  OP_X_INDIRECT8       }, // 0xc1
  { "???",  OP_NONE              }, // 0xc2
  { "???",  OP_NONE              }, // 0xc3
  { "cpy",  OP_ADDRESS8          }, // 0xc4
  { "cmp",  OP_ADDRESS8          }, // 0xc5
  { "dec",  OP_ADDRESS8          }, // 0xc6
  { "smb4", OP_ADDRESS8          }, // 0xc7
  { "iny",  OP_NONE              }, // 0xc8
  { "cmp",  OP_IMMEDIATE         }, // 0xc9
  { "dex",  OP_NONE              }, // 0xca
  { "wai",  OP_NONE              }, // 0xcb
  { "cpy",  OP_ADDRESS16         }, // 0xcc
  { "cmp",  OP_ADDRESS16         }, // 0xcd
  { "dec",  OP_ADDRESS16         }, // 0xce
  { "bbs4", OP_ADDRESS8_RELATIVE }, // 0xcf
  { "bne",  OP_RELATIVE          }, // 0xd0
  { "cmp",  OP_INDIRECT8_Y       }, // 0xd1
  { "cmp",  OP_INDIRECT8         }, // 0xd2
  { "???",  OP_NONE              }, // 0xd3
  { "???",  OP_NONE              }, // 0xd4
  { "cmp",  OP_INDEXED8_X        }, // 0xd5
  { "dec",  OP_INDEXED8_X        }, // 0xd6
  { "smb5", OP_ADDRESS8          }, // 0xd7
  { "cld",  OP_NONE              }, // 0xd8
  { "cmp",  OP_INDEXED16_Y       }, // 0xd9
  { "phx",  OP_NONE              }, // 0xda
  { "stp",  OP_NONE              }, // 0xdb
  { "???",  OP_NONE              }, // 0xdc
  { "cmp",  OP_INDEXED16_X       }, // 0xdd
  { "dec",  OP_INDEXED16_X       }, // 0xde
  { "bbs5", OP_ADDRESS8_RELATIVE }, // 0xdf
  { "cpx",  OP_IMMEDIATE         }, // 0xe0
  { "sbc",  OP_X_INDIRECT8       }, // 0xe1
  { "???",  OP_NONE              }, // 0xe2
  { "???",  OP_NONE              }, // 0xe3
  { "cpx",  OP_ADDRESS8          }, // 0xe4
  { "sbc",  OP_ADDRESS8          }, // 0xe5
  { "inc",  OP_ADDRESS8          }, // 0xe6
  { "smb6", OP_ADDRESS8          }, // 0xe7
  { "inx",  OP_NONE              }, // 0xe8
  { "sbc",  OP_IMMEDIATE         }, // 0xe9
  { "nop",  OP_NONE              }, // 0xea
  { "???",  OP_NONE              }, // 0xeb
  { "cpx",  OP_ADDRESS16         }, // 0xec
  { "sbc",  OP_ADDRESS16         }, // 0xed
  { "inc",  OP_ADDRESS16         }, // 0xee
  { "bbs6", OP_ADDRESS8_RELATIVE }, // 0xef
  { "beq",  OP_RELATIVE          }, // 0xf0
  { "sbc",  OP_INDIRECT8_Y       }, // 0xf1
  { "sbc",  OP_INDIRECT8         }, // 0xf2
  { "???",  OP_NONE              }, // 0xf3
  { "???",  OP_NONE              }, // 0xf4
  { "sbc",  OP_INDEXED8_X        }, // 0xf5
  { "inc",  OP_INDEXED8_X        }, // 0xf6
  { "smb7", OP_ADDRESS8          }, // 0xf7
  { "sed",  OP_NONE              }, // 0xf8
  { "sbc",  OP_INDEXED16_Y       }, // 0xf9
  { "plx",  OP_NONE              }, // 0xfa
  { "???",  OP_NONE              }, // 0xfb
  { "???",  OP_NONE              }, // 0xfc
  { "sbc",  OP_INDEXED16_X       }, // 0xfd
  { "inc",  OP_INDEXED16_X       }, // 0xfe
  { "bbs7", OP_ADDRESS8_RELATIVE }, // 0xff
};

