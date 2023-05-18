.PHONY: build configure

build:
	cmake --build build

configure:
	cmake -Bbuild -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -GNinja
