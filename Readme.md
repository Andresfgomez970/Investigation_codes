The basic idea of this programs is the construction of useful routines to study the probability distribution of early stage mergers; in a first approximation the problem is studied under the fact that the interaction can be approximatted by the one of two bodies. Being this not the case, the educated guess that we are making could be characterize by simulating the merger completely and seeing what are the principal factors that make our guess wrong.

On the other hand, there are several reasons for which this first approximation could be interesting to be made (see ... for this).


----------------------------------------------------------------------
---------------------------------------------------------------------
The following are the dependencies of the .h files and their goal

-------------------------------------------
input_ouput.h : none

This program seeks to have general and useful input-output funtions that could be used with data in different situations.

--

-------------------------------------------
two_body_analytical.h : input_output.h

This programs seeks to have different useful routines about the two body problem. 

--

-------------------------------------------
montecarlo.h : none

This programs seeks to have different useful routines about the montecarlo methodology. 

--

-------------------------------------------
mappings.h :    input_output.h montecarlo.h two_body_analytical.h

This programs seeks to have functions to be mapped and routines to map such functions under different constrains. 

--

----


Comments about the standar of the code:
A pointer can be converted to another whose type is the same, except for the addition of qualifiers, thefore as the addition of them would be pure noise I only used const as a dynamic Macro.
In addition to this the const declared could be lost modyfying by reference for pointers declared all locally.

As I final point I say that this could be used in function protypes such that the compilator will give you error by trying to change such variables inside, but as it wouldn't even have real effect and
  the warnings are tiresome and the one in question ifvery useful (-Wincompatible-pointer-types) I would definitely ommit const; even more tiresome would be the use of 
  #pragma GCC diagnostic ignored "-Wenum-compare".

Also note that in init function the first const qualifier would be either  way ignored, so I erase it.
