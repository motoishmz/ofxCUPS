# ofxCUPS
a simple wrapper of CUPS API to manipulate your printer from oF.

recently this addon supports the functions below:

- printing direct from oF
- catching errors e.g. low-toner, media tray mepty…
- adding print options e.g. media size, print quality… 

(have to be more useful)

## Notes
- not available for windows.
- required adding libcups.2.dylib (see Screen Shot 2012-03-05 17.35.48.png)

## Installation Instructions
1. Download and install [gutenprint](http://gimp-print.sourceforge.net/MacOSX.php).
	- You can use either the DMG file or compile from source.
2. Add the libcups.2.dylib to your project. 
	- In the Projects window click on your Target.
	- Go to the "Build Phases" tab.
	- Click the "+" button underneath "Link Binary With Libraries"
	- In the ensuing window prompt search for "libcups.2.dylib". Select that and choose add. **You do not need any of the other dylibs listed**.
3. Include the addon in your openFrameworks project.
4. Look to the included example file for how to implement ofxCUPs in your project.	

## Original Forum Post
- http://forum.openframeworks.cc/index.php/topic,1474.0.html

## CUPS api documentation
- http://www.cups.org/documentation.php/doc-1.5/api-cups.html
- http://www.cups.org/documentation.php/options.html





