#include <iostream>
#include <openssl/md5.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../AirinSE/AirinStore.hpp"
#include "../AirinSE/AirinStoreScanner.hpp"
#include "../AirinSE/AirinMem.hpp"

#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)

using namespace std;

class IO_TEST {
public:

    int len;
    char *str;

    IO_TEST() {;
        createDB((char *) DB_DATA_PATH);
        FILE* fp = fopen("test/example.txt", "r");
        fseek(fp, 0, SEEK_END);
        size_t fsize = ftell(fp);
        len = fsize;
        str = new char[fsize + 1];
        fread(str, fsize, 1, fp);
    }

    ~IO_TEST() {
        delete str;
    }
    
    void createDB(char *dir) {
        DIR *db = NULL;
        char varDir[strlen(dir)];
        memcpy(varDir, dir, sizeof varDir);
        if ((db = opendir(dir)) == NULL) {
            for (int i = 1; varDir[i] != '\0'; i++) {
                if (varDir[i] == '/') varDir[i] = '\0';
                if (mkdir(varDir, MODE) == 1) {
                    cout << "Unable to mkdir: " << varDir << endl;
                    throw exception();
                }
                if (varDir[i] == '\0') varDir[i] = '/';
            }
        }
    }
    
    static void sprintMd5(unsigned char *md, char *ret) {
        for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
            sprintf(ret + (i << 1), "%02x", md[i]);
        }
    }

    IO_TEST* randStr(int p, int num) {
        for (int i = p; i < p + num; i++) {
            str[i] = 'A' + rand() % 64;
        }
        return this;
    }

    IO_TEST* fillMemStore(AirinMem *mem, long long num) {
        unsigned char md[MD5_DIGEST_LENGTH];
        char strMd5[(MD5_DIGEST_LENGTH << 1) + 1];
        for (int i = 0; i < num; i++) {
            // prepare key
            ULL k = Helper::getRandom(0, (1LL << 60));
            MD5((const unsigned char *) &k, sizeof k, md);
            sprintMd5(md, strMd5);

            // random value
            int vlen = Helper::getRandom((int)(len * 0.75), len);
            randStr(0, Helper::getRandom((int)(vlen * 0.25), vlen));

            AirinCell *cell = new AirinCell(strMd5, str,  MD5_DIGEST_LENGTH << 1, vlen);
            mem->add(cell);

            if (i % 1000000 == 0) {
                cout << "Generating " << i << " records, current cell: (" << endl;
                cout << "  ";
                cell->debug();
                cout << ")" << endl;
            }
        }
        return this;
    }

    IO_TEST* scan(AirinStore *store, char *startRow, char *endRow, int fetchRow = -1, int round = -1) {
        AirinStoreScanner *scanner = new AirinStoreScanner(store);
    
        AirinCell *searchStartRow = new AirinCell(startRow, NULL, strlen(startRow), 0);
        AirinCell *searchEndRow = new AirinCell(endRow, NULL, strlen(endRow), 0);
        
        cout << "Scaning from [" << startRow << "] to [" << endRow << "], limit = " << fetchRow << endl;
        
        scanner->init(searchStartRow, searchEndRow);
        
        int num = 0;
        while (true) {
            AirinCell *cell = scanner->next();
            if (cell == NULL)
                break;
            if (++num == fetchRow)
                break;
            if (round != -1)
                cout << round << "|";
            cell->debug();
        }
        cout << "Scaned " << num << " rows" << endl;
        scanner->close();
        return this;
    }

    IO_TEST* get(AirinStore *store, char *row) {
        AirinStoreScanner *scanner = new AirinStoreScanner(store);
    
        AirinCell *searchStartRow = new AirinCell((unsigned char*) row, NULL, (time_t) 0, strlen(row), 0);
        AirinCell *searchEndRow = new AirinCell((unsigned char*) row, NULL, (time_t) (1LL << 61), strlen(row), 0);
        
        cout << "Scaning [" << row << "]" << endl;
        
        scanner->init(searchStartRow, searchEndRow, true);
        
        int num = 0;
        while (true) {
            AirinCell *cell = scanner->next();
            if (cell == NULL)
                break;
            num++;
            cell->debug();
        }
        cout << "Scaned " << num << " rows" << endl;
        scanner->close();
        return this;
    }

    IO_TEST* compact(AirinStore *store) {
        char filename[1024];
        sprintf(filename, "%s/%ld_%lld.%s", DB_DATA_PATH, time(0), Helper::getRandom(0, (1LL << 60)), "compacted");
        AirinFile *file = new AirinFile(filename);
        file->open("w+");
        AirinFilePopulation *pop = new AirinFilePopulation(file);
        AirinStoreScanner *scanner = new AirinStoreScanner(store);
        scanner->init();
        int num = 0;
        while (true) {
            AirinCell *cell = scanner->next();
            if (cell == NULL)
                break;
            //cell->debug();
            pop->add(new AirinCell(cell));
            num++;
        }
        cout << "Compacted " << num << " rows to " << filename << endl;
        scanner->close();
        pop->close();
        file->close();
        return this;
    }

};
