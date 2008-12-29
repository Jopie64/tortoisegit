#include "StdAfx.h"
#include "GitStringList.h"

//CGitStringList::CGitStringList(void)
//{
//}

CGitStringList::~CGitStringList(void)
{
}


INT_PTR CGitStringList::GetCount() const
{
	return m_lString.size();
}

INT_PTR CGitStringList::GetSize() const
{
	return m_lString.size();//?
}

BOOL CGitStringList::IsEmpty() const
{
	return m_lString.empty();
}


// peek at head or tail
CString& CGitStringList::GetHead()
{
	return m_lString.front();
}

const CString& CGitStringList::GetHead() const
{
	return m_lString.front();
}

CString& GetTail();
const CString& CGitStringList::GetTail() const
{
	return m_lString.back();
}


// Operations
// get head or tail (and remove it) - don't call on empty list!
CString CGitStringList::RemoveHead()
{
	CString W_csReturn=m_lString.front();
	m_lString.pop_front();
	return W_csReturn;
}

CString CGitStringList::RemoveTail()
{
	CString W_csReturn=m_lString.back();
	m_lString.pop_back();
	return W_csReturn;
}


// add before head or after tail
CGitStringList::POSITION CGitStringList::AddHead(LPCTSTR newElement)
{
	return m_lString.insert(m_lString.begin(),newElement);
}

CGitStringList::POSITION CGitStringList::AddTail(LPCTSTR newElement)
{
	return m_lString.insert(m_lString.end(),newElement);
}


CGitStringList::POSITION CGitStringList::AddHead(const CString& newElement)
{
	return m_lString.insert(m_lString.begin(),newElement);
}

CGitStringList::POSITION CGitStringList::AddTail(const CString& newElement)
{
	return m_lString.insert(m_lString.end(),newElement);
}



// add another list of elements before head or after tail
void CGitStringList::AddHead(CGitStringList* pNewList)
{
	return m_lString.insert(m_lString.begin(),pNewList->m_lString.begin(),pNewList->m_lString.end());
}

void CGitStringList::AddTail(CGitStringList* pNewList)
{
	return m_lString.insert(m_lString.end(),pNewList->m_lString.begin(),pNewList->m_lString.end());
}


// remove all elements
void CGitStringList::RemoveAll()
{
	m_lString.clear();
}


// iteration
CGitStringList::POSITION CGitStringList::GetHeadPosition() const
{
	return m_lString.begin();
}

CGitStringList::POSITION CGitStringList::GetTailPosition() const
{
	return m_lString.end();
}

CString& CGitStringList::GetNext(POSITION& rPosition) // return *Position+
{
	CString& W_csReturn=*rPosition;
	++rPosition;
	return W_csReturn;
}

const CString& CGitStringList::GetNext(POSITION& rPosition) const // return *Position+
{
	CString& W_csReturn=*rPosition;
	++rPosition;
	return W_csReturn;
}

CString& CGitStringList::GetPrev(POSITION& rPosition) // return *Position-
{
	CString& W_csReturn=*rPosition;
	if(rPosition==m_lString.begin())
		rPosition=m_lString.end();
	else
		--rPosition;
	return W_csReturn;
}

const CString& CGitStringList::GetPrev(POSITION& rPosition) const // return *Position-
{
	CString& W_csReturn=*rPosition;
	if(rPosition==m_lString.begin())
		rPosition=m_lString.end();
	else
		--rPosition;
	return W_csReturn;
}


// getting/modifying an element at a given position
CString& CGitStringList::GetAt(POSITION position)
{
	return *position;
}

const CString& CGitStringList::GetAt(POSITION position) const
{
	return *position;
}

void CGitStringList::SetAt(POSITION pos, LPCTSTR newElement)
{
	*pos=newElement;
}


void CGitStringList::SetAt(POSITION pos, const CString& newElement)
{
	*pos=newElement;
}


void CGitStringList::RemoveAt(POSITION position)
{
	m_lString.erase(position);
}


// inserting before or after a given position
CGitStringList::POSITION CGitStringList::InsertBefore(POSITION position, LPCTSTR newElement)
{
	return m_lString.insert(position,newElement);
}

CGitStringList::POSITION CGitStringList::InsertAfter(POSITION position, LPCTSTR newElement)
{
	++position;
	return m_lString.insert(position,newElement);
}


CGitStringList::POSITION CGitStringList::InsertBefore(POSITION position, const CString& newElement)
{
	return m_lString.insert(position,newElement);
}

CGitStringList::POSITION CGitStringList::InsertAfter(POSITION position, const CString& newElement)
{
	++position;
	return m_lString.insert(position,newElement);
}



// helper functions (note: O(n) speed)
//POSITION CGitStringList::Find(LPCTSTR searchValue, POSITION startAfter = NULL) const
//{
//}

					// defaults to starting at the HEAD
					// return NULL if not found
//POSITION CGitStringList::FindIndex(INT_PTR nIndex) const
//{
//}

					// get the 'nIndex'th element (may return NULL)
