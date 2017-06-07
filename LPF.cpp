#include "LPF.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 2;

void LPF::calculateLPFCoeffs(float fCutoffFreq, float fQ)

// Massburg coefficients.

{
  float theta_c;
  float g1;
  float gr;
  float gp;
  float gz;
  float sOmp;     //small omega
  float sOmz;
  float sOmr;
  float sOmm;
  float bOmr;
  float bOms;     //big omega
  float bOmm;
  float Qp;       //Q
  float Qz;
  float gam0;     //gamma
  float alp0;     //alpha
  float alp1;
  float alp2;
  float bet1;     //beta
  float bet2;
  
  
  
  theta_c = 2.0*pi*fCutoffFreq/(float)GetSampleRate();
  
  g1 = 2 / sqrt(pow(2 - pow((sqrt(2)*pi ) / theta_c, 2) , 2)  +  pow((2*pi) / (fQ*theta_c) , 2));
  
  
  // big Omega value is dependent on fQ:
  
  if(fQ > sqrt(0.5))
  {
    gr = (2*pow(fQ, 2))  /  sqrt(4*pow(fQ, 2) - 1);
    sOmr = theta_c * sqrt( 1 - ( 1 / pow(2*fQ, 2) ) );
    bOmr = tan (sOmr/2);
    bOms = bOmr * pow( ( pow(gr, 2) - pow(g1, 2 )) / ( pow(gr, 2) - 1) , (1/4));
  }
  else
  {
    sOmm = theta_c * sqrt(( 2 - ( 1 / (2*pow(fQ, 2) ) ) + sqrt(( ( 1- (4*pow(fQ, 2)))/ ( pow(fQ, 4))) +(4/g1))) / 2);
    bOmm = tan(sOmm/2);
    bOms = theta_c *  pow( 1 - pow( g1, 2), 1/4) / 2;
    bOms = fmin(bOms, bOmm);
  }
  
  //calculate pols / zeros (small omega), gains(g) and Qs:
  
  sOmp = 2 * atan(bOms);
  sOmz = 2 * atan(bOms / sqrt(g1));
  
  gp = 1 / ( sqrt( pow( 1 - pow( sOmp / theta_c, 2), 2) + pow(sOmp/(fQ*theta_c), 2)));
  gz = 1 / ( sqrt( pow(1 - pow( sOmz / theta_c, 2), 2) + pow(sOmz/(fQ*theta_c), 2)));
  
  Qp = sqrt((g1*( pow(gp, 2) - pow(gz, 2)))/(( g1 + pow(gz, 2) ) * pow(g1 - 1, 2)));
  Qz = sqrt((pow(g1, 2)*( pow(gp, 2) - pow(gz, 2)))/(pow(gz, 2)*(g1 + pow(gp, 2))*pow(g1 - 1, 2)));
  
  gam0 = pow(bOms, 2) + ((1/Qp) * bOms) + 1;
  
  alp0 = pow(bOms, 2) + ((sqrt(g1)/Qz) * bOms) + g1;
  alp1 = 2*(pow(bOms, 2) - g1);
  alp2 = pow(bOms, 2) - ((sqrt(g1)/Qz) * bOms) + g1;
  
  bet1 = 2*(pow(bOms, 2) - 1);
  bet2 = pow(bOms, 2) - ((1/Qp) * bOms) + 1;
  
  
  
  //left channel:
  
  m_LeftLPF.m_f_a0 = alp0/gam0;
  m_LeftLPF.m_f_a1 = alp1/gam0;
  m_LeftLPF.m_f_a2 = alp2/gam0;
  m_LeftLPF.m_f_b1 = bet1/gam0;
  m_LeftLPF.m_f_b2 = bet2/gam0;
  
  //left channel:
  
  m_RightLPF.m_f_a0 = alp0/gam0;
  m_RightLPF.m_f_a1 = alp1/gam0;
  m_RightLPF.m_f_a2 = alp2/gam0;
  m_RightLPF.m_f_b1 = bet1/gam0;
  m_RightLPF.m_f_b2 = bet2/gam0;
  
  
}

enum EParams
{
  k_f_fc_Hz,
  k_f_Q,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  
  k_f_fc_HzX = 75,
  k_f_fc_HzY = 125,
  
  k_f_QX = 150,
  k_f_QY = 125,
  
  kKnobFrames = 60
};

LPF::LPF(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
  TRACE;
  
  Reset();
  
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(k_f_fc_Hz)->InitDouble("Frequency", 1000., 100., 5000., 1., "Hz");
  GetParam(k_f_fc_Hz)->SetShape(2.);
  
  GetParam(k_f_Q)->InitDouble("Frequency", 0.707, 0.5, 20, 0.1, "Q");
  GetParam(k_f_Q)->SetShape(2.);
  
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  
  pGraphics->AttachPanelBackground(&COLOR_GRAY);
  
  IBitmap* knob = pGraphics->LoadPointerToBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
  pGraphics->AttachControl(new IKnobMultiControl(this, k_f_fc_HzX, k_f_fc_HzY, k_f_fc_Hz, knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, k_f_QX, k_f_QY, k_f_Q, knob));
  
  
  IColor textColor = IColor(255, 0, 0, 0);
  IText textProps4(24, &textColor, "Arial", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityDefault);
  pGraphics->AttachControl(new ITextControl(this, DRAW_RECT(IRECT(80, 44, 220, 84)), &textProps4, "Biquad LPF"));
  
  AttachGraphics(pGraphics);
  
  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

LPF::~LPF() {}

void LPF::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.
  
  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];
  
  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
    *out1 = m_LeftLPF.doBiQuad(*in1);
    *out2 = m_RightLPF.doBiQuad(*in2);
  }
}

void LPF::Reset()
{
  TRACE;
  IMutexLock lock(this);
  
  m_LeftLPF.flushDelays();
  m_RightLPF.flushDelays();
  
  calculateLPFCoeffs(m_f_fc_Hz, m_f_Q);
  
}

void LPF::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  
  
  switch (paramIdx)
  {
    case k_f_fc_Hz:
      m_f_fc_Hz = GetParam(k_f_fc_Hz)->Value();
      
      break;
    case k_f_Q:
      m_f_Q = GetParam(k_f_Q)->Value();
      
      break;
      
    default:
      break;
  }
  calculateLPFCoeffs(m_f_fc_Hz, m_f_Q);
}
