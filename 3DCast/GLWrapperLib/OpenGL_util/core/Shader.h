#pragma once

#include "vendor/glm/glm.hpp"
#include "debug/Debug.hpp"

#include "misc/Material.hpp"
#include "misc/Light.hpp"

namespace GL::Core {
	struct ShaderProgramSource {
		std::string VertexSource;
		std::string FragmentSource;
	};

	class Shader
	{
	public:
		Shader(const std::string& path_vert, const std::string& path_frag);
		~Shader();

		void Bind() const;

		void Unbind() const;

		void SetUniform1i(const std::string& name, int v0);

		void SetUniform1f(const std::string& name, float v0);
		void SetUniform2f(const std::string& name, float v0, float v1);
		void SetUniform3f(const std::string& name, float v0, float v1, float v2);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

		void SetUniform1iv(const std::string& name, unsigned int size, int* v);

		void SetUniform1fv(const std::string& name, unsigned int size, float* v);
		void SetUniform3fv(const std::string& name, unsigned int size, glm::vec3* v);

		void SetUniformMaterial(const std::string& name, material::Material& m);

		void SetUniformConstantLight(const std::string& name, light::ConstantLight& m);
		void SetUniformSpotLight(const std::string& name, light::Spotlight& m);
		void SetUniformPointLight(const std::string& name, light::PointLight& m);
		void SetUniformDirectionalLight(const std::string& name, light::DirectionalLight& m);

		void SetUniformConstantLight(const std::string& name, light::ConstantLight& m, unsigned int index);
		void SetUniformSpotLight(const std::string& name, light::Spotlight& m, unsigned int index);
		void SetUniformPointLight(const std::string& name, light::PointLight& m, unsigned int index);
		void SetUniformDirectionalLight(const std::string& name, light::DirectionalLight& m, unsigned int index);

		void SetUniformMat4f(const std::string& name, const glm::mat4& mat);

	private:
		unsigned int m_RendererID;
		const std::string& m_PathVert;
		const std::string& m_PathFrag;

		mutable std::unordered_map<std::string, int> m_UniformLacationCache{};

		int GetUniformLocation(const std::string& name) const;

		const std::string StringFromPath(const std::string& path);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		ShaderProgramSource ParseShader(const std::string path_vertex, const std::string path_frag);
	};
}