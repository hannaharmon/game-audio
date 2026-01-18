#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "random_sound_container.h"

namespace py = pybind11;
using namespace audio;

void bind_random_sound_container(py::module_& m) {
    // Bind the configuration struct
    py::class_<RandomSoundContainerConfig>(m, "RandomSoundContainerConfig",
        "Configuration for random sound containers.\n\n"
        "Controls randomization behavior including pitch variation and repeat avoidance.")
        .def(py::init<>())
        .def_readwrite("avoid_repeat", &RandomSoundContainerConfig::avoidRepeat,
                      "bool: Avoid playing the same sound twice in a row")
        .def_readwrite("pitch_min", &RandomSoundContainerConfig::pitchMin,
                      "float: Minimum pitch shift (1.0 = normal pitch)")
        .def_readwrite("pitch_max", &RandomSoundContainerConfig::pitchMax,
                      "float: Maximum pitch shift (1.0 = normal pitch)")
        .def_readwrite("group", &RandomSoundContainerConfig::group,
                      "GroupHandle: Audio group handle to assign sounds to")
        .def_readwrite("max_duration", &RandomSoundContainerConfig::maxDuration,
                      "float: Maximum duration in seconds (0 = no limit)");
    
    // Bind the container class
    py::class_<RandomSoundContainer>(m, "RandomSoundContainer",
        "Container for playing randomized sounds with pitch variation.\n\n"
        "Similar to Wwise random containers, this class manages a collection\n"
        "of sound variants and plays them randomly with optional pitch shifts\n"
        "and repeat avoidance.")
        .def(py::init<const std::string&, const RandomSoundContainerConfig&>(),
             py::arg("name"),
             py::arg("config") = RandomSoundContainerConfig(),
             "Construct a random sound container.\n\n"
             "Args:\n"
             "    name (str): Name identifier for this container\n"
             "    config (RandomSoundContainerConfig, optional): Configuration settings")
        
        // Sound management
        .def("add_sound", &RandomSoundContainer::AddSound,
             py::arg("filepath"),
             "Add a sound to the container.\n\n"
             "Args:\n"
             "    filepath (str): Path to the audio file\n\n"
             "Raises:\n"
             "    FileLoadException: If the file cannot be loaded")
        
        .def("load_from_folder", &RandomSoundContainer::LoadFromFolder,
             py::arg("folder_path"),
             "Load all .wav files from a folder.\n\n"
             "Args:\n"
             "    folder_path (str): Path to the folder containing sound files")
        
        // Playback
        .def("play", &RandomSoundContainer::Play,
             "Play a random sound from the container.\n\n"
             "Automatically applies pitch variation within the configured range\n"
             "and avoids repeats if enabled.")
        
        .def("play_with_volume", &RandomSoundContainer::PlayWithVolume,
             py::arg("volume"),
             "Play a random sound with specific volume.\n\n"
             "Args:\n"
             "    volume (float): Volume level (0.0 to 1.0)")
        
        .def("stop_all", &RandomSoundContainer::StopAll,
             "Stop all currently playing sounds from this container.")
        
        // Configuration
        .def("set_pitch_range", &RandomSoundContainer::SetPitchRange,
             py::arg("min_pitch"),
             py::arg("max_pitch"),
             "Set the pitch range for randomization.\n\n"
             "Args:\n"
             "    min_pitch (float): Minimum pitch multiplier (e.g., 0.95 for 5% lower)\n"
             "    max_pitch (float): Maximum pitch multiplier (e.g., 1.05 for 5% higher)")
        
        .def("set_avoid_repeat", &RandomSoundContainer::SetAvoidRepeat,
             py::arg("avoid"),
             "Enable or disable repeat avoidance.\n\n"
             "Args:\n"
             "    avoid (bool): True to avoid playing the same sound twice in a row")
        
        // Getters
        .def("get_name", &RandomSoundContainer::GetName,
             "Get the name of this container.\n\n"
             "Returns:\n"
             "    str: Container name");
}
