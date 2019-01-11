#pragma once

class tictac
{
  public:
    void print();
    char checkWin();
    bool checkPos(int i, int j);
    void setVal(char val, int i, int j);

    tictac()
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                board[i][j] = '-';
            }
        }
    }

  private:
    char board[3][3];
};