#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>

#include "../src/audio_manager.h"

namespace py = pybind11;

// Forward declarations of binding functions
void bind_audio_manager(py::module_&);
void bind_random_sound_container(py::module_&);
void bind_audio_session(py::module_&);

PYBIND11_MODULE(audio_py, m) {
    m.doc() = "Python bindings for the Game Audio Module";

    // Register custom exceptions
    py::register_exception<audio::AudioException>(m, "AudioException");
    py::register_exception<audio::InvalidHandleException>(m, "InvalidHandleException");
    py::register_exception<audio::FileLoadException>(m, "FileLoadException");
    py::register_exception<audio::NotInitializedException>(m, "NotInitializedException");

    // Bind all submodules
    bind_audio_manager(m);
    bind_random_sound_container(m);
    bind_audio_session(m);
}
