%{
#include<bits/stdc++.h>
using namespace std;
#define program 1
#define var 2
#define begin 3
#define end 4
#define end_dot 5
#define integer 6
#define fore 7
#define reader 8
#define writer 9
#define to 10
#define doe 11
#define semicolon 12
#define colon 13
#define comma 14
#define equivalence 15
#define plus 16
#define minus 17
#define star 18
#define div 19
#define openbracket 20
#define closedbracket 21
#define id 22
#define inti 23

typedef struct yyl{
    char*str;
    char*num;
}yyl;

int cnt=1;
yyl yytemp;

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

hashtable idtable(30);
hashtable inttable(30);
int install_id(string sym){
    int sol = idtable.insert(sym);
    return sol;
}
int install_num(string num){
    int sol = inttable.insert(num);
    return sol;
}

%}

%option noyywrap

%%

PROGRAM 			{return program;}
VAR 				{return var;}
BEGIN  				{return begin;}
END 				{return end;}
"END."          	{return end_dot;}
INTEGER         	{return integer;}
FOR 				{return fore;}	
READ 				{return reader;}
WRITE 				{return writer;}
TO  				{return to;}
DO 					{return doe;}
";" 				{return semicolon;}
":" 				{return colon;}
"," 				{return comma;}
":="  				{return equivalence;}
"+" 				{return plus;}
"-" 				{return minus;}
"*" 				{return star;}
DIV 				{return div;}
"(" 				{return openbracket;}
")"					{return closedbracket;}

[A-Z]([A-Z0-9])* 	{
						yytemp.str = yytext;
						install_id(yytemp.str);
						return id;
					}

[0-9]+				{
						yytemp.num = yytext;
						install_num(yytemp.num);
						return inti;
					}

[\t]				{}
" "					{}
"\n"				{cnt++;}
.					{}

%%


int main(){
    int s = 0;
    yyin = fopen("input.txt","r"); // open the input file
	if(!yyin){
		exit(1);
	}
    cout<<"Line"<<"\t"<<"\t"<<"Token"<<"\t\t"<<"Token "<<"Specifier"<<endl;
	while(s=yylex()){
        if(s==id){
            cout<<cnt<<"\t\t22\t\t^"<<yytemp.str<<endl;
        }
        else if(s==inti){
            cout<<cnt<<"\t\t23\t\t#"<<yytemp.num<<endl;
        }
        else{
            cout<<cnt<<"\t\t"<<s<<endl;
        }
	}
	return 1;
}