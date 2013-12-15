//===========================================================================
//===========================================================================
//===========================================================================
//==   Direct_Access_Image_Sample.cpp  ==  Author: Costin-Anton BOIANGIU   ==
//===========================================================================
//===========================================================================
//===========================================================================

//===========================================================================
//===========================================================================
#include "stdafx.h"
#include <tesseract\baseapi.h>
#include <leptonica\allheaders.h>
#include <iostream>
#include <stdlib.h>
#include <strsafe.h>
#include "Direct_Access_Image.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <string>
#include <direct.h>
#include<vector>
using namespace std;
//===========================================================================
//===========================================================================

//===========================================================================
//===========================================================================

vector<char*> exeList;
TCHAR BAMdirectoryPath[MAX_PATH];

int getExeFiles(_TCHAR* argv[]){

	TCHAR path[MAX_PATH];
	DWORD dwError=0;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	string exe_extension = ".exe";	
	
	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.
	StringCchCopy(path, MAX_PATH, argv[1]);
	StringCchCat(path, MAX_PATH, TEXT("\\*"));
	hFind = FindFirstFile(path, &ffd);

	if (INVALID_HANDLE_VALUE == hFind) 
	{
		//TODO handle error
		return dwError;
	} 
   
	

	do
	{
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			//convert from wide char to narrow char array WCHAR=>char=>string
			char ch[260];
			char DefChar = ' ';
			WideCharToMultiByte(CP_ACP,0,ffd.cFileName,-1, ch,260,&DefChar, NULL);			
			string ss(ch);

			if(ss.rfind(exe_extension)==ss.length()-4){
				exeList.push_back(strdup(ch));			
			}else{
				continue;
			}
			
			/*cout<<"|||"<<ch<<"||||\n";
			cout<<ss.length()<<"\n";*/
		}
	
	}while (FindNextFile(hFind, &ffd) != 0);
 
	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES) 
	{
		cout<<"Received error: "<<dwError;
		return dwError;
	}

	FindClose(hFind);
	return 0;

}

int executeBAMS(char dir[]){

	int i;

	char exe[MAX_PATH];
	char input[MAX_PATH];
	char output[MAX_PATH];
	char output_conf[MAX_PATH];
	char cmd[500];
	char filename[100];

	for(i = 0; i<exeList.size();i++){
		strcpy(exe,dir);
		strcat(exe,"\\\\");
		strcat(exe,exeList.at(i));

		strcpy(cmd,exe);
		strcat(cmd," ");

		strcpy(input,dir);
		strcat(input,"\\\\");
		strcat(input,"in4_grayscale.tif");

		strcat(cmd,input);
		strcat(cmd, " ");

		strcpy(output,dir);
		strcat(output,"\\\\");
		sprintf(filename,"out%d.tif",i+1);
		strcat(output,filename);

		strcat(cmd,output);
		strcat(cmd, " ");

		strcpy(output_conf,dir);
		strcat(output_conf,"\\\\");
		sprintf(filename,"out_conf%d.tif",i+1);
		strcat(output_conf,filename);

		strcat(cmd,output_conf);

		cout<<"output: "<<output<<"\n\n";
		cout<<"CMD: "<<cmd<<"\n\n";

		int retCode = system(cmd);
		cout<<"======================"<<retCode<<"\n\n";
		
	}

	
	
	

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])

{
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();	
	int i;
	char dir[MAX_PATH];
	char DefChar = ' ';

    //Verify command-line usage correctness
    if (argc != 2)
    {
        _tprintf(_T("Use: %s <BAM exe directory>\n"), argv[0]);
        return -1;
    }
	WideCharToMultiByte(CP_ACP,0,argv[1],-1, dir,260,&DefChar, NULL);	
	getExeFiles(argv);
	executeBAMS(dir);
	
    //Return with success
    return 0;
}
//===========================================================================
//===========================================================================