#ifndef __LPF__
#define __LPF__

#include "IPlug_include_in_plug_hdr.h"
#include "Biquad.hpp"

class LPF : public IPlug
{
public:
  LPF(IPlugInstanceInfo instanceInfo);
  ~LPF();
  
  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void calculateLPFCoeffs(float fCutoffFreq, float fQ);
  float m_f_fc_Hz;
  float m_f_Q;
  Biquad m_LeftLPF;
  Biquad m_RightLPF;
  
private:

  
};




#endif
