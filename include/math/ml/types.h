#pragma once
#include <stdint.h>

// typedef int8_t I8;
// typedef int16_t I16;
// typedef int32_t I32;
// typedef int64_t I64;
// typedef uint8_t U8;
// typedef uint16_t U16;
// typedef uint32_t U32;
// typedef uint64_t U64;
// typedef float F32;
// typedef double F64;
//
enum Datatype {
  I8,
  I16,
  I32,
  I64,

  U8,
  U16,
  U32,
  U64,

  F32,
  F64
};

// using enum Datatype;
const Datatype promotion_table[10][10] = {
    {I8, I16, I32, I64, I16, I32, I64, F64, F32, F64},
    {I16, I16, I32, I64, I16, I32, I64, F64, F32, F64},
    {I32, I32, I32, I64, I32, I32, I64, F64, F32, F64},
    {I64, I64, I64, I64, I64, I64, I64, F64, F64, F64},
    {I16, I16, I32, I64, U8, U16, U32, U64, F32, F64},
    {I32, I32, I32, I64, U16, U16, U32, U64, F32, F64},
    {I64, I64, I64, I64, U32, U32, U32, U64, F32, F64},
    {F64, F64, F64, F64, U64, U64, U64, U64, F64, F64},
    {F32, F32, F32, F32, F32, F32, F32, F32, F32, F64},
    {F64, F64, F64, F64, F64, F64, F64, F64, F64, F64}};
