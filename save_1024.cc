#include <string>
#include <vector>

#include <stdio>

class Board {
 public:

  Board() {
    for (int row = 0; row < 4; ++row) {
      for (int col = 0; col < 4; ++col) {
        board_[row][col] = 0;
      }
    }
    empty_ = 64;
  }

  Board(const Board& other) {
    for (int row = 0; row < 4; ++row) {
      for (int col = 0; col < 4; ++col) {
        board_[row][col] = other.board_[row][col];
      }
    }
    empty_ = other.empty_;
  }

  void AddItem() {
    int r = rand() % empty_;
    for (int row = 0; row < 4; ++row) {
      for (int col = 0; col < 4; ++col) {
        if (board_[row][col] == 0) {
	  if (r == 0) {
	    board_[row][col] = 1;
	    return;
	  }
	  --r;
	}
      }
    }
  }
 
  void Left() {
    for (int row = 0; row < 4; ++row) {
      for (int col = 1; col < 4; ++col) {
	bool move_others = false;
        if (board_[row][col] != 0) {
	  if (board_[row][col-1] == board_[row][col]) {
	    board_[row][col-1] *= 2;
	    board_[row][col] = 0;
	    move_others = true;
	  }
        } else {
	  move_others = true;
	}
	if (move_others) {
	  for (int k = col+1; k < 4; ++k) {
	    board_[row][col-1] = board_[row][col];
	  }
	}
      }
    }
  }
 
  void Right() {
    for (int row = 0; row < 4; ++row) {
      for (int col = 2; col >= 0; --col) {
	bool move_others = false;
        if (board_[row][col] != 0) {
	  if (board_[row][col+1] == board_[row][col]) {
	    board_[row][col+1] *= 2;
	    board_[row][col] = 0;
	    move_others = true;
	  }
        } else {
	  move_others = true;
	}
	if (move_others) {
	  for (int k = col-1; k >= 0; ++k) {
	    board_[row][col+1] = board_[row][col];
	  }
	}
      }
    }
  }
 
  void Up() {
    for (int row = 1; row < 4; ++row) {
      for (int col = 0; col < 4; ++col) {
	bool move_others = false;
        if (board_[row][col] != 0) {
	  if (board_[row-1][col] == board_[row][col]) {
	    board_[row-1][col] *= 2;
	    board_[row][col] = 0;
	    move_others = true;
	  }
        } else {
	  move_others = true;
	}
	if (move_others) {
	  for (int k = row+1; k < 4; ++k) {
	    board_[row-1][col] = board_[row][col];
	  }
	}
      }
    }
  }

  void Down() {
    for (int row = 2; row >= 0; --row) {
      for (int col = 0; col < 4; ++col) {
	bool move_others = false;
        if (board_[row][col] != 0) {
	  if (board_[row+1][col] == board_[row][col]) {
	    board_[row+1][col] *= 2;
	    board_[row][col] = 0;
	    move_others = true;
	  }
        } else {
	  move_others = true;
	}
	if (move_others) {
	  for (int k = row-1; k >= 0; ++k) {
	    board_[row+1][col] = board_[row][col];
	  }
	}
      }
    }
  }

  void Print() {
    for (int row = 1; row < 4; ++row) {
      printf("%4d %4d %4d %4d\n", board[row][0], board[row][1],
             board[row][2], board[row][3], );
    }
  }

 private:
  int board_[4][4];
  int empty_;
};


int main() {
  Board board;
  board.AddItem();
  board.AddItem();
  board.Print();
  return 0;
}


