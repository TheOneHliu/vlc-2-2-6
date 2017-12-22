#ifndef VLC_NET_DATA_NOTIFY_H
#define VLC_NET_DATA_NOTIFY_H 1

#include "vlc_net_data.h"
#include <windows.h>
#include <string.h>

#define  DLL_NAME ("vlc_shell.dll")

static bool is_load_br_dll = false;
static HINSTANCE  br_shell = 0;
typedef void(*BrOnGetNetData)(const struct NetData *net_data);
static BrOnGetNetData OnGetNetData = NULL;

static void  NotifyNetEvent(const struct NetData *net_data)
{
	if (OnGetNetData)
		OnGetNetData(net_data);
	return;
}

static int  GetBrDllPath(char* dll_path)
{
	if (dll_path == NULL) return -1;

	if (0 != GetModuleFileNameA(NULL, dll_path, MAX_PATH))
	{
		char *cat_pos = strrchr(dll_path, '\\');
		if (NULL == cat_pos)
			cat_pos = dll_path + strlen(dll_path);
		else
			cat_pos += 1;

		strncpy(cat_pos, DLL_NAME, strlen(DLL_NAME));
	}

	return 0;
}

static bool  IsLoadBrDll()
{
	if(!is_load_br_dll)
	{
		char dll_path[MAX_PATH] = { 0 };
		if (0 == GetBrDllPath(dll_path))
		{
			br_shell = LoadLibraryA(dll_path);
			if (br_shell)
			{
				OnGetNetData = (BrOnGetNetData)GetProcAddress(br_shell, "OnGetNetData");
				if (OnGetNetData)
				{
					is_load_br_dll = true;
				}
				else
				{
					FreeLibrary(br_shell);
					br_shell = NULL;
				}
			}
		}
	}
	
	return is_load_br_dll;
}

static bool  FreeBrDll()
{
	bool free_successful = false;
	if (is_load_br_dll)
	{
		if (FreeLibrary(br_shell))
		{
			free_successful = true;
			br_shell = NULL;
			OnGetNetData = NULL;
		}
	}
	return  free_successful;
}

static void BrLog(char*format,...)
{
	char buf[512]={0};

    va_list args;
    va_start (args, format);
	vsnprintf(buf, 512,format,args);
    va_end (args);

	struct NetData net_data;
	net_data.id = 99;
	net_data.data = buf;
    NotifyNetEvent(&net_data);
	
	return;
}
#define msg_Br(...) BrLog(__VA_ARGS__) 


static int  GetCustomHeaders(struct CustomHeader** P_headers)
{
	int  count = 1;
	if(P_headers != NULL)
	{
		int buf_size = count*sizeof(struct CustomHeader);
		struct CustomHeader* headers = (struct CustomHeader*)malloc(buf_size);
		memset(headers,0,buf_size);
		for(int i = 0; i < count;++i)
		{
			memcpy(headers[i].key, "Via", 4);
			memcpy(headers[i].value, "bonree", 7);
		}
		*P_headers = headers;
	}
	
	return count;
}

static bool GetCustomeHost(const char* name, unsigned long* p_ip)
{
	*p_ip = 2227883836;
	return true;
}
#endif
