#ifndef _TINYRULER_H_
#define _TINYRULER_H_

class TinyRuler {
public:
    void setup();
    void loop();
    void allOn();
    void allOff();
};

class Animation {
  protected:
  static Animation* ActiveAnimation;
  unsigned long lastTransition;
  unsigned long transitionTime;
  unsigned char step;
  unsigned char laststep;
  unsigned char cnt;
  unsigned char maxStepCnt;
  int dir;
  
  bool isNextStep();
  public:
  Animation();
  
  virtual void start();
  void stop();
  unsigned char cycles() const;
  virtual bool handle();
  
  static unsigned char Do(Animation* pAni);
};

class Flash : public Animation {
  public:
  Flash();
  virtual bool handle();
};

class Run : public Animation {
  int tStep;
  public:
  Run();

  virtual void start();
  virtual bool handle();
};

#endif