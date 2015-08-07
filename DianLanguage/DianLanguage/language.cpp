#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

#define TRUE 1;
#define FALSE 0;

/**
 * Primitive language commands
 */
typedef enum { 
    ADDITION, // +
    SUBTRACTION, // -
	MULTIPLY, // *
	DIVIDE, // /
	MODULUS, // %
	PRINT, // print
	IF, // if
	IFCOMPARISON, //if with condition
	ENDIF, // ]
	ELSE, // else
	WHILE, // while
	VAR, // var (variable declaratioin)
	EQUALS, // =
	GREATER_THAN, // >
	LESS_THAN // <
    //SHIFT_LEFT, // <
    //SHIFT_RIGHT, // >
    //INPUT, // ,
    //OUTPUT // .
} Command;

// Forward references. Silly C++!
class MathNode;
//class Loop;
class Program;
class ControlNode;
class Declarations;
class Displaying;

/**
 * Visits?!? Well, that'd indicate visitors!
 * A visitor is an interface that allows you to walk through a tree and do stuff.
 */
class Visitor {
    public:
 //       virtual void visit(const MathNode * leaf) = 0;
 //       virtual void visit(const Loop * loop) = 0;
        virtual void visit(const Program * program) = 0;
		virtual void visit(const ControlNode * program) = 0;
		virtual void visit(const Declarations * program) = 0;
		virtual void visit(const Displaying * program) = 0;
};

/**
 * The Node class (like a Java abstract class) accepts visitors, but since it's pure virtual, we can't use it directly.
 */
class Node {
    public:
        virtual void accept (Visitor *v) = 0;
};

/**
 * MathNode publicly extends Node to accept visitors.
 * MathNode represents a leaf node with a primitive Brainfuck command in it.
 */
//class MathNode : public Node {
//    public:
//        Command command;
//		MathNode(char c) {
//            switch(c) {
//                case '+': command = ADDITION; break;
//                case '-': command = SUBTRACTION; break;
//				case '*': command = MULTIPLY; break;
//				case '/': command = DIVIDE; break;
//				case '%': command = MODULUS; break;
//                //case '<': command = SHIFT_LEFT; break;
//                //case '>': command = SHIFT_RIGHT; break;
//                //case ',': command = INPUT; break;
//                //case '.': command = OUTPUT; break;
//            }
//        }
//
//        void accept (Visitor * v) {
//            v->visit(this);
//        }
//};

class Declarations : public Node {
    public:
        Command command;
		string n;
		string v;
		Declarations(string name, string value) {
            command = VAR;
			n = name;
			v = value;
        }

        void accept (Visitor * v) {
            v->visit(this);
        }
};

class Displaying : public Node {
    public:
        Command command;
		string v;
		Displaying(string value) {
            command = PRINT;
			v = value;
        }

        void accept (Visitor * v) {
            v->visit(this);
        }
};

class ControlNode : public Node {
	public:
		Command command;
		string l;
		string c;
		string r;
		string boolee;
		ControlNode(string left, string cmprsn, string right) {
			command = IFCOMPARISON;
			l = left;
			c = cmprsn;
			r = right;
		}

		ControlNode(string boolean){
			boolee = boolean;
			if(boolean == "true" || boolean == "false"){
				command = IF;
			} else if(boolean == "]"){
				command = ENDIF;
			} else if(boolean == "else"){
				command = ELSE;
			}
		}
		void accept (Visitor * v) {
            v->visit(this);
        }
};

class Container: public Node {
    public:
        vector<Node*> children;
        virtual void accept (Visitor * v) = 0;
};

/**
 * Loop publicly extends Node to accept visitors.
 * Loop represents a loop in Brainfuck.
 */
//class Loop : public Container {
//    public:
//        void accept (Visitor * v) {
//            v->visit(this);
//        }
//};

/**
 * Program is the root of a Brainfuck program abstract syntax tree.
 * Because Brainfuck is so primitive, the parse tree is the abstract syntax tree.
 */
class Program : public Container {
    public:
        void accept (Visitor * v) {
            v->visit(this);
        }
};

/**
 * Read in the file by recursive descent.
 * Modify as necessary and add whatever functions you need to get things done.
 */
void parse(fstream & file, Container * container) {
    char c;
	vector<char> command;//will hold the command before the deliminating space
	bool error = false;
//	Loop * l;
	while(file >> c) {
		if(c != ' ' || c != '\n'){//space, new line will deliminate the commands
			command.push_back(c);
		} else {//once it sees a space or a new line character, it computes the command
			string str(command.begin(), command.end());//convert char vector to string
			command.clear();//reset the vector for new commands
			if(str == "" || str == " "){//if the command captured nothing or a blank string
				continue;
			} else if(str == "var" ){/////////////////VARIABLE DECLARATIONS
				string name;
				string value;
				int i;
				for(i = 0; i < 2;i++){//get the next two arguments
					while(file >> c){
						if(c != ' ' || c != '\n'){
							command.push_back(c);
						} else {
							string str1(command.begin(), command.end());//convert char vector to string
							command.clear();//reset the vector for new commands	
							switch(i){
								case 0: name = str1;
								case 1: value = str1;
							}
							break;
						}
					}
				}

				container->children.push_back(new Declarations(name, value));
			} else if(str == "if"){ //IF ELSE
				string left;
				string cmprsn;
				string right;
				int i;
				for(i = 0; i < 3;i++){//get the next two arguments
					while(file >> c){
						if(c != ' ' || c != '\n'){
							command.push_back(c);
						} else if(c == '\n' && i == 0) {
							string str1(command.begin(), command.end());//convert char vector to string
							command.clear();//reset the vector for new commands	
							i = 3;//break out of for loop on next iteration
							container->children.push_back(new ControlNode(str1));
							break;
						} else {
							string str1(command.begin(), command.end());//convert char vector to string
							command.clear();//reset the vector for new commands	
							switch(i){
								case 0: left = str1;
								case 1: cmprsn = str1;
								case 2: right = str1;
							}
							break;
						}
					}
				}

				container->children.push_back(new ControlNode(left,cmprsn,right));
			} else if(str == "]" || str == "else"){//ENDIF and ELSE command

				container->children.push_back(new ControlNode(str));

			////} else if(str == "+" || str == "-" || str == "*" || str == "/" || str == "%"){//math operators 
			////	container->children.push_back(new MathNode((char) str[0]));
			//} else if(c == '['){//if beginnning of a loop then create a loop object and loop through 
			//	l = new Loop();
			//	parse(file, l);//use the parse function to create a sub program to do the loop
			//	container->children.push_back(l);
			//} else if (c == ']') {
			//	return;
			//}
		} else if (str == "print"){
			string output;
			while(file >> c){
				if(c != '\n'){
					command.push_back(c);
				} else {
					string str1(command.begin(), command.end());//convert char vector to string
					output = str1;
					command.clear();
					break;
				}
			}
			container->children.push_back(new Displaying(str));
		}
			
	}
	}

}

/**
 * A printer for Brainfuck abstract syntax trees.
 * As a visitor, it will just print out the commands as is.
 * For Loops and the root Program node, it walks trough all the children.
 */
//class Printer : public Visitor {
//    public:
//        void visit(const MathNode * leaf) {
//            switch (leaf->command) {
//                case ADDITION:   cout << '+'; break;
//                case SUBTRACTION:   cout << '-'; break;
//                //case SHIFT_LEFT:  cout << '<'; break;
//                //case SHIFT_RIGHT: cout << '>'; break;
//                //case INPUT:       cout << ','; break;
//                //case OUTPUT:      cout << '.'; break;
//            }
//        }
//        void visit(const Loop * loop) {
//            cout << '[';
//            for (vector<Node*>::const_iterator it = loop->children.begin(); it != loop->children.end(); ++it) {
//                (*it)->accept(this);
//            }
//            cout << ']';
//        }
//        void visit(const Program * program) {
//            for (vector<Node*>::const_iterator it = program->children.begin(); it != program->children.end(); ++it) {
//                (*it)->accept(this);
//            }
//            cout << '\n';
//        }
//};

class Interpreter : public Visitor {
	bool exec; // this variable allows execution of commands based on controls.
    char memory[300];
	string var_names[300];
    int pointer;
    public:
     //   void visit(const MathNode * leaf) {
     //       switch (leaf->command) {
     //           case ADDITION:
					//memory[pointer]++;
     //               break;
     //           case SUBTRACTION:
					//memory[pointer]--;
     //               break;
     ////           case SHIFT_LEFT:
					////pointer--;
     ////               break;
     ////           case SHIFT_RIGHT:
					////pointer++;
     ////               break;
     ////           case INPUT:
					////cin >> memory[pointer];
     ////               break;
     ////           case OUTPUT:
					////cout << memory[pointer];
     ////               break;
     //       }
     //   }

		void visit(const ControlNode * leaf) {
			int i, l, r;
			switch(leaf->command){
				
				case IF:{
					if(leaf->boolee == "true"){
						exec = true;
					}else{
						exec = false;
					}
				}
				case IFCOMPARISON:{
					
					bool l_done=false, r_done=false; //bools to tell if to take the given values or numbers
					for(i=0;i < pointer; i++){
						if(var_names[i] == leaf->l){
							l = memory[i];
							l_done = true;
						}
						if(var_names[i] == leaf->r){
							r = memory[i];
							r_done = true;
						}
					}
					if(!l_done){
						l = stoi(leaf->l);
					}
					if(!r_done){
						r = stoi(leaf->r);
					}
					if(leaf->c == "=="){//OPERATION COMPARISON
						exec = l == r;
					} else if(leaf->c == "!="){
						exec = l != r;
					} else if(leaf->c == ">"){
						exec = l > r;
					} else if(leaf->c == "<"){
						exec = l < r;
					} else if(leaf->c == ">="){
						exec = l >= r;
					} else if(leaf->c == "<="){
						exec = l <= r;
					}
				}
				case ENDIF: {
					if(!exec){//if it skipped the loop, return to it's original
						exec = !exec;
				}
				case ELSE: {
					if(leaf->c == "=="){//OPPOSITE OPERATION COMPARISON
						exec = l != r;
					} else if(leaf->c == "!="){
						exec = l == r;
					} else if(leaf->c == ">"){
						exec = l <= r;
					} else if(leaf->c == "<"){
						exec = l >= r;
					} else if(leaf->c == ">="){
						exec = l < r;
					} else if(leaf->c == "<="){
						exec = l > r;
					}
				}
			}
		}
		}



		void visit(const Displaying * leaf) {
			if(exec){
				int i;
				for(i=0;i < pointer; i++){
					if(var_names[i] == leaf->v){
						cout << to_string(memory[i]) << '\n';
						break;
					}
				}
				if(pointer == i){
					cout << leaf->v << "\n";//print the value if the variable did not exist.
				}
			}
		}

		void visit(const Declarations * leaf) {
			if(exec){
				var_names[pointer] = leaf->n;
				memory[pointer] = stoi(leaf->v); //transform the value of string to int
				pointer++;//next declaration will be saved in the next index
			}
		}

   //     void visit(const Loop * loop) {
   //         while(memory[pointer]){
			//	for (vector<Node*>::const_iterator it = loop->children.begin(); it != loop->children.end(); ++it) {
			//		(*it)->accept(this);
			//	}
			//}
   //     }

        void visit(const Program * program) {
            // zero init the memory array
            // set pointer to zero
			exec = true;
			pointer=0;
			for(int i=0; i < 300; i++){
				memory[i]=0;
				var_names[i]="";
			}
            for (vector<Node*>::const_iterator it = program->children.begin(); it != program->children.end(); ++it) {
                (*it)->accept(this);
            }
        }
};

int main(int argc, char *argv[]) {
    fstream file;
    Program program;
//    Printer printer;
    Interpreter interpreter;
    if (argc == 1) {
        cout << argv[0] << ": No input files." << endl;
    } else if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            file.open(argv[i], fstream::in);
            parse(file, & program);
            //program.accept(&printer);
            program.accept(&interpreter);
            file.close();
        }
    }
}

