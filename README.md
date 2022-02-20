# MicroBoards

In diesem Projekt werden µC-Boards bereitgestellt, die im Rahmen des Unterrichts am CRBK entstanden sind.

## Nutzung der Boards
Um die Boards innerhalb der Arduino IDE nutzen zu können müssen folgende Schritte durchgeführt werden:
- Ergänze die Boardverwalter URL in der Arduino-IDE (Datei/Voreinstellungen) um die Adresse zur JSON Datei: https://raw.githubusercontent.com/bkZuendorf/MicroBoards/main/package_crbk_index.json
- Öffne in der Arduino-IDE unter Werkzeuge/Board/ den Boardverwalter
- Suche nach CRBK
- Installiere CRBK Mikrocontroller Boards

## Boards
Im Augenblick steht ein einfaches Board zur Verfügung

### TinyRuler
Der TinyRuler ist das erste Board des CRBKs auf Basis eine ATTiny 84A. Ziel war es ein einfaches Board zum direkten Programmieren über den USB-Port zu erzeugen. Das Board ist lediglich dazu gedacht 5 LEDs abhängig von einem Lagesensor anzusprechen. Der erste Entwurf sah die Verwendung der Platinen innerhalb eines 3-D gedruckten Lineals vor. So kann mit Anheben des Lineals die Animation der LEDs verändert werden.  

Das [Video](https://www.abitur-am-berufskolleg.info/wp-content/uploads/2021/11/TinyRuler.mp4) zeigt das geplante aussehen in einer ersten Version.

Zur Verwendung des TinyRuler ist die Installation eines Treibers erforderlich. Dies kann entsprechend der Anleitung auf der Original-Bootloader Seite [Micronucleus]([https://github.com/micronucleus/micronucleus/tree/master/windows_driver_installer) erfolgen.

![Schematic](https://github.com/bkZuendorf/MicroBoards/blob/main/documentation/TinyRuler/PCB-Layout/TinyRuler_schematic.png)
![Board](https://github.com/bkZuendorf/MicroBoards/blob/main/documentation/TinyRuler/PCB-Layout/TinyRuler_board.png)
Das [Eagle Layout](https://github.com/bkZuendorf/MicroBoards/documentation/TinyRuler/PCB-Layout) kann verwendet werden, um die Platine nachzubauen.
