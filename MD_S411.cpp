#include "MD_S411.h"
#include <Windows.h>

MD_S411::MD_S411(std::wstring NameSuffix)
{

	std::wstring IniName = L"ProdDsr";
	if (!NameSuffix.empty()) {
		IniName += L'_' + NameSuffix;
	}
	IniName += L".ini";
	GetIniPath(IniName);
	XML_Parser MyParser;// m_PathInputIni
	bool res = MyParser.FillFromXML(IniName);
	if (!res)
		return;
	MyMainBlock = MyParser.GetResultElement();
	res = AttachVariables();
	if (!res)
		return;

}

bool MD_S411::MakeMeta(std::wstring XmlPath)
{
	if (!MyMainBlock)
		return false;

	bool res = CreateXML(XmlPath, MyMainBlock);
	return res;
}

void MD_S411::GetIniPath(std::wstring NameOfFile)
{


	//m_IniPath
	wchar_t* buff = new wchar_t[MAX_PATH];
	GetModuleFileNameW(0, buff, MAX_PATH);
	std::wstring dir_str(buff);
	delete[] buff;
	auto pos = dir_str.rfind(L"\\");

	if (pos != std::wstring::npos) {
		m_PathInputIni = dir_str.substr(0, pos+1)+ NameOfFile;
	}
	
}

bool MD_S411::AttachVariables()
{
	std::wstring Name = L"ec:S100_ExchangeCatalogue\\ec:contact\\gmd:CI_ResponsibleParty\\gmd:individualName\\gco:CharacterString";
	if(!MyMainBlock->AttachByPath(&Something_Test, Name,'\\'))
		return false;
	return true;
}
