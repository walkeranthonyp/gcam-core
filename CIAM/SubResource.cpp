/*! 
* \file SubResource.cpp
* \ingroup CIAM
* \brief SubResource class source file.
* \author Sonny Kim
* \date $Date$
* \version $Revision$
*/

#include "Definitions.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime> 
#include <cassert>
#include "scenario.h"
#include "modeltime.h"
#include "subResource.h"
#include "Grade.h"
#include "xmlHelper.h"

using namespace std;

extern ofstream bugoutfile, outfile;	
extern Scenario* scenario;

const double SCALE_FACTOR_DEFAULT = 1;
const double GDP_EXPANS_DEFAULT = 1;

//! Default constructor.
SubResource::SubResource() {
   nograde = 0;
   minShortTermSLimit = 0;
   priceElas = 1;	// default value if not read in
   
   const Modeltime* modeltime = scenario->getModeltime();
   const int maxper = modeltime->getmaxper();
   cumulativeTechChange.resize( maxper ); // cumulative technical change
   cumulativeTechChange[0] = 1.0;
   annualprod.resize( maxper ); // annual production of subresource
   rscprc.resize( maxper ); // subresource price
   techChange.resize( maxper ); // subresource tech change
   environCost.resize( maxper ); // environmental extraction costs change
   severanceTax.resize( maxper ); // subresource severance tax
   available.resize( maxper ); // total available resource
   cumulprod.resize( maxper ); // cumulative production of subrsrc
   gdpExpans.resize( maxper, GDP_EXPANS_DEFAULT ); // cumulative production of subrsrc
   scaleFactor.resize( maxper, SCALE_FACTOR_DEFAULT );
   
}

//! Destructor.
SubResource::~SubResource() {
   
   for ( vector<Grade*>::iterator outerIter = grade.begin(); outerIter != grade.end(); outerIter++ ) {
         delete *outerIter;
   }
}

//! Clear member variables
void SubResource::clear(){
   name = ""; // MSVC is missing String::Clear();
   nograde = 0;
   minShortTermSLimit = 0;
   priceElas = 1;
   grade.clear();
   rscprc.clear();
   techChange.clear();
   environCost.clear();
   severanceTax.clear();
   available.clear();
   annualprod.clear();
   cumulprod.clear();
}

//! Initialize member variables from xml data
void SubResource::XMLParse( const DOMNode* node )
{	
   DOMNodeList* nodeList = 0;
   DOMNodeList* childNodeList = 0;
   DOMNode* curr = 0;
   DOMNode* currChild = 0;
   string nodeName;
   string childNodeName;
   Grade* tempGrade = 0;
   
   const Modeltime* modeltime = scenario->getModeltime();
   updateAvailable( 0 ); 
   
   // make sure we were passed a valid node.
   assert( node );
   
   // get the name attribute.
   name = XMLHelper<string>::getAttrString( node, "name" );
   
   // get all child nodes.
   nodeList = node->getChildNodes();
   
   // loop through the child nodes.
   for( int i = 0; i < static_cast<int>( nodeList->getLength() ); i++ ){
      curr = nodeList->item( i );
      nodeName = XMLHelper<string>::safeTranscode( curr->getNodeName() );
      
      if( nodeName == "#text" ) {
         continue;
      }

      else if( nodeName == "grade" ){
         map<string,int>::const_iterator gradeMapIter = gradeNameMap.find( XMLHelper<string>::getAttrString( curr, "name" ) );
         if( gradeMapIter != gradeNameMap.end() ) {
            // grade already exists.
            grade[ gradeMapIter->second ]->XMLParse( curr );
         }
         else {
            tempGrade = new Grade();
            tempGrade->XMLParse( curr );
            grade.push_back( tempGrade );
            gradeNameMap[ tempGrade->getName() ] = grade.size() - 1;
         }
      }
      else if( nodeName == "annualprod" ){
         XMLHelper<double>::insertValueIntoVector( curr, annualprod, modeltime );
      }
      else if( nodeName == "minShortTermSLimit" ){
         minShortTermSLimit = XMLHelper<double>::getValue( curr );
      }
      else if( nodeName == "priceElas" ){
         priceElas = XMLHelper<double>::getValue( curr );
      }
      else if( nodeName == "techChange" ){
         XMLHelper<double>::insertValueIntoVector( curr, techChange, modeltime );
      }
      else if( nodeName == "environCost" ){
         XMLHelper<double>::insertValueIntoVector( curr, environCost, modeltime );
      }
      else if( nodeName == "severanceTax" ){
         XMLHelper<double>::insertValueIntoVector( curr, severanceTax, modeltime );
      }
      else if( nodeName == "gdpExpans" ){
         XMLHelper<double>::insertValueIntoVector( curr, gdpExpans, modeltime );
      }
     else if( nodeName == "scaleFactor" ){
        XMLHelper<double>::insertValueIntoVector( curr, scaleFactor, modeltime );
        }
      else {
         XMLDerivedClassParse( nodeName, curr );
      }  
   }
}

//! Complete initialization.
void SubResource::completeInit() {
   nograde = grade.size(); // number of grades for each subresource   
   initializeResource(); // Do any initializations needed for this resource
}

//! Blank definition so that don't have to define in derived classes if there is nothing to write out
void SubResource::toXMLforDerivedClass( ostream& out ) const {   
}	

//! Do any initializations needed for this resource
void SubResource::initializeResource( ) {   
}	

void SubResource::toXML( ostream& out ) const {

    const Modeltime* modeltime = scenario->getModeltime();
    int m = 0;
    // write the beginning tag.
    Tabs::writeTabs( out );
    out << "<subresource name=\"" << name << "\">"<< endl;
    
    // increase the indent.
    Tabs::increaseIndent();
    
    // write the xml for the class members.
    
    // write out the grade objects.
    for( vector<Grade*>::const_iterator i = grade.begin(); i != grade.end(); i++ ){	
        ( *i )->toXML( out );
    }
    
    for(m = 0; m < static_cast<int>(annualprod.size() ); m++ ) {
        XMLWriteElement(annualprod[m],"annualprod",out,modeltime->getper_to_yr(m));
    }
    
    for(m = 0; m < static_cast<int>(gdpExpans.size() ); m++ ) {
        XMLWriteElement(gdpExpans[m],"gdpExpans",out,modeltime->getper_to_yr(m));
    }
    
    for(m = 0; m < static_cast<int>(techChange.size() ); m++ ) {
        XMLWriteElement(techChange[m],"techChange",out,modeltime->getper_to_yr(m));
    }
    
    for(m = 0; m < static_cast<int>(environCost.size() ); m++ ) {
        XMLWriteElement(environCost[m],"environCost",out,modeltime->getper_to_yr(m));
    }
    
    for(m = 0; m < static_cast<int>(severanceTax.size() ); m++ ) {
        XMLWriteElement(severanceTax[m],"severanceTax",out,modeltime->getper_to_yr(m));
    }
    
	for(m = 0; m < static_cast<int>(scaleFactor.size() ); m++ ) {
	   if ( scaleFactor[m] != SCALE_FACTOR_DEFAULT ) {  
         XMLWriteElement(scaleFactor[m],"scaleFactor",out,modeltime->getper_to_yr(m));
      }
	}

    XMLWriteElement(minShortTermSLimit,"minShortTermSLimit",out);
    XMLWriteElement(priceElas,"priceElas",out);
    
    // finished writing xml for the class members.
    
    // write out anything specific to the derived classes
    toXMLforDerivedClass( out );
    
    // decrease the indent.
    Tabs::decreaseIndent();
    
    // write the closing tag.
    Tabs::writeTabs( out );
    out << "</subresource>" << endl;
}

void SubResource::toDebugXML( const int period, ostream& out ) const {
    
    // write the beginning tag.
    Tabs::writeTabs( out );
    out << "<subresource name=\"" << name << "\">"<< endl;
    
    // increase the indent.
    Tabs::increaseIndent();
    
    // write the xml for the class members.
    XMLWriteElement( nograde, "nograde", out );
    XMLWriteElement( minShortTermSLimit, "minShortTermSLimit", out );
    XMLWriteElement( priceElas, "priceElas", out );
    
    // Write out data for the period we are in from the vectors.
    XMLWriteElement( rscprc[ period ], "rscprc", out );
    XMLWriteElement( available[ period ], "available", out );
    XMLWriteElement( annualprod[ period ], "annualprod", out );
    XMLWriteElement( cumulprod[ period ], "cumulprod", out );
    XMLWriteElement( gdpExpans[ period ], "gdpExpans", out );
    XMLWriteElement( techChange[ period ], "techChange", out );
    XMLWriteElement( environCost[ period ], "environCost", out );
    XMLWriteElement( severanceTax[ period ], "severanceTax", out );
   if ( scaleFactor[ period ] != SCALE_FACTOR_DEFAULT ) {  
         XMLWriteElement( scaleFactor[ period ], "scaleFactor", out );
   }
    
    // write out the grade objects.
    for( int i = 0; i < static_cast<int>( grade.size() ); i++ ){	
        grade[ i ]->toDebugXML( period, out );
    }
    
    // finished writing xml for the class members.
    
    // write out anything specific to the derived classes
    toXMLforDerivedClass( out );
    
    // decrease the indent.
    Tabs::decreaseIndent();
    
    // write the closing tag.
    Tabs::writeTabs( out );
    out << "</subresource>" << endl;
}


//! return SubResource name
string SubResource::getName() const {
   return name;
}

double SubResource::getPrice(int per)
{
   return rscprc[per] ;
}

int SubResource::getMaxGrade() // returns total number of grades
{
   return nograde;
}

void SubResource::cumulsupply(double prc,int per)
{	
    const Modeltime* modeltime = scenario->getModeltime();
    int i=0,maxgrd;
    double slope=0;
    cumulprod[per]=0.0;
    
    rscprc[per] = prc;
    // the index of the last grade is number of grades minus one
    // don't forget 0 as first
    maxgrd = nograde-1;
    
    // calculate total extraction cost for each grade
    // This is a waste of time, should only do this once!
    for (int gr=0; gr<nograde; gr++) {
        if (per > 0) {
            cumulativeTechChange[per] = cumulativeTechChange[ per-1 ] * 
                pow( ( 1.0 + techChange[per] ), modeltime->gettimestep(per) );
        }
        // Determine cost
        grade[gr]->calcCost( severanceTax[per],cumulativeTechChange[per], environCost[ per ], per );
    }
    
    if (per == 0) {
        cumulprod[per] = 0.0;
    }
    else {
        // Case 1
        // if market price is less than cost of first grade, then zero cumulative 
        // production
        if (prc <= grade[0]->getCost(per)) {
            cumulprod[per] = cumulprod[per-1];
        }
        
        // Case 2
        // if market price is in between cost of first and last grade, then calculate 
        // cumulative production in between those grades
        if (prc > grade[0]->getCost(per) && prc <= grade[maxgrd]->getCost(per)) {
            int iL=0,iU=0;
            while (grade[i]->getCost(per) < prc) {
                iL=i; i++; iU=i;
            }
            // add subrsrcs up to the lower grade
            for (i=0;i<=iL;i++) {
                cumulprod[per] += grade[i]->getAvail();
            }
            // price must reach upper grade cost to produce all of lower grade
            slope = grade[iL]->getAvail()
                / (grade[iU]->getCost(per) - grade[iL]->getCost(per));
            cumulprod[per] -= slope * (grade[iU]->getCost(per) - prc);
        }
        
        // Case 3
        // if market price greater than the cost of the last grade, then
        // cumulative production is the amount in all grades
        if (prc > grade[maxgrd]->getCost(per)) {
            for (i=0;i<nograde;i++) {
                cumulprod[per] += grade[i]->getAvail();
            }
        }
    }

   // This doesn't seem to work for dep resource. sjs
   if ( scaleFactor[ per ] != 1 ) {
     cout << "scale factor: " << scaleFactor[ per ] << " cumulprod[ per ] : " << cumulprod[ per ]  
     << " cumulprod[ per-1 ]: " << cumulprod[ per-1 ] << endl;
   }
   // multiply change in cumulative produciton by the adjustement scale factor
   cumulprod[ per ] = cumulprod[ per-1 ] + (cumulprod[ per ] - cumulprod[ per-1 ]) * scaleFactor[ per ];
   
   if ( scaleFactor[ per ] != 1 ) {
     cout << " new cumul prod: " << cumulprod[ per ] << endl;
   }

   //available is adjusted in annualsupply
    //available[per]=available[0]-cumulprod[per];
}

double SubResource::getCumulProd(int per)
{
   return cumulprod[per];
}

// there is no period info now, returns the same value for 
// any period.  shk 6/30
void SubResource::updateAvailable( const int period ){
   available[ period ] = 0;
   for ( int i = 0; i < nograde; i++ ) {
      available[ period ] += grade[ i ]->getAvail();
   }
}

//! calculate annual supply
/*! Takes into account short-term capacity limits.
Note that cumulsupply() must be called before calling this function. */
void SubResource::annualsupply(int per,double gnp,double prev_gnp,double price,double prev_price) {
   const Modeltime* modeltime = scenario->getModeltime();
   // for per = 0 use initial annual supply
   // cumulative production is 0 for per = 0
   if (per >= 1) {
      // 2 is for the average of the annual productions
      annualprod[per] = 2.0 * (cumulprod[per] - cumulprod[per-1])/modeltime->gettimestep(per)
         - annualprod[per-1];
      if(annualprod[per] <= 0) {
         cumulprod[per] = cumulprod[per-1];
         annualprod[per] = 0.0;
      } 
      
      // incorporate short-term capacity limits after 1990
      // This represents a limit to how fast production capacity can expand
      if (per >= 2) {
         // minShortTermSLimit is defined in object and read in from xml
         // minShortTermSLimit is the minimun short-term capacity limit
         double cur_annualprod = 0;
         
         // check to see if base short-term capacity (supply) limit is smaller than the minimum
         double max_annualprod = annualprod[per-1]
            *pow(gnp/prev_gnp,gdpExpans[per])
            *pow((1+techChange[per]),modeltime->gettimestep(per));
         
         if(minShortTermSLimit < max_annualprod) { 
            cur_annualprod = max_annualprod; 
         }
         else { 
            cur_annualprod = minShortTermSLimit; 
         }
         
         // adjust short-term capacity limit for price effects
         cur_annualprod *= pow((price/prev_price),priceElas);
         
         // Adjust current production and cumulative production to date
         // if greater than the short-term capacity limit
         if(cur_annualprod < annualprod[per]) {
            cumulprod[per] = cumulprod[per-1]+(cur_annualprod+annualprod[per-1])
               *modeltime->gettimestep(per)/2.0;
            annualprod[per] = cur_annualprod;
         }
      }
      // available is the total resource (stock)
      //available[per]=available[per-1]-(annualprod[per]*modeltime->gettimestep(per)/2);
      available[per]=available[per-1]-((annualprod[per]+annualprod[per-1])
         /2*modeltime->gettimestep(per));
      if (available[per]<=0) available[per] = 0;
   }
}

//! return annual production for period
double SubResource::getAnnualProd(int per)
{
   return annualprod[per];
}

//! return available resource for period
double SubResource::getAvailable(int per)
{
   return available[per];
}

//! write SubResource output to database
void SubResource::MCoutput( const string &regname, const string& secname )
{
   // function protocol
   void dboutput4(string var1name,string var2name,string var3name,string var4name,
      string uname,vector<double> dout);
   
   int i=0, m=0;
   const Modeltime* modeltime = scenario->getModeltime();
   const int maxper = modeltime->getmaxper();
   vector<double> temp(maxper);
   string tssname = name; // tempory subsector name
   string str; // tempory string
   str = name + "Total";
   
   // function arguments are variable name, double array, db name, table name
   // the function writes all years
   // total subsector output
   dboutput4(regname,"Pri Energy Production",secname,name,"EJ",annualprod);
   dboutput4(regname,"Resource",secname,str,"EJ",available);
   dboutput4(regname,"Price",secname,name,"$/GJ",rscprc);
   
   // do for all grades in the sector
   for (i=0;i<nograde;i++) {
      str = tssname + "_" + grade[i]->getName();
      // grade cost
      for (m=0;m<maxper;m++)
         temp[m] = grade[i]->getCost(m);
      dboutput4(regname,"Price",secname,str,"$/GJ",temp);
      // grade extraction cost
      for (m=0;m<maxper;m++)
         temp[m] = grade[i]->getExtCost();
      dboutput4(regname,"Price ExtCost",secname,str,"$/GJ",temp);
      // available resource for each grade
      for (m=0;m<maxper;m++)
         temp[m] = grade[i]->getAvail();
      dboutput4(regname,"Resource",secname,str,"EJ",temp);
   }
}

//! write SubResource output to file
void SubResource::outputfile( const string &regname, const string& sname) {
   const Modeltime* modeltime = scenario->getModeltime();
   // function protocol
   void fileoutput3( string var1name,string var2name,string var3name,
      string var4name,string var5name,string uname,vector<double> dout);
   
   const int maxper = modeltime->getmaxper();
   vector<double> temp(maxper);
   
   // function arguments are variable name, double array, db name, table name
   // the function writes all years
   // total subsector output
   fileoutput3( regname,sname,name," ","production","EJ",annualprod);
   fileoutput3( regname,sname,name," ","resource","EJ",available);
   
}


// ************************************************************
// Definitions for two of the derived classes below.
// Since these are very small changes, keep in same file for simplicity
// ************************************************************

//! Returns the type of the Resource.
string SubDepletableResource::getType() const {
   return "Depletable";
}

//! Parses any input variables specific to this derived class
void SubDepletableResource::XMLDerivedClassParse( const string nodeName, const DOMNode* node ) {
}

//! Returns the type of the Resource.
string SubFixedResource::getType() const {
   return "Fixed";
}

//! Parses any input variables specific to this derived class
void SubFixedResource::XMLDerivedClassParse( const string nodeName, const DOMNode* node ) {
}
