/*
 * This file is part of the stm32 tic-tac-toe game (https://github.com/Sisoog/stm32_tictactoc_game).
 * Copyright (c) 2021 mohammad mazarei.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// ----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "Board.h"
#include <stm32f0xx.h>
#include <stm32f0xx_iwdg.h>
#include <Uart/Usart.h>
#include <xprintf/xprintf.h>
#include <stdbool.h>
// ----- main() ---------------------------------------------------------------

#define LsiFreq 	 40000
// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"
void HW_Init(void);

#define max(a, b)           \
  (                         \
      {                     \
        typeof(a) _a = (a); \
        typeof(b) _b = (b); \
        _a > _b ? _a : _b;  \
      })

#define min(a, b)           \
  (                         \
      {                     \
        typeof(a) _a = (a); \
        typeof(b) _b = (b); \
        _a < _b ? _a : _b;  \
      })

typedef enum
{
  cell_free = ' ',
  cell_player_X = 'X',
  cell_player_O = 'O',
} BoardCell_t;

typedef struct
{
  int row, col;
} move_t;

// This function print the board
void print_board(BoardCell_t board[3][3])
{
	xprintf("\r\n\tTic Tac Toe\r\n\r\n");
	xprintf("Player 1 (O)  -  Player 2 (X)\r\n\r\n\r\n");
	xprintf("     |     |     \n");
	xprintf("  %c  |  %c  |  %c \n", board[0][0], board[0][1], board[0][2]);
	xprintf("_____|_____|_____\n");
	xprintf("     |     |     \n");
	xprintf("  %c  |  %c  |  %c \n", board[1][0], board[1][1], board[1][2]);
	xprintf("_____|_____|_____\n");
	xprintf("     |     |     \n");
	xprintf("  %c  |  %c  |  %c \n", board[2][0], board[2][1], board[2][2]);
	xprintf("     |     |     \n\n");
}

// This function returns true if there are moves
// remaining on the board. It returns false if
bool IsMovesLeft(BoardCell_t board[3][3])
{
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if (board[i][j] == cell_free)
        return true;
  return false;
}

// Returns a value based on who is winning
// b[3][3] is the Tic-Tac-Toe board
int BoardEvaluate(BoardCell_t board[3][3])
{
  // Checking for Rows for X or O victory.
  for (int row = 0; row < 3; row++)
  {
    if (board[row][0] == board[row][1] && board[row][1] == board[row][2])
    {
      if (board[row][0] == cell_player_X)
        return +10;
      else if (board[row][0] == cell_player_O)
        return -10;
    }
  }

  // Checking for Columns for X or O victory.
  for (int col = 0; col < 3; col++)
  {
    if (board[0][col] == board[1][col] && board[1][col] == board[2][col])
    {
      if (board[0][col] == cell_player_X)
        return +10;
      else if (board[0][col] == cell_player_O)
        return -10;
    }
  }

  // Checking for Diagonals for X or O victory.
  if (board[0][0] == board[1][1] && board[1][1] == board[2][2])
  {
    if (board[0][0] == cell_player_X)
      return +10;
    else if (board[0][0] == cell_player_O)
      return -10;
  }
  if (board[0][2] == board[1][1] && board[1][1] == board[2][0])
  {
    if (board[0][2] == cell_player_X)
      return +10;
    else if (board[0][2] == cell_player_O)
      return -10;
  }

  // Else if none of them have won then return 0
  return 0;
}

// This is the minimax function. It considers all
// the possible ways the game can go and returns
// the value of the board
int minimax(BoardCell_t board[3][3], int depth, bool isMax)
{
  int score = BoardEvaluate(board);

  // If Maximizer has won the game return his/her
  // evaluated score
  if (score == 10)
    return score - depth;

  // If Minimizer has won the game return his/her
  // evaluated score
  if (score == -10)
    return score + depth;

  // If there are no more moves and no winner then
  // it is a tie
  if (IsMovesLeft(board) == false)
    return 0;

  // If this maximizer's move
  if (isMax)
  {
    int best = -1000;

    // Traverse all cells
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        // Check if cell is empty
        if (board[i][j] == cell_free)
        {
          // Make the move
          board[i][j] = cell_player_X;

          // Call minimax recursively and choose
          // the maximum value
          best = max(best, minimax(board, depth + 1, !isMax));

          // Undo the move
          board[i][j] = cell_free;
        }
      }
    }
    return best;
  }

  // If this minimizer's move
  else
  {
    int best = 1000;

    // Traverse all cells
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        // Check if cell is empty
        if (board[i][j] == cell_free)
        {
          // Make the move
          board[i][j] = cell_player_O;

          // Call minimax recursively and choose
          // the minimum value
          best = min(best, minimax(board, depth + 1, !isMax));

          // Undo the move
          board[i][j] = cell_free;
        }
      }
    }
    return best;
  }
}

// This will return the best possible move for the player
move_t findBestMove(BoardCell_t board[3][3])
{
  int bestVal = -1000;
  move_t bestMove;
  bestMove.row = -1;
  bestMove.col = -1;

  // Traverse all cells, evaluate minimax function for
  // all empty cells. And return the cell with optimal
  // value.
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      // Check if cell is empty
      if (board[i][j] == cell_free)
      {
        // Make the move
        board[i][j] = cell_player_X;

        // compute evaluation function for this
        // move.
        int moveVal = minimax(board, 0, false);

        // Undo the move
        board[i][j] = cell_free;

        // If the value of the current move is
        // more than the best value, then update
        // best/
        if (moveVal > bestVal)
        {
          bestMove.row = i;
          bestMove.col = j;
          bestVal = moveVal;
        }
      }
    }
  }
  xprintf("my move is Col:%d - Row %d\r\n", bestMove.col, bestMove.row);
  return bestMove;
}

move_t GetUserMove()
{
  int Ucode = 0;
  char stmp[10];
  do
  {
    xprintf("Enter your move (1~9):");
    xgets (stmp, sizeof(stmp));
    //sscanf(stmp,"%d", &Ucode);
    Ucode = atoi(stmp);
    if (!(Ucode > 0 && Ucode < 10))
      xprintf("you must be entered value between 1~9\r\n");
  } while (!(Ucode > 0 && Ucode < 10));

  Ucode--;
  move_t usermove;
  usermove.row = 2 - (Ucode / 3);
  usermove.col = (Ucode % 3);
  return usermove;
}

bool MoveAction(BoardCell_t board[3][3], move_t move, BoardCell_t player)
{
  /*check this position for free*/
  if (board[move.row][move.col] != cell_free)
    return false;

  board[move.row][move.col] = player;
  return true;
}

int main(int argc, char* argv[])
{
	SystemInit();
	SystemCoreClockUpdate();
	HW_Init();

	while(1)
	{
		BoardCell_t board[3][3] = {
			  {cell_free, cell_free, cell_free},
			  {cell_free, cell_free, cell_free},
			  {cell_free, cell_free, cell_free}};

		int Evaluate = 0;

		print_board(board);
		while (1)
		{
			move_t user = GetUserMove();

			if (!MoveAction(board, user, cell_player_O))
			{
			  /*check for end game*/
			  if (!IsMovesLeft(board))
				break;

			  xprintf("You can't use this location\r\nThis location has already been used!\r\n");
			  continue;
			}

			LED_GPIO->ODR &= ~(1<<LED_Pin);
			move_t ai = findBestMove(board);
			if (ai.col != -1)
			  MoveAction(board, ai, cell_player_X);
			else
			  break;

			print_board(board);
			LED_GPIO->ODR |= (1<<LED_Pin);

			/*check for winner*/
			Evaluate = BoardEvaluate(board);
			if (Evaluate != 0)
			  break;
		}

		if (Evaluate == 0)
		  xprintf("\r\n\tThe game equalised\r\n");
		else if (Evaluate > 0)
		  xprintf("\r\n\tMe Won :)\r\n");
		else
		  xprintf("\r\n\tyou Won! :(\r\n");

		xprintf("\r\nPress any key for new game ....");
		Uart1_GetBlock();
	}
}


void HW_Init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB , ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF , ENABLE);
	Usart_init(115200);

	/*init LED*/
	LED_GPIO->OSPEEDR |= GPIO_Speed_50MHz << (LED_Pin<<1);
	LED_GPIO->OTYPER |= (uint16_t)(((uint16_t)GPIO_OType_PP) << ((uint16_t)LED_Pin));
	LED_GPIO->MODER |= (((uint32_t)GPIO_Mode_OUT) << (LED_Pin <<1));

	LED_GPIO->ODR |= (1<<LED_Pin);

}





#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
