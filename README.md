# OpenBraillePrinter
Aplicacion para la plataforma Lpcxpresso Lpc1769 - (Arm Cortex M3) que permite hacer una impresora braille casera. Reemplazando la electronica de una epson lx810, por PCBs caseros, y cambiando el tipo de cabezal queda funcionando.
Esto nació como una combinación de proyectos entregados para la universidad, que luego de aprobarlos me parecieron útiles publicarlos aquí, como open source, para quien le pueda servir.   

English resume: Microcontroller software for the Lpcxpresso Lpc1769 - (Arm Cortex M3)  that allows you to make a homebrew braille printer, with a PS2 keyboard input.



Información Técnica: ( Para más info, ver "/Docs/" )

 

La aplicacion realiza procesos en simultáneo, utilizando programacion hecha con máquinas de estado.

Diagrama bloques del software:

![](https://github.com/ADHSoft/OpenBraillePrinter/blob/master/Documentation/img/b.png?raw=true)

Leyenda: 
	Bloques verdes: registros de archivos de drivers.
	Bloques rojos: Hardware externo. (una PC es opcional).
	"MDE": Máquina De Estado. (Finite State Machine)

Diagrama bloques del Hardware:

![](https://github.com/ADHSoft/OpenBraillePrinter/blob/master/Documentation/img/a.png?raw=true)


Ejemplo de la programación de las máquinas de estado:

![](https://github.com/ADHSoft/OpenBraillePrinter/blob/master/Documentation/img/c.png?raw=true)

![](https://github.com/ADHSoft/OpenBraillePrinter/blob/master/Documentation/img/d.png?raw=true)


Info detallada de las variables de MdEImpresion() :


![](https://github.com/ADHSoft/OpenBraillePrinter/blob/master/Documentation/img/impresion.png?raw=true)

----
