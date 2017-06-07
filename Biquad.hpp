//  Biquad.hpp
//  LZLPF
//  Created by Luke Zeitlin on 23/05/2017.


#ifndef Biquad_hpp
#define Biquad_hpp

#include <stdio.h>

#define FLT_MIN_PLUS          1.175494351e-38         /* min positive value */
#define FLT_MIN_MINUS        -1.175494351e-38         /* min negative value */


class Biquad
{
protected:
    
    //delay elements for biQuad
    
    float m_f_Xz_1;
    float m_f_Xz_2;
    float m_f_Yz_1;
    float m_f_Yz_2;
    
public:
    
    //Filter Coefficients
    
    float m_f_a0;
    float m_f_a1;
    float m_f_a2;
    float m_f_b1;
    float m_f_b2;
    
    //class methods
    
    void flushDelays();
    float doBiQuad(float f_xn);
};

#endif /* Biquad_hpp */
