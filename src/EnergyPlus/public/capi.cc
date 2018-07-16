// EnergyPlus, Copyright (c) 1996-2018, The Board of Trustees of the University of Illinois,
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy), Oak Ridge
// National Laboratory, managed by UT-Battelle, Alliance for Sustainable Energy, LLC, and other
// contributors. All rights reserved.
//
// NOTICE: This Software was developed under funding from the U.S. Department of Energy and the
// U.S. Government consequently retains certain rights. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable,
// worldwide license in the Software to reproduce, distribute copies to the public, prepare
// derivative works, and perform publicly and display publicly, and to permit others to do so.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//
// (3) Neither the name of the University of California, Lawrence Berkeley National Laboratory,
//     the University of Illinois, U.S. Dept. of Energy nor the names of its contributors may be
//     used to endorse or promote products derived from this software without specific prior
//     written permission.
//
// (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in stand-alone form
//     without changes from the version obtained under this License, or (ii) Licensee makes a
//     reference solely to the software portion of its product, Licensee must refer to the
//     software as "EnergyPlus version X" software, where "X" is the version number Licensee
//     obtained under this License and may not use a different name for the software. Except as
//     specifically required in this Section (4), Licensee shall not use in a company name, a
//     product name, in advertising, publicity, or other promotional activities any name, trade
//     name, trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or confusingly
//     similar designation, without the U.S. Department of Energy's prior written consent.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include "capi.hh"

void WrappedMessageCallback( std::string const &message )
{
	const char * msg = message.c_str();
	if (GLOBAL_MESSAGE_CALLBACK == NULL)
	{
		std::cerr << "GLOBAL_MESSAGE_CALLBACK is NULL" << std::endl;
	}
	else
	{
		GLOBAL_MESSAGE_CALLBACK(msg);
	}
}

void WrappedProgressCallback(int const progress)
{
	if (GLOBAL_PROGRESS_CALLBACK == NULL)
	{
		std::cerr << "GLOBAL_PROGRESS_CALLBACK is NULL" << std::endl;
	}
	else
	{
		GLOBAL_PROGRESS_CALLBACK(progress);
	}
}

extern "C"
{
	int EXPORTCALL CALLCONV CheckConnect()
	{
		return 42;
	}

	int EXPORTCALL CALLCONV HelloWorld(char * greeting_out, int greeting_out_buffer_count)
	{
		char standard_greeting[] = "Hello, World!";
		// additional "+ 1" for the end of line marker
		int standard_greeting_count = strlen(standard_greeting) + 1;
		
		if (greeting_out_buffer_count < standard_greeting_count)
		{
			return standard_greeting_count;
		}
		strcpy(greeting_out, standard_greeting);
		return 0;
	}

	int EXPORTCALL CALLCONV RunEPlus(const char* path, int path_length)
	{
		int status(EXIT_FAILURE);
		std::string filepath;
		if (path_length == 0)
		{
			filepath = std::string();
		}
		else
		{
			filepath = std::string(path, path_length);
		}
		try
		{
			status = EnergyPlusPgmReturnCodes(filepath);
		}
		catch (...)
		{
			// Don't allow any exceptions to leak out to the caller
			std::cerr << "Exception encountered" << std::endl;
			return EXIT_FAILURE;
		}
		return status;
	}

	int EXPORTCALL CALLCONV SetMessageCallback(MsgCallback f)
	{
		GLOBAL_MESSAGE_CALLBACK = f;
		StoreMessageCallback(WrappedMessageCallback);
		return 0;
	}

	int EXPORTCALL CALLCONV SetProgressCallback(ProgressCallback f)
	{
		GLOBAL_PROGRESS_CALLBACK = f;
		StoreProgressCallback(WrappedProgressCallback);
		return 0;
	}
}