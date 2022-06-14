#pragma once
#include "XML_Manager.h"
class MD_S411
{
public:
	MD_S411(std::wstring NameSuffix);


public:
	Xml_Elem_ptr *MyMainBlock;
	bool MakeMeta(std::wstring XmlPath);//Our main function-  we pass params and get xml file on output
protected:
	std::wstring m_PathInputIni;
	void GetIniPath(std::wstring NameOfFile);
	bool AttachVariables();
protected: //MyData
	std::wstring Something_Test=L"Vasiliy";
};

