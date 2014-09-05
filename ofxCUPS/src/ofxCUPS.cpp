#include "ofxCUPS.h"



#pragma mark -
#pragma mark constructor, destructor
ofxCUPS::ofxCUPS()
{
    // Initialize options array
    num_options = 0;
    options     = (cups_option_t *)0;
}

ofxCUPS::ofxCUPS(string printerName_)
{
    ofxCUPS(); // Run the default constructor to initialize variables
    setPrinterName(printerName_);
}

ofxCUPS::~ofxCUPS()
{
    // Free CUPS options array
    cupsFreeOptions(num_options, options);
}



#pragma mark -
void ofxCUPS::listPrinters()
{
    cout << "---------- installed printers ----------" << endl;
    int i;  
    cups_dest_t *dests, *dest;  
    int num_dests = cupsGetDests(&dests);  
    
    for (i = num_dests, dest = dests; i > 0; i --, dest ++)
    {
        if (dest->instance)
            printf("%s/%s\n", dest->name, dest->instance);
        else
            puts(dest->name);
    }
    cout << "----------------------------------------" << endl;
}

vector<string> ofxCUPS::getPrinterList()
{
    vector<string> printerList;
    int i;
    cups_dest_t *dests, *dest;
    int num_dests = cupsGetDests(&dests);
    
    for (i = num_dests, dest = dests; i > 0; i --, dest ++)
    {
        stringstream ss;
        if (dest->instance) {
            ss << dest->name << "/" << dest->instance;
        } else {
            ss << dest->name;
        }
        printerList.push_back(ss.str());
    }
    
    cupsFreeDests(num_dests, dests);
    
    return printerList;
}

string ofxCUPS::getDefaultPrinterName()
{
    /* This is bad according to CUPS API Reference
    const char* defaultPrinter = cupsGetDefault();
    stringstream ss;
    ss << defaultPrinter;
    return ss.str();
     */
    
    // Let's use cupsGetDests
    int i;
    // Returns an empty string if no default printer found
    string defaultPrinterName = "";
    cups_dest_t *dests, *dest;
    int num_dests = cupsGetDests(&dests);
    for (i = num_dests, dest = dests; i > 0; i--, dest++) {
        if (dest->is_default) {
            stringstream ss;
            if (dest->instance) {
                ss << dest->name << "/" << dest->instance;
            } else {
                ss << dest->name;
            }
            defaultPrinterName = ss.str();
            break;
        }
    }
    
    return defaultPrinterName;
}

void ofxCUPS::printImage(string filename)
{
    printImage(filename, false);
}

void ofxCUPS::printImage(string filename, bool isAbsolutePath)
{
    string printFile;
    if(isAbsolutePath) {
        printFile = filename;
    } else {
        printFile = ofToDataPath("./", true) + filename;
    }
    
    /*
     So here we go again with a better explanation of what exactly cupsPrintFile does.
     http://www.cups.org/doc-1.1/spm.html#cupsPrintFile
     
     Argument           Description
     ---                ---
     printer            The printer or class to print to.
     filename           The file to print.
     title              The job title.
     num_options        The number of options in the options array.
     options            A pointer to the options array.
     */
    
    int last_job_id = cupsPrintFile(printerName.c_str(),
                                    printFile.c_str(),
                                    jobTitle.c_str() ? jobTitle.c_str() : "print from ofxCUPS",
                                    num_options, // out class instance variable
                                    options); // our instance variable again
    
    cout << "the job id is: " << last_job_id << endl;
    if (last_job_id == 0) {
        cout << "print Error: " << cupsLastErrorString() << endl;
    }
}

void ofxCUPS::printImageWithDefaultOptions(string filename)
{
    printImageWithDefaultOptions(filename, false);
}

void ofxCUPS::printImageWithDefaultOptions(string filename, bool isAbsolutePath)
{
    string printFile;
    if(isAbsolutePath) {
        printFile = filename;
    } else {
        printFile = ofToDataPath("./", true) + filename;
    }
    
    // There has to be a much clever solution. Later. TODO!
    int i, jobid;
    cups_dest_t *dests, *dest;
    int num_dests = cupsGetDests(&dests);
    // Find our printer!
    for (i = num_dests, dest = dests; i > 0; i--, dest++) {
        // Create a printer name that we can compare to the one we have saved
        stringstream ss;
        if (dest->instance) {
            ss << dest->name << "/" << dest->instance;
        } else {
            ss << dest->name;
        }
        
        if (ss.str() == printerName) {
            // We found our printer! Print it!
            jobid = cupsPrintFile(dest->name,
                                  printFile.c_str(),
                                  jobTitle.c_str() ? jobTitle.c_str() : "print from ofxCUPS",
                                  dest->num_options,
                                  dest->options);
            
            cout << "the job id is: " << jobid << endl;
            if (jobid == 0) {
                cout << "print Error: " << cupsLastErrorString() << endl;
            }

            break; // Break the for loop
        }
    }
    
    cupsFreeDests(num_dests, dests);
}

void ofxCUPS::clearAllJobs()
{
    cupsCancelJob2(CUPS_HTTP_DEFAULT, printerName.c_str(), CUPS_JOBID_ALL, 1);
}


void ofxCUPS::updatePrinterInfo()
{
    // Clear existing state and info before updating
    setPrinterState(0);
    setPrinterInfo("");
    
    cups_dest_t *dest;
    cups_dest_t *dests;
    int num_dests = cupsGetDests(&dests);
    //    cout << "num_dests: " << num_dests << endl;
    
    int i;
    const char *value;
    for (i=num_dests, dest=dests; i>0; i--, dest++)
    {
        
        if (dest->instance == NULL)
        {
            if (dest->name != printerName)
                return;
            //            cout << "name: " << dest->name << endl;
            //            cout << "is_default: " << dest->is_default << endl;
            //            cout << "num_options: " << dest->num_options << endl;
            //            cout << "options: " << dest->options << endl;
            
            
            value = cupsGetOption("printer-state", dest->num_options, dest->options);
            setPrinterState(ofToInt(value));
            
            value = cupsGetOption("printer-state-reasons", dest->num_options, dest->options);
            setPrinterInfo(ofToString(value));
            
            //            cout << "------------------------" << endl;
        }
    }
}


void ofxCUPS::addOption(string optionKey, string optionValue)
{
    /*
     Ok, here we need a more clear explanation of what the cupsAddOption() method does.
     Found good explanation here: http://www.cups.org/doc-1.1/spm.html#cupsAddOption
     
     Argument           Description
     ---                ---
     name               The name of the option.
     value              The value of the option.
     num_options        Number of options currently in the array.
     options            Pointer to the options array.
     
     That means that we need to store num_options and options as instance variables
     of this class. We init them in the constructor.
     */
    
    // Here we just operate on the ofxCUPS instance variables.
    // First attempt to set options is going to create new options array,
    // every following addOption() call will add or replace an option.
    num_options = cupsAddOption(optionKey.c_str(), optionValue.c_str(), num_options, &options);
    
    // This option array has to be added as argument to the cupsPrintFile() method
    // in the printImage() method of ofxCUPS.
}

void ofxCUPS::parseOptions(string optionString)
{
    num_options = cupsParseOptions(optionString.c_str(), num_options, &options);
}


void ofxCUPS::checkActiveJobStatus()
{  
    cups_job_t *jobs;  
    string info;
    
    int num_jobs = cupsGetJobs(&jobs, printerName.c_str(), 0, CUPS_WHICHJOBS_ACTIVE);  
    cout << num_jobs << " active jobs." << endl;
    
    
    for (int i = 0; i < num_jobs; i ++)
    { 
        ipp_jstate_t job_state = jobs[i].state;  
        switch (job_state)  
        {  
            case IPP_JOB_PENDING :  
                info += "Job " + ofToString(jobs[i].id) + " is pending.\n";  
                break;  
            case IPP_JOB_HELD :  
                info += "Job " + ofToString(jobs[i].id) + " is held.\n";  
                break;  
            case IPP_JOB_PROCESSING :  
                info += "Job " + ofToString(jobs[i].id) + " is processing.\n";  
                break;  
            case IPP_JOB_STOPPED :  
                info += "Job " + ofToString(jobs[i].id) + " is stopped.\n";  
                break;  
            case IPP_JOB_CANCELED :  
                info += "Job " + ofToString(jobs[i].id) + " is pending.\n";  
                break;  
            case IPP_JOB_ABORTED :  
                info += "Job " + ofToString(jobs[i].id) + " is aborted.\n";  
                break;  
            case IPP_JOB_COMPLETED :  
                info += "Job " + ofToString(jobs[i].id) + " is completed.\n";  
                break;  
        }
    }
    cout << info << endl;
    cupsFreeJobs(num_jobs, jobs);
}




#pragma mark -
#pragma mark getter, setter
/**
 *  printerName....
 */
void ofxCUPS::setPrinterName(string newPrinterName)
{
    printerName = newPrinterName;
}
string ofxCUPS::getPrinterName()
{
    return printerName;
}


/**
 *  printerInfo....
 */
void ofxCUPS::setPrinterInfo(string newPrinterInfo)
{
    printerInfo = newPrinterInfo;
}

string ofxCUPS::getPrinterInfo()
{
    return printerInfo;
}


/**
 *  printerState....
 */
void ofxCUPS::setPrinterState(int newPrinterState)
{
    printerState = newPrinterState;
}

int ofxCUPS::getPrinterState()
{
    return printerState;
}


/**
 *  jobTitle....
 */
void ofxCUPS::setJobTitle(string newJobTitle)
{
    jobTitle = newJobTitle;
}

