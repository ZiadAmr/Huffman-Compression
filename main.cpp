#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <queue>
#include <bitset>
#include <filesystem>

#include "node.h"
#include "FileReader.h"

struct Arguments
{
    bool encode = false;
    bool decode = false;
    std::string input_file;
    std::string output_file;
};

void encode(std::string filename, std::string output_name){
    // read file and generate probabilites
    std::map<char, int> occurances;
    int total = 0;

    if(std::ifstream file{filename, std::ios::binary}){
        char byte; 
        while(file.read(&byte, sizeof(byte))){
            total += 1;
            if(occurances[byte]){
                occurances[byte] += 1;
            } else {
                occurances[byte] = 1;
            }
        }

        file.close();
    } else {
        std::cerr << "couldn't open file" << std::endl;
    }

    // generate huffman code
    std::priority_queue<Node<char>*, std::vector<Node<char>*>, Node<char> > queue;
    for (std::map<char, int>::iterator it = occurances.begin(); it != occurances.end(); it++){
        Node<char>* node = new Node<char>(it->first, (double)it->second / total, nullptr, nullptr);
        queue.push(node);
    }

    while(queue.size() > 1){
        Node<char>* lastNode1 = queue.top();
        queue.pop();
        Node<char>* lastNode2 = queue.top();
        queue.pop();
        
        Node<char>* newNode = new Node('\0', lastNode1->probability + lastNode2->probability, lastNode1, lastNode2);
        queue.push(newNode);
    }
   
    Node<char>* head = queue.top();

    // traverse tree
    std::map<char, std::string> map;
    inorderTraversal(head, "", map);

    std::ofstream fout{output_name, std::ios::binary | std::ios::out};

    // write the probabilities to the file
    //first element is an integer storing size of probabilities map
    int mapSize = map.size();
    fout.write(reinterpret_cast<const char *>(&mapSize), sizeof(int));

    for(const auto &[k, v] : occurances) {
        double prob = v / (double)total;
        fout.write(reinterpret_cast<const char*>(&k), sizeof(char));
        fout.write(reinterpret_cast<const char*>(&prob), sizeof(double));
    }

    if(std::ifstream file{filename, std::ios::binary}){
        char byte; 
        char buffer = 0;
        char count = 0;
        while(file.read(&byte, sizeof(byte))){
            std::string encoding = map[byte];

            for(char bit: encoding){
                buffer <<= 1;
                if (bit == '1') buffer |= 1; 
                count++;

                if (count == 8) {
                    fout.write(&buffer, sizeof(buffer));
                    buffer = 0;
                    count = 0;
                }
            }
        }

        if(count > 0){
            while(count < 8){
                buffer <<= 1;
                buffer |= 1;
                count++;
            }
        }
        fout.write(&buffer, sizeof(buffer));

        file.close();
    } else {
        std::cerr << "couldn't open file" << std::endl;
    }
    fout.close();

    double ifileSize = std::filesystem::file_size(filename);
    double ofileSize = std::filesystem::file_size(output_name);

    std::cout << filename << " " << ifileSize << " To " << ofileSize << " : " << ofileSize / ifileSize << std::endl;
}

void decode(std::string filename, std::string output_name){
    if(std::ifstream file{filename, std::ios::binary | std::ios::in}){
        int mapSize;
        file.read(reinterpret_cast<char *>(&mapSize), sizeof(int));
        std::priority_queue<Node<char>*, std::vector<Node<char>*>, Node<char> > queue;
        // read the probabilities for each character and reconstruct the tree
        for(int i = 0; i < mapSize; i++){
            char key; 
            double value; 
            file.read(&key, sizeof(char));
            file.read(reinterpret_cast<char *>(&value), sizeof(double));
            Node<char>* node = new Node<char>(key, value, nullptr, nullptr);
            queue.push(node);
        }

        while(queue.size() > 1){
            Node<char>* lastNode1 = queue.top();
            queue.pop();
            Node<char>* lastNode2 = queue.top();
            queue.pop();

            Node<char>* newNode = new Node('\0', lastNode1->probability + lastNode2->probability, lastNode1, lastNode2);
            queue.push(newNode);
        }
   
        Node<char>* head = queue.top();

        FileReader fr(filename);
        bool bit;
        int count = 0; 
        int prefixSize = mapSize * (sizeof(char)*8 + sizeof(double)*8) + sizeof(int)*8;
        for(int i = 0; i < prefixSize; i++){
            fr.readBit(bit);
        }

        // traverse tree 
        std::ofstream fout(output_name, std::ios::out);
        
        Node<char>* current = head;

        while(fr.readBit(bit)){
            if(current->left == nullptr || current->right == nullptr){
                fout.write(&current->value, sizeof(char));
                current = head;
            }

            if(bit == 0){
                current = current->left;
            } else {
                current = current->right;
            }
        }

    }
}

void print_usage(const char* program)
{
    std::cout << "Usage:\n"
              << program << " -e|-d -i <input_file> -o <output_file>\n\n"
              << "Options:\n"
              << "  -e              Encode mode\n"
              << "  -d              Decode mode\n"
              << "  -i <file>      Input file\n"
              << "  -o <file>      Output file\n";
}

bool parse_arguments(int argc, char* argv[], Arguments& args)
{
    if (argc < 6)
        return false;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "-e")
        {
            args.encode = true;
        }
        else if (arg == "-d")
        {
            args.decode = true;
        }
        else if (arg == "-i" && i + 1 < argc)
        {
            args.input_file = argv[++i];
        }
        else if (arg == "-o" && i + 1 < argc)
        {
            args.output_file = argv[++i];
        }
        else
        {
            return false;
        }
    }

    // Validation
    if (args.encode == args.decode) // both true or both false
        return false;

    if (args.input_file.empty() || args.output_file.empty())
        return false;

    return true;
}

int main(int argc, char* argv[])
{
    Arguments args;

    if (!parse_arguments(argc, argv, args))
    {
        print_usage(argv[0]);
        return 1;
    }

    if(args.decode) decode(args.input_file, args.output_file);
    else if(args.encode) encode(args.input_file, args.output_file);

    return 0;
}