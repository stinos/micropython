#include "../../micropython-wrap/classwrapper.h"
#include "../../micropython-wrap/functionwrapper.h"
#include <iostream>

class SomeClass
{
public:
  SomeClass()
  {
  }

  std::string Concat( std::string a, int n )
  {
    return a + std::to_string( n );
  }
};

int SomeFunc()
{
  return 35;
}

struct Funcs
{
  func_name_def( Concat )
  func_name_def( SomeFunc )
};

extern "C"
{
  void RegisterMyModule()
  {
    auto mod = upywrap::CreateModule( "mod" );

    upywrap::ClassWrapper< SomeClass > wrapclass( "SomeClass", mod );
    wrapclass.DefInit<>();
    wrapclass.Def< Funcs::Concat >( &SomeClass::Concat );

    upywrap::FunctionWrapper wrapfunc( mod );
    wrapfunc.Def< Funcs::SomeFunc >( SomeFunc );
  }
}

extern "C" int cmain( int, char** );

int main(int argc, char **argv) {
    return cmain( argc, argv );
}
