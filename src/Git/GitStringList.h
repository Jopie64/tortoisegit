#pragma once

#include <list>

class CGitStringList : public CObject
{

//	DECLARE_SERIAL(CGitStringList)

public:
	typedef std::list<CString> ClString;
	typedef ClString::iterator POSITION;

// Construction
	/* explicit */ CGitStringList(INT_PTR nBlockSize = 10);

// Attributes (head and tail)
	// count of elements
	INT_PTR GetCount() const;
	INT_PTR GetSize() const;
	BOOL IsEmpty() const;

	// peek at head or tail
	CString& GetHead();
	const CString& GetHead() const;
	CString& GetTail();
	const CString& GetTail() const;

// Operations
	// get head or tail (and remove it) - don't call on empty list!
	CString RemoveHead();
	CString RemoveTail();

	// add before head or after tail
	POSITION AddHead(LPCTSTR newElement);
	POSITION AddTail(LPCTSTR newElement);

	POSITION AddHead(const CString& newElement);
	POSITION AddTail(const CString& newElement);


	// add another list of elements before head or after tail
	void AddHead(CGitStringList* pNewList);
	void AddTail(CGitStringList* pNewList);

	// remove all elements
	void RemoveAll();

	// iteration
	POSITION GetHeadPosition() const;
	POSITION GetTailPosition() const;
	CString& GetNext(POSITION& rPosition); // return *Position++
	const CString& GetNext(POSITION& rPosition) const; // return *Position++
	CString& GetPrev(POSITION& rPosition); // return *Position--
	const CString& GetPrev(POSITION& rPosition) const; // return *Position--

	// getting/modifying an element at a given position
	CString& GetAt(POSITION position);
	const CString& GetAt(POSITION position) const;
	void SetAt(POSITION pos, LPCTSTR newElement);

	void SetAt(POSITION pos, const CString& newElement);

	void RemoveAt(POSITION position);

	// inserting before or after a given position
	POSITION InsertBefore(POSITION position, LPCTSTR newElement);
	POSITION InsertAfter(POSITION position, LPCTSTR newElement);

	POSITION InsertBefore(POSITION position, const CString& newElement);
	POSITION InsertAfter(POSITION position, const CString& newElement);


	// helper functions (note: O(n) speed)
//	POSITION Find(LPCTSTR searchValue, POSITION startAfter = m_lString.end()) const;
						// defaults to starting at the HEAD
						// return NULL if not found
//	POSITION FindIndex(INT_PTR nIndex) const;
						// get the 'nIndex'th element (may return NULL)

// Implementation
protected:
	//Dirty fix for const functions: mutable
	mutable ClString	m_lString;


public:
	~CGitStringList();

//	void Serialize(CArchive&);
//#ifdef _DEBUG
//	void Dump(CDumpContext&) const;
//	void AssertValid() const;
//#endif
	// local typedefs for class templates
	typedef CString BASE_TYPE;
	typedef LPCTSTR BASE_ARG_TYPE;
};

//typedef CGitStringList CStringList;