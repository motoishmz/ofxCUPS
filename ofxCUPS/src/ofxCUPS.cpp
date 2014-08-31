#include "ofxCUPS.h"



#pragma mark -
#pragma mark constructor, destructor
ofxCUPS::ofxCUPS(){}
ofxCUPS::~ofxCUPS(){}
ofxCUPS::ofxCUPS(string printerName_)
{
    setPrinterName(printerName_);
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

void ofxCUPS::printImage(string filename)
{
    printImage(filename, false);
}

void ofxCUPS::printImage(string filename, bool isAbsolutePath)
{
    int num_options = 0;
    cups_option_t *options = NULL;  

    string printFile;
    if(isAbsolutePath)
	{
        printFile = filename;
    }
    else
	{
        printFile = ofToDataPath("./", true) + filename;
    }

    //optionen = "media=DS_PC_size"; //ds40
    //num_options = cupsParseOptions(optionen.c_str(), num_options, &options);  
    
    int last_job_id = cupsPrintFile(printerName.c_str(),
                                    printFile.c_str(),
                                    jobTitle.c_str() ? jobTitle.c_str() : "print from ofxCUPS",
                                    num_options,
                                    options);  
    
    cout << "the job id is: " << last_job_id << endl;
    if (last_job_id == 0)
    {
        cout << "print Error: " << cupsLastErrorString() << endl;
    }
    
    cupsFreeOptions(num_options,options);
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
    cups_dest_t *dests;
    int num_dests = cupsGetDests(&dests);
    cups_dest_t *dest = cupsGetDest(printerName.c_str(), NULL, num_dests, dests);
    
    int num_options = 0;
    cups_option_t *options = (cups_option_t *)0;
    num_options = cupsAddOption(optionKey.c_str(), optionValue.c_str(), num_options, &options);            
    
    cupsFreeDests(num_dests, dests);
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

