#include "rom_int.h"
#include "rom_float.h"
#include "rom_math.h"

using namespace std;
using namespace rom;

int main(void)  {
/*
cout << endl;
floatxx_t<32> b{factorial<rom::floatxx_t<32>>(10001) / factorial<rom::floatxx_t<32>>(10000) };
cout << "10001! / 10000! = \t"<< b << endl;
cout << endl;

floatxx_t<32> s{7};
cout <<"           7: "<< s << endl;
cout <<"plus minus 7: "<<(s=test_plus_minus(s,1,5000)) << endl;
cout <<"mul  div   7: "<<(s=test_mul_div(s,1,5000)) << endl;
cout << endl;

cout << "2^2281 - 1 is a mersenne prime number" << endl;
auto  f{intxx_t{2}^2281};  f--;
cout << "it is: "<< endl << f << endl;
cout << std::endl;


*/
cout << "e^1       is:\t" << exp(floatxx_t<64>{1.0}) << endl;
cout << "ln(1500.0)is:\t" << log(floatxx_t<64>{1500.0}) << endl;
cout << "3^3.1     is:\t" << pow(floatxx_t<64>{3.0},floatxx_t<64>{3.1}) << endl;
cout << "sin(20.0)  is:\t" << sin(floatxx_t<128>{20.0}) << endl;
cout << "cos(20.0)  is:\t" << cos(floatxx_t<128>{20.0}) << endl;
cout << "cos(20.0)*cos(20.0) + sin(20.0)*sin(20.0)  is:\t" << endl;
cout <<
cos(floatxx_t<128>{20.0})*
cos(floatxx_t<128>{20.0})+
sin(floatxx_t<128>{20.0})*
sin(floatxx_t<128>{20.0}) << std::endl;



cout << std::endl;


}
