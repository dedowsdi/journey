#include "line.h"

namespace zxd
{

//--------------------------------------------------------------------
void line::create_primitive_set()
{
  clear_primitive_sets();
  add_primitive_set(std::make_shared<draw_arrays>(m_mode, 0, num_vertices()));
}

}
