/**
 * This file is part of Patternshop Project.
 * 
 * Patternshop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Patternshop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Patternshop.  If not, see <http://www.gnu.org/licenses/>
*/

#include "PsMacApp.h"
#include "PsMacTools.h"
#include "PsWinProperties.h"
#include "PsController.h"

PsWinProperties*	PsWinProperties::instance = 0;

OSStatus PsWinPropertiesEvents(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinProperties *doc = (PsWinProperties*)userData;
	return doc->EventsHandler(myHandler, event);
}

#define TAB_ID 128
#define TAB_SIGNATURE 'tabs'

static int tabList[] = {3, 129, 130, 131};   // Tab UserPane IDs
static int lastTabIndex = -1;

static void SelectItemOfTabControl(ControlRef tabControl)
{
	ControlRef userPane;
	ControlRef selectedPane = NULL;
	ControlID controlID;
	UInt16 i;
	
	SInt16 index = GetControlValue(tabControl);
	
	if (lastTabIndex != -1 && lastTabIndex != index)
		PsWinProperties::Instance().OnTabChange(index);
	
	lastTabIndex = index;
	controlID.signature = TAB_SIGNATURE;
	
	for (i = 1; i < tabList[0] + 1; ++i)
	{
		controlID.id = tabList[i];
		GetControlByID(GetControlOwner(tabControl), &controlID, &userPane);
		
		if (i == index) 
		{
			selectedPane = userPane;
		} else 
		{
			SetControlVisibility(userPane,false,false);
			DisableControl(userPane);
		}
	}
	
	if (selectedPane != NULL) 
	{
		EnableControl(selectedPane);
		SetControlVisibility(selectedPane, true, true);
	}
	
	Draw1Control(tabControl);
}

void PsWinProperties::OnTabChange(UInt16 tabId)
{
	m_PatternProperties.OnShowWindow(tabId == 3);
}

static pascal OSStatus TabEventHandler(EventHandlerCallRef inHandlerRef, EventRef inEvent, void *inUserData)
{
	OSStatus result = eventNotHandledErr;
	
	ControlRef theControl;
	ControlID controlID;
	
	GetEventParameter(inEvent, kEventParamDirectObject, typeControlRef, NULL, sizeof( ControlRef ), NULL, &theControl );
	
	GetControlID(theControl, &controlID);
	
	// If this event didn't trigger a tab change, give somebody else a chance to handle it.
	if (controlID.id == TAB_ID && GetControlValue(theControl) != lastTabIndex) {
		result = noErr;
		SelectItemOfTabControl(theControl);
	}    
	
	return result;
}

static void InstallTabHandler(WindowRef window)
{
	EventTypeSpec	controlSpec = { kEventClassControl, kEventControlHit }; // event class, event kind
	ControlRef 		tabControl;
	ControlID 		controlID;
	
	// Find the tab control and install an event handler on it.
	controlID.signature = TAB_SIGNATURE;
	controlID.id = TAB_ID;
	GetControlByID(window, &controlID, &tabControl);
	
	InstallEventHandler(GetControlEventTarget(tabControl),
							  NewEventHandlerUPP(TabEventHandler),
							  1,
							  &controlSpec,
							  0,
							  NULL);
	
	//Select the active tab to start with.
	lastTabIndex = GetControlValue(tabControl);
	SelectItemOfTabControl(tabControl); 
}

static OSStatus OnBoundsChange(EventHandlerCallRef myHandler, EventRef event, void *userData)
{
	PsWinProperties *View = (PsWinProperties*)userData;
	GetEventParameter(event, kEventParamCurrentBounds, typeQDRectangle, NULL, sizeof(Rect), NULL, &View->bounds);
	return eventNotHandledErr;
}

PsWinProperties::PsWinProperties()
{
	OSStatus err = noErr;
	
	// events de la fenetre
	const EventTypeSpec myBoundsSpec[] = { kEventClassWindow, kEventWindowBoundsChanged };
	const EventTypeSpec kWindowEvents[] = { kEventClassCommand, kEventCommandProcess };
	
	//-- création de la fenetre
	err = CreateWindowFromNib(sNibRef, CFSTR("PsWinProperties"), &window);
   require_noerr( err, CantCreateWindow );
	
	//-- handle sur le déplacement de la fenetre
	InstallWindowEventHandler(window, NewEventHandlerUPP (OnBoundsChange),
										GetEventTypeCount (myBoundsSpec), myBoundsSpec, (void *) this, NULL);
	InstallWindowEventHandler(window, NewEventHandlerUPP(PsWinPropertiesEvents),
									  GetEventTypeCount(kWindowEvents), kWindowEvents, (void*)this, NULL );
	InstallTabHandler(window);
	
	m_matrixProperties.parent = this;
	m_imageProperties.parent = this;
	m_PatternProperties.parent = this;

	m_matrixProperties.InstallEventHandlers();
	m_imageProperties.InstallEventHandlers();
	m_PatternProperties.InstallEventHandlers();

	m_matrixProperties.Disable();
	m_imageProperties.Disable();
	m_PatternProperties.Disable();

CantCreateWindow:
	;
}

void PsWinProperties::UpdateValue(OSType signature, char *buffer)
{
	ControlRef control;
	ControlID myControlID;
	
	// protection
	if (!bNoProtect)
	{
		GetKeyboardFocus(window, &control);
		GetControlID(control, &myControlID);
		if (myControlID.signature == signature) 
			return;
	}
	
	// mise à jour
	myControlID.signature = signature;
	myControlID.id = 0;
	GetControlByID(window, &myControlID, &control);
	SetControlData(control, kControlNoPart, kControlEditTextTextTag, strlen(buffer), buffer);	
}

PsWinProperties&	PsWinProperties::Instance()
{
	if(!instance)
		instance = new PsWinProperties();
	return *instance;
}

PsWinProperties::~PsWinProperties()
{

}

void	PsWinProperties::Delete()
{
	if(instance)
	{
		delete instance;
		instance = 0;
	}
}

void PsWinProperties::Show()
{
	ShowWindow(window);
}

void PsWinProperties::GetLocation(PsPoint &p)
{
	Rect bound;
	GetWindowBounds(window, kWindowStructureRgn, &bound);
	p.x = bound.left;
	p.y = bound.top;
}

void PsWinProperties::SetLocation(int x, int y)
{
	Rect bound;
	GetWindowBounds(window, kWindowStructureRgn, &bound);
	int w = bound.right - bound.left;
	int h = bound.bottom - bound.top;
	bound.left = x;
	bound.right = x + w;
	bound.top = y;
	bound.bottom = y + h;
	SetWindowBounds(window, kWindowStructureRgn, &bound);
}

OSStatus PsWinProperties::EventsHandler(EventHandlerCallRef myHandler, EventRef event)
{
	switch (lastTabIndex)
	{
		case 1: return m_matrixProperties.EventsHandler(myHandler, event);
		case 2: return m_imageProperties.EventsHandler(myHandler, event);
		case 3: return m_PatternProperties.EventsHandler(myHandler, event);
		default: assert(false && "PsWinProperties out of index"); break;
	}
	return eventNotHandledErr;
}

void PsWinProperties::DisableControl(OSType signature)
{
	ControlRef control;
	ControlID controlId;
	controlId.id = 0;
	controlId.signature = signature;
	GetControlByID(window, &controlId, &control);
	DeactivateControl(control);
}

void PsWinProperties::EnableControl(OSType signature)
{
	ControlRef control;
	ControlID controlId;
	controlId.id = 0;
	controlId.signature = signature;
	GetControlByID(window, &controlId, &control);
	ActivateControl(control);
}

void PsWinProperties::UpdateInformation(PsProject *project)
{
	if (!project)
	{
		m_imageProperties.Update(NULL);
		m_matrixProperties.Update(NULL);
	}
	else
	{
		m_imageProperties.Update(project->image);
		m_matrixProperties.Update(project->matrix);
	}
	m_PatternProperties.Update();
}

void PsWinProperties::FocusImageInformation()
{
	ControlRef tabControl;
	ControlID controlID;
	controlID.signature = TAB_SIGNATURE;
	controlID.id = TAB_ID;
	GetControlByID(window, &controlID, &tabControl);
	SetControlValue(tabControl, 2);
	SelectItemOfTabControl(tabControl);
}

void PsWinProperties::FocusMatrixInformation()
{
	ControlRef tabControl;
	ControlID controlID;
	controlID.signature = TAB_SIGNATURE;
	controlID.id = TAB_ID;
	GetControlByID(window, &controlID, &tabControl);
	SetControlValue(tabControl, 1);
	SelectItemOfTabControl(tabControl);
}
