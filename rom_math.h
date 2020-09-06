#include <utility>
#include <array>
#include "rom_int.h"
#include "rom_float.h"

#ifndef ROM_MATH
#define ROM_MATH

namespace rom {
template <class ui>             	//any literal type
const ui& factorial_inv(size_t inp);   //calculate (1.0/inp!) //recursive cached algorithm
}

namespace {//anonymous

template <class flt>    	//any floating point type
flt log_core(flt x) {        	//Returns the natural log of x; abs(xin) should be smaller than 2.0
x -= 1;                         //this value has to be smaller than 1.0 otherwise our
flt val{0.0};                   //taylor series will never converge --> infinite loop
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

template <class flt>    	//any floating point type
flt log_core_2(const flt& xin) {     	//Returns the natural logarithm of x
static const auto flt1_9{flt{1.9}};   //should work for all values that are larger than 0.0
static const flt inv_1_9{flt{1}/flt1_9};
if (xin<=0)     {throw std::runtime_error("cannot calculate ln of negative number");}
if (xin > flt1_9) {     //shotcut for large numbers
        static auto log_1_9{log_core(flt1_9)};
        return  log_1_9 + log_core_2(xin*inv_1_9);
        }
return log_core(xin);
}

template <class flt>    	//any floating point type
flt exp_core(const flt& inp) {	//only for use with exp() function!!!!!!!
flt val{0};				//we are using taylor series to calculate this value
flt last_val{0};
size_t i{0};
do      {
        last_val = val;
        val += (inp^i)*::rom::factorial_inv<flt>(i);
        ++i;
        } while (last_val != val);
return val;
}

}// namespace anonymous
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rom {

template <size_t precision = 64>     //intended numer of bits representing the mantissa
class floatxx_t;

template <size_t p>
auto log(const floatxx_t<p>& xin,bool debug=false) -> floatxx_t<p> {
typename floatxx_t<p>::temp_type m{xin._mant};				//split in mantissa and exponent
typename floatxx_t<p>::temp_type e{xin._exp};
static auto log_2{log_core_2(typename floatxx_t<p>::temp_type{2})}; //  this is bad recursion!!!!!  ; //ln(2.0)
//if (debug) {std::cout << "log[ " << xin.operator std::string() << " ] = ";}
//if (debug) {std::cout << "[[exp " << e.operator std::string() << "  ";}
//if (debug) {std::cout << "man " << m.operator std::string() << "]]  ";}
//if (debug) {std::cout << "( " << e.operator std::string()  <<" * "<<log_2.operator std::string() <<" * "<<log_core_2(m).operator std::string()   <<" ) = ";}
auto ret{e*log_2 + log_core_2(m)};
floatxx_t<p> ret2 {ret};
//if (debug) {std::cout << "[ " << ret.operator std::string() << " ] -> \t";}
return ret2;
}

template <size_t p>
floatxx_t<p> log(const floatxx_t<p>& x,const floatxx_t<p>& bas) {return log(x,false)/log(bas);}//overload for arbitrary base

template <size_t p>
floatxx_t<p> exp(const floatxx_t<p>& xin) {	//Returns e^x
static auto ln2{log(floatxx_t<p>{2})};

return {floatxx_t<p>::_2pow(xin/ln2)};
}

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
static auto ret{exp(flt{1.0})};
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
//std::cout << "pow( " << base.operator std::string()  <<", "<<exponent.operator std::string()<<" ) = \t";
auto log_base{log(base,true)};
//std::cout << "e^( " << exponent.operator std::string()  <<", "<<log_base.operator std::string()<<" ) = \t";
auto ret {exp(exponent*log_base)};
//std::cout << ret.operator std::string() << std::endl;
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

