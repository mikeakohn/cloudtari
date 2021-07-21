.6502

.org 0xf000
start:
  ldx #1
  ldy #2
  lda #5
  sta 0x80
  lda #6
  sta 0x80, x
  lda #7
  sta 0x80, y
  jsr blah
  clc
  adc #2
not_5:
  brk

blah:
  inx
  rts

.org 0xfffc
  .dw start
.org 0xfffe
  .dw start

