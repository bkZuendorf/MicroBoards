#include "TinyRuler.h"

TinyRuler ruler;

bool set=true;
int index=0;

void setup() {
  ruler.init();
}

void loop()
{
  // true zeigt liegende Position an
  bool sensor = ruler.getSensor();

  if(sensor) {
    // wenn es liegt, die LEDs einzeln umschalten
    ruler.toggle(index);
    delay(100);
  } else {
    // andernfalls nur blinken lassen
    if(ruler.get(0)) {
      ruler.resetAll();
    } else {
      ruler.setAll();
    }
    delay(10);
  }
    
  index=(index+1)%5;

  // rufe handle auf, damit sich das Lineal nach einer Zeit schlafen legen kann
  // und automatisch wieder aufwacht
  // hier: Lasse Lineal 2 Sekunden nach dem Erkennen der Lage in den Schlafmodus wechseln
  ruler.handle(2000);
}