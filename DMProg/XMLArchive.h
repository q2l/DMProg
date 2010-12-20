// XMLArchive.h: interface for the CXMLArchive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLARCHIVE_H__9D849A31_6DC4_11D6_BC05_0000E878C400__INCLUDED_)
#define AFX_XMLARCHIVE_H__9D849A31_6DC4_11D6_BC05_0000E878C400__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stack>


#define XMLCLASSNODE { CXMLArchive& xmlArchive = static_cast<CXMLArchive&>(ar); \
						xmlArchive.GetNode(GetActualClass()->m_lpszClassName);
#define XMLDATA(attrName) { CXMLArchiveNode* nodePtr = xmlArchive.GetCurrentNode();\
	if (nodePtr != NULL) {nodePtr->DataNode(#attrName, attrName);}}
#define XMLINTDATA(attrName) { CXMLArchiveNode* nodePtr = xmlArchive.GetCurrentNode();\
	if (nodePtr != NULL) {nodePtr->DataNode(#attrName, (int&)(attrName));}}
#define XMLENDNODE { CXMLArchiveNode* nodePtr = xmlArchive.GetCurrentNode();\
	if (nodePtr != NULL) {nodePtr->Close(); }}}


class CXMLArchiveNode;

class CXMLArchive : public CArchive  
{
	std::stack  <CXMLArchiveNode*> m_nodeList;
	static CFile	m_dummyFile;
	IStream*		m_streamPtr;

public:
	CXMLArchive(const CString& fileName, UINT nMode, 
		        IStream* streamPtr = NULL,
				CDocument* docPtr = NULL);
	virtual ~CXMLArchive();
	void	Close();
	CXMLArchiveNode*	GetNode(LPCTSTR nodeNameStr);	
	void				RemoveNode(CXMLArchiveNode* xmlArchiveNodePtr);
	CXMLArchiveNode*	GetCurrentNode();

// Attributes
	const CString				m_fileName;
	MSXML::IXMLDOMDocumentPtr	m_xmlDocPtr;
};

class CXMLArchiveNode 
{
	friend class CXMLArchive;
	
	// Private constructor, can only be constructed by CXMLArchive
	CXMLArchiveNode(CXMLArchive* archivePtr, 	MSXML::IXMLDOMElementPtr newNodePtr, MSXML::IXMLDOMNodePtr fatherNodePtr);
protected:
	// Cache of RUNTIME_CLASS created, to speed up createion
	static CMapStringToPtr			m_classMap;
	MSXML::IXMLDOMElementPtr const	m_nodePtr;
	MSXML::IXMLDOMNodePtr const		m_fatherNodePtr;
	CXMLArchive* const				m_archivePtr;
	MSXML::IXMLDOMNodeListPtr		m_childNodeListPtr;
	int								m_childIndex;

public:
	virtual ~CXMLArchiveNode();
	MSXML::IXMLDOMNodePtr CreateDataNode(LPCTSTR attrName, CString attrValue);
	MSXML::IXMLDOMNodePtr CXMLArchiveNode::GetDataNode(LPCTSTR nodeName, CString& nodeText);
	int		GetNoChildren();
	CString GetChildName(int childIndex);
	int		GetNextChildIndex() { return m_childIndex++; };
	void	Close();
	void DataNode(LPCTSTR attrName, CTime& attrValue);
	void DataNode(LPCTSTR attrName, CTimeSpan& attrValue);
	void DataNode(LPCTSTR attrName, CString& attrValue);
	void DataNode(LPCTSTR attrName, bool& attrValue);
	void DataNode(LPCTSTR attrName, int& attrValue);
	void DataNode(LPCTSTR attrName, double& attrValue);
	void DataNode(LPCTSTR attrName, LONG& attrValue);
	void DataNode(LPCTSTR attrName, BYTE& attrValue);
	void DataNode(LPCTSTR attrName, UINT& attrValue);
	void DataNode(LPCTSTR attrName, CMapStringToPtr& pointerMap);
	void DataNode(LPCTSTR attrName, CMapStringToString& stringMap);
	void DataNode(LPCTSTR attrName, CStringArray& stringArray);
	void DataNode(LPCTSTR attrName, CByteArray& byteArray);
	void DataNode(LPCTSTR attrName, CDWordArray& wordArray);

	// Loads into existing objects
	void DataNode(LPCTSTR attrName, CObject& object);

	// Creates new object when loading
	void DataNode(LPCTSTR attrName, CObject*& objectPtr);

	// Creates new object from node name
	static CObject*  CreateObject(const CString& className);

};


#endif // !defined(AFX_XMLARCHIVE_H__9D849A31_6DC4_11D6_BC05_0000E878C400__INCLUDED_)
