syn match glEnum /\v\CGL_\w+/
syn match glmVec /\v\C<vec\d>/
syn match glmMat /\v\C<mat\d>/
syn match glmNamespace /\v\C<glm>/
syn match glfwMacro /\v\C<GLFW_\w+/
syn match glfwType /\v\C<GLFW\l+/

syn keyword qtMacro SIGNAL SLOT Q_OBJECT

highlight link glEnum Constant
highlight link glmVec Type
highlight link glmMat Type
highlight link glmNamespace Constant
highlight link glfwMacro Constant
highlight link glfwType Type

highlight link qtMacro Macro
