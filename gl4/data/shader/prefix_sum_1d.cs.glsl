#version 430 core

layout (local_size_x = 1024) in;
layout (binding = 0) coherent buffer block1
{
  float input_data[gl_WorkGroupSize.x];
};

layout (binding = 1) coherent buffer block2
{
  float output_data[gl_WorkGroupSize.x];
};

shared float shared_data[gl_WorkGroupSize.x * 2];

void main(void)
{
  uint id = gl_LocalInvocationID.x;
  uint rd_id;
  uint wr_id;
  uint mask;

  // the number of steps is the log base 2 of the work group size, which should
  // be a pow of 2;
  const uint steps = uint(log2(gl_WorkGroupSize.x)) + 1; // why + 1?
  uint step = 0;

  // each invocation is responsible for the content of two elements of the
  // output array
  shared_data[id * 2] = input_data[id * 2];
  shared_data[id * 2 + 1] = input_data[id * 2 + 1];

  // synchorinize to make sure that everyone has initialize their elements of
  // shared_data][ with data loaded from the input arrays
  barrier();
  memoryBarrierShared();

  // for each step...
  for (step = 0; step < steps; step++) 
  {
    // calculate the read and write index in the shared array
    mask = (1 << step) - 1;
    rd_id = ((id >> step) << (step + 1)) + mask;
    wr_id =  rd_id + 1 + (id & mask);

    // accumulate the read data into out element
    shared_data[wr_id] += shared_data[rd_id];

    // synchronize again to make sure that every one has caught up with us
    barrier();
    memoryBarrierShared();
  }

  // finally write our data back to the output image
  output_data[id * 2] = shared_data[id * 2];
  output_data[id * 2 + 1] = shared_data[id * 2 + 1];
}
