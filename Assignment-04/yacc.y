%{
 #include<bits/stdc++.h>
 using namespace std;
struct linklist{
    string val;
    linklist* next;
};


//hash table implementation
class hashtable{
    vector<linklist*>hash;
    int size =0;
    
    public:
        int hasher(string s){
            int ans = 0;
            for(int i=0;i<s.size();i++){
                int k=0;
                k+=(int)s[i];
                ans+=k;
                ans%=size;
            }
            return ans;
        }
        hashtable(int size){
            this->hash = vector<linklist*>(size,NULL);
            this->size = size;
        }
        int find(string value){
            int ind = hasher(value);
            linklist* node = hash[ind];
            bool ok = 0;
            while(node){
                if(node->val==value){
                    ok=1;
                    break;
                }
                node = node->next;
            }
            return ok;
        }
        int totsize(){
            int ans = 0,i=0;
            while(i<size){
                while(hash[i]){
                    ans++;
                    hash[i] = hash[i]->next;
                }
                i++;
            }
            return ans;
        }

        int empty(){
            return this->totsize()==0;
        }

        void erase(string value){
            bool  ok = find(value);
            if(ok==0){
                return;
            }
            int ind = hasher(value);
            linklist*temp = hash[ind];
            linklist*prev = NULL;
            while(temp->val!=value){
                prev = temp;
            }
            prev->next = temp->next;
            temp->next = NULL;
            return;
        }
        int insert(string value){
            bool ok=0;
            ok = find(value);
            if(ok){
                return 0;
            }
            else{
                int ind = hasher(value);
                linklist* node = new linklist;
                node->val = value;
                node->next = NULL;
                linklist* temp = hash[ind];
                node->next = temp;
                hash[ind] = node;
                return 1;
            }
        }

};


 extern int yylex(void);
 extern int cnt;
 void yyerror(const string s);
 extern vector<string>errors;    
 struct variableinfo{
     string name;
     int type;
     int line_no;
 };

 
 int variable_activated = 0;
 vector<string>varids;
 unordered_map<string,struct variableinfo>symtab;
 extern vector<string>reservedwords; 
%}

%union{
    int ivalue;
    double dvalue;
    char* svalue;
}

%token T_PROGRAM 1
%token T_VAR 2
%token T_BEGIN 3
%token T_END 4
%token T_END_DOT 5
%token <ivalue> T_INT_TYPE 6
%token <ivalue> T_REAL_TYPE 7
%token T_FOR 8
%token T_READ 9
%token T_WRITE 10
%token T_TO 11
%token T_DO 12
%token T_SEMICOLON 13
%token T_COLON 14
%token T_COMMA 15
%token T_ASSIGN 16
%token T_PLUS 17
%token T_MINUS 18
%token T_MULT 19
%token T_DIV 20
%token T_OPEN_BRACKET 21
%token T_CLOSE_BRACKET 22
%token <svalue> T_ID 23
%token <ivalue> T_INT 24
%token <dvalue> T_REAL 25

%type <svalue> ID
%type <ivalue> type exp term factor

%%
prog        : T_PROGRAM prog_name T_VAR {variable_activated=1;} dec_list {variable_activated=0;} T_BEGIN stmt_list T_END_DOT
              ;

prog_name   : ID
              ;

dec_list    : dec | dec_list T_SEMICOLON dec
              ;

dec         : id_list T_COLON type{
                for(auto ids:varids){
                    if(symtab.find(ids) != symtab.end()){
                        string err ="";
                        err = "Duplicate Variable definition is there at the line "+to_string(cnt) + ", "+ids + " is already been defined at the line "+to_string(symtab.find(ids)->second.line_no);
                        errors.push_back(err);
                    }
                    else{
                        struct variableinfo tmp;
                        tmp.type = $3;
                        tmp.name = ids;
                        tmp.line_no = cnt;
                        symtab.insert({ids,tmp});
                    }
                }
                varids.clear();
            }
             ;

type        : T_REAL_TYPE {$$ = T_REAL_TYPE;} | T_INT_TYPE {$$ = T_INT_TYPE;}
              ;

id_list     : ID {
                if(variable_activated){
                    string tmp($1);
                    varids.push_back(tmp);
                }
            }
            | id_list T_COMMA T_ID{
              if(variable_activated){
                  string tmp($3);
                  varids.push_back(tmp);
              }
            }
             ;

stmt_list   : stmt | stmt_list T_SEMICOLON stmt
              ;

stmt        : assign | for |  write | read 
              ;

assign      : ID T_ASSIGN exp{
                char* temp;
                temp = (char *)malloc(sizeof(char)*256);
                for(int i = 0; i < 256 && $1[i] != ' '; ++i) {
                    temp[i] = $1[i];
                }
                string tmp(temp);
                
                $1 = temp;
                if(symtab.find(tmp) == symtab.end()){
                    string err = "";
                    err+="Error at the line "+to_string(cnt)+", "+$1+" variable is not declared in the scope";
                    errors.push_back(err);
                }
                else if(symtab.find(tmp)->second.type != $3){
                    int type1 = symtab.find(tmp)->second.type;
                    int type2 = $3 ;
                    string t1,t2,err="";
                    if(type1 == (int) T_INT_TYPE ){
                        t1 = "INTEGER";
                    }
                    else if(type1 == (int)T_REAL_TYPE){
                        t1 = "REAL";
                    }
                    if(type2 == (int)T_INT_TYPE){
                        t2 = "INTEGER";
                    }
                    else if(type2 == (int)T_REAL_TYPE){
                        t2 = "REAL";
                    }
                    err+="Error at the line "+to_string(cnt)+", Incompatible types "+t1+" & "+t2;
                    errors.push_back(err);
                }

            }
              ;

exp         : term {$$ = $1;}
            | exp T_MINUS term {
                bool ok= ($1==$3);
                if(!ok){
                    int type1 = $1;
                    int type2 = $3 ;
                    string t1,t2,err="";
                    if(type1 == (int) T_INT_TYPE ){
                        t1 = "INTEGER";
                    }
                    else if(type1 == (int)T_REAL_TYPE){
                        t1 = "REAL";
                    }
                    if(type2 == (int)T_INT_TYPE){
                        t2 = "INTEGER";
                    }
                    else if(type2 == (int)T_REAL_TYPE){
                        t2 = "REAL";
                    }
                    err+="Error at the line "+to_string(cnt)+", Incompatible types "+t1+" & "+t2;
                    errors.push_back(err);
                }
                else{
                    $$ = $1;
                }
            }
            | exp T_PLUS term {
                bool ok= ($1==$3);
                if(!ok){
                    int type1 = $1;
                    int type2 = $3 ;
                    string t1,t2,err="";
                    if(type1 == (int) T_INT_TYPE ){
                        t1 = "INTEGER";
                    }
                    else if(type1 == (int)T_REAL_TYPE){
                        t1 = "REAL";
                    }
                    if(type2 == (int)T_INT_TYPE){
                        t2 = "INTEGER";
                    }
                    else if(type2 == (int)T_REAL_TYPE){
                        t2 = "REAL";
                    }
                    err+="Error at the line "+to_string(cnt)+", Incompatible types "+t1+" & "+t2;
                    errors.push_back(err);
                }
                else{
                    $$ = $1;
                }
            }
            
              ;

term        : factor {$$ = $1;}
            | term T_DIV factor {
                bool ok= ($1==$3);
                if(!ok){
                    int type1 = $1;
                    int type2 = $3 ;
                    string t1,t2,err="";
                    if(type1 == (int) T_INT_TYPE ){
                        t1 = "INTEGER";
                    }
                    else if(type1 == (int)T_REAL_TYPE){
                        t1 = "REAL";
                    }
                    if(type2 == (int)T_INT_TYPE){
                        t2 = "INTEGER";
                    }
                    else if(type2 == (int)T_REAL_TYPE){
                        t2 = "REAL";
                    }
                    err+="Error at the line "+to_string(cnt)+", Incompatible types "+t1+" & "+t2;
                    errors.push_back(err);
                }
                else{
                    $$ = $1;
                }
            }
            | term T_MULT factor {
                bool ok= ($1==$3);
                if(!ok){
                    int type1 = $1;
                    int type2 = $3 ;
                    string t1,t2,err="";
                    if(type1 == (int) T_INT_TYPE ){
                        t1 = "INTEGER";
                    }
                    else if(type1 == (int)T_REAL_TYPE){
                        t1 = "REAL";
                    }
                    if(type2 == (int)T_INT_TYPE){
                        t2 = "INTEGER";
                    }
                    else if(type2 == (int)T_REAL_TYPE){
                        t2 = "REAL";
                    }
                    err+="Error at the line "+to_string(cnt)+", Incompatible types "+t1+" & "+t2;
                    errors.push_back(err);
                }
                else{
                    $$ = $1;
                }
            }
              ;

factor      : ID {
                char* temp;
                temp = (char *)malloc(sizeof(char)*256);
                for(int i = 0; i < 256 && $1[i] != ' '; ++i) {
                    temp[i] = $1[i];
                }
                string tmp(temp);
                
                $1 = temp;
                if(symtab.find(tmp)==symtab.end()){
                    string err = "";
                    err+="Error at the line"+to_string(cnt)+", "+$1+" variable is not declared in the scope";
                    errors.push_back(err);
                }
                else{
                    $$ = symtab.find(tmp)->second.type;
                }

            }
            | T_REAL {$$ = T_REAL_TYPE;}
            | T_INT {$$ = T_INT_TYPE;}
            | T_OPEN_BRACKET exp T_CLOSE_BRACKET {$$ = $2;}
              ;

read        : T_READ T_OPEN_BRACKET id_list T_CLOSE_BRACKET
              ;

write       : T_WRITE T_OPEN_BRACKET id_list T_CLOSE_BRACKET
              ;

for         : T_FOR index_exp T_DO body
              ;

index_exp   : ID T_ASSIGN exp T_TO exp {
                char* temp;
                temp = (char *)malloc(sizeof(char)*256);
                for(int i = 0; i < 256 && $1[i] != ' '; ++i) {
                    temp[i] = $1[i];
                }
                string tmp(temp);
                
                $1 = temp;
                if(symtab.find(tmp)==symtab.end()){
                    string err = "";
                    err+="Error at the line"+to_string(cnt)+", "+$1+" variable is not declared in the scope";
                    errors.push_back(err);
                }
                else{
                    if(symtab.find(tmp)->second.type!=$3){
                        int type1 = symtab.find(tmp)->second.type;
                        int type2 = $3 ;
                        string t1,t2,err="";
                        if(type1 == (int) T_INT_TYPE ){
                            t1 = "INTEGER";
                        }
                        else if(type1 == (int)T_REAL_TYPE){
                            t1 = "REAL";
                        }
                        if(type2 == (int)T_INT_TYPE){
                            t2 = "INTEGER";
                        }
                        else if(type2 == (int)T_REAL_TYPE){
                            t2 = "REAL";
                        }
                        err+="Error at the line "+to_string(cnt)+", Incompatible types "+t1+" & "+t2;
                        errors.push_back(err);
                    }
                    else if(symtab.find(tmp)->second.type!=$5){
                        int type1 = symtab.find(tmp)->second.type;
                        int type2 = $5 ;
                        string t1,t2,err="";
                        if(type1 == (int) T_INT_TYPE ){
                            t1 = "INTEGER";
                        }
                        else if(type1 == (int)T_REAL_TYPE){
                            t1 = "REAL";
                        }
                        if(type2 == (int)T_INT_TYPE){
                            t2 = "INTEGER";
                        }
                        else if(type2 == (int)T_REAL_TYPE){
                            t2 = "REAL";
                        }
                        err+="Error at the line "+to_string(cnt)+", Incompatible types "+t1+" & "+t2;
                        errors.push_back(err);
                    }
                }
            }
              ;

body        : stmt | T_BEGIN stmt_list T_END
              ;

ID          : T_ID
                {
                    string tmp_name(yylval.svalue);
                    for(auto wor:reservedwords){
                        if(wor == tmp_name){
                            string err = "";
                            err+="Error at the line"+to_string(cnt)+", "+tmp_name+" cannot be a reserved keyword";
                            errors.push_back(err);
                        }
                    }
                    $$ = yylval.svalue;
                    
                }
                ;

%%

void yyerror(const string s){
    string err = "";
    err+="Error at the line "+to_string(cnt)+", "+s;
    errors.push_back(err);
}

