/*
 * README: Portions of this file are merged at file generation
 * time. Edits can be made *only* in between specified code blocks, look
 * for keywords <Begin user code> and <End user code>.
 */
/*
 * Generated by the ICS Builder Xcessory (BX).
 *
 * Builder Xcessory Personal Edition Version 5.0.5
 * Code Generator Xcessory 5.0.3 (08/18/99) Script Version 5.0.6
 *
 */


/* Begin user code block <file_comments> */
/* End user code block <file_comments> */

/*
 * Motif required Headers
 */
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#if (XmVersion >= 1002)
#include <Xm/RepType.h>
#endif
#include <Xm/MwmUtil.h>

/*
 * Globally included information.
 */


/*
 * Headers for classes used in this program
 */

/*
 * Common constant and pixmap declarations.
 */
#include "mbnavedit_creation.h"

/*
 * Convenience functions from utilities file.
 */
extern void RegisterBxConverters(XtAppContext);
extern XtPointer BX_CONVERT(Widget, char *, char *, int, Boolean *);
extern XtPointer BX_DOUBLE(double);
extern XtPointer BX_SINGLE(float);
extern void BX_MENU_POST(Widget, XtPointer, XEvent *, Boolean *);
extern Pixmap XPM_PIXMAP(Widget, char **);
extern void BX_SET_BACKGROUND_COLOR(Widget, ArgList, Cardinal *, Pixel);

/*
 * Declarations for shell callbacks.
 */
extern void BxExitCB(Widget, XtPointer, XtPointer);

/* Begin user code block <globals> */
/* End user code block <globals> */

/*
 * Change this line via the Output Application Names Dialog.
 */
#define BX_APP_CLASS "mbnavedit"

int main( int argc, char **argv)
{
    Widget       parent;
    XtAppContext app;
    Arg          args[256];
    Cardinal     ac;
    Boolean      argok=False;
    Widget   topLevelShell;
    Widget   mainWindow;
    
    /* Begin user code block <declarations> */
    /* End user code block <declarations> */
    
    /*
     * The applicationShell is created as an unrealized
     * parent for multiple topLevelShells.  The topLevelShells
     * are created as popup children of the applicationShell.
     * This is a recommendation of Paul Asente & Ralph Swick in
     * _X_Window_System_Toolkit_ p. 677.
     */
    
    parent = XtVaAppInitialize ( &app, BX_APP_CLASS, NULL, 0, 
#ifndef XlibSpecificationRelease
    (Cardinal *) &argc, 
#else
#if (XlibSpecificationRelease>=5)
    &argc, 
#else
    (Cardinal *) &argc, 
#endif
#endif
    argv, NULL, 
    NULL );
    
    RegisterBxConverters(app);
#if (XmVersion >= 1002) 
    XmRepTypeInstallTearOffModelConverter();
#endif
    
    /* Begin user code block <create_shells> */
    /* End user code block <create_shells> */
    
    /*
     * Create classes and widgets used in this program. 
     */
    
    /* Begin user code block <create_topLevelShell> */
    /* End user code block <create_topLevelShell> */
    
    ac = 0;
    XtSetArg(args[ac], XmNtitle, "MBnaveditold"); ac++;
    XtSetArg(args[ac], XmNiconName, "MBnaveditold"); ac++;
    XtSetArg(args[ac], XmNx, 10); ac++;
    XtSetArg(args[ac], XmNy, 29); ac++;
    XtSetArg(args[ac], XmNwidth, 1020); ac++;
    XtSetArg(args[ac], XmNheight, 676); ac++;
    topLevelShell = XtCreatePopupShell("topLevelShell",
        topLevelShellWidgetClass,
        parent,
        args, 
        ac);
    XtAddCallback(topLevelShell, XmNdestroyCallback, BxExitCB, (XtPointer)0);
    mainWindow = (Widget)CreatemainWindow(topLevelShell);
    XtManageChild(mainWindow);
    XtPopup(XtParent(mainWindow), XtGrabNone);
    
    /* Begin user code block <app_procedures> */
    
    /* initialize app value and wait until view realized */
    do_wait_until_viewed(app);
    
    /* initialize everything */
    do_mbnavedit_init(argc,argv);
    
    /* End user code block <app_procedures> */
    
    /* Begin user code block <main_loop> */
    /* End user code block <main_loop> */
    
    XtAppMainLoop(app);
    
    /*
     * A return value regardless of whether or not the main loop ends. 
     */
     return(0); 
}
