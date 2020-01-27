this repository declares and defines two unlimited dynamic integer types
1.      rom::uintxx_t   an unsigned integer with dynamic and possibly unlimited size
2.      rom::intxx_t    an signed integer with dynamic and possibly unlimited size
(recomendation:  only use rom::intxx_t type to be safe if negative values come up)
and one unlimited floatingpoint types with user defined precision
3.	rom::floatxx_t<size_t nr>  (nr defines the number of significant bits of this type)


--> download all files into one dirctory

--> compile:
g++ example.cpp -o example -std=c++11 -Wno-psabi -Weffc++ -Wall -pedantic -pthread -Os 

--> run:
./example	//you will see a short demo


example output might be:

e       is:	+2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274274663919320030599218174135e+0
ln(1500.0)is:	+7.313220387090301e+0
3^3.1     is:	+3.013532569891542e+1
sin(20.0)  is:	+9.1294525072762765437609998384583e-1
cos(20.0)  is:	+4.0808206181339198606226786092756e-1
cos(20.0)*cos(20.0) + sin(20.0)*sin(20.0)  is:	
+1.0000000000000000000000000000002e+0
arcsin(-0.5)  is:	-5.235987755982988e-1
pi            is:	+3.141592653589793238462643383279502884197169399375105820974944592e+0

