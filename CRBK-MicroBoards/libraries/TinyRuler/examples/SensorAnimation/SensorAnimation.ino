#include "TinyRuler.h"

TinyRuler ruler;

bool set=true;
int index=0;

void setup() {
  ruler.init();
}

void loop()
{
  bool sensor = ruler.getSensor();
  
  int multiplier = sensor ? 10 : 1;
  
  ruler.resetAll();
  if(set)
    ruler.set(index);
  else
    ruler.reset(index);

  index++;
  index=index%5;
  if(!index) set=!set;
  delay(10*multiplier);

  ruler.handle(5000);
}