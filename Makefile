
default:
	@+make -C build

clean:
	@rm -f build/*.o moln_spelare
	@echo "Clean!"

