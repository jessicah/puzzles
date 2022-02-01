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
	void	MouseDown(BPoint point);
	void	MouseUp(BPoint point);
private:
	// controls here
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

	const rgb_color& get_colour(int index)
	{
		return colours[index];
	}
};


struct frontend haiku_api {
	// draw_text
	[](void *self, int x, int y, int fontType,
		int fontSize, int align, int colour, const char *text)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		frontEnd->view->SetHighColor(frontEnd->get_colour(colour));

		// could probably calculate this once
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

	// draw_rect
	[](void *self, int x, int y, int w, int h, int colour)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		frontEnd->view->SetHighColor(frontEnd->get_colour(colour));
		frontEnd->view->FillRect(BRect(x, y, x+w-1, y+h-1));
	},

	// draw_line
	[](void *self, int x1, int y1, int x2, int y2, int colour)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		frontEnd->view->SetHighColor(frontEnd->get_colour(colour));
		frontEnd->view->StrokeLine(BPoint(x1, y1), BPoint(x2, y2));
		//printf("draw line: (%d,%d) => (%d,%d)\n", x1, y1, x2, y2);
	},

	// draw_polygon
	[](void *self, const int *coords, int numPoints,
			int fillColour, int strokeColour)
	{

	},

	// draw_circle
	[](void *self, int cx, int cy, int radius,
			int fillColour, int strokeColour)
	{
	},

	// draw_update
	[](void *self, int x, int y, int w, int h)
	{

	},

	// clip
	[](void *self, int x, int y, int w, int h)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		frontEnd->view->ClipToRect(BRect(x, y, w+x, h+y));
	},

	// unclip
	[](void *self)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		frontEnd->view->ConstrainClippingRegion(NULL);
	},

	// start_draw
	[](void *self)
	{

	},

	// end_draw
	[](void *self)
	{

	},

	// status_bar
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


void
PuzzleView::MouseDown(BPoint point)
{
	int32 buttons = 0;
	if (Window() != NULL && Window()->CurrentMessage() != NULL) {
		Window()->CurrentMessage()->FindInt32("buttons", &buttons);
	}

	if (buttons == 0)
		return;

	int translatedButton = 0;
	if (buttons & B_PRIMARY_MOUSE_BUTTON)
		translatedButton |= LEFT_BUTTON;
	if (buttons & B_SECONDARY_MOUSE_BUTTON)
		translatedButton |= RIGHT_BUTTON;
	
	midend_process_key(haiku_api.midEnd, point.x, point.y, translatedButton);
}


void
PuzzleView::MouseUp(BPoint point)
{
	int32 buttons = 0;
	if (Window() != NULL && Window()->CurrentMessage() != NULL) {
		Window()->CurrentMessage()->FindInt32("buttons", &buttons);
	}

	if (buttons == 0)
		return;

	int translatedButton = 0;
	if (buttons & B_PRIMARY_MOUSE_BUTTON)
		translatedButton |= LEFT_RELEASE;
	if (buttons & B_SECONDARY_MOUSE_BUTTON)
		translatedButton |= RIGHT_RELEASE;
	
	midend_process_key(haiku_api.midEnd, point.x, point.y, translatedButton);
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
	haiku_api.colours = new rgb_color[numColours];
	for (int i = 0; i < numColours; ++i)
	{
		haiku_api.colours[i].red   = 255 * colours[i * 3 + 0];
		haiku_api.colours[i].green = 255 * colours[i * 3 + 1];
		haiku_api.colours[i].blue  = 255 * colours[i * 3 + 2];
	}

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
