//
// Created by Philipp on 19.11.15.
//

#define errorString "\0"
/* @param: int argc, char *argv[]: Kommandozeilenparrameter aus main()
 * @ret: -1 bei Fehler (non valid Input numer) wrongParamError
 * @ret:
 *
 * */
char* checkParam(int argc, char *argv[]){
    if(argc < 1 || argc > 2) {
        return errorString; //wrongParamError
    }
    else{
        char *GameID;
        GameID = malloc(11 * sizeof(char));
        int count = 0;
        while(count<11){
            if(argv[1][count]=='\0'){
                return errorString; //wrongParamError
            }
            GameID[count]=argv[1][count];
            count++;
        }
        if(argv[1][count]!='\0'){
            return errorString; //wrongParamError
        }
        return GameID;
    }
}

