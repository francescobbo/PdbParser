// This is the main DLL file.

#include "stdafx.h"
#include "PdbParser.h"
#include <vcclr.h>

namespace PdbParser
{
	PdbParser::PdbParser(String^ szFilename)
	{
		CoInitialize(NULL);

		IDiaDataSource *src;
		HRESULT hr = CoCreateInstance(__uuidof(DiaSource), NULL, CLSCTX_INPROC_SERVER, __uuidof(IDiaDataSource), (void **) &src);
		if (FAILED(hr))
			throw "CoCreateInstance Failed!";
		g_pDiaDataSource = src;

		pin_ptr<const wchar_t> wch = PtrToStringChars(szFilename);

		hr = (g_pDiaDataSource)->loadDataFromPdb(wch);
		if (FAILED(hr))
			throw "loadDataFromPdb failed!";

		IDiaSession *sess;
		hr = (g_pDiaDataSource)->openSession(&sess);
		if (FAILED(hr))
			throw "openSession failed!";
		g_pDiaSession = sess;

		IDiaSymbol *symb;
		hr = (g_pDiaSession)->get_globalScope(&symb);
		if (hr != S_OK)
			throw "get_globalScope failed!";
		g_pGlobalSymbol = symb;
	}

	PdbParser::~PdbParser()
	{
		if (g_pGlobalSymbol)
		{
			g_pGlobalSymbol->Release();
			g_pGlobalSymbol = NULL;
		}

		if (g_pDiaSession)
		{
			g_pDiaSession->Release();
			g_pDiaSession = NULL;
		}

		CoUninitialize();
	}

	List<Symbol ^> ^PdbParser::DumpAllPublics()
	{
		List<Symbol ^> ^symbols = gcnew List<Symbol ^>();

		// Retrieve all the public symbols
		IDiaEnumSymbols *pEnumSymbols;

		if (FAILED(g_pGlobalSymbol->findChildren(SymTagPublicSymbol, NULL, nsNone, &pEnumSymbols)))
			throw "Can't find children";

		IDiaSymbol *pSymbol;
		ULONG celt = 0;

		while (SUCCEEDED(pEnumSymbols->Next(1, &pSymbol, &celt)) && (celt == 1))
		{
			try
			{
				Symbol^ s = ParseSymbol(pSymbol);
				symbols->Add(s);
			}
			catch (Exception ^)
			{
			}

			pSymbol->Release();
		}

		pEnumSymbols->Release();

		return symbols;
	}

	Symbol^ PdbParser::ParseSymbol(IDiaSymbol *pSymbol)
	{
		Symbol^ s = gcnew Symbol;

		DWORD rva, section, offset;
		BSTR bstrName;

		if (pSymbol->get_relativeVirtualAddress(&rva) != S_OK)
			throw;
		pSymbol->get_addressSection(&section);
		pSymbol->get_addressOffset(&offset);

		s->Rva = rva;
		s->Section = section;
		s->Offset = offset;

		if (pSymbol->get_name(&bstrName) == S_OK)
		{
			s->Name = gcnew String(bstrName, 0, wcslen(bstrName));
			SysFreeString(bstrName);
		}
		else
			throw;

		return s;
	}
}
