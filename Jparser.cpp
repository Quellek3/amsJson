#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>

void mkTab(int i){//used for tabbing in prettypritn
    for(int j = 0; j < i; ++j)
        std::cout << "\t";
}

struct Value{
    virtual ~Value() = default;
    virtual void print(int&){};
    virtual int weight(){};
};

struct Null:Value{
    virtual void print(int &depth){
        mkTab(++depth);
        std::cout << "NULL" << std::endl;
        --depth;
    }
    virtual int weight(){
        return 1;
    }
};

struct Bool:Value{
    Bool(bool B):Val(B){};
    bool Val;
    virtual void print(int &depth){
        mkTab(++depth);
        if(Val = 1)
            std::cout << "true";
        else
            std::cout << "false";
        std::cout << std::endl;
        --depth;
    }
    virtual int weight(){
        return 1;
    }
};

struct String:Value, std::string{
    using std::string::string;
    virtual void print(int &depth){
        mkTab(++depth);
        std::cout << "\"" << *this << "\"" << std::endl;
        --depth;
    }
    virtual int weight(){
        return 1;
    }
};

struct Double:Value{
    Double(double D):Val(D){}
    double Val;
    virtual void print(int &depth){
        mkTab(++depth);
        std::cout << Val << std::endl;
        --depth;
    }
    virtual int weight(){
        return 1;
    }
};

struct Array:Value{
    std::vector<Value*> vals;
    void push(Value* v){
        vals.push_back(v);
    }
    virtual void print(int &depth){
        mkTab(++depth);
        std::cout << "[ " << std::endl;
        for(int i=0; i<vals.size(); ++i){
            vals[i]->print(depth);
            std::cout << " ";
        }
        mkTab(depth--);
        std::cout << "]" << std::endl;
    }
    virtual int weight(){
        int weightsum = 0;
        for(int i=0; i<vals.size(); ++i){
            weightsum += vals[i]->weight();
        }
        return 1+weightsum;//+1 so it includes itself
    }
};

struct Object:Value{
    std::unordered_map<std::string, Value*> obj;
    void insert(std::string s, Value* v){
        obj.insert(std::make_pair(s, v));
    }
    virtual void print(int &depth){
        mkTab(++depth);
        std::cout << "{" << std::endl;
        ++depth;
        for (std::pair<std::string, Value*> element : obj){
            mkTab(depth);
            std::cout << element.first << " :: " << std::endl;
            element.second->print(depth);
        }
        --depth;
        mkTab(depth);
        std::cout << "}" << std::endl;
        --depth;
    }
    virtual int weight(){
        int weightsum = 0;
        for (std::pair<std::string, Value*> element : obj){
            weightsum += element.second->weight();
        }
        return 1+weightsum;//+1 so it includes itself
    }
};

Object* parseObject(std::ifstream&);
String* parseString(std::ifstream&);
Double* parseDouble(std::ifstream&, char); //takes a char as well because it needs the first digit
Bool* parseBool(std::ifstream&, char); //takes a char for same reason as above
Null* parseNull();
Array* parseArray(std::ifstream&);
int getWeight(Object*);
void prettyPrint(Object*);

int main(int argc, char *argv[]){
    std::string input;
    if ( argc != 2 ){ // argc should be 2 for correct execution
        std::cout<<"usage: "<< argv[0] <<" <filename>\n";\
        return 1;
    }else{
        input = argv[1];
    }

    std::ifstream infile(input);

    Object* json = parseObject(infile);

    prettyPrint(json);
    
    std::cout << "weight: " << json->weight() << std::endl;

}

Object* parseObject(std::ifstream& i){
    Object* json = new Object;

    char c;
    std::string tempstring = "";
    std::string lastkey = "";

    int READY = 0;
    int KEY = 1;
    int VALUE = 2;

    int state = 0;
    while(i.get(c)){
        if(!isspace(c)){
            if(state == READY){
                if(c == '"')
                    state = KEY;
            }else if(state == KEY){
                if(c == '\\'){ //skip to next character if escape is found
                    i.get(c);
                    lastkey += c;
                }else if(c != '"'){
                    lastkey += c;
                }else{
                    i.get(c); //skip over colon
                    state = VALUE;
                } 
            }else if(state == VALUE){
                if(c == '"'){ //check string
                    json->insert(lastkey, parseString(i));
                }else if(isdigit(c)){ //check double
                    json->insert(lastkey, parseDouble(i, c));
                }else if(c == 't' || c == 'f'){ //check bool
                    json->insert(lastkey, parseBool(i, c));
                }else if(c == 'n'){ //check null
                    json->insert(lastkey, parseNull());
                }else if(c == '{'){ //check object
                    json->insert(lastkey, parseObject(i));
                }else if(c == '['){ //check array
                    json->insert(lastkey, parseArray(i));
                }

                //i.seekg(-1, i.cur);
                //c == i.peek();
                /*std::cout << c << std::endl;
                if(c == '}'){
                    std::cout << "curly";
                    i.get(c);
                    return json;
                }else if(c == ',')
                    std::cout << "comma";*/

                lastkey = "";
                tempstring = "";
                state = READY;
            }
            c == i.peek();
            if(c == '}' || c == ',')
                return json;
        } 
    }
    return json;
}

String* parseString(std::ifstream& i){
    char c;
    String tempstring = "";
    while(i.get(c)){
            if(c == '\\'){ //skip to next character if escape is found
                i.get(c);
                tempstring += c;
            }else if(c != '"'){
                tempstring += c;
            }else{
                break;
            }
    }
    String* output = new String;
    *output = tempstring;
    i.get(c); //advance past next comma

    return output;
}

Double* parseDouble(std::ifstream& i, char first){
    char c;
    std::string tempstring = "";
    tempstring += first; //adds on the first character that it found because the next i.get(c) will skip it

    while(i.get(c)){
        if(!isspace(c)){
            if(isdigit(c) || c == '.'){
                tempstring += c;
            }else{
                break;
            }
        } 
    }
    Double* output = new Double(std::stod(tempstring));
    return output;
}

Bool* parseBool(std::ifstream&i, char c){
    char first = c;

    while(i.get(c)){
        if(!isspace(c)){
            if(c == ',' || c == '}' || c == ']')//skips over rest of bool
                break;
        }
    }

    if(first = 't'){
        Bool* output = new Bool(true);
        return output;
    }else{
        Bool* output = new Bool(false);
        return output;
    }
}

Null* parseNull(){
    Null* output = new Null;
    return output;
}

Array* parseArray(std::ifstream& i){
    char c;
    Array* vals = new Array;
    while(i.get(c)){
        if(!isspace(c)){                       
            if(c == '"'){ //check string
                vals->push(parseString(i));
            }else if(isdigit(c)){ //check double
                vals->push(parseDouble(i,c));
            }else if(c == 't' || c == 'f'){ //check double
                vals->push(parseBool(i,c));
            }else if(c == 'n'){ //check null
                vals->push(parseNull());
            }else if(c == '{'){
                vals->push(parseObject(i));
            }else if(c == '['){
                vals->push(parseArray(i));
            }
            i.seekg(-1, i.cur);//moves the file reader back one character to check
            i.get(c);
            if(c == ']')//check if current character is ] to close array
                break;   //if its not ] move forward again to resume business as usual
                         //this is still gross
        }
    }
    i.get(c);
    return vals;
}

int getWeight(Object* obj){
    return obj->weight()-1; //-1 so it doesnt count the entire object
}

void prettyPrint(Object* obj){
    int depth = -1; //starts at -1 so first brackets are inline with start
    obj->print(depth);

}



















