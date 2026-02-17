#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <functional>

#include "audio_manager.h"

namespace py = pybind11;
using namespace audio;

void bind_audio_manager(py::module_& m) {
    py::class_<TrackHandle>(m, "TrackHandle")
        .def(py::init<uint32_t>(), py::arg("value") = 0)
        .def_property_readonly("value", &TrackHandle::Value)
        .def("__int__", [](const TrackHandle& h) { return h.Value(); })
        .def("__bool__", [](const TrackHandle& h) { return h.IsValid(); })
        .def("is_valid", &TrackHandle::IsValid,
             "Check if the handle is valid.\n\n"
             "Returns:\n"
             "    bool: True if the handle is valid, False otherwise")
        .def_static("invalid", &TrackHandle::Invalid,
                   "Get an invalid handle.\n\n"
                   "Returns:\n"
                   "    TrackHandle: An invalid handle (value = 0)")
        .def("__repr__", [](const TrackHandle& h) { return "TrackHandle(" + std::to_string(h.Value()) + ")"; })
        .def("__hash__", [](const TrackHandle& h) { return std::hash<uint32_t>{}(h.Value()); })
        .def(py::self == py::self)
        .def(py::self != py::self);

    py::class_<GroupHandle>(m, "GroupHandle")
        .def(py::init<uint32_t>(), py::arg("value") = 0)
        .def_property_readonly("value", &GroupHandle::Value)
        .def("__int__", [](const GroupHandle& h) { return h.Value(); })
        .def("__bool__", [](const GroupHandle& h) { return h.IsValid(); })
        .def("is_valid", &GroupHandle::IsValid,
             "Check if the handle is valid.\n\n"
             "Returns:\n"
             "    bool: True if the handle is valid, False otherwise")
        .def_static("invalid", &GroupHandle::Invalid,
                   "Get an invalid handle.\n\n"
                   "Returns:\n"
                   "    GroupHandle: An invalid handle (value = 0)")
        .def("__repr__", [](const GroupHandle& h) { return "GroupHandle(" + std::to_string(h.Value()) + ")"; })
        .def("__hash__", [](const GroupHandle& h) { return std::hash<uint32_t>{}(h.Value()); })
        .def(py::self == py::self)
        .def(py::self != py::self);

    py::class_<SoundHandle>(m, "SoundHandle")
        .def(py::init<uint32_t>(), py::arg("value") = 0)
        .def_property_readonly("value", &SoundHandle::Value)
        .def("__int__", [](const SoundHandle& h) { return h.Value(); })
        .def("__bool__", [](const SoundHandle& h) { return h.IsValid(); })
        .def("is_valid", &SoundHandle::IsValid,
             "Check if the handle is valid.\n\n"
             "Returns:\n"
             "    bool: True if the handle is valid, False otherwise")
        .def_static("invalid", &SoundHandle::Invalid,
                   "Get an invalid handle.\n\n"
                   "Returns:\n"
                   "    SoundHandle: An invalid handle (value = 0)")
        .def("__repr__", [](const SoundHandle& h) { return "SoundHandle(" + std::to_string(h.Value()) + ")"; })
        .def("__hash__", [](const SoundHandle& h) { return std::hash<uint32_t>{}(h.Value()); })
        .def(py::self == py::self)
        .def(py::self != py::self);

    py::enum_<LogLevel>(m, "LogLevel")
        .value("Off", LogLevel::Off)
        .value("Error", LogLevel::Error)
        .value("Warn", LogLevel::Warn)
        .value("Info", LogLevel::Info)
        .value("Debug", LogLevel::Debug);

    // Bind AudioManager singleton (use nodelete since it has private destructor)
    py::class_<AudioManager, std::unique_ptr<AudioManager, py::nodelete>>(m, "AudioManager",
        "Central manager for all audio functionality.\n\n"
        "The AudioManager class provides a singleton interface for all audio operations.\n"
        "It manages audio tracks, groups, and individual sounds.\n\n"
        "All methods may raise exceptions:\n"
        "- InvalidHandleException: When using an invalid handle\n"
        "- FileLoadException: When a file cannot be loaded\n"
        "- AudioException: For general audio errors")
        
        // Singleton access
        .def_static("get_instance", &AudioManager::GetInstance, 
                   py::return_value_policy::reference,
                   "Get the singleton instance of the AudioManager.\n\n"
                   "Returns:\n"
                   "    AudioManager: The singleton instance")
        
        // System Lifecycle
        .def("initialize", &AudioManager::Initialize,
             "Initialize the audio system.\n\n"
             "Must be called before any other audio operations.\n\n"
             "Returns:\n"
             "    bool: True if initialization was successful, False if already initialized\n\n"
             "Raises:\n"
             "    AudioException: If audio engine initialization fails")
        
        .def("shutdown", &AudioManager::Shutdown,
             "Shut down the audio system.\n\n"
             "Cleans up all audio resources and stops all playback.\n"
             "Should be called before application exit.")
        
        .def("is_initialized", &AudioManager::IsInitialized,
             "Check if the audio system is initialized and running.\n\n"
             "Returns:\n"
             "    bool: True if the system is initialized, False otherwise")
        
        // System Control
        .def("set_master_volume", &AudioManager::SetMasterVolume,
             py::arg("volume"),
             "Set the master volume for all audio.\n\n"
             "Args:\n"
             "    volume (float): Volume level (0.0 = silence, 1.0 = full volume)")
        
        .def("get_master_volume", &AudioManager::GetMasterVolume,
             "Get the current master volume level.\n\n"
             "Returns:\n"
             "    float: Current master volume (0.0 to 1.0)")
        
        .def_static("set_log_level", &AudioManager::SetLogLevel,
             py::arg("level"),
             "Set the global audio log level.\n\n"
             "Args:\n"
             "    level (LogLevel): Desired logging level")
        .def_static("get_log_level", &AudioManager::GetLogLevel,
             "Get the current audio log level.\n\n"
             "Returns:\n"
             "    LogLevel: Current logging level")
        
        // Track Management
        .def("create_track", &AudioManager::CreateTrack,
             "Create a new audio track.\n\n"
             "Tracks can contain multiple synchronized audio layers.\n\n"
             "Returns:\n"
             "    TrackHandle: Handle to the newly created track\n\n"
             "Raises:\n"
             "    AudioException: If track creation fails")
        
        .def("destroy_track", &AudioManager::DestroyTrack,
             py::arg("track"),
             "Destroy an audio track.\n\n"
             "Args:\n"
             "    track (TrackHandle): Handle to the track to destroy")
        
        .def("play_track", &AudioManager::PlayTrack,
             py::arg("track"),
             "Start playing an audio track.\n\n"
             "All layers in the track will begin playing.\n\n"
             "Args:\n"
             "    track (TrackHandle): Handle to the track to play\n\n"
             "Raises:\n"
             "    InvalidHandleException: If track handle is invalid")
        
        .def("stop_track", &AudioManager::StopTrack,
             py::arg("track"),
             "Stop playing an audio track.\n\n"
             "All layers in the track will stop playing.\n\n"
             "Args:\n"
             "    track (TrackHandle): Handle to the track to stop\n\n"
             "Raises:\n"
             "    InvalidHandleException: If track handle is invalid")
        
        // Layer Operations
        .def("add_layer", &AudioManager::AddLayer,
             py::arg("track"),
             py::arg("layer_name"),
             py::arg("filepath"),
             py::arg("group") = GroupHandle::Invalid(),
             "Add an audio layer to a track.\n\n"
             "Layers are individual sounds that play simultaneously within a track.\n\n"
             "Args:\n"
             "    track (TrackHandle): Handle to the track\n"
             "    layer_name (str): Name identifier for the layer\n"
             "    filepath (str): Path to the audio file\n"
             "    group (GroupHandle, optional): Group handle to route the layer through (invalid = default/master)\n\n"
             "Raises:\n"
             "    InvalidHandleException: If track handle is invalid\n"
             "    FileLoadException: If audio file cannot be loaded")
        
        .def("remove_layer", &AudioManager::RemoveLayer,
             py::arg("track"),
             py::arg("layer_name"),
             "Remove a layer from a track.\n\n"
             "Args:\n"
             "    track (TrackHandle): Handle to the track\n"
             "    layer_name (str): Name of the layer to remove")
        
        .def("set_layer_volume", &AudioManager::SetLayerVolume,
             py::arg("track"),
             py::arg("layer_name"),
             py::arg("volume"),
             "Set the volume of a specific layer.\n\n"
             "Args:\n"
             "    track (TrackHandle): Handle to the track\n"
             "    layer_name (str): Name of the layer\n"
             "    volume (float): Volume level (0.0 to 1.0)")
        
        .def("fade_layer", &AudioManager::FadeLayer,
             py::arg("track"),
             py::arg("layer_name"),
             py::arg("target_volume"),
             py::arg("duration"),
             "Fade a layer's volume to a target value over time.\n\n"
             "Args:\n"
             "    track (TrackHandle): Handle to the track\n"
             "    layer_name (str): Name of the layer\n"
             "    target_volume (float): Target volume level (0.0 to 1.0)\n"
             "    duration (timedelta): Duration of the fade")
        
        // Group Operations
        .def("create_group", &AudioManager::CreateGroup,
             "Create a new audio group.\n\n"
             "Groups allow collective control of multiple sounds.\n\n"
             "Returns:\n"
             "    GroupHandle: Handle to the newly created group\n\n"
             "Raises:\n"
             "    AudioException: If group creation fails")
        
        .def("destroy_group", &AudioManager::DestroyGroup,
             py::arg("group"),
             "Destroy an audio group.\n\n"
             "Args:\n"
             "    group (GroupHandle): Handle to the group to destroy")
        
        .def("set_group_volume", &AudioManager::SetGroupVolume,
             py::arg("group"),
             py::arg("volume"),
             "Set the volume for an entire audio group.\n\n"
             "Affects all sounds assigned to this group.\n\n"
             "Args:\n"
             "    group (GroupHandle): Handle to the group\n"
             "    volume (float): Volume level (0.0 to 1.0)\n\n"
             "Raises:\n"
             "    InvalidHandleException: If group handle is invalid")
        
        .def("get_group_volume", &AudioManager::GetGroupVolume,
             py::arg("group"),
             "Get the current volume for an audio group.\n\n"
             "Args:\n"
             "    group (GroupHandle): Handle to the group\n\n"
             "Returns:\n"
             "    float: Current volume level (0.0 to 1.0)")
        
        .def("fade_group", &AudioManager::FadeGroup,
             py::arg("group"),
             py::arg("target_volume"),
             py::arg("duration"),
             "Fade a group's volume to a target value over time.\n\n"
             "Args:\n"
             "    group (GroupHandle): Handle to the group\n"
             "    target_volume (float): Target volume level (0.0 to 1.0)\n"
             "    duration (timedelta): Duration of the fade")
        
        // Sound Operations
        .def("load_sound", 
             py::overload_cast<const std::string&>(&AudioManager::LoadSound),
             py::arg("filepath"),
             "Load a sound from a file.\n\n"
             "Args:\n"
             "    filepath (str): Path to the audio file\n\n"
             "Returns:\n"
             "    SoundHandle: Handle to the loaded sound\n\n"
             "Raises:\n"
             "    FileLoadException: If the file cannot be loaded")
        
        .def("load_sound",
             py::overload_cast<const std::string&, GroupHandle>(&AudioManager::LoadSound),
             py::arg("filepath"),
             py::arg("group"),
             "Load a sound from a file and assign it to a group.\n\n"
             "Args:\n"
             "    filepath (str): Path to the audio file\n"
             "    group (GroupHandle): Handle to the audio group\n\n"
             "Returns:\n"
             "    SoundHandle: Handle to the loaded sound\n\n"
             "Raises:\n"
             "    FileLoadException: If the file cannot be loaded")
        
        .def("destroy_sound", &AudioManager::DestroySound,
             py::arg("sound"),
             "Destroy a previously loaded sound.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound to destroy")
        
        .def("play_sound", &AudioManager::PlaySound,
             py::arg("sound"),
             "Play a sound.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound to play\n\n"
             "Raises:\n"
             "    InvalidHandleException: If sound handle is invalid")
        
        .def("stop_sound", &AudioManager::StopSound,
             py::arg("sound"),
             "Stop a currently playing sound.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound to stop\n\n"
             "Raises:\n"
             "    InvalidHandleException: If sound handle is invalid")
        
        .def("set_sound_volume", &AudioManager::SetSoundVolume,
             py::arg("sound"),
             py::arg("volume"),
             "Set the volume of a sound.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound\n"
             "    volume (float): Volume level (0.0 to 1.0)\n\n"
             "Raises:\n"
             "    InvalidHandleException: If sound handle is invalid")
        
        .def("set_sound_pitch", &AudioManager::SetSoundPitch,
             py::arg("sound"),
             py::arg("pitch"),
             "Set the pitch of a sound for its next playback.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound\n"
             "    pitch (float): Pitch multiplier (1.0 = normal, 0.5 = half speed, 2.0 = double speed)")
        
        .def("is_sound_playing", &AudioManager::IsSoundPlaying,
             py::arg("sound"),
             "Check if a sound is currently playing.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound\n\n"
             "Returns:\n"
             "    bool: True if the sound is playing, False otherwise")
        
        .def("play_random_sound_from_folder", &AudioManager::PlayRandomSoundFromFolder,
             py::arg("folder_path"),
             py::arg("group") = GroupHandle::Invalid(),
             "Play a random sound from a folder.\n\n"
             "Loads all .wav files from the specified folder and plays one randomly.\n"
             "Sounds are cached after first load for efficiency.\n\n"
             "Args:\n"
             "    folder_path (str): Path to the folder containing sound files\n"
             "    group (GroupHandle, optional): Group handle to assign the sounds to");
}
