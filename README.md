![Banner image](.github/images/screenshot.png)

# Tetragon

This is the repository where I learn OpenGL using C++. In no way 
should this be considered a serious project with any purpose beside learning.

## Building

1. This project uses GLAD to load OpenGL functions. Download it by [this link](https://glad.dav1d.de/#language=c&specification=gl&api=gl%3D3.3&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&profile=core&loader=on)
   and place files into the corresponding folders of this project.
   (Files from GLAD's `src/` go to this project's `src/`, etc.)

2. You can install the rest of the needed packages using [vcpkg](https://vcpkg.io/):
```sh
vcpkg install
```

3. Then run CMake and compile the project using Makefile:
```sh
mkdir build
cd build
cmake .. && make
```

4. A single executable should be generated:
```sh
./tetragon
```
