#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>

#include "audio_manager.h"
#include "path_utils.h"

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

    // Path utilities for working directory management
    m.def("set_working_directory", &audio::SetPythonWorkingDirectory,
          py::arg("path"),
          "Set the working directory for resolving relative file paths.\n\n"
          "This should typically be set to the directory containing your Python script.\n"
          "Relative paths passed to load_sound(), add_layer(), etc. will be resolved\n"
          "against this directory.\n\n"
          "Args:\n"
          "    path (str): Path to set as the working directory\n\n"
          "Example:\n"
          "    import os\n"
          "    import game_audio\n"
          "    script_dir = os.path.dirname(os.path.abspath(__file__))\n"
          "    game_audio.set_working_directory(script_dir)");
    
    m.def("get_working_directory", &audio::GetPythonWorkingDirectory,
          "Get the current working directory used for resolving relative paths.\n\n"
          "Returns:\n"
          "    str: The current working directory, or empty string if not set");

    // Bind all submodules
    bind_audio_manager(m);
    bind_random_sound_container(m);
    bind_audio_session(m);
}
