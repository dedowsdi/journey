struct blinn_program : public zxd::program {
  GLint al_vertex;
  GLint al_normal;

  virtual void updateModel(const mat4 &_modelMatrix) {
    modelMatrix = _modelMatrix;
    modelViewMatrix = viewMatrix * modelMatrix;
    modelViewMatrixInverseTranspose =
      glm::inverse(glm::transpose(modelViewMatrix));
    modelViewProjMatrix = projMatrix * modelViewMatrix;

    glUniformMatrix4fv(loc_modelViewMatrixInverseTranspose, 1, 0,
      value_ptr(modelViewMatrixInverseTranspose));
    glUniformMatrix4fv(loc_modelViewMatrix, 1, 0, value_ptr(modelViewMatrix));
    glUniformMatrix4fv(
      loc_modelViewProjMatrix, 1, 0, value_ptr(modelViewProjMatrix));
  }
  virtual void attach_shaders() {
    attach_shader_file(GL_VERTEX_SHADER, "data/shader/blinn.vs.glsl");
    string_vector sv;
    sv.push_back("#version 430 core\n #define LIGHT_COUNT 8\n");
    sv.push_back(readFile("data/shader/blinn.frag"));
    attach_shader_source_and_file(
      GL_FRAGMENT_SHADER, sv, "data/shader/blinn.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    // uniform_location(&loc_eye, "eye");
    uniform_location(&loc_modelViewMatrix, "modelViewMatrix");
    uniform_location(
      &loc_modelViewMatrixInverseTranspose, "modelViewMatrixInverseTranspose");
    uniform_location(&loc_modelViewProjMatrix, "modelViewProjMatrix");
  }

  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
    al_normal = attrib_location("normal");
  };
};
