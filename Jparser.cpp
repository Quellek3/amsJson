#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>

struct Value{
    virtual ~Value() = default;
    virtual void print(){};
};

struct Null:Value{
};

struct Bool:Value{
    Bool(bool B):Val(B){};
    bool Val;
};

struct String:Value, std::string{
    using std::string::string;
    std::string str;
    virtual void print(){
        std::cout << str << "test";
    }
};

struct Double:Value{
    Double(double D):Val(D){}
    double Val;
};

struct Array:Value{
    std::vector<Value*> vals;
};

struct Object:Value{
    std::unordered_map<std::string, Value*> obj;
    void insert(std::string s, Value* v){
        obj.insert(std::make_pair(s, v));
    }
    void print(){
        Value* object = new Value; 
        Value temp;

        for (std::pair<std::string, Value*> element : obj){
            object = element.second;
            temp = *object;

            //element.second->print();
            
            std::cout << element.first << " :: " << element.second << std::endl;
        }
    }

};

Object* parseObject(std::ifstream&);
String* parseString(std::ifstream&);

int main(){
    std::ifstream input("./example.json");
    
    char c;
    std::string tempstring = "";
    Object* json = parseObject(input);


    //{"id":1,"name":"Agreendoor","price":12.50,"tags":["home","green"]}

    json->print();
    

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

    i.get(c); //skip over curly bracket
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
                if(c == '"'){
                    json->insert(lastkey, parseString(i));
                }

                lastkey = "";
                tempstring = "";
                state = READY;
            }
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
    
    //std::cout << tempstring << " " << output << std::endl;
    output->print();

    return output;

}

void parseNumber(std::ifstream& i){
    char c;
    std::string tempstring = "";

    while(i.get(c)){
        if(!isspace(c)){
            std::cout << c;
        } 
    }
}
void parseArray(std::ifstream& i){
    char c;
    std::string tempstring = "";

    while(i.get(c)){
        if(!isspace(c)){
            std::cout << c;
        } 
    }
}





















