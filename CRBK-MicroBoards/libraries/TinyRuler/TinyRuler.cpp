#include <Arduino.h>

#include <avr/io.h>

#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "TinyRuler.h"

//const int outs[]= {PA0, PA1, PA2, PA3, PA5};
const int outs[]= {PORTA0, PORTA1, PORTA2, PORTA3, PORTA5};
const int outCnt = 5;
Animation* Animation::ActiveAnimation=0;
Run   defaultAnimation;
Flash flash;
TinyRuler tr;

int          TinyRuler::stabilerSensorWert=-1;
bool         TinyRuler::letzterSensorWert=false;
unsigned int TinyRuler::letzterSensorwertWechsel=0;

// Zustände
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
void Animation::stop() { tr.resetAll(); }
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
      tr.resetAll();
    else {
      tr.setAll();
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

void TinyRuler::init() {
  cli();
  
  PORTA = 0;
  DDRA = 1<<DDA0 | 1<<DDA1 | 1<<DDA2 | 1<<DDA3 | 1<<DDA5;
  DDRB = 1<<DDB2;
      
  GIMSK = 1<<PCIE0;         // Enable Pin Change Interrupts
  PCMSK0 = 1<<PCINT7;       // Use PA7 as interrupt pin
  GIFR &= ~(1<<PCIF0);      // Reset pin change interrupt flag
 
  sei();
}

void TinyRuler::animate() {
  bool aufgewacht = handle();
 
  if(millis()<5000) {
    // StartPhase
    Animation::Do(&defaultAnimation);
  } else {
    if(!getSensor()) {
      Animation::Do(&defaultAnimation);
    } else {
      // Lineal liegt, bereite schlafen vor
      if(Animation::Do(&flash)==20)
        gotoSleep();
    } 
  }
}

bool TinyRuler::handle(unsigned int sleepDelay) {
  bool geradeErwacht = false;
  bool sensor = getSensorValue();
  if(letzterSensorWert!=sensor) {
    letzterSensorwertWechsel=millis();
    letzterSensorWert=sensor;
    stabilerSensorWert=-1;
  } else if((millis()-letzterSensorwertWechsel)>500) {
    stabilerSensorWert = letzterSensorWert ? 1 : 0;
  }
  
  if(millis()>5000) {
    // gehe in den Schlaf wenn der Lagesensor 5s nach der Startphase
    // eindeutiges Signal liefert, oder wenn 20 Sekunden lang nichts passiert ist
    if( (stabilerSensorWert==1 && (millis()-letzterSensorwertWechsel)>sleepDelay) ||
        (millis()-letzterSensorwertWechsel)>20000) {
      gotoSleep();
      geradeErwacht=true;
    } 
  }
  
  return geradeErwacht;
}

void TinyRuler::setAll() {
  unsigned char o=0;
  for(int i=0; i< outCnt; i++) {
    o |= 1<<outs[i];
  }
  PORTA |= o;
}

void TinyRuler::resetAll() {
  unsigned char o=0;
  for(int i=0; i< outCnt; i++) {
    o |= 1<<outs[i];
  }
  PORTA &=~o;
}

void TinyRuler::set(int index) {
  if(index<0 || index >=outCnt) return;

  PORTA |= (1<<outs[index]);
}

void TinyRuler::reset(int index) {
  if(index<0 || index >=outCnt) return;

  PORTA &= ~(1<<outs[index]);
}

void TinyRuler::setStatus() {
   PORTB&=~(1<<PORTB2);
}

void TinyRuler::resetStatus() {
   PORTB|=(1<<PORTB2);
}

bool TinyRuler::getSensorValue() {
  return (PINA & (1<<PORTA7))!=0;
}

bool TinyRuler::getSensor() {
  // gibt den stabilen Sensorwert zurück, falls nicht vorhanden, den instabilen Wert
  return stabilerSensorWert>=0 ? (stabilerSensorWert==1) : getSensorValue();
}

//Attiny in den Schlafmodus setzen
void TinyRuler::gotoSleep()
{
  resetStatus();
  resetAll();
  
  cli();                      // Disable interrupts  
    GIFR &= ~(1<<PCIF0);
    MCUCR |= (1 << SM1); // set_sleep_mode(SLEEP_MODE_PWR_DOWN);   
    MCUCR |= (1 << SE); //  sleep_enable();                        Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sei();                      // Enable interrupts
  
  sleep_cpu();                // sleep

  cli();                      // Disable interrupts
    MCUCR &=~(1 << SE); //    sleep_disable();                        // Clear SE bit
  sei();                      // Enable interrupts
  
  setStatus();
  letzterSensorwertWechsel = millis();
  stabilerSensorWert=-1;
}