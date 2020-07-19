# OpenBraillePrinter
Aplicacion para la plataforma Lpcxpresso Lpc1769 - (Arm Cortex M3) que permite hacer una impresora braille casera. Reemplazando la electronica de una epson lx810, por PCBs caseros, y cambiando el tipo de cabezal queda funcionando.  // Es una combinacion de proyectos entregados para la facultad que luego de aprobarlos me parecieron utiles dejar como open source para quien le pueda servir.   ////////  English: Microcontroller software for the Lpcxpresso Lpc1769 - (Arm Cortex M3)  that allows you to make a homebrew braille printer, with a PS2 keyboard input.
Contact me if you want to use the full version with PCM audio playback and PC USB connection.

 Info detallada: Ver "Docs/Informe.pdf"

La aplicacion realiza procesos en simultaneo gracias a la programacion hecha con maquinas de estado.

Diagrama bloques del software:

![](https://github.com/ADHSoft/OpenBraillePrinter/blob/master/Docs/src/b.png?raw=true)

Hardware:

![](https://github.com/ADHSoft/OpenBraillePrinter/blob/master/Docs/src/a.png?raw=true)


Ejemplo de la programación de las máquinas de estado:

![](https://github.com/ADHSoft/OpenBraillePrinter/blob/master/Docs/src/c.png?raw=true)

![](https://github.com/ADHSoft/OpenBraillePrinter/blob/master/Docs/src/d.png?raw=true)


Info detallada de las variables de MdEImpresion() :


![](https://github.com/ADHSoft/OpenBraillePrinter/blob/master/Docs/src/impresion.png?raw=true)

----
