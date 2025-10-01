#pragma once

#include <vendor/glm/glm.hpp>
#include <vector>

namespace Math
{
    // Structure to hold tangent space data
    struct TangentSpace
    {
        glm::vec3 tangent{1.0f, 0.0f, 0.0f};
        glm::vec3 bitangent{0.0f, 0.0f, 1.0f};
    };

    /**
     * Calculate a normal vector from three triangle vertices
     * @param v0 First vertex position
     * @param v1 Second vertex position
     * @param v2 Third vertex position
     * @return Normalized normal vector
     */
    inline glm::vec3 CalculateNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
    {
        return glm::normalize(glm::cross(v1 - v0, v2 - v0));
    }

    /**
     * Calculate tangent space vectors from triangle vertices and their UVs
     * @param v0 First vertex position
     * @param v1 Second vertex position
     * @param v2 Third vertex position
     * @param uv0 First vertex texture coordinates
     * @param uv1 Second vertex texture coordinates
     * @param uv2 Third vertex texture coordinates
     * @return TangentSpace structure with tangent and bitangent vectors
     */
    inline TangentSpace CalculateTangentSpace(
        const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
        const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2)
    {
        TangentSpace result;

        const glm::vec3 edge1 = v1 - v0;
        const glm::vec3 edge2 = v2 - v0;
        const glm::vec2 deltaUV1 = uv1 - uv0;
        const glm::vec2 deltaUV2 = uv2 - uv0;

        const float denominator = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        const float f = denominator != 0.0f ? 1.0f / denominator : 0.0f;

        result.tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        result.tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        result.tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        result.bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        result.bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        result.bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        return result;
    }

    /**
     * Orthogonalize tangent with respect to normal using Gram-Schmidt process
     * @param tangent Tangent vector to orthogonalize
     * @param normal Normal vector to orthogonalize against
     * @return Orthogonalized tangent vector
     */
    inline glm::vec3 OrthogonalizeTangent(const glm::vec3& tangent, const glm::vec3& normal)
    {
        // Gram-Schmidt orthogonalization
        return glm::normalize(tangent - normal * glm::dot(normal, tangent));
    }

    /**
     * Generate default tangent space from a normal vector
     * @param normal Normal vector
     * @return TangentSpace structure with default tangent and bitangent
     */
    inline TangentSpace GenerateDefaultTangentSpace(const glm::vec3& normal)
    {
        TangentSpace result;

        // Choose a tangent vector not collinear with normal
        if (std::abs(normal.x) < 0.9f)
            result.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
        else
            result.tangent = glm::vec3(0.0f, 1.0f, 0.0f);

        // Orthogonalize
        result.tangent = OrthogonalizeTangent(result.tangent, normal);

        // Calculate bitangent to ensure right-handed coordinate system
        result.bitangent = glm::cross(normal, result.tangent);

        return result;
    }

    /**
     * Calculate normals for mesh vertices by averaging face normals
     * @param positions Vertex positions
     * @param indices Triangle indices (must be in groups of 3)
     * @return Vector of calculated normals
     */
    inline std::vector<glm::vec3> CalculateNormals(
        const std::vector<glm::vec3>& positions,
        const std::vector<unsigned int>& indices)
    {
        std::vector<glm::vec3> normals(positions.size(), glm::vec3(0.0f));

        // Calculate and accumulate face normals
        for (size_t i = 0; i < indices.size(); i += 3) {
            const unsigned int idx0 = indices[i];
            const unsigned int idx1 = indices[i + 1];
            const unsigned int idx2 = indices[i + 2];

            const glm::vec3 normal = CalculateNormal(positions[idx0], positions[idx1], positions[idx2]);

            normals[idx0] += normal;
            normals[idx1] += normal;
            normals[idx2] += normal;
        }

        // Normalize accumulated normals
        for (auto& normal : normals) {
            normal = glm::normalize(normal);
        }

        return normals;
    }

    /**
     * Calculate tangent space vectors for mesh vertices
     * @param positions Vertex positions
     * @param texCoords Vertex texture coordinates
     * @param normals Vertex normals
     * @param indices Triangle indices (must be in groups of 3)
     * @return Vector of TangentSpace structures for each vertex
     */
    inline std::vector<TangentSpace> CalculateTangentSpaces(
        const std::vector<glm::vec3>& positions,
        const std::vector<glm::vec2>& texCoords,
        const std::vector<glm::vec3>& normals,
        const std::vector<unsigned int>& indices)
    {
        std::vector<TangentSpace> tangentSpaces(positions.size(), TangentSpace{});

        // Calculate and accumulate tangent spaces for each triangle
        for (size_t i = 0; i < indices.size(); i += 3) {
            const unsigned int idx0 = indices[i];
            const unsigned int idx1 = indices[i + 1];
            const unsigned int idx2 = indices[i + 2];

            auto [tangent, bitangent] = CalculateTangentSpace(
                positions[idx0], positions[idx1], positions[idx2],
                texCoords[idx0], texCoords[idx1], texCoords[idx2]
            );

            tangentSpaces[idx0].tangent += tangent;
            tangentSpaces[idx1].tangent += tangent;
            tangentSpaces[idx2].tangent += tangent;

            tangentSpaces[idx0].bitangent += bitangent;
            tangentSpaces[idx1].bitangent += bitangent;
            tangentSpaces[idx2].bitangent += bitangent;
        }

        // Normalize and orthogonalize
        for (size_t i = 0; i < tangentSpaces.size(); i++) {
            tangentSpaces[i].tangent = glm::normalize(tangentSpaces[i].tangent);
            tangentSpaces[i].bitangent = glm::normalize(tangentSpaces[i].bitangent);

            // Orthogonalize tangent with respect to normal
            tangentSpaces[i].tangent = OrthogonalizeTangent(tangentSpaces[i].tangent, normals[i]);

            // Ensure right-handed coordinate system
            if (glm::dot(glm::cross(normals[i], tangentSpaces[i].tangent), tangentSpaces[i].bitangent) < 0.0f) {
                tangentSpaces[i].tangent = -tangentSpaces[i].tangent;
            }

            // Recalculate bitangent to ensure orthogonality
            tangentSpaces[i].bitangent = glm::cross(normals[i], tangentSpaces[i].tangent);
        }

        return tangentSpaces;
    }
}