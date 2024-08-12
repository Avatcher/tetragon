#ifndef TETRAGON_INITIALIZATIONS_HPP
#define TETRAGON_INITIALIZATIONS_HPP

#include <GLFW/glfw3.h>

#define INJECTED(x) #x

namespace tetragon {

void init_logs();
void init_glfw();
void init_glad();

} // tetragon

#endif // TETRAGON_INITIALIZATIONS_HPP