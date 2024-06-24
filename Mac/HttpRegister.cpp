#ifndef _DEMO

#include "PsIdentity.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>

#include <CoreFoundation/CoreFoundation.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/network/IOEthernetInterface.h>
#include <IOKit/network/IONetworkInterface.h>
#include <IOKit/network/IOEthernetController.h>

#define PACKET_SIZE  1024

static int writen(int fd, char *ptr, int n)
{
	int nl, nw;
	
	nl = n;
	while (nl > 0)
	{
		nw = send (fd, ptr, nl, 0);		
		if (nw <= 0)
			return nw;		
		nl -= nw;
		ptr += nw;
	}	
	return (n-nl);
}

static int readn(int fd, char *ptr, int n)
{
	int nl, nr;
	
	nl = n;	
	while (nl > 0)
	{
		nr = recv (fd, ptr, nl, 0);		
		if (nr < 0)
			return nr;
		else if (nr == 0)
			break;
		nl -= nr;
		ptr += nr;
	}	
	return (n - nl);
}

static kern_return_t FindEthernetInterfaces(io_iterator_t *matchingServices)
{
	kern_return_t    kernResult; 
	CFMutableDictionaryRef  matchingDict;
	CFMutableDictionaryRef  propertyMatchDict;
	
	matchingDict = IOServiceMatching(kIOEthernetInterfaceClass);
	if (NULL == matchingDict) {
		//printf("IOServiceMatching returned a NULL dictionary.\n");
	}
	else {
		propertyMatchDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
																	 &kCFTypeDictionaryKeyCallBacks,
																	 &kCFTypeDictionaryValueCallBacks);		
		if (NULL == propertyMatchDict) {
			//printf("CFDictionaryCreateMutable returned a NULL dictionary.\n");
		}
		else {
			CFDictionarySetValue(propertyMatchDict, CFSTR(kIOPrimaryInterface), kCFBooleanTrue); 
			CFDictionarySetValue(matchingDict, CFSTR(kIOPropertyMatchKey), propertyMatchDict);
			CFRelease(propertyMatchDict);
		}
	}
	kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDict, matchingServices);    
	if (KERN_SUCCESS != kernResult) {
		//printf("IOServiceGetMatchingServices returned 0x%08x\n", kernResult);
	}
	return kernResult;
}

static kern_return_t GetMACAddress(io_iterator_t intfIterator, UInt8 *MACAddress, UInt8 bufferSize)
{
	io_object_t    intfService;
	io_object_t    controllerService;
	kern_return_t  kernResult = KERN_FAILURE;
	
	if (bufferSize < kIOEthernetAddressSize) {
		return kernResult;
	}
	bzero(MACAddress, bufferSize);
	while (intfService = IOIteratorNext(intfIterator))
	{
		CFTypeRef  MACAddressAsCFData;        
		kernResult = IORegistryEntryGetParentEntry(intfService,
																 kIOServicePlane,
																 &controllerService);		
		if (KERN_SUCCESS != kernResult) {
			//printf("IORegistryEntryGetParentEntry returned 0x%08x\n", kernResult);
		}
		else {
			MACAddressAsCFData = IORegistryEntryCreateCFProperty(controllerService,
																				  CFSTR(kIOMACAddress),
																				  kCFAllocatorDefault,
																				  0);
			if (MACAddressAsCFData) {
				//CFShow(MACAddressAsCFData);
				CFDataGetBytes((CFDataRef)MACAddressAsCFData, CFRangeMake(0, kIOEthernetAddressSize), MACAddress);
				CFRelease(MACAddressAsCFData);
			}
			(void) IOObjectRelease(controllerService);
		}
		(void) IOObjectRelease(intfService);
	}
	
	return kernResult;
}

int GetMacAddress(std::string& address)
{
	kern_return_t  kernResult = KERN_SUCCESS; // on PowerPC this is an int (4 bytes)
	io_iterator_t  intfIterator;
	UInt8 MACAddress[kIOEthernetAddressSize];
	
	address = "";
	kernResult = FindEthernetInterfaces(&intfIterator);
	if (KERN_SUCCESS != kernResult) {
		//printf("FindEthernetInterfaces returned 0x%08x\n", kernResult);
	}
	else {
		kernResult = GetMACAddress(intfIterator, MACAddress, sizeof(MACAddress));
		
		if (KERN_SUCCESS != kernResult) {
			//printf("GetMACAddress returned 0x%08x\n", kernResult);
		}
		else {
			char buffer[1024];
			sprintf(buffer, "%02x%02x%02x%02x%02x%02x",
			MACAddress[0], MACAddress[1], MACAddress[2], MACAddress[3], MACAddress[4], MACAddress[5]);
			address = buffer;
		}
	}
	
	(void) IOObjectRelease(intfIterator);  // Release the iterator.
	
	return address == "";
}

std::string GetValidationString()
{
   const char *code = getUserCode();
   if (!code || !strlen(code)) return "";
   std::string   mac;
   char buffer[1024];
   GetMacAddress (mac);
   const char* macBuffer = mac.c_str();
   *buffer= 0;
   for ( int i = 0; i < 8; ++i )
   {
      char item[ 128 ];

      sprintf( item, "%d", code[ i ] + ( mask[ i ] - '0' ) );
      if ( i > 0 )
      {
         strcat( buffer, "-" );
      }
      strcat( buffer, item );
   }
   for ( int i = 0; i < 12; ++i )
   {
      char item[ 128 ];

      sprintf( item, "%d", macBuffer[ i ] + ( mask[ i + 8 ] - '0' ) );
      strcat( buffer, "-" );
      strcat( buffer, item );
   }

   return std::string(buffer);
}

int RegisterApplicationProc(std::string& receive)
{
	struct sockaddr_in	serverSockAddr;
	struct hostent			*serverHostEnt;
	long					hostAddr;
	int						rc;
	int						sock;
	char					buffer[PACKET_SIZE + 2];
	std::string				line;
	std::string				mac;
	std::string::size_type	pos, a, b;

	if (GetMacAddress (mac))
		return 2;
	memset (&serverSockAddr, 0, sizeof (serverSockAddr));
	hostAddr = inet_addr(server_name);
	if (hostAddr != -1)
		memcpy(&serverSockAddr.sin_addr, &hostAddr, sizeof (hostAddr));
	else /* si on a donne un nom  */
	{
		serverHostEnt = gethostbyname(server_name);
		if (serverHostEnt == NULL)
			return 1;
		memcpy(&serverSockAddr.sin_addr, serverHostEnt->h_addr, serverHostEnt->h_length);
	}

	serverSockAddr.sin_port = htons (server_port);
	serverSockAddr.sin_family = AF_INET;
	if ((sock = (int)socket (AF_INET, SOCK_STREAM, 0)) < 0)
		return 1;
	if (connect (sock, (struct sockaddr*)&serverSockAddr, sizeof (serverSockAddr)) < 0)
		return 1;

   std::string activationCode = GetValidationString();
   const char* myData = activationCode.c_str();

   /* envoi de la requete POST */
   sprintf (buffer,
            "POST %s HTTP/1.1\r\n"
            "HOST: %s\r\n"
            "Content-Type: application/x-www-form-urlencoded\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n"
            "var=%s",
            request_path,
            request_host,
            strlen( myData ) + 4,
            myData );

	writen (sock, buffer, (int)strlen (buffer) + 1);
	while ((rc = readn (sock, buffer, PACKET_SIZE)))
		line += buffer;
	if ((pos = line.find ("\r\n\r\n")) == std::string::npos)
		receive = "";
	else if ((a = line.find ("[", pos)) == std::string::npos || (b = line.find ("]", pos)) == std::string::npos)
		receive = "";
	else
		receive = line.substr (a + 1, b - a - 1);
	shutdown(sock, 2);
	close(sock);	
	if (receive.length() == 0) return 4; // pas de message -> pas de license
	return 0;
}

std::string authorizeAnswer;

void *AuthorizeThreadProc(void *parameter) 
{ 
	return (void*)RegisterApplicationProc(authorizeAnswer);
}

int RegisterApplication(std::string& receive)
{
	pthread_t thread1;
	int iret1 = -1;
	pthread_create(&thread1, NULL, AuthorizeThreadProc, NULL);
	pthread_join(thread1, (void**)&iret1);
	receive = authorizeAnswer;
	return (int)iret1;
}
 

#endif /* _DEMO */
