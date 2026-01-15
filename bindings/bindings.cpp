#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>

namespace py = pybind11;

// Forward declarations of binding functions
void bind_audio_manager(py::module_&);
void bind_sfx_player(py::module_&);
void bind_random_sound_container(py::module_&);

PYBIND11_MODULE(audio_py, m) {
    m.doc() = "Python bindings for the Game Audio Module";

    // Bind all submodules
    bind_audio_manager(m);
    bind_sfx_player(m);
    bind_random_sound_container(m);
}
