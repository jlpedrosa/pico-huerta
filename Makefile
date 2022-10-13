
init-submodules:
	git submodule sync
init-build:
	rm -rf build; mkdir build; cd build; cmake .. -DPICO_BOARD=pico_w ..

build-all:
	cd build; make