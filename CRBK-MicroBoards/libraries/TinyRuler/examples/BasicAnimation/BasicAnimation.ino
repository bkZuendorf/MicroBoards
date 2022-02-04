#include "TinyRuler.h"

TinyRuler ruler;

void setup() {
  ruler.init();
}

void loop()
{
	ruler.resetAll();
	delay(1000);
	ruler.set(0);
	delay(1000);
	ruler.set(1);
	delay(1000);
	ruler.set(2);
	delay(1000);
	ruler.set(3);
	delay(1000);
	ruler.set(4);
	delay(1000);
	ruler.reset(4);
	delay(1000);
	ruler.reset(3);
	delay(1000);
	ruler.reset(2);
	delay(1000);
	ruler.reset(1);
	delay(1000);
	ruler.reset(0);
	
	delay(2000);
	for(int i=0; i<10; i++) {
		ruler.setAll();
		delay(100);
		ruler.resetAll();
		delay(100);
	}
	delay(1000);
	
	ruler.handle();
}