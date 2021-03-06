/*--------------------------------------------------------------
   BIGJOB3.C -- Peek Message approach to lengthy processing job
 ---------------------------------------------------------------*/

#define INCL_WIN

#include <os2.h>
#include <math.h>
#include <stdio.h>
#include "bigjob.h"

INT main (VOID)
     {
     static CHAR szClassName [] = "BigJob3" ;
     HMQ         hmq ;
     HWND        hwndFrame, hwndClient ;
     QMSG        qmsg ;
     ULONG       flCreateFlags = FCF_SIZEBORDER|FCF_TITLEBAR|FCF_SYSMENU|
					 FCF_MINMAX|FCF_MENU|FCF_SHELLPOSITION;

     hab = WinInitialize (NULL) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClassName, ClientWndProc,
                            CS_SIZEREDRAW, 0) ;

     hwndFrame = WinCreateStdWindow (HWND_DESKTOP,
                    WS_VISIBLE, &flCreateFlags,
                    szClassName, "BigJob Demo No. 3",
                    0L, NULL, ID_RESOURCE, &hwndClient) ;

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;

     return 0 ;
     }
	
MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1,
                                                     MPARAM mp2)
     {
     static BOOL   bContinueCalc = FALSE ;
     static SHORT  iStatus = STATUS_READY ;
     static SHORT  iCalcRep, iCurrentRep = IDM_10 ;
     static ULONG  lElapsedTime ;
     double        A ;
     SHORT         i ;
     QMSG          qmsg ;

     switch (msg)
          {
          case WM_COMMAND:

               switch (LOUSHORT (mp1))
                    {
                    case IDM_10:
                    case IDM_100:
                    case IDM_1000:
                    case IDM_10000:
                         CheckMenuItem (hwnd, iCurrentRep, FALSE) ;
                         iCurrentRep = LOUSHORT (mp1) ;
                         CheckMenuItem (hwnd, iCurrentRep, TRUE) ;
                         break ;

                    case IDM_START:
                         EnableMenuItem (hwnd, IDM_START, FALSE) ;
                         EnableMenuItem (hwnd, IDM_ABORT, TRUE) ;

                         iStatus = STATUS_WORKING ;
                         WinInvalidateRect (hwnd, NULL, FALSE) ;

                         iCalcRep = iCurrentRep ;
                         bContinueCalc = TRUE ;
                         lElapsedTime = WinGetCurrentTime (hab) ;

                         qmsg.msg = WM_NULL ;

                         for (A = 1.0, i = 0 ; i < iCalcRep ; i++)
                              {
                              A = Savage (A) ;

                              while (WinPeekMsg (hab, &qmsg, NULL, 0, 0,
                                                  PM_NOREMOVE))
                                   {
                                   if (qmsg.msg == WM_QUIT)
                                        break ;

                                   WinGetMsg (hab, &qmsg, NULL, 0, 0) ;
                                   WinDispatchMsg (hab, &qmsg) ;

                                   if (!bContinueCalc)
                                        break ;
                                   }

                              if (!bContinueCalc || qmsg.msg == WM_QUIT)
                                   break ;
                              }
                         lElapsedTime = WinGetCurrentTime (hab) - 
                                                  lElapsedTime ;

                         if (!bContinueCalc || qmsg.msg == WM_QUIT)
                              iStatus = STATUS_READY ;
                         else
                              iStatus = STATUS_DONE ;

                         WinInvalidateRect (hwnd, NULL, FALSE) ;

                         EnableMenuItem (hwnd, IDM_START, TRUE) ;
                         EnableMenuItem (hwnd, IDM_ABORT, FALSE) ;
                         break ;

                    case IDM_ABORT:
                         bContinueCalc = FALSE ;
                         break ;
     
                    default:
                         break ;
                    }
               break ;

          case WM_PAINT:
               PaintWindow (hwnd, iStatus, iCalcRep, lElapsedTime) ;
               break ;

          default:
               return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
          }
     return 0L ;
     }
