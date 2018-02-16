syn match glEnum /\v\CGL_\w+/
syn match glmVec /\v\C<vec\d>/
syn match glmMat /\v\C<mat\d>/
syn match glmNamespace /\v\C<glm>/

highlight link glEnum Constant
highlight link glmVec Type
highlight link glmMat Type
highlight link glmNamespace Constant
