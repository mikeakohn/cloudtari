.6502

.include "atari2600.inc"

.org 0xf000
start:
  sei
  cld
  lda #0xff
  txs

main:
  ;; Set player 0 sprite as 00000000.
  lda #0x00
  sta GRP0
  ;; Set player 0 color as index 0x1c.
  lda #0x1c
  sta COLUP0

while_1:
  ;; Vblank.
  lda #0x02
  sta VSYNC
  sta WSYNC
  sta WSYNC
  sta WSYNC
  lda #0x00
  sta VSYNC

  ldx #37
wait_blank:
  sta WSYNC
  dex
  bne wait_blank

  ;; Need 192 lines for screen.
  ldx #20
wait_top:
  sta WSYNC
  dex
  bne wait_top

  ;; NOP is 2 cycles (6 cycles of TIA). Need at least 68 cycles to
  ;; get past the hsync area then set RESP0 to set sprite position.
  ;; Set player 0 motion (HMP0) to -1 so it moves to the right.
  ;; Set player 0 sprite as 11000001 (GRP0).
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  lda #0xf0
  sta HMP0
  sta RESP0
  lda #0xc1
  sta GRP0
  sta WSYNC
  sta HMOVE
  sta WSYNC
  sta HMOVE
  sta WSYNC
  sta HMOVE
  sta WSYNC
  sta HMOVE
  nop
  nop
  nop
  nop
  sta HMCLR
  sta WSYNC
  sta HMOVE
  sta WSYNC

  ;; Set player 0 sprite as 00000000.
  lda #0x00
  sta GRP0

  ldx #165
wait_bottom:
  sta WSYNC
  dex
  bne wait_bottom

  ;; 30 lines of overscan.
  ldx #30
wait_overscane:
  sta WSYNC
  dex
  bne wait_overscane
  jmp while_1

.org 0xfffc
  .dw start
.org 0xfffe
  .dw start

