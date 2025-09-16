#pragma once

#include <vendor/glm/glm.hpp>
#include <yaml-cpp/yaml.h>
#include <string>

namespace YAML
{
    template<>
    struct convert<glm::vec3>
    {
        static YAML::Node encode(const glm::vec3& rhs)
        {
            YAML::Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const YAML::Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4>
    {
        static YAML::Node encode(const glm::vec4& rhs)
        {
            YAML::Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const YAML::Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    template<>
    struct convert<glm::mat4>
    {
        static YAML::Node encode(const glm::mat4& rhs)
        {
            YAML::Node node;
            // Store all 16 values in column-major order
            for (int col = 0; col < 4; col++) {
                for (int row = 0; row < 4; row++) {
                    node.push_back(rhs[col][row]);
                }
            }
            return node;
        }

        static bool decode(const YAML::Node& node, glm::mat4& rhs)
        {
            if (!node.IsSequence() || node.size() != 16)
                return false;

            // Read all 16 values in column-major order
            for (int col = 0; col < 4; col++) {
                for (int row = 0; row < 4; row++) {
                    rhs[col][row] = node[col * 4 + row].as<float>();
                }
            }
            return true;
        }
    };

    inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::mat4& m)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq;
        for (int col = 0; col < 4; col++) {
            for (int row = 0; row < 4; row++) {
                out << m[col][row];
            }
        }
        out << YAML::EndSeq;
        return out;
    }
}

namespace Cast::Serialization
{
    // Base64 encoding utility functions
    inline std::string Base64Encode(const void* data, const size_t size)
    {
        static constexpr char base64Chars[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        std::string result;
        result.reserve((size + 2) / 3 * 4); // Reserve space for the encoded data

        const auto bytes = static_cast<const unsigned char*>(data);

        for (size_t i = 0; i < size; i += 3)
        {
            // Load up to 3 bytes into a 24-bit buffer
            unsigned int buffer = bytes[i] << 16;
            if (i + 1 < size) buffer |= bytes[i + 1] << 8;
            if (i + 2 < size) buffer |= bytes[i + 2];

            // Extract four 6-bit values
            result.push_back(base64Chars[(buffer >> 18) & 0x3F]);
            result.push_back(base64Chars[(buffer >> 12) & 0x3F]);
            result.push_back((i + 1 < size) ? base64Chars[(buffer >> 6) & 0x3F] : '=');
            result.push_back((i + 2 < size) ? base64Chars[buffer & 0x3F] : '=');
        }

        return result;
    }

    inline unsigned char* Base64Decode(const std::string& encoded, const size_t size = 0)
    {
        static const unsigned char lookup[256] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63,
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0,
            0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0,
            0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0
        };

        const size_t maxSize = size == 0 ? (encoded.size() * 3) / 4 : size;
        const auto result = (unsigned char*)malloc(maxSize);
        size_t resultIndex = 0;

        unsigned int buffer = 0;
        int bits = 0;

        for (const char c : encoded)
        {
            if (c == '=') break; // Padding character, end of data

            if ((c < 0) || (c > 255) || (lookup[c] == 0 && c != 'A'))
                continue; // Skip invalid characters

            buffer = (buffer << 6) | lookup[c];
            bits += 6;

            if (bits >= 8 && resultIndex < maxSize)
            {
                bits -= 8;
                result[resultIndex++] = (buffer >> bits) & 0xFF;
            }
        }

        return result;
    }
}