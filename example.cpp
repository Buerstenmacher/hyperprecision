//#include "../rom_header/rom_time.h"
#include "rom_math.h"

using namespace std;
using namespace rom;

int main(void)  {
//auto start{rom::mashinetime()};

cout << "(2^2281 - 1) is a mersenne prime number" << endl;
auto  f{(intxx_t{2}^2281) - 1};
cout << "it is: "<< endl << f << endl;

cout << "e    is:\t" << e<floatxx_t<128>>() << endl;
cout << "pi   is:\t" << pi<floatxx_t<128>>() << endl;

auto mil{exp(floatxx_t<128>(1000000))};

cout << "ln(e^(1.0)) is:\t" << log(exp(floatxx_t<128>{1.0})) << endl;
cout << "ln(e^(10.0)) is:\t" << log(exp(floatxx_t<128>{10.0})) << endl;
cout << "ln(e^(100.0)) is:\t" << log(exp(floatxx_t<128>{100.0})) << endl;
cout << "ln(e^(1000.0)) is:\t" << log(exp(floatxx_t<128>{1000.0})) << endl;
cout << "ln(e^(10000.0)) is:\t" << log(exp(floatxx_t<128>{10000.0})) << endl;
cout << "ln(e^(100000.0)) is:\t" << log(exp(floatxx_t<128>{100000.0})) << endl;
//cout << "ln(e^(1000001.0)) is:\t" << log(exp(floatxx_t<128>{1000001.0})) << endl;

cout << "e^(0.001) is:\t" << exp(floatxx_t<128>{0.001}) << endl;
cout << "e^(0.01) is:\t" << exp(floatxx_t<128>{0.01}) << endl;
cout << "e^(0.1) is:\t" << exp(floatxx_t<128>{0.1}) << endl;
cout << "e^(1) is:\t" << exp(floatxx_t<128>{1}) << endl;
cout << "e^(10) is:\t" << exp(floatxx_t<128>{10}) << endl;
cout << "e^(100) is:\t" << exp(floatxx_t<128>{100}) << endl;
cout << "e^(1000) is:\t" << exp(floatxx_t<128>{1000}) << endl;
cout << "e^(10000) is:\t" << exp(floatxx_t<128>{10000}) << endl;
//cout << "e^(100000) is:\t" << exp(floatxx_t<128>{100000}) << endl;
//cout << "e^(1000000) is:\t" << exp(floatxx_t<128>{1000000}) << endl;



cout << "arcsin(sin(0.8))is:\t" << arcsin(sin(floatxx_t<128>{8}/floatxx_t<128>{10})) << endl;
cout << "arccos(cos(0.8))is:\t" << arcsin(sin(floatxx_t<128>{8}/floatxx_t<128>{10})) << endl;

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

cout << "+10^3= \t" << pow(floatxx_t<128>(+10),floatxx_t<128>(3)) << endl;
cout << "-10^3= \t" << pow(floatxx_t<128>(-10),floatxx_t<128>(3)) << endl;
cout << "-10^4= \t" << pow(floatxx_t<128>(-10),floatxx_t<128>(4)) << endl;
cout << "+10^4= \t" << pow(floatxx_t<128>(+10),floatxx_t<128>(4)) << endl;
cout << "+10^-4= \t" << pow(floatxx_t<128>(-10),floatxx_t<128>(-4)) << endl;
cout << "+10^-4= \t" << pow(floatxx_t<128>(+10),floatxx_t<128>(-4)) << endl;


//auto time{rom::mashinetime()-start};
//cout <<"This calculation took " << time << " seconds."<< std::endl;
}
