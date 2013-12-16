MPS project - VBAM


Install steps

1) Clone this repo
2) Download tesseract.zip (We used https://docs.google.com/file/d/0B52jOwoWvqGyX0U3THFHZEJkMlk/edit)
3) extract it in current directory(root folder)
4) Compile -- see Troubleshooting below for more info



Troubleshooting & requirements

1) Check project properties(Visual Studio) for the 
   following configurations:
		- at C/C++ -> General -> Additional Include Directories
		  tesseract\include must be present
		- at Linker -> General -> Additional Library Directories
		  tesseract\lib must be present
		-at Linker -> Input -> Additional Dependencies
		   liblept168.lib,
		   libtesseract302.lib must be present
2) Visual studio project expects to find the tesseract folder in root directory

3) Tesseract-OCR must be installed (we used https://code.google.com/p/tesseract-ocr/downloads/detail?name=tesseract-ocr-setup-3.01-1.exe&can=2&q )

4) Project expects to find installed data in 
	C:\Program Files (x86)\Tesseract-OCR

TODO : prettify the README LINKS 