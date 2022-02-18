# CRBK-MicroBoards

In diesem Projekt werden µC-Boards bereitgestellt, die im Rahmen des Unterrichts am CRBK entstanden sind.

## Integration der Boards in die Arduino IED
Um die Boards innerhalb der Arduino IDE nutzen zu können, müssen folgende Schritte durchgeführt werden:
- Ergänzung der Boardverwalter URL in der Arduino-IDE (unter Datei/Voreinstellungen) um die Adresse zur JSON Datei: https://raw.githubusercontent.com/bkZuendorf/MicroBoards/main/package_crbk_index.json
- Öffne in der Arduino-IDE unter Werkzeuge/Board/ den Boardverwalter
- Suche nach CRBK
- Installiere CRBK Mikrocontroller Boards

## Boards
Im Augenblick steht ein einfaches Board zur Verfügung.

### TinyRuler
Der TinyRuler ist das erste Board des CRBKs auf Basis eine ATTiny 84A. Ziel war es ein einfaches Board zum direkten Programmieren über den USB-Port zu erzeugen. Das Board ist lediglich dazu gedacht 5 LEDs abhängig von einem Lagesensor anzusprechen. Der erste Entwurf sah die Verwendung der Platinen innerhalb eines 3-D gedruckten Lineals vor. So kann mit Anheben des Lineals die Animation der LEDs verändert werden. Besonderes Augenmerk galt der energiesparenden Umsetzung der Lösung.
Das Board benötigt im Tiefschlafmodus weniger als 1µA und lässt so mit einer 200mAh Knopfzelle eine mehrmonatige Nutzung zu. 

Im Rahmen der Ausrichtung des Girls' Day wurde die Platine als Bestandteil eines Lineals vorgesehen. Das "Meldelineal" für Schülerinnen und Schüler wurde wir folgt gestaltet:
![Design für den Girls' Day](https://github.com/bkZuendorf/MicroBoards/blob/main/documentation/TinyRuler/TinyRuler.png)

Das [Video](https://www.abitur-am-berufskolleg.info/wp-content/uploads/2021/11/TinyRuler.mp4) zeigt das geplante Aussehen in einer ersten Version.

Ein Video des fertigen Exponats folgt ...

![Schematic](https://github.com/bkZuendorf/MicroBoards/blob/main/documentation/TinyRuler/PCB-Layout/TinyRuler_schematic.png)
![Board](https://github.com/bkZuendorf/MicroBoards/blob/main/documentation/TinyRuler/PCB-Layout/TinyRuler_board.png)

Das [Eagle Layout](https://github.com/bkZuendorf/MicroBoards/tree/main/documentation/TinyRuler/PCB-Layout) kann verwendet werden, um die Platine nachzubauen.
Die [Fusion CAD-Datei](https://github.com/bkZuendorf/MicroBoards/tree/main/documentation/TinyRuler/Fusion-Design) dient der Erzeugung einer 3D-Druck Datei.

#### Treiber
Zur Verwendung des TinyRuler ist die Installation eines Treibers erforderlich. Dies kann entsprechend der Anleitung auf der Original-Bootloader Seite [Micronucleus](https://github.com/micronucleus/micronucleus/tree/master/windows_driver_installer) erfolgen.

#### Erstprogrammierung
Bevor das Board über die USB-Schnittstelle programmiert werden kann, muss einmalig ein Bootloader programmiert werde,
Der Bootloader muss mit einem Programmieradapter mit üblichen Mitteln (avrdude, Microchip-Studio, ...) programmiert werden. Das fertig [compiliertes Produktionsfile](https://github.com/bkZuendorf/MicroBoards/blob/main/ATTiny84_productionfile.elf) steht zur Verfügung.
Zusätzlich müssen die Fuses geändert werden:
- SELFPRGEN: enable 
- WDTON: disable (keinen Watchdog, wird nicht genutzt und benötigt im Tiefschlaf zu viel Energie)
- BODLEVEL: disable (Brown-out detection deaktiviern, damit µC auch mit geringer Spannung arbeitet)

Am Ende sollten die Fuse-Register folgende Werte haben:
- EXTENDED: 0xFE
- HIGH: 0xDF
- LOW: 0xE2

#### Programmierung
Nach Integration in die Arduino IDE (s.o.) stehen einige [Beispiele](https://github.com/bkZuendorf/MicroBoards/tree/main/CRBK-MicroBoards/libraries/TinyRuler/examples) zur Verfügung. Mit recht einfachen Mitteln können die LEDs programmiert, Animationen erzeugt werden und der µC in einen energiesparenden Modus versetzt werden.

Das untenstehende Beispiel zeigt eine typische Anwendung der TinyRuler-Bibliothek
```
#include "TinyRuler.h" // Binde den Bibliothek-Header ein

TinyRuler ruler;  // Erstelle eine Instanz des Lineals

void setup() {
  ruler.init();  // Initialisiere das Lineal bei Start des µCs 
}

void loop()
{
  bool sensor = ruler.getSensor(); // ermittelt die Linealposition, true zeigt liegende Position an

  if(sensor) {
    // Lineal liegt
    ruler.reset(0);   // schalte LED 0 aus
    ruler.toggle(1);  // wechsle den Zustand der LED 1
    delay(500);
    ruler.toggle(2);  // wechsle den Zustand der LED 2
    delay(500);
  } else {
    // Lineal liegt nicht
    if(ruler.get(1)) {
      // wenn LED 1 leuchtet, schalte alle LEDs aus
      ruler.resetAll();
    } else {
      // wenn LED 1 nicht leuchtet, schalte alle LEDs an
      ruler.setAll();
    }
    delay(50);
  }
    
  // rufe handle auf, damit der Lagezustand ermittelt werden kann und 
  // sich das Lineal nach einer Zeit schlafen legt und automatisch wieder aufwacht
  // hier: Lasse Lineal 2,5 Sekunden nach dem Erkennen der Lage in den Schlafmodus wechseln
  ruler.handle(2500);
}
```
