syn match glEnum /\v\CGL_\w+/
syn match glmVec /\v\C<vec\d>/
syn match glmMat /\v\C<mat\d>/
syn match glmNamespace /\v\C<glm>/
syn match glfwMacro /\v\C<GLFW_\w+/
syn match glfwType /\v\C<GLFW\l+/

syn keyword qtMacro SIGNAL SLOT Q_OBJECT signals slots emit
syn match qtqMacro /\vQ_[A_Z_]+/
syn keyword qtPrimitive qint8 quint8 qint16 quint16 qint32 quint32 qint64 quint64 qlonglong qulonglong
syn match qtClass /\v<Q\u\w*>/

highlight link glEnum Constant
highlight link glmVec Type
highlight link glmMat Type
highlight link glmNamespace Constant
highlight link glfwMacro Constant
highlight link glfwType Type

highlight link qtMacro Macro
highlight link qtqMacro Macro
highlight link qtClass Type
highlight link qtPrimitive Type
