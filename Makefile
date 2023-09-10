.PHONY: build configure

build:
	cmake --build build

configure:
#	cmake -Bbuild -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -GNinja
	cmake -Bbuild -DCMAKE_TOOLCHAIN_FILE=c:\dev\vcpkg\scripts\buildsystems\vcpkg.cmake
