#include "i18n.h"
#include "utils.h"

#include <map>
#include <fstream>
#include <iostream>

//------------------------------------------------------------------------------
std::map <std::string, std::string> i18n;


//------------------------------------------------------------------------------
std::string i18n::translate(std::string msgid)
{
    if (i18n.count(msgid))
        return i18n[msgid];
    else
        return "Error can't find \"" + msgid + "\" in language file.";
}


//------------------------------------------------------------------------------
int i18n::loadTranslation(const std::string& language)
{
    DIR *dpdf;
    std::string i18nPath =  "./languages/";
    std::string line, msgid, msgstr;
    bool msgid_status = false, msgstr_status = false;
    struct dirent *epdf;

    std::map <int, std::string > languages;

    int number = 0, selected_language = 0;
    dpdf = opendir( i18nPath.c_str() ); //read name of i18n-files

    while (epdf = readdir(dpdf) ){
        if(  epdf->d_type == DT_DIR ) continue;
        ++number;
        std::string lng_file =  std::string( epdf->d_name ).substr( 0, std::string( epdf->d_name ).size() - 4 );
        cout << number << "\t" << lng_file << endl; //please put it in a 'drop down menu'
        languages.insert(std::make_pair( number, epdf->d_name ) );
    }
    
    std::string filename = i18nPath + language;

    ifstream input( filename.c_str() );
    if (!input)
    {
        cerr << "Can't read:  " << filename << "\n";
        return -1;
    }
    size_t found_first, found_last;

    int line_number = 0;
    while( getline( input, line ) ){
        line_number++;
        if( line.find( "#" ) != std::string::npos ) continue; //ignore comments
        if( line.find( "msgid" ) != std::string::npos  ){
            if( msgid_status ){
                cerr << "Syntax error in file " << filename << " near line: " << line_number << endl;
                return -1;
            }
            msgid_status = true;
            found_first = line.find( "\"", 5 );
            found_last = line.find( "\"", found_first + 1 );
            msgid = line.substr ( found_first + 1 , found_last - found_first - 1  );
        }
        if( line.find( "msgstr" ) != std::string::npos ){
            if( msgstr_status ){
                cerr << "Syntax error in file " << filename << " near line: " << line_number << endl;
                return -1;
            }
            msgstr_status = true;
            found_first = line.find( "\"", 6 );
            found_last = line.find( "\"", found_first + 1 );
            msgstr = line.substr ( found_first + 1 , found_last - found_first - 1 );
        }
        if( msgid_status && msgstr_status ){
            msgid_status = msgstr_status = false;
            if( !i18n.count( msgid ) )
                i18n.insert( make_pair( msgid, msgstr ) );
            else{
                cerr << "Doublet error in file " << filename << " line: " << line_number - 1 << "." << endl;
                return -1;
            }
        }
    }//end while
}

//------------------------------------------------------------------------------
