#include<bits/stdc++.h>
#define push_back pb;
using namespace std;

extern int yyparse(void);
extern FILE* yyin;

vector<string>errors;
vector<string>reservedwords={"PROGRAM","VAR","BEGIN","END","END.","INTEGER","REAL","FOR","READ","WRITE","TO","DO","DIV"};

int main(int argc, char *argv[]){
    FILE* inp;
    if (argc > 1)
    {
        inp = fopen(argv[1], "r");
    }
    else{
    inp= fopen("incorrect.txt","r");
    }

    if(!inp){
        cout<<"Can't open the file"<<endl;
        return -1;
    }
    yyin = inp;
    cout<<"Compilation has been started"<<endl;
    int ok = yyparse();
    if(!ok){
        cout<<"-----Parse completed Successfully-------"<<endl;
    }
    else{
        cout<<"-----Parse is Unsuccessful------"<<endl;
    }
    cout<<"Compilation has been completed"<<endl;
    fclose(inp);
    ok = errors.size();
    if(ok){
        cout<<"----Errors Are:----"<<endl;
        for(auto err:errors){
            cout<<err<<endl;
        }
    }
    else{
        cout<<"No Syntax or Semantic errors have been found"<<endl;
    }
    return 0;
}