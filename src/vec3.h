#pragma once

#include <cmath>

/**
 * @file vec3.h
 * @brief 3D vector class for spatial audio positioning
 * 
 * This is an engine-agnostic 3D vector implementation for spatial audio.
 * It provides basic vector operations needed for 3D audio positioning.
 * 
 * Coordinate System:
 * The audio system uses the OpenGL/miniaudio coordinate system:
 * - Positive X: Right
 * - Positive Y: Up
 * - Negative Z: Forward (camera looks down -Z axis)
 * 
 * This matches most 3D game engines. To convert from your engine's
 * coordinate system, simply map your engine's X, Y, Z components to
 * Vec3's x, y, z components. If your engine uses a different handedness
 * or axis orientation, you may need to swap or negate components.
 */

namespace audio {

/**
 * @struct Vec3
 * @brief 3D vector for spatial audio positioning
 * 
 * Represents a position or direction in 3D space. This is engine-agnostic
 * and can be used with any game engine by converting from the engine's
 * vector type to Vec3.
 * 
 * The Vec3 struct uses simple x, y, z float components, making it easy
 * to convert from any engine's vector type (e.g., Basilisk Engine nodes,
 * Unity Vector3, Unreal FVector, etc.).
 * 
 * Example conversion from a game engine node:
 * ```cpp
 * // If your engine node has position as (x, y, z) tuple or struct
 * auto node_pos = node.get_position();
 * audio::Vec3 audio_pos(node_pos.x, node_pos.y, node_pos.z);
 * audio.SetSoundPosition(sound, audio_pos);
 * ```
 */
struct Vec3 {
    float x;  ///< X component
    float y;  ///< Y component
    float z;  ///< Z component

    /**
     * @brief Default constructor (initializes to origin)
     */
    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}

    /**
     * @brief Constructor with components
     * 
     * @param x X component
     * @param y Y component
     * @param z Z component
     */
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    /**
     * @brief Get the length of the vector
     * 
     * @return float Length of the vector
     */
    float Length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    /**
     * @brief Get the squared length of the vector (faster, no sqrt)
     * 
     * @return float Squared length of the vector
     */
    float LengthSquared() const {
        return x * x + y * y + z * z;
    }

    /**
     * @brief Normalize the vector in place
     */
    void Normalize() {
        float len = Length();
        if (len > 0.0f) {
            x /= len;
            y /= len;
            z /= len;
        }
    }

    /**
     * @brief Get a normalized copy of the vector
     * 
     * @return Vec3 Normalized vector
     */
    Vec3 Normalized() const {
        Vec3 result = *this;
        result.Normalize();
        return result;
    }

    /**
     * @brief Calculate distance to another point
     * 
     * @param other Other point
     * @return float Distance between points
     */
    float Distance(const Vec3& other) const {
        return (*this - other).Length();
    }

    /**
     * @brief Calculate squared distance to another point (faster, no sqrt)
     * 
     * @param other Other point
     * @return float Squared distance between points
     */
    float DistanceSquared(const Vec3& other) const {
        return (*this - other).LengthSquared();
    }

    // Arithmetic operators
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    Vec3 operator/(float scalar) const {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vec3& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vec3& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    bool operator==(const Vec3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Vec3& other) const {
        return !(*this == other);
    }
};

/**
 * @brief Dot product of two vectors
 * 
 * @param a First vector
 * @param b Second vector
 * @return float Dot product
 */
inline float Dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

/**
 * @brief Cross product of two vectors
 * 
 * @param a First vector
 * @param b Second vector
 * @return Vec3 Cross product
 */
inline Vec3 Cross(const Vec3& a, const Vec3& b) {
    return Vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

} // namespace audio
