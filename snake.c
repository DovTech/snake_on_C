#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>

enum { delay_duration = 110 };
enum { field_my = 30, field_mx = 80, field_border = 35 };
enum { 
    head       = 48, 
    tail       = 111, 
    start_size = 3,
    start_y    = 20,
    start_x    = 50, 
    apple      = 64
};
enum { 
            escape = 27,
       down_add_up = 517,   //KEY_DOWN + KEY_UP = 517 
    left_add_right = 521    //KEY_LEFT + KEY_RIGHT = 521
};

static const char game_over[] = "GAME OVER";
static const char your_score[] = "Your score: ";

struct snake_part
{
    char part;
    int sp_y;
    int sp_x;
};

static void print_fields()
{
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    for(int i = 0; i < field_mx; i += 2) {
        move(0, i);
        addch(field_border | COLOR_PAIR(4) | A_BOLD);
        addch(' ');
        move(field_my, i);
        addch(field_border | COLOR_PAIR(4) | A_BOLD);
        addch(' ');
    }
    for(int i = 0; i <= field_my; i++) {
        move(i, 0);        
        addch(field_border | COLOR_PAIR(4) | A_BOLD);
        move(i, field_mx);
        addch(field_border | COLOR_PAIR(4) | A_BOLD);
    }
    refresh();
}

static void init_snake(struct snake_part snake_arr[], int *snake_real_size)
{
    *snake_real_size = start_size;
    int inity = start_y;
    snake_arr[0].part = head;
    snake_arr[0].sp_y = inity;
    inity--;
    snake_arr[0].sp_x = start_x;
    for(int i = 1; i < *snake_real_size; i++) {
        snake_arr[i].part = tail; 
        snake_arr[i].sp_y = inity;
        inity--;
        snake_arr[i].sp_x = start_x;
    }
    for(int i = 0; i < *snake_real_size; i++) {
        move(snake_arr[i].sp_y, snake_arr[i].sp_x);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);     //snake
        addch(snake_arr[i].part | COLOR_PAIR(2));
    }
    move(field_my + 2, field_mx + 2);
    refresh();
}

static int check_border(struct snake_part head, const int *score)
{
    if(head.sp_y <= 0 || head.sp_y >= field_my ||
       head.sp_x <= 0 || head.sp_x >= field_mx)
        return 0;
    else return 1;
}

static void move_snake(struct snake_part body[], int snake_size, int dy, int dx)
{
    init_pair(2, COLOR_GREEN, COLOR_BLACK);     //snake
    int oldy = body[snake_size-1].sp_y;
    int oldx = body[snake_size-1].sp_x;
    for(int i = snake_size-1; i > 0; i--) {
        body[i].sp_y = body[i-1].sp_y;
        body[i].sp_x = body[i-1].sp_x;
    }
    body[0].sp_y += dy;
    body[0].sp_x += dx;
    move(body[0].sp_y, body[0].sp_x);
    addch(body[0].part | A_BOLD | COLOR_PAIR(2));
    move(body[1].sp_y, body[1].sp_x);
    addch(body[1].part | COLOR_PAIR(2));
    move(oldy, oldx);
    addch(' ');
    refresh();
}

static int check_snake(struct snake_part body[], int snake_size)
{
    for(int i = 1; i < snake_size; i++)
        if(body[0].sp_y == body[i].sp_y && body[0].sp_x == body[i].sp_x)
            return 0;
    return 1;
}

static void apple_generator(struct snake_part snake[], int snake_size, int *appley, int *applex)
{
    *appley = rand() % (field_my - 2) + 1;    
    *applex = rand() % (field_my - 2) + 1;    
    for(int i = 0; i < snake_size; i++)
        if(snake[i].sp_y == *appley && snake[i].sp_x == *applex) {
            *appley = rand() % (field_my - 2) + 1;    
            *applex = rand() % (field_my - 2) + 1;    
        }
    move(*appley, *applex);
    init_pair(1, COLOR_RED, COLOR_BLACK);   //apple
    addch(apple | COLOR_PAIR(1) | A_BOLD);
    refresh();
}

static int check_apple(const struct snake_part head, const int *appley, const int *applex)
{
    if(head.sp_y == *appley && head.sp_x == *applex)
        return 1;
    else return 0;
}

static void show_characts(struct snake_part head, const int *score)
{
    mvprintw(field_my+1, 5, "(%d,%d)    ", head.sp_x, head.sp_y);
    mvprintw(field_my+2, 5, "score: %d", *score);
    refresh();
}

int main()
{
    srand(time(NULL));

    int key = KEY_DOWN;
    int oldkey = key;
    int snake_size;
    int border_fail_flag = 1;
    int snake_fail_flag = 1;
    int row = field_my;
    int col = field_mx;
    struct snake_part snake_parts[100];
    int apple_y, apple_x;
    int score = 0;
    int apple_flag = 0;

    initscr();
    int colored_term = has_colors();
    if(colored_term)
        start_color();
    cbreak();
    keypad(stdscr, 1);
    noecho();
    getmaxyx(stdscr, row, col);
    curs_set(0);

    print_fields();
    init_snake(snake_parts, &snake_size);
    apple_generator(snake_parts, snake_size, &apple_y, &apple_x);
    while(key != escape && border_fail_flag && snake_fail_flag) {
        timeout(delay_duration);
        key = getch(); 
        if(key == ERR || (key + oldkey == down_add_up) || (key + oldkey == left_add_right))
            key = oldkey;
        else
            oldkey = key;
        switch(key) {
            case KEY_UP:
                move_snake(snake_parts, snake_size, -1, 0);
                border_fail_flag = check_border(snake_parts[0], &score);
                snake_fail_flag = check_snake(snake_parts, snake_size);
                apple_flag = check_apple(snake_parts[0], &apple_y, &apple_x);
                break;
            case KEY_DOWN:
                move_snake(snake_parts, snake_size, 1, 0);
                border_fail_flag = check_border(snake_parts[0], &score);
                snake_fail_flag = check_snake(snake_parts, snake_size);
                apple_flag = check_apple(snake_parts[0], &apple_y, &apple_x);
                break;
            case KEY_LEFT:
                move_snake(snake_parts, snake_size, 0, -1);
                border_fail_flag = check_border(snake_parts[0], &score);
                snake_fail_flag = check_snake(snake_parts, snake_size);
                apple_flag = check_apple(snake_parts[0], &apple_y, &apple_x);
                break;
            case KEY_RIGHT:
                move_snake(snake_parts, snake_size, 0, 1);
                border_fail_flag = check_border(snake_parts[0], &score);
                snake_fail_flag = check_snake(snake_parts, snake_size);
                apple_flag = check_apple(snake_parts[0], &apple_y, &apple_x);
                break;
        }
        show_characts(snake_parts[0], &score);
        print_fields();     //почему-то повреждается граница на очках 1, 7, 12, 17, 21, перерисовываю её
        if(apple_flag) {
            apple_generator(snake_parts, snake_size, &apple_y, &apple_x);
            score++;
            snake_size++;
            apple_flag = 0;
        }
    }
    int gameover_y = field_my / 2;
    int gameover_x = (field_mx - sizeof(game_over) - 1) / 2;
    mvprintw(gameover_y, gameover_x, "%s", game_over);
    mvprintw(gameover_y+1, gameover_x, "%s%d", your_score, score);
    refresh();
    sleep(100);
    endwin();

    return 0;
}
