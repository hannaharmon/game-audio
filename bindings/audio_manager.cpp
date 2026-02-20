#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <functional>

#include "audio_manager.h"
#include "vec3.h"

namespace py = pybind11;
using namespace audio;

void bind_audio_manager(py::module_& m) {
    // Bind Vec3 for 3D positions
    py::class_<Vec3>(m, "Vec3",
        "3D vector for spatial audio positioning.\n\n"
        "Represents a position or direction in 3D space. This is engine-agnostic\n"
        "and can be used with any game engine by converting from the engine's\n"
        "vector type to Vec3.\n\n"
        "Coordinate System:\n"
        "The audio system uses OpenGL/miniaudio convention:\n"
        "- Positive X: Right\n"
        "- Positive Y: Up\n"
        "- Negative Z: Forward (camera looks down -Z axis)\n\n"
        "To convert from a game engine (e.g., Basilisk Engine nodes):\n"
        "  node_pos = node.get_position()\n"
        "  audio_pos = game_audio.Vec3(node_pos.x, node_pos.y, node_pos.z)\n"
        "  audio.set_sound_position(sound, audio_pos)")
        .def(py::init<>(), "Create a Vec3 at the origin (0, 0, 0)")
        .def(py::init<float, float, float>(), 
             py::arg("x"), py::arg("y"), py::arg("z"),
             "Create a Vec3 with components.\n\n"
             "Args:\n"
             "    x (float): X component\n"
             "    y (float): Y component\n"
             "    z (float): Z component")
        .def_readwrite("x", &Vec3::x, "X component")
        .def_readwrite("y", &Vec3::y, "Y component")
        .def_readwrite("z", &Vec3::z, "Z component")
        .def("length", &Vec3::Length,
             "Get the length of the vector.\n\n"
             "Returns:\n"
             "    float: Length of the vector")
        .def("length_squared", &Vec3::LengthSquared,
             "Get the squared length of the vector (faster, no sqrt).\n\n"
             "Returns:\n"
             "    float: Squared length of the vector")
        .def("normalize", &Vec3::Normalize,
             "Normalize the vector in place.")
        .def("normalized", &Vec3::Normalized,
             "Get a normalized copy of the vector.\n\n"
             "Returns:\n"
             "    Vec3: Normalized vector")
        .def("distance", &Vec3::Distance,
             py::arg("other"),
             "Calculate distance to another point.\n\n"
             "Args:\n"
             "    other (Vec3): Other point\n\n"
             "Returns:\n"
             "    float: Distance between points")
        .def("distance_squared", &Vec3::DistanceSquared,
             py::arg("other"),
             "Calculate squared distance to another point (faster, no sqrt).\n\n"
             "Args:\n"
             "    other (Vec3): Other point\n\n"
             "Returns:\n"
             "    float: Squared distance between points")
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self * float())
        .def(py::self / float())
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self *= float())
        .def(py::self /= float())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__repr__", [](const Vec3& v) {
            return "Vec3(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
        });

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
        
        .def("set_sound_looping", &AudioManager::SetSoundLooping,
             py::arg("sound"),
             py::arg("should_loop"),
             "Set whether a sound should loop.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound\n"
             "    should_loop (bool): Whether the sound should loop continuously\n\n"
             "Raises:\n"
             "    InvalidHandleException: If sound handle is invalid")
        
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
             "    group (GroupHandle, optional): Group handle to assign the sounds to")
        
        // Spatial Audio (3D Positioning)
        .def("set_listener_position", &AudioManager::SetListenerPosition,
             py::arg("position"),
             py::arg("listener_index") = 0,
             "Set the listener position in 3D space.\n\n"
             "The listener represents the 'ears' of the player/camera.\n"
             "All spatialized sounds are positioned relative to the listener.\n\n"
             "Args:\n"
             "    position (Vec3): 3D position of the listener\n"
             "    listener_index (int, optional): Index of the listener (default 0)")
        
        .def("get_listener_position", &AudioManager::GetListenerPosition,
             py::arg("listener_index") = 0,
             "Get the listener position.\n\n"
             "Args:\n"
             "    listener_index (int, optional): Index of the listener (default 0)\n\n"
             "Returns:\n"
             "    Vec3: Current listener position")
        
        .def("set_listener_direction", &AudioManager::SetListenerDirection,
             py::arg("direction"),
             py::arg("listener_index") = 0,
             "Set the listener direction (forward vector).\n\n"
             "The direction vector represents which way the listener is facing.\n"
             "Should be normalized.\n\n"
             "Args:\n"
             "    direction (Vec3): Forward direction vector (should be normalized)\n"
             "    listener_index (int, optional): Index of the listener (default 0)")
        
        .def("get_listener_direction", &AudioManager::GetListenerDirection,
             py::arg("listener_index") = 0,
             "Get the listener direction.\n\n"
             "Args:\n"
             "    listener_index (int, optional): Index of the listener (default 0)\n\n"
             "Returns:\n"
             "    Vec3: Current listener direction")
        
        .def("set_listener_up", &AudioManager::SetListenerUp,
             py::arg("up"),
             py::arg("listener_index") = 0,
             "Set the listener up vector.\n\n"
             "The up vector defines the orientation of the listener.\n"
             "Typically (0, 1, 0) for a standard Y-up coordinate system.\n\n"
             "Args:\n"
             "    up (Vec3): Up vector (should be normalized)\n"
             "    listener_index (int, optional): Index of the listener (default 0)")
        
        .def("get_listener_up", &AudioManager::GetListenerUp,
             py::arg("listener_index") = 0,
             "Get the listener up vector.\n\n"
             "Args:\n"
             "    listener_index (int, optional): Index of the listener (default 0)\n\n"
             "Returns:\n"
             "    Vec3: Current listener up vector")
        
        .def("set_sound_position", &AudioManager::SetSoundPosition,
             py::arg("sound"),
             py::arg("position"),
             "Set the 3D position of a sound.\n\n"
             "Sets the position for the sound. The sound will be spatialized\n"
             "relative to the listener position.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound\n"
             "    position (Vec3): 3D position of the sound\n\n"
             "Raises:\n"
             "    InvalidHandleException: If sound handle is invalid")
        
        .def("get_sound_position", &AudioManager::GetSoundPosition,
             py::arg("sound"),
             "Get the 3D position of a sound.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound\n\n"
             "Returns:\n"
             "    Vec3: Current 3D position\n\n"
             "Raises:\n"
             "    InvalidHandleException: If sound handle is invalid")
        
        .def("set_sound_min_distance", &AudioManager::SetSoundMinDistance,
             py::arg("sound"),
             py::arg("min_distance"),
             "Set the minimum distance for distance attenuation.\n\n"
             "At distances less than minDistance, the sound will be at full volume.\n"
             "Beyond minDistance, volume will attenuate based on the attenuation model.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound\n"
             "    min_distance (float): Minimum distance (must be > 0)\n\n"
             "Raises:\n"
             "    InvalidHandleException: If sound handle is invalid")
        
        .def("get_sound_min_distance", &AudioManager::GetSoundMinDistance,
             py::arg("sound"),
             "Get the minimum distance of a sound.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound\n\n"
             "Returns:\n"
             "    float: Current minimum distance\n\n"
             "Raises:\n"
             "    InvalidHandleException: If sound handle is invalid")
        
        .def("set_sound_max_distance", &AudioManager::SetSoundMaxDistance,
             py::arg("sound"),
             py::arg("max_distance"),
             "Set the maximum distance for distance attenuation.\n\n"
             "At distances beyond maxDistance, the sound will be at minimum gain.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound\n"
             "    max_distance (float): Maximum distance (must be > minDistance)\n\n"
             "Raises:\n"
             "    InvalidHandleException: If sound handle is invalid")
        
        .def("get_sound_max_distance", &AudioManager::GetSoundMaxDistance,
             py::arg("sound"),
             "Get the maximum distance of a sound.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound\n\n"
             "Returns:\n"
             "    float: Current maximum distance\n\n"
             "Raises:\n"
             "    InvalidHandleException: If sound handle is invalid")
        
        .def("set_sound_rolloff", &AudioManager::SetSoundRolloff,
             py::arg("sound"),
             py::arg("rolloff"),
             "Set the rolloff factor for distance attenuation.\n\n"
             "Higher values mean faster volume dropoff with distance.\n"
             "Typical values: 1.0 (linear), 2.0 (inverse square)\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound\n"
             "    rolloff (float): Rolloff factor (typically 1.0 to 2.0)\n\n"
             "Raises:\n"
             "    InvalidHandleException: If sound handle is invalid")
        
        .def("get_sound_rolloff", &AudioManager::GetSoundRolloff,
             py::arg("sound"),
             "Get the rolloff factor of a sound.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound\n\n"
             "Returns:\n"
             "    float: Current rolloff factor\n\n"
             "Raises:\n"
             "    InvalidHandleException: If sound handle is invalid")
        
        .def("set_sound_spatialization_enabled", &AudioManager::SetSoundSpatializationEnabled,
             py::arg("sound"),
             py::arg("enabled"),
             "Enable or disable spatialization for a sound.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound\n"
             "    enabled (bool): Whether to enable spatialization\n\n"
             "Raises:\n"
             "    InvalidHandleException: If sound handle is invalid")
        
        .def("is_sound_spatialization_enabled", &AudioManager::IsSoundSpatializationEnabled,
             py::arg("sound"),
             "Check if spatialization is enabled for a sound.\n\n"
             "Args:\n"
             "    sound (SoundHandle): Handle to the sound\n\n"
             "Returns:\n"
             "    bool: True if spatialization is enabled\n\n"
             "Raises:\n"
             "    InvalidHandleException: If sound handle is invalid");
}
