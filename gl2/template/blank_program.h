struct prg : public program{
  GLint al_vertex;

  virtual void attach_shaders(){

  };

  virtual void bind_uniform_locations(){
  
  };

  virtual void bind_attrib_locations(){
    al_vertex = attrib_location("vertex");
  };
};
