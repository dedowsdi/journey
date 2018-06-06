/*
 * gen_linear_hierachy generate lineay inheritance
 */
#include "type_list.h"

namespace mp
{

template<typename TList, template<typename atomicType, typename Base> class Unit, typename Root = empty_type>
struct gen_linear_hierachy;

// recurse
template<typename H, typename T, template<typename atomicType, typename Base> class Unit, typename Root>
struct gen_linear_hierachy< type_list<H,T>, Unit, Root>:
  public Unit<H, gen_linear_hierachy< T, Unit, Root> >
{
};

// ender of recursion
template<typename T, template<typename atomicType, typename Base> class Unit, typename Root>
struct gen_linear_hierachy< type_list<T,null_type>, Unit, Root>:
  public Unit<T, Root>
{
};

}
