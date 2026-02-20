#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>

#include "audio_manager.h"

namespace py = pybind11;

// Forward declarations of binding functions
void bind_audio_manager(py::module_&);
void bind_random_sound_container(py::module_&);
void bind_audio_session(py::module_&);

PYBIND11_MODULE(game_audio, m) {
    m.doc() = "Python bindings for the Game Audio Module";

    // Register custom exceptions with proper inheritance hierarchy
    // Register base exception first
    py::register_exception<audio::AudioException>(m, "AudioException", PyExc_RuntimeError);
    
    // Register derived exceptions - get the AudioException Python type to use as base
    py::object audio_exception = m.attr("AudioException");
    py::register_exception<audio::InvalidHandleException>(m, "InvalidHandleException", audio_exception.ptr());
    py::register_exception<audio::FileLoadException>(m, "FileLoadException", audio_exception.ptr());
    py::register_exception<audio::NotInitializedException>(m, "NotInitializedException", audio_exception.ptr());

    // Bind all submodules
    bind_audio_manager(m);
    bind_random_sound_container(m);
    bind_audio_session(m);
}
