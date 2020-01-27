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
for (	;inp!=0;inp--) {res *= ui(inp);}
return res;
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
flt pow(const flt& base, const flt& exponent) {return exp(exponent*log(base));}

template <class flt>                    //any floating point type
flt sin(const flt& x) {               //Returns the sinus of x radiants
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
flt cos(const flt& x) {               //Returns the cosinus of x radiants
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
flt val{0.0};
flt last_val{0.0};
size_t i{0};
do      {
        last_val = val;
	flt tmp1{factorial<flt>(2*i)};
//	std::cout <<"tmp1 "<< std::string(tmp1) << std::endl;
	flt tmp2{x^(2*i+1)};
//	std::cout <<"tmp2 "<< std::string(tmp2) << std::endl;
	flt tmp3(flt(2.0) ^ (2 * i));
//	std::cout <<"tmp3 "<< std::string(tmp3) << std::endl;
	flt tmp4{(factorial<flt>(i)^2.0)};
//	std::cout <<"tmp4 "<< std::string(tmp4) << std::endl;
	flt tmp5{2.0 * i +1.0};
//	std::cout <<"tmp5 "<< std::string(tmp5) << std::endl;
	flt one_term = tmp1*tmp2/(tmp3*tmp4*tmp5);
//	std::cout << "one_term "<< std::string(one_term) << std::endl;
	val += one_term;
//	std::cout << std::string(val) << std::endl;
	i++;
        } while (last_val != val);
return val;
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


