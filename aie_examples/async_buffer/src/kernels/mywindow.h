
#ifndef __MYWINDOW_H__
#define __MYWINDOW_H__
#include <stdio.h>

#define FOR_READ  1
#define FOR_WRITE 0

// Define NULL, because we don't include stddef.h
#define NULL 0
#define __MATHENGINE__ 1
#undef DEBUG 
#undef DEBUGLOCKS 

#include <adf.h>
#include <adf/sync/mesync.h>

struct mywindow_internal
{
    window_datatype * __restrict ptr;
    window_datatype * __restrict head;
    char is_ping;
    window_datatype *          buffer_ping;
    unsigned int lockid_ping;
    window_datatype *          buffer_pong;
    unsigned int lockid_pong;
    unsigned int size;
    unsigned int winsize;
};

#undef DEF_W_INPUT_TYPES
#undef DEF_W_OUTPUT_TYPES

#define DEF_W_INPUT_TYPES(element_type)		\
  struct input_mywindow_ ## element_type {		\
    /*const*/ window_datatype * __restrict ptr;			\
    /*const*/ window_datatype * /*const*/    head;		\
    char is_ping;						\
    /*const*/ window_datatype * /*const*/    buffer_ping;	\
    unsigned int lockid_ping;					\
    /*const*/ window_datatype * /*const*/    buffer_pong;	\
    unsigned int lockid_pong;					\
    /*const*/ unsigned int  size;				\
    /*const*/ unsigned int winsize;				\
  };

#define DEF_W_OUTPUT_TYPES(element_type)    \
  struct output_mywindow_ ## element_type { \
    window_datatype * __restrict ptr;			\
    /*const*/ window_datatype * /*const*/    head;		\
    char is_ping;						\
    /*const*/ window_datatype * /*const*/    buffer_ping;	\
    unsigned int lockid_ping;					\
    /*const*/ window_datatype * /*const*/    buffer_pong;	\
    unsigned int lockid_pong;					\
    /*const*/ unsigned int           size;			\
    /*const*/ unsigned int           winsize;			\
  };

DEF_W_INPUT_TYPES(int32)
DEF_W_OUTPUT_TYPES(int32)

#undef DEF_W_INPUT_TYPES
#undef DEF_W_OUTPUT_TYPES

static inline void mywindow_init(mywindow_internal *w, 
				 int const channels, 
				 window_datatype *buffer_ping,
				 unsigned int lockid_ping, 
				 window_datatype *buffer_pong, 
				 unsigned int lockid_pong, 
				 int const size, 
				 int const winsize) {
    //assert(size % 8 == 0);
    for (int i = 0; i < channels; ++i) 
    {
        w[i].buffer_ping = buffer_ping+size*i;
	w[i].lockid_ping = lockid_ping;
        w[i].buffer_pong = buffer_pong+size*i;
	w[i].lockid_pong = lockid_pong;
	w[i].is_ping= 1;
        w[i].ptr    = w[i].buffer_ping;
        w[i].head   = w[i].buffer_ping;
        w[i].size   = size;
	w[i].winsize= winsize;
    }
} 

#undef W_CTOR

#define W_CTOR(type)                                                     \
  static inline void mywindow_init(mywindow_internal *w, int const channels, \
				   type *buffer_ping, unsigned int lockid_ping,	\
				   type *buffer_pong, unsigned int lockid_pong,	\
				   int const size, int const winsize)	\
  {									\
    int const sz = (size * sizeof(type))/sizeof(window_datatype);	\
    int const winsz =  (winsize * sizeof(type))/sizeof(window_datatype); \
    mywindow_init(w,channels, (window_datatype *)buffer_ping, lockid_ping, (window_datatype *)buffer_pong, lockid_pong, sz, winsz); \
  }									\
  static inline void mywindow_init(mywindow_internal *w, int const channels, \
				   type *buffer_ping, unsigned int lockid_ping,	\
				   type *buffer_pong, unsigned int lockid_pong,	\
				   int const size)			\
  {									\
    mywindow_init (w, channels, buffer_ping, lockid_ping, buffer_pong, lockid_pong, size, size); \
  }                                                                      

W_CTOR(int32)
W_CTOR(v4int32)

#undef W_CTOR

#undef INCR_SCALAR_OPS
#undef DECR_SCALAR_OPS

#define INCR_SCALAR_OPS(direction, type)				\
  static inline void mywindow_incr(direction ## _mywindow_ ## type * w, int count) { \
    mywindow_internal* wi = (mywindow_internal*) w;			\
    count *= sizeof(type) / sizeof(window_datatype);			\
    wi->ptr = cyclic_add(wi->ptr, count, wi->head, wi->size);		\
  }

#define DECR_SCALAR_OPS(direction, type)				\
  static inline void mywindow_decr(direction ## _mywindow_ ## type * w, int count) { \
    mywindow_incr(w, -count);						\
  } 

  INCR_SCALAR_OPS(input, int32)   // increments of 4-bytes
  DECR_SCALAR_OPS(input, int32)   // decrements of 4-bytes
  INCR_SCALAR_OPS(output, int32)   // increments of 4-bytes
  DECR_SCALAR_OPS(output, int32)   // decrements of 4-bytes

#undef INCR_SCALAR_OPS
#undef DECR_SCALAR_OPS

#undef SCALAR_OPS

#define SCALAR_OPS(scalartype)						\
  static inline void mywindow_write(output_mywindow_ ## scalartype * w, scalartype value) { \
    *((scalartype * restrict)(((mywindow_internal*) w)->ptr)) = value;	\
  }									\
  static inline void mywindow_writeincr(output_mywindow_ ## scalartype * w, scalartype value) {	\
    *((scalartype * restrict)(((mywindow_internal*) w)->ptr)) = value;	\
    mywindow_incr(w,1);							\
  }									\
  static inline scalartype mywindow_read(input_mywindow_ ## scalartype *w) { \
    return *((scalartype * restrict)(((mywindow_internal*) w)->ptr));	\
  }									\
  static inline void mywindow_read(input_mywindow_ ## scalartype *w, scalartype & value) { \
    value = *((scalartype * restrict)(((mywindow_internal*) w)->ptr));	\
  }									\
  static inline void mywindow_readincr(input_mywindow_ ## scalartype *w, scalartype & value) { \
    value = *((scalartype * restrict)(((mywindow_internal*) w)->ptr));	\
    mywindow_incr(w,1);							\
  }									\
  static inline scalartype mywindow_readincr(input_mywindow_ ## scalartype *w) { \
    scalartype  value;							\
    mywindow_readincr(w, value);					\
    return value;							\
 } 

  SCALAR_OPS(int32)  // writes 4-bytes
  
#undef SCALAR_OPS

#undef ACQREL_OPS

#define ACQREL_OPS(scalartype)						\
  static inline void mywindow_acquire(input_mywindow_ ## scalartype* w) { \
    mywindow_internal* wi = (mywindow_internal*) w;			\
    if (wi->is_ping) {							\
      acquire(wi->lockid_ping, FOR_READ);				\
      wi->head = wi->buffer_ping;					\
      wi->ptr = wi->head;						\
    } else {								\
      acquire(wi->lockid_pong, FOR_READ);				\
      wi->head = wi->buffer_pong;					\
      wi->ptr = wi->head;						\
    }									\
  }									\
  static inline void mywindow_release(input_mywindow_ ## scalartype* w) { \
    mywindow_internal* wi = (mywindow_internal*) w;			\
    if (wi->is_ping) {							\
      release(wi->lockid_ping, FOR_WRITE);				\
      wi->is_ping = 0;  /* Toggle */					\
    } else {								\
      release(wi->lockid_pong, FOR_WRITE);				\
      wi->is_ping = 1;  /* Toggle */					\
    }									\
  }									\
  static inline void mywindow_acquire(output_mywindow_ ## scalartype* w) { \
    mywindow_internal* wi = (mywindow_internal*) w;			\
    if (wi->is_ping) {							\
      acquire(wi->lockid_ping, FOR_WRITE);				\
      wi->head = wi->buffer_ping;					\
      wi->ptr = wi->head;						\
    } else {								\
      acquire(wi->lockid_pong, FOR_WRITE);				\
      wi->head = wi->buffer_pong;					\
      wi->ptr = wi->head;						\
    }									\
  }									\
  static inline void mywindow_release(output_mywindow_ ## scalartype* w) { \
    mywindow_internal* wi = (mywindow_internal*) w;			\
    if (wi->is_ping) {							\
      release(wi->lockid_ping, FOR_READ);				\
      wi->is_ping = 0;  /* Toggle */					\
    } else {								\
      release(wi->lockid_pong, FOR_READ);				\
      wi->is_ping = 1;  /* Toggle */					\
    }									\
  }

ACQREL_OPS(int32)

#undef ACQREL_OPS

#endif
