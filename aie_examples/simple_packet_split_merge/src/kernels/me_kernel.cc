#include <stdlib.h>
#include <stdio.h>
#include <adf.h>

void me_kernel(input_stream_int32 * in, output_pktstream * out) {
	printf("start of me_kernel \n");
	int32 i;
  int32 output_arr[8];
  int32 pktId = getPacketid(out, 0);

//  printf(" me_kernel pktId=%d,   pktHeader=%d \n", pktId, pktHeader);
  for (i=0;i<8;i++) {
    int32 val = readincr(in);
    int32 result = val * val;
    output_arr[i] = result;
    printf(" me_kernel for loop i=%d,   result=%d \n", i, result);
  }
  writeHeader(out, 0, pktId);
  for (i=0;i<8;i++) {
	printf(" me_kernel for loop write data i=%d,    output_arr[i]=%d \n", i,  output_arr[i]);
    writeincr(out, output_arr[i], i==7);
  }
};

