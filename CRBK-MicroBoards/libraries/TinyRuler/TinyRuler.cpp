#include <Arduino.h>

#include <avr/io.h>

#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "TinyRuler.h"

#define DONT_USE_MYTIMER

const int outs[]= {PORTA0, PORTA1, PORTA2, PORTA3, PORTA5};
const int outCnt = 5;
Animation* Animation::ActiveAnimation=0;
Run   defaultAnimation;
Flash flash;
TinyRuler tr;

bool          TinyRuler::stabilerSensorWert = false;
bool          TinyRuler::letzterSensorWert = false;
unsigned long TinyRuler::letzterSensorwertWechsel = 0;

// ATTiny 84SSU mit 8 MHz
#define F_CPU 8000000UL

#ifdef USE_MYTIMER
volatile unsigned long milliseconds;
ISR(TIM0_COMPA_vect)
{
    milliseconds++;
}
unsigned long mymillis()
{
  return millis();//milliseconds;
}
#else
unsigned long mymillis()
{
  return millis();
}
#endif


ISR(PCINT0_vect)
{
  // externer Interrupt
  // GIFR |&= (1<<PCIF0); // könnte gecleart werden, wird aber laut Datenblatt automatisch
}

bool Animation::isNextStep() {
    if((mymillis() - lastTransition) > transitionTime) {
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
      lastTransition = mymillis();
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
  // LED Ausgänge
  DDRA = 1<<DDA0 | 1<<DDA1 | 1<<DDA2 | 1<<DDA3 | 1<<DDA5;
  // Status LED
  DDRB = 1<<DDB2;
  
  // DDB0, DDB1 USB Eingänge
  // DDB3, DDA4, DDA6 Programmiereingänge
  // DDA7 Sensoreingang

      
#ifdef USE_MYTIMER
  // start the timer 0, prescaler
  TCCR0A = (1<<WGM01); // CTC  mode, Clear Timer on Compare Match 
  TCCR0B = (1<<CS00)|(1<<CS01);   // Prescaler div64 -> 8µs je Zähltakt
  OCR0A = 0.001 * F_CPU/64.0 - 1;  // 1ms, F_CPU @8MHz, div64, 8 Bit Output Compar Register
  OCR0A = 124;
  TIMSK0 |= (1<<OCIE0A);
#endif

  // external interrupt zum Aufwachen
  GIMSK = 1<<PCIE0;         // Enable Pin Change Interrupts
  PCMSK0 = 1<<PCINT7;       // Use PA7 as interrupt pin
 
  sei();
}

void TinyRuler::animate() {
  bool aufgewacht = handle();
 
  if(mymillis()<5000) {
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
    letzterSensorwertWechsel=mymillis();
    letzterSensorWert=sensor;
  } else if((mymillis()-letzterSensorwertWechsel)>500) {
    stabilerSensorWert = letzterSensorWert;
  }
  
  if(mymillis()>5000) {
    // gehe in den Schlaf wenn der Lagesensor 5s nach der Startphase
    // eindeutiges Signal liefert, oder wenn 20 Sekunden lang nichts passiert ist
    if( (stabilerSensorWert && (mymillis()-letzterSensorwertWechsel)>sleepDelay) ||
        (mymillis()-letzterSensorwertWechsel)>20000) {
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

bool TinyRuler::get(int index) {
  if(index<0 || index >=outCnt) return false;

  return (PORTA & (1<<outs[index]))!=0;
}

bool TinyRuler::toggle(int index) {
  if(index<0 || index >=outCnt) return false;

  if(get(index)) reset(index);
  else set(index);
  return get(index);
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
  return stabilerSensorWert;
}

//Attiny in den Schlafmodus setzen
void TinyRuler::gotoSleep()
{
  // reset all Display-LEDs to reduce power
  resetAll();
  // reset status LED
  resetStatus();

  ADCSRA = 0;           // turn off ADC
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);     
  cli();                // Disable interrupts  
  GIFR &= ~(1<<PCIF0);  // Reset pin change interrupt
  sleep_enable();       // sleep
  sleep_bod_disable();
  sei();                // Enable interrupts
  sleep_cpu();
  sleep_disable();
  
  // set status LED
  setStatus();
  
   // init sensor state variables
  letzterSensorwertWechsel = mymillis();
  letzterSensorWert = getSensorValue();
  stabilerSensorWert = letzterSensorWert;
}