/**
 * This file is part of Patternshop Project.
 * 
 * Patternshop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Patternshop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Patternshop.  If not, see <http://www.gnu.org/licenses/>
*/

#include "PsMacTools.h"
#include "PsFileTypes.h"

#define DEFAULT_FILENAME "Sans titre.pts"

#include <string>
#include <vector>

/*
** Filtre la boite d'ouverture de fichier sur les fichiers PTS
*/
static void SetPatternshopFileFilter(NavDialogRef dialogRef)
{
	CFMutableArrayRef identifiers = CFArrayCreateMutable( kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks );
	CFStringRef fileRef = UTTypeCreatePreferredIdentifierForTag( kUTTagClassFilenameExtension, CFSTR("pts"), kUTTypeData );
	CFArrayAppendValue( identifiers, fileRef );
	NavDialogSetFilterTypeIdentifiers(dialogRef, identifiers);	
}

/*
 ** Filtre la boite d'ouverture de fichier sur les fichiers images
 */
static void SetReadImageFilter(NavDialogRef dialogRef)
{
	CFMutableArrayRef identifiers = CFArrayCreateMutable( kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks );
	for (int i = 1; i < READ_FORMAT_COUNT; ++i)
	{
		char *type = strdup(READ_FORMATS[i][1] + 2);
		char *p = type;
		for (; *p && *p != ';';p++);
		*p = 0;
		CFStringRef tmp = CFStringCreateWithCString(NULL, type, kCFStringEncodingASCII);
		CFStringRef fileRef = UTTypeCreatePreferredIdentifierForTag( kUTTagClassFilenameExtension, tmp, kUTTypeData );
		CFArrayAppendValue(identifiers, fileRef);
		CFRelease(tmp);
		delete type;
	}
	NavDialogSetFilterTypeIdentifiers(dialogRef, identifiers);	
}

void SetFileDialogWorkingDirecory(NavDialogRef dialogRef, char *path)
{
	OSStatus status = noErr;
	OSErr theErr = noErr;
	FSRef ref;
	Boolean isDirectory;
	UInt8 pathc[1024];
	int l = strlen(path);
	for (int i = 0; i < l; ++i)
		pathc[i] = path[i];
	pathc[l] = 0;
	status = FSPathMakeRef(pathc, &ref, &isDirectory);
	if (status != noErr) return;
	FSSpec fsSpec;
	status = FSGetCatalogInfo(&ref, kFSCatInfoNone, NULL, NULL, &fsSpec, NULL);	
	assert(status == noErr);
	AEDesc theLocation;// = {typeNull, NULL);
	status = AECreateDesc(typeFSS, &fsSpec, sizeof(FSSpec), &theLocation);
	assert(status == noErr);
	theErr = NavCustomControl(dialogRef, kNavCtlSetLocation, &theLocation);
	assert(theErr == noErr);
}

/*
** Cette fonction ouvre une boite de dialogue d'ouverture de fichier 
** et remplit le buffer avec son chemin complet.
*/ 
bool PsOpenFileDialog(char *path, PsFileKind kind)
{
	bool bResult = false;
	NavDialogRef dialogRef;
	OSStatus status;
	NavDialogCreationOptions options;
	NavReplyRecord reply;
	
	NavGetDefaultDialogCreationOptions(&options);
	options.modality = kWindowModalityAppModal;
	options.location.h = options.location.v = -1;
	options.optionFlags = kNavDefaultNavDlogOptions;
	options.optionFlags |= kNavSelectAllReadableItem;
				
	NavCreateGetFileDialog(&options, NULL, NULL, NULL, NULL, NULL, &dialogRef);
	switch (kind)
	{
		case PS_PROJECT_FILE: 
			SetPatternshopFileFilter(dialogRef); 
			break;
		case PS_MOTIF_FILE:
		{
			SetReadImageFilter(dialogRef); 
			char current_path[1024];
			getwd(current_path);
			strcat(current_path, "/Motifs");
			SetFileDialogWorkingDirecory(dialogRef, current_path);
			break;
		}
		case PS_PATRON_FILE:
		{
			SetReadImageFilter(dialogRef); 
			char current_path[1024];
			getwd(current_path);
			strcat(current_path, "/Patrons");
			SetFileDialogWorkingDirecory(dialogRef, current_path);
			break;
		}
		default: 
			assert(0 && "PsOpenFileDialog, wrong PsFileKind");
			break;
	}
	
	status = NavDialogRun(dialogRef);
	if (!NavDialogGetReply (dialogRef, &reply))
	{
		AEKeyword keyword;
		AEDesc desc;
		if (!AEGetNthDesc (&reply.selection, 1, typeFSRef, &keyword, &desc))
		{
			FSRef ref;
			BlockMoveData (*desc.dataHandle, &ref, sizeof (FSRef));
			FSRefMakePath (&ref, (unsigned char *) path, 1023);
			AEDisposeDesc (&desc);
			bResult = true;
			//printf ("Fichier choisi: %s\n", path);
		}
		NavDisposeReply (&reply);
	}
	NavDialogDispose(dialogRef);
	return bResult;
}

std::vector<std::string> SetWriteImageFilterExensions;
int PsSaveFileDialogchosenItem = 0;

/*
 ** Filtre la boite d'enregistrement de fichier sur les fichiers images
 */
static void SetWriteImageFilter(NavDialogCreationOptions &options)
{
	if (!SetWriteImageFilterExensions.size())
	{
		for (int i = 0; i < WRITE_FORMAT_COUNT; ++i)
		{
			char *type = strdup(WRITE_FORMATS[i][1] + 2);
			char *p = type;
			for (; *p && *p != ';';p++);
			*p = 0;
			SetWriteImageFilterExensions.push_back(std::string(type));
			delete type;
		}
	}
	
	PsSaveFileDialogchosenItem = 0;
	
	 CFMutableArrayRef identifiers = CFArrayCreateMutable( kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks );
	 for (int i = 0; i < WRITE_FORMAT_COUNT; ++i)
	 {
		 char *type = strdup(WRITE_FORMATS[i][1] + 2);
		 char *p = type;
		 for (; *p && *p != ';';p++);
		 *p = 0;
		 CFStringRef tmp = CFStringCreateWithCString(NULL, type, kCFStringEncodingASCII);
		 CFStringRef fileRef = UTTypeCreatePreferredIdentifierForTag( kUTTagClassFilenameExtension, tmp, kUTTypeData );
		 CFStringRef description = UTTypeCopyDescription(fileRef);
		 //CFArrayAppendValue( identifiers, fileRef );
		 CFArrayAppendValue( identifiers, description );
		 CFRelease(tmp);
		 delete type;
	 }
	 options.optionFlags &= ~kNavNoTypePopup;
	// NavDialogSetFilterTypeIdentifiers(dialogRef, identifiers);	
	 options.popupExtension = identifiers;
}


void MyNavEventProc(NavEventCallbackMessage callBackSelector, NavCBRecPtr callBackParms, void * callBackUD)
{
	switch(callBackSelector)
	{
		case kNavCBPopupMenuSelect:
		{
			NavMenuItemSpecPtr chosenItem;
			chosenItem = (NavMenuItemSpecPtr)
				(callBackParms->eventData.eventDataParms.param);
			PsSaveFileDialogchosenItem = chosenItem->menuType;
			break;
		}
	}
}

/*
 ** Cette fonction ouvre une boite de dialogue d'enregistrement de fichier 
 ** et remplit le buffer avec son chemin complet.
 */ 
bool PsSaveFileDialog(char *path, PsFileKind kind)
{
	bool bResult = false;
	NavDialogRef dialogRef;
	NavDialogCreationOptions options;
	NavReplyRecord reply;
	
	NavGetDefaultDialogCreationOptions(&options);
	options.modality = kWindowModalityAppModal;
	options.location.h = options.location.v = -1;
	options.optionFlags = kNavDefaultNavDlogOptions;
	options.optionFlags |= kNavPreserveSaveFileExtension;
	if (kind == PS_PROJECT_FILE) options.saveFileName = CFSTR(DEFAULT_FILENAME);

	switch (kind)
	{
		case PS_PROJECT_FILE: break;
		default: SetWriteImageFilter(options); break;
	}
	
	NavCreatePutFileDialog(&options, NULL, 'pts0', &MyNavEventProc, NULL, &dialogRef);
	
	switch (kind)
	{
		case PS_PROJECT_FILE: SetPatternshopFileFilter(dialogRef); break;
		default: break;
	}
	
	NavDialogRun(dialogRef);
	if (NavDialogGetUserAction(dialogRef) == kNavUserActionCancel || kNavUserActionNone)
	{
		NavDialogDispose(dialogRef);
		return false;
	}
	if (!NavDialogGetReply (dialogRef, &reply))
	{
		AEDesc actualDesc;
		FSRef fileRefParent;
		AECoerceDesc(&reply.selection, typeFSRef, &actualDesc);
		if (AEGetDescData(&actualDesc, &fileRefParent, sizeof(FSRef)) == noErr )
		{
			FSRefMakePath(&fileRefParent, (UInt8*)path, 1024);
			char BufferName[1024];
			CFStringGetCString(reply.saveFileName, BufferName, 1023, kCFStringEncodingUTF8);
			strcat(path, "/");
			strcat(path, BufferName);
			if (kind == PS_IMAGE_FILE)
			{
				strcat(path, ".");
				strcat(path, SetWriteImageFilterExensions[PsSaveFileDialogchosenItem].c_str());
			//	printf("%s\n", path);
			}
			bResult = true;			
		}
		AEDisposeDesc(&actualDesc);		
		NavDisposeReply(&reply);
	}
	NavDialogDispose(dialogRef);
	return bResult;
}

/*
** Cette fonction affiche une boite de dialogue avec juste un message
*/
void PsMessageBox(char *msg)
{
	SInt16 reply;
	Str255 body;
	CFStringRef tmp = CFStringCreateWithCString (kCFAllocatorDefault, msg, kCFStringEncodingWindowsLatin1 );
	CFStringGetPascalString (tmp, body, 255, kCFStringEncodingMacRoman );
	StandardAlert(kAlertCautionAlert, body, 0, NULL, &reply);
	CFRelease(tmp);
}

/*
 ** Cette fonction affiche une boite de dialogue avec une question
 */
bool PsQuestionBox(char *msg)
{
	SInt16 reply;
	Str255 body;
	CFStringRef tmp = CFStringCreateWithCString (kCFAllocatorDefault, msg, kCFStringEncodingWindowsLatin1 );
	CFStringGetPascalString (tmp, body, 255, kCFStringEncodingMacRoman );
	AlertStdAlertParamRec option;
	option.movable = true;
   option.helpButton = false;
   option.filterProc = NULL;
   option.defaultText = "\pOui";
   option.cancelText = "\pNon";
   option.otherText = NULL;
   option.defaultButton = kAlertStdAlertOKButton;
   option.cancelButton = kAlertStdAlertCancelButton;
   option.position = kWindowCenterParentWindow;
	StandardAlert(kAlertCautionAlert, body, NULL, &option, &reply);
	CFRelease(tmp);
	if (reply == kAlertStdAlertOKButton)
		return true;
	return false;
}

/*
 ** Cette fonction crée une image à partir d'un PNG
 */
CGImageRef LoadCGImageFromPng (const char *imageName)
{
	CGImageRef image;
	CGDataProviderRef provider;
	CFStringRef name;
	CFURLRef url;
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	
	// Get the URL to the bundle resource.
	name = CFStringCreateWithCString (NULL, imageName, kCFStringEncodingUTF8);
	url = CFBundleCopyResourceURL(mainBundle, name, CFSTR("png"), NULL);
	CFRelease(name);
	assert(url != NULL);
	
	// Create the data provider object
	provider = CGDataProviderCreateWithURL(url);
	CFRelease (url);
	
	// Create the image object from that provider.
	image = CGImageCreateWithPNGDataProvider(provider, NULL, true,
															 kCGRenderingIntentDefault);
	CGDataProviderRelease (provider);
	
	return (image);
}

void DrawCGImage(CGContextRef myContext, int iWinHeigth, CGImageRef myImage, int x, int y)
{
	CGContextTranslateCTM( myContext, 0, iWinHeigth);
	CGContextScaleCTM(myContext, 1.0, -1.0 );
	CGContextDrawImage(myContext, CGRectMake(x, iWinHeigth - y - CGImageGetHeight(myImage), 
			CGImageGetWidth(myImage), CGImageGetHeight(myImage)), myImage);
	CGContextScaleCTM(myContext, 1.0, -1.0 );	
	CGContextTranslateCTM( myContext, 0, -iWinHeigth);
}

bool IsMouseInCGImage(CGImageRef myImage, int x, int y, int tx, int ty)
{
	if (tx > x 
		 && ty > y 
		 && tx < x + CGImageGetWidth(myImage) 
		 && ty < y + CGImageGetHeight(myImage))
		return true;
	return false;
}

const void *MyProviderGetBytePointer(void *info)
{
	return info;
}

void LoadSoftwareBufferFromPng(SoftwareBuffer &buffer, const char *filename)
{
	buffer.buffer = LoadCGImageFromPng(filename);
	// FIXME : context ?
}

	