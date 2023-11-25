#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <ctime>
#include <map>

using VSI = std::vector <std::pair<std::string, int>>;

struct TicketInfo
{
    std::time_t enter_time;
    std::string client_id;
    VSI products_list;
    std::string ticked_id;
};

using MSTI = std::map <std::string, TicketInfo>;
using VS = std::vector <std::string>;

MSTI clientsInfo;

void ReadTickets()
{
    std::fstream file;                                              //pointer to the file
    file.open("./data/hackathon_tickets.csv", std::ios::in);        //open file and selecting read

    std:: string row, temp, word, auxNumeroElement; 
    std::tm tm;
    int numeroElements;
    getline(file, row);                                             //ens petem la primera

    while (getline(file, row))
    {
        struct TicketInfo aux;                                      //chequear esta declaracion dentro del while
        std::stringstream s(row);
        
        getline(s, word, ';');
        std::istringstream timeStream(word);
        timeStream >> std::get_time(&tm, "%H:%M:%S");               //se necesitaba el iomanip
        aux.enter_time = std::mktime(&tm);

        getline(s, word, ';');
        aux.client_id = word;

        getline(s, word, ';');
        getline(s, auxNumeroElement, ';');
        numeroElements = stoi(auxNumeroElement);

        MSTI::iterator ite = clientsInfo.find(aux.client_id);
        

        if (ite == clientsInfo.end())
        {
            aux.products_list.push_back(std::make_pair(word, numeroElements));

            getline(s, word, ';');
            aux.ticked_id = word;
            clientsInfo.insert(make_pair(aux.client_id, aux));
        }
        else
        {
            ite->second.products_list.push_back(std::make_pair(word, numeroElements));
            getline(s, word, ';');
        }
    }
}

int main() {
    ReadTickets();
    MSTI::const_iterator ite = clientsInfo.begin();
    MSTI::const_iterator itefinal = clientsInfo.end();

    while(ite != itefinal)
    {
        std::cout << "Id del cliente: " << ite->first << "Lista de elementos: " << std::endl;
        for (int i = 0; i < ite->second.products_list.size(); ++i)
        {
            std::cout << ite->second.products_list[i].first << " cantidad " << ite->second.products_list[i].second << " " << std::endl;
        }
        ++ite;
    }
}