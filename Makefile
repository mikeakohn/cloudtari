
default:
	@+make -C build

nosdl:
	@+make -C build nosdl

testing:
	naken_asm -b -l -o test_1.bin test/test_1.asm

clean:
	@rm -f build/*.o cloudtari test_1.bin test_1.lst
	@echo "Clean!"

