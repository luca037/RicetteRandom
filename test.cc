#include <curses.h>
#include <signal.h>

bool shouldExit = false;
WINDOW* win;

void handle_resize(int sig) {
    endwin(); // Chiude la modalità ncurses
    //refresh(); // Aggiorna lo schermo del terminale

    // Reinizializza ncurses dopo il ridimensionamento
    initscr();
    wclear(win); // Cancella lo schermo

    wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
    mvwprintw(win, 0, 0, "Terminal Resized!");

    wrefresh(win);
}

int main() {
    // Inizializza ncurses
    initscr();
    cbreak(); // Abilita la modalità carattere singolo
    noecho(); // Disabilita l'eco dell'input
    keypad(stdscr, TRUE); // Abilita la lettura dei tasti speciali
    signal(SIGWINCH, handle_resize);

    win = newwin(20, 40, 0, 0);
    wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
    wrefresh(win);

    mvwprintw(win, 1, 1, "Resize the terminal!");
    wrefresh(win);

    while (!shouldExit) {
        int ch = wgetch(win); // Legge l'input dell'utente

        // Aggiungi qui il codice per gestire l'input
        if (ch == 'q') {
            shouldExit = true;
        }
    }

    delwin(win);
    endwin();

    return 0;
}
