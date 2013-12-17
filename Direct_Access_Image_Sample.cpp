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
using namespace std;
//===========================================================================
//===========================================================================

//===========================================================================
//===========================================================================

 vector<char*> exeList, tifList;
 TCHAR BAMdirectoryPath[MAX_PATH];
 
 int getFiles(_TCHAR* argv[], string extension, vector<char*> & folderList){
 
   TCHAR path[MAX_PATH];
   DWORD dwError=0;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   WIN32_FIND_DATA ffd;
   LARGE_INTEGER filesize;
   
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
 
       if(ss.rfind(extension)==ss.length()-4){
         folderList.push_back(strdup(ch));      
       }else{
         continue;
       }

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


 void determinePNSR(char* filename, KImage *tessimage) {

	 
    //Buffer for the new file names
    TCHAR strNewFileName[0x100];

    //Load and verify that input image is a True-Color one
	printf("filename is %s\n", filename);
	TCHAR* fname =  _T("C:\\Users\\andrei\\Documents\\GitHub\\vbam-evaluator\\BAMexe\\alin_lipan.tif"); //we can because we use unicode
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
}
 void determine_winner() {
 }

 void compute_best_image() {
//we use peak-signal-to-noise ratio to determine winner

 }
 
 int tesseractOutput(){
 
     char *outText;
 
     tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
     // Initialize tesseract-ocr with English, without specifying tessdata path
     if (api->Init(NULL, "eng")) {
         fprintf(stderr, "Could not initialize tesseract.\n");
         exit(1);
     }
 
     // Open input image with leptonica library
     Pix *image = pixRead("C:\Users\andrei\Documents\GitHub\vbam-evaluator\BAMexe\in4_grayscale.tif");
     api->SetImage(image);
     // Get OCR result
     outText = api->GetUTF8Text();
     printf("OCR output:\n%s", outText);
 
     // Destroy used object and release memory
     api->End();
     delete [] outText;
     pixDestroy(&image);
 
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
   getFiles(argv, ".exe", exeList);
   getFiles(argv, ".tif", tifList);
   executeBAMS(dir);
   tesseractOutput();
   TCHAR* tessfile = _T("C:\\Users\\andrei\\Documents\\GitHub\\vbam-evaluator\\BAMexe\\out1.tif");
   KImage *tssImage = new KImage(tessfile);
   char *path= (char*) "C:\\Users\\andrei\\Documents\\GitHub\\vbam-evaluator\\BAMexe\\alin_lipan.tif";
   printf("path is %s\n", path);
   determinePNSR(path, tssImage);
    //Return with success
    return 0;
}
//===========================================================================
//===========================================================================
