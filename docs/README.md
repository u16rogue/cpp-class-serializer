# cpp-class-serializer
A C++ header only library for serializing and deserializing C++ classes into a binary format with metadata that's mostly resilient to class changes.

```c++
#include "../include/cppcsr.hpp"

class SampleClass
{
  cppcsr_member(bool, somebool);
  cppcsr_member(float, somefloat);
  cppcsr_member(int, someint);
};

int main()
{
  SampleClass sample;
  
  // Binary array de/serialization - Allows you to create a binary array representation of the class, utilizing this is upto you.
  
  cppcsr_declare_std_array(SampleClass) sample_buffer = {}; // std::array to hold serialized class
  // Serialize class to a binary array
  cppcsr::serialize<SampleClass>(sample, sample_buffer);
  // Deserialize binary array to a class
  cppcsr::deserialize<SampleClass>(sample_buffer.data(), sample);
  
  // Binary file de/serialization - Allows you to save a class into a binary file and back (binary file to a class).
  // (file extension doesn't matter)
  // Serialize class to a binary file
  cppcsr::serialize<SampleClass>(sample, L"sample.cfg");
  // Deserialize binary file into a class
  cppcsr::deserialize<SampleClass>(L"sample.cfg", sample);
}
```

# Usage
* Include [cppcsr.hpp](../include/cppcsr.hpp)
* Declare member variables that you want to have a serializable metadata using the `cppcsr_member` macro.
* Use the `cppcsr_declare_std_array` to automaticlly create an **std::array** buffer to hold the serialized class

# Example
* [main.cpp](../example/main.cpp)

# Note / Disclaimer
* Only supports raw data types (No string pointers, containers, etc...)
* This was mainly made as a Proof of concept, so it might not be as good as a "reliable" library. If you're looking for one check out [cereal](https://uscilab.github.io/cereal/).
