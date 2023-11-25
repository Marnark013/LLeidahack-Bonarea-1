    /*
    PRINT PASSABLE BOARD
    
    for(int i = 0; i < passableBoard.size(); ++i) {
        for(int j = 0;  j < passableBoard[0].size(); ++j) {
            if(passableBoard[i][j]) std::cout << "  ";
            else std::cout << "X ";
        }
        std::cout << std::endl;
    }

    PRINT BOARD INFO

    for(int i = 0; i < boardInfo.size(); ++i) {
        for(int j = 0;  j < boardInfo[0].size(); ++j) {
            if(boardInfo[i][j] == "") std::cout << "  ";
            else std::cout << "A ";
        }
        std::cout << std::endl;
    }

    PRINT PASSABLE BOARD AND BOARD INFO

    for(int i = 0; i < boardInfo.size(); ++i) {
        for(int j = 0;  j < boardInfo[0].size(); ++j) {
            if(boardInfo[i][j] == "" and passableBoard[i][j]) std::cout << "  ";
            else if(not passableBoard[i][j]) std::cout << "X ";
            else if(boardInfo[i][j] != "") std::cout << "- ";
        }
        std::cout << std::endl;
    } 

    PRINT ITEMINFOMAP

    void printItemMap(const std::map<std::string, itemInfo>& myMap) {
        for (const auto& pair : myMap) {
            std::cout << "Key: " << pair.first << std::endl;
            std::cout << "Item Name: " << pair.second.itemName << std::endl;
            std::cout << "Values: ";
            for (int value : pair.second.pickingTimes) {
                std::cout << value << " ";
            }
            std::cout << "\n\n"; // Separate entries with empty lines
        }
    }
    
    PRINT CLIENTIDTOTICKETINFO

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
    
    PRINT TIMEENTERANDCLIENTID

    ReadTickets();
    MTS::iterator ite = ordClients.begin();
    MTS::const_iterator itefinal = ordClients.end();

    while(ite != itefinal)
    {
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&ite->first));
        std::cout << "Id costumer: " << ite->second << " " << buffer << std::endl;
        ++ite;
    }*/
