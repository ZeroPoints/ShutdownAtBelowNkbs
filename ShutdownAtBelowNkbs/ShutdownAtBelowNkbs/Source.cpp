#ifndef UNICODE
#define UNICODE
#endif

#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "iphlpapi.lib")

int main()
{
	int l = 0;//converted choice
	int n = 4;//kb/s input
	int i = 0;// i is number of entries in interfacetable
	int j = 0;//j is counter up to i until it finds the correct interface that matches the desc of the adapter of choice
	int k = 0;//k is the actual interface number that comes from j
	int x = 0;//x is counter up to the user chosen adapter value
	int y = 0;//y is total counter for how many adapters are there so user can see the order they are in
	int check;//this is the strcmp of adapt desc and int desc
	int done = 1;//flag for when the users inputted option is the same as one of the adapters numbers
	char timertxt[5];//goes with timer
	int timer = 1;//use this so we get a time over 10 seconds not every second as speeds go up and down. eg stable -> then spike -> 10seconds seems adequate
	double oldb = 0;//old kb/b for shutdowner//double............................................................................................
	char choice[2];//users choice for what adapter
	char KBValue[5];//users choice for shutdown at N kb/s	
	char dummy[100];	//shove the description into this so it can be compared to the chosen adapter
	char Adapt[100];//adapter that the user defines as the one needed to be monitored



	PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
	MIB_IFTABLE *pIfTable;
	DWORD dwRetVal = 0;//return values for errors
	DWORD dwSize = 0;//size for interface table
	ULONG ulOutBufLen;//buffer for adapter info

	



	//needed????????????????????????
    ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof (IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL)
	{
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        return 1;
    }
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            return 1;
        }
    }
	//////////////////////////


	//list adapaters for user
	printf("List of adapters\n\n");
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
        pAdapter = pAdapterInfo;
        while (pAdapter)
		{
			y++;
			printf("\t%d: ", y);
            printf("\t%s\n", pAdapter->Description);            
            pAdapter = pAdapter->Next;//goes to next adapter
        }
    }
	else 
	{
        printf("you killed it.\n");
	}


	//user enters adapater choice
	printf("\nPlease enter the number for the adapter you want to monitor: ");
	std::cin >> choice;
	l = atoi(choice);
	//
	
	//redo adapter info to get the one thats needed...by user selection
	GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
    pAdapter = pAdapterInfo;
	while (done)
	{
		x++;
		if(x == l)
		{
			strcpy(Adapt, pAdapter->Description);
			done = 0;
		}
		else
		{            
			pAdapter = pAdapter->Next;
		}
    }
    
	printf("%s\n",Adapt);
	



	//interface table allocation.
	// Allocate memory for our pointers.
    pIfTable = (MIB_IFTABLE *) malloc(sizeof (MIB_IFTABLE));
    if (pIfTable == NULL) {
        printf("Error allocating memory needed to call GetIfTable\n");
        return 1;
    }
    // Make an initial call to GetIfTable to get the
    // necessary size into dwSize
    dwSize = sizeof (MIB_IFTABLE);
    if (GetIfTable(pIfTable, &dwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER) {
        free(pIfTable);
        pIfTable = (MIB_IFTABLE *) malloc(dwSize);
        if (pIfTable == NULL) {
            printf("Error allocating memory needed to call GetIfTable\n");
            return 1;
        }
    }

	GetIfTable(pIfTable, &dwSize, FALSE);


	i = pIfTable->dwNumEntries;//sorts through the tables until final one looking for the one i want

	for(j = 0; j < i; j++)
	{
		for(int m = 0; m < 100; m++)
		{
			dummy[m] = '\0';
		}
		sprintf(dummy,"%s",pIfTable->table[j].bDescr);
		check = strcmp(dummy, Adapt);
		
		if(check == 0)
		{
			k = j;
			break;
		}
	}


	printf("---------------------------------------------------------\nType the min value you want your network traffic to shutdown at if it reachs.(just the number...)\n---------------------------------------------------------\n");	
	std::cin >> KBValue;
	n = atoi(KBValue);

	printf("---------------------------------------------------------\nType the time intervals you want the computer to check the traffic speed.(just the number...)\n---------------------------------------------------------\n");	
	std::cin >> timertxt;
	timer = atoi(timertxt);


	ShowWindow(GetConsoleWindow(), SW_HIDE);

	GetIfTable(pIfTable, &dwSize, FALSE);
	oldb = pIfTable->table[k].dwInOctets;
	Sleep(1000*timer);
	GetIfTable(pIfTable, &dwSize, FALSE);

	while(((pIfTable->table[k].dwInOctets - oldb)/(1024*timer)) >= n)
	{		
		printf("\tKB/s:\t\t %f\n",(pIfTable->table[k].dwInOctets - oldb)/(1024*timer));
		oldb = pIfTable->table[k].dwInOctets;
		Sleep(1000*timer);
		GetIfTable(pIfTable, &dwSize, FALSE);				
	}
	system("shutdown -s");


    

    return 0;
}