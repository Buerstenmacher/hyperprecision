#include <utility>
#include <array>
#include "rom_int.h"
#include "rom_float.h"

#ifndef ROM_MATH
#define ROM_MATH

namespace rom {

template <class inti> //any integer
inti sqrt(const inti& in) {
if (in<0)       {throw std::runtime_error("cannot calculate sqrt of negative integer");}
if (in<2)       {return in;}
auto largecan =  (sqrt((in>>2))<<1) + inti{1};
return ((largecan^2) > in)?(--largecan):(largecan);
}

template <class ui>             //any literal type
ui factorial_core(size_t inp) {      //calculate inp!
ui res{1};
while(inp) {res *= ui(inp--);}
return res;
}

template <class ui>             	//any literal type
const ui& factorial(size_t inp) {      	//calculate inp! //recursive cached algorithm
static std::vector<ui> mem{1,1,2,6,24}; //we define the first values ourself 0! =1; 1! =1; 2! = 2; 3! = 6 .......
if (inp < mem.size())	{return mem.at(inp);}
while (inp>=mem.size())	{mem.push_back(mem.back()*mem.size());}
return factorial<ui>(inp);
}

//to store the inverses of factorial numbers in an static std::vector may help speed up our taylor series
template <class ui>             	//any literal type
const ui& factorial_inv(size_t inp) {   //calculate (1.0/inp!) //recursive cached algorithm
static std::vector<ui> mem{1,1,0.5}; 	//we define the first values ourself
if (inp < mem.size())	{return mem.at(inp);}
while (inp>=mem.size())	{mem.push_back(ui{1}/factorial<ui>(mem.size()));}
return factorial_inv<ui>(inp);
}

template <class flt>    	//any floating point type
flt modf(flt x,flt* intpart){	//shoould behave like modf from standard library
*intpart = flt(intxx_t(x));
return (x - *intpart);
}

template <class flt>		//any floating point type
flt exp_core(const flt& inp) {	//only for use with exp() function!!!!!!!
flt val{0};			//we are usion taylor series to calculate this value
flt last_val{0};
size_t i{0};
do 	{
	last_val = val;
	val += (inp^i)*factorial_inv<flt>(i);
	++i;
	} while (last_val != val);
return val;
}

template <class flt>	//any floating point type
flt exp(flt inp) {//Returns the base-e exponential function of inp, which is e raised to the power inp
static const flt flt0{0};
static const flt flt1{1};
if (inp==flt0) {return flt1;}			//x^0.0 is always 1.0
if (inp<flt0)  {return flt1/exp(flt0-inp);}	//if we get a negative number we will calculate the
static const flt flt11{11};			//inverse of its positive representation
if (inp<flt11) {return exp_core(inp);}		//if we get a small number we will calculate it directly
static constexpr uint8_t expected_digits{12};	//do not calculate e^(10^(more than 12)) !!!
static std::array<flt,expected_digits> mem{};	//e^10^0,e^10^1,e^10^2......
uint8_t digit{expected_digits};
while (--digit >= 1) {
	auto num{flt{uintxx_t::ten_pow(digit)}};
	if (inp > num) {	//shortcut for large numbers
		if (mem.at(digit)==flt0) {mem.at(digit) = exp(num);}
		flt muli{1};
		while (inp >= num) {
			inp -= num;
			muli*=mem.at(digit);
			}
		return muli*exp(inp);
		}
	}
throw std::runtime_error("function rom::exp() failed");
}

template <class flt>	//any floating point type
flt e(void) {		//calculate eulers number
static auto ret{exp(flt{1.0})};
return ret;
}

template <class flt>            //any floating point type//only for use with log() function!!!!!!!
flt log_core(const flt& xin) {	//Returns the natural log of x; abs(xin) should be smaller than 2.0
flt x = xin-1;  		//this value has to be smaller than 1.0 otherwise our
flt val{0.0};   		//taylor series will never converge --> infinite loop
flt last_val{0.0};
size_t i{1};
do      {
        last_val = val;
        val += (x^i)/flt(i);
        ++i;
        if (val == last_val) {break;}
        last_val = val;
        val -= (x^i)/flt(i);
        ++i;
        } while (last_val != val);
return val;
}

template <class flt>	//any floating point type
flt log(flt xin) { 	//Returns the natural logarithm of x
if (xin<=0)	{throw std::runtime_error("cannot calculate ln of negative number");}
static const flt flt0{0};
static const flt flt1{1};
static const auto flt1_6{flt{1.6}};
static const auto flt1_5{flt{1.5}};
if (xin==flt1) 	{return flt0;}			//ln^1.0 is 0.0
if (xin<=flt1_6){return log_core(xin);}		//if we get a small number we will calculate it directly
static constexpr uint8_t expected_digits{24};	//do not calculate ln^(10^(more than 24)) !!!
static std::array<flt,expected_digits> mem{};
uint8_t i{expected_digits};
while (--i >= 1) {
	auto num{flt{uintxx_t::ten_pow(i)}};
	if (xin > num) {	//shortcut for large numbers
		auto divi{flt1/num};
		if (mem.at(i) == flt0)	{mem.at(i) = rom::log<flt>(num);}
		flt multi{};	//0.0
		while (xin > num) {
			multi += mem.at(i);
			xin *= divi;
			}
        	return  multi + log(xin);
        }
}
if (xin > flt1_6) {	//shotcut for large numbers
        static auto log_1_5{rom::log<flt>(flt1_5)};
        return  log_1_5 + log(xin/flt1_5);
        }
throw std::runtime_error("function rom::log() failed");
}

template <class flt>                    //any floating point type
flt pow(const flt& base, const flt& exponent) {
if (base<=0)	{//negative base will only be accepted if exponent is integer
	static flt neg1{-1};
	flt intpart;
	flt fractpart = rom::modf(exponent,&intpart);
	intxx_t in{intpart};
	if (fractpart == flt{0}) {
		return bool(in%2)?(neg1*pow(neg1*base,intpart)):pow(neg1*base,intpart);
		}
	throw std::runtime_error("cannot calculate exp of negative base, result would be a complex number");
	}
return exp(exponent*log(base));
}

template <class flt>
flt pi(void);

template <class flt>
flt remove_entire_circles(const flt& rad) {
static flt two_pi{ pi<flt>() * flt(2) };
static flt fractpart{},intpart{};
fractpart = rom::modf(rad/two_pi,&intpart);
return fractpart * two_pi;
}

template <class flt>                    //any floating point type
flt sin(const flt& xin) {		//Returns the sinus of x radiants
auto x{remove_entire_circles(xin)};
flt val{0.0};
flt last_val{0.0};
size_t i{1};
do      {
        last_val = val;
        val += (x^i)*factorial_inv<flt>(i);
        i+=2;
        if (val == last_val) {break;}
        last_val = val;
        val -= (x^i)*factorial_inv<flt>(i);
        i+=2;
        } while (last_val != val);
return val;
}

/* 	OK but lower accuracy
template <class flt>                    //any floating point type
flt cos(const flt& xin) {               //Returns the cosinus of x radiants
static flt pi_halve{ pi<flt>() / flt(2) };
return sin(xin+pi_halve);		//cos(x)  = sin(x+90)
}*/

template <class flt>                    	//any floating point type
flt cos(const flt& xin) {               	//Returns the cosinus of x radiants
auto x{remove_entire_circles(xin)};
flt val{0.0};
flt last_val{0.0};
size_t i{0};
do      {
        last_val = val;
        val += (x^i)*factorial_inv<flt>(i);
        i+=2;
        if (val == last_val) {break;}
        last_val = val;
        val -= (x^i)*factorial_inv<flt>(i);
        i+=2;
        } while (last_val != val);
return val;
}

template <class flt>				//any floating point type
flt tan(const flt& x) {return sin(x)/cos(x);}	//Returns the tangens of x radiants

template <class flt>				//any floating point type
flt arcsin(const flt& x) {			//Returns arcsin of x
if (x<=-1.0 or x>=1.0)	{throw std::runtime_error("cannot calculate arcsin of "+std::string(x));}
flt val{0.0};
flt last_val{0.0};
size_t i{0};
do      {
        last_val = val;
	flt tmp1{factorial<flt>(2*i)};
	flt tmp2{x^(2*i+1)};
	flt tmp3(flt(2.0) ^ (2 * i));
	flt tmp4{factorial<flt>(i)^2.0};
	flt tmp5{2.0 * i +1.0};
	val += 	(tmp1*tmp2/(tmp3*tmp4*tmp5));
	i++;
        } while (last_val != val);
return val;
}

template <class flt>	//any floating point type
flt pi(void) {		//calculate pi 3.1415........
static flt ret{arcsin(flt{0.5})*flt{6.0}};
return ret;
}

template <class flt>				//any floating point type
flt arccos(const flt& x) {			//Returns arccos of x
if (x<=-1.0 or x>=1.0)	{throw std::runtime_error("cannot calculate arcos of "+std::string(x));}
return pi<flt>()*flt{0.5} - arcsin(x);
}

//otherwise useless testfunction
template <class ui>     //any number type
ui test_plus_minus(ui in, int64_t min, int64_t max) {
ui ret{in};
for (int64_t i{min}; i!=max; i++)       {ret += ui(rom::intxx_t(i));}
for (int64_t i{min}; i!=max; i++)       {ret -= ui(rom::intxx_t(i));}
return ret;
}

//otherwise useless testfunction
template <class flt>    //any number type
flt test_mul_div(flt in, int64_t min, int64_t max) {
flt ret{in};
for (int64_t i{min}; i!=max; i++)       {ret *= i;}
for (int64_t i{min}; i!=max; i++)       {ret /= i;}
return ret;
}

}//namespace rom

/////////////////////////////////////////////////////////////////////////////////////////$
#endif


