#pragma once
#include <iostream>
#include <afxinet.h>
#include <WININET.H>

using namespace std;

void OnOK();
CString ErrorOut(DWORD dError);
int Send_Internet(CString sURL, CString sPostMsg, CString& sRecv);