#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../src/random_sound_container.h"

namespace py = pybind11;
using namespace audio;

void bind_random_sound_container(py::module_& m) {
    // Bind the configuration struct
    py::class_<RandomSoundContainerConfig>(m, "RandomSoundContainerConfig")
        .def(py::init<>())
        .def_readwrite("avoid_repeat", &RandomSoundContainerConfig::avoidRepeat,
                      "Avoid playing the same sound twice in a row")
        .def_readwrite("pitch_min", &RandomSoundContainerConfig::pitchMin,
                      "Minimum pitch shift (1.0 = normal pitch)")
        .def_readwrite("pitch_max", &RandomSoundContainerConfig::pitchMax,
                      "Maximum pitch shift (1.0 = normal pitch)")
        .def_readwrite("group", &RandomSoundContainerConfig::group,
                      "Audio group to assign sounds to")
        .def_readwrite("max_duration", &RandomSoundContainerConfig::maxDuration,
                      "Maximum duration in seconds (0 = no limit)");
    
    // Bind the container class
    py::class_<RandomSoundContainer>(m, "RandomSoundContainer")
        .def(py::init<const std::string&, const RandomSoundContainerConfig&>(),
             py::arg("name"),
             py::arg("config") = RandomSoundContainerConfig(),
             "Construct a random sound container")
        
        // Sound management
        .def("add_sound", &RandomSoundContainer::AddSound,
             py::arg("filepath"),
             "Add a sound to the container")
        .def("load_from_folder", &RandomSoundContainer::LoadFromFolder,
             py::arg("folder_path"),
             "Load all .wav files from a folder")
        
        // Playback
        .def("play", &RandomSoundContainer::Play,
             "Play a random sound from the container")
        .def("play_with_volume", &RandomSoundContainer::PlayWithVolume,
             py::arg("volume"),
             "Play a random sound with specific volume")
        .def("stop_all", &RandomSoundContainer::StopAll,
             "Stop all currently playing sounds from this container")
        
        // Configuration
        .def("set_pitch_range", &RandomSoundContainer::SetPitchRange,
             py::arg("min_pitch"),
             py::arg("max_pitch"),
             "Set the pitch range for randomization")
        .def("set_avoid_repeat", &RandomSoundContainer::SetAvoidRepeat,
             py::arg("avoid"),
             "Enable or disable repeat avoidance")
        
        // Getters
        .def("get_name", &RandomSoundContainer::GetName,
             "Get the name of this container");
}
