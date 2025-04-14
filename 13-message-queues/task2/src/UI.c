#include <./UI.h>
#include <./Queues.h>

#include <string.h>

int open_windows(struct Win_St *mes_hist_win_st, struct Win_St *user_list_win_st, struct Win_St *input_win_st)
{
    int term_h, term_w;
    getmaxyx(stdscr, term_h, term_w);

    printf("term_h = %d term_w = %d\n", term_h, term_w);
    if (term_h < MIN_TERM_H || term_w < MIN_TERM_W)
        return invalid_term_size;

    user_list_win_st->width = USER_NAME_SIZE + 2;
    user_list_win_st->height = term_h - 3;
    user_list_win_st->x_pos = term_w - USER_NAME_SIZE - 2;
    user_list_win_st->y_pos = 0;
    user_list_win_st->win = newwin(user_list_win_st->height, user_list_win_st->width, user_list_win_st->y_pos, user_list_win_st->x_pos);

    input_win_st->width = term_w;
    input_win_st->height = 3;
    input_win_st->x_pos = 0;
    input_win_st->y_pos = term_h - 3;
    input_win_st->win = newwin(input_win_st->height, input_win_st->width, input_win_st->y_pos, input_win_st->x_pos);

    mes_hist_win_st->width = term_w - USER_NAME_SIZE - 2;
    mes_hist_win_st->height = term_h - 3;
    mes_hist_win_st->x_pos = 0;
    mes_hist_win_st->y_pos = 0;
    mes_hist_win_st->win = newwin(mes_hist_win_st->height, mes_hist_win_st->width, mes_hist_win_st->y_pos, mes_hist_win_st->x_pos);

    return success;
}

void print_user_list_win(struct Win_St win_st, struct UsersList *user_list)
{
    wclear(win_st.win);
    box(win_st.win, 0, 0);

    struct User *tmp = user_list->head;
    int indent_from_begin = 0; // if there is so many users that they cant fit to user_list_win
    int i =0;
    
    if(win_st.height -2 < user_list->num)
    {
        indent_from_begin = user_list->num - win_st.height +3;
        wmove(win_st.win, win_st.height -2, 1);
        wprintw(win_st.win,"[%d of %d]",  user_list->num - indent_from_begin, user_list->num);
    }

    while (tmp != NULL)
    {
        if(indent_from_begin)
        {
            indent_from_begin--;
            tmp = tmp->next;
            continue;
        }    
        wmove(win_st.win, 1 + i, 1);
        wprintw(win_st.win, "%s", tmp->user_name);
        tmp = tmp->next;
        i++;
    }
    wrefresh(win_st.win);
}

void print_mes_hist_win(struct Win_St win_st, struct Msg_history* msg_hist)
{
    wclear (win_st.win);
    box(win_st.win, 0, 0);

    while(win_st.height-1 <= msg_hist->num) // delete old msgs which dont fit to window
        Msg_history_pop_back(msg_hist);

    struct UI_Msg* tmp = msg_hist->head;
    
    int i = 1;
	while(tmp != NULL)
	{
        wmove(win_st.win, win_st.height - 1 - i, 1);
        if( (int) strlen(tmp->msg) > win_st.width-2)
        {
            tmp->msg[win_st.width-3] = '>';
            tmp->msg[win_st.width-2] = 0;
        }
        wprintw(win_st.win, "%s", tmp->msg);
		tmp = tmp->next;
        i++;
	}
    wrefresh(win_st.win);
}

void print_input_win(struct Win_St win_st, char *input_str, int str_len, char* user_name)
{
    wclear(win_st.win);
    box(win_st.win, 0, 0);

    wmove(win_st.win, 0, (win_st.width - strlen(user_name))/2 - 4);
    wprintw (win_st.win, " <%s> ", user_name);

    input_str[str_len+1] = 0;
    
    wmove(win_st.win, 0, 1);
    wprintw (win_st.win, "[%d/%d]", str_len , INPUT_MSG_SIZE);
    wmove(win_st.win, 1, 1);
    if(str_len >  win_st.width - 3)
        wprintw (win_st.win, "<%s", input_str+(win_st.width - 3));
    else
        wprintw (win_st.win, "%s_", input_str);
    wrefresh(win_st.win);
}