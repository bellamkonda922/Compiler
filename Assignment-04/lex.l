%{
  #include<bits/stdc++.h>
  #include "yacc.tab.h"

  using namespace std;

  int cnt =  1;
  extern vector<string>errors;
  void errorhandler();  
  string converttoocta(int ans);
%}

%option noyywrap

%%

PROGRAM 			{return T_PROGRAM;}
VAR 				{return T_VAR;}
BEGIN  				{return T_BEGIN;}
END 				{return T_END;}
"END."          	{return T_END_DOT;}
INTEGER         	{return T_INT_TYPE;}
REAL                {return T_REAL_TYPE;}
FOR 				{return T_FOR;}	
READ 				{return T_READ;}
WRITE 				{return T_WRITE;}
TO  				{return T_TO;}
DO 					{return T_DO;}
";" 				{return T_SEMICOLON;}
":" 				{return T_COLON;}
"," 				{return T_COMMA;}
":="  				{return T_ASSIGN;}
"+" 				{return T_PLUS;}
"-" 				{return T_MINUS;}
"*" 				{return T_MULT;}
DIV 				{return T_DIV;}
"(" 				{return T_OPEN_BRACKET;}
")"					{return T_CLOSE_BRACKET;}

[A-Z]([A-Z0-9])* 	{
						yylval.svalue = yytext;
						return T_ID;
					}

[0-9]+				{
						yylval.ivalue = atoi(yytext);
						return T_INT;
					}

[0-9]+"."[0-9]+	    {
						yylval.dvalue = atof(yytext);
						return T_REAL;
                    }

[\t]				{}
" "					{}
"\n"				{cnt++;}
.					{errorhandler();return -1;}

%%

void errorhandler(){
    string err = "\'";
    err.push_back(yytext[0]);
    err+="\'";
    err+=" (";
    string tmp="";
    int ans = (int)yytext[0];
    tmp = converttoocta(ans);
    err+="0";
    err+=tmp;
    err+=")";
    err+=" : illegal character present at line ";
    err+=to_string(cnt);
    errors.push_back(err);
}

string converttoocta(int ans){
  string res="";
  while(ans){
    char c;
    c = ans%8;
    c = c+'0';
    res.push_back(c);
    ans = ans/8;
  }
  string final = "";
  int sz = res.size();
  for(int i=sz;i>=0;i--){
    char c ;
    c = res[i];
    final.push_back(c);
  }
  return final;
}