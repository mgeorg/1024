#include <memory>
#include <string>
#include <vector>

#include <stdlib.h>
#include <stdio.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#define CHECK {int* ptr = NULL; \
               printf("CHECK failed.\n"); \
               fflush(stdout); \
	       *ptr = 0;}

Display                 *dpy;
Window                  root;
GLint                   att[] =
    { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo             *vi;
Colormap                cmap;
XSetWindowAttributes    swa;
Window                  win;
GLXContext              glc;
XWindowAttributes       gwa;
XEvent                  xev;

class Board {
 public:

  Board() {
    for (int row = 0; row < 4; ++row) {
      for (int col = 0; col < 4; ++col) {
        board_[row][col] = 0;
      }
    }
    empty_ = 16;
  }

  Board(const Board& other) {
    *this = other;
  }

  void operator=(const Board& other) {
    for (int row = 0; row < 4; ++row) {
      for (int col = 0; col < 4; ++col) {
        board_[row][col] = other.board_[row][col];
      }
    }
    empty_ = other.empty_;
  }

  bool operator==(const Board& other) {
    if (empty_ != other.empty_) {
      return false;
    }
    for (int row = 0; row < 4; ++row) {
      for (int col = 0; col < 4; ++col) {
        if (board_[row][col] != other.board_[row][col]) {
	  return false;
	}
      }
    }
    return true;
  }

  int Empty() const {
    return empty_;
  }

  void AddItem() {
    int r = rand() % empty_;
    printf("%d of %d\n", r, empty_);
    for (int row = 0; row < 4; ++row) {
      for (int col = 0; col < 4; ++col) {
        if (board_[row][col] == 0) {
	  if (r == 0) {
	    board_[row][col] = 1;
            --empty_;
	    return;
	  }
	  --r;
	}
      }
    }
    CHECK;
  }
 
  void AllItems(std::vector<Board>* all_boards) {
    all_boards->reserve(empty_);
    for (int row = 0; row < 4; ++row) {
      for (int col = 0; col < 4; ++col) {
        if (board_[row][col] == 0) {
	  all_boards->emplace_back(*this);
	  all_boards->back().board_[row][col] = 1;
	  all_boards->back().empty_ -= 1;
	}
      }
    }
  }
 
  void SetEmpty() {
    empty_ = 0;
    for (int row = 0; row < kRowMax; ++row) {
      for (int col = 0; col < kColMax; ++col) {
        if (board_[row][col] == 0) {
	  ++empty_;
	}
      }
    }
  }

  bool Left(Board* new_position) {
    bool no_change = true;
    for (int row = 0; row < kRowMax; ++row) {
      // Squeeze everything over to one side.
      int i = 0;  // The current position in the new board.
      for (int col = 0; col < kColMax; ++col) {
        if (board_[row][col] != 0) {
	  new_position->board_[row][i] = board_[row][col];
	  if (i != col) {
	    no_change = false;
          }
          ++i;
        }
      }
      // Fill in the remaining zeros.
      for (; i < kColMax; ++i) {
	new_position->board_[row][i] = 0;
      }
      // Combine pieces.
      for (int col = 0; col < kColMax-1; ++col) {
        if (new_position->board_[row][col] > 0 &&
	    new_position->board_[row][col] ==
	    new_position->board_[row][col+1]) {
	  no_change = false;
	  new_position->board_[row][col] *= 2;
	  for (int i = col+1; i < kColMax-1; ++i) {
	    new_position->board_[row][i] = new_position->board_[row][i+1];
	  }
	  new_position->board_[row][kColMax-1] = 0;
        }
      }
    }
    new_position->SetEmpty();
    //return no_change;
    return !(*this == *new_position);
  }
 
  bool Right(Board* new_position) {
    bool no_change = true;
    for (int row = 0; row < kRowMax; ++row) {
      // Squeeze everything over to one side.
      int i = kColMax-1;  // The current position in the new board.
      for (int col = kColMax-1; col >= 0; --col) {
        if (board_[row][col] != 0) {
	  new_position->board_[row][i] = board_[row][col];
	  if (i != col) {
	    no_change = false;
          }
          --i;
        }
      }
      // Fill in the remaining zeros.
      for (; i >= 0; --i) {
	new_position->board_[row][i] = 0;
      }
      // Combine pieces.
      for (int col = kColMax-1; col >= 0; --col) {
        if (new_position->board_[row][col] > 0 &&
            new_position->board_[row][col] ==
	    new_position->board_[row][col-1]) {
	  no_change = false;
	  new_position->board_[row][col] *= 2;
	  for (int i = col-1; i >= 1; --i) {
	    new_position->board_[row][i] = new_position->board_[row][i-1];
	  }
	  new_position->board_[row][0] = 0;
        }
      }
    }
    new_position->SetEmpty();
    return !(*this == *new_position);
  }
 
  bool Up(Board* new_position) {
    bool no_change = true;
    for (int col = 0; col < kColMax; ++col) {
      // Squeeze everything over to one side.
      int i = 0;  // The current position in the new board.
      for (int row = 0; row < kRowMax; ++row) {
        if (board_[row][col] != 0) {
	  new_position->board_[i][col] = board_[row][col];
	  if (i != col) {
	    no_change = false;
          }
          ++i;
        }
      }
      // Fill in the remaining zeros.
      for (; i < kRowMax; ++i) {
	new_position->board_[i][col] = 0;
      }
      // Combine pieces.
      for (int row = 0; row < kRowMax-1; ++row) {
        if (new_position->board_[row][col] > 0 &&
            new_position->board_[row][col] ==
	    new_position->board_[row+1][col]) {
	  no_change = false;
	  new_position->board_[row][col] *= 2;
	  for (int i = row+1; i < kRowMax-1; ++i) {
	    new_position->board_[i][col] = new_position->board_[i+1][col];
	  }
	  new_position->board_[kRowMax-1][col] = 0;
        }
      }
    }
    new_position->SetEmpty();
    return !(*this == *new_position);
  }

  bool Down(Board* new_position) {
    bool no_change = true;
    for (int col = 0; col < kColMax; ++col) {
      // Squeeze everything over to one side.
      int i = kRowMax-1;  // The current position in the new board.
      for (int row = kRowMax-1; row >= 0; --row) {
        if (board_[row][col] != 0) {
	  new_position->board_[i][col] = board_[row][col];
	  if (i != col) {
	    no_change = false;
          }
          --i;
        }
      }
      // Fill in the remaining zeros.
      for (; i >= 0; --i) {
	new_position->board_[i][col] = 0;
      }
      // Combine pieces.
      for (int row = kRowMax-1; row >= 0; --row) {
        if (new_position->board_[row][col] > 0 &&
            new_position->board_[row][col] ==
	    new_position->board_[row-1][col]) {
	  no_change = false;
	  new_position->board_[row][col] *= 2;
	  for (int i = row-1; i >= 1; --i) {
	    new_position->board_[i][col] = new_position->board_[i-1][col];
	  }
	  new_position->board_[0][col] = 0;
        }
      }
    }
    new_position->SetEmpty();
    return !(*this == *new_position);
  }

  void Print() {
    for (int row = 0; row < 4; ++row) {
      printf("%4d %4d %4d %4d\n", board_[row][0], board_[row][1],
             board_[row][2], board_[row][3]);
    }
    printf("\n");
  }

 private:
  static const int kRowMax;
  static const int kColMax;

  int board_[4][4];
  int empty_;
};

const int Board::kRowMax = 4;
const int Board::kColMax = 4;

void DrawBoard(const Board& board) {
  printf("Drawing board.\n");
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 glOrtho(-1., 1., -1., 1., 1., 20.);

 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

 glBegin(GL_QUADS);
  glColor3f(1., 0., 0.); glVertex3f(-.75, -.75, 0.);
  glColor3f(0., 1., 0.); glVertex3f( .75, -.75, 0.);
  glColor3f(0., 0., 1.); glVertex3f( .75,  .75, 0.);
  glColor3f(1., 1., 0.); glVertex3f(-.75,  .75, 0.);
 glEnd();
}

int main(int argc, char *argv[]) {
  dpy = XOpenDisplay(NULL);
 
  if(dpy == NULL) {
    printf("\n\tcannot connect to X server\n\n");
    exit(0);
  }

  root = DefaultRootWindow(dpy);
  
  vi = glXChooseVisual(dpy, 0, att);
  
  if(vi == NULL) {
    printf("\n\tno appropriate visual found\n\n");
    exit(0);
  } 
  else {
    /* %p creates hexadecimal output like in glxinfo */
    printf("\n\tvisual %p selected\n", (void *)vi->visualid);
  }
  
  cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
  
  swa.colormap = cmap;
  swa.event_mask = ExposureMask | KeyPressMask;
  
  win = XCreateWindow(dpy, root, 0, 0, 600, 600, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
  
  XMapWindow(dpy, win);
  XStoreName(dpy, win, "1024 Game");
  
  glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
  glXMakeCurrent(dpy, win, glc);
  
  glEnable(GL_DEPTH_TEST); 

  Board board;
  board.AddItem();
  board.AddItem();
  board.Print();
  while(1) {
    bool no_update = true;

    XNextEvent(dpy, &xev);
    
    if (xev.type == Expose) {
      XGetWindowAttributes(dpy, win, &gwa);
      glViewport(0, 0, gwa.width, gwa.height);
      DrawBoard(board); 
      glXSwapBuffers(dpy, win);
    } else if (xev.type == KeyPress) {
      if (xev.xkey.keycode == 114) {
        Board new_board;
        if (!board.Right(&new_board)) {
	  printf("The board didn't change!!!\n");
	} else {
	  no_update = false;
          board = new_board;
        }
      } else if (xev.xkey.keycode == 113) {
        Board new_board;
	board.Left(&new_board);
        if (board == new_board) {
	  printf("The board didn't change!!!\n");
	} else {
	  no_update = false;
          board = new_board;
        }
      } else if (xev.xkey.keycode == 111) {
        Board new_board;
        if (!board.Up(&new_board)) {
	  printf("The board didn't change!!!\n");
	} else {
	  no_update = false;
          board = new_board;
	}
        board = new_board;
      } else if (xev.xkey.keycode == 116) {
        Board new_board;
        if (!board.Down(&new_board)) {
	  printf("The board didn't change!!!\n");
	} else {
	  no_update = false;
          board = new_board;
	}
        board = new_board;
      } else if (xev.xkey.keycode == 53 /*q*/) {
        glXMakeCurrent(dpy, None, NULL);
        glXDestroyContext(dpy, glc);
        XDestroyWindow(dpy, win);
        XCloseDisplay(dpy);
        exit(0);
      } else {
        printf("KeyPress: keycode %u ('%c') state %u\n",
               xev.xkey.keycode, static_cast<char>(xev.xkey.keycode),
               xev.xkey.state);
      }
    }
    
    if (!no_update) {
      board.AddItem();
    }
    board.Print();
  }
  return 0;
}


