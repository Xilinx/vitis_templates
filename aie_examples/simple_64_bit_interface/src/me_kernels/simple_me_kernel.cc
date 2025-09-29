#include <adf.h>
void simple_me_kernel(input_stream_int32 *input, 
              output_stream_int32 *output) 
{ 
  for (int32_t i = 0 ; i < 512; i++) 
  {
    //int32_t value = readincr(input);
    
    int32_t data=readincr(input);
    printf("data = %d\n",data);
    if(i == 511)
    {
       printf("in if condition i == %d, \n", i);
       writeincr(output, data, true);
    }
    else
    {
       printf("in else condition i == %d, \n", i);
       writeincr(output, data);
    }
  }
}
