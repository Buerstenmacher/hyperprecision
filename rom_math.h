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

template <class ui>             //any type
ui factorial(size_t inp) {      //calculate inp!
ui res{1.0};
while(inp) {res *= ui(inp--);}
return res;
}

template <class flt>    //any floating point type
flt modf(flt x,flt* intpart){
*intpart = flt(intxx_t(x));
x -= (*intpart);
return x;
}

template <class flt>	//any floating point type
flt exp(const flt& inp) {//Returns the base-e exponential function of inp, which is e raised to the power inp
flt val{0.0};		//we are usion taylor series to calculate this value
flt last_val{0.0};
size_t i{0};
do 	{
	last_val = val;
	val += (inp^i)/factorial<flt>(i);
	++i;
	} while (last_val != val);
return val;
}

template <class flt>	//any floating point type
flt e(void) {		//calculate eulers number
static flt ret{0.0};
if (ret==flt{0.0}) {ret=exp(flt{1.0});}
return ret;
}

template <class flt>                    //any floating point type
flt log(const flt& xin) {               //Returns the natural logarithm of x
if (xin<=0)	{throw std::runtime_error("cannot calculate ln of negative number");}
if (xin >= 1.6) {
        static flt log_two{rom::log<flt>(1.5)};
        return log_two + log<flt>(xin/1.5);
        }
flt x = xin-1;  //this value has to be smaller than 1.0 otherwise our
flt val{0.0};   //taylor series will never converge --> infinite loop
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

template <class flt>                    //any floating point type
flt pow(const flt& base, const flt& exponent) {
if (base<=0)	{throw std::runtime_error("cannot calculate exp of negative base");}
return exp(exponent*log(base));
}

template <class flt>
flt pi(void);

template <class flt>
flt remove_entire_circles(const flt& x) {
static flt two_pi{ pi<flt>() * flt(2) };
static flt fractpart{},intpart{};
fractpart = rom::modf(x/two_pi,&intpart);
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
        val += (x^i)/factorial<flt>(i);
        i+=2;
        if (val == last_val) {break;}
        last_val = val;
        val -= (x^i)/factorial<flt>(i);
        i+=2;
        } while (last_val != val);
return val;
}

template <class flt>                    //any floating point type
flt cos(const flt& xin) {               //Returns the cosinus of x radiants
auto x{remove_entire_circles(xin)};
flt val{0.0};
flt last_val{0.0};
size_t i{0};
do      {
        last_val = val;
        val += (x^i)/factorial<flt>(i);
        i+=2;
        if (val == last_val) {break;}
        last_val = val;
        val -= (x^i)/factorial<flt>(i);
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
static flt ret{0.0};
if (ret==flt{0.0}) {ret=arcsin(flt{0.5})*flt{6.0};}
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


