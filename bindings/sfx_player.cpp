#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../src/sfx_player.h"

namespace py = pybind11;
using namespace audio;

void bind_sfx_player(py::module_& m) {
    // SFXPlayer is a singleton with private destructor - use nodelete
    py::class_<SFXPlayer, std::unique_ptr<SFXPlayer, py::nodelete>>(m, "SFXPlayer",
        "Manages and plays all game sound effects.\n\n"
        "Central manager for all SFX in the game. Internally creates and manages\n"
        "all RandomSoundContainers needed for the game.")
        
        // Singleton access
        .def_static("get_instance", &SFXPlayer::GetInstance,
                   py::return_value_policy::reference,
                   "Get the singleton instance of the SFXPlayer.\n\n"
                   "Returns:\n"
                   "    SFXPlayer: The singleton instance")
        
        // Initialization
        .def("initialize", &SFXPlayer::Initialize,
             py::arg("sfx_group"),
             "Initialize with SFX group.\n\n"
             "Must be called before first use.\n\n"
             "Args:\n"
             "    sfx_group (int): Handle to the SFX audio group")
        
        // Playback
        .def("play", &SFXPlayer::Play,
             py::arg("sfx_name"),
             "Play a sound effect.\n\n"
             "Args:\n"
             "    sfx_name (str): Name of the SFX to play (e.g., 'footstep', 'explosion')")
        
        .def("play_with_volume", &SFXPlayer::PlayWithVolume,
             py::arg("sfx_name"),
             py::arg("volume"),
             "Play a sound effect with specific volume.\n\n"
             "Args:\n"
             "    sfx_name (str): Name of the SFX to play\n"
             "    volume (float): Volume level (0.0 to 1.0)");
}
