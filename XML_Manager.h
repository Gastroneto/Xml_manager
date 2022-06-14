#pragma once
#include <fstream>
#include <string>
#include <vector>
#include<iostream>

class Xml_Elem_ptr {
public:
	Xml_Elem_ptr() :IsInside(false), Tag(L""){};
	Xml_Elem_ptr(std::wstring TagName) :IsInside(false) {
		RenameTag(TagName);
	};
	~Xml_Elem_ptr();
	bool RenameTag(std::wstring NewTagName);
	void AttachToVariable(void* VariableRef);
public:
	bool AttachByPath(void* VariableRef, std::wstring PathOfVariable, wchar_t Separator = '\\');
	bool AttachByVec(void* VariableRef, std::vector<std::wstring> &VecOfPathes, int CurLevel);
public:
	std::wstring GetSchema(int CurrentLevel);
	bool PutElem(Xml_Elem_ptr *ChildElem);
	void Clear(); //Full clear the whole tree
	void MakeInside(); //Make this fragment inside one
	virtual bool IsTagInside() {
		return IsInside;
	}
	//Get First daughter from our vector
	Xml_Elem_ptr* GetFirstInheritor();
	void SetValue(wchar_t *NameOfNewValue);
	std::wstring Tag;
protected:
	//OurStructOfValues
	void* Value=NULL; //Our Pointer to Value
	bool IsInside = false;
	bool m_bIsInitByText = false;

	std::vector<Xml_Elem_ptr*> Inheritors;


protected:
	virtual std::wstring GetStrValue();
};

class Xml_Elem_ptr_string :public Xml_Elem_ptr {
public:
	Xml_Elem_ptr_string(std::wstring NewTagName);
	Xml_Elem_ptr_string();
	//void SetValue(std::wstring &NewValue);
protected:
	virtual std::wstring GetStrValue();

};

class Xml_Elem_ptr_Numb :public Xml_Elem_ptr {
public:
	Xml_Elem_ptr_Numb();
	Xml_Elem_ptr_Numb(std::wstring NewTagName);
protected:
	virtual std::wstring GetStrValue();
};

class Xml_Elem_ptr_NumbSet :public Xml_Elem_ptr {
public:
	Xml_Elem_ptr_NumbSet();
	Xml_Elem_ptr_NumbSet(std::wstring NewTagName);
protected:
	char separator = ' ';
	virtual std::wstring GetStrValue();
};



bool CreateXML(std::wstring  PathOfFile, Xml_Elem_ptr* Element, std::wstring IncludingPart = L"<?xml version='1.0' encoding='utf-8'?>");
bool CreateXML(std::wstring PathOfile, std::wstring StringForWriting, std::wstring IncludingPart = L"<?xml version='1.0' encoding='utf-8'?>");


class XML_Parser {
public:
	XML_Parser();
	~XML_Parser();

	bool FillFromXML(std::wstring PathToXml);

	Xml_Elem_ptr* GetResultElement();

protected:
	void InitVariables();

	Xml_Elem_ptr* m_xml_Result; //our Final result Element
	std::ifstream m_ifstr; //File, we are working with this one

	wchar_t* m_MainBuff; //We use them to parse values, init them only once
	wchar_t* m_SubBuff;

protected://My Parser 
	/*
	* Default action, we call this function, when we find not closed Open Tag
	* Return int Value: -1 = error (Error)
	* Return int Value:  1 = an element is initialized?
	* //IsFirst - Is that a base element
	*/
	int FillThisElement(Xml_Elem_ptr* CurElem, bool IsFirst = false);

	/*
	* Return int Value: -1 = error (Error)
	* Return int Value: (all unsigned) = count of characters between tags.
	* 
	*/
	int GoToNextTag();

	/*
	* Result: -1 = error (ERROR)
	* Result:  0 = Close Tag </
	* Result:  1 = Open Tag <__ IMPORTANT: We return value of this open tag
	* Result:  2 = Comment <!--
	* Result:  3 = System compiled code <?
	* Result:  4 = We close tag in open tag <___ ... /> IMPORTANT: We return value of this open tag
	*/
	Xml_Elem_ptr* ParseTag(int& Result, Xml_Elem_ptr* BaseValue);
	/*
	* Return int value: -1 = error (Error)
	* Return int value: 0 = No Elements Added, Last symbol = '>'
	* Return int value: 1 = Attribute added, Last symbol = Closing quote
	* Return int value: 4 = No Elements Added, It was one line Tag ( .../>
	*/
	int ReadNextAttribute(Xml_Elem_ptr* BaseValue);

};

std::vector<std::wstring> GetVecFromPath(std::wstring &TreePath, wchar_t Separator); //Get from path like Var1//Var2//Var3 - Vector like Var1, Var2, Var3 , where // is a separator 