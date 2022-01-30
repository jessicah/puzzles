#include <Application.h>
#include <Rect.h>
#include <Screen.h>
#include <View.h>
#include <Window.h>

#include <stdio.h>
#include <stdlib.h>

#include "puzzles.h"


class PuzzleView : public BView {
public:
			PuzzleView(BRect frame);
	virtual	~PuzzleView();

	void	AttachedToWindow();
	void	MessageReceived(BMessage *message);
	void	Pulse();

private:
	// controls here
};

class PuzzleWindow : public BWindow {
public:
			PuzzleWindow(BRect frame);
	virtual	~PuzzleWindow();
};

class PuzzleApp : public BApplication {
public:
			PuzzleApp();
	virtual	~PuzzleApp();

	void	ReadyToRun();
	void	MessageReceived(BMessage *message);

private:
	PuzzleWindow*	fPuzzleWindow;
};

// PuzzleView

PuzzleView::PuzzleView(BRect frame)
	: BView(frame, "PuzzleView", B_FOLLOW_ALL, B_PULSE_NEEDED)
{
	SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
	SetLowUIColor(ViewUIColor());
}


PuzzleView::~PuzzleView()
{
}

void
PuzzleView::AttachedToWindow()
{
}


void
PuzzleView::MessageReceived(BMessage *message)
{
	switch (message->what) {
		default:
			message->PrintToStream();
			BView::MessageReceived(message);
	}
}


void
PuzzleView::Pulse()
{
	BString info;
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	// TODO: use strftime and locale settings
	info << asctime(t);
	info.RemoveSet("\r\n");
	//fInfoView->SetText(info.String());
}


// PuzzleWindow


PuzzleWindow::PuzzleWindow(BRect frame)
	: BWindow(frame, "Portable Puzzle Collection",
		B_TITLED_WINDOW_LOOK, 
		B_NORMAL_WINDOW_FEEL, 
		//B_NOT_MOVABLE | B_NOT_CLOSABLE | B_NOT_ZOOMABLE | 
		//B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | 
		B_ASYNCHRONOUS_CONTROLS,
		B_ALL_WORKSPACES)
{
	PuzzleView *v = new PuzzleView(Bounds());
	AddChild(v);
	SetPulseRate(1000000LL);
}


PuzzleWindow::~PuzzleWindow()
{
}


// PuzzleApp


PuzzleApp::PuzzleApp()
	:
	BApplication("application/x-vnd.Jessica.L.Hamilton-PortablePuzzleCollection")
{
}


PuzzleApp::~PuzzleApp()
{
}


void
PuzzleApp::ReadyToRun()
{
	float sizeDelta = (float)be_plain_font->Size()/12.0f;
	BRect frame(0, 0, 450 * sizeDelta, 150 * sizeDelta);
	// frame.OffsetBySelf(screen.Frame().Width()/2 - frame.Width()/2,
	// 	screen.Frame().Height()/2 - frame.Height()/2);
	fPuzzleWindow = new PuzzleWindow(frame);
	fPuzzleWindow->Show();
}


void
PuzzleApp::MessageReceived(BMessage *message)
{
	switch (message->what) {
		default:
			BApplication::MessageReceived(message);
	}
}


// global functions


void fatal(const char *fmt, ...)
{
    va_list ap;

    fprintf(stderr, "fatal error: ");

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");
    exit(1);
}


void get_random_seed(void **randseed, int *randseedsize)
{
    struct timeval *tvp = snew(struct timeval);
    gettimeofday(tvp, NULL);
    *randseed = (void *)tvp;
    *randseedsize = sizeof(struct timeval);
}


void activate_timer(frontend *fe)
{
    if (!fe)
		return;			       /* can happen due to --generate */
    
	if (!fe->timer_active) {
        //fe->timer_id = g_timeout_add(20, timer_func, fe);
		gettimeofday(&fe->last_time, NULL);
    }

    fe->timer_active = true;
}


void deactivate_timer(frontend *fe)
{
    if (!fe)
		return;			       /* can happen due to --generate */

    if (fe->timer_active)
        ;//g_source_remove(fe->timer_id);

    fe->timer_active = false;
}


/*
 * Since this front end does not support printing (yet), we need
 * this stub to satisfy the reference in midend_print_puzzle().
 */
void document_add_puzzle(document *doc, const game *game, game_params *par,
		game_state *st, game_state *st2)
{
}


void frontend_default_colour(frontend *fe, float *output)
{
	output[0] = output[1] = output[2] = 0.9F;
}


extern "C"
int main(int, char **)
{
	PuzzleApp app;
	app.Run();

	return 0;
}
