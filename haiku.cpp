#include <Application.h>
#include <Rect.h>
#include <View.h>
#include <Window.h>

#include <stdio.h>
#include <stdlib.h>

#include "puzzles.h"


class PuzzleView;
class PuzzleWindow;


struct frontend {
	midend *midEnd;

	bool timer_active;
	struct timeval last_time;
};


class PuzzleView : public BView {
private:
			PuzzleView(BRect frame);
public:
	virtual	~PuzzleView();

	void	AttachedToWindow();
	void	MessageReceived(BMessage *message);
	void	Pulse();

	static PuzzleView* Self();

	static frontend* Frontend();

	static void		DrawText(void *view, int x, int y, int fonttype,
						int fontsize, int align, int colour, const char *text);
	static void		DrawRect(void *view, int x, int y, int w, int h, int colour);
	static void		DrawLine(void *view, int x1, int y1, int x2, int y2, int colour);
	static void		DrawPolygon(void *view, const int *coords, int npoints,
						int fillcolour, int outlinecolour);
	static void		DrawCircle(void *view, int cx, int cy, int radius, int fillcolour,
						int outlinecolour);
	static void		DrawUpdate(void *view, int x, int y, int w, int h);
	static void		Clip(void *view, int x, int y, int w, int h);
	static void		Unclip(void *view);
	static void		StartDraw(void *view);
	static void		EndDraw(void *view);
	static void		StatusBar(void *view, const char *text);
	static blitter* BlitterNew(void *view, int w, int h);
	static void		BlitterFree(void *view, blitter *bl);
	static void		BlitterSave(void *view, blitter *bl, int x, int y);
	static void		BlitterLoad(void *view, blitter *bl, int x, int y);
	// text_fallback
	// draw_thick_line
private:
	// controls here
};


const struct drawing_api haiku_drawing = {
    PuzzleView::DrawText,
    PuzzleView::DrawRect,
    PuzzleView::DrawLine,
    PuzzleView::DrawPolygon,
    PuzzleView::DrawCircle,
    PuzzleView::DrawUpdate,
    PuzzleView::Clip,
    PuzzleView::Unclip,
    PuzzleView::StartDraw,
    PuzzleView::EndDraw,
    PuzzleView::StatusBar,
    PuzzleView::BlitterNew,
    PuzzleView::BlitterFree,
    PuzzleView::BlitterSave,
    PuzzleView::BlitterLoad,
	// printing
    NULL, NULL, NULL, NULL, NULL, NULL, /* {begin,end}_{doc,page,puzzle} */
    NULL, NULL,			       /* line_width, line_dotted */
    NULL,
    NULL,
};


// PuzzleWindow


class PuzzleWindow : public BWindow {
public:
			PuzzleWindow(BRect frame);
	virtual	~PuzzleWindow();

	frontend *Frontend() const { return fFrontEnd; }

private:
	frontend *fFrontEnd;
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


// static member functions


static PuzzleView *sPuzzleView = NULL;


PuzzleView*
PuzzleView::Self()
{
	if (sPuzzleView == NULL) {
		sPuzzleView = new PuzzleView(BRect(0, 0, 450, 150));
	}

	return sPuzzleView;
}


frontend*
PuzzleView::Frontend()
{
	return static_cast<PuzzleWindow*>(Self()->Window())->Frontend();
}


void
PuzzleView::DrawText(void *handle, int x, int y, int fonttype, int fontsize,
	int align, int colour, const char *text)
{

}


void
PuzzleView::DrawRect(void *handle, int x, int y, int w, int h, int colour)
{
	/*Self()->SetHighColor(
		Frontend()->colours[3 * colour + 0],
		Frontend()->colours[3 * colour + 1],
		Frontend()->colours[3 * colour + 2]
	);*/
	Self()->FillRect(BRect(x, y, w, h));
}


void
PuzzleView::DrawLine(void *view, int x1, int y1, int x2, int y2, int colour)
{

}

void
PuzzleView::DrawPolygon(void *view, const int *coords, int npoints,
	int fillcolour, int outlinecolour)
{

}
	
void
PuzzleView::DrawCircle(void *view, int cx, int cy, int radius, int fillcolour,
	int outlinecolour)
{

}


void
PuzzleView::DrawUpdate(void *view, int x, int y, int w, int h)
{

}


void
PuzzleView::Clip(void *view, int x, int y, int w, int h)
{

}


void
PuzzleView::Unclip(void *view)
{

}


void
PuzzleView::StartDraw(void *view)
{

}


void
PuzzleView::EndDraw(void *view)
{

}


void
PuzzleView::StatusBar(void *view, const char *text)
{

}


blitter*
PuzzleView::BlitterNew(void *view, int w, int h)
{
	return NULL;
}


void
PuzzleView::BlitterFree(void *view, blitter *bl)
{

}


void
PuzzleView::BlitterSave(void *view, blitter *bl, int x, int y)
{

}


void
PuzzleView::BlitterLoad(void *view, blitter *bl, int x, int y)
{

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
	PuzzleView *v = PuzzleView::Self();
	AddChild(v);

	// we don't need to pulse unless a timer is requested
	//SetPulseRate(1000000LL);

	fFrontEnd = snew(frontend);
	memset(fFrontEnd, 0, sizeof(frontend));

	fFrontEnd->midEnd = midend_new(fFrontEnd, &thegame, &haiku_drawing, fFrontEnd);
	midend_new_game(fFrontEnd->midEnd);

	int numColours;
	float *colours;

	colours = midend_colours(fFrontEnd->midEnd, &numColours);

	int x, y;
	x = 1000; y = 1000;
	midend_size(fFrontEnd->midEnd, &x, &y, false);

	ResizeTo(x, y);
	CenterOnScreen();

	midend_force_redraw(fFrontEnd->midEnd);
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
