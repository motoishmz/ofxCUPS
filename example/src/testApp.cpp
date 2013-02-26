#include "testApp.h"


#pragma setup
void testApp::setup()
{
    ofSetFrameRate(1);
    
    
    // listing up installed printer drivers...
    printer.listPrinters();
    
    // set printer name which you want to use...
    printer.setPrinterName("ENTER_YOUR_PRINTER_NAME_HERE");
    
    // print options....... see also http://www.cups.org/documentation.php/doc-1.5/options.html
    // or, set printer default option from http://localhost:631/printers/
    printer.addOption("media", "KG.Maximum");
//    printer.addOption("resolution", "300dpi"); // one particularly useful option to know about.
        
    // if necessary.......
    printer.setJobTitle("ofxCUPS Test");
    
}



#pragma update
void testApp::update()
{
    
    printer.updatePrinterInfo();
    
}


#pragma draw
void testApp::draw()
{
    
    // printer state check
    string printerState;
    switch (printer.getPrinterState())
    {
        case 3:
            printerState = "3: Idle";
            break;
            
        case 4:
            printerState = "4: Printing a job";
            break;
            
        case 5:
            printerState = "5: Stopped";
            break;
            
        default:
            break;
    }
    
    
    // state-reason check for printer error handling. ex) low-toner, paper-stuck....
    // the details of log message depend on the printer type.
    string printerInfo = printer.getPrinterInfo();
    if (strstr( printerInfo.c_str(), "media-empty" ) != NULL)
    {
        // do something...........
    }
    else if (strstr( printerInfo.c_str(), "low-toner" ) != NULL)
    {
        // do something...........
    }
    else if (strstr( printerInfo.c_str(), "offline" ) != NULL)
    {
        // do something...........
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("Printer not connected......", 20, 160);
    }
    
    
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("Press [p] to print test-image.jpg from your data directory.", 20, 20);
    ofDrawBitmapString("Press [o] to choose a jpg or png and print that.", 20, 40);
    ofDrawBitmapString("Press [c] to check state of active jobs... (the results will appear on console)",   20, 60);
    ofDrawBitmapString("Press [a] to clear all jobs... (the results will appear on console)",   20, 80);
    ofDrawBitmapString("printerName:  " + printer.getPrinterName(), 20, 120);
    ofDrawBitmapString("printerState: " + printerState,             20, 140);
    ofDrawBitmapString("printerInfo:  " + printerInfo,              20, 160);
}


#pragma mark -
#pragma keypressed
void testApp::keyPressed(int key)
{
    
    if (key == 'p')
    {
        // prints from your app's data directory.
        printer.printImage("test-image.png");
    }
    else if(key == 'o')
    {
        ofFileDialogResult fileDialogResult = ofSystemLoadDialog("Select a valid image file");

        if(fileDialogResult.bSuccess) {
            // ghetto way of testing the mime type
            ofImage validImageTest;
            if(validImageTest.loadImage(fileDialogResult.getPath())) {
                // by passing the true boolean, the printer will assume an absolute path to the image to print.
                printer.printImage(fileDialogResult.getPath(),true);
            }
            else {
                ofLog(OF_LOG_ERROR, "Please select a valid image.");
            }
        }
        
    }
    if (key == 'a')
    {
        printer.clearAllJobs();
    }
    if(key == 'c')
    {
        printer.checkActiveJobStatus();
    }
    
}
