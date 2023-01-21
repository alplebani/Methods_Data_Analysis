#ifndef OUTFUNC_H
#define OUTFUNC_H

#include <iostream>
#include <string>
#include <vector>
#include <TMVA/Reader.h>
#include "Event.h"

using namespace std;

class OutFunc {

  public: 

    OutFunc(string name, string dir, 
	    string prefix, double C, vector<bool> useVar);
    ~OutFunc();
    double val(Event* evt);

  private:

    TMVA::Reader* m_reader;
    float m_x, m_y, m_z;            // TMVA needs float
    double m_C;
    string m_name;
    bool m_use_x;
    bool m_use_y;
    bool m_use_z;

};


#endif
