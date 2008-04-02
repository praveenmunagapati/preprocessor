#ifndef __Value_hpp__
#define __Value_hpp__

#include "Types.hpp"
#include "NDimArray.hpp"
#include "NumericArray.hpp"
class StructArray;
class CellArray;
class StringArray;

class Value {
public:
  enum Type {
    Invalid = 0,
    Cell = 1,
    Struct = 2,
    String = 3,
    Bool = 4,
    Int8 = 5,
    UInt8 = 6,
    Int16 = 7,
    UInt16 = 8,
    Int32 = 9,
    UInt32 = 10,
    Int64 = 11,
    UInt64 = 12,
    Float = 13,
    Double = 14,
    CellArray = 15,
    StructArray = 16,
    StringArray = 17,
    BoolArray = 18,
    Int8Array = 19,
    UInt8Array = 20,
    Int16Array = 21,
    UInt16Array = 22,
    Int32Array = 23,
    UInt32Array = 24,
    Int64Array = 25,
    UInt64Array = 26,
    FloatArray = 27,
    DoubleArray = 28,
  };
  inline Value() : 
    m_real_u64(0), m_type(Invalid) {}
  inline Value(CellArray *p) : 
    m_real_p(p), m_type(CellArray) {}
  inline Value(StructArray *p) : 
    m_real_p(p), m_type(StructArray) {}
  inline Value(int8 real, int8 imag = 0) : 
    m_real_i8(real), m_imag_i8(imag), m_type(Int8) {}
  inline Value(uint8 real, uint8 imag = 0) :
    m_real_u8(real), m_imag_u8(imag), m_type(UInt8) {}
  inline Value(int16 real, int16 imag = 0) : 
    m_real_i16(real), m_imag_i16(imag), m_type(Int16) {}
  inline Value(uint16 real, uint16 imag = 0) :
    m_real_u16(real), m_imag_u16(imag), m_type(UInt16) {}
  inline Value(int32 real, int32 imag = 0) : 
    m_real_i32(real), m_imag_i32(imag), m_type(Int32) {}
  inline Value(uint32 real, uint32 imag = 0) :
    m_real_u32(real), m_imag_u32(imag), m_type(UInt32) {}
  inline Value(int64 real, int64 imag = 0) : 
    m_real_i64(real), m_imag_i64(imag), m_type(Int64) {}
  inline Value(uint64 real, uint64 imag = 0) :
    m_real_u64(real), m_imag_u64(imag), m_type(UInt64) {}
  inline Value(float real, float imag = 0) :
    m_real_f(real), m_imag_f(imag), m_type(Float) {}
  inline Value(double real, double imag = 0) :
    m_real_d(real), m_imag_d(imag), m_type(Double) {}
  inline Value(BasicArray<logical> *pr, BasicArray<logical> *pi = 0) :
    m_real_p(pr), m_imag_p(pi), m_type(BoolArray) {}
  inline Value(BasicArray<int8> *pr, BasicArray<int8> *pi = 0) :
    m_real_p(pr), m_imag_p(pi), m_type(Int8Array) {}
  inline Value(BasicArray<uint8> *pr, BasicArray<uint8> *pi = 0) :
    m_real_p(pr), m_imag_p(pi), m_type(UInt8Array) {}


  inline Value(NumericArray<logical> *p) : m_ptr(p), m_type(BoolArray) {}
  inline Value(NumericArray<int8> *p) : m_ptr(p), m_type(Int8Array) {}
  inline Value(NumericArray<uint8> *p) : m_ptr(p), m_type(UInt8Array) {}
  inline Value(NumericArray<int16> *p) : m_ptr(p), m_type(Int16Array) {}
  inline Value(NumericArray<uint16> *p) : m_ptr(p), m_type(UInt16Array) {}
  inline Value(NumericArray<int32> *p) : m_ptr(p), m_type(Int32Array) {}
  inline Value(NumericArray<uint32> *p) : m_ptr(p), m_type(UInt32Array) {}
  inline Value(NumericArray<int64> *p) : m_ptr(p), m_type(Int64Array) {}
  inline Value(NumericArray<uint64> *p) : m_ptr(p), m_type(UInt64Array) {}
  inline Value(NumericArray<float> *p) : m_ptr(p), m_type(FloatArray) {}
  inline Value(NumericArray<double> *p) : m_ptr(p), m_type(DoubleArray) {}
  //
  inline const CellArray* asConstCellArray() const {
    if (m_type != Cell) throw Exception("Not a cell array");
    return reinterpret_cast<const CellArray*>(m_ptr);
  }
  inline CellArray* asCellArray() {
    if (m_type != Cell) throw Exception("Not a cell array");
    return reinterpret_cast<CellArray*>(m_ptr);
  }
  inline const StructArray* asConstStructArray() const {
    if (m_type != Struct) throw Exception("Not a struct array");
    return reinterpret_cast<const StructArray*>(m_ptr);
  }
  inline StructArray* asStructArray() {
    if (m_type != Struct) throw Exception("Not a struct array");
    return reinterpret_cast<StructArray*>(m_ptr);
  }
  inline const NumericArray<logical>* asConstLogicalArray() const {
    if (m_type != BoolArray) throw Exception("Not a bool array");
    return reinterpret_cast<const NumericArray<logical>*>(m_ptr);
  }
  inline NumericArray<logical>* asLogicalArray() {
    if (m_type != BoolArray) throw Exception("Not a bool array");
    return reinterpret_cast<NumericArray<logical>*>(m_ptr);
  }
  inline const NumericArray<double>* asConstDoubleArray() const {
    if (m_type != DoubleArray) throw Exception("Not a bool array");
    return reinterpret_cast<const NumericArray<double>*>(m_ptr);
  }
  inline NumericArray<double>* asDoubleArray() {
    if (m_type != DoubleArray) throw Exception("Not a bool array");
    return reinterpret_cast<NumericArray<double>*>(m_ptr);
  }
  Value(const Value &ref);
  inline const Type type() const {
    return m_type;
  }
private:
  union {
    void *m_real_p;
    int8 m_real_i8;
    uint8 m_real_u8;
    int16 m_real_i16;
    uint16 m_real_u16;
    int32 m_real_i32;
    uint32 m_real_u32;
    int64 m_real_i64;
    uint64 m_real_u64;
    float m_real_f;
    double m_real_d;
  };
  union {
    void *m_imag_p;
    int8 m_imag_i8;
    uint8 m_imag_u8;
    int16 m_imag_i16;
    uint16 m_imag_u16;
    int32 m_imag_i32;
    uint32 m_imag_u32;
    int64 m_imag_i64;
    uint64 m_imag_u64;
    float m_imag_f;
    double m_imag_d;
  };
  Type m_type;
};

#endif