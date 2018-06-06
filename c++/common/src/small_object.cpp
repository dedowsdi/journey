#include "small_object.h"
#include <cassert>

namespace som
{

//--------------------------------------------------------------------
void fixed_allocator::chunk::init(std::size_t block_size, uchar blocks)
{
  data = static_cast<uchar*>(malloc(block_size * blocks));
  first_available_block = 0;
  blocks_available = blocks;

  // create link between unused blocks.
  uchar* p = data;
  for (uchar i = 0; i != blocks_available; p+= block_size ) {
    *p = ++i;
  }
}

//--------------------------------------------------------------------
void* fixed_allocator::chunk::allocate(std::size_t block_size)
{
  if(!blocks_available)
    return 0;

  uchar* res = data + block_size * first_available_block;
  // shift to next available block
  first_available_block = *res;
  --blocks_available;

  return res;
}

//--------------------------------------------------------------------
void fixed_allocator::chunk::deallocate(void* p, std::size_t block_size)
{
  assert(p >= data);
  uchar* up = static_cast<uchar*>(p);
  // alignment check
  assert( (up - data)%block_size == 0);

  // reset fist available block, create link
  *up = first_available_block;
  first_available_block = static_cast<uchar>((up - data) / block_size);

  // truncation check
  assert(first_available_block == (up - data) / block_size);

  ++blocks_available;
}

//--------------------------------------------------------------------
void fixed_allocator::chunk::release()
{
  delete[] data;
}

//--------------------------------------------------------------------
void fixed_allocator::do_deallocate(void* p)
{
  assert(m_dealloc_chunk->data <= p);
  assert(m_dealloc_chunk->data + m_num_blocks * m_block_size > p);

  // a chunk is freed only when there are two empty chunks, so as to avoid
  // frequent create and release chunk in some loop.

  m_dealloc_chunk->deallocate(p, m_block_size);

  if (m_dealloc_chunk->blocks_available == m_num_blocks) {
    // dealloc chunk is completely free, should we release it ?
    chunk& last_chunk = m_chunks.back();

    if (&last_chunk == m_dealloc_chunk) {
      // check if we have two last chunks empty
      if (m_chunks.size() > 1 && m_dealloc_chunk[-1].blocks_available == m_num_blocks) {
        // two free chunks, discard the last one
        last_chunk.release();
        m_chunks.pop_back();
        m_alloc_chunk = m_dealloc_chunk = &m_chunks.front(); // is front a random choice?
      }

      return;
    }

    if (last_chunk.blocks_available == m_num_blocks) {
        // two free chunks, discard the last one
        last_chunk.release();
        m_chunks.pop_back();
        m_alloc_chunk = m_dealloc_chunk;
    }else{
      // move the empty chunk to the end
      std::swap(*m_dealloc_chunk, last_chunk);
      m_dealloc_chunk = &m_chunks.back();
    }
    
  }
}

//--------------------------------------------------------------------
fixed_allocator::chunk* fixed_allocator::vicinity_find(void* p)
{
  assert(!m_chunks.empty());
  assert(m_dealloc_chunk);

  const std::size_t chunk_length = m_num_blocks * m_block_size;
  chunk* iter0 = m_dealloc_chunk;
  chunk* iter1 = m_dealloc_chunk + 1;
  chunk* chunks_start = &m_chunks.front();
  chunk* chunks_end = &m_chunks.back() + 1; // why not just use .end ?

  // special case: m_dealloc_chunk is the last in the array
  if (iter1 == chunks_end)
    iter1 = 0;

  while(true){

    // search from iter0 to chunks_start
    if (iter0) {
      if (p >= iter0->data && p < iter0->data + chunk_length) {
        return iter0;
      }

      if(iter0 == chunks_start)
        iter0 = 0;
      else
        --iter0;
    }

    // search from iter1 to chunks_end
    if(iter1){
      if(p >= iter1->data && p < iter1->data + chunk_length){
        return iter1;
      }

      if(++iter1 == chunks_end) 
        iter1 = 0;
    }
  }

  assert(false);
  return 0;
}

//--------------------------------------------------------------------
void* fixed_allocator::allocate()
{
  if(m_alloc_chunk == 0 || m_alloc_chunk->blocks_available == 0){

    for(auto iter = m_chunks.begin(); ; ++iter){
      // create new chunk if all chunks are full filled
      if (iter == m_chunks.end()) {
        m_chunks.push_back(chunk());
        m_alloc_chunk = &m_chunks.back();
        m_alloc_chunk->init(m_block_size, m_num_blocks);
        m_dealloc_chunk = m_alloc_chunk;
        break;
      }

      if (iter->blocks_available > 0) {
        m_alloc_chunk = &*iter;
        break;
      }
      
    }
  }

  assert(m_alloc_chunk != 0);
  assert(m_alloc_chunk->blocks_available > 0);
  return m_alloc_chunk->allocate(m_block_size);
}

//--------------------------------------------------------------------
void fixed_allocator::dellocate(void *p)
{
  assert(!m_chunks.empty());
  assert(&m_chunks.front() <= m_dealloc_chunk);
  assert(&m_chunks.back() >= m_dealloc_chunk);

  assert(m_dealloc_chunk);
}

}
