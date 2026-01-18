#include <pybind11/pybind11.h>

#include "audio_session.h"

namespace py = pybind11;
using namespace audio;

void bind_audio_session(py::module_& m) {
    py::class_<AudioSession>(m, "AudioSession",
        "RAII helper that initializes audio on creation and shuts down on close/destruction.")
        .def(py::init<>())
        .def("close", &AudioSession::Close,
             "Shut down the audio system if this session owns initialization.")
        .def("__enter__", [](AudioSession& self) -> AudioSession& { return self; },
             py::return_value_policy::reference_internal)
        .def("__exit__", [](AudioSession& self, py::object, py::object, py::object) {
            self.Close();
            return false;
        });
}
