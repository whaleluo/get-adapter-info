#include <winsock2.h>
#include "iostream"
#include <ws2tcpip.h>
#pragma comment(lib, "IPHLPAPI.lib")
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <napi.h>

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

Napi::Object CreateObject(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    std::string arg0;
    DWORD dwSize = 0;
    unsigned int i, j;
    if (info.Length() == 1 && !info[0].IsString()) {
        Napi::TypeError::New(env, "Expect String").ThrowAsJavaScriptException();
    }
    if (info.Length() == 1 && info[0].IsString()) {
        arg0 =info[0].As<Napi::String>().Utf8Value();
    }
    // std::cout << arg0;
    // printf("params str:%s %zdn",arg0.c_str(),arg0.size());

    Napi::Object obj = Napi::Object::New(env);

    // getNowNetAdapterName
    std::string nowNetAdapterName;
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL)
    {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
    }
    // Make an initial call to GetAdaptersInfo to get
    // the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL)
        {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
    {
        pAdapter = pAdapterInfo;
        while (pAdapter)
        {
            if (std::string(pAdapter->GatewayList.IpAddress.String).compare("0.0.0.0") != 0)
            {
                nowNetAdapterName = pAdapter->Description;
                break;
            }
            pAdapter = pAdapter->Next;
        }
    }
    else
    {
        printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
    }
    if (pAdapterInfo)
    {
        FREE(pAdapterInfo);
    }
    // Declare and initialize variables.

    /* variables used for GetIfTable and GetIfEntry */
    MIB_IFTABLE *pIfTable;
    MIB_IFROW *pIfRow;
    // Allocate memory for our pointers.
    pIfTable = (MIB_IFTABLE *)MALLOC(sizeof(MIB_IFTABLE));
    if (pIfTable == NULL) {
        Napi::TypeError::New(env, "Error allocating memory needed to call GetIfTable")
                .ThrowAsJavaScriptException();
    }
    // Make an initial call to GetIfTable to get the
    // necessary size into dwSize
    dwSize = sizeof(MIB_IFTABLE);
    if (GetIfTable(pIfTable, &dwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER)
    {
        FREE(pIfTable);
        pIfTable = (MIB_IFTABLE *)MALLOC(dwSize);
        if (pIfTable == NULL) {
            Napi::TypeError::New(env, "Error allocating memory needed to call GetIfTable")
                            .ThrowAsJavaScriptException();
        }
    }
    // Make a second call to GetIfTable to get the actual data we want.
    if ((dwRetVal = GetIfTable(pIfTable, &dwSize, FALSE)) == NO_ERROR)
    {
        for (i = 0; i < pIfTable->dwNumEntries; i++)
        {
            pIfRow = (MIB_IFROW *)&pIfTable->table[i];
            if ((pIfRow->dwOperStatus == IF_OPER_STATUS_OPERATIONAL || pIfRow->dwOperStatus == IF_OPER_STATUS_CONNECTED)
               && (pIfRow->dwType == IF_TYPE_ETHERNET_CSMACD || pIfRow->dwType == IF_TYPE_IEEE80211)){

                    std::string bDescr;
                    // string
                    for (j = 0; j < pIfRow->dwDescrLen - 1; j++)
                        bDescr += pIfRow->bDescr[j];

                    if (nowNetAdapterName.compare(bDescr)==0)
                    {
                        obj.Set(Napi::String::New(env, "bDescr"), Napi::String::New(env, bDescr));

                        // The number of octets of data received through this interface.
                        obj.Set(Napi::String::New(env, "dwInOctets"), Napi::Number::New(env, pIfRow->dwInOctets));
                        // The number of octets of data sent through this interface.
                        obj.Set(Napi::String::New(env, "dwOutOctets"), Napi::Number::New(env, pIfRow->dwOutOctets));

                        std::wstring wsz_name(pIfRow->wszName);
                        std::string sz_name(wsz_name.begin(), wsz_name.end());
                        obj.Set(Napi::String::New(env, "wszName"), Napi::String::New(env, sz_name));
                        obj.Set(Napi::String::New(env, "dwType"), Napi::Number::New(env, pIfRow->dwType));
                        obj.Set(Napi::String::New(env, "dwOperStatus"), Napi::Number::New(env, pIfRow->dwOperStatus));
                        obj.Set(Napi::String::New(env, "dwAdminStatus"), Napi::Number::New(env, pIfRow->dwAdminStatus));
                        obj.Set(Napi::String::New(env, "dwSpeed"), Napi::Number::New(env, pIfRow->dwSpeed));
                    }
                }
        }
    }
    else
    {
        printf("GetIfTable failed with error: \n");
        if (pIfTable != NULL)
        {
            FREE(pIfTable);
            pIfTable = NULL;
        }
        // Here you can use FormatMessage to find out why
        // it failed.
    }

    // release memory
    if (pIfTable != NULL)
    {
        FREE(pIfTable);
        pIfTable = NULL;
    }

    return obj;
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    return Napi::Function::New(env, CreateObject, "createObject");
}

NODE_API_MODULE(getAdapterInfo, Init)