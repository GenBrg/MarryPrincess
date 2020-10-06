#include "Texture2DProgram.hpp"

#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

Load<Texture2DProgram> texture2d_program(LoadTagEarly);

static unsigned int box_index_buffer_content[]{0, 1, 2, 1, 2, 3};
static GLuint box_index_buffer{0};
static GLuint default_texture{0};

static Load<void> load_box_index_buffer(LoadTagEarly, []() {
	glGenBuffers(1, &box_index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box_index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), box_index_buffer_content, GL_STATIC_DRAW);
});

static Load<void> load_default_texture(LoadTagEarly, []() {
	glGenTextures(1, &default_texture);

	glBindTexture(GL_TEXTURE_2D, default_texture);
	std::vector< glm::u8vec4 > tex_data(1, glm::u8vec4(0xff));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
});

Texture2DProgram::Texture2DProgram() {
	//Compile vertex and fragment shaders using the convenient 'gl_compile_program' helper function:
	program = gl_compile_program(
		//vertex shader:
		"#version 330\n"
		"in vec4 Position;\n"
		"in vec4 Color;\n"
		"in vec2 TexCoord;\n"
		"out vec2 texCoord;\n"
		"out vec4 color;\n"
		"void main() {\n"
		"	gl_Position = Position;\n"
		"	texCoord = TexCoord;\n"
		"	color = Color;\n"
		"}\n"
	,
		//fragment shader:
		"#version 330\n"
		"uniform sampler2D TEX;\n"
		"in vec4 color;\n"
		"in vec2 texCoord;\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"	fragColor = texture(TEX, texCoord) * color;\n"
		"}\n"
	);
	//As you can see above, adjacent strings in C/C++ are concatenated.
	// this is very useful for writing long shader programs inline.

	//look up the locations of vertex attributes:
	Position_vec4 = glGetAttribLocation(program, "Position");
	Color_vec4 = glGetAttribLocation(program, "Color");
	TexCoord_vec2 = glGetAttribLocation(program, "TexCoord");

	//look up the locations of uniforms:
	GLuint TEX_sampler2D = glGetUniformLocation(program, "TEX");

	//set TEX to always refer to texture binding zero:
	glUseProgram(program); //bind program -- glUniform* calls refer to this program now

	glUniform1i(TEX_sampler2D, 0); //set TEX to sample from GL_TEXTURE0

	glUseProgram(0); //unbind program -- glUniform* calls refer to ??? now
}

Texture2DProgram::~Texture2DProgram() {
	glDeleteProgram(program);
	program = 0;
}

GLuint Texture2DProgram::GetVao(GLuint vertex_buffer) const
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    glEnableVertexAttribArray(Position_vec4);
    glVertexAttribPointer(Position_vec4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

	glEnableVertexAttribArray(Color_vec4);
    glVertexAttribPointer(Color_vec4, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void*)offsetof(Vertex, Color));

    glEnableVertexAttribArray(TexCoord_vec2);
    glVertexAttribPointer(TexCoord_vec2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoord));

    return vao;
}

void Texture2DProgram::SetBox(BoxDrawable& drawable, const glm::vec4& box, const glm::u8vec4& color) const
{
	drawable.Clear();

	glGenBuffers(1, &drawable.vertex_buffer_);
	drawable.vertex_array_ = texture2d_program->GetVao(drawable.vertex_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, drawable.vertex_buffer_);

	Vertex vertexes[]{
			{{box[0], box[1]}, color, {0, 1}},
			{{box[2], box[1]}, color, {1, 1}},
			{{box[0], box[3]}, color, {0, 0}},
			{{box[2], box[3]}, color, {1, 0}}};
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), static_cast<const void *>(vertexes), GL_STATIC_DRAW);
}

void Texture2DProgram::SetCircle(CircleDrawable& drawable, const glm::vec2& origin, float radius, int vertex_count, const glm::u8vec4& color) const
{
	drawable.Clear();
	
	glGenBuffers(1, &drawable.vertex_buffer_);
	drawable.vertex_array_ = texture2d_program->GetVao(drawable.vertex_buffer_);
	drawable.vertex_count_ = vertex_count;
	glBindBuffer(GL_ARRAY_BUFFER, drawable.vertex_buffer_);

	std::vector<Vertex> vertexes(vertex_count);

	
	
	float angle_inc = 360.0f / vertex_count;
	float angle = 0.0f;

	for (int i = 0; i < vertex_count; ++i) {
		Vertex& vertex = vertexes[i];
		vertex.Color = color;
		vertex.Position[0] = origin[0] + radius * glm::cos(glm::radians(angle));
		vertex.Position[1] = origin[1] + radius * glm::sin(glm::radians(angle));
		angle += angle_inc;
	}

	glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), static_cast<const void *>(vertexes.data()), GL_STATIC_DRAW);
}

void Texture2DProgram::DrawBox(const BoxDrawable& drawable) const
{
	glUseProgram(program);
	glBindVertexArray(drawable.vertex_array_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box_index_buffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, default_texture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, static_cast<const void *>(0));
}

void Texture2DProgram::DrawCircle(const CircleDrawable& drawable) const
{
	glUseProgram(program);
	glBindVertexArray(drawable.vertex_array_);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, default_texture);
	glDrawArrays(GL_LINE_LOOP, 0, drawable.vertex_count_);
}

void Texture2DProgram::BoxDrawable::Clear()
{
	if (vertex_buffer_ > 0) {
		glDeleteBuffers(1, &vertex_buffer_);
		vertex_buffer_ = 0;
	}

	if (vertex_array_ > 0) {
		glDeleteVertexArrays(1, &vertex_array_);
		vertex_array_ = 0;
	}
}

void Texture2DProgram::CircleDrawable::Clear()
{
	if (vertex_buffer_ > 0) {
		glDeleteBuffers(1, &vertex_buffer_);
		vertex_buffer_ = 0;
	}

	if (vertex_array_ > 0) {
		glDeleteVertexArrays(1, &vertex_array_);
		vertex_array_ = 0;
	}
}
