#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ROWS 25
#define COLS 80
#define CLOCK_USLEEP 160000
#define CLOCK_CHANGE 15000
#define MIN_CLOCK 50000
#define MAX_CLOCK 230000
#define BUTTON_UP_CLOCK 'W'
#define BUTTON_UP_CLOCK_SMALL 'w'
#define BUTTON_DOWN_CLOCK 'E'
#define BUTTON_DOWN_CLOCK_SMALL 'e'
#define BUTTON_QUET_LIFE 'Q'
#define BUTTON_QUET_LIFE_SMALL 'q'

void input(int firstArray[ROWS][COLS]);
void output(int firstArray[ROWS][COLS]);
void updateField(int firstArray[ROWS][COLS], int secondArray[ROWS][COLS], int *countStatic);
int liveCell(int firstArray[ROWS][COLS], int doteX, int doteY);
int deadCell(int firstArray[ROWS][COLS], int doteX, int doteY);
void initNcurs();
void menuButton();

int main() {
    int firstArray[ROWS][COLS] = {0}, secondArray[ROWS][COLS] = {0};
    int clock = CLOCK_USLEEP;
    int countStatic = 0;
    input(firstArray);
    initNcurs();
    curs_set(0);
    raw();

    int flag = 1;

    while (flag) {
        updateField(firstArray, secondArray, &countStatic);
        if (countStatic == 15) {
            flag = 0;
        }
        move(0, 0);
        char button = getch();
        if (button == BUTTON_QUET_LIFE || button == BUTTON_QUET_LIFE_SMALL) {
            exit(1);
        } else if ((button == BUTTON_UP_CLOCK || button == BUTTON_UP_CLOCK_SMALL) && clock < MAX_CLOCK)
            clock += CLOCK_CHANGE;
        else if ((button == BUTTON_DOWN_CLOCK || button == BUTTON_DOWN_CLOCK_SMALL) && clock > MIN_CLOCK)
            clock -= CLOCK_CHANGE;
        output(firstArray);
        usleep(clock);
    }
    endwin();
    return 0;
}

void input(int firstArray[ROWS][COLS]) {
    int i, j;
    char k;
    for (i = 0; i < ROWS; i++)
        for (j = 0; j < COLS; j++) {
            k = getchar();
            if (k != '0' && k != '1')
                j--;
            else
                firstArray[i][j] = k - '0';
        }
}

void output(int firstArray[ROWS][COLS]) {
    move(0, 0);
    int i, j;
    for (i = 0; i < ROWS; i++)
        for (j = 0; j < COLS; j++) {
            if (firstArray[i][j] == 1)
                printw("*");
            else
                printw(" ");
            if (j == COLS - 1) printw("\n");
        }

    refresh();
    menuButton();
}

void updateField(int firstArray[ROWS][COLS], int secondArray[ROWS][COLS], int *countStatic) {
    int flag = 1;
    int i, j;
    for (i = 0; i < ROWS; i++)
        for (j = 0; j < COLS; j++) {
            if (firstArray[i][j] == 1)
                secondArray[i][j] = liveCell(firstArray, i, j);
            else
                secondArray[i][j] = deadCell(firstArray, i, j);
        }

    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            if (firstArray[i][j] != secondArray[i][j]) {
                firstArray[i][j] = secondArray[i][j];
                flag = 0;
            }
        }
    }
    *countStatic += flag;
}

int liveCell(int firstArray[ROWS][COLS], int doteX, int doteY) {
    int result = 1;
    int count = 0;
    if (firstArray[(doteX + ROWS) % ROWS][(doteY + 1 + COLS) % COLS] == 1) count++;
    if (firstArray[(doteX + 1 + ROWS) % ROWS][(doteY + COLS) % COLS] == 1) count++;
    if (firstArray[(doteX + 1 + ROWS) % ROWS][(doteY + 1 + COLS) % COLS] == 1) count++;
    if (firstArray[(doteX - 1 + ROWS) % ROWS][(doteY + 1 + COLS) % COLS] == 1) count++;
    if (firstArray[(doteX + 1 + ROWS) % ROWS][(doteY - 1 + COLS) % COLS] == 1) count++;
    if (firstArray[(doteX - 1 + ROWS) % ROWS][(doteY + COLS) % COLS] == 1) count++;
    if (firstArray[(doteX + ROWS) % ROWS][(doteY - 1 + COLS) % COLS] == 1) count++;
    if (firstArray[(doteX - 1 + ROWS) % ROWS][(doteY - 1 + COLS) % COLS] == 1) count++;

    if (count < 2 || count > 3) result = 0;

    return result;
}

int deadCell(int firstArray[ROWS][COLS], int doteX, int doteY) {
    int result = 0;
    int count = 0;
    if (firstArray[(doteX + ROWS) % ROWS][(doteY + 1 + COLS) % COLS] == 1) count++;
    if (firstArray[(doteX + 1 + ROWS) % ROWS][(doteY + COLS) % COLS] == 1) count++;
    if (firstArray[(doteX + 1 + ROWS) % ROWS][(doteY + 1 + COLS) % COLS] == 1) count++;
    if (firstArray[(doteX - 1 + ROWS) % ROWS][(doteY + 1 + COLS) % COLS] == 1) count++;
    if (firstArray[(doteX + 1 + ROWS) % ROWS][(doteY - 1 + COLS) % COLS] == 1) count++;
    if (firstArray[(doteX - 1 + ROWS) % ROWS][(doteY + COLS) % COLS] == 1) count++;
    if (firstArray[(doteX + ROWS) % ROWS][(doteY - 1 + COLS) % COLS] == 1) count++;
    if (firstArray[(doteX - 1 + ROWS) % ROWS][(doteY - 1 + COLS) % COLS] == 1) count++;

    if (count == 3) result = 1;

    return result;
}

void initNcurs() {
    curs_set(1);
    initscr();
    if (freopen("/dev/tty", "rw", stdin)) {
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
    }
}

void menuButton() {
    printw("\n\n%14cPress the button %c or %c if you want to leave the game\n\n", ' ', BUTTON_QUET_LIFE,
           BUTTON_QUET_LIFE_SMALL);
    printw("Press button %c or %c - down the time", BUTTON_DOWN_CLOCK, BUTTON_DOWN_CLOCK_SMALL);
    printw("%8cPress button %c or %c - up the time", ' ', BUTTON_UP_CLOCK, BUTTON_UP_CLOCK_SMALL);
}
