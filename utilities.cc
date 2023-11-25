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
    }*/
