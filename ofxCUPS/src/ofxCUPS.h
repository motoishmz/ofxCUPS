#pragma once

#include "ofMain.h"
#include <cups/cups.h>


class ofxCUPS
{
    
public:
    ofxCUPS();
    ofxCUPS(string printerName);
    ~ofxCUPS();
    
    void listPrinters();
    vector<string> getPrinterList();
    string getDefaultPrinterName();
    void printImage(string filename);
    void printImage(string filename, bool isAbsolutePath);
    
    // Uses user defined default printer settings for printing the file
    void printImageWithDefaultOptions(string filename);
    void printImageWithDefaultOptions(string filename, bool isAbsolutePath);

    void updatePrinterInfo();
    void clearAllJobs();
    void checkActiveJobStatus();
    
    int getPrinterState();
    string getPrinterName();
    string getPrinterInfo();
    void setPrinterName(string newPrinterName);
    void setPrinterInfo(string newPrinterInfo);
    void setPrinterState(int newPrinterState);
    void setJobTitle(string newJobTitle);
    
    void addOption(string optionKey, string optionValue);
    void parseOptions(string optionString);
    
    
private:
    int printerState;
    int num_options; // Number of options currently in the options array.
    cups_option_t *options; // Pointer to options array
    string printerName;
    string printerInfo;
    string printOption;
    string jobTitle;
};
