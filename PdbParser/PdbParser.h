// PdbParser.h

#pragma once

#include <dia2.h>
#include "Symbol.h"

using namespace System;
using namespace System::Collections::Generic;

namespace PdbParser
{
	public ref class PdbParser
	{
	public:
		PdbParser(String^ szFilename);
		~PdbParser();

		List<Symbol ^> ^DumpAllPublics();

	private:
		Symbol^ ParseSymbol(IDiaSymbol *pSymbol);

		IDiaDataSource *g_pDiaDataSource;
		IDiaSession *g_pDiaSession;
		IDiaSymbol *g_pGlobalSymbol;
	};
}
