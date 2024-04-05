/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: string.cpp			|
	| Date: 00/00/2005			|
	| Authors: Giuseppe M.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

/*
	**************************
	**	 Include	**
	**************************
*/
#include <string>
#include <new>

/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace std;


/*
	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////
*/


string::string(): m_data(0), m_size(0)
{}

string::string(const char* data)
{
    m_size = 0;

    while(data[m_size] != '\0')
	m_size++;
    
    m_data = new char[m_size];

    for(unsigned int  i=0; i<m_size; i++)
	m_data[i] = data[i];
}

string::string(char* d, unsigned int s): m_size(s)
{
    m_data = new char[m_size];

    for(unsigned int  i=0; i<m_size; i++)
	m_data[i] = d[i];
}

string::string(const string& cpy)
{
    m_size = cpy.m_size;
    
    m_data = new char[m_size];

    for(unsigned int  i=0; i<m_size; i++)
	m_data[i] = cpy.m_data[i];
}

string::string(int value)
{ // Thanks to Enrico B.

    int div = 1;

    do
    {
	m_size++;
	div *= 10;        
    }
    while( (value / div) != 0 );

    m_data = new char[m_size];

    m_data[m_size - 1] = '\0';
		
    unsigned int i = m_size;

    do
    {
	m_data[i--] = (value % 10) + '0';
	value /= 10;
    }
    while( value != 0 && i >= 0 );
}

string::~string()
{
    if(m_data)
    {
	delete [] m_data;
	m_data = 0;
	m_size = 0;
    }
}

void string::operator+=(char ch)
{
    unsigned int newsize = m_size + 1;

    char* newdata = new char[newsize];

    for(unsigned int i=0; i<m_size; i++)
	newdata[i] = m_data[i];

    newdata[m_size] = ch;

    if(m_data)
	delete [] m_data;

    m_size = newsize;
    m_data = newdata;
}
	    
void string::operator+=(const char* str)
{
    unsigned isize = 0;

    while(str[isize] != '\0')
	isize++;

    for(unsigned int i=0; i<isize; i++)
	push_back(str[i]);
}

string& string::operator+=(const string& s)
{
    unsigned int newsize = m_size + s.size();

    char* new_data = new char[newsize];

    for(unsigned int i=0; i< m_size; i++)
	new_data[i] = m_data[i];

    for(unsigned int i=m_size; i<newsize; i++)
	new_data[i] = s.at(i-m_size);

    if(m_data)
        delete [] m_data;
    
    m_data = new_data;
    m_size = newsize;
    
    return *this;
}

string& string::operator=(const string& cpy)
{
    m_size = cpy.m_size;
    
    m_data = new char[m_size];

    for(unsigned int  i=0; i<m_size; i++)
	m_data[i] = cpy.m_data[i];
    
    return *this;
}

void string::push_back(char ch)
{
    unsigned int newsize = m_size + 1;

    char* newdata = new char[newsize];

    for(unsigned int i=0; i<m_size; i++)
	newdata[i] = m_data[i];

    newdata[m_size] = ch;

    delete [] m_data;

    m_size = newsize;
    m_data = newdata;
}

void string::push_back(char* str, unsigned int size)
{
    for(unsigned int i=0; i<size; i++)
	push_back(str[i]);
}

void string::push_back(const char* str, unsigned int size)
{
    for(unsigned int i=0; i<size; i++)
	push_back(str[i]);
}
