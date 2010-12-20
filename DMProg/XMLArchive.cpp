// XMLArchive.cpp: implementation of the CXMLArchive class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XMLArchive.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFile CXMLArchive::m_dummyFile;

CXMLArchive::CXMLArchive(const CString& fileName, UINT nMode, 
						 IStream* streamPtr /* = NULL*/,
						 CDocument* docPtr /*= NULL*/) :
CArchive(&m_dummyFile, nMode, 0, NULL),
m_fileName(fileName),
m_streamPtr(streamPtr)
{
	m_pDocument = docPtr;
	m_bForceFlat = FALSE;

	ASSERT(!fileName.IsEmpty() || streamPtr != NULL);

	// Get reference of XML document	
	try 
	{
		m_xmlDocPtr = MSXML::IXMLDOMDocumentPtr(__uuidof(MSXML::DOMDocument));
	}

	catch (_com_error e)
	{

	}

	if (m_xmlDocPtr == NULL)
	{
		::AfxMessageBox(_T("Can't get reference to XML parser, Ensure that msxml.dll is installed"));
		return;
	}
	
	// Don't want async
	m_xmlDocPtr->put_async(VARIANT_FALSE);

	if (IsLoading())
	{
		VARIANT_BOOL varResult = VARIANT_FALSE;

		try
		{
			if (m_streamPtr == NULL)
			{
				varResult = m_xmlDocPtr->load(_variant_t(fileName));
			}
			else
			{
				LARGE_INTEGER largeZero;

				largeZero.QuadPart = 0;

				m_streamPtr->Seek(largeZero, 0, NULL);
				varResult = m_xmlDocPtr->load(_variant_t(m_streamPtr));
			}
		}

		catch (_com_error e)
		{
			varResult = VARIANT_FALSE;
		}
	}
}



CXMLArchive::~CXMLArchive()
{
	CXMLArchive::Close();
}

void CXMLArchive::Close()
{

	HRESULT hr;

	try
	{
	if (IsStoring())
	{
		if (m_streamPtr == NULL)
		{
			BSTR fileNameBSTR = m_fileName.AllocSysString();
			hr = m_xmlDocPtr->save(fileNameBSTR);
			::SysFreeString(fileNameBSTR);
		}
		else
		{
			hr = m_xmlDocPtr->save(_variant_t(m_streamPtr));
		}
	}
	}

	catch (_com_error e)
	{
		ASSERT(FALSE);
	}
}

CXMLArchiveNode* CXMLArchive::GetCurrentNode()
{ 
	if (m_nodeList.size() <= 0)
	{
		return NULL;
	}
		
	return m_nodeList.top();
};

void CXMLArchive::RemoveNode(CXMLArchiveNode* xmlArchiveNodePtr)
{
	ASSERT(m_nodeList.size() > 0);
	ASSERT(xmlArchiveNodePtr == m_nodeList.top());

	delete m_nodeList.top();
	m_nodeList.pop();
}

CXMLArchiveNode* CXMLArchive::GetNode(LPCTSTR nodeNameStr)
{
	CString nodeName(nodeNameStr);

	try
	{
		BSTR nodeNameBSTR = nodeName.AllocSysString();
		
		MSXML::IXMLDOMNodePtr fatherNodePtr;
		
		if (m_nodeList.size() == 0)
		{
			fatherNodePtr = m_xmlDocPtr;
		}
		else
		{
			fatherNodePtr = m_nodeList.top()->m_nodePtr;
		}

		if (fatherNodePtr == NULL)
		{
			return NULL;
		}

		if (IsStoring())
		{
			// Archive is storing
			CXMLArchiveNode* xmlArchiveNodePtr = new CXMLArchiveNode(this, m_xmlDocPtr->createElement(nodeNameBSTR), fatherNodePtr);

			::SysFreeString(nodeNameBSTR);
			
			m_nodeList.push(xmlArchiveNodePtr);

			return xmlArchiveNodePtr;
		}

		// Archive is Loading
		MSXML::IXMLDOMNodeListPtr	nodeListPtr;
		MSXML::IXMLDOMNodePtr		nodePtr;

		// If child node list is not empty, we are loading using the tags to
		// create CObject derived objects (CArray<Cobject* CObject*>, use child list
		
		if (m_nodeList.size() > 0)
		{
			CXMLArchiveNode* xmlNodePtr = m_nodeList.top();
			nodeListPtr = xmlNodePtr->m_childNodeListPtr;

			if (nodeListPtr != NULL && nodeListPtr->length > 0)
			{
				int childIndex = xmlNodePtr->m_childIndex;

				if (childIndex < nodeListPtr->length)
				{
					nodeListPtr->get_item(childIndex, &nodePtr);

					CXMLArchiveNode* xmlArchiveNodePtr = new CXMLArchiveNode(this, nodePtr, m_xmlDocPtr);

					m_nodeList.push(xmlArchiveNodePtr);

					::SysFreeString(nodeNameBSTR);
		
					return xmlArchiveNodePtr;
				}

				ASSERT(FALSE);
			}
		}

		// Get all nodes with this name
		if (MSXML::IXMLDOMDocumentPtr(fatherNodePtr) != NULL)
		{
			// First level node in document
			ASSERT(!nodeName.IsEmpty());
			nodeListPtr = MSXML::IXMLDOMDocumentPtr(fatherNodePtr)->getElementsByTagName(nodeNameBSTR);
		}
		else
		{
			// Get node with desired name
			nodeListPtr = MSXML::IXMLDOMElementPtr(fatherNodePtr)->getElementsByTagName(nodeNameBSTR);
		}

		::SysFreeString(nodeNameBSTR);

		int childIndex = 0;
		if (m_nodeList.size() > 0)
		{
			childIndex = m_nodeList.top()->m_childIndex;
		}

		if (childIndex < nodeListPtr->length)
		{
			nodeListPtr->get_item(childIndex, &nodePtr);
		}

		CXMLArchiveNode* xmlArchiveNodePtr = new CXMLArchiveNode(this, nodePtr, m_xmlDocPtr);

		m_nodeList.push(xmlArchiveNodePtr);

		return xmlArchiveNodePtr;
	}

	catch (...)
	{

	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Class CXMLArchiveNode
//

// Cache of objects created
CMapStringToPtr		CXMLArchiveNode::m_classMap;

CXMLArchiveNode::CXMLArchiveNode(CXMLArchive* archivePtr, MSXML::IXMLDOMElementPtr newNodePtr, MSXML::IXMLDOMNodePtr fatherNodePtr) :
m_archivePtr(archivePtr), 
m_nodePtr(newNodePtr),
m_fatherNodePtr(fatherNodePtr)
{
	m_childIndex = 0;
}

CXMLArchiveNode::~CXMLArchiveNode()
{
}

void CXMLArchiveNode::Close()
{
	CXMLArchive* archivePtr = m_archivePtr;

	if (archivePtr->IsStoring())
	{
		m_fatherNodePtr->appendChild(m_nodePtr);
	}

	archivePtr->RemoveNode(this);
}

CString CXMLArchiveNode::GetChildName(int childIndex)
{
	ASSERT(m_nodePtr != NULL);

	if (m_nodePtr == NULL)
	{
		return CString();
	}

	ASSERT(m_childNodeListPtr != NULL && childIndex < m_childNodeListPtr->length);

	MSXML::IXMLDOMNodePtr nodePtr;
	BSTR bstr;

	m_childNodeListPtr->get_item(childIndex, &nodePtr);

	nodePtr->get_nodeName(&bstr);

	CString childName(bstr);

	::SysFreeString(bstr);

	return childName;

}

int CXMLArchiveNode::GetNoChildren()
{
	if (m_nodePtr == NULL)
	{
		return 0;
	}

	if (m_childNodeListPtr == NULL)
	{
		// Get all nodes with this name
		m_childNodeListPtr = m_nodePtr->childNodes;
	}  

	return m_childNodeListPtr->length;
}

MSXML::IXMLDOMNodePtr CXMLArchiveNode::CreateDataNode(LPCTSTR nodeName, CString nodeText)
{
	// Check if there are trailing spaces
	if (nodeText.Right(1) == _T(" "))
	{
#if _MFC_VER >= 0x0700
		nodeText.Append(_T("_TRSP#@"));
#else
		nodeText += _T("_TRSP#@");
#endif
	}

	BSTR nodeNameBSTR = CString(nodeName).AllocSysString();
	BSTR nodeValueBSTR = nodeText.AllocSysString();

	MSXML::IXMLDOMNodePtr newNodePtr = m_archivePtr->m_xmlDocPtr->createElement(nodeNameBSTR);
	
	newNodePtr->put_text(nodeValueBSTR);

	m_nodePtr->appendChild(newNodePtr);

	::SysFreeString(nodeNameBSTR);
	::SysFreeString(nodeValueBSTR);
	return newNodePtr;
}

MSXML::IXMLDOMNodePtr CXMLArchiveNode::GetDataNode(LPCTSTR nodeName, CString& nodeText)
{
	BSTR nodeNameBSTR = CString(nodeName).AllocSysString();

	// Archive is Loading
	MSXML::IXMLDOMNodeListPtr nodeListPtr;
	MSXML::IXMLDOMNodePtr dataNodePtr;

	// Get all nodes with this name
	CXMLArchiveNode* nodePtr = m_archivePtr->GetCurrentNode();

	if (nodePtr == NULL)
	{
		return NULL;
	}

	nodeListPtr = nodePtr->m_nodePtr->getElementsByTagName(nodeNameBSTR);

	::SysFreeString(nodeNameBSTR);

	BSTR bstr;
	nodeListPtr->get_item(m_childIndex, &dataNodePtr);

	if (NULL == dataNodePtr)
	{
		return NULL;
	}

	dataNodePtr->get_text(&bstr);
	nodeText = CString(bstr);

	// Check if there are trailing spaces
	if (nodeText.Right(7) == _T("_TRSP#@"))
	{
		nodeText = nodeText.Left(nodeText.GetLength() - 7);
	}
	
	return dataNodePtr;
}


void CXMLArchiveNode::DataNode(LPCTSTR attrName, CString& attrValue)
{
	if (m_nodePtr == NULL)
	{
		return;
	}

	if (m_archivePtr->IsStoring())
	{
		CreateDataNode(attrName, attrValue);
	}
	else
	{
		if (GetDataNode(attrName, attrValue) == NULL)
		{ 
			// No node, do not assign anything
			return;
		}
	}
}

void CXMLArchiveNode::DataNode(LPCTSTR attrName, CTimeSpan& attrValue)
{
	if (m_nodePtr == NULL)
	{
		return;
	}

	if (m_archivePtr->IsStoring())
	{
		CString attrText;
		
		attrText = attrValue.Format(_T("%D %H:%M:%S"));

		CreateDataNode(attrName, attrText);
	}
	else
	{
		CString tmpString;

		if (GetDataNode(attrName, tmpString) == NULL)
		{
			return;
		}

		int d=0,h=-1,mm=-1,s=-1;

		attrValue = CTimeSpan(0);

		int noFields = sscanf_s(tmpString,  _T("%d %d:%d:%d"), &d, &h, &mm, &s);
	
		if (noFields != 4)
		{
			return;
		}

		attrValue = CTimeSpan(d, h, mm, s);
	}
}

void CXMLArchiveNode::DataNode(LPCTSTR attrName, CTime& attrValue)
{
	if (m_nodePtr == NULL)
	{
		return;
	}

	if (m_archivePtr->IsStoring())
	{
		CString attrText;
		
		attrText = attrValue.Format(_T("%d/%m/%Y %H:%M:%S"));

		CreateDataNode(attrName, attrText);
	}
	else
	{
		CString tmpString;

		if (GetDataNode(attrName, tmpString) == NULL)
		{
			return;
		}

		int d=0,m=0,y=0,h=-1,mm=-1,s=-1;

		attrValue = CTime(0);

		int noFields = sscanf_s(tmpString, _T("%d/%d/%d %d:%d:%d"), &d, &m, &y, &h, &mm, &s);
	
		if (noFields != 6)
		{
			return;
		}

		attrValue = CTime(y, m, d, h, mm, s);
	}
}

void CXMLArchiveNode::DataNode(LPCTSTR attrName, bool& attrValue)
{
	if (m_nodePtr == NULL)
	{
		return;
	}

	if (m_archivePtr->IsStoring())
	{
		CString attrText;
		
		attrText = attrValue ? _T("true") : _T("false");

		CreateDataNode(attrName, attrText);
	}
	else
	{
		CString tmpString;

		if (GetDataNode(attrName, tmpString) == NULL)
		{
			return;
		}

		attrValue = tmpString == _T("true") ? true : false;
	}
}

void CXMLArchiveNode::DataNode(LPCTSTR attrName, double& attrValue)
{
	if (m_nodePtr == NULL)
	{
		return;
	}

	if (m_archivePtr->IsStoring())
	{
		CString attrText;

		attrText.Format(_T("%.2f"), attrValue);

		CreateDataNode(attrName, attrText);
	}
	else
	{
		CString tmpString;

		if (GetDataNode(attrName, tmpString) == NULL)
		{
			return;
		}

		attrValue = atof(tmpString);
	}
}

void CXMLArchiveNode::DataNode(LPCTSTR attrName, int& attrValue)
{
	if (m_nodePtr == NULL)
	{
		return;
	}

	if (m_archivePtr->IsStoring())
	{
		CString attrText;

		attrText.Format(_T("%d"), attrValue);

		CreateDataNode(attrName, attrText);
	}
	else
	{
		CString tmpString;

		if (GetDataNode(attrName, tmpString) = NULL)
		{
			return;
		}

		attrValue = atoi(tmpString);
	}
}

void CXMLArchiveNode::DataNode(LPCTSTR attrName, LONG& attrValue)
{
	int tmpValue = attrValue;
	DataNode(attrName, (int&)tmpValue);
	attrValue = tmpValue;
}

void CXMLArchiveNode::DataNode(LPCTSTR attrName, BYTE& attrValue)
{
	int tmpValue = attrValue;
	DataNode(attrName, (int&)tmpValue);
	attrValue = tmpValue;
}

void CXMLArchiveNode::DataNode(LPCTSTR attrName, UINT& attrValue)
{
	int tmpValue = attrValue;
	DataNode(attrName, (int&)tmpValue);
	attrValue = tmpValue;
}

void CXMLArchiveNode::DataNode(LPCTSTR attrName, CStringArray& stringArray)
{
	if (m_nodePtr == NULL)
	{
		return;
	}

    CXMLArchiveNode* curNodePtr = m_archivePtr->GetNode(attrName);

	if (m_archivePtr->IsStoring())
	{
		for (int arrayIndex = 0; arrayIndex < stringArray.GetSize(); arrayIndex++)
		{
			curNodePtr->DataNode(_T("CString"), stringArray[arrayIndex]);
		}
	}
	else
	{
		// Loading
		stringArray.RemoveAll();

		int numberObjects = curNodePtr->GetNoChildren();

		for (m_childIndex = 0; m_childIndex < numberObjects; m_childIndex++)
		{
			CString attrValue;

			DataNode(_T("CString"), attrValue);

			stringArray.Add(attrValue);
		}
	}

	m_childIndex = 0;
	curNodePtr->Close();
}

void CXMLArchiveNode::DataNode(LPCTSTR attrName, CDWordArray& wordArray)
{
	if (m_nodePtr == NULL)
	{
		return;
	}

    CXMLArchiveNode* curNodePtr = m_archivePtr->GetNode(attrName);

	if (m_archivePtr->IsStoring())
	{
		for (int arrayIndex = 0; arrayIndex < wordArray.GetSize(); arrayIndex++)
		{
			int tempInt = wordArray[arrayIndex];
			curNodePtr->DataNode(_T("int"), tempInt);
		}
	}
	else
	{
		// Loading
		wordArray.RemoveAll();

		int numberObjects = curNodePtr->GetNoChildren();

		for (m_childIndex = 0; m_childIndex < numberObjects; m_childIndex++)
		{
			int attrValue;

			DataNode(_T("int"), attrValue);

			wordArray.Add(attrValue);
		}
	}

	m_childIndex = 0;
	curNodePtr->Close();
}

void CXMLArchiveNode::DataNode(LPCTSTR attrName, CMapStringToPtr& pointerMap)
{
	CString keyString;
	int		tempInt;

	if (m_nodePtr == NULL)
	{
		return;
	}

    CXMLArchiveNode* curNodePtr = m_archivePtr->GetNode(attrName);

	if (m_archivePtr->IsStoring())
	{
		for	(POSITION pos = pointerMap.GetStartPosition(); pos != NULL;)
		{
			pointerMap.GetNextAssoc(pos, keyString, (void*&)tempInt);
			curNodePtr->DataNode(_T("keyString"), keyString);
			curNodePtr->DataNode(_T("int"), tempInt);
		}
	}
	else
	{
		// Loading
		pointerMap.RemoveAll();

		int numberObjects = curNodePtr->GetNoChildren();

		for (int i = 0; i < numberObjects; m_childIndex++, i+=2)
		{
			DataNode(_T("keyString"), keyString);
			
			DataNode(_T("int"), tempInt);

			pointerMap.SetAt(keyString, (void*&) tempInt);
		}
	}
	
	m_childIndex = 0;
	curNodePtr->Close();
}

void CXMLArchiveNode::DataNode(LPCTSTR attrName, CMapStringToString& stringMap)
{
	CString keyString;
	CString	tempString;

	if (m_nodePtr == NULL)
	{
		return;
	}

    CXMLArchiveNode* curNodePtr = m_archivePtr->GetNode(attrName);

	if (m_archivePtr->IsStoring())
	{
		for	(POSITION pos = stringMap.GetStartPosition(); pos != NULL;)
		{
			stringMap.GetNextAssoc(pos, keyString, tempString);
			curNodePtr->DataNode(_T("keyString"), keyString);
			curNodePtr->DataNode(_T("string"), tempString);
		}
	}
	else
	{
		// Loading
		stringMap.RemoveAll();

		int numberObjects = curNodePtr->GetNoChildren();

		for (int i = 0; i < numberObjects; m_childIndex++, i+=2)
		{
			DataNode(_T("keyString"), keyString);
			DataNode(_T("string"), tempString);

			stringMap.SetAt(keyString, tempString);
		}
	}
	
	m_childIndex = 0;
	curNodePtr->Close();
}

void CXMLArchiveNode::DataNode(LPCTSTR attrName, CByteArray& byteArray)
{
	if (m_nodePtr == NULL)
	{
		return;
	}

    CXMLArchiveNode* curNodePtr = m_archivePtr->GetNode(attrName);

	if (m_archivePtr->IsStoring())
	{
		for (int arrayIndex = 0; arrayIndex < byteArray.GetSize(); arrayIndex++)
		{
			int tempInt = byteArray[arrayIndex];
			curNodePtr->DataNode(_T("byte"), tempInt);
		}
	}
	else
	{
		// Loading
		byteArray.RemoveAll();

		int numberObjects = curNodePtr->GetNoChildren();

		for (m_childIndex = 0; m_childIndex < numberObjects; m_childIndex++)
		{
			int attrValue;

			DataNode(_T("byte"), attrValue);

			byteArray.Add(attrValue);
		}
	}

	m_childIndex = 0;
	curNodePtr->Close();
}

// Loads into existing objects
void CXMLArchiveNode::DataNode(LPCTSTR attrName, CObject& object)
{
	m_archivePtr->GetNode(attrName);
	object.Serialize(*m_archivePtr);
	m_archivePtr->GetCurrentNode()->Close();
}

// Creates new object when loading
void CXMLArchiveNode::DataNode(LPCTSTR attrName, CObject*& objectPtr)
{
	if (m_archivePtr->IsStoring() && objectPtr == NULL)
	{
		return;
	}

    CXMLArchiveNode* curNodePtr = m_archivePtr->GetNode(attrName);

	if (curNodePtr == NULL)
	{
		return;
	}

	if (m_archivePtr->IsStoring())
	{
		objectPtr->Serialize(*m_archivePtr);
	}
	else
	{
		objectPtr = NULL;

		do
		{
			// Dummy loop, executes only once
			int numberObjects = curNodePtr->GetNoChildren();

			if (numberObjects == 0)
			{
				break;
			}

			CString childNodeName = curNodePtr->GetChildName(0);

			objectPtr = CreateObject(childNodeName);

			if (objectPtr == NULL)
			{
				ASSERT(FALSE);
				break;
			}

			objectPtr->Serialize(*m_archivePtr);

			break;
		}
		while(FALSE); // C4702 OK
	}

	m_childIndex = 0;
	curNodePtr->Close();
}

CObject* CXMLArchiveNode::CreateObject(const CString& className)
{
	CRuntimeClass* pClass = NULL;

	// First see if the CRuntimeClass object is cached
	if (!m_classMap.Lookup(className, (void*&)pClass))
	{
		// Search app specific classes
		AFX_MODULE_STATE* pModuleState = AfxGetModuleState();

		for (pClass = pModuleState->m_classList; pClass != NULL; pClass = pClass->m_pNextClass)
		{
			if (className == pClass->m_lpszClassName)
			{
				m_classMap[className] = (void*&) pClass;
				break;
			}
		}
	}

	if (pClass == NULL)
	{
		TRACE1("You did not declare the class %s DECLARE_XMLSERIAL", (const char *) className); 
		ASSERT(FALSE);
		return NULL;
	}

	return pClass->CreateObject();
}

