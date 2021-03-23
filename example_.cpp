#include "rom_uint.h"
#include "string"

using namespace rom;

uint64_t euclid_gcd(uint64_t a, uint64_t b){	//euclidean algorithm
if (a == 0)	{return b;}
return euclid_gcd(b % a, a);
}

int64_t ext_euclid_gcd(int64_t a,int64_t b,int64_t& x,int64_t& y) {//extended euclidean algorithm
if (a == 0) {    // Base Case
	x = 0;
	y = 1;
	return b;
	}
int64_t x1, y1; // To store results of recursive call
auto gcd = ext_euclid_gcd(b%a, a, x1, y1);
x = y1 - (b/a) * x1;	    // Update x and y using results of recursive call
y = x1;
return gcd;
}

int64_t mul_inv_32bit(int32_t inp) {
int64_t mod{std::numeric_limits<uint32_t>::max()};
mod++;	//2^32
int64_t x, y;
auto res{ext_euclid_gcd(inp,mod,x,y)};
if (res!=1) {throw std::runtime_error("Modular mulitiplicative inverse for "+std::to_string(inp)+" does not exist");}
return (x%mod + mod)%mod;
}

int main(void) {

uintxx__t<uint64_t> bbb{1};
std::cout <<  bbb << std::endl;
for (uint16_t i(1);i!=21;++i)	{bbb*=i;}
std::cout <<  bbb << std::endl;
for (uint16_t i(1);i!=21;++i)	{bbb = bbb.div_one_digit(i);}
std::cout <<  bbb << std::endl;

uint64_t a = 8176640000;
uint64_t b = 243290200;
std::cout << "gcd(" << a << ", " << b << ") = " << euclid_gcd(a, b) << std::endl;
int64_t x, y;
a = 23;
b = 128;
std::cout << "ext_gcd(" << a << ", " << b << ") = " << ext_euclid_gcd(a, b,x,y) << std::endl;
uint32_t res;
std::cout << "mul_inv_32bit of " << a << "= "<<mul_inv_32bit(a) << std::endl;
res = uint32_t(92*uint32_t(mul_inv_32bit(a)));
std::cout << "92/23= "<< res << std::endl;

/*uintxx__t<> num{0};
std::cout << num.effective_size();
std::cout << '\t' << num << std::endl;
*/

/*for (uint8_t i(0);i!=8;++i) {
uint64_t n;
n = uint64_t(1)<<i;
num = n;
std::cout << num.effective_size();
std::cout << '\t' << num << std::endl;
}

uintxx__t<uint8_t> a{77};
std::cout <<  a << std::endl;
for (uint16_t i(1);i!=20;++i) {
	a <<= i;
//	std::cout <<  a << std::endl;
//	a+=i;
//	std::cout <<  a << std::endl;
	}
std::cout <<  a << std::endl;
a*=a;
std::cout <<  a << std::endl;
for (uint16_t i(1);i!=20;++i) {
	a >>= i;
//	std::cout <<  a << std::endl;
//	a-=i;
//	std::cout <<  a << std::endl;
	}
for (uint16_t i(1);i!=20;++i) {
	a >>= i;
//	std::cout <<  a << std::endl;
//	a-=i;
//	std::cout <<  a << std::endl;
	}
std::cout <<  a << std::endl;
*/


}
