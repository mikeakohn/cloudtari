INCLUDE_DIR=../../naken_asm/include/6502/

default:
	naken_asm -I $(INCLUDE_DIR)  -l -type bin -o test_1.bin test_1.asm
	naken_asm -I $(INCLUDE_DIR)  -l -type bin -o hmove.bin hmove.asm

clean:
	@rm -f *.bin *.lst
	@echo "Clean!"

