//#include "../rom_header/rom_time.h"
#include "rom_math.h"

using namespace std;
using namespace rom;

int main(void)  {
//auto start{rom::mashinetime()};

cout << "(2^2281 - 1) is a mersenne prime number" << endl;
auto  f{(intxx_t{2}^2281) - 1};
cout << "it is: "<< endl << f << endl;

cout << "e    is:\t" << e<floatxx_t<256>>() << endl;
cout << "pi   is:\t" << pi<floatxx_t<256>>() << endl;

cout << "e^(ln(5000.0)) is:\t" << exp(log(floatxx_t<64>{5000.0})) << endl;
cout << "acsin(sin(0.8))is:\t" << arcsin(sin(floatxx_t<64>{0.8})) << endl;
cout << "accos(cos(0.8))is:\t" << arcsin(sin(floatxx_t<64>{0.8})) << endl;

cout << "cos(20.0)      is:\t" << cos(floatxx_t<32>{20.0}) << endl;
cout << "cos(20.0)*cos(20.0) + sin(20.0)*sin(20.0)  is:\t" << endl;
cout <<
pow(cos(floatxx_t<8>{20.0}),floatxx_t<8>(2))+
pow(sin(floatxx_t<8>{20.0}),floatxx_t<8>(2)) << std::endl <<

pow(cos(floatxx_t<16>{20.0}),floatxx_t<16>(2))+
pow(sin(floatxx_t<16>{20.0}),floatxx_t<16>(2)) << std::endl <<

pow(cos(floatxx_t<32>{20.0}),floatxx_t<32>(2))+
pow(sin(floatxx_t<32>{20.0}),floatxx_t<32>(2)) << std::endl <<

pow(cos(floatxx_t<64>{20.0}),floatxx_t<64>(2))+
pow(sin(floatxx_t<64>{20.0}),floatxx_t<64>(2)) << std::endl <<

pow(cos(floatxx_t<128>{20.0}),floatxx_t<128>(2))+
pow(sin(floatxx_t<128>{20.0}),floatxx_t<128>(2)) << std::endl;

//auto time{rom::mashinetime()-start};
//cout <<"This calculation took " << time << " seconds."<< std::endl;
}
