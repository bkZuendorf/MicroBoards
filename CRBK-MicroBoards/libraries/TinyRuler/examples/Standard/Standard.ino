#include "TinyRuler.h"

TinyRuler ruler;

void setup() {
  ruler.init();
}

void loop()
{
  ruler.animate();
}