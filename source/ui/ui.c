#include "ui/ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <switch.h>

/*
Redesign ideas:

NOTE:

Each scene:
- Should possess its own result
- Should have a pointer to the previous scene (if there is one)
- There should be a "GetPriorResults" function that creates a vector of all results prior to the current scene

A scene source:
- Has a "full" size
- Is implemented as a stack, where elements are popped and put into the scene manager instead

The scene manager should:
    - Default to the main menu scene
    - Delegate input to the current scene
    - Wait until the current scene has a result, then act accordingly. Results may be:
        - Progress to the next scene, storing result
        - Return to previous scene
        - Error
        - Return to main menu
    - Should have a reset state method (for returning to main menu)
    - May have a SceneSource to provide a predefined set of scenes
    - Should have a scene stack. Elements should first be returned to the scene source (if present). Once the scene source is full,
      it should be deleted when trying to add a new element.
*/

PrintConsole *g_console;
View *g_currentView;

void pushView(View *view)
{
    View *oldView = g_currentView;

    //If there is already a view being shown, link the new one to it
    if (g_currentView)
        view->prevView = oldView;

    g_currentView = view;

    // Find the first selectable menu entry
    for (unsigned char i = 0; i < g_currentView->numEntries; i++)
    {
        ViewEntry entry = g_currentView->viewEntries[i];

        if (entry.type == ViewEntryType_Select)
        {
            view->cursorPos = i;
            break;
        }
    }

    displayCurrentView();
}

void unwind(void)
{
    // The base view will not have a previous view, and thus we should not pop it
    if (!g_currentView->prevView)
        return;

    View *oldView = g_currentView;

    // Set the current view to the view which originally created the current one
    // Deallocate the old current view
    g_currentView = oldView->prevView;
    free(oldView);

    // Note: We should not need to setup the cursor position again as it should be preserved
    // from when the view was used previously
    displayCurrentView();
}

void moveCursor(signed char off)
{
    if (off != -1 && off != 1)
        return;

    clearCursor();

    for (unsigned char i = 0; i < g_currentView->numEntries; i++)
    {
        signed char newCursorPos = (g_currentView->cursorPos + i * off + off);

        // Negative numbers should wrap back around to the end
        if (newCursorPos < 0)
            newCursorPos = g_currentView->numEntries + newCursorPos;

        // Numbers greater than the number of entries should wrap around to the start
        newCursorPos %= g_currentView->numEntries;

        ViewEntry entry = g_currentView->viewEntries[newCursorPos];
    
        if (entry.type == ViewEntryType_Select)
        {
            g_currentView->cursorPos = newCursorPos;
            break;
        }
    }

    displayCursor();
}

void onButtonPress(u64 kDown)
{
    if (kDown & KEY_DOWN)
        moveCursor(1);
    else if (kDown & KEY_UP)
        moveCursor(-1);
    else if (kDown & KEY_A)
    {
        ViewEntry curViewEntry = g_currentView->viewEntries[g_currentView->cursorPos];

        if (curViewEntry.onSelected != NULL && curViewEntry.type == ViewEntryType_Select)
            curViewEntry.onSelected();
    }
    else if (kDown & KEY_B)
        unwind();
}

void displayCurrentView(void)
{
    consoleClear();

    for (unsigned char i = 0; i < g_currentView->numEntries; i++)
    {
        ViewEntry entry = g_currentView->viewEntries[i];

        switch (entry.type)
        {
            case ViewEntryType_Heading:
                g_console->flags |= CONSOLE_COLOR_BOLD;
                printf("%s\n", entry.text);
                g_console->flags &= ~CONSOLE_COLOR_BOLD;
                break;

            case ViewEntryType_SelectInactive:
                g_console->flags |= CONSOLE_COLOR_FAINT;
                printf("  %s\n", entry.text);
                g_console->flags &= ~CONSOLE_COLOR_FAINT;
                break;

            case ViewEntryType_Select:
                printf("  %s\n", entry.text);
                break;

            default:
                printf("\n");
        }
    }

    displayCursor();
}

void displayCursor(void)
{
    g_console->cursorX = 0;
    g_console->cursorY = g_currentView->cursorPos;
    g_console->flags |= CONSOLE_COLOR_BOLD;
    printf("> ");
    g_console->flags &= ~CONSOLE_COLOR_BOLD;
}

void clearCursor(void)
{
    g_console->cursorX = 0;
    g_console->cursorY = g_currentView->cursorPos;
    printf("  ");
}