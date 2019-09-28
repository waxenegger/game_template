#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP

	#include "includes.hpp"

	class Vertex {
	public:
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;

		Vertex(glm::vec3 position) {
			this->position = position;
		}
	};

	class Mesh {
	private:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		GLuint VAO = 0, VBO = 0, EBO = 0;

		void setupMesh() {
			// create buffers/arrays
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			// vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
			// vertex texture coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

			glBindVertexArray(0);
		}

	public:
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
			this->vertices = vertices;
			this->indices = indices;
			this->setupMesh();
		}

		void render() {
			glBindVertexArray(this->VAO);
			glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
		}
	};

	class Renderable {
	private:
		std::vector<Mesh> meshes;

	public:
		Renderable() {}
		Renderable(const std::vector<Mesh> & meshes) {
			this->meshes = meshes;
		}

		void render() {
			for(auto & mesh: this->meshes) mesh.render();
		}

		void addMesh(const Mesh & mesh) {
			this->meshes.push_back(mesh);
		}

		void loadMeshFromObjectFile(const std::string & file) {
			Assimp::Importer importer;
			const aiScene *scene = importer.ReadFile(file.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
			if (scene == nullptr) {
				std::cerr << importer.GetErrorString() << std::endl;
				return;
			}

			if (scene->HasMeshes()) {
				for (unsigned int m = 0;m<scene->mNumMeshes; m++) {
					const aiMesh * someMesh = scene->mMeshes[m];
					std::vector<Vertex>  vertices;
					std::vector<unsigned int> indexes;

					for (unsigned int t = 0; t < someMesh->mNumVertices; ++t) {
						const aiVector3D someVec = someMesh->mVertices[t];
						vertices.push_back(Vertex(glm::vec3(someVec.x, someVec.y, someVec.z)));
					}

					for (unsigned int t = 0; t<someMesh->mNumFaces; t++) {
						const struct aiFace face = someMesh->mFaces[t];

						for(unsigned int i = 0; i < face.mNumIndices; i++) {
							int index = face.mIndices[i];
							indexes.push_back(index);
						}
					}
					this->meshes.push_back(Mesh(vertices, indexes));
				}
			}
		}
	};

	class Shader final {
	private:
		static const int NUM_SHADERS = 2;
		std::string m_file_name;
		GLuint m_program;
		GLuint m_shaders[NUM_SHADERS];

		std::string read(const int type) const {
			std::ifstream file;
			std::string prefixed_file_name = this->m_file_name;
			if (type == GL_FRAGMENT_SHADER) prefixed_file_name += ".fs";
			else prefixed_file_name += ".vs";
			file.open(prefixed_file_name.c_str());

			std::string output;
			std::string line;
			if(file.is_open()) {
				while(file.good()) {
					getline(file, line);
					output.append(line + "\n");
				}
				//cout << output << endl;
			} else std::cerr << "Unable to read shader: " << this->m_file_name << std::endl;

			return output;
		}

		void checkForError(GLuint shader, GLuint flag, bool isProgram, const std::string & errorMessage) const {
			GLint success = 0;
			GLchar error[1024] = { 0 };

			if(isProgram)
				glGetProgramiv(shader, flag, &success);
			else
				glGetShaderiv(shader, flag, &success);

			if(success == GL_FALSE)
			{
				if(isProgram)
					glGetProgramInfoLog(shader, sizeof(error), NULL, error);
				else
					glGetShaderInfoLog(shader, sizeof(error), NULL, error);

				std::cerr << errorMessage << ": '" << error << "'" << std::endl;
			}
		}

		GLuint create(unsigned int type) const {
			const std::string text = this->read(type);
			GLuint shader = glCreateShader(type);

			if(shader == 0) std::cerr << "Error compiling shader type " << type << std::endl;

			const GLchar* p[1];
			p[0] = text.c_str();
			GLint lengths[1];
			lengths[0] = text.length();

			glShaderSource(shader, 1, p, lengths);
			glCompileShader(shader);

			this->checkForError(shader, GL_COMPILE_STATUS, false, "Error compiling shader!");

			return shader;
		}

	public:
		Shader(const std::string & file_name) {
			this->m_file_name = file_name;
			this->m_program = glCreateProgram();
			this->m_shaders[0] = this->create(GL_VERTEX_SHADER);
			this->m_shaders[1] = this->create(GL_FRAGMENT_SHADER);

			for(unsigned int i = 0; i < this->NUM_SHADERS; i++)
				glAttachShader(this->m_program, this->m_shaders[i]);

			glBindAttribLocation(m_program, 0, "position");

			glLinkProgram(m_program);
			this->checkForError(this->m_program, GL_LINK_STATUS, true, "Error linking shader program");

			glValidateProgram(m_program);
			this->checkForError(this->m_program, GL_LINK_STATUS, true, "Invalid shader program");
		}

		virtual ~Shader() {
			for(unsigned int i = 0; i < this->NUM_SHADERS; i++) {
				glDetachShader(this->m_program, this->m_shaders[i]);
				glDeleteShader(this->m_shaders[i]);
			}

			glDeleteProgram(this->m_program);
		}

		void setBool(const std::string &name, bool value) const {
			glUniform1i(glGetUniformLocation(this->m_program, name.c_str()), (int)value);
		}

		void setInt(const std::string &name, int value) const {
			glUniform1i(glGetUniformLocation(this->m_program, name.c_str()), value);
		}

		void setFloat(const std::string &name, float value) const
		{
			glUniform1f(glGetUniformLocation(this->m_program, name.c_str()), value);
		}

		void setVec2(const std::string &name, const glm::vec2 &value) const
		{
			glUniform2fv(glGetUniformLocation(this->m_program, name.c_str()), 1, &value[0]);
		}
		void setVec2(const std::string &name, float x, float y) const
		{
			glUniform2f(glGetUniformLocation(this->m_program, name.c_str()), x, y);
		}
		void setVec3(const std::string &name, const glm::vec3 &value) const
		{
			glUniform3fv(glGetUniformLocation(this->m_program, name.c_str()), 1, &value[0]);
		}
		void setVec3(const std::string &name, float x, float y, float z) const
		{
			glUniform3f(glGetUniformLocation(this->m_program, name.c_str()), x, y, z);
		}
		void setVec4(const std::string &name, const glm::vec4 &value) const
		{
			glUniform4fv(glGetUniformLocation(this->m_program, name.c_str()), 1, &value[0]);
		}
		void setVec4(const std::string &name, float x, float y, float z, float w)
		{
			glUniform4f(glGetUniformLocation(this->m_program, name.c_str()), x, y, z, w);
		}
		void setMat2(const std::string &name, const glm::mat2 &mat) const
		{
			glUniformMatrix2fv(glGetUniformLocation(this->m_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		void setMat3(const std::string &name, const glm::mat3 &mat) const
		{
			glUniformMatrix3fv(glGetUniformLocation(this->m_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		void setMat4(const std::string &name, const glm::mat4 &mat) const
		{
			glUniformMatrix4fv(glGetUniformLocation(this->m_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		GLuint getId() const {
			return this->m_program;
		}

		void use() const {
			glUseProgram(this->m_program);
		}
	};
#endif
