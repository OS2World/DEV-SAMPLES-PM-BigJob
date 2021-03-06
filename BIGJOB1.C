/*-------------------------------------------------------
   BIGJOB1.C -- Naive approach to lengthy processing job
 --------------------------------------------------------*/

#define INCL_WIN

#include <os2.h>
#include <math.h>
#include <stdio.h>
#include "bigjob.h"

INT main (VOID)
     {
     static CHAR szClassName [] = "BigJob1" ;
     HMQ         hmq ;
     HWND        hwndFrame, hwndClient ;
     QMSG        qmsg ;
	 ULONG       flCreateFlags ;

     hab = WinInitialize (NULL) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (hab, szClassName, ClientWndProc,
                            CS_SYNCPAINT | CS_SIZEREDRAW, 0 ) ;

	 flCreateFlags = FCF_SIZEBORDER|FCF_TITLEBAR|FCF_SYSMENU|FCF_MINMAX|
		 FCF_MENU|FCF_SHELLPOSITION;
     hwndFrame = WinCreateStdWindow (HWND_DESKTOP,
                    WS_VISIBLE, &flCreateFlags,
                    szClassName, "BIGJOB Demo No. 1",
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
     static SHORT iCalcRep, iCurrentRep = IDM_10 ;
     static SHORT iStatus = STATUS_READY ;
     static ULONG lElapsedTime ;
     double       A ;
     SHORT        i ;

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
                         lElapsedTime = WinGetCurrentTime (hab) ;

                         for (A = 1.0, i = 0 ; i < iCalcRep ; i++)
                              A = Savage (A) ;

                         lElapsedTime = WinGetCurrentTime (hab) -
                                        lElapsedTime ;

                         iStatus = STATUS_DONE ;
                         WinInvalidateRect (hwnd, NULL, FALSE) ;

                         EnableMenuItem (hwnd, IDM_START, TRUE) ;
                         EnableMenuItem (hwnd, IDM_ABORT, FALSE) ;
                         break ;

                    case IDM_ABORT:

                              /* Not much we can do here */

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
