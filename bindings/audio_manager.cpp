#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>

#include "../src/audio_manager.h"

namespace py = pybind11;
using namespace audio;

void bind_audio_manager(py::module_& m) {
    // Bind AudioManager singleton (use nodelete since it has private destructor)
    py::class_<AudioManager, std::unique_ptr<AudioManager, py::nodelete>>(m, "AudioManager")
        // Singleton access
        .def_static("get_instance", &AudioManager::GetInstance, 
                   py::return_value_policy::reference,
                   "Get the singleton instance of the AudioManager")
        
        // System Lifecycle
        .def("initialize", &AudioManager::Initialize,
             "Initialize the audio system")
        .def("shutdown", &AudioManager::Shutdown,
             "Shut down the audio system")
        
        // System Control
        .def("set_master_volume", &AudioManager::SetMasterVolume,
             py::arg("volume"),
             "Set the master volume for all audio (0.0 to 1.0)")
        .def("get_master_volume", &AudioManager::GetMasterVolume,
             "Get the current master volume level")
        
        // Track Management
        .def("create_track", &AudioManager::CreateTrack,
             "Create a new audio track")
        .def("destroy_track", &AudioManager::DestroyTrack,
             py::arg("track"),
             "Destroy an audio track")
        .def("play_track", &AudioManager::PlayTrack,
             py::arg("track"),
             "Start playing an audio track")
        .def("stop_track", &AudioManager::StopTrack,
             py::arg("track"),
             "Stop playing an audio track")
        
        // Layer Operations
        .def("add_layer", &AudioManager::AddLayer,
             py::arg("track"),
             py::arg("layer_name"),
             py::arg("filepath"),
             py::arg("group") = "",
             "Add an audio layer to a track (group parameter is group name string)")
        .def("remove_layer", &AudioManager::RemoveLayer,
             py::arg("track"),
             py::arg("layer_name"),
             "Remove a layer from a track")
        .def("set_layer_volume", &AudioManager::SetLayerVolume,
             py::arg("track"),
             py::arg("layer_name"),
             py::arg("volume"),
             "Set the volume of a specific layer")
        .def("fade_layer", &AudioManager::FadeLayer,
             py::arg("track"),
             py::arg("layer_name"),
             py::arg("target_volume"),
             py::arg("duration"),
             "Fade a layer's volume to a target value over time")
        
        // Group Operations
        .def("create_group", &AudioManager::CreateGroup,
             py::arg("name") = "",
             "Create a new audio group")
        .def("destroy_group", &AudioManager::DestroyGroup,
             py::arg("group"),
             "Destroy an audio group")
        .def("set_group_volume", &AudioManager::SetGroupVolume,
             py::arg("group"),
             py::arg("volume"),
             "Set the volume for an entire audio group")
        .def("get_group_volume", &AudioManager::GetGroupVolume,
             py::arg("group"),
             "Get the current volume for an audio group")
        .def("fade_group", &AudioManager::FadeGroup,
             py::arg("group"),
             py::arg("target_volume"),
             py::arg("duration"),
             "Fade a group's volume to a target value over time")
        
        // Sound Operations
        .def("load_sound", 
             py::overload_cast<const std::string&>(&AudioManager::LoadSound),
             py::arg("filepath"),
             "Load a sound from a file")
        .def("load_sound",
             py::overload_cast<const std::string&, GroupHandle>(&AudioManager::LoadSound),
             py::arg("filepath"),
             py::arg("group"),
             "Load a sound from a file and assign it to a group")
        .def("destroy_sound", &AudioManager::DestroySound,
             py::arg("sound"),
             "Destroy a previously loaded sound")
        .def("start_sound", &AudioManager::StartSound,
             py::arg("sound"),
             "Start playing a sound")
        .def("stop_sound", &AudioManager::StopSound,
             py::arg("sound"),
             "Stop a currently playing sound")
        .def("set_sound_volume", &AudioManager::SetSoundVolume,
             py::arg("sound"),
             py::arg("volume"),
             "Set the volume of a sound")
        .def("set_sound_pitch", &AudioManager::SetSoundPitch,
             py::arg("sound"),
             py::arg("pitch"),
             "Set the pitch of a sound for its next playback")
        .def("is_sound_playing", &AudioManager::IsSoundPlaying,
             py::arg("sound"),
             "Check if a sound is currently playing")
        .def("play_random_sound_from_folder", &AudioManager::PlayRandomSoundFromFolder,
             py::arg("folder_path"),
             py::arg("group") = 0,
             "Play a random sound from a folder");
}
