
#include <iostream>
#include "Semaphore.h"
#include "Output.h"

Output* Output::singleton_ = nullptr;

Output* Output::GetInstance()
{
    if (singleton_ == nullptr) {
        singleton_ = new Output();
    }
    return singleton_;
}

Output::Output() : sem_std_out(1)
{
}

void Output::print_error(const char* error_message)
{
    sem_std_out.wait();
    perror(error_message);
    sem_std_out.notify();
}

bool Output::confirm_exit()
{
    //sem_std_out.wait();
    std::cout << std::endl;
    std::cout << "[MAIN] ATTENTION : Si vous arretez le serveur, tous les joueurs seront deconnectes !" << std::endl;
    std::cout << "[MAIN] Etes-vous sur de vouloir continuer ? [O] Oui  [N] Non" << std::endl;
    char c;
    std::cin >> c;
    bool r = c == 'O' || c == 'o' || c == 'Y' || c == 'y';
    sem_std_out.notify();
    return r;
}

