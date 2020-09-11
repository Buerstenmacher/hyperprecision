//#include "../rom_header/rom_time.h"

#include "rom_lns.h"
#include "rom_math.h"

using namespace std;
using namespace rom;

int main(void)  {
//auto start{rom::mashinetime()};

std::cout << "lns    \t (5 / 50)+0.9 = " ;
std::cout <<  lns<>(5)/lns<>(50) +lns<>(0.9) <<" " << std::endl;

auto tm{exp(floatxx_t<64>{0.001})};
cout << "e^(0.001) is:\t" << tm << endl;

tm = {exp(floatxx_t<64>{0.01})};
cout << "e^(0.01) is:\t" << tm << endl;

tm = {exp(floatxx_t<64>{0.1})};
cout << "e^(0.1) is:\t" << tm << endl;

tm = {exp(floatxx_t<64>{1})};
cout << "e^(1) is: \t" << tm << endl;

tm = {exp(floatxx_t<64>{10})};
cout << "e^(10) is: \t" << tm << endl;

tm = {exp(floatxx_t<64>{100})};
cout << "e^(100) is: \t" << tm << endl;

tm = {exp(floatxx_t<64>{1000})};
cout << "e^(1000) is: \t" << tm << endl;

tm = {exp(floatxx_t<64>{10000})};
cout << "e^(10000) is: \t" << tm << endl;

tm = {exp(floatxx_t<64>{100000})};
cout << "e^(100000) is: \t" << tm << endl;

tm = {exp(floatxx_t<64>{1000000})};
cout << "e^(1000000) is: \t" << tm << endl;

tm = {exp(floatxx_t<64>{10000000})};
cout << "e^(10000000) is:\t" << tm << endl;

tm = {exp(floatxx_t<64>{100000000})};
cout << "e^(100000000) is:\t" << tm << endl;

auto number = rom::lns<>{5};
cout <<"The lns<> number that is nearest to 5 is: \t"<< number.value() <<  endl;
number = rom::lns<>{-5};
cout <<"The lns<> number that is nearest to -5 is: \t"<< number.value() <<  endl;
cout <<"The lns<> number that is nearest to 0 is: \t"<< rom::lns<>(0) <<  endl;
cout <<"The smallest lns<> number is: \t"<< number._min() <<  endl;
cout <<"The largest  lns<> number is: \t"<< number._max() <<  endl;


cout << "(2^3217- 1) is a mersenne prime number" << endl;
//auto  f{(intxx_t{2}^2281) - 1};
//auto  f{(intxx_t{2}^9941) - 1};
auto  f{(intxx_t{2}^3217) - 1};
rom::lns<14> l{f};
cout << "it is: "<< endl << f << endl;
cout << "it is: "<< endl << l << endl;

cout << "e    is:\t" << e<floatxx_t<256>>() << endl;
cout << "pi   is:\t" << pi<floatxx_t<256>>() << endl;

cout << "ln(e^(1.0)) is:\t" << log(exp(floatxx_t<64>{1.0})) << endl;
cout << "ln(e^(10.0)) is:\t" << log(exp(floatxx_t<64>{10.0})) << endl;
cout << "ln(e^(100.0)) is:\t" << log(exp(floatxx_t<64>{100.0})) << endl;
cout << "ln(e^(1000.0)) is:\t" << log(exp(floatxx_t<64>{1000.0})) << endl;
cout << "ln(e^(10000.0)) is:\t" << log(exp(floatxx_t<64>{10000.0})) << endl;
cout << "ln(e^(100000.0)) is:\t" << log(exp(floatxx_t<64>{100000.0})) << endl;
cout << "ln(e^(1000000.0)) is:\t" << log(exp(floatxx_t<64>{1000000.0})) << endl;

cout << "ln(ln(ln(e^(e^(e^(4.0)))))) is:\t" <<
log(log(log(
exp(exp(exp(floatxx_t<64>{4.0})))))) <<"\t Try this calculation on any other calculator!!!" << endl;

cout << "arcsin(sin(0.8))is:\t" << arcsin(sin(floatxx_t<128>{8}/floatxx_t<128>{10})) << endl;
cout << "arccos(cos(0.8))is:\t" << arcsin(sin(floatxx_t<128>{8}/floatxx_t<128>{10})) << endl;

cout << "cos(20.0)      is:\t" << cos(floatxx_t<128>{20.0}) << endl;

cout << "cos(20)*cos(20) + sin(20)*sin(20)  is:\t" << endl;

cout <<
pow(cos(floatxx_t<8>{20.0}),floatxx_t<8>(2))+
pow(sin(floatxx_t<8>{20.0}),floatxx_t<8>(2)) <<"\t  8bit Float "<< std::endl <<

pow(cos(floatxx_t<16>{20.0}),floatxx_t<16>(2))+
pow(sin(floatxx_t<16>{20.0}),floatxx_t<16>(2)) <<"\t 16bit Float "<< std::endl <<

(cos(floatxx_t<8>{20.0})^2)+
(sin(floatxx_t<8>{20.0})^2) <<"\t  8bit Float "<< std::endl <<

(cos(floatxx_t<16>{20.0})^2)+
(sin(floatxx_t<16>{20.0})^2) <<"\t 16bit Float "<< std::endl<<

pow(cos(floatxx_t<32>{20.0}),floatxx_t<32>(2))+
pow(sin(floatxx_t<32>{20.0}),floatxx_t<32>(2)) <<"\t 32bit Float "<< std::endl <<

pow(cos(floatxx_t<64>{20.0}),floatxx_t<64>(2))+
pow(sin(floatxx_t<64>{20.0}),floatxx_t<64>(2)) <<"\t 64bit Float "<< std::endl;

auto googol {pow(floatxx_t<512>{10},floatxx_t<512>{100})};  //this calculation may take longer than one hour
cout << "Googol is 10^100 =              \t" << googol << endl;

auto googolplex {pow(floatxx_t<512>{10},googol)};           //this calculation may take longer than one hour
cout << "Googolplex is 10^Googol =       \t" << googolplex << endl;

auto sqrt_googolplex {pow(googolplex,floatxx_t<512>(0.5))};	           //this calculation may take longer than one hour
cout << "Square root of Googolplex is: = \t" << sqrt_googolplex << endl;


//auto time{rom::mashinetime()-start};
//cout <<"This calculation took " << time << " seconds."<< std::endl;

}
