#pragma once
#include <JuceHeader.h>
#include <vector>
#include <cmath>
struct TimberSound:juce::SynthesiserSound{bool appliesToNote(int)override{return true;}bool appliesToChannel(int)override{return true;}};
class TimberVoice:public juce::SynthesiserVoice{
public:
 bool canPlaySound(juce::SynthesiserSound*s)override{return dynamic_cast<TimberSound*>(s)!=nullptr;}
 void prepare(double sr){sampleRate=sr;delay.assign(4096,0);reset();}
 void setParams(float body,float stringTone,float pick,float resonance,float human){bodyAmt=body;stringAmt=stringTone;pickPos=pick;resAmt=resonance;humanAmt=human;}
 void startNote(int note,float vel,juce::SynthesiserSound*,int)override{
  freq=(float)juce::MidiMessage::getMidiNoteInHertz(note);level=vel;length=juce::jlimit(2,4094,(int)std::round(sampleRate/freq));index=0;age=0;active=true;
  juce::Random&r=juce::Random::getSystemRandom();float variation=1.0f+(r.nextFloat()-.5f)*0.08f*humanAmt;
  for(int i=0;i<length;++i){float n=(r.nextFloat()*2-1)*vel*variation;float pos=(float)i/(float)length;float notch=std::abs(std::sin(juce::MathConstants<float>::pi*pos*juce::jmap(pickPos,1.0f,8.0f)));delay[(size_t)i]=n*notch;}
  lp=0;body1=body2=0;
 }
 void stopNote(float,bool tail)override{if(!tail){active=false;clearCurrentNote();}}
 void pitchWheelMoved(int)override{}void controllerMoved(int,int)override{}
 void renderNextBlock(juce::AudioBuffer<float>&out,int start,int n)override{
  if(!active)return;float damp=juce::jmap(stringAmt,0.0f,1.0f,0.986f,0.9988f);damp-=humanAmt*0.0003f;
  for(int i=0;i<n;++i){int next=(index+1)%length;float y=delay[(size_t)index];float avg=.5f*(y+delay[(size_t)next]);lp=.55f*lp+.45f*avg;delay[(size_t)index]=lp*damp;index=next;
   // Coupled spruce/air body surrogate: two ringing body modes excited by string bridge motion.
   body1=0.9972f*body1+0.0028f*y;body2=0.994f*body2+0.006f*(y-body1);
   float body=y+bodyAmt*(body1*2.4f+body2*1.3f);float sat=std::tanh(body*(1.0f+resAmt*1.8f));float sample=sat*level*.42f;
   for(int ch=0;ch<out.getNumChannels();++ch)out.addSample(ch,start+i,sample);++age;
   if(age>(int)(sampleRate*12.0)||std::abs(y)<1e-6f&&age>(int)sampleRate){active=false;clearCurrentNote();break;}
  }
 }
private:
 double sampleRate=44100;std::vector<float>delay;int length=100,index=0,age=0;float freq=440,level=0,lp=0,body1=0,body2=0;float bodyAmt=.55f,stringAmt=.65f,pickPos=.4f,resAmt=.45f,humanAmt=.35f;bool active=false;
 void reset(){std::fill(delay.begin(),delay.end(),0);active=false;}
};
