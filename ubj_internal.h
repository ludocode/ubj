#ifndef UBJ_INTERNAL_H
#endif UBJ_INTERNAL_H

#include "ubj.h"
#include <stdlib.h>
#include <string.h>

#if _MSC_VER
#define inline __inline
#endif


static const uint8_t UBJI_TYPEC_convert[UBJ_NUM_TYPES] = "\x00ZNTFCSHiUIlLdD[{";

static const int UBJI_TYPE_size[UBJ_NUM_TYPES] =
	{ -1,	 //MIXED
	0,	 //NULLTYPE
	0,	 //NOOP
	0,   //BOOL_TRUE
	0,   //BOOL_FALSE
	1,   //CHAR
	sizeof(const char*), //STRING
	sizeof(const char*), //high-precision
	1,					//INT8
	1,					//UINT8
	2,					//int16
	4,					//int32
	8,					//int64
	4,					//float32
	8,					//float64
	-1,					//array
	-1					//object
	};

static const size_t UBJR_TYPE_localsize[UBJ_NUM_TYPES] =
{
	sizeof(ubjr_dynamic_t),	 //MIXED
	0,	 //NULLTYPE
	0,	 //NOOP
	0,   //BOOL_TRUE
	0,   //BOOL_FALSE
	sizeof(ubjr_string_t),   //CHAR
	sizeof(ubjr_string_t), //STRING
	sizeof(ubjr_string_t), //high-precision
	sizeof(int8_t),					//INT8
	sizeof(uint8_t),					//UINT8
	sizeof(int16_t),					//int16
	sizeof(int32_t),					//int32
	sizeof(int64_t),					//int64
	sizeof(float),					//float32
	sizeof(double),					//float64
	sizeof(ubjr_array_t),					//array
	sizeof(ubjr_object_t)					//object
};

static inline void _to_bigendian16(uint8_t* outbuffer, uint16_t input)
{
	*outbuffer++ = (input >> 8); // Get top order byte (guaranteed endian-independent since machine registers)
	*outbuffer++ = input & 0xFF; // Get bottom order byte
}
static inline void _to_bigendian32(uint8_t* outbuffer, uint32_t input)
{
	_to_bigendian16(outbuffer, (uint16_t)(input >> 16)); // Get top order 2 bytes
	_to_bigendian16(outbuffer + 2, (uint16_t)(input & 0xFFFF)); // Get bottom order 2 bytes
}
static inline void _to_bigendian64(uint8_t* outbuffer, uint64_t input)
{
	_to_bigendian32(outbuffer, (uint32_t)(input >> 32));
	_to_bigendian32(outbuffer + 4, (uint32_t)(input & 0xFFFFFFFF));
}

static inline uint8_t _is_bigendian()
{
	int i = 1;
	char *low = (char*)&i;
	return *low ? 0 : 1;
}

#define BUF_BIG_ENDIAN_SWAP(type,func,ptr,num)  \
	{											\
		size_t i;type* d = (type*)ptr; 					\
		for (i = 0; i < num; i++)				\
		{										\
			func((uint8_t*)&d[i], d[i]);		\
		}										\
	}											\

static inline void buf_endian_swap(uint8_t* buf, size_t sz, size_t n)
{
	if (!_is_bigendian())
	{
		switch (sz)
		{
		case 1:
		case 0:
			break;
		case 2:
			BUF_BIG_ENDIAN_SWAP(uint16_t, _to_bigendian16,buf,n);
			break;
		case 4:
			BUF_BIG_ENDIAN_SWAP(uint32_t, _to_bigendian32,buf,n);
			break;
		case 8:
			BUF_BIG_ENDIAN_SWAP(uint64_t, _to_bigendian64,buf,n);
			break;
		};
	}
}