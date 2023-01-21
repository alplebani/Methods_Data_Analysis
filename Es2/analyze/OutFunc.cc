#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <TMVA/Reader.h>
#include "Event.h"
#include "OutFunc.h"

using namespace std;

OutFunc::OutFunc(string name, string dir, string prefix, double C,
                 vector<bool> useVar){
  m_reader = new TMVA::Reader();

  m_use_x = useVar[0];
  m_use_y = useVar[1];
  m_use_z = useVar[2];

  if ( m_use_x ) { m_reader->AddVariable("x", &m_x); }
  if ( m_use_y ) { m_reader->AddVariable("y", &m_y); }
  if ( m_use_z ) { m_reader->AddVariable("z", &m_z); }
  m_reader->BookMVA(name, dir + prefix + "_" + name + ".weights.xml");
  m_name = name;
  m_C = C;
}

OutFunc::~OutFunc() {

  delete m_reader;

}

double OutFunc::val(Event* evt){
  if ( m_use_x ) { m_x = evt->x; }
  if ( m_use_y ) { m_y = evt->y; }
  if ( m_use_z ) { m_z = evt->z; }
  double f = m_reader->EvaluateMVA(m_name) - m_C;
  return f;
}

