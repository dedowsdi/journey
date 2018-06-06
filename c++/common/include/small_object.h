// small object memory
#include <iostream>
#include <vector>
namespace som{

typedef unsigned char uchar;

/*
 * allocates only objects of one given size. 
 */
class fixed_allocator{

private:

  /*
   * allocates objects of one given size. has a fixed upper limit(at most 256)
   * of unumber of objects allocated.
   *
   * we can store whatever we want in an unused block, so we use 1st byte of an
   * unused block to store index of the next unused block, the sequence doesn't
   * matter.
   *
   * we use uchar to store blocks, 
   */
  struct chunk{

    uchar* data;
    uchar first_available_block;
    uchar blocks_available;

    void init(std::size_t block_size, uchar blocks);

    /*
     * you have to pass a size to allocate and dellocate because chunk does not
     * hold it. This is because the block size is known at a superior level.
     */
    void* allocate(std::size_t block_size);

    void deallocate(void* p, std::size_t block_size);

    void release();
  };
  typedef std::vector<chunk> chunks;

  std::size_t m_block_size;
  uchar m_num_blocks;
  chunks m_chunks;
  chunk* m_alloc_chunk; // last chunk that was used for an allocation
  chunk* m_dealloc_chunk; // last chunk that was used for a deallocation

private:
  void do_deallocate(void* p);
  chunk* vicinity_find(void* p);

public:
  
  void* allocate();

  void dellocate(void *p);
};

}
