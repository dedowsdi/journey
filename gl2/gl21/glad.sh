#!/bin/bash -x
glad --profile compatibility --api gl=2.1  --out-path=. --generator c --extensions=\
GL_ARB_vertex_array_object,\
GL_ARB_framebuffer_object,\
GL_ARB_texture_stencil8,\
GL_ARB_texture_float,\
GL_ARB_geometry_shader4,\
GL_ARB_debug_output,\
GL_ARB_debug_output,\
GL_KHR_debug,\
GL_ARB_draw_instanced,\
GL_ARB_instanced_arrays,\
GL_EXT_packed_depth_stencil,\
GL_EXT_gpu_shader4,\
GL_ARB_imaging,\
GL_ARB_ES3_compatibility,\

glad --api glx=1.4 --spec=glx --out-path=. --generator c --extensions=\
GLX_EXT_swap_control

glad --api wgl=1.0 --spec=wgl --out-path=. --generator c --extensions=\
WGL_EXT_swap_control
