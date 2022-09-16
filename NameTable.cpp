// NameTable.cpp



#include "NameTable.h"
#include <string>
#include <vector>
#include <list>
#include <functional>

using namespace std;


class NameTableImpl
{
  public:
    NameTableImpl();
    void enterScope();
    bool exitScope();
    bool declare(const string& id, int lineNum);
    int find(const string& id) const;
  private:
    const int NUM_BUCKETS = 19997; // use prime number
    struct Node
    {
        string m_id;
        int m_line;
    };
    
    unsigned int myMapFunction(const string &hashMe) const; // give me the bucket
    
    vector<string> m_ids;
    vector<list<Node>> table;
};


NameTableImpl :: NameTableImpl()
{
    table.resize(NUM_BUCKETS); // resize to the number of bukcets
}

unsigned int NameTableImpl :: myMapFunction(const string &hashMe) const // from nachenberg cs32 slides
{
        hash<string> str_hash; // create string hasher
        unsigned int hashValue = str_hash(hashMe); // hash the string
        
        unsigned int bucketNum = hashValue % NUM_BUCKETS; // get bucket number that fits the vector
        return bucketNum;
}

void NameTableImpl::enterScope()
{
      // Extend the id vector with an empty string that
      // serves as a scope entry marker.

    m_ids.push_back("");
    
}

bool NameTableImpl::exitScope()
{
      // Remove ids back to the last scope entry.
    
    while(!m_ids.empty() && !m_ids.back().empty()) // while we dont encounter a diff scope
    {
        int x = myMapFunction(m_ids.back()); // get the bucket where the current id is
        table[x].pop_front();   // pop the most recently added id
        
        m_ids.pop_back(); // keep going down the id vector until we reach an entry marker
    }
    
    if(m_ids.empty() == true)
    {
        return false;
    }
    
      // Remove the scope entry marker itself.

    m_ids.pop_back();
    
    return true;
}

bool NameTableImpl::declare(const string& id, int lineNum)
{
    if (id.empty())
        return false;

      // Check for another declaration in the same scope.
      // Return if found, break out if encounter the scope
      // entry marker.
    
    int i = m_ids.size();
    
    while( i > 0)
    {
        i--; // decrement first
        if(m_ids[i] == "") // checking  "" is faster than  m_ids[i].empty() for some reason
        {
            break; // if this is the first time we are adding the id cin this scope
        }
        if(m_ids[i] == id)
        {
            return false; // if in same scope return false
        }
    }
    
    m_ids.push_back(id); //push the current id
    size_t x = myMapFunction(id); // get a bucket in the table
    
    Node y;
    y.m_id = id;
    y.m_line = lineNum;
    table[x].push_front(y); // add the node with the info we want to the hash table

      // Save the declaration

    return true;
}

int NameTableImpl::find(const string& id) const
{
    if (id.empty())
        return -1;

      // Search back for the most recent declaration still
      // available.

    size_t x = myMapFunction(id);
    
    list<Node> :: const_iterator it;
    
    for(it = table[x].begin(); it != table[x].end(); it++) // traverse the bucket's list
    {
        if(it->m_id == id)
        {
            return it->m_line; // return the line it was found at if found
        }
    }
    return -1; // otherwise return -1
}

//*********** NameTable functions **************

// For the most part, these functions simply delegate to NameTableImpl's
// functions.

NameTable::NameTable()
{
    m_impl = new NameTableImpl;
}

NameTable::~NameTable()
{
    delete m_impl;
}

void NameTable::enterScope()
{
    m_impl->enterScope();
}

bool NameTable::exitScope()
{
    return m_impl->exitScope();
}

bool NameTable::declare(const string& id, int lineNum)
{
    return m_impl->declare(id, lineNum);
}

int NameTable::find(const string& id) const
{
    return m_impl->find(id);
}
