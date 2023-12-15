
#include "TXLib.h"
#include <fstream>
#include <stdio.h>
#include <dirent.h>
#include "button.cpp"
#include "pictures.cpp"

using namespace std;

int  get_w(string adress)
 {
    FILE *f1 = fopen(adress.c_str(),"rb");
    unsigned char heaberinfo[54];
    fread(heaberinfo,sizeof(unsigned char),54,f1);
    int w = *(int *)&heaberinfo[18];
    return w;
 }

int get_h(string adress)
{
    FILE *f1 = fopen(adress.c_str(),"rb");
    unsigned char heaberinfo[54];
    fread(heaberinfo,sizeof(unsigned char),54,f1);
    int h = *(int *)&heaberinfo[22];
    return h;
}

int readFromDir(string adress,Pictures menupic[], int cont_pic)
{
    DIR *dir;
    struct dirent *ent;
    int X = 10;
    int Y = 100;
    if ((dir = opendir (adress.c_str())) !=NULL)
    {
      while ((ent = readdir (dir)) !=NULL)
     {
        if((string)ent->d_name != "." && (string)ent->d_name != "..")
        {
            if(Y<=550 && X == 10)
            {
                X = 10;
            }
            else if (Y>550)
            {
                X=120;
                Y=100;
            }
            menupic[cont_pic].x = X;
            menupic[cont_pic].y = Y;
            menupic[cont_pic].adress = adress + (string)ent->d_name;
            cont_pic++;
            Y += 150;
        }
     }
      closedir(dir);
    }
    return cont_pic;
}

string runFileDiolog (bool isSave)
{
    string  FileName = "";

    OPENFILENAME ofn;
    TCHAR szFile[260] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = txWindow();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = ("Text\0*.TNT\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (isSave)
    {
         if(GetSaveFileName(&ofn)==TRUE)
         {
            FileName = ofn.lpstrFile;
            FileName = FileName  + ".txt";
         }
     }
     else
     {
       if (GetOpenFileName(&ofn)==TRUE)
       {
           FileName = ofn.lpstrFile;
       }

     }

    return  FileName;
}

inline int GetFilePointer(HANDLE FileHandle)
{
    return SetFilePointer(FileHandle, 0, 0, FILE_CURRENT);
}

bool SaveBMPFile(char *filename, HBITMAP bitmap, HDC bitmapDC, int width, int height)
{
    bool Success=0;
    HBITMAP OffscrBmp=NULL;
    HDC OffscrDC=NULL;
    LPBITMAPINFO lpbi=NULL;
    LPVOID lpvBits=NULL;
    HANDLE BmpFile=INVALID_HANDLE_VALUE;
    BITMAPFILEHEADER bmfh;
    if ((OffscrBmp = CreateCompatibleBitmap(bitmapDC, width, height)) == NULL)
        return 0;
    if ((OffscrDC = CreateCompatibleDC(bitmapDC)) == NULL)
        return 0;
    HBITMAP OldBmp = (HBITMAP)SelectObject(OffscrDC, OffscrBmp);
    BitBlt(OffscrDC, 0, 0, width, height, bitmapDC, 0, 0, SRCCOPY);
    if ((lpbi = (LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)])) == NULL)
        return 0;
    ZeroMemory(&lpbi->bmiHeader, sizeof(BITMAPINFOHEADER));
    lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    SelectObject(OffscrDC, OldBmp);
    if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, NULL, lpbi, DIB_RGB_COLORS))
        return 0;
    if ((lpvBits = new char[lpbi->bmiHeader.biSizeImage]) == NULL)
        return 0;
    if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, lpvBits, lpbi, DIB_RGB_COLORS))
        return 0;
    if ((BmpFile = CreateFile(filename,
                        GENERIC_WRITE,
                        0, NULL,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL)) == INVALID_HANDLE_VALUE)
        return 0;
    DWORD Written;
    bmfh.bfType = 19778;
    bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
    if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
        return 0;
    if (Written < sizeof(bmfh))
        return 0;
    if (!WriteFile(BmpFile, &lpbi->bmiHeader, sizeof(BITMAPINFOHEADER), &Written, NULL))
        return 0;
    if (Written < sizeof(BITMAPINFOHEADER))
        return 0;
    int PalEntries;
    if (lpbi->bmiHeader.biCompression == BI_BITFIELDS)
        PalEntries = 3;
    else PalEntries = (lpbi->bmiHeader.biBitCount <= 8) ?
                      (int)(1 << lpbi->bmiHeader.biBitCount) : 0;
    if(lpbi->bmiHeader.biClrUsed)
    PalEntries = lpbi->bmiHeader.biClrUsed;
    if(PalEntries){
    if (!WriteFile(BmpFile, &lpbi->bmiColors, PalEntries * sizeof(RGBQUAD), &Written, NULL))
        return 0;
        if (Written < PalEntries * sizeof(RGBQUAD))
            return 0;
    }
    bmfh.bfOffBits = GetFilePointer(BmpFile);
    if (!WriteFile(BmpFile, lpvBits, lpbi->bmiHeader.biSizeImage, &Written, NULL))
        return 0;
    if (Written < lpbi->bmiHeader.biSizeImage)
        return 0;
    bmfh.bfSize = GetFilePointer(BmpFile);
    SetFilePointer(BmpFile, 0, 0, FILE_BEGIN);
    if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
        return 0;
    if (Written < sizeof(bmfh))
        return 0;

    CloseHandle (BmpFile);

    delete [] (char*)lpvBits;
    delete [] lpbi;

    DeleteDC (OffscrDC);
    DeleteObject (OffscrBmp);


    return 1;
}

bool ScreenCapture(int x, int y, int width, int height, char *filename, HWND hwnd)
{
    HDC hDC = GetDC(hwnd);
    HDC hDc = CreateCompatibleDC(hDC);

    HBITMAP hBmp = CreateCompatibleBitmap(hDC, width, height);

    HGDIOBJ old= SelectObject(hDc, hBmp);
    BitBlt(hDc, 0, 0, width, height, hDC, x, y, SRCCOPY);

    bool ret = SaveBMPFile(filename, hBmp, hDc, width, height);

    SelectObject(hDc, old);

    DeleteObject(hBmp);

    DeleteDC (hDc);
    ReleaseDC (hwnd, hDC);

    return ret;
}
//���
HDC Fon = txLoadImage("Pictures/Fon.bmp");

const int cont_btn = 13;
//�� ���������� ������
const int btn_prstc = cont_btn -6;
//�� ����������
const int btn_save = cont_btn -5;
//�� ��������
const int btn_load = cont_btn -4;
//�� ������
const int btn_help = cont_btn -3;
//�� ������
const int btn_exit = cont_btn -2;
//�� ����
const int btn_menu = cont_btn -1;

//����� ����
const int page_menu = 0;
//����� ���������
const int page_redactor = 1;
//����� �������
const int page_help = 2;


int main()
{
    txCreateWindow (1250,750);
    txDisableAutoPause();
    txTextCursor (false);
    int cont_pic = 0;
    int n_sent_pic = 0;

    int page = page_menu;

    //Button �������
    Button btn[cont_btn];
    btn[0] = {60,30,"������","������"};
    btn[1] = {205,30,"������� ��","������� ��"};
    btn[2] = {350,30,"����� ��","����� ��"};
    btn[3] = {495,30,"������� �����","������� �����"};
    btn[4] = {640,30,"������ �����","������ �����"};
    btn[5] = {785,30,"�������","�������"};
    btn[6] = {930,30,"������","������"};
    btn[7] = {1100,450,"������ ������",""};
    btn[8] = {1100,550,"���������",""};
    btn[9] = {1100,600,"���������",""};
    btn[10] = {1100,500,"�������",""};
    btn[11] = {1100,650,"�����",""};
    btn[12] = {550,350,"������",""};

    //��� ������ ����
     Pictures menupic[100];

     cont_pic =readFromDir("Pictures/������/",menupic,cont_pic );
     cont_pic =readFromDir("Pictures/������� ��/",menupic,cont_pic);
     cont_pic =readFromDir("Pictures/����� ��/",menupic,cont_pic);
     cont_pic =readFromDir("Pictures/������� �����/",menupic,cont_pic);
     cont_pic =readFromDir("Pictures/������ �����/",menupic,cont_pic);
     cont_pic =readFromDir("Pictures/�������/",menupic,cont_pic);
     cont_pic =readFromDir("Pictures/������/",menupic,cont_pic);

    for(int i=0; i<cont_pic; i++)
{
    menupic[i].pic = txLoadImage(menupic[i].adress.c_str());

    menupic[i].w = get_w(menupic[i].adress);
    menupic[i].h = get_h(menupic[i].adress);

    menupic[i]. visible = false;

    string str = menupic[i].adress.c_str();
    int pos1 = str.find("/");
    int pos2 = str.find("/",pos1+1);
    menupic[i].category = str.substr(pos1+1, pos2-pos1-1);

    if(menupic[i].category == "������" )
    {
        menupic[i].w_scr =  menupic[i].w/1;
        menupic[i].h_scr =  menupic[i].h/1;
    }
   if(menupic[i].category == "������� ��" )
    {
        menupic[i].w_scr =  menupic[i].w/2 ;
        menupic[i].h_scr =  menupic[i].h/2 ;
    }
   if(menupic[i].category == "����� ��" )
    {
        menupic[i].w_scr =  menupic[i].w/2;
        menupic[i].h_scr =  menupic[i].h/2;
    }
   if(menupic[i].category == "������� �����" )
    {
        menupic[i].w_scr = menupic[i].w/2 ;
        menupic[i].h_scr = menupic[i].h/2 ;
    }
   if(menupic[i].category == "������ �����" )
    {
        menupic[i].w_scr =  menupic[i].w/2;
        menupic[i].h_scr =  menupic[i].h/2;
    }
   if(menupic[i].category == "�������" )
    {
        menupic[i].w_scr = menupic[i].w/2;
        menupic[i].h_scr = menupic[i].h/2;
    }
   if(menupic[i].category == "������" )
    {
        menupic[i].w_scr = menupic[i].w/1;
        menupic[i].h_scr = menupic[i].h/1;
    }

}
    // ��� ������ ��������/
    Pictures centpic[100];

    int vybor = -1;
    bool mouse_click = false;


    while(!btn[btn_exit].click())
{
    txBegin();
    txClear();
    txBitBlt(txDC() ,0 ,0, 1250,750, Fon);

    //����� ����
    if( page == page_menu)
    {
     btn[btn_menu].draw();
        if(btn[btn_menu].click())
        {
              page = page_redactor;
              txSleep(100);
        }
     btn[btn_help].draw();
         if(btn[btn_help].click())
         {
              page = page_help;
              btn[btn_help].name = "������";
              txSleep(100);
         }
    }
    //����� ���������
    if (page ==  page_redactor )
    {
         txSetColor    (TX_BLACK);
         txSetFillColor(TX_NULL);
         txRectangle(250,100,950,700);

         txSetColor    (TX_BLACK);
         txSetFillColor(TX_NULL);

         //������
         for (int i=0;i< cont_btn-1 ; i++)
         {
          btn[i].draw();
         }
        //��������
        //����
        for (int i=0;i<cont_pic; i++)
         {
          menupic[i].draw();
         }
         //�����
        for (int i=0;i<n_sent_pic; i++)
         {
           centpic[i].draw();
         }
         //��������� ��������� ��� ���� �� ���������
        for (int ib=0;ib<cont_btn; ib++)
    {
          if(btn[ib].click())
        {
            for (int ip=0;ip<cont_pic; ip++)
            {
                menupic[ip].visible= false;
                if(menupic[ip].category == btn[ib].category)
                {
                 menupic[ip].visible= true;
                }
            }
        }
    }

        //��������� ��. ������.
        for (int npic=0; npic<cont_pic;  npic++)
        {
            if(menupic[npic].click() && menupic[npic].visible)
            {
                while(txMouseButtons() == 1)
                {
                  txSleep(10);
                }

                centpic[n_sent_pic] = { 200,
                                        100,
                                        menupic[npic].adress,
                                        menupic[npic].pic,
                                        menupic[npic].w,
                                        menupic[npic].h,
                                        menupic[npic].w,
                                        menupic[npic].h,
                                        menupic[npic].visible,
                                        menupic[npic].category};
                n_sent_pic ++;
            }
        }

        //����� ��������
        for(int i=0; i<n_sent_pic; i++)
        {
         if(centpic[i].click() && centpic[i].visible)
             {
               vybor = i;
               mouse_click =false;
             }

        }

        if(vybor>=0)
        {
           if(GetAsyncKeyState(VK_LEFT ))
           {
            centpic[vybor].x -= 5;
           }
           if(GetAsyncKeyState(VK_RIGHT))
           {
            centpic[vybor].x += 5;
           }
           if(GetAsyncKeyState(VK_UP   ))
           {
            centpic[vybor].y -= 5;
           }
           if(GetAsyncKeyState(VK_DOWN ))
           {
            centpic[vybor].y += 5;
           }
           if(GetAsyncKeyState(VK_OEM_PLUS))
           {
            centpic[vybor].w_scr =centpic[vybor].w_scr * 1.1;
            centpic[vybor].h_scr =centpic[vybor].h_scr * 1.1;
           }
           if(GetAsyncKeyState(VK_OEM_MINUS))
           {
            centpic[vybor].w_scr =centpic[vybor].w_scr * 0.9;
            centpic[vybor].h_scr =centpic[vybor].h_scr * 0.9;
           }
        }
        //�������� �� �����
        if(vybor>=0)
        {
            if(txMouseButtons() == 1 && !mouse_click)
            {
                centpic[vybor].x = txMouseX() - centpic[vybor].w_scr/2;
                centpic[vybor].y = txMouseY() - centpic[vybor].h_scr/2;
            }
            else
            {
                if(txMouseButtons() !=1)
                {
                    mouse_click = true;
                }
            }
        }
        //�������� ��������
        if(vybor>=0 && GetAsyncKeyState(VK_DELETE))
        {
          centpic[vybor] = centpic[n_sent_pic-1];
          n_sent_pic--;
          vybor = -1;
          mouse_click = true;
        }

        //���� ����
            if(btn[btn_save].click())
            {
                string FileName = runFileDiolog (true);

                ofstream  fileout;               // ����� ��� ������
                fileout.open(FileName);      // ��������� ���� ��� ������
                if (fileout.is_open())
                {
                    for(int i=0; i<n_sent_pic;i++)
                    {
                        fileout << centpic[i].x << endl;
                        fileout << centpic[i].y << endl;
                        fileout << centpic[i].adress << endl;
                        fileout << centpic[i].w_scr << endl;
                        fileout << centpic[i].h_scr << endl;
                    }
                }
                fileout.close();                //��������� ���� ��� ������
            }
        //������ �� �����
            if(btn[btn_load].click())
            {
                string FileName = runFileDiolog (false);

                for(int i=0;i<cont_pic; i++)
                {
                 centpic[i].visible = false;
                }

                char buff[50];
                ifstream filein(FileName);  // �������� ���� ��� ������

                    while (filein.good())
                    {

                        filein.getline(buff,50);
                        int x = atoi(buff);
                        filein.getline(buff,50);
                        int y = atoi(buff);
                        filein.getline(buff,50);
                        string adress = buff;
                        filein.getline(buff,50);
                        int w_scr = atoi(buff);
                        filein.getline(buff,50);
                        int h_scr = atoi(buff);

                        for(int i = 0; i<cont_pic; i++)
                        {
                            if(menupic[i].adress == adress)
                           {

                                 centpic[n_sent_pic] ={ x,y,
                                                        menupic[i].adress,
                                                        menupic[i].pic,
                                                        w_scr,
                                                        h_scr,
                                                        menupic[i].w,
                                                        menupic[i].h,
                                                        true,
                                                        menupic[i].category};
                                 n_sent_pic ++;
                            }
                        }
                    }
                    filein.close();
            }

            if(btn[btn_help].click())
            {
              page = page_help;
              btn[btn_help].name = "������";
              txSleep(100);
            }
    }
    //����� �������
    else if (page == page_help)
    {

        txSelectFont("Times New Roman", 42);
        txSetColor(TX_BLACK);
        txDrawText(0,200, 1200 , 600,
                   "�������� '�������� ����� �������'\n"
                   "������� ��������� ��� ����������\n"
                   "�������� ������������ ��������� � ������\n"
                   "�������� ����� ��������� '+',���������'-'\n"
                   "���������� ����� ���������\n"
                   "���������� ����� ���������\n"
                   "���������� ����� ����������\n"
                   "����� �� �������� �� �� '�����'");
        btn[btn_help].draw();
        if(btn[btn_help].click())
        {
              page = page_redactor;
               btn[btn_help].name = "�������" ;
              txSleep(100);
        }
    }

     if(btn[btn_prstc].click())
     {
        ScreenCapture(250, 100, 700, 600, "1.bmp", txWindow());
            txMessageBox("��������� � 1.bmp");
     }

    txEnd();
    txSleep(50);

}
    for (int i=0; i<cont_pic; i++)
    {
     txDeleteDC(menupic[i].pic);
    }
    for (int i=0; i<n_sent_pic; i++)
    {
     txDeleteDC(centpic[i].pic);
    }
     txDeleteDC(Fon);

return 0;
}
