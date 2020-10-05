#pragma once

#include "GL.hpp"
#include "Load.hpp"
#include "Scene.hpp"

//Shader program that draws transformed, vertices tinted with vertex colors:
struct Texture2DProgram {
	Texture2DProgram();
	~Texture2DProgram();

	GLuint program = 0;
	//Attribute (per-vertex variable) locations:
	GLuint Position_vec4 = -1U;
	GLuint Color_vec4 = -1U;
	GLuint TexCoord_vec2 = -1U;
	//Textures:
	//TEXTURE0 - texture that is accessed by TexCoord

	struct Vertex {
		glm::vec2 Position;
		glm::u8vec4 Color;
		glm::vec2 TexCoord;
	};
	static_assert(sizeof(Vertex) == 2 * 4 + 4 * 1 + 2 * 4, "Vertex is packed.");

	struct BoxDrawable {
		GLuint vertex_array_ { 0 };
		GLuint vertex_buffer_ { 0 };

		void Clear();
	};

    GLuint GetVao(GLuint vertex_buffer) const;
	void SetBox(BoxDrawable& drawable, const glm::vec4& box, const glm::u8vec4& color) const;
	void DrawBox(const BoxDrawable& drawable) const;
};

extern Load<Texture2DProgram> texture2d_program;
