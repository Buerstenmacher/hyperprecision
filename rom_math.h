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

template <class lit>		//any literal type
lit abs(const lit& in) {	//should calculate the abs of every literal type
static auto zero = lit{0};
return (in<zero)?(zero-in):(in);
}

template <class ui>             	//any literal type
ui factorial_core(size_t inp) {      	//calculate inp!
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
flt modf(const flt& x,flt* intpart){	//shoould behave like modf from standard library
*intpart = flt(intxx_t(x));
return (x - *intpart);
}

template <class flt>    		//any floating point type
flt modf(const flt& x,intxx_t* intpart){	//overload of modf with pointer to universal integer type
*intpart = intxx_t(x);
return (x - *intpart);
}

template <class flt>	//any floating point type
flt e(void) {		//calculate eulers number
static auto ret{flt::exp(flt{1.0})};
return ret;
}

template <class flt>                    	//any floating point type
flt pow(const flt& base, const flt& exponent) {	//todo: find out why it is inaccurate with low bitnumbers
if (base==0)	{return flt{0};}
if (base<0)	{//negative base will only be accepted if exponent is integer
	static flt neg1{-1};
	intxx_t in{};
	if (rom::modf(exponent,&in) == flt{0}) {return bool(in%2)?(neg1*pow(abs(base),flt(in))):pow(abs(base),flt(in));}
	throw std::runtime_error("cannot calculate exp of negative base, result would be a complex number");
	}
auto ret {flt::exp(exponent*flt::log(base))};
return ret;
}

template <class flt>
flt pi(void);

template <class flt>
flt remove_entire_circles(const flt& rad) {//angle in radiants is reduced
static auto two_pi{ pi<flt>() * flt(2) };
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
return sin(xin+pi_halve);		//cos(x)  = sin(x+90°)
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


/*template <class flt>		//any floating point type
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
if (inp<flt0)  {return flt1/exp(abs(inp));}	//if we get a negative number we will calculate the
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
*/

