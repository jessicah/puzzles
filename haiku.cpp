#include <Application.h>
#include <Rect.h>
#include <View.h>
#include <Window.h>

#include <stdio.h>
#include <stdlib.h>

#include "puzzles.h"


class PuzzleView;
class PuzzleWindow;





class PuzzleView : public BView {
public:
			PuzzleView(BRect frame);
	virtual	~PuzzleView();

	void	AttachedToWindow();
	void	MessageReceived(BMessage *message);
	void	Pulse();
	void	Draw(BRect updateRect);

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
    NULL,//PuzzleView::BlitterNew,
    NULL,//PuzzleView::BlitterFree,
    NULL,//PuzzleView::BlitterSave,
    NULL,//PuzzleView::BlitterLoad,
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

private:
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
	: BView(frame, "PuzzleView", B_FOLLOW_ALL, B_PULSE_NEEDED | B_WILL_DRAW)
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


void
PuzzleView::DrawText(void *handle, int x, int y, int fonttype, int fontsize,
	int align, int colour, const char *text)
{
	
}


void
PuzzleView::DrawRect(void *handle, int x, int y, int w, int h, int colour)
{
	
}


void
PuzzleView::DrawLine(void *view, int x1, int y1, int x2, int y2, int colour)
{
	//Self()->SetHighColor(0, 255, 0);
	//Self()->StrokeLine(BPoint(x1, y1), BPoint(x2, y2));
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


// haiku API


struct frontend : drawing_api {
	midend *midEnd;

	bool timer_active;
	struct timeval last_time;

	PuzzleView *view;

	rgb_color *colours;
};

struct frontend haiku_api {
	[](void *self, int x, int y, int fontType,
		int fontSize, int align, int colour, const char *text)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		rgb_color fore;
		fore.set_to(16, 16, 16);
		frontEnd->view->SetHighColor(fore);

		BFont font(be_plain_font);
		int textWidth = font.StringWidth(text);
		font_height fontHeight;
		font.GetHeight(&fontHeight);

		BPoint startPoint(x, y);

		if (align & ALIGN_VCENTRE)
		{
			// offset y, so that it's middle of capitalised text
			startPoint.y -= fontHeight.ascent / 2;
		}

		if (align & ALIGN_HRIGHT)
		{
			startPoint.x -= textWidth;
		}
		else if (align & ALIGN_HCENTRE)
		{
			startPoint.x -= textWidth / 2;
		}

		frontEnd->view->DrawString(text, startPoint);
	},

	[](void *self, int x, int y, int w, int h, int colour)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		/*Self()->SetHighColor(
			Frontend()->colours[3 * colour + 0],
			Frontend()->colours[3 * colour + 1],
			Frontend()->colours[3 * colour + 2]
		);*/
		rgb_color fore;
		fore.set_to(0, 255, 0);
		switch (colour) {
			case 1: fore.set_to(0, 0, 0); break;
			case 8: fore.set_to(64, 127, 88); break;
			case 7: fore.set_to(127, 88, 64); break;
			case 32: fore.set_to(255, 255, 255); break;
			case 31: fore.set_to(222, 222, 222); break;
			case 20: fore.set_to(0, 0, 200); break;
			default: fore.set_to(0, 255, 255); break;
		}
		frontEnd->view->SetHighColor(fore);
		frontEnd->view->MovePenTo(0., 0.);
		frontEnd->view->FillRect(BRect(x, y, x+w, y+h));
	},

	[](void *self, int x1, int y1, int x2, int y2, int colour)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		frontEnd->view->SetHighColor(0, 255, 0);
		frontEnd->view->StrokeLine(BPoint(x1, y1), BPoint(x2, y2));
	},

	[](void *self, const int *coords, int numPoints,
			int fillColour, int strokeColour)
	{

	},

	[](void *self, int cx, int cy, int radius,
			int fillColour, int strokeColour)
	{
	},

	[](void *self, int x, int y, int w, int h)
	{

	},

	[](void *self, int x, int y, int w, int h)
	{

	},

	[](void *self)
	{

	},

	[](void *self)
	{

	},

	[](void *self)
	{

	},

	[](void *self, const char *text)
	{

	},

	NULL, // blitter_new
	NULL, // blitter_free
	NULL, // blitter_save
	NULL, // blitter_load
	// printing
    NULL, NULL, NULL, NULL, NULL, NULL, /* {begin,end}_{doc,page,puzzle} */
    NULL, NULL,			       /* line_width, line_dotted */
    NULL,
    NULL,
};


void
PuzzleView::Draw(BRect updateRect)
{
	midend_force_redraw(haiku_api.midEnd);
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
	PuzzleView *view = new PuzzleView(frame);
	

	// we don't need to pulse unless a timer is requested
	//SetPulseRate(1000000LL);

	haiku_api.view = view;
	haiku_api.midEnd = midend_new(&haiku_api, &thegame, &haiku_api, &haiku_api);
	midend_new_game(haiku_api.midEnd);

	int numColours;
	float *colours;

	colours = midend_colours(haiku_api.midEnd, &numColours);

	int x, y;
	x = 1000; y = 1000;
	midend_size(haiku_api.midEnd, &x, &y, false);

	AddChild(view);
	ResizeTo(x, y);
	CenterOnScreen();
}


PuzzleWindow::~PuzzleWindow()
{
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
