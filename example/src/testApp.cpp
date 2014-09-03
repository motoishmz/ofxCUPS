#include "testApp.h"


// required adding [libcups.2.dylib] into your project.
// see also readme.md
ofxCUPS printer;


// ---------------------------------
void testApp::setup()
{
    ofSetFrameRate(1);
    
    
    // listing up installed printer drivers...
    //printer.listPrinters();
    
    // Another way of getting printer list
    vector<string> printerList = printer.getPrinterList();
    int i;
    cout << "--- Printer List" << endl;
    for (i = 0; i < printerList.size(); i++) {
        cout << printerList[i] << endl;
    }
    cout << "--- End Printer List" << endl;
    
    // Get default printer
    string defaultPrinterName = printer.getDefaultPrinterName();
    cout << "Default Printer: " << defaultPrinterName << endl;
    
    // set printer name which you want to use...
    printer.setPrinterName(defaultPrinterName);
    
    // set print options....... see also http://www.cups.org/documentation.php/doc-1.5/options.html
    // or, set printer default option from http://localhost:631/printers/
    printer.addOption("media", "KG.Maximum");
//    printer.addOption("resolution", "300dpi"); // one particularly useful option to know about.
        
    // if necessary.......
    printer.setJobTitle("ofxCUPS Test");
    
}


// ---------------------------------
void testApp::update()
{
    printer.updatePrinterInfo();
}


// ---------------------------------
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
    
    
    // state-reason check for printer error handling e.g. low-toner, paper-stuck....
    // notethe details of log messages depend on your printer
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


// ---------------------------------
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

        if(fileDialogResult.bSuccess)
		{
            // ghetto way of testing the mime type
            ofImage validImageTest;
			
            if(validImageTest.loadImage(fileDialogResult.getPath()))
			{
                // by passing the true boolean, the printer will assume an absolute path to the image to print.
                printer.printImage(fileDialogResult.getPath(), true);
            }
            else {
                ofLog(OF_LOG_ERROR, "Please select a valid image.");
            }
        }
        
    }
    else if (key == 'a')
    {
        printer.clearAllJobs();
    }
    else if(key == 'c')
    {
        printer.checkActiveJobStatus();
    }
    
}
