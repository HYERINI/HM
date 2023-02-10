#include "client.h"

void OnOK()

{

    // TODO: Add extra validation here


    //CDialog::OnOK();

    cout << "function call()" << endl;

    CString sRecv;

    DWORD dwError = 0;

    dwError = Send_Internet("https://127.0.0.18080/front-controller/v2/members", "testdata=data", sRecv);

    

    if (dwError != 0)

    {
        cout << "Error required" << endl;
        //�����޽��� �ۼ��� ����.

        LPVOID lpMsgBuf;

        FormatMessage(

            FORMAT_MESSAGE_ALLOCATE_BUFFER |

            FORMAT_MESSAGE_FROM_SYSTEM |

            FORMAT_MESSAGE_IGNORE_INSERTS,

            NULL,

            dwError,

            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),

            (LPTSTR)&lpMsgBuf, 0, NULL);


        CString Msg = lpMsgBuf == 0 ? ErrorOut(dwError) : (char*)lpMsgBuf;


        char buffer[20];

        _itoa(dwError, buffer, 10);

        Msg = buffer;

        LocalFree(lpMsgBuf);

        return;

    }
    cout << "send success" << endl;
}





CString ErrorOut(DWORD dError)

{

    switch (dError) {

    case 12001:

        return "(Out of handles)";



    case 12002:

        return "(Timeout)";



    case 12004:

        return "(Internal Error)";



    case 12005:

        return "(Invalid URL)";



    case 12006:

        return "(Unrecognized Scheme : Please check the URL name in 'TVFilmAlerter.ini'.)";



    case 12007:

        return "(Service Name Not Resolved)";



    case 12008:

        return "(Protocol Not Found)";



    case 12013:

        return "(Incorrect User Name)";



    case 12014:

        return "(Incorrect Password)";



    case 12015:

        return "(Login Failure)";



    case 12016:

        return "(Invalid Operation)";



    case 12017:

        return "(Operation Canceled)";



    case 12020:

        return "(Not Proxy Request)";



    case 12023:

        return "(No Direct Access)";



    case 12026:

        return "(Request Pending)";



    case 12027:

        return "(Incorrect Format)";



    case 12028:

        return "(Item not found)";



    case 12029:

        return "(Cannot connect)";



    case 12030:

        return "(Connection Aborted)";



    case 12031:

        return "(Connection Reset)";



    case 12033:

        return "(Invalid Proxy Request)";



    case 12034:

        return "(Need UI)";



    case 12035:

        return "(Sec Cert Date Invalid)";



    case 12038:

        return "(Sec Cert CN Invalid)";



    case 12044:

        return "(Client Auth Cert Needed)";



    case 12045:

        return "(Invalid CA Cert)";



    case 12046:

        return "(Client Auth Not Setup)";



    case 12150:

        return "(HTTP Header Not Found)";



    case 12152:

        return "(Invalid HTTP Server Response)";



    case 12153:

        return "(Invalid HTTP Header)";



    case 120154:

        return "(Invalid Query Request)";



    case 120156:

        return "(Redirect Failed)";



    case 120159:

        return "(TCP/IP not installed)";



    default:

        return "UnKnown";



    }

}



int Send_Internet(CString sURL, CString sPostMsg, CString& sRecv)

{

    try

    {

        HINTERNET hSession, hConnection, hRequest;



        CString strServerName;

        CString strObject, strHeader, strData;

        INTERNET_PORT nPort;

        DWORD dwServiceType;

        CString sSubPath;

        DWORD dwFlags;

        BOOL bRead;



        char* pszBuf = NULL;

        DWORD dwBytesRead = 0;

        DWORD bufSize = 4096;



        CString aHeader;

        aHeader += _T("Accept: text*/*\r\n");

        aHeader += _T("User-Agent: Mozilla/4.0 (compatible; MSIE 5.0;* Windows NT)\r\n");

        aHeader += _T("Content-type: application/x-www-form-urlencoded\r\n");

        aHeader += _T("\r\n\r\n");





        /*

        ex)

        sURL = "http://127.0.0.1/test.asp";

        strServerName = "127.0.0.1";

        nPort = 80;

        sSubPath = "/test.asp";



        */

        // sURL���� ���� �ּҿ� ������ �������� ��´�. http�� https�� �ƴ� ��쿡�� ����

        if (!AfxParseURL(sURL, dwServiceType, strServerName, strObject, nPort) ||

            (dwServiceType != AFX_INET_SERVICE_HTTP && dwServiceType != AFX_INET_SERVICE_HTTPS))

        {

            return GetLastError();

        }



        //�߰���� ����

        int nPos = sURL.Find(strServerName, 0);

        sSubPath = sURL.Mid(nPos + strServerName.GetLength());



        //���� ����

        hSession = InternetOpen((LPCWSTR)"Request", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

        if (hSession == NULL)

            return GetLastError();



        //���� �� ��Ʈ ��� ����

        hConnection = InternetConnectA(hSession, (LPCSTR)(LPCTSTR)strServerName, nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);

        if (hConnection == NULL)

        {

            InternetCloseHandle(hSession);

            return GetLastError();

        }



        //���� https ssl ����� �ʿ��ϴٸ� INTERNET_FLAG_SECURE �÷��� �߰�

        if (dwServiceType == AFX_INET_SERVICE_HTTPS)

        {

            dwFlags = INTERNET_FLAG_SECURE |

                INTERNET_FLAG_RELOAD |

                INTERNET_FLAG_DONT_CACHE |

                INTERNET_FLAG_NO_COOKIES;

        }

        else

        {

            dwFlags = INTERNET_FLAG_RELOAD |

                INTERNET_FLAG_DONT_CACHE |

                INTERNET_FLAG_NO_COOKIES;

        }


        /*

        Method ��� ���� �� �÷��� ����

        GET�� ��� sSubPath�� ���ϴ� ���ڸ� �־��ָ� �ȴ�.

        POST�� ��쿡 ���� ��θ� �־��ָ� �ȴ�. ���� �����ʹ� HttpSendRequest���� ����

        */

        hRequest = HttpOpenRequestA(hConnection, "POST", (LPCSTR)(LPCTSTR)sSubPath, "HTTP/1.1", NULL, NULL, dwFlags, 0);

        if (hRequest == NULL)

        {

            InternetCloseHandle(hConnection);

            InternetCloseHandle(hSession);


            return GetLastError();

        }



        /*

        https�� ����� ��� SSL ����� �� �� ������ ���� �ɼ�

        �׽�Ʈ ȯ�濡�� ���������� ���� ������ �ϰ�� 12045 ������ �߻��Ѵ�.

        12045 �˼� ���� �߱ޱ�� ������ ���쿡 �� �ɼ��� ������ �ָ� �����ϰ� ����Ѵ�.

        */

        if (dwServiceType == AFX_INET_SERVICE_HTTPS)

        {

            dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |

                SECURITY_FLAG_IGNORE_REVOCATION |

                SECURITY_FLAG_IGNORE_REDIRECT_TO_HTTP |

                SECURITY_FLAG_IGNORE_REDIRECT_TO_HTTPS |

                SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |

                SECURITY_FLAG_IGNORE_CERT_CN_INVALID;

            if (!InternetSetOption(hRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags)))

            {

                InternetCloseHandle(hRequest);

                InternetCloseHandle(hConnection);

                InternetCloseHandle(hSession);


                return GetLastError();

            }

        }



        //����� ��� ����

        dwFlags = HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD;

        if (!HttpAddRequestHeaders(hRequest, aHeader, aHeader.GetLength(), dwFlags))

        {

            InternetCloseHandle(hRequest);

            InternetCloseHandle(hConnection);

            InternetCloseHandle(hSession);


            return GetLastError();

        }



        //HttpSendRequest ������ ������ ������ �������� �ʰ� ���� �غ� �ϴ� �����̴�.

        //���� ������ �� �� �Ǿ��ų� ip, port �� ���� ���� ��� ��κ� ���⼭ ������ �߻��Ѵ�.

        if (!HttpSendRequest(hRequest, NULL, NULL, sPostMsg.GetBuffer(0), sPostMsg.GetLength()))

        {

            InternetCloseHandle(hRequest);

            InternetCloseHandle(hConnection);

            InternetCloseHandle(hSession);



            return GetLastError();

        }



        //request ���� response�� �޴� �κ�

        pszBuf = (char*)malloc(sizeof(char) * bufSize + 1);

        memset(pszBuf, '\0', bufSize + 1);



        do

        {

            bRead = InternetReadFile(hRequest, pszBuf, bufSize, &dwBytesRead);



            if ((bRead) && (dwBytesRead > 0)) {

                sRecv += pszBuf;

                memset(pszBuf, '\0', bufSize + 1);

            }

        } while ((bRead == TRUE) && (dwBytesRead > 0));



        InternetCloseHandle(hRequest);

        InternetCloseHandle(hConnection);

        InternetCloseHandle(hSession);



        free(pszBuf);

    }

    catch (CException& e)

    {

        char   szCause[255];

        CString strFormatted;



        e.GetErrorMessage((LPTSTR)szCause, (UINT)255,(PUINT)0);



        sRecv = szCause;



        return 1;

    }

    return NULL;

}