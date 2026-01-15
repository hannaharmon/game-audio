#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../src/sfx_player.h"

namespace py = pybind11;
using namespace audio;

void bind_sfx_player(py::module_& m) {
    // SFXPlayer is a singleton with private destructor - use nodelete
    py::class_<SFXPlayer, std::unique_ptr<SFXPlayer, py::nodelete>>(m, "SFXPlayer")
        // Singleton access
        .def_static("get_instance", &SFXPlayer::GetInstance,
                   py::return_value_policy::reference,
                   "Get the singleton instance of the SFXPlayer")
        
        // Initialization
        .def("initialize", &SFXPlayer::Initialize,
             py::arg("sfx_group"),
             "Initialize with SFX group (must be called before first use)")
        
        // Playback
        .def("play", &SFXPlayer::Play,
             py::arg("sfx_name"),
             "Play a sound effect")
        .def("play_with_volume", &SFXPlayer::PlayWithVolume,
             py::arg("sfx_name"),
             py::arg("volume"),
             "Play a sound effect with specific volume");
}
