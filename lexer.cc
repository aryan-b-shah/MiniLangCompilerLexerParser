#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE", "INT", "REAL", "BOOL", "TR",
"FA", "IF", "WHILE", "SWITCH", "CASE", "PUBLIC", "PRIVATE", "NUM", 
"REALNUM", "NOT", "PLUS", "MINUS", "MULT", "DIV", "GTEQ", "GREATER", 
"LTEQ", "NOTEQUAL", "LESS", "LPAREN", "RPAREN", "EQUAL", "COLON", 
"COMMA", "SEMICOLON", "LBRACE", "RBRACE", "ID", "ERROR"
};

#define KEYWORDS_COUNT 11
string keyword[] = { "int", "real", "bool", "true", "false", "if", "while", "switch", "case", "public", "private" };

LexicalAnalyzer lexer;
Token token;
TokenType tempTokenType;
float enumCount = 4;

struct sTableEntry{
    string name;
    int lineNo;
    float type;
    bool printed;
};

struct sTable{
    sTableEntry* item;
    sTable *prev;
    sTable *next;
};

sTable* symbolTable;
char* lResolve;
char* rResolve;
int line = 0;

void addList(std::string name, int line, int type){
    if(symbolTable == NULL){
        sTable* newEntry = new sTable();
        sTableEntry* newItem = new sTableEntry();

        newItem->name = name;
        newItem->lineNo = token.line_no;
        newItem->type = type;
        newItem->printed = false;

        newEntry->item = newItem;
        newEntry->next = NULL;
        newEntry->prev = NULL;

        symbolTable = newEntry;
    }
    else{
        sTable* temp = symbolTable;
        
        while(temp->next != NULL){
            temp = temp->next;
        }

        sTable* newEntry = new sTable();
        sTableEntry* newItem = new sTableEntry();

        newItem->name = name;
        newItem->lineNo = token.line_no;
        //memcpy(newItem->scope, currentScope.c_str(), currentScope.size()+1);
        newItem->type = type;
        newItem->printed = false;

        newEntry->item = newItem;
        newEntry->next = NULL;
        newEntry->prev = temp;
        temp->next = newEntry;
    }
}

int Search_List(std::string n){
    sTable* temp = symbolTable;
    bool found = false;
    if (symbolTable == NULL){
        //cout<<"duplicate check"<<endl;
        addList(n, token.line_no, enumCount);
        enumCount++;
        return (4);
    }
    else{
        while(temp->next != NULL){
            if(strcmp(temp->item->name.c_str(), n.c_str()) == 0){
                found = true;
                return(temp->item->type);
            }
            else{
                temp = temp->next;
            }
        }
        if(strcmp(temp->item->name.c_str(), n.c_str()) == 0){
            found = true;
            return(temp->item->type);
        }
        if(!found){
            addList(n, token.line_no, enumCount);
            enumCount++;
            int t = enumCount-1;
            return(t);
        }
        else{}
    }
}


void Token::Print()
{
    cout << "{" << this->lexeme << " , "
        << reserved[(int) this->token_type] << " , "
        << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    line = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');
    line = line_no;

    while (!input.EndOfInput() && isspace(c))
    {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
        line = line_no;
    }

    if (!input.EndOfInput())
    {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::SkipComments()
{
    bool comments = false;
    char c;
    if(input.EndOfInput() )
    {
        input.UngetChar(c);
        return comments;
    }

    input.GetChar(c);

    if(c == '/')
    {
        input.GetChar(c);
        if(c == '/')
        {
            comments = true;
            while(c != '\n')
            {
                comments = true;
                input.GetChar(c);
            }
            line_no++;
            line = line_no;
            SkipComments();
        }
        else
        {
            comments = false;
            cout << "Syntax Error\n";
            exit(0);
        }
    }
    else
    {
        input.UngetChar(c);
        return comments;
    }
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
        {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
        {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;
    bool realNUM = false;
    input.GetChar(c);
    if (isdigit(c))
    {
        if (c == '0')
        {
            tmp.lexeme = "0";
            input.GetChar(c);
            if(c == '.')
            {            
                input.GetChar(c);

                if(!isdigit(c))
                {
                    input.UngetChar(c);
                }
                else
                {
                    while (!input.EndOfInput() && isdigit(c))
                    {
                        tmp.lexeme += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                    input.UngetChar(c);
                }
            }
            else
            {
                input.UngetChar(c);
            }
        }
        else
        {
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c))
            {
                tmp.lexeme += c;
                input.GetChar(c);
            }
            if(c == '.')
            {               
                input.GetChar(c);

                if(!isdigit(c))
                {
                    input.UngetChar(c);
                }
                else
                {
                    while (!input.EndOfInput() && isdigit(c))
                    {
                        tmp.lexeme += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                }
            }
            if (!input.EndOfInput())
            {
                input.UngetChar(c);
            }
        }
        // TODO: You can check for REALNUM, BASE08NUM and BASE102NUM here!
        if(realNUM)
        {
            tmp.token_type = REALNUM;
        }
        else
        {
            tmp.token_type = NUM;
        }
        tmp.line_no = line_no;
        return tmp;
    }
    else
    {
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c))
    {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c))
        {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;

        if (IsKeyword(tmp.lexeme))
        {
             tmp.token_type = FindKeywordIndex(tmp.lexeme);
        }
        else
        {
            tmp.token_type = ID;
        }
    }
    else
    {
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//

TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty())
    {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    SkipComments();
    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    //cout << "\n Char obtained " << c << "\n";
    switch (c)
    {
        case '!':
            tmp.token_type = NOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '>':
            input.GetChar(c);
            if(c == '=')
            {
                tmp.token_type = GTEQ;
            }
            else
            {
                input.UngetChar(c);
                tmp.token_type = GREATER;
            }
            return tmp;
        case '<':
            input.GetChar(c);
            if(c == '=')
            {
                tmp.token_type = LTEQ;
            }
            else if (c == '>')
            {
                tmp.token_type = NOTEQUAL;
            }
            else
            {
                input.UngetChar(c);
                tmp.token_type = LESS;
            }
            return tmp;
        case '(':
            //cout << "\n I am here" << c << " \n";
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '{':
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            return tmp;
        default:
            if (isdigit(c))
            {
                input.UngetChar(c);
                return ScanNumber();
            }
            else if (isalpha(c))
            {
                input.UngetChar(c);
                //cout << "\n ID scan " << c << " \n";
                return ScanIdOrKeyword();
            }
            else if (input.EndOfInput())
            {
                tmp.token_type = END_OF_FILE;
            }
            else
            {
                tmp.token_type = ERROR;
            }
            return tmp;
    }
}

bool ifMatch(int i){
    // Check if 'i' is one of the operator tokens
    return (i == PLUS || i == MINUS || i == MULT || i == DIV || 
            i == GREATER || i == LESS || i == GTEQ || i == LTEQ || 
            i == EQUAL || i == NOTEQUAL);
}

bool ifMatch2(int c){
    // Check if 'c' is not one of the specific tokens
    return (c != 101 && c != 102 && c != 103 && c != 104 && c != 105 && 
            c != 100 && c != 107 && c != 108 && c != 109 && c != 110);
}

bool ifMatch3(int n){
    // Check if 'n' is one of the valid tokens
    return (n == ID || n == NUM || n == REALNUM || n == TR ||
            n == FA || n == PLUS || n == MINUS || n == MULT || 
            n == DIV || n == LESS || n == GREATER || n == GTEQ || 
            n == LTEQ || n == EQUAL || n == NOTEQUAL || n == NOT);
}

void matchL(int line_No, int token_Type){
    // Set type for the symbol with given line number
    sTable* temp = symbolTable;
    while(temp->next != NULL){
        if(temp->item->lineNo == line_No){
            temp->item->type = token_Type;
        }
        temp = temp->next;
    }
    if(temp->item->lineNo == line_No){
        temp->item->type = token_Type;
    }
}

void updateType(int currentType, int newType){
    // Change all symbols of 'currentType' to 'newType'
    sTable* temp = symbolTable;
    while(temp->next != NULL){
        if(temp->item->type == currentType){
            temp->item->type = newType;
        }
        temp = temp->next;
    }
    if(temp->item->type == currentType){
        temp->item->type = newType;
    }
}

int parse_varlist(void)
{
    token = lexer.GetToken();
    int tempI;
    char* lexeme = (char*)malloc(sizeof(token.lexeme)+1);
    memcpy(lexeme, (token.lexeme).c_str(), (token.lexeme).size()+1);
    addList(lexeme, token.line_no, 0);

    if(token.token_type == ID){
        token = lexer.GetToken();
        if(token.token_type == COMMA){
        tempI = parse_varlist();
        }
        else if(token.token_type == COLON){
            tempTokenType = lexer.UngetToken(token);
        }
        else{
            cout << "\n Syntax Error \n";
        }
    }
    else{
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_body(void);
Token token1;

int parse_unaryOperator(void){
    token = lexer.GetToken();

    if(token.token_type == NOT){
        return(1);
    }
    else{
        cout << "\n Syntax Error\n";
        return(0);
    }
}

int parse_binaryOperator(void){
    token = lexer.GetToken();
    if(token.token_type == PLUS  ){
        return(101);
    }
    else if(token.token_type == MINUS ){
        return(102);
    }
    else if(token.token_type == MULT){
        return(103);
    }
    else if(token.token_type == DIV ){
        return(104);
    }
    else if(token.token_type == GREATER){
        return(100);
    }
    else if(token.token_type == LESS  ){
        return(109);
    }
    else if(token.token_type == GTEQ ){
        return(105);
    }
    else if(token.token_type == LTEQ){
        return(107);
    }
    else if(token.token_type == EQUAL ){
        return(110);
    }
    else if(token.token_type == NOTEQUAL){
        return(108);
    }
    else{
        cout << "\n Syntax Error \n";
        return(-1);
    }
}

int parse_primary(void){
    token = lexer.GetToken();
    if(token.token_type == ID ){
        return(Search_List(token.lexeme));
    }
    else if(token.token_type == NUM ){
        return(1);
    }
    else if(token.token_type == REALNUM){
        return(2);
    }
    else if(token.token_type == TR ){
        return(3);
    }
    else if(token.token_type == FA){
        return(3);
    }
    else{
        cout << "\n Syntax Error \n";
        return(0);
    }
}

int parse_expression(void){
    int tempI;

    token = lexer.GetToken();

    if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA ){
        lexer.UngetToken(token);
        tempI = parse_primary();
    }else if(ifMatch(token.token_type)){
        int expressionL;
        int expressionR;
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_binaryOperator(); 
        expressionL = parse_expression();
        expressionR = parse_expression();     
        if((expressionL != expressionR) || ifMatch2(tempI)){ 
            if(tempI == 101 || tempI == 102 || tempI == 103 || tempI == 104){
                if(expressionL <= 2 && expressionR > 3){
                    updateType(expressionR, expressionL);
                    expressionR = expressionL;
                }
                else if(expressionL > 3 && expressionR <= 2){
                    updateType(expressionR, expressionL);
                    expressionL = expressionR;
                }
                else if(expressionL > 3 && expressionR > 3){
                    updateType(expressionR, expressionL);
                    expressionR = expressionL;
                }
                else{
                    cout << "TYPE MISMATCH " << token.line_no << " C2"<<endl;
                    exit(1);
                }
            }
            else if(tempI == 105 || tempI == 100 || tempI == 107 || tempI == 108 || tempI == 109 || tempI == 110){
                if(expressionR > 3 && expressionL > 3){
                    updateType(expressionR, expressionL);
                    expressionR = expressionL;
                    return(3);
                }
                else{
                    cout << "TYPE MISMATCH " << token.line_no << " C2"<<endl;
                    exit(1);
                }
            }
            else{
                cout << "TYPE MISMATCH " << token.line_no << " C2"<<endl;
                exit(1);
            }
        }
        if(tempI == 105 || tempI == 100 || tempI == 107 || tempI == 109 || tempI == 110 ){
            tempI = 3;
        }
        else{
            tempI = expressionR;
        }
    }
    else if(token.token_type == NOT){
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_unaryOperator(); 
        tempI = parse_expression();
        if(tempI != 3){
            cout << "TYPE MISMATCH " << token.line_no << " C3"<<endl;
            exit(1);
        }
    }
    else{
        cout << "\n Syntax Error \n";
        return(0);
    }
    return tempI;
}

int parse_assstmt(void){
    int tempI;    
    char* name;
    float leftHandSide;
    float rightHandSide;
    token = lexer.GetToken();
    if(token.token_type == ID){
        leftHandSide = Search_List(token.lexeme);
        token = lexer.GetToken();
        if(token.token_type == EQUAL){
            token = lexer.GetToken();
            if(ifMatch3(token.token_type)){
                lexer.UngetToken(token);
                rightHandSide = parse_expression();
                if(leftHandSide == 1 || leftHandSide == 2 || leftHandSide == 3){
                    if(leftHandSide != rightHandSide){
                        if(leftHandSide >= 3){
                            updateType(rightHandSide,leftHandSide);
                            rightHandSide = leftHandSide;
                        }
                        else{
                            cout << "TYPE MISMATCH " << token.line_no << " C1" << endl;
                            exit(1);
                        }
                    }
                }
                else{
                    updateType(leftHandSide,rightHandSide);
                    leftHandSide = rightHandSide;
                }
                token = lexer.GetToken();
                if(token.token_type == SEMICOLON){
                    //cout << "\n Rule parsed: assignment_stmt -> ID EQUAL expression SEMICOLON"<<endl;
                }
                else{
                    cout << "\n HI Syntax Error " << token.token_type << " \n";
                }
            }
            else{
                cout << "\n Syntax Error \n";
            }
        }
        else{
            cout << "\n Syntax Error \n";
        }
    }
    else{
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_case(void){
    int tempI;
    token = lexer.GetToken();
    if(token.token_type == CASE ){
        token = lexer.GetToken();
        if(token.token_type == NUM){
            token = lexer.GetToken();
            if(token.token_type == COLON){
                tempI = parse_body();
            }
            else{
                cout << "\n Syntax Error \n";
            }
        }
        else{
            cout << "\n Syntax Error \n";
        }
    }
    else{
        cout << "\n Syntax Error \n";
    }
}

int parse_caselist(void){
    int tempI;
    token = lexer.GetToken();
    if(token.token_type == CASE){
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_case();
        token = lexer.GetToken();
        if(token.token_type == CASE){
            tempTokenType = lexer.UngetToken(token);
            tempI = parse_caselist();
        }
        else if(token.token_type == RBRACE){
            tempTokenType = lexer.UngetToken(token);
        }
    }
    return(0);
}

int parse_switchstmt(void){
    int tempI;
    token = lexer.GetToken();
    if(token.token_type == SWITCH){
        token = lexer.GetToken();
        if(token.token_type == LPAREN){
            tempI = parse_expression();
            if(tempI <= 3 && tempI != 1){
                cout<< "TYPE MISMATCH " << token.line_no << " C5"<<endl;
                exit(1);
            }
            token = lexer.GetToken();
            if(token.token_type == RPAREN){
                token = lexer.GetToken();
                if(token.token_type == LBRACE){
                    tempI = parse_caselist();
                    token = lexer.GetToken();
                    if(token.token_type == RBRACE){
                        //cout << "\n Rule parsed: switch_stmt -> SWITCH LPAREN expression RPAREN LBRACE case_list RBRACE \n";
                    }
                    else{
                        cout << "\n Syntax Error \n";
                    }
                }
                else{
                    cout << "\n Syntax Error \n";
                }
            }
            else{
                cout << "\n Syntax Error \n";
            }
        }
        else{
            cout << "\n Syntax Error \n";
        }
    }
    else{
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_whilestmt(void){
    int tempI;

    token = lexer.GetToken();
    if(token.token_type == WHILE){
        token = lexer.GetToken();
        if(token.token_type == LPAREN){
            tempI = parse_expression();
            if(tempI != 3){
                cout<< "TYPE MISMATCH " << token.line_no << " C4" << endl;
                exit(1);
            }
            token = lexer.GetToken();
            if(token.token_type == RPAREN){
                //cout << "\n Rule parsed: whilestmt -> WHILE LPAREN expression RPAREN body \n";
                tempI = parse_body();
            }
            else{
                cout << "\n Syntax Error \n";
            }
        }
        else{
            cout << "\n Syntax Error \n";
        }
    }
    else{
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_ifstmt(void){
    int tempI;
    token = lexer.GetToken();
    if(token.token_type == IF){
        token = lexer.GetToken();
        if(token.token_type == LPAREN){
            tempI = parse_expression();
            if(tempI != 3){
                cout<< "TYPE MISMATCH " << token.line_no << " C4" << endl;
                exit(1);
            }
            token = lexer.GetToken();
            if(token.token_type == RPAREN){
                //cout << "\n Rule parsed: ifstmt -> IF LPAREN expression RPAREN body \n";
                tempI = parse_body();
            }
            else{
                cout << "\n Syntax Error \n";
            }
        }
        else{
            cout << "\n Syntax Error \n";
        }
    }
    else{
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_stmt(void){
    int tempI;
    token = lexer.GetToken();
    if(token.token_type == ID){
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_assstmt();

    }
    else if(token.token_type == IF){
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_ifstmt();
    }
    else if(token.token_type == WHILE){
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_whilestmt();
    }
    else if(token.token_type == SWITCH){
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_switchstmt();
    }
    else{
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_stmtlist(void){
    token = lexer.GetToken();
    int tempI;
    if(token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH){
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_stmt();
        token = lexer.GetToken();
        if(token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH){
            tempTokenType = lexer.UngetToken(token);
            tempI = parse_stmtlist();

        }
        else if (token.token_type == RBRACE){
            tempTokenType = lexer.UngetToken(token);           
        }
    }
    else{
        cout << "\n Syntax Error \n";
    }
    return(0);
}

int parse_body(void){
    token = lexer.GetToken();
    int tempI;
    if(token.token_type == LBRACE){
        tempI = parse_stmtlist();
        token = lexer.GetToken();
        if(token.token_type == RBRACE){
            return(0);
        }
        else{
            cout << "\n Syntax Error\n ";
            return(0);
        }
    }
    else if(token.token_type == END_OF_FILE){
        tempTokenType = lexer.UngetToken(token);
        return(0);
    }
    else{
        cout << "\n Syntax Error \n ";
        return(0);
    }
}

int parse_typename(void){
    token = lexer.GetToken();
    if(token.token_type == INT || token.token_type == REAL || token.token_type == BOO){
        matchL(token.line_no, token.token_type);
    }
    else{
        cout << "\n Syntax Error in parse_typename \n";
    }
    return(0);
}

int parse_vardecl(void){
    int tempI;
    token = lexer.GetToken();
    if(token.token_type == ID){
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_varlist();
        token = lexer.GetToken();
        if(token.token_type == COLON){
            tempI = parse_typename();
            token = lexer.GetToken();
            if(token.token_type == SEMICOLON){
                //cout << "\n Rule parsed: var_decl -> var_list COLON type_name SEMICOLON"<<endl;
            }
            else{
                cout << "\n Syntax Error \n";
            }
        }
        else{
            cout << "\n Syntax Error \n";
        }
    }
    else{
        cout << "\n Syntax Error \n";
    }    
    return(0);
}

int parse_vardecllist(void){
    int tempI;
    token = lexer.GetToken();
    while(token.token_type == ID){
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_vardecl();
        token = lexer.GetToken();
    }
    tempTokenType = lexer.UngetToken(token);
    return(0);
}

string global = "::";
int parse_globalVars(void){
    token = lexer.GetToken();
    int tempI;
    if(token.token_type == ID){
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_vardecllist();
    }
    else{
        cout << "Syntax Error";
    }
    return(0);
}

int parse_program(void){
    token = lexer.GetToken();
    int tempI;
    while (token.token_type != END_OF_FILE){
        if(token.token_type == ID){
            tempTokenType = lexer.UngetToken(token);
            tempI = parse_globalVars();
            tempI = parse_body();
        }
        else if(token.token_type == LBRACE){
            tempTokenType = lexer.UngetToken(token);
            tempI = parse_body();
        }
        else if(token.token_type == END_OF_FILE){
            return(0);
        }
        else{
            cout << "\n Syntax Error\n";
            return(0);
        }
        token = lexer.GetToken();
    }
}

string output = "";

void printList(void){
    sTable* temp = symbolTable;
    float temp1;

    while(temp->next != NULL){
       if(temp->item->type < 4 && temp->item->printed == false){
            string lCase = keyword[int(temp->item->type)-1 ];
            float temp1 = temp->item->type;
            output = temp->item->name + ": " + lCase + " #";
            cout << output <<endl;
            output = "";
            temp->item->printed = true;           

            while(temp->next != NULL  && temp->next->item->type == temp1){
                temp = temp->next;
                string lCase2 = keyword[int(temp->item->type)-1];
                output = temp->item->name + ": " + lCase2 + " #";
                cout << output <<endl;
                temp->item->printed = true;
                output = "";
            }
        }
        else if(temp->item->type > 3 && temp->item->printed == false){          
            temp1 = temp->item->type;
            output += temp->item->name;
            temp->item->printed = true;
            while(temp->next != NULL){
                temp = temp->next;
                if(temp->item->type == temp1){
                    output += ", " + temp->item->name;
                    temp->item->printed = true;
                }
                else{}
            }
            output += ": ? #";
            cout << output <<endl;
            temp->item->printed = true;
            output = "";
            temp = symbolTable;
        }
        else{
            temp = temp->next;
        }
    }
    if(temp->item->type > 3 && temp->item->printed == false){
        output += temp->item->name + ":" + " ? " + "#";
        cout << output <<endl;
        output = "";
    }
    else if(temp->item->type <= 3 && temp->item->printed == false){        
        string lCase3 = keyword[int(temp->item->type)-1];
        output += temp->item->name + ": " + lCase3 + " #";
        cout << output <<endl;
        output = "";
    } 
    else {}
}

char null[] = "NULL";

int main() {
    int i;
    i = parse_program();
    printList();   
}