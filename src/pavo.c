#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Maximum lengths for various components
#define MAX_TOKEN_LEN 256
#define MAX_IDENTIFIERS 100
#define MAX_SOURCE_LEN 1000

// Token types
typedef enum {
    TOKEN_EOF,
    TOKEN_LET,
    TOKEN_INT,
    TOKEN_PRINT,
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER,
    TOKEN_COLON_EQUALS,
    TOKEN_EQUALS,
    TOKEN_SEMICOLON,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_EQ_EQ,
    TOKEN_LESS_THAN,
    TOKEN_GREATER_THAN,
    TOKEN_NOT_EQ,
    TOKEN_NOT, //!
    TOKEN_AND, //&
    TOKEN_OR, //|
    TOKEN_IF,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_RETURN,
    TOKEN_LOOP,
    TOKEN_BREAK,
} TokenType;

typedef enum{//0 false 1 true
    TYPE_INTEGER,
    TYPE_UNKNOWN,
} VarType;

// Token structure
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LEN];
    int line;
    int column;
} Token;

// AST node types
typedef enum {
    NODE_NUMBER,
    NODE_VARIABLE,
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_BINOP,
    NODE_REASSIGN,
    NODE_COMPARE,
    NODE_LOGIC,
    NODE_IF_STMT,
    NODE_BLOCK,
    NODE_RETURN,
    NODE_LOOP,
    NODE_BREAK,
} NodeType;

// AST node structure
typedef struct ASTNode {
    NodeType type;
    char value[MAX_TOKEN_LEN];  // Used for variable names and number values
    struct ASTNode* left;       // Used for assignment left side
    struct ASTNode* right;      // Used for assignment right side or print expression
    struct ASTNode* next;
    struct ConditionalBranch* cond_chain;
    struct ASTNode* condition; //loop condition
} ASTNode;

typedef struct ConditionalBranch {
    ASTNode* condition;
    ASTNode* body; //local scope
} ConditionalBranch;

//kell free!!!!!!!!

// Symbol table entry
typedef struct {
    char name[MAX_TOKEN_LEN];
    VarType type;
    int value; //int es bool (0, 1)
} Symbol;

// Global variables for lexer
char* source;
int pos = 0;
int line = 1;
int column = 1;
Symbol symbol_table[MAX_IDENTIFIERS];
int symbol_count = 0;

// Function to create a new AST node
ASTNode* create_node(NodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->left = NULL;
    node->right = NULL;
    node->value[0] = '\0';
    return node;
}

// Function to free an AST node and its children
void free_ast(ASTNode* node) {
    if (node == NULL) return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}

// Lexer error handling
void lexer_error() {
    fprintf(stderr, "Lexical error at line %d, column %d\n", line, column);
    exit(1);
}

// Get the current character
char current_char() {
    if (pos >= strlen(source)) return '\0';
    return source[pos];
}

// Advance to the next character
void advance() {
    pos++;
    column++;
    if (current_char() == '\n') {
        line++;
        column = 1;
    }
}

// Skip whitespace
void skip_whitespace() {
    while (current_char()) {
        if (isspace(current_char())){
            advance();
        } else if (current_char()=='#'){
            while (current_char() && current_char() != '\n'){
                advance();
            }
        } else {
            break;
        }
    }
}

// Get the next token from input (lexer)
Token get_next_token() {
    Token token;
    token.line = line;
    token.column = column;

    skip_whitespace();

    if (current_char() == '\0') {
        token.type = TOKEN_EOF;
        token.value[0] = '\0';
        return token;
    }

    // Handle numbers
    if (isdigit(current_char())) {
        int i = 0;
        while (current_char() && isdigit(current_char())) {
            token.value[i++] = current_char();
            advance();
        }
        token.value[i] = '\0';
        token.type = TOKEN_INTEGER;
        return token;
    }

    // Handle identifiers and keywords
    if (isalpha(current_char())) {
        int i = 0;
        while (current_char() && (isalnum(current_char()) || current_char() == '_')) {
            token.value[i++] = current_char();
            advance();
        }
        token.value[i] = '\0';

        // Check for keywords
        if (strcmp(token.value, "return")==0){
            token.type = TOKEN_RETURN;
        } else if (strcmp(token.value, "loop")==0){
            token.type = TOKEN_LOOP;
        } else if (strcmp(token.value, "break")==0){
            token.type = TOKEN_BREAK;
        } else if (strcmp(token.value, "let") == 0) {
            token.type = TOKEN_LET;
        } else if (strcmp(token.value, "print") == 0) {
            token.type = TOKEN_PRINT;
        } else if (strcmp(token.value, "if") == 0) {
            token.type = TOKEN_IF;
        } else {
            token.type = TOKEN_IDENTIFIER;
        }
        return token;
    }

    if (current_char() == ':'){
        advance();
        if (current_char()=='='){
            token.type = TOKEN_COLON_EQUALS;
            token.value[0] = ':';
            token.value[1] = '=';
            token.value[2] = '\0';
            advance();
            return token;
        }
        lexer_error();
    }

    // Handle special characters
    if (current_char() == '=') {
        advance();
        if (current_char()=='='){
            token.type = TOKEN_EQ_EQ;
            token.value[0] = '=';
            token.value[1] = '=';
            token.value[2] = '\0';
            advance();
            return token;
        }
        token.type = TOKEN_EQUALS;
        token.value[0] = '=';
        token.value[1] = '\0';
        return token;
    }

    if (current_char()=='<') {
        token.type = TOKEN_LESS_THAN;
        token.value[0] = '<';
        token.value[1] = '\0';
        advance();
        return token;
    }

    if (current_char()=='>'){
        token.type = TOKEN_GREATER_THAN;
        token.value[0] = '>';
        token.value[1] = '\0';
        advance();
        return token;
    }

    if (current_char()=='!'){
        advance();
        if (current_char()=='='){
            token.type = TOKEN_NOT_EQ;
            token.value[0] = '!';
            token.value[1] = '=';
            token.value[2] = '\0';
            advance();
        }  else {
            token.type = TOKEN_NOT;
            token.value[0] = '!';
            token.value[1] = '\0';
        }
        return token;
    }

    if (current_char()=='&'){
        token.type = TOKEN_AND;
        token.value[0] = '&';
        token.value[1] = '\0';
        advance();
        return token;
    }

    if (current_char()=='|'){
        token.type = TOKEN_OR;
        token.value[0] = '|';
        token.value[1] = '\0';
        advance();
        return token;
    }

    if (current_char() == ';') {
        token.type = TOKEN_SEMICOLON;
        token.value[0] = ';';
        token.value[1] = '\0';
        advance();
        return token;
    }

    if (current_char()=='+'){
        token.type = TOKEN_PLUS;
        token.value[0] = '+';
        token.value[1] = '\0';
        advance();
        return token;
    }

    if (current_char()=='-'){
        token.type = TOKEN_MINUS;
        token.value[0] = '-';
        token.value[1] = '\0';
        advance();
        return token;
    }

    if (current_char()=='{'){
        token.type = TOKEN_LBRACE;
        token.value[0] = '{';
        token.value[1] = '\0';
        advance();
        return token;
    }

    if (current_char()=='}'){
        token.type = TOKEN_RBRACE;
        token.value[0] = '}';
        token.value[1] = '\0';
        advance();
        return token;
    }

    lexer_error();
    token.type = TOKEN_EOF;  // To satisfy compiler
    return token;
}

// Parser variables
Token current_token;

// Parser error handling
void parser_error() {
    fprintf(stderr, "Syntax error at line %d, column %d\n",
            current_token.line, current_token.column);
    exit(1);
}

// Consume a token of expected type
void eat(TokenType type) {
    if (current_token.type == type) {
        current_token = get_next_token();
    } else {
        parser_error();
    }
}

// Forward declarations for parser functions
ASTNode* parse_expression();
ASTNode* parse_statement();
ASTNode* parse_block();

// Parse a primary expression (number or variable)
ASTNode* parse_primary() {
    ASTNode* node;

    switch (current_token.type) {
        case TOKEN_INTEGER: {
            node = create_node(NODE_NUMBER);
            strcpy(node->value, current_token.value);
            eat(TOKEN_INTEGER);
            return node;
        }
        case TOKEN_IDENTIFIER: {
            node = create_node(NODE_VARIABLE);
            strcpy(node->value, current_token.value);
            eat(TOKEN_IDENTIFIER);
            return node;
        }
        case TOKEN_LBRACE: {
            return parse_block();
        }
        default:
            parser_error();
            return NULL;  // To satisfy compiler
    }
}

ASTNode* parse_arithmetic() {
    ASTNode* node = parse_primary();

    while (current_token.type==TOKEN_PLUS || current_token.type==TOKEN_MINUS) {
        TokenType op_type = current_token.type;
        eat(op_type);

        ASTNode* new_node = create_node(NODE_BINOP);
        new_node->left = node;
        strcpy(new_node->value, op_type==TOKEN_PLUS ? "+" : "-");
        new_node->right = parse_primary();

        node = new_node;
    }

    return node;
}

//LOGICAL:
ASTNode* parse_not() {
    if (current_token.type == TOKEN_NOT){
        eat(TOKEN_NOT);
        ASTNode* node = create_node(NODE_LOGIC);
        strcpy(node->value, "!");
        node->right = parse_not();
        return node;
    }
    return parse_arithmetic();
}

ASTNode* parse_and(){
    ASTNode* node = parse_not();

    while (current_token.type == TOKEN_AND){
        eat(TOKEN_AND);
        ASTNode* new_node = create_node(NODE_LOGIC);
        new_node->left = node;
        strcpy(new_node->value, "&");
        new_node->right = parse_not();
        node = new_node;
    }
    return node;
}

ASTNode* parse_logical() {
    ASTNode* node = parse_and();

    while (current_token.type==TOKEN_OR){
        eat(TOKEN_OR);
        ASTNode* new_node = create_node(NODE_LOGIC);
        new_node->left = node;
        strcpy(new_node->value, "|");
        new_node->right = parse_and();
        node = new_node;
    }
    return node;
}

ASTNode* parse_expression() {
    ASTNode* node = parse_logical();  // Start with logical operators

    // Then handle comparisons
    if (current_token.type == TOKEN_EQ_EQ ||
        current_token.type == TOKEN_LESS_THAN ||
        current_token.type == TOKEN_GREATER_THAN ||
        current_token.type == TOKEN_NOT_EQ) {

        TokenType optype = current_token.type;
        eat(optype);

        ASTNode* new_node = create_node(NODE_COMPARE);
        new_node->left = node;

        switch(optype) {
            case TOKEN_EQ_EQ: strcpy(new_node->value, "=="); break;
            case TOKEN_LESS_THAN: strcpy(new_node->value, "<"); break;
            case TOKEN_GREATER_THAN: strcpy(new_node->value, ">"); break;
            case TOKEN_NOT_EQ: strcpy(new_node->value, "!="); break;
            default: printf("goofy token\n"); break;
        }

        new_node->right = parse_logical();  // Parse right side as logical expression
        return new_node;
    }

    return node;
}

ASTNode* parse_block();

// Parse a statement
ASTNode* parse_statement() {
    ASTNode* node;

    switch (current_token.type) {
        case TOKEN_PRINT: {
            node = create_node(NODE_PRINT);
            eat(TOKEN_PRINT);
            node->right = parse_expression();
            eat(TOKEN_SEMICOLON);
            return node;
        }
        case TOKEN_LET: {
            node = create_node(NODE_ASSIGN);
            eat(TOKEN_LET);

            // Parse variable name
            if (current_token.type != TOKEN_IDENTIFIER) {
                parser_error();
            }
            node->left = create_node(NODE_VARIABLE);
            strcpy(node->left->value, current_token.value);
            eat(TOKEN_IDENTIFIER);

            if (current_token.type != TOKEN_COLON_EQUALS){
                fprintf(stderr, "must use := when initializing\n");
                exit(1);
            }
            eat(TOKEN_COLON_EQUALS);

            node->right = parse_expression();
            eat(TOKEN_SEMICOLON);
            return node;
        }
        case TOKEN_IDENTIFIER: {
            node = create_node(NODE_REASSIGN);
            node->left = create_node(NODE_VARIABLE);
            strcpy(node->left->value, current_token.value);
            eat(TOKEN_IDENTIFIER);

            if (current_token.type != TOKEN_EQUALS) {
                fprintf(stderr, "must use = for reassignment\n");
                exit(1);
            }
            eat(TOKEN_EQUALS);

            node->right = parse_expression();
            eat(TOKEN_SEMICOLON);
            return node;
        }
        case TOKEN_IF: {
            eat(TOKEN_IF);

            node = create_node(NODE_IF_STMT);

            ConditionalBranch* branch = malloc(sizeof(ConditionalBranch));
            branch->condition = parse_expression();
            branch->body = parse_block();

            ConditionalBranch* current = branch;

            node->cond_chain = branch;
            return node;
        }
        case TOKEN_LOOP: {
            eat(TOKEN_LOOP);

            node = create_node(NODE_LOOP);

            if (current_token.type != TOKEN_LBRACE){
                node->condition = parse_expression();
            }

            node->right = parse_block();
            return node;
        }
        case TOKEN_BREAK: {
            node = create_node(NODE_BREAK);
            eat(TOKEN_BREAK);
            eat(TOKEN_SEMICOLON);
            return node;
        }
        default:
            parser_error();
            return NULL;  // To satisfy compiler
    }
}

ASTNode* parse_block(){ //if else
    eat(TOKEN_LBRACE);

    ASTNode* block = create_node(NODE_BLOCK);
    ASTNode* first_stmt = NULL;
    ASTNode* current = NULL;

    while (current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_EOF){
        if (current_token.type == TOKEN_RETURN){
            eat(TOKEN_RETURN);
            ASTNode* ret = create_node(NODE_RETURN);
            ret->right = parse_expression();
            eat(TOKEN_SEMICOLON);

            if (first_stmt==NULL){
                first_stmt = ret;
            } else {
                current->next = ret;
            }

            block->right = first_stmt;
            eat(TOKEN_RBRACE);
            return block;
        }

        ASTNode* stmt = parse_statement();

        if (first_stmt==NULL) {
            first_stmt = stmt;
            current = stmt;
        } else {
            current->next = stmt;
            current = stmt;
        }
    }

    block->right = first_stmt;
    eat(TOKEN_RBRACE);
    return first_stmt;
}

// Symbol table operations
int get_variable(const char* name) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            return symbol_table[i].value;
        }
    }
    fprintf(stderr, "Undefined variable: %s\n", name);
    exit(1);
}

void set_variable(const char* name, int value) {
    // Update existing variable
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            symbol_table[i].value = value;
            return;
        }
    }

    // Add new variable
    if (symbol_count >= MAX_IDENTIFIERS) {
        fprintf(stderr, "Too many variables\n");
        exit(1);
    }

    strcpy(symbol_table[symbol_count].name, name);
    symbol_table[symbol_count].value = value;
    symbol_count++;
}

// Interpreter
int interpret_node(ASTNode* node) {
    if (node == NULL) return 0;

    switch (node->type) {
        case NODE_NUMBER:
            return atoi(node->value);

        case NODE_VARIABLE:
            return get_variable(node->value);

        case NODE_ASSIGN: {
            const char* name = node->left->value;
            for (int i=0; i<symbol_count; i++){
                if (strcmp(symbol_table[i].name, name)==0){
                    fprintf(stderr, "var %s is declared already\n", name);
                    exit(1);
                }
            }

            int value = interpret_node(node->right);
            set_variable(name, value);
            return value;
        }

        case NODE_LOGIC: {
            if (strcmp(node->value, "!")==0){
                int right_val = interpret_node(node->right);
                return right_val==0? 1:0;
            }

            int left = interpret_node(node->left);
            int right = interpret_node(node->right);

            if (strcmp(node->value, "&")==0){
                return (left != 0 && right != 0) ? 1:0;
            } else if (strcmp(node->value, "|")==0){
                return (left != 0 || right != 0) ? 1:0;
            }

            fprintf(stderr, "unknown logical operator: %s\n", node->value);
            exit(1);
        }

        case NODE_REASSIGN: {
            const char* name = node->left->value;
            int found = 0;
            for (int i=0; i<symbol_count;i++){
                if (strcmp(symbol_table[i].name, name)==0){
                    found = 1;
                    break;
                }
            }

            if (!found){
                fprintf(stderr, "cannot reassign undeclared variable\n");
                exit(1);
            }
            int value = interpret_node(node->right);
            set_variable(name, value);
            return value;
        }

        case NODE_PRINT: {
            int value = interpret_node(node->right);
            printf("%d\n", value);
            return value;
        }

        case NODE_BINOP: {
            int left_val = interpret_node(node->left);
            int right_val = interpret_node(node->right);

            if (strcmp(node->value, "+")==0){
                return left_val + right_val;
            } else if (strcmp(node->value, "-")==0){
                return left_val - right_val;
            }

            fprintf(stderr, "unknown operator: %s\n", node->value);
            exit(1);
        }


        case NODE_COMPARE: {
            int left_val = interpret_node(node->left);
            int right_val = interpret_node(node->right);

            if (strcmp(node->value, "==")==0){
                return left_val==right_val ? 1:0;
            } else if(strcmp(node->value, "<")==0){
                return left_val<right_val ? 1:0;
            } else if(strcmp(node->value, ">")==0){
                return left_val>right_val ? 1:0;
            } else if(strcmp(node->value, "!=")==0){
                return left_val!=right_val ? 1:0;
            }

            fprintf(stderr, "unknown comparison operator: %s\n", node->value);
            exit(1);
        }

        case NODE_IF_STMT: {
            ConditionalBranch* branch = node->cond_chain;

            if (interpret_node(branch->condition) != 0){
                ASTNode* stmt = branch->body;
                while (stmt != NULL){
                    interpret_node(stmt);
                    stmt = stmt->next;
                }
            }
            return 0;
        }

        case NODE_BLOCK: {
            ASTNode* stmt = node->right;
            int last_value = 0;

            while (stmt != NULL){
                last_value = interpret_node(stmt);
                if (stmt->type == NODE_RETURN) {
                    return last_value;
                }
                stmt = stmt->next;
            }

            return last_value;
        }

        case NODE_RETURN: {
            return interpret_node(node->right);
        }

        case NODE_LOOP: {
            while (1) {
                if (node->condition != NULL){
                    if (interpret_node(node->condition)==0){
                        break;
                    }
                }

                ASTNode* stmt = node->right;
                while (stmt != NULL){
                    int result = interpret_node(stmt);
                    //-999999 spec val
                    if (result==-999999) {
                        return 0;
                    }
                    stmt = stmt->next;
                }
            }
            return 0;
        }

        case NODE_BREAK: {
            return -999999;
        }
    }

    return 0;  // To satisfy compiler
}

// Main interpreter function
void interpret(const char* input) {
    clock_t start = clock();
    // Initialize interpreter
    source = (char*)input;
    pos = 0;
    line = 1;
    column = 1;
    symbol_count = 0;

    // Get first token
    current_token = get_next_token();

    // Parse and interpret until EOF
    while (current_token.type != TOKEN_EOF) {
        ASTNode* node = parse_statement();
        interpret_node(node);
        free_ast(node);
    }

    clock_t end = clock();
    double cpu_time_used = ((double)(end-start))/CLOCKS_PER_SEC;
    printf("execution time: %f seconds\n", cpu_time_used);
}

char* read_pavo_file(const char* filename){
    const char* extension = strrchr(filename, '.');
    if (extension==NULL || strcmp(extension, "pavo")==0){
        fprintf(stderr, "error: the file extension .pavo\n");
        return NULL;
    }

    FILE* file = fopen(filename, "r");
    if (file==NULL){
        fprintf(stderr, "error: could not open '%s'\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char *)malloc(file_size + 1);
    if (buffer==NULL){
        fprintf(stderr, "error: could not allocate memory for the file\n");
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';

    if (bytes_read<file_size) {
        fprintf(stderr, "warning: only read %zu of %ld bytes from file\n",
            bytes_read, file_size);
    }

    fclose(file);
    return buffer;
}

void interpret_file(const char* filename){
    char* source = read_pavo_file(filename);
    if (source==NULL){
        return;
    }

    interpret(source);

    free(source);
}

int main(int argc, char* argv[]) {
    if (argc != 2){
        fprintf(stderr, "usage: %s <filename.pavo>\n", argv[0]);
        fprintf(stderr, "example: %s program.pavo\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];

    interpret_file(filename);

    return 0;
}
