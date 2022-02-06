#include <Application.h>
#include <Bitmap.h>
#include <LayoutBuilder.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Rect.h>
#include <StringView.h>
#include <View.h>
#include <Window.h>

#include <stdio.h>
#include <stdlib.h>

#include "puzzles.h"


const uint32 GAME_TYPE = 'gmtp';
const uint32 SOLVE_GAME = 'slve';


class PuzzleView : public BView {
public:
			PuzzleView();
	virtual	~PuzzleView() {};

	void	MessageReceived(BMessage *message);
	void	Pulse();
	void	Draw(BRect updateRect);
	void	GetPreferredSize(float *_width, float *_height);
};


// PuzzleWindow


class PuzzleWindow : public BWindow {
public:
			PuzzleWindow(BRect frame);
	virtual	~PuzzleWindow() {};

	void	MessageReceived(BMessage *message);

	BStringView	*StatusBar() const {
		return fStatusBar;
	}
private:
	void	BuildMenu(BMenu *parentMenu, preset_menu *puzzleMenu);

	PuzzleView  *fPuzzleView;
	BStringView	*fStatusBar;
};


class PuzzleApp : public BApplication {
public:
			PuzzleApp();
	virtual	~PuzzleApp() {};

	void	ReadyToRun();
	void	MessageReceived(BMessage *message);

private:
	PuzzleWindow	*fPuzzleWindow;
};


// PuzzleView


PuzzleView::PuzzleView()
	: BView("PuzzleView", B_PULSE_NEEDED | B_WILL_DRAW)
{
	SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
	SetLowUIColor(ViewUIColor());
}


// PuzzleApp


PuzzleApp::PuzzleApp()
	:
	BApplication("application/x-vnd.Jessica.L.Hamilton-PortablePuzzleCollection")
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
	BView *offscreen_view;
	BBitmap *offscreen;

	rgb_color *colours;

	int32 button_state;

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

		frontEnd->offscreen_view->SetHighColor(frontEnd->get_colour(colour));

		BFont font(be_plain_font);
		font.SetSize(floorf((72./96.) * fontSize));

		int textWidth = font.StringWidth(text);

		font_height fontHeight;
		font.GetHeight(&fontHeight);

		BPoint startPoint(x, y);

		if (align & ALIGN_VCENTRE)
		{
			startPoint.y += (fontHeight.ascent + fontHeight.descent) / 2 - fontHeight.descent;
		}

		if (align & ALIGN_HRIGHT)
		{
			startPoint.x -= textWidth;
		}
		else if (align & ALIGN_HCENTRE)
		{
			startPoint.x -= textWidth / 2;
		}

		frontEnd->offscreen_view->SetFont(&font);
		frontEnd->offscreen_view->DrawString(text, startPoint);
	},

	// draw_rect
	[](void *self, int x, int y, int w, int h, int colour)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		frontEnd->offscreen_view->SetHighColor(frontEnd->get_colour(colour));
		frontEnd->offscreen_view->FillRect(BRect(x, y, x+w-1, y+h-1));
	},

	// draw_line
	[](void *self, int x1, int y1, int x2, int y2, int colour)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		frontEnd->offscreen_view->SetHighColor(frontEnd->get_colour(colour));
		frontEnd->offscreen_view->StrokeLine(BPoint(x1, y1), BPoint(x2, y2));
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
			frontEnd->offscreen_view->SetHighColor(frontEnd->get_colour(fillColour));
			frontEnd->offscreen_view->FillPolygon(points, numPoints);
		}

		frontEnd->offscreen_view->SetHighColor(frontEnd->get_colour(strokeColour));
		frontEnd->offscreen_view->StrokePolygon(points, numPoints);
	},

	// draw_circle
	[](void *self, int cx, int cy, int radius,
			int fillColour, int strokeColour)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		if (fillColour >= 0) {
			frontEnd->offscreen_view->SetHighColor(frontEnd->get_colour(fillColour));
			frontEnd->offscreen_view->FillEllipse(BPoint(cx, cy), radius, radius);
		}

		frontEnd->offscreen_view->SetHighColor(frontEnd->get_colour(strokeColour));
		frontEnd->offscreen_view->StrokeEllipse(BPoint(cx, cy), radius, radius);
	},

	// draw_update
	[](void *self, int x, int y, int w, int h)
	{

	},

	// clip
	[](void *self, int x, int y, int w, int h)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		frontEnd->offscreen_view->ClipToRect(BRect(x, y, w+x, h+y));
	},

	// unclip
	[](void *self)
	{
		frontend *frontEnd = static_cast<frontend*>(self);

		frontEnd->offscreen_view->ConstrainClippingRegion(NULL);
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
		frontend *frontEnd = static_cast<frontend*>(self);

		PuzzleView *view = static_cast<PuzzleView*>(frontEnd->view);
		PuzzleWindow *window = static_cast<PuzzleWindow*>(view->Window());

		window->StatusBar()->SetText(text);
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



struct LockBitmap
{
	LockBitmap() {
		haiku_api.offscreen->Lock();
	}
	~LockBitmap() {
		haiku_api.offscreen->Unlock();
	}
};


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

	{
		LockBitmap _;

		midend_timer(haiku_api.midEnd, ((float)delta) / 1000000.);
	}

	Invalidate();
}


void
PuzzleView::Draw(BRect updateRect)
{
	haiku_api.offscreen->Lock();

	midend_redraw(haiku_api.midEnd);

	haiku_api.offscreen_view->Sync();
	haiku_api.view->DrawBitmap(haiku_api.offscreen, BPoint(0, 0));
	haiku_api.offscreen->Unlock();
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

				{
					LockBitmap _;

					midend_process_key(haiku_api.midEnd, where.x, where.y, translatedButtons);
				}

				Invalidate();

				return;
			}
		case B_MOUSE_MOVED:
			{
				uint32 buttons = 0;

				GetMouse(&where, &buttons);

				if (buttons == 0)
					return;

				if (buttons & B_PRIMARY_MOUSE_BUTTON)
					translatedButtons |= LEFT_DRAG;
				if (buttons & B_SECONDARY_MOUSE_BUTTON)
					translatedButtons |= RIGHT_DRAG;

				{
					LockBitmap _;

					midend_process_key(haiku_api.midEnd, where.x, where.y, translatedButtons);
				}

				Invalidate();

				return;
			}
		case B_KEY_UP:
		case B_KEY_DOWN:
			{
				int32 key;
				message->FindInt32("raw_char", &key);
				haiku_api.view->GetMouse(&where, NULL);

				{
					LockBitmap _;

					midend_process_key(haiku_api.midEnd, where.x, where.y, key);
				}

				Invalidate();

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
	fPuzzleView = view;

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

	bool addStatusBar = midend_wants_statusbar(haiku_api.midEnd);

	if (addStatusBar)
	{
		printf("a status bar is wanted\n");

		fStatusBar = new BStringView("status bar", "");

		BFont font(be_plain_font);
		font.SetSize(font.Size() * 0.8);
		fStatusBar->SetFont(&font, B_FONT_SIZE);
	}

	int menu_limit = 0;
	preset_menu *menu = midend_get_presets(haiku_api.midEnd, &menu_limit);

	BMenuBar *mainMenu = new BMenuBar("main menu");

	BMenu *gameMenu = new BMenu("Game");
	gameMenu->AddItem(new BMenuItem("Solve", new BMessage(SOLVE_GAME)));

	BMenu *typeMenu = new BMenu("Type");
	BuildMenu(typeMenu, menu);

	mainMenu->AddItem(gameMenu);
	mainMenu->AddItem(typeMenu);

	auto layoutBuilder = BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(mainMenu)
		.Add(view);

	if (addStatusBar) {
		layoutBuilder
			.Add(fStatusBar);
	}

	CenterOnScreen();

	view->MakeFocus();

	ResizeToPreferred();

	haiku_api.offscreen = new BBitmap(view->Bounds(), B_RGB_32_BIT, true);
	haiku_api.offscreen_view = new BView(view->Bounds(), "offscreen view", B_FOLLOW_ALL, B_WILL_DRAW);
	haiku_api.offscreen->AddChild(haiku_api.offscreen_view);
}


void
PuzzleWindow::BuildMenu(BMenu *parentMenu, preset_menu *puzzleMenu)
{
	preset_menu_entry *menuEntry = NULL;

	for (int i = 0; i < puzzleMenu->n_entries; ++i) {
		menuEntry = &puzzleMenu->entries[i];

		printf("%s\n", menuEntry->title);

		if (menuEntry->params == NULL) {
			BMenu *subMenu = new BMenu(menuEntry->title);
			BuildMenu(subMenu, menuEntry->submenu);
			parentMenu->AddItem(subMenu);

			continue;
		}

		BMessage *message = new BMessage(GAME_TYPE);
		message->AddInt32("index", menuEntry->id);
		message->AddString("name", menuEntry->title);
		message->AddPointer("params", menuEntry->params);

		BMenuItem *menuItem = new BMenuItem(menuEntry->title, message);

		menuItem->SetTarget(fPuzzleView);
		parentMenu->AddItem(menuItem);
	}
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

				Lock();
				if (haiku_api.offscreen->Bounds() != haiku_api.view->Bounds()) {
					BBitmap *offscreen = haiku_api.offscreen;
					BView *offscreen_view = haiku_api.offscreen_view;

					offscreen->Lock();
					offscreen->RemoveChild(offscreen_view);
					offscreen->Unlock();

					haiku_api.offscreen = new BBitmap(haiku_api.view->Bounds(), B_RGB_32_BIT, true);
					haiku_api.offscreen_view = new BView(haiku_api.view->Bounds(), "offscreen view", B_FOLLOW_ALL, B_WILL_DRAW);
					haiku_api.offscreen->AddChild(haiku_api.offscreen_view);

					delete offscreen;
					delete offscreen_view;
				}
				Unlock();

				haiku_api.view->Invalidate();

				return;
			}
		case SOLVE_GAME:
			{
				LockBitmap _;

				midend_solve(haiku_api.midEnd);

				return;
			}
		default:
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
