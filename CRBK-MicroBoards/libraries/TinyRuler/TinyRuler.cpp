#include <Arduino.h>

#include <avr/io.h>

#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "TinyRuler.h"

void gotoSleep();

//const int outs[]= {PA0, PA1, PA2, PA3, PA5};
const int outs[]= {PORTA0, PORTA1, PORTA2, PORTA3, PORTA5};
const int outCnt = 5;
Animation* Animation::ActiveAnimation=0;
Run   defaultAnimation;
Flash flash;
TinyRuler tr;


// Zustände
int stabilerSensorWert=-1;
int letzterSensorWert=0;
unsigned int letzterSensorwertWechsel=0;
bool aufgewacht=false;

ISR(PCINT0_vect)
{
  GIFR &= ~(1<<PCIF0);
}


bool Animation::isNextStep() {
    if((millis() - lastTransition) > transitionTime) {
      laststep=step;
      step+=dir;
      if(step >= maxStepCnt) {
        cnt++;
        if(dir>0) {
          step = 0;
        } else {
          step=maxStepCnt-1;          
        }
      }
      lastTransition = millis();
      return true;
    }
    return false;
}

Animation::Animation() {
    lastTransition=0;
    transitionTime=50;
    step=0;
    cnt=0;
    dir=1;  
    maxStepCnt=outCnt;
    laststep=0;
}

void Animation::start() { cnt=0; step=0; laststep=0;}
void Animation::stop() { tr.allOff(); }
unsigned char Animation::cycles() const { return cnt;  }
  
unsigned char Animation::Do(Animation* pAni) {
  if(ActiveAnimation!=pAni) {
    ActiveAnimation->stop();
    ActiveAnimation= pAni;
    ActiveAnimation->start();
  }
  if(!ActiveAnimation)
    return 0;
  
  ActiveAnimation->handle();
  return ActiveAnimation->cycles();
}

bool Animation::handle() {
  if(isNextStep()==true) {    
    PORTA &= ~(1 << outs[laststep]);
    PORTA |= 1 << outs[step];
    return true;
  }
  return false;
}

Flash::Flash() { maxStepCnt=2; transitionTime=20; }

bool Flash::handle() {
  if(isNextStep()==true) {
    if(!step)
      tr.allOff();
    else {
      tr.allOn();
    }
    return true;
  }
  
  return false;
}

Run::Run() { tStep=-1; }

void Run::start() { Animation::start(); transitionTime=55; }

bool Run::handle() {  
  if(Animation::handle()==true) {
    transitionTime+=tStep;

    if(transitionTime<20) {
      tStep=1;
      transitionTime=20;
    } else if(transitionTime>70) {
      transitionTime=70;
      tStep=-1;
      if(dir==1)
        dir=-1;
      else
        dir=1;
    }
    return true;  
  }
  return false;
}

void TinyRuler::setup() {
  cli();
  
  PORTA = 0;
  DDRA = 1<<DDA0 | 1<<DDA1 | 1<<DDA2 | 1<<DDA3 | 1<<DDA5;
  DDRB = 1 <<DDB2;
      
  GIMSK = 1<<PCIE0;             // Enable Pin Change Interrupts
  PCMSK0 = 1<<PCINT7;           // Use PA7 as interrupt pin
  GIFR &= ~(1<<PCIF0);          // Reset pin change interrupt flag

  PORTB|=(1<<PORTB2); // Status LED ausschalten);
  
  sei();    
}

void TinyRuler::loop() {
  int sensor =(PINA & (1<<PORTA7));
  if(letzterSensorWert!=sensor) {
    letzterSensorwertWechsel=millis();
    letzterSensorWert=sensor;
  }
  else if((millis()-letzterSensorwertWechsel)>500) {
    stabilerSensorWert = letzterSensorWert;
  }
  
  if(millis()<5000) {
    // StartPhase
    Animation::Do(&defaultAnimation);
  } else if(aufgewacht==true) {
    if(stabilerSensorWert>=0)
      aufgewacht=false;
    else if((millis()-letzterSensorwertWechsel)>700)
      gotoSleep();
  } else {
    if(stabilerSensorWert==0) {
      PORTB|=(1<<PORTB2);
      Animation::Do(&defaultAnimation);
    } else if(stabilerSensorWert>0) {
      PORTB&=~(1<<PORTB2);
      // Lineal liegt, bereite schlafen vor
      if(Animation::Do(&flash)==20)
        gotoSleep();
    } 
  }
    
}

void TinyRuler::allOn() {
  unsigned char o=0;
  for(int i=0; i< outCnt; i++) {
    o |= 1<<outs[i];
  }
  PORTA |= o;
}

void TinyRuler::allOff() {
  unsigned char o=0;
  for(int i=0; i< outCnt; i++) {
    o |= 1<<outs[i];
  }
  PORTA &=~o;
}

//Attiny in den Schlafmodus setzen
void gotoSleep()
{
  aufgewacht=false;
  PORTB|=(1<<PORTB2);
  tr.allOff();
  
  ADCSRA = 0;           // turn off ADC
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);     
  cli();                // Disable interrupts  
  GIFR &= ~(1<<PCIF0);  // Reset pin change interrupt
  sleep_enable();       // sleep
  sleep_bod_disable();
  sei();                // Enable interrupts
  sleep_cpu();
  sleep_disable();
  
  aufgewacht=true;
  letzterSensorwertWechsel=millis();
  stabilerSensorWert=-1;
}