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
#include <windows.h>
#include "Direct_Access_Image.h"
#include <iostream>
#include <tesseract\baseapi.h>
#include <leptonica\allheaders.h>
#include <stdlib.h>
#include <strsafe.h>
#include "Direct_Access_Image.h"
#include <stdio.h>
#include <conio.h>
#include <string>
#include <direct.h>
#include<vector>
#include <math.h>
#include "Shlwapi.h"
using namespace std;
//===========================================================================
//===========================================================================

//===========================================================================
//===========================================================================

 vector<TCHAR*> exeList;
 TCHAR BAMdirectoryPath[MAX_PATH];
 
 int getFiles(_TCHAR* argv[], string extension, vector<TCHAR*> & folderList){
 
   WIN32_FIND_DATA ffd;
   LARGE_INTEGER filesize;
   TCHAR szDir[MAX_PATH];
   size_t length_of_arg;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   DWORD dwError=0;


   // Check that the input path plus 3 is not longer than MAX_PATH.
   // Three characters are for the "\*" plus NULL appended below.

   StringCchLength(argv[1], MAX_PATH, &length_of_arg);

   if (length_of_arg > (MAX_PATH - 3))
   {
      _tprintf(TEXT("\nDirectory path is too long.\n"));
      return (-1);
   }

   // Prepare string for use with FindFile functions.  First, copy the
   // string to a buffer, then append '\*' to the directory name.

   StringCchCopy(szDir, MAX_PATH, argv[3]);
   StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

   // Find the first file in the directory.
   hFind = FindFirstFile(szDir, &ffd);
   if (INVALID_HANDLE_VALUE == hFind) 
   {
      return dwError;
   } 

   // List all the files in the directory with some info about them.
   do
   {
      if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
		  if(wcsstr(ffd.cFileName, L".exe")){
			  _tprintf(TEXT("  %s   \n"), ffd.cFileName);
			 exeList.push_back(_tcsdup(ffd.cFileName));
		  }
      }
      else
      {
		 continue;
      }
   }
   while (FindNextFile(hFind, &ffd) != 0);
   FindClose(hFind);
   return dwError;
 }
 


 int executeBAMS(vector<TCHAR*> bamList,TCHAR* dir, TCHAR* inFile, TCHAR* imgname, DWORD time){
	int i;
	LPDWORD exitcode = 0;
	STARTUPINFO procStartupInfo;
	PROCESS_INFORMATION procInfo;
	
	for(i = 0; i < bamList.size(); i++) {

		TCHAR *dest, *cmd;
		dest = _tcscpy(new TCHAR[_tcslen(dir) + _tcslen(bamList.at(i)) +1], dir);
		_tcscat(dest, _T("\\"));
		_tcscat(dest, bamList.at(i));
		_tprintf(_T("Complete Path = %s\n"), dest);
		// path-to-exec path-to-image exec_name.image_name.tiff |exec_name.image_name.conf.tiff
		cmd = _tcscpy(new TCHAR[_tcslen(dest) + 1 + _tcslen(inFile) + 1 + 2 * (_tcslen(bamList.at(i)) + 1 + _tcsclen(imgname)) + 20], dest); //da, am calculat !
		_tcscat(cmd, _T(" ")); _tcscat(cmd, inFile); _tcscat(cmd, _T(" "));
		_tcscat(cmd, bamList.at(i)); _tcscat(cmd, _T(".")); _tcscat(cmd, imgname); _tcscat(cmd, _T(".tiff ")); 
		_tcscat(cmd, bamList.at(i)); _tcscat(cmd, _T(".")); _tcscat(cmd, imgname); _tcscat(cmd, _T(".conf")); _tcscat(cmd, _T(".tiff ")); 
		_tprintf(_T("Complete cmd = %s\n"), cmd);
		
		CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &procStartupInfo, &procInfo);
		WaitForSingleObject(procInfo.hProcess, INFINITE); //TODO: CHANGE Infinite to proper value(ca in t1/t2);
		GetExitCodeProcess(procInfo.hProcess, exitcode); //now we get process exitcode
		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);

	}
 
   return 0;
 }


 float determinePNSR(char* filename, KImage *tessimage) {

	 
    //Buffer for the new file names
    TCHAR strNewFileName[0x100];

    //Load and verify that input image is a True-Color one
	printf("filename is %s\n", filename);
	TCHAR* fname =  _T("C:\\Users\Daniela\\Desktop\\VBAM\\vbam-evaluator\\BAMexe\\alin_lipan.tif"); //we can because we use unicode
	_tprintf(_T("DEBUG STUB, FILENAME IS %s\n"), fname);
    KImage *pImage = new KImage(fname);

    //Request direct access to image pixels in raw format
    BYTE **pDataMatrix= NULL;
	BYTE **tessDataMatrix = NULL;
	float pnsr, avg;
	int sum = 0;
    if ((pImage->BeginDirectAccess() && (pDataMatrix = pImage->GetDataMatrix()) != NULL) &
		(tessimage->BeginDirectAccess() && (tessDataMatrix = tessimage->GetDataMatrix()) != NULL))
    {
        //If direct access is obtained get image attributes and start processing pixels
        int intWidth = pImage->GetWidth();
        int intHeight = pImage->GetHeight();
		int tWidth = tessimage->GetWidth();
		int tHeight = tessimage->GetHeight();
		if( intWidth != tWidth || intHeight != tHeight) {
			printf("SIZE MISSMATCH, PROGRAM FAILURE\n");
			printf("intW, intH, tW, tH %d %d %d %d\n", intWidth, intHeight, tWidth, tHeight);
			exit(-1);
		}
		if (pImage->BeginDirectAccess() && tessimage->BeginDirectAccess())
        {
            for (int y = intHeight - 1; y >= 0; y--)
                for (int x = intWidth - 1; x >= 0; x--)
                {
                    //You may use this instead of the line below: 
                    //    BYTE PixelAtXY = pImageGrayscale->Get8BPPPixel(x, y)
                    BYTE &PixelAtXY = pDataMatrix[y][x];
					BYTE &TPixelatXY = tessDataMatrix[y][x];
					sum += ((int)(PixelAtXY - TPixelatXY))^2;
                /*    if (PixelAtXY < 0x80)
                        //...if closer to black, set to black
                        pImageBinary->Put1BPPPixel(x, y, false);
                    else
                        //...if closer to white, set to white
                        pImageBinary->Put1BPPPixel(x, y, true);
				*/
                }

            //Close direct access
            pImage->EndDirectAccess();
			tessimage->EndDirectAccess();
			avg = (float)sum / (intWidth * intHeight);
			printf("sum = %d, divs = %d\n", sum, intWidth * intHeight);
			printf("avg = %f\n", avg);
			pnsr = 10 * log10f(255*255/avg);
			printf("pnsr = %f\n", pnsr);
	 }
}
	return pnsr;
}
 void determine_winner() {
 }

 void compute_best_image() {
//we use peak-signal-to-noise ratio to determine winner

 }
 

 int tesseractOutput(TCHAR* imagepath){
 
     char *outText;
 
     tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
     // Initialize tesseract-ocr with English, without specifying tessdata path
     if (api->Init(NULL, "eng")) {
         fprintf(stderr, "Could not initialize tesseract.\n");
         exit(1);
     }
 
     // Open input image with leptonica library
    // Pix *image = pixRead("C:\Users\andrei\Documents\GitHub\vbam-evaluator\BAMexe\in4_grayscale.tif");
	 //TODO TESSERACT TCHAR TO CHAR conversion is fucked, we need to fix it
	 //=======================================================================
	 char *imgpath = new char [_tcsclen(imagepath) + 1]();
	 //imgpath = (char*) imagepath;
	 WideCharToMultiByte(CP_ACP, 0, imagepath, _tcsclen(imagepath), imgpath, _tcsclen(imagepath) + 1, NULL, FALSE); 
	 printf("imgpath = %s\n", imgpath);
	 Pix *image = pixRead(imgpath);  
     api->SetImage(image);
     // Get OCR result
     outText = api->GetUTF8Text();
     printf("OCR output:\n%s", outText);
 
     // Destroy used object and release memory
     api->End();
     delete [] outText;
     pixDestroy(&image);
	 //=======================================================================
     return 0;
 
 }
 

char *GetStringFromArg(_TCHAR* arg) {
	   char ch[260];
       char DefChar = ' ';
       WideCharToMultiByte(CP_ACP,0,arg,-1, ch,260,&DefChar, NULL);
	   return ch;
}

char* getFilename(string path){
  string filename;
  
  size_t pos = path.find_last_of("\\");
  if(pos != std::string::npos)
    filename.assign(path.begin() + pos + 1, path.end());
  else
    filename = path;
  
  char *result = new char[filename.length() + 1];
  printf("getfilename %s\n", path.c_str());
  strcpy(result, filename.c_str());
  return result;
}

int _tmain(int argc, _TCHAR* argv[])
{

   tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();  
   int i;
   TCHAR inputimg_name[256], inputimg_extension[256];

    //Verify command-line usage correctness
    if (argc != 7)
    {
		_tprintf(_T("Use: %s <timeout1> <timeout2> <bam_dir> <img_name>  <output_dir> <rez_img_name>\n"), argv[0]);
        return -1;
    }


   getFiles(argv, ".exe", exeList); //works fine.
   TCHAR *inputimg_path = _tcscpy (new TCHAR[_tcslen(argv[4]) + 1], argv[4]);
   _tprintf(_T("img_name = %s\n"), inputimg_path);
   _wsplitpath(inputimg_path, NULL, NULL, inputimg_name,inputimg_extension);
   _tcscat(inputimg_name, inputimg_extension);
   _tprintf(_T("img_name = %s\n"), inputimg_name);
  
   executeBAMS(exeList, argv[3], inputimg_path, inputimg_name, 0); // in progress
   //tesseractOutput(inputimg_path);

 
  /* TCHAR* tessfile = _T("C:\\Users\\andrei\\Documents\\GitHub\\vbam-evaluator\\BAMexe\\out1.tif");
   KImage *tssImage = new KImage(tessfile);
   char *path= (char*) "C:\\Users\\andrei\\Documents\\GitHub\\vbam-evaluator\\BAMexe\\alin_lipan.tif";
   printf("path is %s\n", path);
   determinePNSR(path, tssImage);*/
    //Return with success
    return 0;
}
//===========================================================================
//===========================================================================
