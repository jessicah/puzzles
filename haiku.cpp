#include <Application.h>
#include <LayoutBuilder.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Picture.h>
#include <Rect.h>
#include <View.h>
#include <Window.h>

#include <stdio.h>
#include <stdlib.h>

#include "puzzles.h"


const uint32 GAME_TYPE = 'gmtp';


class PuzzleView : public BView {
public:
			PuzzleView();
	virtual	~PuzzleView();

	void	AttachedToWindow();
	void	MessageReceived(BMessage *message);
	void	Pulse();
	void	Draw(BRect updateRect);
	void	GetPreferredSize(float *_width, float *_height);
private:
	// controls here
};


// PuzzleWindow


class PuzzleWindow : public BWindow {
public:
			PuzzleWindow(BRect frame);
	virtual	~PuzzleWindow();

	void	MessageReceived(BMessage *message);
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


PuzzleView::PuzzleView()
	: BView("PuzzleView", B_PULSE_NEEDED | B_WILL_DRAW)
{
	SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
	SetLowUIColor(ViewUIColor());
}


PuzzleView::~PuzzleView()
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

	bigtime_t delta;

	PuzzleView *view;

	rgb_color *colours;

	int32 button_state;

	BPicture *picture;

	const rgb_color& get_colour(int index)
	{
		return colours[index];
	}

	template <typename Func>
	void draw(const Func& func)
	{
		view->AppendToPicture(picture);
		func();
		picture = view->EndPicture();
		view->Invalidate();
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
			startPoint.y += fontHeight.ascent / 2;
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
	},

	// draw_polygon
	[](void *self, const int *coords, int numPoints,
			int fillColour, int strokeColour)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		BPoint points[numPoints] = {};
		for (int i = 0; i < numPoints; ++i)
			points[i] = BPoint(coords[i*2+0], coords[i*2+1]);

		if (fillColour >= 0) {
			frontEnd->view->SetHighColor(frontEnd->get_colour(fillColour));
			frontEnd->view->FillPolygon(points, numPoints);
		}

		frontEnd->view->SetHighColor(frontEnd->get_colour(strokeColour));
		frontEnd->view->StrokePolygon(points, numPoints);
	},

	// draw_circle
	[](void *self, int cx, int cy, int radius,
			int fillColour, int strokeColour)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		if (fillColour >= 0) {
			frontEnd->view->SetHighColor(frontEnd->get_colour(fillColour));
			frontEnd->view->FillEllipse(BPoint(cx, cy), radius, radius);
		}

		frontEnd->view->SetHighColor(frontEnd->get_colour(strokeColour));
		frontEnd->view->StrokeEllipse(BPoint(cx, cy), radius, radius);
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
PuzzleView::AttachedToWindow()
{
	haiku_api.draw([] {
		midend_redraw(haiku_api.midEnd);
	});
}


void
PuzzleView::Pulse()
{
	/*BString info;
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	// TODO: use strftime and locale settings
	info << asctime(t);
	info.RemoveSet("\r\n");
	//fInfoView->SetText(info.String());*/

	bigtime_t current = system_time();
	bigtime_t delta = current - haiku_api.delta;
	haiku_api.delta = current;

	haiku_api.draw([delta] {
		midend_timer(haiku_api.midEnd, ((float)delta) / 1000000.);
	});
}


void
PuzzleView::Draw(BRect updateRect)
{
	printf("in draw\n");
	//midend_redraw(haiku_api.midEnd);
	haiku_api.view->DrawPicture(haiku_api.picture, BPoint(0, 0));
	printf("out of draw\n");
}


void
PuzzleView::MessageReceived(BMessage *message)
{
	BPoint where;
	int32 buttons = 0, translatedButtons = 0;

	switch (message->what) {
		case B_MOUSE_DOWN:
		case B_MOUSE_UP:
			{
				message->FindPoint("where", &where);

				// mouse up only occurs when no buttons are depressed
				if (message->what == B_MOUSE_DOWN)
					message->FindInt32("buttons", &buttons);

				// if the state hasn't changed, do nothing
				if (buttons == haiku_api.button_state)
					return;

				// forward/back buttons for redo/undo
				if (buttons == B_MOUSE_BUTTON(4))
					if (midend_can_undo(haiku_api.midEnd))
						translatedButtons = UI_UNDO;
				if (buttons == B_MOUSE_BUTTON(5))
					if (midend_can_redo(haiku_api.midEnd))
						translatedButtons = UI_REDO;

				if (buttons & B_PRIMARY_MOUSE_BUTTON)
					translatedButtons |= LEFT_BUTTON;
				else if (haiku_api.button_state & B_PRIMARY_MOUSE_BUTTON)
					translatedButtons |= LEFT_RELEASE;

				if (buttons & B_SECONDARY_MOUSE_BUTTON)
					translatedButtons |= RIGHT_BUTTON;
				else if (haiku_api.button_state & B_SECONDARY_MOUSE_BUTTON)
					translatedButtons |= RIGHT_RELEASE;

				if (buttons & B_TERTIARY_MOUSE_BUTTON)
					translatedButtons |- MIDDLE_BUTTON;
				else if (haiku_api.button_state & B_TERTIARY_MOUSE_BUTTON)
					translatedButtons |= MIDDLE_RELEASE;

				haiku_api.button_state = buttons;

				haiku_api.draw([&where, translatedButtons] {
					midend_process_key(haiku_api.midEnd, where.x, where.y, translatedButtons);
				});

				return;
			}
		case B_MOUSE_MOVED:
			{
				message->FindPoint("where", &where);
				message->FindInt32("buttons", &buttons);

				if (buttons == 0)
					return;

				if (buttons & B_PRIMARY_MOUSE_BUTTON)
					translatedButtons |= LEFT_DRAG;
				if (buttons & B_SECONDARY_MOUSE_BUTTON)
					translatedButtons |= RIGHT_DRAG;

				haiku_api.draw([&where, translatedButtons] {
					midend_process_key(haiku_api.midEnd, where.x, where.y, translatedButtons);
				});

				return;
			}
		case B_KEY_UP:
		case B_KEY_DOWN:
			{
				int32 key;
				message->FindInt32("raw_char", &key);
				haiku_api.view->GetMouse(&where, NULL);

				haiku_api.draw([&where, key] {
					midend_process_key(haiku_api.midEnd, where.x, where.y, key);
				});

				return;
			}
		default:
			message->PrintToStream();
			BView::MessageReceived(message);
	}
}


void
PuzzleView::GetPreferredSize(float *_width, float *_height)
{
	int x, y;
	x = 1000; y = 1000;
	midend_size(haiku_api.midEnd, &x, &y, false);

	if (_width != NULL) *_width = x;
	if (_height != NULL) *_height = y;
}


// PuzzleWindow


PuzzleWindow::PuzzleWindow(BRect frame)
	: BWindow(frame, "Portable Puzzle Collection",
		B_TITLED_WINDOW_LOOK,
		B_NORMAL_WINDOW_FEEL,
		B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_QUIT_ON_WINDOW_CLOSE)
{
	PuzzleView *view = new PuzzleView();


	// we don't need to pulse unless a timer is requested
	//SetPulseRate(1000000LL);

	haiku_api.picture = new BPicture();
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

	// walk through the presets, and print them out, to see what we get :-)
	int menu_limit = 0;
	preset_menu *menu = NULL;
	preset_menu_entry *menu_entry = NULL;

	menu = midend_get_presets(haiku_api.midEnd, &menu_limit);

	BMenuBar *mainMenu = new BMenuBar("main menu");
	BMenu *typeMenu = new BMenu("Type");

	while (menu != NULL) {
		for (int i = 0; i < menu->n_entries; ++i) {
			menu_entry = &menu->entries[i];

			printf("%s\n", menu_entry->title);

			if (menu_entry->params == NULL) {
				printf("skipping menu entry, no game params: %s\n", menu_entry->title);
				continue;
			}

			BMessage *message = new BMessage(GAME_TYPE);
			message->AddInt32("index", menu_entry->id);
			message->AddString("name", menu_entry->title);
			message->AddPointer("params", menu_entry->params);

			BMenuItem *menuItem = new BMenuItem(menu_entry->title, message);

			menuItem->SetTarget(view);
			typeMenu->AddItem(menuItem);
		}

		menu = NULL;
	}

	mainMenu->AddItem(typeMenu);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(mainMenu)
		.Add(view)
		.End();

	CenterOnScreen();

	view->MakeFocus();

	ResizeToPreferred();

	haiku_api.draw([] {
		midend_force_redraw(haiku_api.midEnd);
	});
}


PuzzleWindow::~PuzzleWindow()
{
}


void
PuzzleWindow::MessageReceived(BMessage *message)
{
	switch (message->what) {
		case GAME_TYPE:
			{
				BString name;
				game_params *params = NULL;

				message->FindString("name", &name);
				message->FindPointer("params", (void**)&params);

				printf("requested new game type: %s\n", name.String());

				if (params == NULL) {
					printf("unable to locate game params\n");
					return;
				}

				midend_set_params(haiku_api.midEnd, params);
				midend_new_game(haiku_api.midEnd);

				InvalidateLayout(true);
				ResizeToPreferred();

				haiku_api.draw([] {
					midend_force_redraw(haiku_api.midEnd);
				});

				return;
			}
		default:
			message->PrintToStream();
			BWindow::MessageReceived(message);
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
	// recommendation is 20ms, but BeAPI docs suggest no less than 100,000us.
	haiku_api.delta = system_time();
    haiku_api.view->Window()->SetPulseRate(100000);
}


void deactivate_timer(frontend *fe)
{
	if (haiku_api.view == NULL || haiku_api.view->Window() == NULL)
		return;

	haiku_api.view->Window()->SetPulseRate(0);
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
