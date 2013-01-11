#pragma once

using namespace System;

namespace PdbParser
{
	public ref class Symbol
	{
	public:
		Symbol(void);

		String ^Name;
		UInt32 Rva;
		UInt32 Section, Offset;
	};
}
