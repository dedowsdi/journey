#include "line.h"

namespace zxd
{

//--------------------------------------------------------------------
void line::create_primitive_set()
{
  m_primitive_sets.clear();
  add_primitive_set(new draw_arrays(m_mode, 0, attrib_array(0)->num_elements()));
  bind_and_update_buffer();
}

}
