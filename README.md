# MicroBoards

In diesem Projekt werden µC-Boards bereitgestellt, die im Rahmen des Unterrichts am CRBK entstanden sind.

## Integration der Boards in die Arduino IED
Um die Boards innerhalb der Arduino IDE nutzen zu können, müssen folgende Schritte durchgeführt werden:
- Ergänze die Boardverwalter URL in der Arduino-IDE (Datei/Voreinstellungen) um die Adresse zur JSON Datei: https://raw.githubusercontent.com/bkZuendorf/MicroBoards/main/package_crbk_index.json
- Öffne in der Arduino-IDE unter Werkzeuge/Board/ den Boardverwalter
- Suche nach CRBK
- Installiere CRBK Mikrocontroller Boards

## Boards
Im Augenblick steht ein einfaches Board zur Verfügung.

### TinyRuler
Der TinyRuler ist das erste Board des CRBKs auf Basis eine ATTiny 84A. Ziel war es ein einfaches Board zum direkten Programmieren über den USB-Port zu erzeugen. Das Board ist lediglich dazu gedacht 5 LEDs abhängig von einem Lagesensor anzusprechen. Der erste Entwurf sah die Verwendung der Platinen innerhalb eines 3-D gedruckten Lineals vor. So kann mit Anheben des Lineals die Animation der LEDs verändert werden. Besonderes Augenmerk galt der energiesparenden Umsetzung der Lösung.
Das Board benötigt im Tiefschlafmodus weniger als 1µA und lässt so mit einer 200mAh Knopfzelle eine mehrmonatige Nutzung zu. 

Im Rahmen des Girls' Day wurde es als "Meldelineal" für Schülerinnen und Schüler gestaltet:
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
Der Bootloader muss mit einem Programmieradapter mit üblichen Mitteln (avrdude, Microchip-Studio, ...) programmiert werden. Das fertig [compiliertes Produktionsfile](https://github.com/bkZuendorf/MicroBoards/blob/main/ATTiny84_productionfile.elf) steht zur Verfügung.
Zusätzlich müssen die Fuses geändert werden:
- SELFPRGEN: enable 
- WDTON: disable (keinen Watchdog, wird nicht genutzt und benötigt im Tiefschlaf zu viel Energie)
- BODLEVEL: disable (Brown-out detection deaktiviern, damit µC auch mit geringer Spannung arbeitet)

#### Programmierung
Nach Integration in die Arduino IDE (s.o.) stehen einige [Beispiele](https://github.com/bkZuendorf/MicroBoards/tree/main/CRBK-MicroBoards/libraries/TinyRuler/examples) zur Verfügung. Mit recht einfachen Mitteln können die LEDs programmiert, Animationen erzeugt werden und der µC in einen energiesparenden Modus versetzt werden.
