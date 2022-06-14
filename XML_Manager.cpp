#include "XML_Manager.h"
#include <iostream>
#include <memory>
#define forever for(;;)
Xml_Elem_ptr::~Xml_Elem_ptr()
{
	//std::string Output(Tag.begin(), Tag.end());
	//std::cout << std::endl << Output;
}
bool Xml_Elem_ptr::RenameTag(std::wstring NewTagName)
{
	Tag = NewTagName;
	return true;
}

void Xml_Elem_ptr::AttachToVariable(void* VariableRef)
{
	if (m_bIsInitByText) {
		m_bIsInitByText = false;
		delete Value;
	}
	Value = VariableRef;
}

bool Xml_Elem_ptr::AttachByPath(void* VariableRef, std::wstring PathOfVariable, wchar_t Separator)
{
	auto VectorOfPathes = GetVecFromPath(PathOfVariable, Separator);
	if (VectorOfPathes.empty())
		return false;
	int CurrentLevel = 0;
	if (VectorOfPathes[0] == Tag) //It means, that we ignore zeroes level, if paths starts from this
		CurrentLevel++;
	bool res = AttachByVec(VariableRef, VectorOfPathes, CurrentLevel);

	return res;
}

bool Xml_Elem_ptr::AttachByVec(void* VariableRef, std::vector<std::wstring>& VecOfPathes, int CurLevel)
{
	std::wstring& CurrentTagName = VecOfPathes[CurLevel];
	int SizeOfVec = VecOfPathes.size();
	int Counter = 0;
	for (auto it = Inheritors.begin(); it != Inheritors.end(); it++) {
		Counter++;
		auto NameOfInher = (*it)->Tag;
		if (NameOfInher == CurrentTagName) {
			if (CurLevel == (SizeOfVec-1)) { //If it is our last element- return true
				(*it)->AttachToVariable(VariableRef);
				return true;
			}
			else
			{
				bool res = (*it)->AttachByVec(VariableRef, VecOfPathes, CurLevel+1);
				if (res)
					return true;
			}
		}
	}
	return false;
}

std::wstring Xml_Elem_ptr::GetSchema(int CurrentLevel)
{

	std::wstring Result; //Result
	if (Tag.empty())
		return Result;

	bool IsValidValue = !GetStrValue().empty();
	if (IsTagInside()) {
		if (IsValidValue) { //If TagName and Value are not empty
			Result = L" " + Tag + L"=\"" + GetStrValue() + L"\"";
		}
		return Result;
	}
	std::wstring NewString = L"\n";
	for (int Tabulations = 0; Tabulations < CurrentLevel; Tabulations++) {
		NewString += '\t';
	}
	Result += NewString +L"<" + Tag; //Open tag
	for (auto it = Inheritors.begin(); it != Inheritors.end(); it++) { //Firstly Fill with Inline tags
		if ((*it)->IsTagInside()) {
			Result += (*it)->GetSchema(CurrentLevel+1);
		}
	};
	Result += L">"; //Open tag - Just close it

	for (auto it = Inheritors.begin(); it != Inheritors.end(); it++) { //Secondly Fill with usual tags
		if (!(*it)->IsTagInside()) {

			Result += (*it)->GetSchema(CurrentLevel+1);
		}
	};
	if (IsValidValue) {
		Result += GetStrValue();
	}
	else {
		Result += NewString;
	}
	Result +=  L"</" + Tag + L">"; //Don't
	 //Close Tag


	return Result;
}

bool Xml_Elem_ptr::PutElem(Xml_Elem_ptr* ChildElem)
{
	bool res = false;

	if (res) {
		IsInside = false; //element can not have inheritors and be inline tag
	}
	Inheritors.push_back(ChildElem); 
	return res;
	
}

void Xml_Elem_ptr::Clear()
{
	for (auto it = Inheritors.begin(); it != Inheritors.end(); it++) {
		if ((*it) != NULL) {
			(*it)->Clear();
			delete *it;
			*it = NULL;
		}
	}
}

void Xml_Elem_ptr::MakeInside()
{

	if (Inheritors.empty()) //We can not make it inside, if our param has Inheritors
		IsInside = true;
}

Xml_Elem_ptr* Xml_Elem_ptr::GetFirstInheritor()
{
	if (!Inheritors.empty()) {
		return Inheritors[0];
	}
	return NULL;
}

void Xml_Elem_ptr::SetValue(wchar_t* NameOfNewValue)
{
	
	if (Value)
		delete Value;
	std::wstring *ValueWstring = new std::wstring(NameOfNewValue);

	AttachToVariable(ValueWstring);
	m_bIsInitByText = true; //Don't Forger to set flag, that we didn't attach it to variable

}

std::wstring Xml_Elem_ptr::GetStrValue()
{
	if (!Value)
		return std::wstring();

	return *((std::wstring*)Value);
}




Xml_Elem_ptr_string::Xml_Elem_ptr_string(std::wstring NewTagName)
{
	RenameTag(NewTagName);
}

Xml_Elem_ptr_string::Xml_Elem_ptr_string()
{
}

std::wstring Xml_Elem_ptr_string::GetStrValue()
{
	if (!Value)
		return std::wstring();
	
	return *((std::wstring*)Value);
}

Xml_Elem_ptr_Numb::Xml_Elem_ptr_Numb()
{
}

Xml_Elem_ptr_Numb::Xml_Elem_ptr_Numb(std::wstring NewTagName)
{
	RenameTag(NewTagName);
}

std::wstring Xml_Elem_ptr_Numb::GetStrValue()
{
	long long ResultValue = *((long long*)Value);
	return std::to_wstring(ResultValue);
}


Xml_Elem_ptr_NumbSet::Xml_Elem_ptr_NumbSet()
{
}

Xml_Elem_ptr_NumbSet::Xml_Elem_ptr_NumbSet(std::wstring NewTagName)
{
	
		RenameTag(NewTagName);
	
}

std::wstring Xml_Elem_ptr_NumbSet::GetStrValue()
{
	std::wstring Result;
	bool FirstSymbPassed = false;
	std::vector<int> *MyVecOfValues = ((std::vector<int>*)Value);
	for (auto it = MyVecOfValues->begin(); it != MyVecOfValues->end(); it++) {
		if (FirstSymbPassed)
			Result += separator;
		else
			FirstSymbPassed = true;
		Result += std::to_wstring(*it);
	}
	return Result;
}



bool CreateXML(std::wstring PathOfFile, Xml_Elem_ptr* Element, std::wstring IncludingPart)
{
	std::wstring StringForOutput = Element->GetSchema(0);
	if (StringForOutput.empty())
		return false;
	return CreateXML(PathOfFile, StringForOutput, IncludingPart);




}



bool CreateXML(std::wstring PathOfile, std::wstring StringForWriting, std::wstring IncludingPart)
{
	FILE* outFile;
	if (1) {
		bool res = _wfopen_s(&outFile, PathOfile.c_str(), L"w+,ccs=UTF-8");
		if (!outFile)
			return false;


		fwrite(IncludingPart.c_str(), wcslen(IncludingPart.c_str()) * sizeof(wchar_t), 1, outFile);

		fwrite(StringForWriting.c_str(), wcslen(StringForWriting.c_str()) * sizeof(wchar_t), 1, outFile);

		fclose(outFile);
	}
	return true;
}

std::vector<std::wstring> GetVecFromPath(std::wstring& TreePath, wchar_t Separator)
{
	int size = TreePath.size();
	wchar_t* wBuff = new wchar_t[_MAX_PATH];
	std::vector<std::wstring> ResultVec;
	if (!size)
		return ResultVec;
	int Counter = 0;
	for (auto it = TreePath.begin(); it != TreePath.end(); it++) {
		if (*it == Separator) {
			if (Counter > 0) {
				wBuff[Counter] = 0;
				std::wstring SubWstr = wBuff;
				ResultVec.push_back(SubWstr);
			}
			Counter = 0;
		}
		else 
			wBuff[Counter++] = *it;
	}
	if (Counter > 1) {
		wBuff[Counter] = 0;
		std::wstring SubWstr = wBuff;
		Counter = 0;
		ResultVec.push_back(SubWstr);
	}
	return ResultVec;
}

XML_Parser::XML_Parser()
{
}

XML_Parser::~XML_Parser()
{
	if (m_MainBuff) {
		delete[] m_MainBuff;
		m_MainBuff = 0;
	}
	if (m_SubBuff) {
		delete[]m_SubBuff;
		m_SubBuff = 0;
	}

	if (m_xml_Result)
	{
		delete m_xml_Result;
		m_xml_Result = NULL;
	}
}

bool XML_Parser::FillFromXML(std::wstring PathToXml)
{
	m_ifstr.open(PathToXml, std::ios::in);
	if (!m_ifstr.is_open())
		return false;
	InitVariables();
	m_xml_Result = new Xml_Elem_ptr;
	int Result = FillThisElement(m_xml_Result,true);
	if (Result == -1) {
		m_xml_Result->Clear();
		delete m_xml_Result;
		m_xml_Result = 0;
		return false;
	}
	return true;
}

Xml_Elem_ptr* XML_Parser::GetResultElement()
{

	Xml_Elem_ptr* Result = m_xml_Result->GetFirstInheritor();
	return Result;
}

void XML_Parser::InitVariables()
{
	if(m_xml_Result)
	{
		delete[] m_xml_Result;
		m_xml_Result = NULL;
	}
	if(!m_MainBuff)
		m_MainBuff = new wchar_t[_MAX_PATH];

	if(!m_SubBuff)
		m_SubBuff = new wchar_t[_MAX_PATH];

	//Create Buffers, if they are not created yet
}


int XML_Parser::FillThisElement(Xml_Elem_ptr* CurElem, bool IsBase)
{

	int ResultOfParsing=0;
	Xml_Elem_ptr* CurrentElement;
	do {
		int nResult = XML_Parser::GoToNextTag(); //Go Find First '<' Symbol
		if (nResult == -1) //If there is a mistake
			return -1;

		if (nResult) { //If we meet Any Symbols 
		
			m_MainBuff[nResult] = 0;
			CurElem->SetValue(m_MainBuff);
		}

		CurrentElement = ParseTag(ResultOfParsing, CurElem); //Parse, untill we meet first '>' symbol
		if (ResultOfParsing == -1) {
			//if there is an error - mark it
			return false;
		}
		if (ResultOfParsing == 1) {
			//If we found an open tag, start working with this element
			int FillResult = FillThisElement(CurrentElement, false);
			if (FillResult == -1) {
				return -1;
			}
		}
		if ((ResultOfParsing == 1) && (IsBase))
			break; //Tree Always starts from one element
	} while ( (ResultOfParsing)); //Parse, untill we meet

	return 1; 
}

int XML_Parser::GoToNextTag()
{
	int Counter = 0;
	wchar_t CurrentCharacter;// = MyStream.get();
	while (true)
	{
		if (m_ifstr.eof()) {
			return 0;
		}
		CurrentCharacter = m_ifstr.get();
		if ((CurrentCharacter != '\t') && (CurrentCharacter != '\n') && (CurrentCharacter != ' ')) {
			break;
		}
	}; //Skip tabulations and new strings
	while (true)
	{

		if (CurrentCharacter == '<')
			return Counter;
		m_MainBuff[Counter++] = CurrentCharacter;
		if ((CurrentCharacter == '\t') || (CurrentCharacter == '\n')) {
			break;
		}
		if (m_ifstr.eof()) {
			return -1;
		}
		CurrentCharacter = m_ifstr.get();
		if (Counter >= _MAX_PATH)
			return -1;
	}; //If we didn't meet '<' look it after spaces
	while (true)
	{
		if (m_ifstr.eof()) {
			return -1;
		}
		if ((CurrentCharacter != '\t') && (CurrentCharacter != '\n') && (CurrentCharacter != ' ')) {
			if (CurrentCharacter == '<')
				return Counter;
			else
				return -1;
		}
		CurrentCharacter = m_ifstr.get();
	}; //Skip tabulations and new strings
	return 0;
}

int XML_Parser::ReadNextAttribute(Xml_Elem_ptr* BaseValue)
{
	wchar_t CurrentCharacter;
	do {
		CurrentCharacter = m_ifstr.get();
		if (m_ifstr.eof())
			return -1;
	} while ((CurrentCharacter == ' ') || (CurrentCharacter == '\n') || (CurrentCharacter == '\t'));

	if (CurrentCharacter == '>')
		return 0; //0 Elements added, but success
	if (CurrentCharacter == '/') {
		CurrentCharacter = m_ifstr.get();
		if (CurrentCharacter == '>')
			return 4; //0 elements Added, but success at all. (Closed Inline Tag)
		return -1; //Error
	}

	//Default (If usual Symbols started)
	int Counter = 0;
	while (true) {
		m_MainBuff[Counter++] = CurrentCharacter;
		CurrentCharacter = m_ifstr.get();

		if ((CurrentCharacter == ' ') || (CurrentCharacter == '=')) {
			break;
		}
		if (m_ifstr.eof() || (Counter >= _MAX_PATH))
			return -1; //Something Went Wrong
	}
	//Find Next quotes
	do {
		CurrentCharacter = m_ifstr.get();
		if (m_ifstr.eof())
			return -1; //Impossible to find Quotes
	} while (CurrentCharacter != '\"');
	m_MainBuff[Counter] = 0;

	Xml_Elem_ptr* Xml_Attribute = new Xml_Elem_ptr_string(m_MainBuff);
	Xml_Attribute->MakeInside();


	//SetValue
	Counter = 0; //Reset counter
	while (true) {
		CurrentCharacter = m_ifstr.get();
		if (CurrentCharacter == '\"') {
			if (Counter == 0) {
				delete Xml_Attribute;
				return 0;
			}
			m_MainBuff[Counter] = 0;

			Xml_Attribute->SetValue(m_MainBuff);
			BaseValue->PutElem(Xml_Attribute);
			return 1;
			//Xml_Attribute.
		}
		m_MainBuff[Counter++] = CurrentCharacter;
		if ((Counter >= _MAX_PATH) || (m_ifstr.eof())) {
			delete Xml_Attribute;
			return -1;
		}
	}


	return -1;
}
Xml_Elem_ptr* XML_Parser::ParseTag(int& Result, Xml_Elem_ptr* BaseValue)
{
	Result = -1;
	wchar_t CurrentCharacter = m_ifstr.get();
	switch (CurrentCharacter) {
	case '!': { //If comment - skip it
		if ((m_ifstr.get() == '-') && (m_ifstr.get() == '-')) {
			while (!m_ifstr.eof()) {
				CurrentCharacter = m_ifstr.get();
				if (CurrentCharacter == '>') {
					Result = 2; //Set Tag as comment
					return NULL;
				}
			}
		}

		Result = -1;
		return NULL;
		break; //Not necessary, cuz we have return
	}
	case '?': { //It can be also programm comment
		while (!m_ifstr.eof()) {
			CurrentCharacter = m_ifstr.get();
			if (CurrentCharacter == '>') {
				Result = 3; //Set Tag as comment
				return NULL;
			}
		}
		Result = -1;
		return NULL;
		break;//Not Necessery, cuz we have "return value"
	}
	case '/': {
		if (BaseValue->Tag.empty()) {
			Result = -1;
			return NULL;
		}
		
		for (auto it = BaseValue->Tag.begin(); it != BaseValue->Tag.end(); it++) {
			CurrentCharacter = m_ifstr.get();
			if ((*it != CurrentCharacter) || (m_ifstr.eof()) || (CurrentCharacter == '>')) {
				Result = -1;
				return NULL;
			}
		}
		if (m_ifstr.get() == '>') {
			Result = 0; //Close tag
			return NULL;
		}
		Result = -1;
		return NULL;
		break;
	}
	case '>': {
		Result = -1;
		return NULL;
	}
	}
	int Counter = 0; //Totally, start working with an open tag
	while (true) {
		m_MainBuff[Counter++] = CurrentCharacter;


		if (m_ifstr.eof() || (Counter >= _MAX_PATH)) {
			Result = -1;
			return NULL;
		}
		CurrentCharacter = m_ifstr.get();

		if (CurrentCharacter == '>') {
			
			m_MainBuff[Counter] = 0;
			Xml_Elem_ptr* MainReturnValue = new Xml_Elem_ptr_string(m_MainBuff);
			BaseValue->PutElem(MainReturnValue);
			Result = 1; //Mark it as an open tag
			return MainReturnValue;
			

		}
		if ((CurrentCharacter == ' ') || (CurrentCharacter == '\n') || (CurrentCharacter == '\t')) {
			m_MainBuff[Counter] = 0;
			Xml_Elem_ptr* MainReturnValue = new Xml_Elem_ptr_string(m_MainBuff);
			while (true) {
				int ResultOfReading = ReadNextAttribute(MainReturnValue);
				if (ResultOfReading == -1) { //If error
					Result = -1;
					delete MainReturnValue;
					return NULL;
				}

				if (ResultOfReading == 4) { //If it is closed open tag
					Result = 4;
					delete MainReturnValue;
					return NULL;
				}

				if (ResultOfReading == 0) {
					Result = 1; //Mark it as an open tag
					BaseValue->PutElem(MainReturnValue);
					return MainReturnValue;
				}
				if (ResultOfReading == 1) {
					//Look for a next attribute
				};
		}
	}

	}
	return nullptr;
}